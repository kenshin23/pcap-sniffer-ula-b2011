#ifndef PACKET_HEADER_H
#define PACKET_HEADER_H

#include <netinet/ether.h>  /* Link      Layer */
#include <netinet/ip.h>     /* Network   Layer */
#include <netinet/tcp.h>    /* Transport Layer */

#include <string>

class PacketHeader
{
    struct iphdr ip;
    struct tcphdr tcp;

    public:
    PacketHeader();

    void setIpHeader(struct iphdr ip);
    struct iphdr getIpHeader();

    void setTcpHeader(struct tcphdr tcp);
    struct tcphdr getTcpHeader();

    std::string getSourceAddress();
    std::string getDestinationAddress();

    int getDataSize();
int getPacketSize();
    bool isSyn();
    bool isSynAck();
    bool isAck();
    bool isFin();
};

#endif // PACKET_HEADER_H
