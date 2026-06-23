#pragma once
#include <string>
#include <vector>
#include "../data_models/coordinates.hpp"

struct MissionConfig
{
    std::string navigation_mode;
    float takeoff_altitude_m;
    float speed_m_s;
    std::vector<RelativeWaypoint> relative_waypoints;
    std::vector<Coords>           absolute_waypoints;
};

MissionConfig load_mission_config(const std::string& path);
