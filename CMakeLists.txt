cmake_minimum_required(VERSION 3.10.2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(mavsdk-mission-runner)

list(APPEND CMAKE_MODULE_PATH "/usr/share/cmake/geographiclib")

find_package(MAVSDK REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(GeographicLib REQUIRED)

# --- Take off and land program ---
add_executable(takeoff_and_land
	src/takeoff_and_land.cpp
	)
target_link_libraries(takeoff_and_land
	MAVSDK::mavsdk
	)


# --- Fly a square program ---
add_executable(fly_a_square
	src/fly_a_square.cpp
	src/helpers/calculate_destination_coordinates.cpp
	)
target_include_directories(fly_a_square PRIVATE
	${GeographicLib_INCLUDE_DIRS}
	)

target_link_libraries(fly_a_square 
	MAVSDK::mavsdk
	yaml-cpp
	${GeographicLib_LIBRARIES}
	)



if(NOT MSVC)
	add_compile_options(takeoff_and_land PRIVATE -Wall -Wextra)
else()
	add_compile_options(takeoff_and_land PRIVATE /W2)
endif()

