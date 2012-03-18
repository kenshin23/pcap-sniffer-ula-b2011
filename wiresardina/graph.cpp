/**
 * graph.cpp
 *
 * Examina la traza y grafica los resultados de la misma
 * en diversos aspectos, dependiendo de los metodos utilizados
 *
 * @author  Carlos Paparoni
 * @date    12/03/2012
 */

#include "graph.h"

int portsToService( map<int,string> &mapeo){
	size_t foundApp;
	size_t foundPort;
	size_t foundAppAndPort;
	string app;
	string port;
	int portNumber;
	string appAndPort;
	string line;
	string aux;
	string auxAux;
	
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
						
						} else {
						port = appAndPort.substr(k+1,foundAppAndPort);
						portNumber = atoi(port.c_str());
					} 
				} //endif
			} //end for
            mapeo[portNumber] = app;
		} //end if	
	} //end while
} //end portToService

bool mypredicate (int i, int j) {
    return (i==j);
}

int graphAppKB(){

    string              current_line;
    vector<string>      current_tokens;
    string              app_name;
    float               kb_total;              
    map<string, float>  results;
    map<string, float>::iterator it;
    float               aux = 0;
    
    ifstream input (capture_file_path.c_str(), ifstream::in );
    
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(" ", "", boost::keep_empty_tokens);
    
    //Se extraen los datos del archivo de traza:
    if ( input.is_open() ) {
        while ( input.good() ) {
            aux         = 0.0;
            kb_total    = 0.0;
            app_name.clear();
            current_line.clear();            
            current_tokens.clear();
            getline (input, current_line);
    
            //Ignoramos las lineas comentadas:
            if ( not boost::starts_with(current_line, "#") ) {
                //Guardar los tokens obtenidos en el vector:
                tokenizer tokens(current_line, sep);
                current_tokens.assign( tokens.begin(), tokens.end() );
                
                if (current_tokens.size() == 0) continue;
                
                //Impresion de resultados -- modo debug:
                if(DEBUG) {
                    cout << "Contenido del vector tokens:";
                    for (unsigned int i=0; i < current_tokens.size(); i++) {
                        cout << " " << current_tokens[i];
                    }
                    cout << endl;
                }

                /**
                 * Recordar que todos son mapeados como strings:
                 * current_tokens[0]  == src_ip
                 * current_tokens[1]  == src_port
                 * current_tokens[2]  == dst_ip
                 * current_tokens[3]  == dst_port
                 * current_tokens[4]  == app
                 * current_tokens[5]  == srvc_port
                 * current_tokens[6]  == in_kb
                 * current_tokens[7]  == out_kb
                 * current_tokens[8]  == in_pckt
                 * current_tokens[9]  == out_pckt
                 * current_tokens[10] == remote_host
                 * current_tokens[11] == time (seconds)
                 *
                 * Dependiendo de lo que queramos mapear, kb_total sera:
                 * 1. kb_total = atof(current_tokens[5]) + atof(current_tokens[6]);
                 * 2. kb_total = atof(current_tokens[5]);
                 * 3. kb_total = atof(current_tokens[6]);
                 */
                 
                if(current_tokens[4] != "") {
                    app_name    = current_tokens[4];
                }
                else {
                    app_name    = "unknown";
                }
                kb_total    = atof(current_tokens[5].c_str()) + atof(current_tokens[6].c_str());
                
                //Ahora mapeamos el resultado de la traza 
                //Busco e inserto/actualizo la aplicacion:
                it = results.find( app_name );
                if( it != results.end() ) {
                    //Actualizar la suma ya encontrada: 
                    aux     = kb_total + it->second;
                    results.erase(it);
                }
                else {
                    //Insertar la suma:
                    aux     = kb_total;
                }
                results.insert( pair<string, float>(app_name, aux) );
            } //endif; 
        } //endwhile;
        input.close();
    }
    else { 
        cout << "No se pudo abrir el archivo 'capture.txt'" << endl;
        return -1;
    }
    
    //Guardamos el resultado en el archivo que hara la grafica:    
    ofstream output ("graphdata_app_kb.dat", ofstream::out );
    if (output.is_open()) {
        output << "# app_name kb_total" << endl;
        for(it = results.begin(); it != results.end(); ++it){
            output << it->first << " " << it->second;
            output << endl;
        }
    }
    else { 
        cout << "No se pudo abrir el archivo 'graphdata_app_kb.txt'" << endl;
        return -1;
    }
    output.close();
    
    //Ahora se llama a GNUplot para graficar el archivo resultante.
    //Nombre de archivo que contiene los comandos para gnuplot: plotAppKb.gp
    system("gnuplot plotAppKb.gp");
    system("eog ImgAppKb.png");
    return 0;
} //end graphAppKB()

int graphPortsKB(){

    vector<string>      current_tokens;
	u_int16_t 			port_int;
	string              current_line, port_str;
	std::stringstream 	stream;
    float               kb_sum, kb_total, aux = 0;          
    map<string, float>  results;          
    map<string, float>  port_percent;
    map<string, float>::iterator result_it;
	map<int, string>  	port_map;		    //Para funcion de conversion de puerto -> aplicación  
    map<int, string>::iterator 	 port_it;
    
	//Se llena el mapa de puertos previamente:
	portsToService(port_map);
	
    ifstream input (capture_file_path.c_str(), ifstream::in );
    
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(" ", "", boost::keep_empty_tokens);
    
    //Se extraen los datos del archivo de traza:
    if ( input.is_open() ) {
        while ( input.good() ) {
            //Aunque no sea del todo necesario, resetear todas las variables
            //a su estado inicial:
            port_int    = 0;
            aux         = 0.0;
            kb_sum      = 0.0;
            port_str.clear();
            current_line.clear();            
            current_tokens.clear();
            getline (input, current_line);
    
            //Ignoramos las lineas comentadas:
            if ( not boost::starts_with(current_line, "#") ) {
                //Guardar los tokens obtenidos en el vector:
                tokenizer tokens(current_line, sep);
                current_tokens.assign( tokens.begin(), tokens.end() );
                
                if (current_tokens.size() == 0) continue;
                
                //Impresion de resultados -- modo debug:
                if(DEBUG) {
                    cout << "Contenido del vector tokens:";
                    for (unsigned int i=0; i < current_tokens.size(); i++) {
                        cout << " " << current_tokens[i];
                    }
                    cout << endl;
                }

                /**
                 * Recordar que todos son mapeados como strings:
                 * current_tokens[0]  == src_ip
                 * current_tokens[1]  == src_port
                 * current_tokens[2]  == dst_ip
                 * current_tokens[3]  == dst_port
                 * current_tokens[4]  == app
                 * current_tokens[5]  == srvc_port
                 * current_tokens[6]  == in_kb
                 * current_tokens[7]  == out_kb
                 * current_tokens[8]  == in_pckt
                 * current_tokens[9]  == out_pckt
                 * current_tokens[10] == remote_host
                 * current_tokens[11] == time (seconds)
                 *
                 * Dependiendo de lo que queramos mapear, kb_total sera:
                 * 1. kb_total = atof(current_tokens[6].c_str()) + atof(current_tokens[7].c_str());
                 * 2. kb_total = atof(current_tokens[6].c_str());
                 * 3. kb_total = atof(current_tokens[7].c_str());
                 */
				port_int 	    = atoi( current_tokens[5].c_str() );
                
				port_it 	    = port_map.find( port_int );
				if(port_it != port_map.end()) {
                    port_str    = port_it->second;
                } else {
                    port_str    = current_tokens[5];
                }
                kb_sum          = atof( current_tokens[6].c_str() ) + 
                                  atof( current_tokens[7].c_str() );
                
                //Impresion de resultados 2 -- modo debug:
				if(DEBUG) {
                    cout << "Salida del mapeo: ";
					cout << port_int << " " << kb_sum << " => ";
					cout << port_str << " " << kb_sum;
                    cout << endl;
                }
				
                //Ahora mapeamos el resultado de la traza 
                //Busco e inserto/actualizo al puerto resultante de la búsqueda anterior:
                result_it	= results.find( port_str );
                if( result_it != results.end() ) {
                    //Actualizar la suma ya encontrada: 
                    aux     = kb_sum + result_it->second;
                    results.erase(result_it);
                } else {
                    //Insertar la suma:
                    aux     = kb_sum;
                }
                kb_total   += kb_sum;
                results.insert( pair<string, float>(port_str, aux) );
            } //endif current_line not #; 
        } //endwhile input.good();
        input.close();
    } else { 
        cout << "No se pudo abrir el archivo 'capture.txt'" << endl;
        return -1;
    }
    
    //Ahora recalculamos los resultados para cada puerto pero en base al porcentaje de uso:
    for(  result_it  = results.begin() ;
          result_it != results.end()   ;
        ++result_it  )
    {
        port_str    = result_it->first;
        aux         = result_it->second;
        aux         = (aux / kb_total) * 100;
        port_percent.insert( pair<string, float>(port_str, aux) );
    }
    
    //Guardamos el resultado en el archivo que hara la grafica:    
    ofstream output ("graphdata_ports_kb.dat", ofstream::out );
    if (output.is_open()) {
        output << "# port kb_total" << endl;
        for(result_it = port_percent.begin(); result_it != port_percent.end(); ++result_it){
            output << result_it->first << " " << result_it->second;
            output << endl;
        }
    }
    else { 
        cout << "No se pudo abrir el archivo 'graphdata_ports_kb.txt'" << endl;
        return -1;
    }
    output.close();
    
    //Ahora se llama a GNUplot para graficar el archivo resultante.
    if(not DEBUG) {
        system("gnuplot plotPortsKb.gp");
        system("eog ImgPortsKb.png");
    }
	return 0;
} //end graphPortsKB()

int graphPortsNotMappedKB(){

    string              current_line;
    vector<string>      current_tokens;
	string              src_port;
    string              dst_port;
	std::stringstream 	stream;
    float               src_kb_total;              
    float               dst_kb_total;              
    map<string, float>  results;
    map<string, float>::iterator result_it;
	float               aux = 0;

    ifstream input (capture_file_path.c_str(), ifstream::in );
    
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(" :", "", boost::keep_empty_tokens);
    
    //Se extraen los datos del archivo de traza:
    if ( input.is_open() ) {
        while ( input.good() ) {
            aux         	= 0.0;
            src_kb_total	= 0.0;
            dst_kb_total	= 0.0;
            src_port.clear();
            dst_port.clear();
            current_line.clear();            
            current_tokens.clear();
            getline (input, current_line);
    
            //Ignoramos las lineas comentadas:
            if ( not boost::starts_with(current_line, "#") ) {
                //Guardar los tokens obtenidos en el vector:
                tokenizer tokens(current_line, sep);
                current_tokens.assign( tokens.begin(), tokens.end() );
                
                if (current_tokens.size() == 0) continue;
                
                //Impresion de resultados -- modo debug:
                if(DEBUG) {
                    cout << "Contenido del vector tokens:";
                    for (unsigned int i=0; i < current_tokens.size(); i++) {
                        cout << " " << current_tokens[i];
                    }
                    cout << endl;
                }

                /**
                 * Recordar que todos son mapeados como strings:
                 * current_tokens[0]  == src_ip
                 * current_tokens[1]  == src_port
                 * current_tokens[2]  == dst_ip
                 * current_tokens[3]  == dst_port
                 * current_tokens[4]  == app
                 * current_tokens[5]  == srvc_port
                 * current_tokens[6]  == in_kb
                 * current_tokens[7]  == out_kb
                 * current_tokens[8]  == in_pckt
                 * current_tokens[9]  == out_pckt
                 * current_tokens[10] == remote_host
                 * current_tokens[11] == time (seconds)
                 *
                 * Dependiendo de lo que queramos mapear, kb_total sera:
                 * 1. kb_total = atof(current_tokens[6].c_str()) + atof(current_tokens[7].c_str());
                 * 2. kb_total = atof(current_tokens[6].c_str());
                 * 3. kb_total = atof(current_tokens[7].c_str());
                 */

				src_port 		= current_tokens[3];
				dst_port 		= current_tokens[4];
				src_kb_total	= atof(current_tokens[5].c_str());
				dst_kb_total	= atof(current_tokens[6].c_str());
                
                //Impresion de resultados 2 -- modo debug:
				if(DEBUG) {
                    cout << "Salida del mapeo: ";
					cout << src_port << " " << dst_port << " " << src_kb_total << " " << dst_kb_total;
                    cout << endl;
                }
				
                //Ahora mapeamos el resultado de la traza 
                //Busco e inserto/actualizo al puerto fuente:
                result_it	= results.find( src_port );
                if( result_it != results.end() ) {
                    //Actualizar la suma ya encontrada: 
                    aux     = src_kb_total + result_it->second;
                    results.erase(result_it);
                }
                else {
                    //Insertar la suma:
                    aux     = src_kb_total;
                }
                results.insert( pair<string, float>(src_port, aux) );

				//Ahora busco e inserto/actualizo el puerto destino:				
                result_it 	= results.find( dst_port );
                if( result_it != results.end() ) {
                    //Actualizar la suma ya encontrada: 
                    aux     = dst_kb_total + result_it->second;
                    results.erase(result_it);
                }
                else {
                    //Insertar la suma:
                    aux     = dst_kb_total;
                }
                results.insert( pair<string, float>(dst_port, aux) );
            } //endif; 
        } //endwhile;
        input.close();
    }
    else { 
        cout << "No se pudo abrir el archivo 'capture.txt'" << endl;
        return -1;
    }
    
    //Guardamos el resultado en el archivo que hara la grafica:    
    ofstream output ("graphdata_portsnotmapped_kb.dat", ofstream::out );
    if (output.is_open()) {
        output << "# port kb_total" << endl;
        for(result_it = results.begin(); result_it != results.end(); ++result_it){
            output << result_it->first << " " << result_it->second;
            output << endl;
        }
    }
    else { 
        cout << "No se pudo abrir el archivo 'graphdata_portsnotmapped_kb.txt'" << endl;
        return -1;
    }
    output.close();
    
    //Ahora se llama a GNUplot para graficar el archivo resultante.
    system("gnuplot plotPortsKb.gp");
    system("eog ImgPortsKb.png");
	return 0;
} //end graphPortsNotMappedKB()

int graphFrequenceKB(){

    vector<string>      current_tokens;
	u_int16_t 			port_int;
	string              current_line, port_str;
	string              ip_str;
	std::stringstream 	stream;
    float               kb_total, aux = 0;
    int                 nro_ip, auxip = 0;
    map<string, int>  results;
    map<string, int>::iterator result_it;
    map<int, string>  	port_map;		    //Para funcion de conversion de puerto -> aplicación
    map<int, string>::iterator 	 port_it;

    multimap<int, string>  results_ip;
    multimap<int, string>::iterator 	 result_ip_it;

	//Se llena el mapa de puertos previamente:
	portsToService(port_map);
	
    ifstream input (capture_file_path.c_str(), ifstream::in );
    
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(" ", "", boost::keep_empty_tokens);
    
    //Se extraen los datos del archivo de traza:
    if ( input.is_open() ) {
        while ( input.good() ) {
            //Aunque no sea del todo necesario, resetear todas las variables
            //a su estado inicial:
            ip_str      = "";
            port_int    = 0;
            aux         = 0.0;
            kb_total    = 0.0;
            port_str.clear();
            current_line.clear();            
            current_tokens.clear();
            getline (input, current_line);

            //Ignoramos las lineas comentadas:
            if ( not boost::starts_with(current_line, "#") ) {
                //Guardar los tokens obtenidos en el vector:
                tokenizer tokens(current_line, sep);
                current_tokens.assign( tokens.begin(), tokens.end() );
                
                if (current_tokens.size() == 0) continue;
                
                //Impresion de resultados -- modo debug:
                if(DEBUG) {
                    cout << "Contenido del vector tokens:";
                    for (unsigned int i=0; i < current_tokens.size(); i++) {
                        //cout << " " << current_tokens[i];
                    }
                    //cout << endl;
                }

                /**
                 * Recordar que todos son mapeados como strings:
                 * current_tokens[0]  == src_ip
                 * current_tokens[1]  == src_port
                 * current_tokens[2]  == dst_ip
                 * current_tokens[3]  == dst_port
                 * current_tokens[4]  == app
                 * current_tokens[5]  == srvc_port
                 * current_tokens[6]  == in_kb
                 * current_tokens[7]  == out_kb
                 * current_tokens[8]  == in_pckt
                 * current_tokens[9]  == out_pckt
                 * current_tokens[10] == remote_host
                 * current_tokens[11] == time (seconds)
                 *
                 * Dependiendo de lo que queramos mapear, kb_total sera:
                 * 1. kb_total = atof(current_tokens[6].c_str()) + atof(current_tokens[7].c_str());
                 * 2. kb_total = atof(current_tokens[6].c_str());
                 * 3. kb_total = atof(current_tokens[7].c_str());
                 */
                ip_str 	        = current_tokens[2].c_str();
                
				//ip_str 	        = port_map.find( port_int );
				//if(port_it != port_map.end()) {
                //    port_str    = port_it->second;
                //} else {
                //    port_str    = "unknown";
                // }
                kb_total        = atof( current_tokens[6].c_str() ) + 
                                  atof( current_tokens[7].c_str() );
                
                //Impresion de resultados 2 -- modo debug:
				if(DEBUG) {
                    cout << "Salida del mapeo: ";
                                        //cout << ip_str << " " << kb_total << " => ";
					//cout << port_str << " " << kb_total;
                   // cout << endl;
                }
				
                //Ahora mapeamos el resultado de la traza 
                //Busco e inserto/actualizo al puerto resultante de la búsqueda anterior:
                result_it	= results.find( ip_str );
                if( result_it != results.end() ) {
                    //Actualizar la suma ya encontrada: 
                    aux     = kb_total + result_it->second;
                    results.erase(result_it);
                } else {
                    //Insertar la suma:
                    aux     = kb_total;
                }
                results.insert( pair<string, int>(ip_str, (int)aux) );
            } //endif current_line not #;
        } //endwhile input.good();
        input.close();
    } else {
        cout << "No se pudo abrir el archivo 'capture.txt'" << endl;
        return -1;
    }

    //Guardamos el resultado en el archivo que hara la grafica:    
    ofstream output ("graphdata_ipfrec_kb.dat", ofstream::out );
    if (output.is_open()) {

        //ALGORITMO PARA: Grafica de frecuencia vs volumend de data (KB):
        output << "# kb_total ip_frec" << endl;
        int k=0;
        for(result_it = results.begin(); result_it != results.end(); ++result_it){
            results_ip.insert( make_pair (result_it->second, result_it->first ));
            k++;
        }
        k=0;
        int max_elem;
        for(result_ip_it = results_ip.begin(); result_ip_it != results_ip.end(); ++result_ip_it){
            if(k == (results_ip.end()->first)-1)
                max_elem=result_ip_it->first;
            k++;
        }
        for(int i=0;i<max_elem;i++)
            output << i << " " << (int)results_ip.count(i) << endl;
        //FIN ALGORITMO.

    }
    else { 
        cout << "No se pudo abrir el archivo 'graphdata_ipfrec_kb.txt'" << endl;
        return -1;
    }
    output.close();

    //Ahora se llama a GNUplot para graficar el archivo resultante.
    if(not DEBUG) {
        system("gnuplot plotipfrecKb.gp");
        system("eog ImgIpFrecKb.png");
    }
	return 0;
} //end graphFrequenceKB()

int graphFrequenceNFlows(){

    vector<string>      current_tokens;
    u_int16_t 		    port_int;
    string              current_line, port_str;
    string              ip_str;
    std::stringstream 	stream;
    float               kb_total, aux = 0;
    int                 nro_ip, auxip = 0;
    map<string, int>  results;
    map<string, int>  results2;
    map<string, int>::iterator result_it;
    map<string, int>::iterator result2_it;
    map<int, string>  	port_map;		    //Para funcion de conversion de puerto -> aplicación
    map<int, string>::iterator 	 port_it;
    vector<string> ip;
    vector<int> nflw;

    map<string,int> nflwr;
    map<string,int>::iterator nflwr_it;
    multimap<int, string>  results_ip;
    multimap<int, string>::iterator 	 result_ip_it;

	//Se llena el mapa de puertos previamente:
	portsToService(port_map);
	
    ifstream input (capture_file_path.c_str(), ifstream::in );
    
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(" ", "", boost::keep_empty_tokens);
    
    //Se extraen los datos del archivo de traza:
    int traza=0;
    if ( input.is_open() ) {
        while ( input.good() ) {
            //Aunque no sea del todo necesario, resetear todas las variables
            //a su estado inicial:
            ip_str      = "";
            port_int    = 0;
            aux         = 0.0;
            kb_total    = 0.0;
            port_str.clear();
            current_line.clear();            
            current_tokens.clear();
            getline (input, current_line);


            //Ignoramos las lineas comentadas:
            if ( not boost::starts_with(current_line, "#") ) {
                //Guardar los tokens obtenidos en el vector:
                tokenizer tokens(current_line, sep);
                current_tokens.assign( tokens.begin(), tokens.end() );

                //cout << traza << endl;
                if (current_tokens.size() == 0) continue;
                
                //Impresion de resultados -- modo debug:
                if(DEBUG) {
                    cout << "Contenido del vector tokens:";
                    for (unsigned int i=0; i < current_tokens.size(); i++) {
                        //cout << " " << current_tokens[i];
                    }
                    //cout << endl;
                }

                ip_str 	        = current_tokens[2].c_str();
                kb_total        = atof( current_tokens[6].c_str() ) + 
                                  atof( current_tokens[7].c_str() );
                ip.push_back( ip_str );
                //cout << traza << " " << ip[traza] << endl;
                traza++;
                //Impresion de resultados 2 -- modo debug:
				if(DEBUG) {
                    cout << "Salida del mapeo: ";
                                        //cout << ip_str << " " << kb_total << " => ";
					//cout << port_str << " " << kb_total;
                   // cout << endl;
                }
				
                //Ahora mapeamos el resultado de la traza 
                //Busco e inserto/actualizo al puerto resultante de la búsqueda anterior:
                result_it	= results.find( ip_str );
                //cout << result_it << endl;
                if( result_it != results.end() ) {
                    //cout << result_it->first << endl;
                    //Actualizar la suma ya encontrada: 
                    aux     = kb_total + result_it->second;


                    results.erase(result_it);
                    //cout << result_it->first << " " << result_it->second << " "
                } else {
                    //Insertar la suma:
                    aux     = kb_total;
                }
                results.insert( pair<string, int>(ip_str, (int)aux) );
                //cout << results2->first <<endl;

            } //endif current_line not #;
        } //endwhile input.good();
        input.close();
    } else {
        cout << "No se pudo abrir el archivo 'capture.txt'" << endl;
        return -1;
    }

    //Guardamos el resultado en el archivo que hara la grafica:    
    ofstream output ("graphdata_ipfrec_flw.dat", ofstream::out );
    if (output.is_open()) {

        //ALGORITMO PARA: Grafica de frecuencia vs numero de flujos:
        output << "# ip_frec n_flows" << endl;
        int k=0;
        for(result_it = results.begin(); result_it != results.end(); ++result_it){
            results_ip.insert( make_pair (result_it->second, result_it->first ));
            k++;
        }

       for(result_it = results.begin(); result_it != results.end(); ++result_it)
            nflw.push_back((int)count(ip.begin(), ip.end(), result_it->first));

       for(int i=44;i>=0;i--)
            nflwr.insert( pair<string, int>(boost::lexical_cast<string>( count(nflw.begin(), nflw.end(), i) ), i) );

       for(nflwr_it = nflwr.begin(); nflwr_it != nflwr.end(); ++nflwr_it)
            output << nflwr_it->first << " " << nflwr_it->second << endl;
        //FIN ALGORITMO.

    }

    else { 
        cout << "No se pudo abrir el archivo 'graphdata_ipfrec_flw.txt'" << endl;
        return -1;
    }

    output.close();

    //Ahora se llama a GNUplot para graficar el archivo resultante.
    if(not DEBUG) {
        system("gnuplot plotipfrecflw.gp");
        system("eog ImgIpFrecflw.png");
    }
	return 0;
} //end graphFrequenceNFlows()

int graphAll()
{
    graphAppKB();
    graphPortsKB();
    graphPortsNotMappedKB();
    graphFrequenceKB();
    graphFrequenceNFlows();    

    return 0;
} //end graphAll()

/* Solo para pruebas con la version individual de este ejecutable:
int main()
{
    graphAppKB();
    graphPortsKB();
    graphPortsNotMappedKB();
    graphFrequenceKB();
    graphFrequenceNFlows();    

    return 0;
}
*/
