RUN = mpirun -n
COMPGEN= gcc GeneradorNumerosAleatorios.c -o genera
GEN= ./genera 
.PHONY:clean
#Ejecutamos make y se limpiará la carpeta y compilaremos el programa que genera
#los numeros aleatorios
clean: 
	$(RM) *~
	$(RM) Toroide Hipercubo
	$(RM) genera
	$(RM) datos.dat
	$(COMPGEN)
#---------------TOROIDE------------------------------
#Numero de procesos distinto al número de datos
toroideFallo1:
	$(GEN) 16
	mpicc -g ToroidePrac1.c -o Toroide -DL=4
	$(RUN) 15 Toroide

#Ejecutamos  no se puede generar la red toroidal por que la dimensión es distinta al número de datos
toroideFallo2:
	$(GEN) 16
	mpicc -g ToroidePrac1.c -o Toroide -DL=5
	$(RUN) 16 Toroide
#Ejecutamos en el caso bueno
toroideBueno1: 
		$(GEN) 16
		mpicc -g ToroidePrac1.c -o Toroide -DL=4
		$(RUN) 16 Toroide
#Otro ejemplo de buena ejecución

toroideBueno2:
		$(GEN) 36 
		mpicc -g ToroidePrac1.c -o Toroide -DL=6
		$(RUN) 36 Toroide
#-----------------HIPERCUBO-----------------------------

#Numero de datos no apto para la dimension  
hipercuboFallo1:
		$(GEN) 16
		mpicc -g -lm HiperPrac1.c -o Hipercubo -DDIMENSION=5
	   	$(RUN) 16 Hipercubo 

#Numero de datos distinto al numero de procesos lanzados
hipercuboFallo2:
		$(GEN) 16
		mpicc -g -lm HiperPrac1.c -o Hipercubo -DDIMENSION=4
	   	$(RUN) 15 Hipercubo 

#Ejecución de un caso correcto

hipercuboBueno1:
		$(GEN) 16
		mpicc -g -lm HiperPrac1.c -o Hipercubo -DDIMENSION=4
		$(RUN) 16 Hipercubo
hipercuboBueno2:
		$(GEN) 32
		mpicc -g -lm HiperPrac1.c -o Hipercubo -DDIMENSION=5
		$(RUN) 32 Hipercubo

