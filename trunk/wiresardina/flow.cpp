#include "flow.h"
#include <cstdlib>
#include <sstream>

#include <arpa/inet.h>

Flow::Flow( u_int32_t sourceAddr, 
            u_int16_t sourcePort, 
            u_int32_t destAddr, 
            u_int16_t destPort,
            std::string application )
{
    this->sourceAddr  = sourceAddr;
    this->sourcePort = sourcePort;

    this->destAddr  = destAddr;
    this->destPort   = destPort;
    this->application = application;

    _syn_ack = false;
    _ack = false;

    in_bytes = 0;
    out_bytes = 0;
    inPacketsCount = 0;
    outPacketsCount = 0;
}

Flow::Flow( u_int32_t sourceAddr, 
            u_int16_t sourcePort, 
            u_int32_t destAddr, 
            u_int16_t destPort)
{
    this->sourceAddr  = sourceAddr;
    this->sourcePort = sourcePort;

    this->destAddr  = destAddr;
    this->destPort   = destPort;

    _syn_ack = false;
    _ack = false;

    in_bytes = 0;
    out_bytes = 0;
    inPacketsCount = 0;
    outPacketsCount = 0;
}


bool Flow::operator == (const Flow f)
{
    return ( (sourceAddr == f.sourceAddr) and
             (sourcePort == f.sourcePort) and
             (destAddr   == f.destAddr  ) and
             (destPort   == f.destPort  ) )
    or
           ( (sourceAddr == f.destAddr  ) and
             (sourcePort == f.destPort  ) and
             (destAddr   == f.sourceAddr) and
             (destPort   == f.sourcePort) );
}

u_int16_t Flow::getSourcePort()
{
    return ntohs(sourcePort);
}

u_int16_t Flow::getDestinationPort()
{
    return ntohs(destPort);
}

std::string Flow::getSourceAddress()
{
    struct in_addr inaddr;
    inaddr.s_addr = sourceAddr;

    return std::string(inet_ntoa(inaddr));
}

std::string Flow::getDestinationAddress()
{
    struct in_addr inaddr;
    inaddr.s_addr = destAddr;

    return std::string(inet_ntoa(inaddr));
}

long Flow::getInBytes()
{
    return in_bytes;
}

long Flow::getInpacketsCount(){
	return inPacketsCount;
}

long Flow::getOutpacketsCount(){
	return outPacketsCount;
}


void Flow::increaseInBytes(long bytes)
{
    this->in_bytes += bytes;
    inPacketsCount ++;	
}

long Flow::getOutBytes()
{
    return out_bytes;
}

void Flow::increaseOutBytes(long out_bytes)
{
    this->out_bytes += out_bytes;
    outPacketsCount++;
}

void Flow::increaseOutPckCount(){
    outPacketsCount++;     
}
void Flow::increaseInPckCount(){
    inPacketsCount ++;	
}

std::string Flow::getApplication()
{
    return application;
}

void Flow::setApplication(std::string application)
{
    this->application = application;
}

std::string Flow::tgtAdressToStdString()
{
    std::stringstream stream;

    stream << getDestinationAddress() << ":" << getDestinationPort();

    return stream.str();
}

std::string Flow::tgtPortToStdString()
{
    std::stringstream stream;

    stream << getDestinationPort();

    return stream.str();
}


std::string Flow::srcAdressToStdString(){

    std::stringstream stream;

    stream  << getSourceAddress() << ":" << getSourcePort();

    return stream.str();
}

std::string Flow::srcPortToStdString(){

    std::stringstream stream;

    stream  << getSourcePort();

    return stream.str();
}


void Flow::setRemoteHostService(std::string rtmHostService){

    this->rtmHostService = rtmHostService;
}

std::string Flow::getRtmHostService(){
    
    return rtmHostService;
}
