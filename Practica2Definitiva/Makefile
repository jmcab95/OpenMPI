RUN = mpirun -np 1 


.PHONY:clean

clean: 
	$(RM) *~
	$(RM) pract2

practica2:
	 mpicc pract2.c -o pract2 -lX11 -DFILTRO=0
	$(RUN) pract2
practica2Sepia:
	mpicc pract2.c -o pract2 -lX11 -DFILTRO=1
	$(RUN) pract2
practica2Verde:
	 mpicc pract2.c -o pract2 -lX11 -DFILTRO=2
	$(RUN) pract2
practica2Azul:
	 mpicc pract2.c -o pract2 -lX11 -DFILTRO=3
	$(RUN) pract2
practica2Rojo:
	 mpicc pract2.c -o pract2 -lX11 -DFILTRO=4
	$(RUN) pract2
