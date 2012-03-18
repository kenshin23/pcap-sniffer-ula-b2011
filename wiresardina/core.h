
#ifndef CORE_H
#define CORE_H

#include "packetheader.h"
#include "flow.h"
#include <vector>
#include <fstream>
#include <map>
#include <stdlib.h>
#include <sys/timeb.h>

#define NOT_FOUND -1
#define INFO_FILE "/etc/services"
using namespace std;

class Core
{
    double           in_kbytes;
    double           out_kbytes;
    int              pckt1500;
    int              pckt1460;
    int              pckt1420;
    int              pckt1064;
    int              pckt428;
    int              pckt100;
    int              pckt80;
    int              pckt72;
    int              pckt64;
    int              pckt52;
    int              pckt50;
    int              pckt43;
    int              countHoras;
    int              processed_packets;
    vector<Flow>     flows;
    vector<Flow>     initiated;
    std::string      ip;
    fstream          file;
    fstream          filePacktCount;
    fstream          filePktCountFrec;
    fstream          fileTrafficVol;
    map<int, int>    frecuencia;
    map<int, string> puertos_mapeo;

    public:
        Core();

        Core(std::string fileName);

        ~Core();

        void setAppName(int    pos,
                        string app);
        void processPacket(PacketHeader packet);
        double getInKByteS();
        double getOutKByteS();
        void countPacket(PacketHeader packet);
        long getPacketsCount();
        vector<Flow> & getInitiatedFlows();
        void dump(Flow flow);
        void writeFlowToFile(Flow flow);

        struct timeb tmb;


        struct timeb tmb2;


        int  count;
        bool bandera;
        int  binCount;

        void printFrenc();
        void printHistogram();
        void printTrafficVol();

    private:
        int searchForFlow(vector<Flow> flows,
                          Flow &       flow);
        std::string findAppNameByPort(std::string port);
        int initPortServices(map<int, string> & mapeo);
        int packetFrecCount(PacketHeader packet);
        std::string getHostServicePort(Flow flow);

    public:
        void setIp(std::string ip);
};
#endif // CORE_H


//~ Formatted by Jindent --- http://www.jindent.com
