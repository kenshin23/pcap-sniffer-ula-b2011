#ifndef PCAPWRAPPER_H
#define PCAPWRAPPER_H

#include <pcap.h> /* Pcap */
#include "packetheader.h"
#include "stdoutputreader.h"

#include <queue>
#include <string>
#include <list>
//#include <pthread.h>

#define LOOP_FOREVER -1
using namespace std;


typedef struct interface {
    char *name;
    int datalink_type;
} Interface;

class PcapWrapper {

    char err[PCAP_ERRBUF_SIZE];
    pcap_t * handler;

    std::string interfaceName;
    std::queue<PacketHeader> captured_packets;

    bool started;

public:
    PcapWrapper();
    ~PcapWrapper();

    bool isStarted();
	
    std::string stdOutputReader(std::string command);		
    std::string getInterfaceName();
    std::list<Interface> getInterfaces();

    std::queue<PacketHeader> & getCapturedPackets();

    std::string getHostIpAddres();

    std::string getPcapError();

    void run();
    void startCapture();
    void stopCapture();
/* Thread Functions */
//    pthread_create(&mythread, NULL, run, NULL); // Creamos el thread	
//    pthread_exit(NULL);    // Última función que debe ejecutar el main() siempre
    void setInterfaceName(std::string interfaceName);

};

#endif // PCAPWRAPPER_H
