cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(mavsdk-mission-runner)

list(APPEND CMAKE_MODULE_PATH "/usr/share/cmake/geographiclib")

find_package(MAVSDK REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(GeographicLib REQUIRED)
find_package(Threads REQUIRED)


add_library(mission_lib
	src/helpers/calculate_destination_coordinates.cpp
	src/helpers/mission_config.cpp
    src/helpers/logger.cpp
)

target_include_directories(mission_lib PUBLIC
	${GeographicLib_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(mission_lib PUBLIC
	yaml-cpp
	${GeographicLib_LIBRARIES}
	Threads::Threads
)

# --- Executables ---
add_executable(takeoff_and_land	src/takeoff_and_land.cpp)
target_link_libraries(takeoff_and_land PRIVATE MAVSDK::mavsdk)

add_executable(fly_a_square	src/fly_a_square.cpp)
target_link_libraries(fly_a_square PRIVATE MAVSDK::mavsdk mission_lib)

add_executable(issue_sim src/fly_a_square.cpp)
target_link_libraries(issue_sim PRIVATE MAVSDK::mavsdk mission_lib)


if(NOT MSVC)
	foreach(tgt takeoff_and_land fly_a_square issue_sim mission_lib)
		target_compile_options(${tgt} PRIVATE -Wall -Wextra)
	endforeach()
endif()


# --- Tests ---
option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.5.2
    )
    FetchContent_MakeAvailable(Catch2)

    add_executable(unit_tests
        tests/test_destination.cpp
        tests/test_mission_config.cpp
    )
    target_link_libraries(unit_tests PRIVATE mission_lib Catch2::Catch2WithMain)

    target_compile_definitions(unit_tests PRIVATE
        FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests/fixtures"
    )

    list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
    include(Catch)
    catch_discover_tests(unit_tests)
endif()