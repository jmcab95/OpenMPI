#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "mpi.h"

//#define DIMENSION 4
#define DATOS "datos.dat"
#define STAN_SIZE 1024

float calculoMaximo(int rank,float buffer,int dimension,int *vecinos);
int leerfichero(float *n,char *d);
void VecinosHiper(int rank,int dimension,int *vecinos);

int main(int argc, char *argv[])
{
	float *numeros=malloc(STAN_SIZE*(sizeof(float)));
	char *datos=malloc(STAN_SIZE*sizeof(char));
	int rank,size,i,flag=1;
	int vecinos[DIMENSION];
	float buffer,global;
	MPI_Status status;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	MPI_Comm_size(MPI_COMM_WORLD,&size);
	

	if (rank==0){
		int numDatos=leerfichero(numeros,datos);
		if(log2(numDatos) != (DIMENSION)){
			printf("El numero de datos generados no es apto para la Dimension que tenemos\n");
			flag=0;
			MPI_Bcast(&flag,1,MPI_INT,0,MPI_COMM_WORLD); // Envio el código de error a todos los nodos
		}
		if(numDatos!=size){
		    printf("El numero de procesos %d es distinto al numero de datos %d\n",size,numDatos);
		    flag=0;
			MPI_Bcast(&flag,1,MPI_INT,0,MPI_COMM_WORLD);// Envio el codigo de error a todos los nodos
		   
		}
		for(i=0;i<size;i++){
			buffer=numeros[i];
			MPI_Send(&buffer,1,MPI_FLOAT,i,0,MPI_COMM_WORLD);
		}
	}
	MPI_Bcast(&flag,1,MPI_INT,0,MPI_COMM_WORLD); // Si se ha producido algún error recibiría 
	//el valor de flag cambiado, no pudiendo entrar así en la condición

	if(flag != 0){
		MPI_Recv(&buffer,1,MPI_FLOAT,0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		VecinosHiper(rank,DIMENSION,vecinos);
		global=calculoMaximo(rank,buffer,DIMENSION,vecinos);

		if(rank==0){
			printf("El dato cuyo valor es mayor en la red es %3.2f\n",global);
		}
	}

	MPI_Finalize();
	free(numeros);
	free(datos);
	return 0;
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

float calculoMaximo(int rank,float buffer,int dimension,int *vecinos){
	int i;
	float global=-3000,mayor;
	MPI_Status status;
	
	for(i=0;i<dimension;i++){
		mayor=buffer;
		if(buffer>global){
			global=mayor;
		}
		MPI_Send(&global,1,MPI_FLOAT,vecinos[i],i,MPI_COMM_WORLD);
		MPI_Recv(&buffer,1,MPI_FLOAT,vecinos[i],i,MPI_COMM_WORLD,&status);
		if(buffer>global){
			global=buffer;
		}
	}
return global;
}

void VecinosHiper(int rank,int dimension,int *vecinos){
int i;

for(i=0;i<dimension;i++){
	vecinos[i]=(rank^((int)pow(2,i)));
}

}