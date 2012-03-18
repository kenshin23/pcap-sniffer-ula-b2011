#ifndef GRAPH_H
#define GRAPH_H

/**
 * graph.h
 *
 * Examina la traza y grafica los resultados de la misma
 * en diversos aspectos, dependiendo de los metodos utilizados
 *
 * @author  Carlos Paparoni
 * @date    12/03/2012
 */

#define DEBUG false

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <stdlib.h>

#define INFO_FILE "/etc/services"
 
using namespace std;
using namespace boost;

string  capture_file_path   = "../capture.txt";

int     portsToService( map<int,string> &mapeo);

bool    mypredicate (int i, int j);

/**
 * graphAppKB
 *
 * Examina la traza y grafica los resultados de la misma 
 * en terminos del consumo de la red por aplicacion.
 */
int     graphAppKB();

/**
 * graphPortsKB
 *
 * Examina la traza y grafica los resultados de la misma 
 * en terminos del consumo de la red por cada servicio.
 */
int     graphPortsKB();

/**
 * graphPortsNotMappedKB
 *
 * Examina la traza y grafica los resultados de la misma 
 * en terminos del consumo de la red por cada puerto.
 */
int     graphPortsNotMappedKB();

/**
 * graphFrequenceKB
 *
 * Examina la traza y grafica los resultados de la misma 
 * en terminos de la frecuencia de los KB por flujo.
 */
int     graphFrequenceKB();

/**
 * graphFrequenceKB
 *
 * Examina la traza y grafica los resultados de la misma 
 * en terminos de la frecuencia de los KB por flujo.
 */
int     graphFrequenceNFlows();

/**
 * graphFrequenceKB
 *
 * Ejecuta las rutinas de graficacion anteriores.
 */
int     graphAll();

#endif // GRAPH_H

