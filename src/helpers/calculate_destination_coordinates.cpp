#include "calculate_destination_coordinates.hpp"
#include <GeographicLib/Geodesic.hpp>

Coords getDestinationCoords(const Coords& startPosition, double azimuth,
                            double distance)
{

    const GeographicLib::Geodesic &geod = GeographicLib::Geodesic::WGS84();
    Coords result;
    geod.Direct(startPosition.latitude, startPosition.longitude, azimuth,
                distance, result.latitude, result.longitude);

    return result;
}
