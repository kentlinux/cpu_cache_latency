#include <iostream>
#include <cstring>
#include <fstream>
#include <string>
#include <sstream>

#include <iostream>
#include <map>
#include <iomanip>
#include <cstring>
#include <string>

#include "utils.hpp"

double Tick2Nano(unsigned tick, long max_cpu_freq_in_hz)
{
    return (double)tick / max_cpu_freq_in_hz * 1000000000;
}

std::string Tick2String(unsigned tick, long max_cpu_freq_in_hz, int verbose_mode)
{
    std::stringstream stream;
    stream << std::fixed << std::setprecision(0) << Tick2Nano(tick, max_cpu_freq_in_hz) << " nanosec";
    if(verbose_mode > 0)
    {
        stream << " ( " << tick<< " ticks )";
    }
    return stream.str();
}

// Get MAX CPU frequency:
long ReadMaxCpuFreqInHZ()
{
    static const char* FILE_WITH_MAX_CPU_FREQ = "/sys/devices/system/cpu/cpu2/cpufreq/cpuinfo_max_freq";

    std::ifstream input_file(std::string(FILE_WITH_MAX_CPU_FREQ), std::ios::binary);

    try
    {
        if (!input_file.is_open()) 
        {
            std::cerr << "ERROR: Could not open the file - '" << FILE_WITH_MAX_CPU_FREQ << "'" << std::endl;
            return 0;
        }

        std::string max_cpu_freq_str;
        std::getline(input_file, max_cpu_freq_str);

        return std::stol(max_cpu_freq_str)*1000;
    }
    catch(...)
    {
        std::cerr << "ERROR: faile to read or parse max_cpu_freq!" << std::endl;
    }
    input_file.close();
    return 0;
}


// Check the CPU has flags:
//  
//    tcs, rdtscp, constant_tsc, nonstop_tsc
//  
// if no such flags - CPU too old for out mesurament method
bool CheckHardware()
{
    static const char* FILE_WITH_CPU_FLAGS = "/proc/cpuinfo";
    std::ifstream input_file(std::string(FILE_WITH_CPU_FLAGS), std::ios::binary);
    try
    {
        if (!input_file.is_open()) 
        {
            std::cerr << "ERROR: Could not open the file - '" << FILE_WITH_CPU_FLAGS << "'" << std::endl;
            return false;
        }

        std::string flags = "";
        std::string line;
        while (std::getline(input_file, line))
        {
            if(line.starts_with("flags"))
            {
                flags = line;
                break;
            }
        }
        input_file.close();

        if(flags == "")
        {
            return false;
        }

        bool got_tsc = false;
        bool got_rdtscp = false;
        bool got_constant_tsc = false;
        bool got_nonstop_tsc = false;


        std::istringstream iss(flags);
        std::string flag;
        while ( getline( iss, flag, ' ' ) ) 
        {
            if(flag == "tsc")
            {
                got_tsc = true;
            }
            else if(flag == "rdtscp")
            {
                got_rdtscp = true;
            }
            else if(flag == "constant_tsc")
            {
                got_constant_tsc = true;
            }
            else if(flag == "nonstop_tsc")
            {
                got_nonstop_tsc = true;
            }
        }

        return got_tsc && got_rdtscp && got_constant_tsc && got_nonstop_tsc;
    }
    catch(...)
    {
        std::cerr << "ERROR: faile to read or parse max_cpu_freq!" << std::endl;
    }
    input_file.close();
    return false;
}


