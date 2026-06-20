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

    // Now position is reliable
    const auto pos = telemetry.position();
    std::cout << "Home-ish position: " << pos.latitude_deg
              << ", " << pos.longitude_deg << "\n";

    if (pos.latitude_deg == 0.0 && pos.longitude_deg == 0.0)
    {
        std::cerr << "Position not valid yet (0,0). Aborting.\n";
        return 1;
    }

    Coords positionA{ .latitude = pos.latitude_deg,
                      .longitude = pos.longitude_deg };
    Coords positionB = getDestinationCoords(positionA, 90, 10);
    Coords positionC = getDestinationCoords(positionB, 180, 10);
    Coords positionD = getDestinationCoords(positionC, 270, 10);

    auto mission = Mission{system.value()};

    auto make_mission_item = [](const Coords& coords)
    {
        Mission::MissionItem item{};
        item.latitude_deg        = coords.latitude;
        item.longitude_deg       = coords.longitude;
        item.relative_altitude_m = 5.0f;
        item.speed_m_s           = 2.0f;
        return item;
    };

    std::vector<Mission::MissionItem> mission_items;
    mission_items.push_back(make_mission_item(positionB));
    mission_items.push_back(make_mission_item(positionC));
    mission_items.push_back(make_mission_item(positionD));
    mission_items.push_back(make_mission_item(positionA));

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
