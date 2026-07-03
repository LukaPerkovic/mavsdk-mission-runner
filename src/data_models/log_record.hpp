#pragma once
#include "coordinates.hpp"

struct LogRecord
{
    double time_s{0.0};
    Coords position{};
    double rel_alt_m{0.0};
    float battery_pct{0.0f};
};
