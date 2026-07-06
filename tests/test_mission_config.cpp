#include <catch2/catch_test_macros.hpp>
#include "helpers/mission_config.hpp"

TEST_CASE("Load mission config fields correctly")
{
    const std::string path = std::string(FIXTURES_DIR) + "/mission_test.yaml";
    
    MissionConfig cfg = load_mission_config(path);

    REQUIRE(cfg.navigation_mode == "relative");
    REQUIRE(cfg.takeoff_altitude_m == 5.0);
    REQUIRE(cfg.speed_m_s == 3.0);
    REQUIRE(cfg.relative_waypoints.size() == 2);
    REQUIRE(cfg.relative_waypoints[1].name == "B");
    REQUIRE(cfg.relative_waypoints[1].distance_m == 10.0);
}