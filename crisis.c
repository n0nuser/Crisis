#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <fcntl.h>

#define PID_BASH getppid()

/* ------------------------------ PROTOTIPOS ------------------------------ */
  int isNumber(char number[]);
  void uso(char mensajes[],int MIN_PROCS,int MAX_PROCS);
  void salir(int retorno);
  void muerte_total(int retorno);
  void muerte_hijo(int retorno);
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
  const int arg1 = atoi(argv[1]);
  int modo;
  FILE *file;
  //SIGACTIONS
  struct sigaction sigint,sigchild;
  sigint.sa_handler=muerte_total;
  sigchild.sa_handler=muerte_hijo;
  if(sigaction(SIGINT,&sigint,NULL)==-1) return -1;
  if(sigaction(SIGCHLD,&sigchild,NULL)==-1) return -1;
  //

  // COMPROBACIONES
  if (argc == 2 || argc == 3){
    // Comprobaciones del segundo argumento
    if(isNumber(argv[1]) == 1){
      perror(" ERROR: el argumento recibido no es un número.");
      uso(mensajes,MIN_PROCS, MAX_PROCS);
      exit(1);
    }
    if((arg1 < MIN_PROCS) || (arg1 > MAX_PROCS)){
        perror(" ERROR: el número de procesos no está en el rango de valores posibles.");
        uso(mensajes,MIN_PROCS, MAX_PROCS);
        exit(1);
    }
    if(argc == 2){
      modo=0;
    }
    if(argc == 3){
      //Si hay 3 argumentos
      //Comprobaciones velocidad
      if(strcmp(argv[2], "normal") == 0){
        modo = 0;
      } else if(strcmp(argv[2], "veloz") == 0){
        modo = 1;
      } else {
        perror(" ERROR: el argumento <velocidad> es 'lento' o 'veloz'.");
        uso(mensajes,MIN_PROCS, MAX_PROCS);
        exit(1);
      }
    }
  }
  else{
    if(argc == 1){
      perror(" ERROR: no se ha recibido el argumento necesario. (Número de procesos)");
      uso(mensajes,MIN_PROCS, MAX_PROCS);
      exit(1);
    }
    else if(argc > 3){
        perror(" ERROR: se han recibido demasiados argumentos. (Minimo 1, maximo 2).");
        uso(mensajes,MIN_PROCS, MAX_PROCS);
        exit(1);
    }
  }
  // PROGRAMA
  sprintf(mensajes,"[DEBUG] %d procesos\n",arg1);
  if(write(1,mensajes,strlen(mensajes))==-1) perror("");
  pid_t pid_inicial = getpid();
  sprintf(mensajes,"Soy el padre de todos [%d]\n\n",pid_inicial);
  if(write(1,mensajes,strlen(mensajes))==-1) perror("");
  //// PARA QUE META UN 0
  int contador1 = 0; //Para pasarlo a int
  int fd = open("contador.txt", O_RDWR | O_CREAT); // Abrimos el archivo
  if(fd == -1) perror("Error al crear archivo: ");
  if (write(fd,&contador1,sizeof(contador1))==-1) perror("Error al escribir en archivo: "); //Escribe un 0
  lockf(fd,F_LOCK,0);
  if (close(fd) < 0) perror("Error al cerrar el archivo"); //Cierra el archivo
  ////
  while(1){
    if(contador1 >= arg1){
      //Aquí para que muera uno cuando el contador llega al valor pasado
      if(getpid() != pid_inicial){
        raise(SIGTERM);
        sprintf(mensajes,"\e[42m# -\e[0m ABRIR FICHERO DESPUÉS DE MATAR - PID: %d - CONTADOR = %d\n",getpid(),contador1);
        if(write(1,mensajes,strlen(mensajes))==-1) perror("");
        //// FICHERO
        fd = open("contador.txt", O_RDWR | O_CREAT); // Abrimos el archivo
        if(fd == -1) perror("Error al crear archivo: ");
        if (read(fd,&contador1,sizeof(contador1))==-1) perror("Error al leer el archivo: ");
        lseek(fd,0,SEEK_SET);
        contador1--;
        if (write(fd,&contador1,sizeof(contador1))==-1) perror("Error al escribir en archivo: "); //Escribe un 0
        lseek(fd,0,SEEK_SET);
        lockf(fd,F_LOCK,0);
        if (close(fd) < 0) perror("Error al cerrar el archivo"); //Cierra el archivo
        ////////////
      }
    }
    //// FICHERO
    fd = open("contador.txt", O_RDWR | O_CREAT); // Abrimos el archivo
    if(fd == -1) perror("Error al crear archivo: ");
    lseek(fd,0,SEEK_SET);
    lockf(fd,F_LOCK,0);
    if (read(fd,&contador1,sizeof(contador1))==-1) perror("Error al leer el archivo: ");
    if (close(fd) < 0) perror("Error al cerrar el archivo"); //Cierra el archivo
    ////////////
    if(contador1 > 1) exit(0);
    for (contador1; contador1 < arg1;) {
      sprintf(mensajes,"   COMIENZA EL FOR\n");
      if(write(1,mensajes,strlen(mensajes))==-1) perror("");
      if(modo==0){
        srand(time(NULL));
        sleep(rand() % 5 + 1);
      }
      child = fork();
      if (child < 0){
          //Hijo no se ha creado correctamente
          perror("Fork() error ");
          exit(1);
      }
      else if (child ==  0) {
        //Hijo creado correctamente
        sprintf(mensajes,"V [\e[1;32m%d\e[0m] P [\e[1;33m%d\e[0m].\n",getpid(),getppid());
        if(write(1,mensajes,strlen(mensajes))==-1) perror("");
        if(getpid() != pid_inicial){
          //// FICHERO
          fd = open("contador.txt", O_RDWR | O_CREAT); // Abrimos el archivo
          if(fd == -1) perror("Error al crear archivo: ");
          if (read(fd,&contador1,sizeof(contador1))==-1) perror("Error al leer el archivo: ");
          lseek(fd,0,SEEK_SET);
          contador1++;
          if (write(fd,&contador1,sizeof(contador1))==-1) perror("Error al escribir en archivo: "); //Escribe un 0
          lseek(fd,0,SEEK_SET);
          lockf(fd,F_LOCK,0);
          if (close(fd) < 0) perror("Error al cerrar el archivo"); //Cierra el archivo
          ////////////
          sprintf(mensajes,"El contador vale %d cuando se crea el primer hijo después de que se abra el archivo, PID: %d\n",contador1,getpid());
          if(write(1,mensajes,strlen(mensajes))==-1) perror("");
        }
        //El hijo termina
        srand(time(NULL));
        if(rand() % 2){
          //Aqui para cuando siga creando hijos
          //El proceso aquí ejecuta lo que sea.
        }
        else{
          //Aquí para cuando muera
          raise(SIGTERM);
          if(getpid() != pid_inicial){
            //// FICHERO
            fd = open("contador.txt", O_RDWR | O_CREAT); // Abrimos el archivo
            if(fd == -1) perror("Error al crear archivo: ");
            if (read(fd,&contador1,sizeof(contador1))==-1) perror("Error al leer el archivo: ");
            lseek(fd,0,SEEK_SET);
            contador1--;
            if (write(fd,&contador1,sizeof(contador1))==-1) perror("Error al escribir en archivo: "); //Escribe un 0
            lseek(fd,0,SEEK_SET);
            lockf(fd,F_LOCK,0);
            if (close(fd) < 0) perror("Error al cerrar el archivo"); //Cierra el archivo
            ////////////
          }
          sprintf(mensajes, "El contador vale %d cuando el proceso se muere, PID: %d\n",contador1,getpid());
          if(write(1,mensajes,strlen(mensajes))==-1) perror("");
          sprintf(mensajes,"V [\e[1;35m%d\e[0m] P [\e[1;36m%d\e[0m].\n",getpid(),getppid());
          if(write(1,mensajes,strlen(mensajes))==-1) perror("");
        }
      }
      else{
        //Este es el proceso padre. Se llega aquí cuando sus hijos han muerto.
        wait(NULL);
        //AQUI SALTA LA MANEJADORA DEL SIGCHILD
      }
      //Cuando un hijo muere, el padre haga un wait sin bloquearse.
      wait(&estado_retorno); //Solo el padre espera
    } // Fin del for
  } // Fin del while
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
  void salir(int signo){}
/*-----------------------------------*/

/*-----------------------------------*/
  void muerte_total(int retorno){
/*-----------------------------------*/
    char mensajes[37];
    if(getppid() == PID_BASH){
      sprintf(mensajes,"\nPrograma acabado correctamente.\n");
      if(write(1,mensajes,strlen(mensajes))==-1) perror("");
      exit(0);
    }
  }


/*-----------------------------------*/
  void muerte_hijo(int retorno){
/*-----------------------------------*/
    char mensajes[37];
    int contador1;
    //AQUI PUEDE HABER LUGAR A CONFUSION
    sprintf(mensajes,"V [\e[1;31m%d\e[0m] P [\e[1;33m%d\e[0m].\n",getpid(),getppid());//EL HIJO ES UN HIJOFRUTA
    if(write(1,mensajes,strlen(mensajes))==-1) perror("");
    //// FICHERO
    int fd = open("contador.txt", O_RDWR | O_CREAT); // Abrimos el archivo
    if(fd == -1) perror("Error al crear archivo: ");
    if (write(fd,&contador1,sizeof(contador1))==-1) perror("Error al escribir en archivo: "); //Escribe un 0
    lseek(fd,0,SEEK_SET);
    lockf(fd,F_LOCK,0);
    if (close(fd) < 0) perror("Error al cerrar el archivo"); //Cierra el archivo
    ////////////
    sprintf(mensajes,"\nP [\e[1;33m%d\e[0m]\n\n",getpid());
    if(write(1,mensajes,strlen(mensajes))==-1) perror("");
  }
