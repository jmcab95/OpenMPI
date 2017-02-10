/* Pract2  RAP 09/10    Javier Ayllon*/

#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h> 
#include <assert.h>   
#include <unistd.h>

#define NIL (0)       
#define NUMEROPROCESOS 3 //Numero de procesos que lanzara el rank 0
#define FOTO "foto.dat"
#define LONLADO 400 // La longitud de un lado de la imagen, la imagen es 400x400
//#define FILTRO //A la hora de compilar elegiremos un valor para ver que filtro aplicamos a la imagen

/*Variables Globales */
void filtros(int filtro,int *buf,int i, int j,unsigned char *rgb);
void control(int *buf);
XColor colorX;
Colormap mapacolor;
char cadenaColor[]="#000000";
Display *dpy;
Window w;
GC gc;
int errcodes[NUMEROPROCESOS];

/*Funciones auxiliares */

void initX() {

      dpy = XOpenDisplay(NIL);
      assert(dpy);

      int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
      int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

      w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
                                     400, 400, 0, blackColor, blackColor);
      XSelectInput(dpy, w, StructureNotifyMask);
      XMapWindow(dpy, w);
      gc = XCreateGC(dpy, w, 0, NIL);
      XSetForeground(dpy, gc, whiteColor);
      for(;;) {
            XEvent e;
            XNextEvent(dpy, &e);
            if (e.type == MapNotify)
                  break;
      }


      mapacolor = DefaultColormap(dpy, 0);

}

void dibujaPunto(int x,int y, int r, int g, int b) {

        sprintf(cadenaColor,"#%.2X%.2X%.2X",r,g,b);
        XParseColor(dpy, mapacolor, cadenaColor, &colorX);
        XAllocColor(dpy, mapacolor, &colorX);
        XSetForeground(dpy, gc, colorX.pixel);
        XDrawPoint(dpy, w, gc,x,y);
        XFlush(dpy);

}



/* Programa principal */

int main (int argc, char *argv[]) {

  int rank,size;
  MPI_Comm commPadre,interComm;
  MPI_Status status;
  int buf[5]; //Buffer para almacenar los 5 elementos que necesito
  //coordenada en x , coordenada en y, así como la tripleta de 
  //los colores, r, g ,b 
  MPI_File fphoto; //descriptor Fichero MPI
  int i,j;

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_get_parent( &commPadre );
  if ( (commPadre==MPI_COMM_NULL) && (rank==0) )  {
	  initX();
    MPI_Comm_spawn("pract2",MPI_ARGV_NULL, NUMEROPROCESOS, MPI_INFO_NULL,0,MPI_COMM_WORLD,&interComm,errcodes);
	   
    //Realizamos un bucle desde 0 a 400*400 que es la dimension de la imagen
    for(i=0;i<(LONLADO*LONLADO);i++){
      MPI_Recv(&buf,5,MPI_INT,MPI_ANY_SOURCE,MPI_ANY_TAG,interComm,&status);

  /*En algun momento dibujamos puntos en la ventana algo como
  dibujaPunto(x,y,r,g,b);  */

      dibujaPunto(buf[0],buf[1],buf[2],buf[3],buf[4]);
    }

   
    sleep(5); //dejamos 5 segundos para que muestre el gato
  }
  else {
    int tamanioLeerProceso,desplazamiento,filaInicio,filaFinal;
    tamanioLeerProceso=LONLADO/NUMEROPROCESOS;
    desplazamiento=tamanioLeerProceso*LONLADO*3*sizeof(unsigned char);
    //Será las filas que lee cada proceso por las columnas que son 400 por la tripleta que leerá en cada punto
    //que es del tipo unsigned char
    unsigned char rgb[3]; //La tripleta de colores por puntos

    filaInicio=rank*tamanioLeerProceso; //Fila en la cual empezamos a leer la imagen, dependerá del rank o identificador del proceso
    filaFinal=filaInicio+tamanioLeerProceso; //Así y teniendo en cuenta lo anterior variará la filaFinal
    //Tendremos para cada proceso que se ejecute un marco en el que moverse

  
    /* Codigo de todos los trabajadores */
    /* El archivo sobre el que debemos trabajar es foto.dat */

    MPI_File_open(MPI_COMM_WORLD,FOTO,MPI_MODE_RDONLY,MPI_INFO_NULL,&fphoto);
    MPI_File_set_view(fphoto,desplazamiento*rank,MPI_UNSIGNED_CHAR,MPI_UNSIGNED_CHAR,"native",MPI_INFO_NULL);
    //Por medio de MPI_File_set_view. Asignamos a cada proceso una vista sobre la que trabajar
    //en paralelo,
    //La vista la definimos por medio de tres parametos :Desplazamiento,tipo de datos elemental
	
    	if(rank==NUMEROPROCESOS-1){
	  	filaFinal=LONLADO;     //Controlo que si el número de procesos no es divisible entre 
					// el tamaño del bloque, se ejecute el programa correctamente
	}
int inicio,fin;
inicio=filaInicio*LONLADO;
fin=filaFinal*LONLADO;

printf("Hola soy el proceso %d y voy a empezar a leer en el punto x=%d , y= %d \n",rank,inicio,fin);
printf("Soy el proceso %d y el tamaño de bloque que he recorrido es: %d \n",rank, fin-inicio);
    for (i=filaInicio;i<filaFinal;i++){
      for(j=0;j<LONLADO;j++){
        MPI_File_read(fphoto,rgb,3,MPI_UNSIGNED_CHAR,&status);

       
        filtros(FILTRO,buf,i,j,rgb); //Llamamos al metodo de aplicacion de filtros
        
        control(buf); // Controlamos si aumentan los valores
        // a más de 255, que es el valor maximo para cada valor rgb
        //Si no lo controlasemos, al aplicar un filtro se vería una imagen errónea

        MPI_Send(&buf,5,MPI_INT,0,1,commPadre);
      }


    }
    MPI_File_close(&fphoto); //Cerramos el fichero, como si se tratase de un fichero
  }

  MPI_Finalize();

}
void control(int *buf){ // Este metodo lo aplicamos para en caso de que 
  // añadamos un filtro, los valores no superen el 255 distorsionando así la imagen
if(buf[2]>255){
          buf[2]=255;
        }
        if(buf[3]>255){
          buf[3]=255;
        }
        if(buf[4]>255){
          buf[4]=255;
        }

}

void filtros(int filtro,int *buf,int i, int j,unsigned char *rgb){

 buf[0]=j; //Almacenamos los valores de la posicion x y del pixel 
  buf[1]=i; //leido

        switch(FILTRO){
          case 0: //Mostrar la imagen de manera normal (SIN FILTROS)
            buf[2]=(int)rgb[0];
            buf[3]=(int)rgb[1];
            buf[4]=(int)rgb[2];
          break;

          case 1: //Mostar la imagen on un filtro sepia
            buf[2]=(int)rgb[0]*0.6;
            buf[3]=(int)rgb[1]*0.28;
            buf[4]=(int)rgb[2]*0.12;
          break;

          case 2: //Mostrar la imagen con un filtro Verde
            buf[2]=(int)rgb[0]+0;
            buf[3]=(int)rgb[1]+145;
            buf[4]=(int)rgb[2]+80;
          break;

          case 3: //Mostrar la imagen con un filtro Azul
            buf[2]=(int)rgb[0]+0;
            buf[3]=(int)rgb[1]+112;
            buf[4]=(int)rgb[2]+184;
          break;

          case 4: // Mostrar la imagen con un filtro Rojo
            buf[2]=(int)rgb[0]+230;
            buf[3]=(int)rgb[1]+0;
            buf[4]=(int)rgb[2]+38;
          break;
        }
}
