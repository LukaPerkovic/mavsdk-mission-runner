#include <chrono>
#include <cstdint>
#include <future>
#include <iostream>
#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/action/action.hpp>
#include <mavsdk/plugins/mission/mission.hpp>
#include <mavsdk/plugins/telemetry/telemetry.hpp>
#include <memory>
#include <thread>

#include "data_models/coordinates.hpp"
#include "helpers/calculate_destination_coordinates.hpp"
#include "helpers/mission_config.hpp"

using namespace mavsdk;
using std::chrono::seconds;
using std::this_thread::sleep_for;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <connection_url>\n";
        return 1;
    }

    Mavsdk mavsdk{Mavsdk::Configuration{ComponentType::GroundStation}};
    ConnectionResult connection_result = mavsdk.add_any_connection(argv[1]);

    if (connection_result != ConnectionResult::Success)
    {
        std::cerr << "Connection failed!" << connection_result << '\n';
        return 1;
    }

    auto system = mavsdk.first_autopilot(3.0);
    if (!system)
    {
        std::cerr << "Timed out waiting for system\n";
        return 1;
    }

    auto telemetry = Telemetry{system.value()};
    auto action = Action{system.value()};

    const auto set_rate_result = telemetry.set_rate_position(5.0);
    if (set_rate_result != Telemetry::Result::Success)
    {
        std::cerr << "Setting rate failed: " << set_rate_result << '\n';
        return 1;
    }

    telemetry.subscribe_position(
        [](Telemetry::Position position) {
            std::cout << "Altitude: " << position.relative_altitude_m << " m\n";
        });

    while (!telemetry.health_all_ok())
    {
        auto h = telemetry.health();
        std::cout << "Waiting for readiness | "
                  << "global_pos=" << h.is_global_position_ok
                  << " home=" << h.is_home_position_ok
                  << " armable=" << h.is_armable << "\n";
        sleep_for(seconds(1));
    }

    const auto pos = telemetry.position();
    std::cout << "Home-ish position: " << pos.latitude_deg
              << ", " << pos.longitude_deg << "\n";

    if (pos.latitude_deg == 0.0 && pos.longitude_deg == 0.0)
    {
        std::cerr << "Position not valid yet (0,0). Aborting.\n";
        return 1;
    }


    MissionConfig cfg = load_mission_config("config/mission_config_fly_a_square.yaml");

    if (argc > 2)
    {
        std::string nav_mode_arg = std::string(argv[2]);
      
        if (nav_mode_arg != "relative" && nav_mode_arg != "absolute")
        {
            std::cerr << "Unrecognized position argument on index [2]" << nav_mode_arg << '\n';
            return 1;
        }
        
        cfg.navigation_mode = nav_mode_arg;
    }

    std::vector<Coords> waypoints;

    if (cfg.navigation_mode == "relative")
    {

        Coords startPosition{ .latitude = pos.latitude_deg,
                        .longitude = pos.longitude_deg };
 	Coords current = startPosition;
        for (const auto& wp: cfg.relative_waypoints)
        {
            if (wp.distance_m == 0.0)
            {
                // Current; Waypoint A
                waypoints.push_back(current);
            }
            else
            {
                current = getDestinationCoords(current, wp.azimuth_deg, wp.distance_m);
                waypoints.push_back(current);
            }
        }
	// Return to original position, complete the square
	waypoints.push_back(startPosition);
    }
    else if (cfg.navigation_mode == "absolute")
    {

        waypoints = cfg.absolute_waypoints;

    }
    else
    {
        std::cerr << "Unknown navigation_mode: " << cfg.navigation_mode << '\n';
        return 1;
    }

    auto mission = Mission{system.value()};

    auto make_mission_item = [&cfg](const Coords& coords)
    {
        Mission::MissionItem item{};
        item.latitude_deg        = coords.latitude;
        item.longitude_deg       = coords.longitude;
        item.relative_altitude_m = cfg.takeoff_altitude_m;
        item.speed_m_s           = cfg.speed_m_s;
        return item;
    };

    std::vector<Mission::MissionItem> mission_items;
    for (const auto& wp: waypoints)
        mission_items.push_back(make_mission_item(wp));

    Mission::MissionPlan mission_plan{};
    mission_plan.mission_items = mission_items;

    std::cout << "Uploading mission...\n";
    if (mission.upload_mission(mission_plan) != Mission::Result::Success)
    {
        std::cerr << "Mission upload failed.\n";
        return 1;
    }

    mission.subscribe_mission_progress([](Mission::MissionProgress p)
    {
        std::cout << "Progress: " << p.current << "/" << p.total << "\n";
    });

    // Arm (NO manual takeoff)
    std::cout << "Arming...\n";
    if (action.arm() != Action::Result::Success)
    {
        std::cerr << "Arming failed.\n";
        return 1;
    }

    // Start mission — PX4 handles takeoff via the mission
    std::cout << "Starting mission...\n";
    const Mission::Result start_result = mission.start_mission();
    if (start_result != Mission::Result::Success)
    {
        std::cerr << "Mission start failed (" << start_result << ").\n";
        return 1;
    }
    std::cout << "Mission started.\n";

    while (!mission.is_mission_finished().second)
        sleep_for(seconds(1));

    std::cout << "Mission complete!\n";


    std::cout << "Landing...\n";
    const Action::Result land_result = action.land();
    if (land_result != Action::Result::Success)
    {
        std::cerr << "Landing failed: " << land_result << '\n';
        return 1;
    }

    while (telemetry.in_air())
    {
        std::cout << "Vehicle is landing...\n";
        sleep_for(seconds(1));
    }

    std::cout << "Landed!\n";

    sleep_for(seconds(3));
    std::cout << "Finished...\n";

    return 0;
}
