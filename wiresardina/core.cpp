#include "core.h"
#include <stdio.h>
#include <iostream>
#include <arpa/inet.h>

Core::Core()
{
    in_kbytes = 0;
    out_kbytes = 0;
    processed_packets = 0;
    binCount = 0;
    file.open("capture.txt",ios::out);
    filePacktCount.open("packtCount.txt",ios::out);
    filePktCountFrec.open("pcktCountFrec.txt",ios::out);
    fileTrafficVol.open("trafficVol.txt",ios::out);
    fileTrafficVol << "0\t" << "5-minutes bin\n";
    pckt1500 = 0;
    pckt1460 = 0;
    pckt1420 = 0;
    pckt1064 = 0;
    pckt428 = 0;
    pckt100 = 0;
    pckt80 = 0;
    pckt72 = 0;
    pckt64 = 0;
    pckt52 = 0;
    pckt50 = 0;
    pckt43 = 0;
    count = 0;
    bandera = true;
    countHoras = 1;

}

Core::Core(std::string fileName )
{
    in_kbytes = 0;
    out_kbytes = 0;
    processed_packets = 0;
    binCount = 0;
    filePacktCount.open("packtCount.txt",ios::out);
    file.open(fileName.c_str(),ios::out);
    filePktCountFrec.open("pcktCountFrec.txt",ios::out);
    fileTrafficVol << "0\t" << "5-minutes bin\n";
    pckt1500 = 0;
    pckt1460 = 0;
    pckt1420 = 0;
    pckt1064 = 0;
    pckt428 = 0;
    pckt100 = 0;
    pckt80 = 0;
    pckt72 = 0;
    pckt64 = 0;
    pckt52 = 0;
    pckt50 = 0;
    pckt43 = 0;
    count = 0;
    bandera = true;
    countHoras = 1;

}

Core::~Core()
{
    while(not initiated.empty()) {

        Flow flow = initiated.back();
        dump(flow);

        initiated.pop_back();

    }

    if(file.is_open())
        file << "# Packets:" << getPacketsCount() << ":" << endl;
    file.close();
    filePacktCount.close();

}

void Core::setAppName(int pos, string app)
{
    if(pos < 0 or pos >= initiated.size() )
        return;

    Flow flow = initiated[pos];
    flow.setApplication(app);
    initiated[pos] = flow;
}

std::string Core::findAppNameByPort(std::string port){

}

void Core::processPacket(PacketHeader packet)
{
    double datasize;
    datasize = packet.getPacketSize();
    countPacket(packet);
    packetFrecCount(packet);
    processed_packets++;

    //cout << "packet size " << datasize  << endl;
//    cout << (datasize/1024) << "KB" << endl;

    std::string src = packet.getSourceAddress();
    std::string tgt = packet.getDestinationAddress();

    /*
    cout << "src" << src << endl;
    cout << "tgt" << tgt << endl;
*/
    if( ip.compare(tgt) == 0 ) // In
        in_kbytes += (datasize/1024.0);

    if( ip.compare(src) == 0 ) //Out
        out_kbytes += (datasize/1024.0);

    struct iphdr ip_header   = packet.getIpHeader();
    struct tcphdr tcp_header = packet.getTcpHeader();

    u_int32_t sourceAddr  = ip_header.saddr;
    
    u_int16_t sourcePort = tcp_header.source;

    u_int32_t destAddr  = ip_header.daddr;
    u_int16_t destPort   = tcp_header.dest;

//    u_int16_t rmtHostPort;
//    rmtHostPort = getHostServicePort(sourcePort ,destPort);
//    string servicio = "Vacio" ;
//
//    if(mapeo.find( rmtHostPort ) !=  mapeo.end() ){
//       servicio = mapeo.find(rmtHostPort)->second;
//    }


    Flow flow(sourceAddr,sourcePort,destAddr,destPort);

    // SYN
    if( packet.isSyn() ) {

        if( packet.getDestinationAddress().compare(ip) == 0)
            flow.increaseInBytes(datasize);

        else
            flow.increaseOutBytes(datasize);



        flows.push_back(flow);
        return;
    }
    // SYN + ACK
    if( packet.isSynAck() ) {

        int found = searchForFlow(flows,flow);

        if( found == NOT_FOUND )
            return;

        if( not flow._syn_ack ){

            flows[found]._syn_ack = true;

            if( packet.getDestinationAddress().compare(ip) == 0)
                flows[found].increaseInBytes(datasize);
            else
                flows[found].increaseOutBytes(datasize);
        }
        return;
    }

    // ACK
    if( packet.isAck() ) {

        int found = searchForFlow(flows,flow);

        if( found != NOT_FOUND ) {

            if( (not flow._ack) and (flow._syn_ack) ) {
                // Flujo Nuevo Establecido

                flows[found]._ack = true;

                if( packet.getDestinationAddress().compare(ip) == 0)
                    flows[found].increaseInBytes(datasize);
                else
                    flows[found].increaseOutBytes(datasize);

                //Flow Detected
                flows.erase(flows.begin()+found);
                initiated.push_back(flow);

            }

        }
        else {

            found = searchForFlow(initiated,flow);

            if( found == NOT_FOUND )
                return;
            //si llega un ack a este parte tiene que ser de un flojo inisiado
            if( packet.getDestinationAddress().compare(ip) == 0){
                initiated[found].increaseInBytes(datasize);
                initiated[found].increaseInPckCount();
            }
            else{
                initiated[found].increaseOutBytes(datasize);
                initiated[found].increaseOutPckCount();
            }
        }

        return;

    }

    // FIN
    if( packet.isFin() ){

        int found = searchForFlow(initiated,flow);

        if( found == NOT_FOUND )
            return;

        if( packet.getDestinationAddress().compare(ip) == 0)
            initiated[found].increaseInBytes(datasize);
        else
            initiated[found].increaseOutBytes(datasize);

        flow = initiated[found];
        //poner alarma
        dump(flow);

        initiated.erase(initiated.begin() + found);
        //        qDebug("flow erased...");

        return;

    }

    // Any kind of packet

    int found = searchForFlow(initiated,flow);

    if( found == NOT_FOUND )
        return;

    if( packet.getDestinationAddress().compare(ip) == 0)
        initiated[found].increaseInBytes(datasize);
    else
        initiated[found].increaseOutBytes(datasize);

}

double Core::getInKByteS()
{
    return in_kbytes;
}

double Core::getOutKByteS()
{
    return out_kbytes;
}

long Core::getPacketsCount()
{
    return processed_packets;
}

vector<Flow> & Core::getInitiatedFlows()
{
    return initiated;
}

int cont=0;


void Core::dump(Flow flow)
{

    char localAdress[30];
    char remoteAdress[30];
    char inKB[17];
    char outKB[18];
    char app[50];
    char service[24];


    if (cont==0)
    {

        char cliente[30];
        char remoteHost[32];
        char inPacket[37];
        char outPacket[39];

        bandera = true;
  sprintf(cliente,"%-23s","Client");
  sprintf(remoteHost,"%-23s","Remote Host");
  sprintf(app,"%-15s","App");
  sprintf(service,"%-9s","Srv Port");
  sprintf(inKB,"%-12s","In KB");
  sprintf(outKB,"%-12s","Out KB");
  sprintf(inPacket,"%-17s","In Packets");
  sprintf(outPacket,"%-14s","Out Packets");
  file << cont;
  file << cliente;
  file << remoteHost;
  file << app;
  file << service;
  file << inKB;
  file << outKB;
  file << endl;
        cont++;
    }    

    std::string str = getHostServicePort(flow);
    sprintf(localAdress,"%-23s",flow.srcAdressToStdString().c_str());
    sprintf(remoteAdress,"%-23s",flow.tgtAdressToStdString().c_str());
    sprintf(app,"%-15s",flow.getApplication().c_str());
    sprintf(service,"%-9s",str.c_str());
    sprintf(inKB,"%-8.5f\t\t",(flow.getInBytes()/1024.0));
    sprintf(outKB,"%8.5f",(flow.getOutBytes()/1024.0));

    file << localAdress;
    file << remoteAdress;
    file << app;
    file << service;
    //file << inKB;
    //file << outKB;
    file << endl;

        //file << "#Horas_trasncurridas:" << HORA*countHoras << endl;

        //exit(0);
        //filePacktCount << "total" << processed_packets << endl;
        //system("gnuplot packetCount.gp");
        //system("eog packetCount.png ");
        countHoras++;
      //      }//fin if
}

int cont1   = 0;
/**
 * Core::writeFlowToFile
 * Vacia el flujo a un archivo, separado por espacios.
 * Es esencialmente una modificacion de Core::dump
 *
 * @author Carlos Paparoni
 */
void Core::writeFlowToFile(Flow flow)
{
    //Determinamos el tiempo actual para el inicio de la traza:
    // Current date/time based on current system

    time_t startTrace   = time(0);

    // Convert now to tm struct for local timezone
    tm* startTracetm    = localtime(&startTrace);
    //Fin tiempo
    ftime(&tmb2);

    if (cont1==0)
    {
        bandera = true;
        file << "#Start time for trace (local system time): " << asctime(startTracetm) << endl;

        //Se imprime el encabezado al archivo:
        file << "# ";           //Indices:
        file << "src_ip ";      //0
        file << "src_port ";    //1
        file << "dst_ip ";      //2
        file << "dst_port ";    //3
        file << "app ";         //4
        file << "srvc_port ";   //5
        file << "in_kb ";       //6
        file << "out_kb ";      //7
        file << "in_pckt ";     //8
        file << "out_pckt ";    //9
        file << "remote_host "; //10
        file << "t (seg)";      //11
        file << endl;
        cont1++;

    }   //endif    
    
    //Ahora se formatea cada dato recibido segun lo anterior:
//    file << count                           << " "; //Indices:
    file << flow.getSourceAddress()         << " "; //0
    file << flow.srcPortToStdString()       << " "; //1
    file << flow.getDestinationAddress()    << " "; //2
    file << flow.tgtPortToStdString()       << " "; //3
    file << flow.getApplication()           << " "; //4
    file << getHostServicePort(flow)        << " "; //5
    file << (flow.getInBytes()  / 1024.0)   << " "; //6
    file << (flow.getOutBytes() / 1024.0)   << " "; //7
    file << flow.getInpacketsCount()        << " "; //8
    file << flow.getOutpacketsCount()       << " "; //9
    file << flow.getRtmHostService()        << " "; //10
    file << tmb2.time - tmb.time;                   //11
    file << endl;

    if((tmb2.time - tmb.time) >= 20 && bandera){
        bandera = false;
        file << "# Ya han pasado 20 seg." << endl;
    }
    count++;

}   //End Flow::writeFlowToFile
int Core::searchForFlow(vector<Flow> flows,Flow & flow)
{
    for( size_t i = 0 ; i < flows.size() ; i++ )
        if( flows[i] == flow ){
        flow = flows[i];
        return int(i);
    }

    return NOT_FOUND;
}

void Core::setIp(std::string ip)
{
    this->ip = ip;
}

int Core::packetFrecCount(PacketHeader packet){
    int dataSize = packet.getPacketSize();

    if(this->frecuencia.find(dataSize) == this->frecuencia.end()){
            this->frecuencia[dataSize] = 1;
        }else{
            int count = this->frecuencia.find(dataSize)->second;
            count++;
            this->frecuencia[dataSize] = count;
        }
}//end of packetFrecCount

void Core::printFrenc(){

    filePktCountFrec << 0 << "\t" << "Frecuency" <<endl;

    while (! frecuencia.empty()){
//        cout << "Pctk Size " << frecuencia.begin()->first <<//
//                "\tCount " << frecuencia.begin()->second << endl;

        filePktCountFrec << frecuencia.begin()->first << "\t"//
                         << frecuencia.begin()->second << endl;

        frecuencia.erase(frecuencia.begin());
    }
}

void Core::printHistogram(){

    float prct1500 = pckt1500;
    float prct1460 = pckt1460;
    float prct1420 = pckt1420;
    float prct1064 = pckt1064;
    float prct428 = pckt428;
    float prct72 = pckt72;
    float prct64 = pckt64;
    float prct50 = pckt50;
    float prct52 = pckt52;
    float prct43 = pckt43;

    filePacktCount << "0\t" << "%"  << endl;
    filePacktCount << "43\t" << (prct43/processed_packets)*100 << endl;
    filePacktCount << "50\t" << (prct50/processed_packets)*100 << endl;
    filePacktCount << "52\t" << (prct52/processed_packets)*100 << endl;
    filePacktCount << "64\t" << (prct64/processed_packets)*100 << endl ;
    filePacktCount << "72\t" << (prct72/processed_packets)*100 << endl;
    filePacktCount << "428\t"  << (prct428/processed_packets)*100 << endl;
    filePacktCount << "1064\t" << (prct1064/processed_packets)*100 << endl;
    filePacktCount << "1420\t" << (prct1420/processed_packets)*100 << endl;
    filePacktCount << "1460\t" << (prct1460/processed_packets)*100 << endl;
    filePacktCount << "1500\t" <<  (prct1500/processed_packets)*100 << endl;
}

void Core::printTrafficVol(){
    binCount++;
    double TotalTraffic = (in_kbytes + out_kbytes)/1024;
    fileTrafficVol << binCount << "\t" << TotalTraffic << endl;

}

std::string Core::getHostServicePort(Flow flow){

    if(flow.getSourcePort() < 1024){
        return flow.srcPortToStdString();
    }
    else if ( flow.getDestinationPort() < 1024  ){
        return flow.tgtPortToStdString();
    }

    if(flow.getSourcePort() < flow.getDestinationPort()){
        return flow.srcPortToStdString();

    }
    else if ( flow.getDestinationPort() < flow.getSourcePort() ){
        return flow.tgtPortToStdString();
    }
    //TODO
    return flow.tgtPortToStdString();
}

void Core::countPacket(PacketHeader packet){

   int size = packet.getPacketSize();

   if(size == 1500  ){
        pckt1500++;
   }
   else if( size == 1460 ){
       pckt1460++;
   }
   else if( size == 1420 ){
       pckt1420++;
   }
   else if( size == 1064 ){
       pckt1064++;
   }
   else if( size == 428 ){
       pckt428++;
   }
   else if( size == 100 ){
       pckt100++;
   }
   else if( size == 80 ){
       pckt80++;
   }
   else if( size == 72 ){
       pckt72++;
   }
   else if( size == 64){
       pckt64++;
   }
   else if( size == 52 ){
       pckt52++;
   }
   else if( size == 50 ){
       pckt50++;
   }
   else if( size == 43 ){
       pckt43++;
   }

}

int Core::initPortServices( map<int,string> & puertos_mapeo){

    size_t foundApp, foundAppAndPort;
    int portNumber;
    string appAndPort, line, aux, auxAux, app, port;
    ifstream file(INFO_FILE);

    if(!file)
        return -1;
    while( !file.eof() ){
        getline(file,line);
        aux = line;

        // la descripcion buscada cumple con esta condicion
        if (  (aux.substr(0,1) != "#" ) && (aux.substr(0,1) != "\0" )  ){
            foundAppAndPort = line.find("/");
            if (foundAppAndPort != string::npos)
                appAndPort = aux.substr(0,foundAppAndPort);

            //validando que sea tcp o udp
            if( string::npos != line.find("ddp") ) {
                continue;
            }

            foundApp = line.find("\t");
            if (foundApp != string::npos)
                app= aux.substr(0,foundApp);

            for (int k = appAndPort.length() ; k >= 0  ; k-- ){
                auxAux = appAndPort.substr(k,1);
                if(auxAux == "\t" ){
                    auxAux = appAndPort.substr(k+1,1);
                    //Look Ahead
                    if( auxAux == "\t" ){
                        port = appAndPort.substr(k+2,foundAppAndPort);
                        portNumber = atoi(port.c_str());
                    }else {
                        port = appAndPort.substr(k+1,foundAppAndPort);
                        portNumber = atoi(port.c_str());
                    }
                }
            }
            puertos_mapeo[portNumber] = app;
        }
    }//end while
}//end of iniServices
