#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main(int argc,char *argv[]){
int numGen;
int i;
char numeroCompleto[20];
float numero;
int parteEntera;
int decimal;
FILE *fp;
srand((unsigned) time (NULL));
if(argc!=2){
	printf("No se generarán números aleatorios \n ");
	exit(0);
}
fp= fopen("datos.dat","w+");

numGen=atoi(argv[1]);

for(i=0;i<numGen;i++){
decimal=rand()%999;
parteEntera=rand()%1000 - 500;
	if(i==(numGen-1)){
	sprintf(numeroCompleto,"%i.%i",parteEntera,decimal);

	}
	else{
		sprintf(numeroCompleto,"%i.%i,",parteEntera,decimal);
	}

fputs(numeroCompleto,fp);
}

fclose(fp);
printf("---- Numeros generados: %i -----\n",numGen);
return 0;
}