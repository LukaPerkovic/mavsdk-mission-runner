#pragma once
#include <chrono>
#include <string>
#include <fstream>
#include <mutex>
#include <atomic>
#include <thread>

#include "../data_models/coordinates.hpp"
#include "../data_models/log_record.hpp"

class Logger 
{
public:
    Logger(const std::string& log_filepath);
    ~Logger();

    void update_position(Coords coords, double rel_alt);
    void update_battery(float remaining);

private:
    void run();

    std::chrono::steady_clock::time_point m_start_time;

    std::ofstream        m_file;
    LogRecord            m_latest;
    std::mutex           m_mutex;
    std::atomic<bool>    m_should_stop;
    std::thread          m_thread;

};