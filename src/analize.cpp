#include <iostream>
#include <map>
#include <iomanip>
#include <cstring>
#include <string>


#include "analize.hpp"
#include "utils.hpp"

// We need to ignore first test - to warm caches (code section at least)
const int NUM_OF_TESTS_TO_IGNORE = 10;


unsigned CalcMediana(std::string prefix, unsigned* results, long num_of_results, long max_cpu_freq_in_hz, int verbose_mode)
{

    if(verbose_mode > 2)
    {
        std::cout << prefix << ": RAW RESULTS:" << std::endl;
        for(auto i=0; i < num_of_results; ++i)
        {
            // Print result
            std::cout << "result[" << i << "]: " << results[i] << std::endl;
        }
        std::cout <<  prefix << ": ----------------------" << std::endl;
    }

    long correct_results_num = 0;
    std::map<unsigned, long> stat;
    for(long i=0; i < num_of_results; ++i)
    {
        if(i < NUM_OF_TESTS_TO_IGNORE)
        {
            continue;
        }

        if(results[i] > 1000000000)
        {
            std::cerr <<  prefix << ": ERROR: Your catch too slow result " << Tick2String(results[i], max_cpu_freq_in_hz, verbose_mode) << " ! - suggest you restart the test. If you got the same error - try to make NUM_OF_TESTS smaller!" << std::endl;
            continue;
        }

        correct_results_num += 1;
        auto search = stat.find(results[i]);
        if (search != stat.end()) 
        {
            search->second += 1;
        }
        else
        {
             stat[results[i]] = 1;
        }
    }

    if(verbose_mode > 1)
    {
        std::cout <<  prefix << ": DISTRIBUTION( " << correct_results_num << " test):" << std::endl;
        for (const auto& [time, number] : stat) 
        {
            std::cout << Tick2String(time, max_cpu_freq_in_hz, verbose_mode) << " --->  " << number << std::endl;
        }
    }

    long total = 0;
    for (const auto& [time, number] : stat) 
    {
        total += number;

        if( total > correct_results_num/2)
        {
            return time;
        }
    }

    return 0;
}

