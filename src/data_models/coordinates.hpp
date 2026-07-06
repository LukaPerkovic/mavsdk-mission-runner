#pragma once
#include <string>

struct Coords
{
    double latitude {0.0};
    double longitude {0.0};
};


struct RelativeWaypoint
{
    std::string name;
    double distance_m;
    double azimuth_deg;
};
