// 2º Ing. Informática - SSOO II
// Francisco Javier Gallego Lahera (A1)
// Pablo Jesús González Rubio (A3)

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
#include <fcntl.h>
#include <errno.h>

#define PID_BASH getpid()
pid_t PIDS[33];
pid_t HIJO;

/* ------------------------------ PROTOTIPOS ------------------------------ */
  int isNumber(char number[]);
  void uso(char mensajes[],int MIN_PROCS,int MAX_PROCS);
  void salir(int retorno);
  void muerte_total(int retorno);
  void muerte_hijo(int signum);
/* ------------------------------------------------------------------------ */

/* ----------------------------------- */
  int main(int argc, char *argv[]){
/* ----------------------------------- */
  setlocale(LC_ALL, "spanish");
  const int MIN_PROCS = 3;
  const int MAX_PROCS = 33;
  char velocidad[2][7] = {"normal", "veloz"};
  char mensajes[300];
  pid_t child;
  int estado_retorno;
  int modo,i,arg1;

  struct sigaction sigint;
  sigint.sa_handler=muerte_total;
  if(sigaction(SIGINT,&sigint,NULL)==-1) return -1;

  if (argc == 2 || argc == 3){
    arg1 = atoi(argv[1]);

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
      if(strcmp(argv[2], "normal") == 0){
        modo = 0;
      } else if(strcmp(argv[2], "veloz") == 0){
        modo = 1;
      } else {
        perror(" ERROR: el argumento <velocidad> es 'normal' o 'veloz'.");
        uso(mensajes,MIN_PROCS, MAX_PROCS);
        exit(1);
      }
    }
  }
  else{
    uso(mensajes,MIN_PROCS, MAX_PROCS);
    exit(1);
  }

  sprintf(mensajes,"[DEBUG] %d procesos\n",arg1);
  if(write(1,mensajes,strlen(mensajes))==-1) perror("");

  pid_t pid_inicial = getpid();
  sprintf(mensajes,"Soy el padre de todos [%d]\n\n",pid_inicial);

  if(write(1,mensajes,strlen(mensajes))==-1) perror("");

  int contador = 0;
  int fd = open("contador.txt", O_RDWR | O_CREAT, 0777);

  if(fd == -1) perror("Error al crear archivo: ");

  if (write(fd,&contador,sizeof(contador))==-1) perror("Error al escribir en archivo: ");

  if (close(fd) < 0) perror("Error al cerrar el archivo 1");

  wait(NULL);

  while(1){
    if(contador >= arg1){

      if(getpid() != pid_inicial){
        pid_t pid_a_matar=0;
        while(PIDS[pid_a_matar] == 0 || PIDS[pid_a_matar] == 1){
          srand(time(NULL));
          pid_a_matar = rand() % (arg1-1) + 1;
        } kill(PIDS[pid_a_matar],SIGTERM);
        fd = open("contador.txt", O_RDWR);
        if(fd == -1) perror("Error al crear archivo: ");

        lseek(fd,0,SEEK_SET);
        if(lockf(fd,F_LOCK,0)==0){
          if (read(fd,&contador,sizeof(contador))==-1) perror("Error al leer el archivo: ");
          lseek(fd,0,SEEK_SET);
          contador--;

          if (write(fd,&contador,sizeof(contador))==-1) perror("Error al escribir en archivo: ");
          lseek(fd,0,SEEK_SET);
          lockf(fd,F_ULOCK,0);
        }
        if (close(fd) < 0) perror("Error al cerrar el archivo 2");
        wait(NULL);
      }
    }

    for (; contador < arg1;) {
      if(modo==0){
        srand(time(NULL));
        sleep(rand() % 5 + 1);
      }
      for (i = 0; i < arg1; i++) {
        if(kill(PIDS[i],0) == -1) PIDS[i] = 0;

      }

      int fd = open("contador.txt", O_RDWR);

      if(fd == -1) perror("Error al crear archivo: ");
      lseek(fd,0,SEEK_SET);

      if(lockf(fd,F_LOCK,0)==0){
        child = fork();
        HIJO = getpid();
        PIDS[contador] = HIJO;
        if (child < 0){
            perror("Fork() error ");
            exit(1);
        }
        else if (child ==  0) {
          sprintf(mensajes,"\nV [\e[1;32m%d\e[0m] P [\e[1;33m%d\e[0m]\n",getpid(),getppid());
          if(write(1,mensajes,strlen(mensajes))==-1) perror("");
          if(getpid() != pid_inicial){
            fd = open("contador.txt", O_RDWR);
            if(fd == -1) perror("Error al crear archivo: ");
            lseek(fd,0,SEEK_SET);
            if(lockf(fd,F_LOCK,0)==0){
              if (read(fd,&contador,sizeof(contador))==-1) perror("Error al leer el archivo: ");
              lseek(fd,0,SEEK_SET);
              contador++;
              if (write(fd,&contador,sizeof(contador))==-1) perror("Error al escribir en archivo: ");
              lseek(fd,0,SEEK_SET);
              lockf(fd,F_ULOCK,0);
            }
            if (close(fd) < 0) perror("Error al cerrar el archivo 3");
            wait(NULL);
          }
          srand(time(NULL));
          if(rand() % 2){}
          else raise(SIGTERM);
        }
        lseek(fd,0,SEEK_SET);
        lockf(fd,F_ULOCK,0);
      }
      while(close(fd) == 1 && errno == EINTR) continue;
      if(close(fd) < 0 && close(fd) != -1) perror("Error al cerrar el archivo 4");

      HIJO=child;
      wait(NULL);
      struct sigaction sigchild;
      sigchild.sa_handler=muerte_hijo;

      if(sigaction(SIGCHLD,&sigchild,NULL)==-1) return -1;
      wait(&estado_retorno);
    }
  }
}


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
      sprintf(mensajes,"\nPrograma acabado correctamente.\n");
      if(write(1,mensajes,strlen(mensajes))==-1) perror("");
      raise(SIGTERM);
      exit(0);
  }

/*-----------------------------------*/
  void muerte_hijo(int signum){
/*-----------------------------------*/
  	pid_t p;
  	int status;
    char mensajes[200];
    int contador;

    sprintf(mensajes,"\nM [\e[1;31m%d\e[0m] P [\e[1;33m%d\e[0m]\n",HIJO,getpid());
    if(write(1,mensajes,strlen(mensajes))==-1) perror("");
    int fd = open("contador.txt", O_RDWR);
    if(fd == -1) perror("Error al crear archivo: ");
    lseek(fd,0,SEEK_SET);

    if(lockf(fd,F_LOCK,0)==0){
      if (read(fd,&contador,sizeof(contador))==-1) perror("Error al leer el archivo: ");
      lseek(fd,0,SEEK_SET);
      contador--;
      if (write(fd,&contador,sizeof(contador))==-1) perror("Error al escribir en archivo: ");
      lseek(fd,0,SEEK_SET);
      lockf(fd,F_ULOCK,0);
    }

    if (close(fd) < 0) perror("Error al cerrar el archivo 5");
    wait(NULL);
  }
