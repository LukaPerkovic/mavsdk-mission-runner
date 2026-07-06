#pragma once
#include "../data_models/coordinates.hpp"

Coords getDestinationCoords(const Coords& startPosition, double azimuth,
                            double distance);
