
#include "stdoutputreader.h"

StdOutputReader::StdOutputReader()
{
    found   = 0;
    found2  = 0;
    command = "ifconfig ";
}

std::string StdOutputReader::read(std::string cmd)
{
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

std::string StdOutputReader::getLines()
{
    FILE * fichero = NULL;

    /* Variable auxiliar para leer el fichero */
    char aux[1000];

    /*
     *  Se abre el COMANDO
     * que se ha podido abrir correctamente
     */
    fichero = popen("netstat -ntp", "r");

    if (fichero == NULL)
    {
        perror("No se puede abrir /bin/ls");
        exit(-1);
    }

    /*
     *  Se lee la primera linea y se hace un bucle, hasta fin de fichero,
     * para ir sacando por pantalla los resultados.
     */
    fgets(aux, 1000, fichero);

    while (!feof(fichero))
    {
        cout << aux << endl;

        fgets(aux, 1000, fichero);
    }

    /* Se cierra el fichero */
    pclose(fichero);
}

std::string StdOutputReader::ifconfigUbuntu(string interface)
{
    std::string newCommand = command + interface;
    std::string outPut;

    outPut = read(newCommand);
    found  = outPut.find("inet:");
    found2 = outPut.find("Difus");
    ip     = outPut.substr(found + strlen("inet:"), (found2 - (found + strlen("Difus") + 2)));

    return ip;
}

std::string StdOutputReader::netstat_tnpUbuntu()
{
    std::string command    = "netstat";
    std::string argument   = " -ntp";
    std::string newCommand = command + argument;
    std::string outPut;
}

/*main (){

    StdOutputReader reader;
    //cout << reader.read("ifconfig wlan0");
    reader.getLine(1);
}
*/


//~ Formatted by Jindent --- http://www.jindent.com
