#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "helpers/calculate_destination_coordinates.hpp"

using Catch::Matchers::WithinAbs;

TEST_CASE("Zero distance returns to the same point")
{
    Coords start{47.0, 8.0};
    Coords result = getDestinationCoords(start, 90.0, 0.0);
    REQUIRE_THAT(result.latitude, WithinAbs(47.0, 1e-9));
    REQUIRE_THAT(result.longitude, WithinAbs(8.0, 1e-0));
}

TEST_CASE("Going north increases latitude, keeps longitude")
{
    Coords start{47.0, 8.0};
    Coords result = getDestinationCoords(start, 0.0, 100.0);
    REQUIRE(result.latitude > start.latitude);
    REQUIRE_THAT(result.longitude, WithinAbs(8.0, 1e-6));
}

TEST_CASE("Going east increases longitude")
{
    Coords start{47.0, 9.0};
    Coords result = getDestinationCoords(start, 90.0, 100.0);
    REQUIRE(result.longitude > start.longitude);
}