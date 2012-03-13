#include "packetheader.h"

#include <arpa/inet.h>

PacketHeader::PacketHeader()
{
    //
}

void PacketHeader::setIpHeader(struct iphdr ip)
{
    this->ip = ip;
}

struct iphdr PacketHeader::getIpHeader()
{
    return ip;
}

void PacketHeader::setTcpHeader(struct tcphdr tcp)
{
    this->tcp = tcp;
}

struct tcphdr PacketHeader::getTcpHeader()
{
    return tcp;
}

std::string PacketHeader::getSourceAddress()
{
    struct in_addr inaddr;
    inaddr.s_addr = ip.saddr;

    return std::string(inet_ntoa(inaddr));
}

std::string PacketHeader::getDestinationAddress()
{
    struct in_addr inaddr;
    inaddr.s_addr = ip.daddr;

    return std::string(inet_ntoa(inaddr));
}

int PacketHeader::getDataSize()
{
    /*
        +-----------+------------+----------------------+
        | IP Header | TCP Header |         Data         |
        +-----------+------------+----------------------+
    */
         //      total leng   -                            IP Header   -            TCP Header
    return (ntohs(ip.tot_len) - sizeof(struct iphdr) - sizeof(struct tcphdr));
}

int PacketHeader::getPacketSize()
{
    /*
        +-----------+------------+----------------------+
        | IP Header | TCP Header |         Data         |
        +-----------+------------+----------------------+
    */
    return (ntohs(ip.tot_len));
}


bool PacketHeader::isSyn()
{
    return (tcp.syn) and (not tcp.ack);
}

bool PacketHeader::isSynAck()
{
    return (tcp.syn) and (tcp.ack);
}

bool PacketHeader::isAck()
{
    return (not tcp.syn) and (tcp.ack) and (not tcp.fin);
}

bool PacketHeader::isFin()
{
    return (tcp.fin);
}
