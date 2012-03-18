
#ifndef STDOUTPUTREADER_h
#define STDOUTPUTREADER_h

#include <string>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFFER 100
using namespace std;

class StdOutputReader
{
    // setup
    std::string data;
    std::string netstat;
    FILE *      stream;
    char        buffer[MAX_BUFFER];
    std::string ip;
    size_t      found;
    size_t      found2;
    std::string command;
    std::string interface;

    // do it

    public:
        StdOutputReader();

        void setInterface();
        std::string getLines();
        std::string ifconfigUbuntu(string interface);
        std::string netstat_tnpUbuntu();

        ~StdOutputReader()
        {
        }

        ;

        // private:
        std::string read(std::string cmd);
};
#endif //STDOUTPUTREADER_h


//~ Formatted by Jindent --- http://www.jindent.com
