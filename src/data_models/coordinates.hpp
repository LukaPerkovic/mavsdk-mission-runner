#pragma once

struct Coords
{
    double latitude;
    double longitude;
};


struct RelativeWaypoint
{
    std::string name;
    double distance_m;
    double azimuth_deg;
};
