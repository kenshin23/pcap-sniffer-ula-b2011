/*
UNIX Daemon Server 
Rafael Solorzano 16/01/2012

para compilar:	gcc -o demonio demonio.c
para correr:		./demonio
test daemon:	ps -ef|grep demonio (or ps -aux on BSD systems)
test log:	tail -f /tmp/sniffer.log
test signal:	kill -HUP `cat /tmp/sniffer.lock`
terminar:	kill `cat /tmp/sniffer.lock`
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string>
#include <fstream>
#include <signal.h>
#include <fstream>
#include <map>
#include "core.h"
#include "flow.h"
#include "packetheader.h"
#include "pcapwrapper.h"
using namespace std;

#define INFO_FILE "/etc/services"
#define RUNNING_DIR	"/tmp"
#define LOCK_FILE	"sniffer.lock"
#define LOG_FILE	"sniffer.log"

using namespace std;

void log_message(char *filename,char *message){
FILE *logfile;
	logfile=fopen(filename,"a");
	if(!logfile) return;
	fprintf(logfile,"%s\n",message);
	fclose(logfile);
}

void signal_handler(int sig){
	switch(sig) {		
	break;	

	case SIGHUP:
		log_message(LOG_FILE,"hangup signal catched");
		
		break;
	case SIGTERM:
		log_message(LOG_FILE,"terminate signal catched");
                exit(0);
                //isRunning = false;
		break;
	}
}

void findAppByPort(std::string line, std::string ip, map<int,string> &mapeo){
    cout << "ip" << ip << endl;
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
    cout << posIniIpLocal << " ";
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
        cout << 11 << endl;
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
        cout << 33 << endl;
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
    cout << "Modifico la referenciaaaaaa" <<endl;
    cout << localPort.c_str() << " " <<  localPort << " " << lclPort << endl;
    //aqui hago el mapeo
    mapeo[lclPort]=app;

    cout << "#" << mapeo.find(lclPort)->second << "#" << endl;
    cout << "#" << mapeo.find(lclPort)->first << "#" << endl;
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
        cout << line;

        findAppByPort(line,ip,mapeo);


        fgets (aux, 1000, fichero);
        line.clear();
    }

    /* Se cierra el fichero */
    pclose (fichero);
}

void lookForInitiatedFlows( std::string line, std::string ip, map<int,string> &mapeo,Core *core ){

    cout << "ip" << ip << endl;
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
            cout << k << endl;
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
        cout << line;
        lookForInitiatedFlows(line, ip , mapeo, &core);
        fgets (aux, 1000, fichero);
        line.clear();
    }

    /* Se cierra el fichero */
    pclose (fichero);
}

struct thread_data {
    string ip; // name of symbol
    Core core; // Tipos de token: IF, ELSE, OPLOG, OPASIG, NUMERO, IDENTIFICADOR
    PcapWrapper pcap;//tipo de dato de las variables (INT,FLOAT,DOUBLE, entre otros)
    map<int,string> mapeo;
} ;

void *timerEvent(void *ptr){
    cout << "entro al hilo" <<endl;

    struct thread_data * my_data;
    my_data = (struct thread_data *) ptr;

    while (true){
        //sleep(1);
        cout << "query" << endl;
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
            string app_name = "unknow";

            if( my_data->mapeo.find(flow.getSourcePort()) != my_data->mapeo.end() ){
                app_name = my_data->mapeo.find(flow.getSourcePort())->second;
            }

//            while(! my_data->mapeo.empty()){
//                cout << "EL MAPEOOOO del hilo " << endl;
//                cout << my_data->mapeo.begin()->first << "=>";
//                cout << my_data->mapeo.begin()->second << endl;
//                my_data->mapeo.erase(my_data->mapeo.begin());
//
//            }

            my_data->core.setAppName(i,app_name);
        }

    }

}//end timerEvent


int main(void) {
        
        /* Our process ID and Session ID */
        pid_t pid, sid;
        
        /* Fork off the parent process */
        pid = fork();
        if (pid < 0) {
                exit(EXIT_FAILURE);
        }
        /* If we got a good PID, then
           we can exit the parent process. */
        if (pid > 0) {
                exit(EXIT_SUCCESS);
        }

        /* Change the file mode mask */
        umask(0);
                
         /* Change the current working directory */
 //       if ((chdir(RUNNING_DIR)) < 0) {
                /* Log the failure */
   //             exit(EXIT_FAILURE);
    //    }
                
        /* Open any logs here */        
        int lfp;
        char str[10];
        /* guarda pid en el lockfile */
        lfp=open(LOCK_FILE,O_RDWR|O_CREAT,0640);
	if (lfp<0) exit(1); 
	if (lockf(lfp,F_TLOCK,0)<0) exit(0); 
	sprintf(str,"%d\n",getpid());
	write(lfp,str,strlen(str)); 
	        
        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
                /* Log the failure */
                exit(EXIT_FAILURE);
        }       
        /* Close out the standard file descriptors 
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
 
 	/*Asignando signals */
        
        signal(SIGHUP,signal_handler);
        signal(SIGTERM,signal_handler); 
        
        /* Daemon-specific initialization goes here */
    thread_data dato;
    std::string ip = dato.pcap.getHostIpAddres();
    dato.core.setIp(ip);
    dato.ip = ip;
    pthread_t thread;

    //ver los flujos iniciados




        /* The Big Loop */

           /* Do some task here ... */
        //captureFile(mapeo);
        int i = pthread_create(&thread, NULL, timerEvent , (void *)&dato);
        netStatQueryForInitFlows(dato.ip,dato.mapeo,dato.core);
        dato.pcap.startCapture();
//        while (1){
//
//
//
//
//          }
          pthread_exit(NULL);
            //flush();

}
