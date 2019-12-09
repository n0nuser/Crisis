#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
/* ------------------------------ PROTOTIPOS ------------------------------ */
  int isNumber(char number[]);
  void uso(char mensajes[],int MIN_PROCS,int MAX_PROCS);
  unsigned int dormir(unsigned int nsecs);
  void sig_alrm(int signo);
  void muerto(int signo);
  void salir(int retorno);
/* ------------------------------------------------------------------------ */

/* ----------------------------------- */
  int main(int argc, char *argv[]){
/* ----------------------------------- */
  setlocale(LC_ALL, "spanish");
  //VARIABILITAS
  const int MIN_PROCS = 3;
  const int MAX_PROCS = 33;
  char velocidad[2][7] = {"normal", "veloz"};
  char mensajes[500];
  pid_t estado_retorno, child;
  int arg1;
  FILE *file;

  // COMPROBACIONES
  if(argc != 2){
    if(argc == 1){
      perror(" ERROR: no se ha recibido el argumento necesario. (Número de procesos)");
      uso(mensajes,MIN_PROCS, MAX_PROCS);
    }
    else if(argc > 3){
        perror(" ERROR: se han recibido demasiados argumentos. (Minimo 1, maximo 2).");
        uso(mensajes,MIN_PROCS, MAX_PROCS);
    }
  }
  else if (argc == 2 || argc == 3){
    // Comprobaciones del segundo argumento
    *argv[1] = tolower(*argv[1]);
    //
    if(isNumber(argv[1]) == 1){
      perror(" ERROR: el argumento recibido no es un número.");
      uso(mensajes,MIN_PROCS, MAX_PROCS);
    }
    arg1 = atoi(argv[1]);
    if((arg1 < MIN_PROCS) || (arg1 > MAX_PROCS)){
        perror(" ERROR: el número de procesos no está en el rango de valores posibles.");
        uso(mensajes,MIN_PROCS, MAX_PROCS);
    }
    if(argc == 3){
      //Si hay 3 argumentos
      *argv[2] = tolower(*argv[2]);
      //
      if(isNumber(argv[1]) == 0){
        perror(" ERROR: el argumento <velocidad> es un número.");
        uso(mensajes,MIN_PROCS, MAX_PROCS);
      }
      else if(strcmp(velocidad[0],argv[2]) != 0 || strcmp(velocidad[1],argv[2]) != 0){
        perror(" ERROR: el valor para la velocidad es incorrecto.");
        uso(mensajes,MIN_PROCS, MAX_PROCS);
      }
    }
  }

  // PROGRAMA
  signal(SIGINT,salir);
  printf("Soy el padre de todos PID: %d\n\n",getpid());
  int fd = open("limitador.tmp", O_RDWR | O_CREAT);
  if(fd == -1) perror("Error al crear archivo: ");
  do{
    for (int i = 0; i < arg1; i++) {
      srand(time(NULL));
      dormir(rand() % 5 + 1);
      child = fork();
      if (child < 0){
          //Hijo no se ha creado correctamente
          perror("Fork() error ");
          exit(1);
        }
      else if (child ==  0) {
        //Hijo creado correctamente
        printf("# [\e[1;32m%d\e[0m] HIJO DE [\e[1;33m%d\e[0m].\n",getpid(),getppid());
        //AQUI TIENE QUE AUMENTAR EL NUMERO DE PROCESOS DEL FICHERO.
        //El hijo termina
        srand(time(NULL));
        if((rand() % 2) == 0){printf("  Crea hijo.\n");}
        else raise(SIGTERM);
      }
      else{
        //Este es el proceso padre. Se llega aquí cuando sus hijos han muerto.
        //Cuando un hijo muere, el padre haga un wait sin bloquearse.
        wait(NULL);
        printf("# [\e[1;31m%d\e[0m] HIJO DE [\e[1;33m%d\e[0m].\n",child,getpid());
        //AQUÍ TIENE QUE DECREMENTAR EL NUMERO DE PROCESOS DEL FICHERO.
        printf("\n~ [\e[1;33m%d\e[0m] PADRE\n\n",getpid());
      }
      wait(&estado_retorno); //Solo el padre espera
    } // Fin del for
  }while(1);
}  // Fin del main

/* ------------------------------------------------------------- */
  void uso(char mensajes[],int MIN_PROCS,int MAX_PROCS){
/* ------------------------------------------------------------- */
    sprintf(mensajes, "\n USO: ./crisis <numero de procesos> [<velocidad>]");
    if(write(1,mensajes,strlen(mensajes))==-1) perror("");

    sprintf(mensajes, "\n El número de procesos debe ser un NÚMERO ENTERO entre %d y %d.", MIN_PROCS, MAX_PROCS);
    if(write(1,mensajes,strlen(mensajes))==-1) perror("");

    sprintf(mensajes,"\n La velocidad puede ser 'normal' o 'veloz'.\n");
    if(write(1,mensajes,strlen(mensajes))==-1) perror("");
}

/* -------------------------------- */
  int isNumber(char number[]){
/* -------------------------------- */
    int i = 0;
    if (number[0] == '-')
      i = 1;
    for (; number[i] != 0; i++){
        if (!isdigit(number[i]))
          return 1;
    } return 0;
}

/*-----------------------------------*/
  void sig_alrm(int signo){}
/*-----------------------------------*/

/*----------------------------------------------*/
  void muerto(int signo){wait(NULL);}
/*----------------------------------------------*/

/* ---------------------------------------------- */
  unsigned int dormir(unsigned int nsecs){
/* ---------------------------------------------- */
	if (signal(SIGALRM, sig_alrm) == SIG_ERR) return(nsecs);
	alarm(nsecs);
	pause();
	return(alarm(0));
}

/* ------------------------------------------------ */
  void salir(int retorno) {
/* ------------------------------------------------ */
  char mensajes[37];
  sprintf(mensajes,"\nPrograma acabado correctamente.\n");
  if(write(1,mensajes,strlen(mensajes))==-1) perror("");
  exit(0);
}
