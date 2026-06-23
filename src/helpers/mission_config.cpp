#include <yaml-cpp/yaml.h>
#include <string>
#include <vector>

#include "../data_models/coordinates.hpp"
#include "mission_config.hpp"

MissionConfig load_mission_config(const std::string& path)
{
    MissionConfig cfg;

    YAML::Node root = YAML::LoadFile(path);
    YAML::Node mission = root["mission"];

    cfg.navigation_mode    = mission["navigation_mode"].as<std::string>();
    cfg.takeoff_altitude_m = mission["takeoff_altitude_m"].as<float>();
    cfg.speed_m_s          = mission["speed_m_s"].as<float>();

    for (const auto& node : mission["relative_waypoints"])
    {
        RelativeWaypoint wp;
        wp.name        = node["name"].as<std::string>();
        wp.distance_m  = node["distance_m"].as<double>();
        wp.azimuth_deg = node["azimuth_deg"].as<double>();
        cfg.relative_waypoints.push_back(wp);
    }

    for (const auto& node : mission["absolute_waypoints"])
    {
        Coords c;
        c.latitude  = node["latitude_deg"].as<double>();
        c.longitude = node["longitude_deg"].as<double>();
        cfg.absolute_waypoints.push_back(c);
    }

    return cfg;
}
