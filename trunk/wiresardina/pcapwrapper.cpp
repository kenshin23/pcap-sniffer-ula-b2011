
#include "pcapwrapper.h"

// #include "stdoutputreader.h"

#include <netinet/in.h>
#include <cstring>
#include <string>
#include <iostream>
#include <net/ethernet.h>   /* Link      Layer */
#include <netinet/tcp.h>    /* Transport Layer */
#include <netinet/ip.h>     /* Network   Layer */
#include <arpa/inet.h>

using namespace std;

void callback(u_char *                 data,
              const struct pcap_pkthdr *,
              const u_char *           packet)
{
    // struct ethhdr * ethptr = (struct ethhdr*)(packet);
    // if(ethptr->h_proto != ETH_P_IP)  // != IP
    // return;
    struct iphdr *ipprt;


    ipprt = (struct iphdr*) (packet + sizeof(struct ether_header));

    // TODO: Falta agregar filtros
    if (ipprt -> protocol != IPPROTO_TCP)    // != TCP
    {
        return;
    }

    struct tcphdr *tcpptr;


    tcpptr = (struct tcphdr*) (packet + sizeof(struct ether_header) + sizeof(struct iphdr));

    std::queue<PacketHeader> * packets = reinterpret_cast<std::queue<PacketHeader> *>(data);
    PacketHeader               pkt;

    pkt.setTcpHeader(*tcpptr);
    pkt.setIpHeader(*ipprt);
    packets -> push(pkt);
}

PcapWrapper::PcapWrapper():
    started(false)
{
    //
}

PcapWrapper::~PcapWrapper()
{
    stopCapture();
}

bool PcapWrapper::isStarted()
{
    return started;
}

void PcapWrapper::setInterfaceName(std::string deviceName)
{
    this -> interfaceName = deviceName;

    // this->interfaceName = "wlan0";
}

std::string PcapWrapper::getInterfaceName()
{
    return interfaceName;
}

std::list<Interface> PcapWrapper::getInterfaces()
{
    std::list<Interface> list;

    pcap_if_t * interfaces;
    pcap_findalldevs(&interfaces, err);

    while (interfaces -> next)
    {
        pcap_t * tmp           = pcap_open_live(interfaces -> name, BUFSIZ, false, -1, err);
        int      datalink_type = pcap_datalink(tmp);

        pcap_close(tmp);

        Interface device;

        device.name          = interfaces -> name;
        device.datalink_type = datalink_type;

        list.push_back(device);

        interfaces = interfaces -> next;
    }

    return list;
}

std::queue<PacketHeader> & PcapWrapper::getCapturedPackets()
{
    return captured_packets;
}

std::string PcapWrapper::getHostIpAddres()
{
    bpf_u_int32 mask;    /* Our netmask */
    bpf_u_int32 net;     /* Our IP */

    // char device[] = "wlan0";
    int ret = pcap_lookupnet(interfaceName.c_str(), &net, &mask, err);

    if (ret == -1)
    {
        return std::string();
    }

    StdOutputReader reader;
    std::string     ip = reader.ifconfigUbuntu(interfaceName);

    return ip;
}

std::string PcapWrapper::getPcapError()
{
    return std::string(err);
}

void PcapWrapper::startCapture()
{
    started = true;

    run();
}

void PcapWrapper::stopCapture()
{
    if (not started)
    {
        return;
    }

    if (handler == NULL)
    {
        return;
    }

    pcap_breakloop(handler);
    pcap_close(handler);

    started = false;
}

void PcapWrapper::run()
{    // char device[] = "wlan0";
    handler = pcap_open_live(interfaceName.c_str(), BUFSIZ, false, LOOP_FOREVER, err);

    // -------------------------------------------ES Necesario ser Super Usuario---------------------------------------------
    pcap_loop(handler, LOOP_FOREVER, callback, (u_char *) (&captured_packets));
}

std::string PcapWrapper::stdOutputReader(std::string cmd)
{
    // setup
    std::string data;

    FILE * stream;

    char buffer[3];

    // do it
    stream = popen(cmd.c_str(), "r");

    while (fgets(buffer, 3, stream) != NULL)
    {
        data.append(buffer);
    }

    pclose(stream);

    // exit
    return data;
}


//~ Formatted by Jindent --- http://www.jindent.com
