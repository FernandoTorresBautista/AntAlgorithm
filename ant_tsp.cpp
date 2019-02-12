#include <vector>
#include <map>
#include <time.h> 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <queue>
#include <sys/time.h>
#include <fstream>
#include <utility>
#include <algorithm>
#include <set>

using namespace std;
	
	int N, alpha, beta, NCxSPA, N_Ants;//Numero de Ciudades por Seleccion de Probabilidad al Azar
	double b_cR;
	vector<int> best;
	map<int, pair<double, double > > Nodos;   // Ciudades y posiciones x e y 
    map< pair< int, int>, double> Distancias; // Distancias en las ciudaes i,j y j,i
    map< pair< int, int>, double> Feromonas;  // Cantidad de feronomonas de forma colectiva en la colonia de N hor migas para N ciudades
    
int siguiente_estado(int actual, vector<int>ruta ){ //Se deben seleccionar a los 5 mejores con respecto a la ciudad actual y despues hacer uso de la probabilidad
	int bestN=0;
	map< double, int > C_d;
	pair< int, int > Ci;
	Ci.first = actual;
	vector<int>::iterator it;
	for( int c=1; c<N+1; c++){	// Obtener las ciudades a las que puede ir la ciudad actual  
		it = find( ruta.begin(), ruta.end(), c);
		if( c!=actual && ( it==ruta.end() ) ){
			Ci.second = c;
			C_d.insert( pair<double, int>( Distancias.find( Ci )->second , c ) );//Insertar para ordenar de menor a mayor 
		}
	}
	map< double, int >::iterator itmap;
	map< double, int >Pro;
	double suma=0;
	double cto, Fac, invD;
	int cdd;
	//Sumatoria
	for(itmap = C_d.begin(); itmap!=C_d.end(); itmap++){
		cto = itmap->first;
		cdd = itmap->second;
		Ci.second = cdd;
		if( Feromonas.find( Ci )!=Feromonas.end() ){
			Fac = Feromonas.find( Ci )->second;
		}
		else{
			Fac = Feromonas.find( pair<int, int>(cdd, Ci.first) )->second;
		}
		invD = 1/cto;
		Fac = pow(Fac, alpha);
		invD = pow(invD, beta);
		suma += (Fac * invD);
	}
	//Probabilidades
	for(itmap = C_d.begin(); itmap!=C_d.end(); itmap++){
		cto = itmap->first;
		cdd = itmap->second;
		Ci.second = cdd;
		if( Feromonas.find( Ci )!=Feromonas.end() ){
			Fac = Feromonas.find( Ci )->second;
		}
		else{
			Fac = Feromonas.find( pair<int, int>(cdd, Ci.first) )->second;
		}
		invD = 1/cto;
		Fac = pow(Fac, alpha);
		invD = pow(invD, beta);
		double Ract = (Fac*invD / suma);
		Pro.insert(pair<double, int>(Ract, cdd));
	}
	map< double, int >::reverse_iterator Pit;//Estan ordenados de menor a mayor probabilidad asi que obtener la ciudad al final del objeto de probabilidades
	if( Pro.size() > NCxSPA ){		//Si son mas de 2 ciudades por ewcoger en la ruta se hace al azar 
		int pos_r = rand()%( NCxSPA ); // 1+rand()%( posibles );
		if(pos_r<=1){
			Pit = Pro.rbegin();
			bestN = Pit->second;
			return bestN;
		}
		for(Pit= Pro.rbegin(); Pit!=Pro.rend(); Pit++){
			if( pos_r==1 ){
				bestN = Pit->second;
				return bestN;
			}
			pos_r--;
		}
	}
	else{
		Pit = Pro.rbegin();
		bestN = Pit->second;
		return bestN;
	}
}

double cRuta( vector<int>ruta ){
	double costo=0.0;
	vector<int>::iterator it = ruta.begin();
	vector<int>::reverse_iterator fruta = ruta.rbegin();
	int actual = *it;
	pair<int,int>Ci;
	Ci.first = actual; Ci.second = *fruta;
	costo= Distancias.find( Ci )->second;	//Del último al primero y despues la ruta normal del primero al último
	it++;
	for( ; it!=ruta.end(); it++){
		Ci.first = actual; Ci.second = *it;
		costo += double( Distancias.find( Ci )->second );
		actual = *it;
	}
	return costo;
}

bool r_ruta(pair<int, int>arc, vector<int>ruta){
	vector<int>::iterator it;
	it = find( ruta.begin(), ruta.end(), arc.first);
	if( it==ruta.end() ){
		return false;
	}
	else{
		int x, y;
		x = *it;
		it++;
		y = *it;
		if( x==arc.first && y==arc.second ){
			return true;
		}
	}
	return false;
}

void iRuta( vector<int>ruta ){
	vector<int>::iterator it = ruta.begin();
	cout << "Ruta: { " ;
	for( ; it!=ruta.end(); it++){
		cout << *it << " - " ;
	}
	it=ruta.begin();
	cout << *it << "}" <<endl;
}

double timeval_diff(struct timeval *a, struct timeval *b){
  return
    (double)(a->tv_sec + (double)a->tv_usec/1000000) -
    (double)(b->tv_sec + (double)b->tv_usec/1000000);
}
	    
int main() {
	srand(time(NULL));
    int aux, nodo, iteraciones=1, max_it;
    double evaporacion;
    pair<  double, double> pos;
    pair< int, int> n1, n2;
    string archivo, cadena, sub;
    //Lectura de archivo 
    cout <<"Archivo:  ";
    cin >> archivo;
	ifstream fe( archivo.c_str() );
	getline(fe, cadena);
    N = aux = atoi( cadena.c_str() );
	while( aux-- ) {
		getline(fe, cadena);
		if(cadena.at(0)==' ' ){
			cadena = cadena.substr(1, cadena.length());
			sub = cadena.substr( 0 , cadena.find_first_of(" ")+1 );
			nodo = int( atoi( sub.c_str() ) );
			sub = cadena.substr( cadena.find_first_of(" ")+1 , cadena.find_last_of(" ")+1 );
			pos.first = double( atoi( sub.c_str() ) );
			sub = cadena.substr( cadena.find_last_of(" ")+1 , cadena.length() );
			pos.second = double( atoi( sub.c_str() ) );
		}
		else{
			sub = cadena.substr( 0 , cadena.find_first_of(" ") );
			nodo = int( atoi( sub.c_str() ) );
			sub = cadena.substr( cadena.find_first_of(" ") , cadena.find_last_of(" ") );
			pos.first = double( atoi( sub.c_str() ) );
			sub = cadena.substr( cadena.find_last_of(" ") , cadena.length() );
			pos.second = double( atoi( sub.c_str() ) );
		}
		Nodos[ nodo ] = pos;
	}
	fe.close();
    //Distancias de i a j, y de j a i  
    for (int i = 1; i < Nodos.size(); ++i){
		for (int j = i+1; j < Nodos.size()+1 ; ++j){
            n1.first=i; n1.second=j;
            n2.first=j; n2.second=i;
            Distancias[n1] = sqrt( pow( Nodos[i].first - Nodos[j].first ,2.0) + pow( Nodos[i].second - Nodos[j].second,2.0) );
            Distancias[n2] = Distancias[n1];
		}
	}
	//Feromonas 
	for( int i=1; i < Nodos.size(); i++){
		for( int j=i+1; j< Nodos.size()+1 ; j++){
			n1.first=i; n1.second=j;
            n2.first=j; n2.second=i;
            double distancia = Distancias[ n1 ];
			Feromonas[ n1 ] = 1/ distancia;
		}
	}
	
	struct timeval t_ini, t_fin;
	double secs;
	vector<double>Tiempos;
	
	cout << "#Hormigas alpha beta rho #Iteraciones [#Ciudades seleccionadas por su probabilidad para escoger el siguiente estado]"<<endl;
	cin >> N_Ants >>alpha >> beta >> evaporacion >> max_it >> NCxSPA;
	while( iteraciones <= max_it ){ // Numero de iteraciones 
		
		gettimeofday(&t_ini, NULL);
		
		for(int ant_k=1; ant_k< N_Ants ; ant_k++){		//Construir una solucion para cada hormiga  
			vector<int>ruta, no_visitado;
			int c_act, c_sig;
			c_act = 1 + ( rand() % N );  //Iniciar la ruta desde una ciudad aleatoria
			ruta.push_back( c_act );
			while( ruta.size() != N ){				// Mientras la ruta no se haya construido, pasar al siguiente estado
				c_sig = siguiente_estado( c_act, ruta );	
				ruta.push_back( c_sig );
				c_act = c_sig;
			}
			map< pair<int, int>, double>::iterator itF;	// Realizar actualizacion de feromonas
			for(itF = Feromonas.begin(); itF!=Feromonas.end(); itF++ ){
				pair<int,int>arco = itF->first;
				pair<int,int>arco_vuelta;
				arco_vuelta.first = arco.second;
				arco_vuelta.second = arco.first;//Si esta en el vector se evapora y se agrega una porcion de feromonas por parte de la hormiga
				if( r_ruta(arco, ruta) || r_ruta(arco_vuelta, ruta)  ){	
					double cFeromonas = itF->second;
					double inv_dist = Distancias.find( arco )->second;
					inv_dist = 1/inv_dist;
					cFeromonas = cFeromonas*(1-evaporacion) + inv_dist;
					itF->second = cFeromonas;
				}
				else{	//Si no esta en el vector de ruta simplemente evaporar las feromonas en la arista
					double cFeromonas = itF->second;
					cFeromonas = cFeromonas*(1-evaporacion);
					itF->second = cFeromonas;
				}
			}
			if( ant_k == 1  && iteraciones == 1 ){
				b_cR = cRuta(ruta);
				best = ruta;
			}
			else{
				double costoT = cRuta( ruta );
				if(costoT < b_cR ){
					best.clear();//best.erase(best.begin(), best.end());
					best = ruta;
					b_cR = costoT;
				}
			}
		}
		iteraciones++;
		
		gettimeofday(&t_fin, NULL);
		secs = (1000)*(timeval_diff(&t_fin, &t_ini));
		Tiempos.push_back( secs );	
	}
	
	cout << "\nCosto de ruta final: "<< b_cR <<endl;
	iRuta( best );
	
	double suma=0;
	cout << " tiempos por iteracion(i) " <<endl;
	for(int i=0; i<int( Tiempos.size() ); i++){
		suma += Tiempos.at(i);
		cout << " t("<< (i+1) << ") = " << Tiempos.at(i) << " ms" <<endl;
	}
	cout << "Promedio de ejecucion = " << ( suma/Tiempos.size() ) << " ms" <<endl;
	
	return 0;
}





