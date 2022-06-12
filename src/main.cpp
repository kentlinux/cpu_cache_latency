#include <iostream>
#include <string>
#include <unistd.h>

#include "mesurament.hpp"

void PrintHelp()
{
    std::cout << "Usage:" << std::endl;
    std::cout << "     ccl [-v] [-vv] [-vvv] [-h]" << std::endl;
    std::cout << "where:" << std::endl;
    std::cout << "  -h             - print this message (help) and exit" << std::endl;
    std::cout << "  -v             - verbose mode 1 (print ticks)" << std::endl;
    std::cout << "  -vv            - verbose mode 2 (print distribution)" << std::endl;
    std::cout << "  -vvv           - verbose mode 3 (print raw data)" << std::endl;
}

int main(int argc, char* argv[])
{
    int verbose_mode = 0;

    auto i = 1;

    while(i < argc)
    {
        auto argv_str = std::string(argv[i]);

        if(argv_str == "-h")
        {
            PrintHelp();
            return 0;
        }
        else if(argv_str == "-v")
        {
            verbose_mode = 1;
        }
        else if(argv_str == "-vv")
        {
            verbose_mode = 2;
        }
        else if(argv_str == "-vvv")
        {
            verbose_mode = 3;
        }
        else
        {
            std::cerr << "ERROR: Unsupported option \"" << argv[i]<< "\"" << std::endl;
            PrintHelp();
            return 1;
        }

        ++i;
    }

    return do_mesurament(verbose_mode);
}
