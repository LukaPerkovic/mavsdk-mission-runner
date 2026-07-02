#include <string>
#include <fstream>
#include <mutex>
#include <atomic>
#include <thread>
#include <stdexcept>
#include <chrono>

#include "logger.hpp"
#include "../data_models/coordinates.hpp"
#include "../data_models/log_record.hpp"


Logger::Logger(const std::string& log_filepath)
    : m_file{log_filepath},
      m_should_stop{false}
{

    if (!m_file.is_open())
    {
        throw std::runtime_error("Could not open log file: " + log_filepath);
    }

    m_start_time = std::chrono::steady_clock::now();

    m_file << "time_s,latitude,longitude,rel_alt_m,battery_pct\n";

    m_thread = std::thread(&Logger::run, this);
}

Logger::~Logger()
{
    m_should_stop = true;
    if (m_thread.joinable())
        m_thread.join();
}


void Logger::update_position(Coords coords, double rel_alt)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_latest.position.latitude = coords.latitude;
    m_latest.position.longitude = coords.longitude;
    m_latest.rel_alt_m = rel_alt;
}

void Logger::update_battery(float remaining)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    m_latest.battery_pct = remaining;
}

void Logger::run()
{
    using namespace std::chrono_literals;
    
    while (!m_should_stop)
    {
        LogRecord snapshot;
        
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            snapshot = m_latest;
        }

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<double> elapsed = now - m_start_time;
        snapshot.time_s = elapsed.count();

        m_file << snapshot.time_s << ","
               << snapshot.position.latitude << "," 
               << snapshot.position.longitude << "," 
               << snapshot.rel_alt_m << "," 
               << snapshot.battery_pct << "\n";
        
        std::this_thread::sleep_for(200ms);
               
    }
}