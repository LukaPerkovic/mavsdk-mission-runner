# Weekly Progress Log

Notes about done work, missed plans, and next targets structured by weeks. Interval for preceeding week marked on each Sunday

## 21.06.2026.

### What shipped
- Working code that flies drone in square.
- Helper script for calculating destination coordinates based on azimuth (direction) and distance.

### What slipped
- Failed to mee all the goals from previous target.
- Had to fix the conflicts between takeoff() and start_mission(). Kept receiving denied permissions.
- Added library (GeographicLib) had different CMake approach that neeeded to be solved.

### Next target
- Producing logs
- YAML configs


## 13.06.2026.

### What shipped
- Github repository made
- Drone and sim software installed: PX4, QGroundControl, MAVSDK
- Takeoff and landing C++ script ran successfuly and with PX4 and QGC.
- Vim configured to support development in C++.

### What slipped
- Ubunutu 24.04 was installed first; still has some incompatibilities.
- Installation of earlier versions of sims i.e. jmavsdk, was unsuccessful. It's deprecated. Earlier version of Github branch was attempted, but failed. Complete reinstallation with up-to-date libraries solved issues.


### Next target
- Deploying a missing with square drone square path.
- Producing logs
- YAML configs
