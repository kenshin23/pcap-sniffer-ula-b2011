#include <iostream>
#include <getopt.h>
#include <sys/timeb.h>
#include <limits>
#include <pcap.h>

#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/timeb.h>
#include <arpa/inet.h>
#include <netinet/ether.h>
#include <fstream>
#include <map>
#include "core.h"
#include "flow.h"
#include "packetheader.h"
#include "pcapwrapper.h"
#include "stdoutputreader.h"
#define HORA 20
#define BIN 10  //300

#define DAY 86400
#define HOUR 3600
#define MINUTE 60

using namespace std;

//Esto es sencillamente despreciable, peeeero...
//static struct timeb t_start, t_current, t_stop;
static struct timeb t_stop;

void findAppByPort(std::string line, std::string ip, map<int,string> &mapeo){
    //cout << "ip" << ip << endl;
    std::string aux;
    size_t posIniIpLocal;
    size_t posEndIpLocal;
    std::string localPort;
    std::string remotePort;
    std::string app;
    int rmtPort = 0;
    std::string ipRemote;
    size_t k=0;
    size_t j=0;
    size_t posIniApp;

    int countPortLocal = 0;
    int countRemotePort= 0;
    if(line.substr(0,3) != "tcp"){
        return;
    }

    posIniIpLocal = line.find(ip,0);
  //cout << posIniIpLocal << " ";
    if( posIniIpLocal != string::npos ){
        posEndIpLocal = posIniIpLocal + ip.size() + 1 ;
        k = posEndIpLocal;
        aux = line.substr(k,1);
        while( aux != " "){
            k++;
            aux = line.substr(k,1);
            countPortLocal++;
        }
        //tengo el portLocal
        localPort = line.substr(posEndIpLocal,countPortLocal);

        aux = line.substr(k,1);
        while( aux == " "){
            k++;
            aux = line.substr(k,1);
        }
        j=k;
        aux = line.substr(j,1);
        while(aux != ":"){
            j++;
            aux = line.substr(j,1);
        }
        j = line.substr(k,20).find(':');
        /*IP remota*/
        ipRemote = line.substr(k,j);
        /*************/
        k = k+j + 1 ;
        j = k+j + 1;
        aux = line.substr(0,1);
        while(  aux != " "  ){
            j++;
            countRemotePort++;
            aux = line.substr(j,1);
        }

        remotePort = line.substr(k,countRemotePort);
        rmtPort =atoi(remotePort.c_str());

        posIniApp = line.find("/") +1 ;
        int appCount=0;
        k=posIniApp;
        aux = line.substr(k,1);
        while( aux != " "  && line.length()-1 != k){
            k++;
            appCount++;
            aux = line.substr(k,1);
        }
        app = line.substr(posIniApp,appCount );

    }

    int lclPort= atoi(localPort.c_str()) ;
    //aqui hago el mapeo
    mapeo[lclPort]=app;
}//fin findAppByport


void netStatQueryFindApp(std::string ip, map<int,string> & mapeo){

    FILE *fichero=NULL;

    /* Variable auxiliar para leer el fichero */
    char aux[185];
    std::string line;
    /* Se abre el COMANDO
        * que se ha podido abrir correctamente */
    fichero = popen ("netstat -ntp |grep ESTABLECIDO", "r");
    if (fichero == NULL)
    {
        perror ("No se puede abrir /bin/ls");
        exit (-1);
    }

    fgets (aux, 1000, fichero);
    while (!feof (fichero))
    {
        //cout << aux <<endl;
        line.append(aux);
        /**tengo la linea**/
        findAppByPort(line,ip,mapeo);


        fgets (aux, 1000, fichero);
        line.clear();
    }

    /* Se cierra el fichero */
    pclose (fichero);
}

void lookForInitiatedFlows( std::string line, std::string ip, map<int,string> &mapeo,Core *core ){

   // cout << "ip" << ip << endl;
    std::string aux;
    size_t posIniIpLocal;
    size_t posEndIpLocal;
    std::string localPort;
    std::string remotePort;
    std::string app;
    int rmtPort = 0;
    std::string ipRemote;
    size_t k=0;
    size_t j=0;
    size_t posIniApp;

    int countPortLocal = 0;
    int countRemotePort= 0;
    if(line.substr(0,3) != "tcp"){
        return;
    }

    posIniIpLocal = line.find(ip,0);
    if( posIniIpLocal != string::npos ){
        posEndIpLocal = posIniIpLocal + ip.size() + 1 ;
        k = posEndIpLocal;
        aux = line.substr(k,1);
        while( aux != " "){
            k++;
            aux = line.substr(k,1);
            countPortLocal++;
        }

        //tengo el portLocal
        localPort = line.substr(posEndIpLocal,countPortLocal);

        aux = line.substr(k,1);
        while( aux == " "){
            k++;
            aux = line.substr(k,1);
        }
        j=k;
        aux = line.substr(j,1);
        while(aux != ":"){
            j++;
            aux = line.substr(j,1);
        }
        j = line.substr(k,20).find(':');
        /*IP remota*/
        ipRemote = line.substr(k,j);
        /*************/
        k = k+j + 1 ;
        j = k+j + 1;
        aux = line.substr(0,1);
        while(  aux != " "  ){
            j++;
            countRemotePort++;
            aux = line.substr(j,1);
        }

        remotePort = line.substr(k,countRemotePort);
        rmtPort =atoi(remotePort.c_str());
        posIniApp = line.find("/") +1 ;
        int appCount=0;
        k=posIniApp;

        aux = line.substr(k,1);

        while( aux != " " && k != line.length()-2) {
            k++;
            appCount++;
            aux = line.substr(k,1);
        }
        app = line.substr(posIniApp,appCount );

    }
    int lclPort= atoi(localPort.c_str()) ;

    //aqui hago el mapeo
   mapeo[lclPort]=app;

    struct in_addr src;
    inet_aton(ip.c_str(),&src);
    u_int32_t saddr   = src.s_addr;
    u_int16_t localPort_  = htons(atoi(localPort.c_str()));

    struct in_addr tgt;
    inet_aton(ipRemote.c_str(),&tgt);
    u_int32_t daddr   = tgt.s_addr;
    u_int16_t rmtPort_    = htons(rmtPort);
    std::string servicio = "vacio";


    Flow flow(saddr,localPort_,daddr,rmtPort_,app);
    //flow.setApplication("flujos q ya estaban");
    core->getInitiatedFlows().push_back(flow);

}

void netStatQueryForInitFlows(std::string ip, map<int,string> & mapeo,Core &core){


    FILE *fichero=NULL;

    /* Variable auxiliar para leer el fichero */
    char aux[185];
    std::string line;
    /* Se abre el COMANDO
        * que se ha podido abrir correctamente */
    fichero = popen ("netstat -ntp |grep ESTABLECIDO", "r");
    if (fichero == NULL)
    {
        perror ("No se puede abrir /bin/ls");
        exit (-1);
    }

    fgets (aux, 1000, fichero);
    while (!feof (fichero))
    {
        //cout << aux <<endl;
        line.append(aux);
        /**tengo la linea**/
        lookForInitiatedFlows(line, ip , mapeo, &core);
        fgets (aux, 1000, fichero);
        line.clear();
    }

    /* Se cierra el fichero */
    pclose (fichero);
}

struct thread_data {
    string ip; //
    Core core; //
    PcapWrapper pcap;
    map<int,string> mapeo;
} ;

void *run(void *ptr){
    cout << "entro al hilo" <<endl;
    struct timeb t_start, t_current;
    
    ftime(&t_start);
    struct thread_data * my_data;
    my_data = (struct thread_data *) ptr;
    int binCount=1;
    bool is_running = true;
    
    while (is_running){
        ftime(&t_current);

        if( difftime( t_current.time, t_start.time ) >= t_stop.time ) {
            is_running = false;
        }
        ////cout << "query" << endl;
        netStatQueryFindApp(my_data->ip,my_data->mapeo);
        while(not my_data->pcap.getCapturedPackets().empty()) {
            PacketHeader packet = my_data->pcap.getCapturedPackets().front();
            my_data->pcap.getCapturedPackets().pop();
            my_data->core.processPacket(packet);
        }

        std::vector<Flow> initiated = my_data->core.getInitiatedFlows();

        for (size_t i = 0; i < initiated.size() ; i++) {
            Flow flow = initiated[i];

            //Buscar el nombre de la aplicacion
            string app_name = "unknown";

            if( my_data->mapeo.find(flow.getSourcePort()) != my_data->mapeo.end() ){
                app_name = my_data->mapeo.find(flow.getSourcePort())->second;
            }
            my_data->core.setAppName(i,app_name);
        } //endfor;
        ftime(&t_current);
    } //endwhile;
    //terminar de procesar lo que queda en la cola de paquetes
    
    while(not my_data->pcap.getCapturedPackets().empty()) {
            PacketHeader packet = my_data->pcap.getCapturedPackets().front();
            my_data->pcap.getCapturedPackets().pop();
            my_data->core.processPacket(packet);
            cout << "Procesando paquetes restantes..." << endl;
        }
    exit(0);
    //my_data->pcap.stopCapture();
} //end timerEvent

//--------------------------------------------------------------------------------------------------
/**
 * help_message
 * Muestra los comandos posibles para el demonio, y sale del programa.
 */
void help_message()
{
    std::cout << "Modo de empleo: ejecutable [opciones]"                            << std::endl;
    std::cout << "Opciones:"                                                        << std::endl;
    
    std::cout << "\t-c --count\tEspecifica el numero de paquetes a obtener,"        << std::endl;
    std::cout << "\t\t\ttras lo cual se sale del programa."                         << std::endl;

    std::cout << "\t-D --td\t\tEspecifica el numero de dias que desea ejecutar"     << std::endl;
    std::cout << "\t\t\tel demonio."                                                << std::endl;


    std::cout << "\t-H --th\t\tEspecifica el numero de horas que desea ejecutar"    << std::endl;
    std::cout << "\t\t\tel demonio."                                                << std::endl;
    
    std::cout << "\t-i --interface\tEspecifica el nombre del dispositivo para"      << std::endl;
    std::cout << "\t\t\trealizar capturas. Por ejemplo, eth0, o wlan0."             << std::endl;
    
    std::cout << "\t-f --filter\tEspecifica los filtros para los paquetes."         << std::endl;
    std::cout << "\t\t\tPor ejemplo, tcp, udp, etc."                                << std::endl;
    
    std::cout << "\t   --help\tMuestra este mensaje de ayuda."                      << std::endl;


    std::cout << "\t-M --tm\t\tEspecifica el numero de minutos que desea ejecutar"  << std::endl;
    std::cout << "\t\t\tel demonio."                                                << std::endl;


    std::cout << "\t-S --ts\t\tEspecifica el numero de segundos que desea ejecutar" << std::endl;
    std::cout << "\t\t\tel demonio."                                                << std::endl;
    
    std::cout << "\t   --version\tMuestra la version del programa."                 << std::endl;
    
    std::cout << "\t-w --write\tEspecifica a que archivo escribir las trazas."      << std::endl;
    
    std::cout << "Reporte bugs (errores) a: trollface@ula.ve"                          << std::endl;
    std::cout << "WireSardina home page: ";
    std::cout << "<http://www.gnu.org/software/wiresardina/>"               << std::endl;
    std::cout << "Ayuda general usando software GNU: <http://www.gnu.org/gethelp/>" << std::endl;
    
    exit(0);
}

void version()
{
    std::cout << "WireSardina v0.1"                             << std::endl;
    std::cout << "Copyright (C) 2012 Universidad de Los Andes." << std::endl;
    std::cout << "License GPLv3+: GNU GPL version 3 or later ";
    std::cout << "<http://gnu.org/licenses/gpl.html>"           << std::endl;
    std::cout << "This is free software: you are free to change and redistribute it." << std::endl;
    std::cout << "There is NO WARRANTY, to the extent permitted by law." << std::endl;
 
    exit(0);
}
//--------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
    int dspl_hlp_flag(0), dspl_ver_flag(0);
    int dias(0), horas(0), minutos(0), segundos(0);
    bool end_time   = false;
        
    static struct option long_options[] =
        {     
            /* Estas opciones activan una bandera. */
            {"help",        no_argument,    &dspl_hlp_flag, 1},
            {"version",     no_argument,    &dspl_ver_flag, 1},
            /*  Estas opciones no activan una bandera.
                Las distinguimos por sus indices. */
            
            {"count",       required_argument, 0, 'c'},    
            {"td",          required_argument, 0, 'D'},
            {"filter",      required_argument, 0, 'f'},
            {"th",          required_argument, 0, 'H'},
            {"interface",   required_argument, 0, 'i'},
            {"tm",          required_argument, 0, 'M'},
            {"ts",          required_argument, 0, 'S'},
            {"write",       required_argument, 0, 'w'},
            {0, 0, 0, 0}    //Necesario para finalizar el conjunto de opciones.
        };  //end static struct option long_options
    
    //Inicializar los argumentos a sus respectivas opciones:
    int c;
    while(1)
    {
        /* getopt_long guarda el indice de opciones aqui. */
        int option_index = 0;
     
        c = getopt_long (argc, argv, "c:D:H:i:M:S:w",
                         long_options, &option_index);
     
        /* Detecta el fin de las opciones. */
        if (c == -1)
            break;
     
        switch (c){
            case 0:
                /* Si esta opcion activo una bandera, no hacer mas nada.. */
                if (long_options[option_index].flag != 0)
                    break;
                printf ("option %s", long_options[option_index].name);
                if (optarg)
                    printf (" with arg %s", optarg);
                printf ("\n");
                break;
            case 'c':
                //TODO:
                break;
            case 'D':
                dias            = atoi(optarg);
                end_time        = true;
                break;
            case 'f':
                //TODO:
                break;
            case 'H':
                horas           = atoi(optarg);
                end_time        = true;
                break;
            case 'i':
                //TODO:
                break;
            case 'M':
                minutos         = atoi(optarg);
                end_time        = true;
                break;
            case 'S':
                segundos        = atoi(optarg);
                end_time        = true;
                break;
            case 'w':
                //Declaracion de la variable filename, usada en core.h
                //TODO:
                break;
            case '?':
                /* getopt_long ya imprimio un mensaje de error. */
                break;
            default:
                abort ();
        }
    } //endwhile;
    
    if(dspl_hlp_flag){  help_message(); }
    if(dspl_ver_flag){  version();      }
    
    //Se inicializa el tiempo al maximo long posible para que nunca termine:
    t_stop.time = std::numeric_limits<long>::max();
    
    if( end_time )
    {
        t_stop.time = (dias * DAY) + (horas * HOUR) + (minutos * MINUTE) + segundos;
    }   
    //Core core;
    //  PcapWrapper pcap;
    //    std::string ip = pcap.getHostIpAddres();
    //core.setIp(ip);
    //map<int,string> mapeo;
    thread_data dato;
    string interface;
    cout << "dame la interface: " <<endl;
    cin >> interface;
    dato.pcap.setInterfaceName(interface);
    std::string ip = dato.pcap.getHostIpAddres();
    dato.core.setIp(ip);
    dato.ip = ip;


    //ver los flujos iniciados
    pthread_t thread;
    int i = pthread_create(&thread, NULL, run , (void *)&dato);
    netStatQueryForInitFlows(dato.ip,dato.mapeo,dato.core);
    dato.pcap.startCapture();

    pthread_exit(NULL);
    return 0;
}
