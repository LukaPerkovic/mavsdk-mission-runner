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
        std::cerr << "Settin rate failed: " << set_rate_result << '\n';
        return 1;
    }

    telemetry.subscribe_position(
        [](Telemetry::Position position) {
            std::cout << "Altitude: " << position.relative_altitude_m << " m\n";
        });

    while (telemetry.health_all_ok() != true)
    {
        std::cout << "Vehicle is getting ready to arm\n";
        sleep_for(seconds(1));
    }

    std::cout << "Arming...\n";
    const Action::Result arm_result = action.arm();

    if (arm_result != Action::Result::Success)
    {
        std::cerr << "Arming failed: " << arm_result << '\n';
        return 1;
    }

    action.set_takeoff_altitude(5.0f);

    std::cout << "Taking off...\n";
    const Action::Result takeoff_result = action.takeoff();
    if (takeoff_result != Action::Result::Success)
    {
        std::cerr << "Takeoff failed: " << takeoff_result << '\n';
        return 1;
    }


    // ---- BEGIN SQUARE FLYING MISSION ----
    
    std::cout << "Creating mission...\n";

    // Create destination points

    Coords positionA {
	    .latitude=telemetry.position().latitude_deg,
            .longitude=telemetry.position().longitude_deg
    };

    Coords positionB = getDestinationCoords(positionA, 10, 90);
    Coords positionC = getDestinationCoords(positionB, 10, 180);
    Coords positionD = getDestinationCoords(positionC, 10, 270);

    // Create mission items

    auto mission = Mission{system.value()};
    std::vector<std::shared_ptr<Mission::MissionItem>> mission_items;

    std::shared_ptr<Mission::MissionItem> pointB(new Mission::MissionItem());
    pointB->latitude_deg = positionB.latitude;
    pointB->longitude_deg = positionB.longitude;
    mission_items.push_back(pointB);

    std::shared_ptr<Mission::MissionItem> pointC(new Mission::MissionItem());
    pointC->latitude_deg = positionC.latitude;
    pointC->longitude_deg = positionC.longitude;
    mission_items.push_back(pointC);

    std::shared_ptr<Mission::MissionItem> pointD(new Mission::MissionItem());
    pointD->latitude_deg = positionD.latitude;
    pointD->longitude_deg = positionD.longitude;
    mission_items.push_back(pointD);

    std::shared_ptr<Mission::MissionItem> pointA(new Mission::MissionItem());
    pointA->latitude_deg = positionA.latitude;
    pointA->longitude_deg = positionA.longitude;
    mission_items.push_back(pointA);

    // Upload mission

    std::cout << "Uploading mission...\n";
    Mission::MissionPlan mission_plan{};
    mission_plan.mission_items = mission_items;
    const Mission::Result result = mission.upload_mission(mission_plan);

    if (result != Mission::Result::Success)
    {
	    std::cout << "Mission upload failed (" << result << "), exiting.\n";
	    return 1;
    }
    std::cout << "Mission uploaded.\n";

    std::cout << "Starting mission...\n";

    const Mission::Result start_result = mission.start_mission();

    if (start_result != Mission::Result::Success)
    {
	    std::cout << "Mission start failed (" << start_result << "), exiting.\n";
            return 1;
    }
    std::cout << "Mission started.\n";


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
