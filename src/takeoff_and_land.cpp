#include <chrono>
#include <cstdint>
#include <future>
#include <iostream>
#include <mavsdk/mavsdk.hpp>
#include <mavsdk/plugins/action/action.hpp>
#include <mavsdk/plugins/telemetry/telemetry.hpp>
#include <memory>
#include <thread>

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

    while (telemetry.position().relative_altitude_m < 4.5f)
    {
        sleep_for(seconds(1));
    }

    sleep_for(seconds(10));

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
