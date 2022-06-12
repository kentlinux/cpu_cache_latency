#include <iostream>
#include <cstring>
#include <string>

#include <pthread.h>

#include "utils.hpp"
#include "analize.hpp"

// Total number of test we take big enought to make statistics works.
// But not too big - we don't want to work more then scheduler time slice
// (may be even less - try to avoid IRQ effects...)
const long NUM_OF_TESTS = 1024*256;
const long NUM_OF_RDTSC_TESTS = 1024*256;


struct CTX
{
    long num_of_tests;
    unsigned *results;

    char buff1[256]; // to move write_time to separate cache-line from other fields of struct
    volatile unsigned write_time;
    char buff2[256];// to move write_time to separate cache-line from other fields of struct
};

void *ProducerFunc(void * arg)
{
    auto ctx = (CTX*)arg;

    // Copy to local variable
    auto num_of_tests = ctx->num_of_tests;

    unsigned write_time = 0;
    unsigned write_time_hi = 0;

    for(long i = 0; i < num_of_tests; ++i)
    {
        // Wait for start
        while(ctx->write_time != 0) 
        {
            continue;
        }


        // Get time by rdtsc instruction and save it
        __asm__ __volatile__ ("rdtsc" : "=a"(write_time), "=d"(write_time_hi));
        ctx->write_time = write_time;
    }   
    return NULL;
}

void *ConsumerFunc(void * arg)
{
    auto ctx = (CTX*)arg;
   
    // Copy to local variable
    auto num_of_tests = ctx->num_of_tests;
    auto results = new unsigned[num_of_tests];

    unsigned read_time = 0;
    unsigned read_time_hi = 0;

    ctx->write_time = 0; // Start test

    for(long i=0; i < num_of_tests; ++i)
    {
        // Wait
        while(ctx->write_time == 0) 
        {
            continue;
        }
    
        __asm__ __volatile__ ("rdtsc" : "=a"(read_time), "=d"(read_time_hi));
    
        results[i] = read_time - ctx->write_time;
        ctx->write_time = 0;  // Start next test
    }

    ctx->results = results;

    return NULL;
}

unsigned GetRdtscLatency(int verbose_mode, long max_cpu_freq_in_hz)
{
    unsigned befor_time = 0;
    unsigned befor_time_hi = 0;
    unsigned after_time = 0;
    unsigned after_time_hi = 0;

    auto results = new unsigned[NUM_OF_RDTSC_TESTS];

    for(long i = 0; i < NUM_OF_RDTSC_TESTS; ++i)
    {
        // Get time by rdtsc instruction and save it
        __asm__ __volatile__ ("rdtsc" : "=a"(befor_time), "=d"(befor_time_hi));
        __asm__ __volatile__ ("rdtsc" : "=a"(after_time), "=d"(after_time_hi));
        results[i] = after_time - befor_time;
    }


    return CalcMediana("RDTSC", results, NUM_OF_RDTSC_TESTS, max_cpu_freq_in_hz, verbose_mode);
}

int do_mesurament(int verbose_mode)
{
    if(!CheckHardware())
    {
        std::cerr << "ERROR: Your CPU is too old! Buy a new one!!!" << std::endl;
        return 256;
    }

    auto max_cpu_freq_in_hz = ReadMaxCpuFreqInHZ();
    if(max_cpu_freq_in_hz == 0)
    {
        std::cerr << "ERROR: faile to get max_cpu_freq!" << std::endl;
        return 1;
    }

    auto rdtsc_latency = GetRdtscLatency(verbose_mode, max_cpu_freq_in_hz);
    if(rdtsc_latency == 0)
    {
        std::cerr << "ERROR: faile to get rdtsc_latency!" << std::endl;
        return 1;
    }

    pthread_t consumerThread;
    pthread_t producerThread;
    int err;

    auto ctx = new CTX();
    ctx->write_time = 1;
    ctx->num_of_tests = NUM_OF_TESTS;

    
    // Start threads
    err = pthread_create(&consumerThread, NULL, &ConsumerFunc, ctx);
    if (err)
    {
        std::cout << "Consumer thread creation failed : " << std::strerror(err) << std::endl;
        return err;
    }
    
    err = pthread_create(&producerThread, NULL, &ProducerFunc, ctx);
    if (err)
    {
        std::cout << "Producer thread creation failed : " << std::strerror(err) << std::endl;
        return err;
    }
   
    // Wait
    err = pthread_join(consumerThread, NULL);
    if (err)
    {
        std::cout << "Consumer thread failed to join : " << std::strerror(err) << std::endl;
        return err;
    }

    err = pthread_join(producerThread, NULL);
    if (err)
    {
        std::cout << "Producer thread failed to join : " << std::strerror(err) << std::endl;
        return err;
    }

    auto cache_latency = CalcMediana("CACHE", ctx->results, ctx->num_of_tests, max_cpu_freq_in_hz, verbose_mode);
    
    if(verbose_mode > 0)
    {
        std::cout <<"RDTSC_MEDIANA : " << Tick2String(rdtsc_latency, max_cpu_freq_in_hz, verbose_mode) << std::endl;
        std::cout <<"MEDIANA_WITH_RDTSC : " << Tick2String(cache_latency, max_cpu_freq_in_hz, verbose_mode) << std::endl;
    }
    std::cout <<"MEDIANA : " << Tick2String(cache_latency - rdtsc_latency, max_cpu_freq_in_hz, verbose_mode) << std::endl;

    delete[] ctx->results;
    delete ctx;

    return 0;
}

