#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "mpi.h"

#define DATOS "datos.dat"
//#define L 4
#define STAN_SIZE 1024

int leerfichero(float *n,char *d);
void Vecinos(int rank, int sq, int *vNorth, int *vSouth, int *vEast, int *vWest);
float calculoMinimo(int rank,float buffer,int sq,int vEast,int vWest,int vNorth,int vSouth);

int main(int argc, char *argv[])
{
	float *numeros=malloc(STAN_SIZE*sizeof(float));
	char *datos=malloc(STAN_SIZE*sizeof(char));
	int i,flag=1,rank,size,numDatos,vNorth,vSouth,vEast,vWest;
	float global,buffer;
	MPI_Status status;
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	
	if (rank==0){
		numDatos=leerfichero(numeros,datos);
		if(numDatos!=(L*L)){
			printf("El numero de datos generados no es compatible con la dimension del toroide\n");
			flag=0;
			MPI_Bcast(&flag,1,MPI_INT,0,MPI_COMM_WORLD);
		}
		if(numDatos!=size){
		   printf("El numero de procesos %d es distinto al numero de datos %d\n",size,numDatos);
		   flag=0;
			MPI_Bcast(&flag,1,MPI_INT,0,MPI_COMM_WORLD);
		   
		}
		for(i=0;i<size;i++){
			buffer=numeros[i];
			MPI_Send(&buffer,1,MPI_FLOAT,i,0,MPI_COMM_WORLD);
		}
	}
	
	MPI_Bcast(&flag,1,MPI_INT,0,MPI_COMM_WORLD);

	if(flag!=0){
		MPI_Recv(&buffer,1,MPI_FLOAT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		Vecinos(rank,L,&vNorth,&vSouth,&vEast,&vWest);
		global=calculoMinimo(rank,buffer,L,vEast,vWest,vNorth,vSouth);
	
		if(rank==0){
			printf("El dato cuyo valor es menor en la red es %3.2f\n",global);
		}
    }

	MPI_Finalize();
	free(numeros);
	free(datos);
	return 0;
}

void Vecinos(int rank, int l, int *vNorth, int *vSouth, int *vEast, int *vWest){
	int fila,columna;
	fila=rank/l;
	columna=rank%l;

	//Saco el nodo al SUR del rank actual
	if(fila==0){
		*vSouth=((l-1)*l)+columna; 
	}
	else{
		*vSouth=((fila-1)*l)+columna;
	}
	//Sacaremos el nodo al NORTE del rank actual

	if(fila==l-1){
		*vNorth=columna;
	}
	else{
		*vNorth=((fila+1)*l)+columna;
	}
	//Sacaremos el nodo al OESTE(izquierda) del rank actual
	if(columna==0){
		*vWest=(fila*l)+(l-1);
	}else{
		*vWest=(fila*l)+(columna-1);
	}
	//Sacamos el nodo al ESTE(derecha) del rank actual
	if((columna+1)==l){
		*vEast=fila*l;
	}
	else{
		*vEast=(fila*l)+(columna+1);
	}
}


int leerfichero(float *n,char *d){

FILE *fp;  //Descriptor del archivo
char *f; //Puntero que vamos a utilizar para ir recogiendo el valor de strtok
int i=0; //Numero de datos finales que tendremos en Numeros 

if((fp=fopen(DATOS,"r"))==NULL){
	fprintf(stderr,"Error al abrir el archivo %s\n",DATOS);
	return EXIT_FAILURE;
}
//almacenamos los string dentro del puntero datos
fscanf(fp,"%s",d);
fclose(fp);
n[i++] = atof(strtok(d,",")); //Guardamos el primer elemento en numeros
while( (f=strtok(NULL,",")) != NULL){
	n[i++] =atof(f);//Recorremos hasta que sea NULL, es decir el ultimo elemento
}
return i; // a la vez vamos almacenando el numero de datos que tenemos en el datos.dat
}
float calculoMinimo(int rank,float buffer,int lado,int vEast,int vWest,int vNorth,int vSouth){
	int i;
	float global=3000,minimo;
	MPI_Status status;
	//Calculamos en orden de fila 
	for(i=0;i<lado;i++){
		minimo=buffer;
		if(buffer<global){
			global=minimo;
		}
		MPI_Send(&global,1,MPI_FLOAT,vEast,i,MPI_COMM_WORLD);
		MPI_Recv(&buffer,1,MPI_FLOAT,vWest,i,MPI_COMM_WORLD,&status);
		if(buffer<global){
			global=buffer;
		}
	}
	//Calculamos en orden de columna
	for(i=0;i<lado;i++){
		MPI_Send(&global,1,MPI_FLOAT,vNorth,i,MPI_COMM_WORLD);
		MPI_Recv(&buffer,1,MPI_FLOAT,vSouth,i,MPI_COMM_WORLD,&status);
		if(buffer<global){
			global=buffer;
		}
	}
return global;
}
