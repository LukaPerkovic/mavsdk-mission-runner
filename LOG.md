# Weekly Progress Log

Notes about done work, missed plans, and next targets structured by weeks. Interval for preceeding week marked on each Sunday

----

## 12.07.2026.

### What shipped
- Final architecture and documentation done.

### What slipped
- Nothing. Writing docs leaves little room for errors apart schedule planning

### Next steps
- Little break and onward to more ambitious project. Spoilers: ML, Camera, more sims.

Notes about done work, missed plans, and next targets structured by weeks. Interval for preceeding week marked on each Sunday

## 06.07.2026.

### What shipped
- Tests: Unit tests for helper functions
- CI pipeline: Automated via Github Actions

### What slipped
- Huge discrepancy between local package of MAVSDK vs. what is available in .deb file.
- Local installation was done by cloning the source. This included the main version which is *kinda* experimental even though in CMake configs was marked as v3.15.0, but it has a big change which is changed headers from `.h` to `.hpp`.
- `.deb` and explicit source version v3.15.0 still have `.h` header files files
- The only resolution was scrubbing clean the local source, getting 3.15.0 .`deb` explicitly and chaning library header reference from `.hpp` to `.h`.

### Next target
- Architecture markdown done
- Readme completed

## 28.06.2026.

### What shipped
- New script, issue_sim.cpp which simulated long distance travel
- Battery param change in QGC
- Error handling in script when failure happens

### What slipped
- The goal was to approach and handle failures, however, it was later discovered that drone's internal system, OS (PX4 in this case) has it's own failure handling, and this should not be interfered with. Thus, what I could do is just handle the script so it accounts for this failure, and adjust it's mission states accordingly.

### Next target
- Produce logs

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
