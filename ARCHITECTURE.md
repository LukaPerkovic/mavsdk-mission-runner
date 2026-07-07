# System Architecture

The `mavsdk-mission-runner` project acts as an offboard control application (Companion Computer) that communicates with a flight controller (PX4 Simulator) via the MAVLink protocol.

## 1. High-Level System Architecture

This diagram illustrates how the components interact in the simulated environment.

```mermaid
flowchart TD
    subgraph "mavsdk-mission-runner (C++ Application)"
        App[Mission Scripts\ntakeoff_and_land, fly_a_square, issue_sim]
        MAVSDK[MAVSDK Library]
        App <-->|C++ API| MAVSDK
    end

    subgraph "Simulation Environment"
        PX4[PX4 Autopilot]
        Sim[Physics Simulator / Gazebo Classic]
        PX4 <--> Sim
    end

    GCS[QGroundControl]

    MAVSDK <--> |MAVLink over UDP\nport 14540| PX4
    GCS <-->|MAVLink over UDP| PX4
```
- **mavsdk-mission-runner:** Higher level mission logic, writen in C++, utliziing MAVSDK.
- **PX4 Autopilot:** The core flight stack responsible for stabilization and low level navigation.
- **QGroundControl:** The Ground Control Station (GCS) used by human to monitor telemetry, manual overrides, and overall status.

## 2. Internal Software Architecture

The C++ codebase is divided into two primary layers: **Application Layer** (executables) and **Library Layer** (`mission_lib`).

```mermaid
flowchart LR
    subgraph "Configuration"
        YAML[YAML Config Files]
    end

    subgraph "Library Layer (mission_lib)"
        Config[mission_config]
        Math[calculate_destination_coordinates]
        LogSys[logger]
        Models[Data Models: Coords, LogRecord]
    end

    subgraph "Application Layer"
        Exec1[takeoff_and_land.cpp]    
        Exec2[fly_a_square.cpp]
        Exec3[issue_sim.cpp]
    end

    subgraph "Outputs"
        Logs[Telemetry CSV Logs]
        Drone[MAVSDK Drone Actions]
    end

    YAML -->|Loaded via yaml-cpp| Config
    Config --> Exec2 & Exec3
    Math --> Exec2 & Exec3
    Exec1 & exec2 --> LogSys
    LogSys --> Logs
    Exec1 & Exec2 & Exec3 <--> Drone
```

### Component Breakdown
1. Executables (Application Layer): Scripts like `fly_a_square.cpp` and `issue_sim.cpp` orchestrate the MAVSDK setup. They parse command line arguments and load the appropriate configurations, and subscribe to different callbacks e.g. telemetry.
2. `mission_lib` (Library Layer):
    - Data Models: Define pure data structures like `Coords` and `RelativeWaypoint`
    - Mission Config: Parses `.yaml` files to decouple mission waypoints (speeds, altitudes, distances) from hardcode C++ logic.
    - Calculations: Uses `GeographicLib` to calculate absolute GPS coordinates based on relative distance and azimuth.
    - Logger: A thread safe logger that periodically flushes telemetry data (altitude, coordinates, battery) to a `.csv` file.