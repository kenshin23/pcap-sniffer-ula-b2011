
#ifndef FLOW_H
#define FLOW_H

#include <netinet/ip.h>     /* Network   Layer */
#include <netinet/tcp.h>    /* Transport Layer */
#include <string>

class Flow
{
    u_int32_t   sourceAddr;    /* Source Address */
    u_int16_t   sourcePort;    /* Source Port */
    u_int32_t   destAddr;      /* Destination Address */
    u_int16_t   destPort;      /* Destination Port */
    long        in_bytes;
    long        out_bytes;
    long        inPacketsCount;
    long        outPacketsCount;
    std::string application;
    std::string rtmHostService;

    public:
        bool _syn_ack;
        bool _ack;

    public:
        Flow(u_int32_t saddr,
             u_int16_t source,
             u_int32_t daddr,
             u_int16_t dest);

        Flow(u_int32_t   saddr,
             u_int16_t   source,
             u_int32_t   daddr,
             u_int16_t   dest,
             std::string servicio);

        bool operator ==(const Flow);
        u_int16_t getSourcePort();
        u_int16_t getDestinationPort();
        std::string getSourceAddress();
        std::string getRtmHostService();
        std::string getDestinationAddress();
        std::string srcAdressToStdString();
        std::string tgtAdressToStdString();
        std::string tgtPortToStdString();
        std::string srcPortToStdString();
        long getInBytes();
        long getInpacketsCount();
        long getOutpacketsCount();
        void increaseOutPckCount();
        void increaseInPckCount();
        void increaseInBytes(long in_bytes);
        long getOutBytes();
        void increaseOutBytes(long out_bytes);
        std::string getApplication();
        void setApplication(std::string application);
        void setRemoteHostService(std::string rtmHostService);
        std::string remoteToStdString();
        std::string localToStdString();
};
#endif // FLOW_H


//~ Formatted by Jindent --- http://www.jindent.com
