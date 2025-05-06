#include "include/computer_club.h"

int main(int argc, char *argv[])
{
    setlocale(LC_ALL, "Russian");
    if (argc != 2)
    {
        printUsage();
        return 1;
    }

    return run_computer_club(argv[1], std::cout);
}