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
  //VARIABILITAS
  const int MIN_PROCS = 3;
  const int MAX_PROCS = 33;
  char velocidad[2][7] = {"normal", "veloz"};
  char mensajes[300];
  pid_t estado_retorno, child, HIJO;
  int modo,i,arg1;
  //SIGACTIONS
  struct sigaction sigint;
  sigint.sa_handler=muerte_total;
  if(sigaction(SIGINT,&sigint,NULL)==-1) return -1;
  //SI MUERE UN HIJO SALTA EL SIGCHILD
  //PARA QUE SE MUESTRE BIEN QUÉ HIJO HA MUERTO HAY QUE

  // COMPROBACIONES
  if (argc == 2 || argc == 3){
    // Comprobaciones del segundo argumento
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
      //Si hay 3 argumentos
      //Comprobaciones velocidad
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
  // PROGRAMA
  sprintf(mensajes,"[DEBUG] %d procesos\n",arg1);
  if(write(1,mensajes,strlen(mensajes))==-1) perror("");
  pid_t pid_inicial = getpid();
  sprintf(mensajes,"Soy el padre de todos [%d]\n\n",pid_inicial);
  if(write(1,mensajes,strlen(mensajes))==-1) perror("");
  //// PARA QUE META UN 0
  int contador1 = 0;
  int fd = open("contador.txt", O_RDWR | O_CREAT, 0777);
  if(fd == -1) perror("Error al crear archivo: ");
  lseek(fd,0,SEEK_SET);
  if(lockf(fd,F_LOCK,0)==0){
    if (write(fd,&contador1,sizeof(contador1))==-1) perror("Error al escribir en archivo: ");
    lseek(fd,0,SEEK_SET);
    lockf(fd,F_ULOCK,0);
  }
  if (close(fd) < 0) perror("Error al cerrar el archivo");
  wait(NULL);
  ////
  while(1){
    if(contador1 >= arg1){
      //Aquí para que muera uno cuando el contador llega al valor pasado
      if(getpid() != pid_inicial){
        //PARA MATAR UN VALOR ALEATORIO SI LLEGA AL MAXIMO
        pid_t pid_a_matar=0;
        while(PIDS[pid_a_matar] == 0 || PIDS[pid_a_matar] == 1 || PIDS[pid_a_matar] == -10){
          srand(time(NULL));
          pid_a_matar = rand() % (arg1-1) + 1;
        } kill(PIDS[pid_a_matar],SIGTERM); // +1 Para no matar al padre
        //// AL MATAR EL HIJO REDUCE EN UNO EL CONTADOR DEL FICHERO
        //////////////////////////////////////////////////
        fd = open("contador.txt", O_RDWR);
        if(fd == -1) perror("Error al crear archivo: ");
        lseek(fd,0,SEEK_SET);
        if(lockf(fd,F_LOCK,0)==0){
          if (read(fd,&contador1,sizeof(contador1))==-1) perror("Error al leer el archivo: ");
          lseek(fd,0,SEEK_SET);
          contador1--;
          if (write(fd,&contador1,sizeof(contador1))==-1) perror("Error al escribir en archivo: ");
          lseek(fd,0,SEEK_SET);
          lockf(fd,F_ULOCK,0);
        }
        if (close(fd) < 0) perror("Error al cerrar el archivo");
        wait(NULL);
        ////////////
      }
    }
    //// LEE EL CONTADOR
    fd = open("contador.txt", O_RDWR);
    if(fd == -1) perror("Error al crear archivo: ");
    lseek(fd,0,SEEK_SET);
    if(lockf(fd,F_LOCK,0)==0){
      if (read(fd,&contador1,sizeof(contador1))==-1) perror("Error al leer el archivo: ");
      lseek(fd,0,SEEK_SET);
      lockf(fd,F_ULOCK,0);
    }
    if (close(fd) < 0) perror("Error al cerrar el archivo");
    wait(NULL); //Cierra el archivo
    ////////////

    for (; contador1 < arg1;) {
      if(contador1 >= arg1){
        //Aquí para que muera uno cuando el contador llega al valor pasado
        if(getpid() != pid_inicial){
          //PARA MATAR UN VALOR ALEATORIO SI LLEGA AL MAXIMO
          pid_t pid_a_matar=0;
          while(PIDS[pid_a_matar] == 0 || PIDS[pid_a_matar] == 1 || PIDS[pid_a_matar] == -10){
            srand(time(NULL));
            pid_a_matar = rand() % (arg1-1) + 1;
          } kill(PIDS[pid_a_matar],SIGTERM); // +1 Para no matar al padre
          //// AL MATAR EL HIJO REDUCE EN UNO EL CONTADOR DEL FICHERO
          //////////////////////////////////////////////////
          fd = open("contador.txt", O_RDWR);
          if(fd == -1) perror("Error al crear archivo: ");
          lseek(fd,0,SEEK_SET);
          if(lockf(fd,F_LOCK,0)==0){
            if (read(fd,&contador1,sizeof(contador1))==-1) perror("Error al leer el archivo: ");
            lseek(fd,0,SEEK_SET);
            contador1--;
            if (write(fd,&contador1,sizeof(contador1))==-1) perror("Error al escribir en archivo: ");
            lseek(fd,0,SEEK_SET);
            lockf(fd,F_ULOCK,0);
          }
          if (close(fd) < 0) perror("Error al cerrar el archivo");
          wait(NULL);
          ////////////
        }
      }
      if(modo==0){
        srand(time(NULL));
        sleep(rand() % 5 + 1);
      }
      for (i = 0; i < arg1; i++) {
        if(kill(PIDS[i],0) == -1) PIDS[i] = -10; // Si es -1, entonces no hay creado un proceso en esa posición
        //No puede haber PIDs negativos
      }
      child = fork();
      HIJO = getpid();
      PIDS[contador1] = HIJO;
      if (child < 0){
          //Hijo no se ha creado correctamente
          perror("Fork() error ");
          exit(1);
      }
      else if (child ==  0) {
        //Hijo creado correctamente
        if(contador1 >= arg1){
          //Aquí para que muera uno cuando el contador llega al valor pasado
          if(getpid() != pid_inicial){
            //PARA MATAR UN VALOR ALEATORIO SI LLEGA AL MAXIMO
            pid_t pid_a_matar=0;
            while(PIDS[pid_a_matar] == 0 || PIDS[pid_a_matar] == 1 || PIDS[pid_a_matar] == -10){
              srand(time(NULL));
              pid_a_matar = rand() % (arg1-1) + 1;
            } kill(PIDS[pid_a_matar],SIGTERM); // +1 Para no matar al padre
            //// AL MATAR EL HIJO REDUCE EN UNO EL CONTADOR DEL FICHERO
            //////////////////////////////////////////////////
            fd = open("contador.txt", O_RDWR);
            if(fd == -1) perror("Error al crear archivo: ");
            lseek(fd,0,SEEK_SET);
            if(lockf(fd,F_LOCK,0)==0){
              if (read(fd,&contador1,sizeof(contador1))==-1) perror("Error al leer el archivo: ");
              lseek(fd,0,SEEK_SET);
              contador1--;
              if (write(fd,&contador1,sizeof(contador1))==-1) perror("Error al escribir en archivo: ");
              lseek(fd,0,SEEK_SET);
              lockf(fd,F_ULOCK,0);
            }
            if (close(fd) < 0) perror("Error al cerrar el archivo");
            wait(NULL);
            ////////////
          }
        }
        sprintf(mensajes,"V [\e[1;32m%d\e[0m] P [\e[1;33m%d\e[0m].\n",getpid(),getppid());
        if(write(1,mensajes,strlen(mensajes))==-1) perror("");
        if(getpid() != pid_inicial){
          //// AUMENTA EN UNO EL CONTADOR
          fd = open("contador.txt", O_RDWR);
          if(fd == -1) perror("Error al crear archivo: ");
          //Bloquea el fichero
          lseek(fd,0,SEEK_SET);
          if(lockf(fd,F_LOCK,0)==0){
            if (read(fd,&contador1,sizeof(contador1))==-1) perror("Error al leer el archivo: ");
            lseek(fd,0,SEEK_SET);
            contador1++;
            if (write(fd,&contador1,sizeof(contador1))==-1) perror("Error al escribir en archivo: ");
            // Desbloqueamos el fichero para que lo pueda escribir otro proceso
            lseek(fd,0,SEEK_SET);
            lockf(fd,F_ULOCK,0);
          }
          if (close(fd) < 0) perror("Error al cerrar el archivo");
          wait(NULL);
          ////////////
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
        }
      }
      else{
        //Este es el proceso padre. Se llega aquí cuando sus hijos han muerto.
        HIJO=child;
        wait(NULL);
        //TODO LO DE muerte_hijo ES LO QUE VENÍA AQUÍ
        struct sigaction sigchild;
        sigchild.sa_handler=muerte_hijo;
        if(sigaction(SIGCHLD,&sigchild,NULL)==-1) return -1;
        //sprintf(mensajes,"M [\e[1;31m%d\e[0m] P [\e[1;33m%d\e[0m].\n",HIJO,getpid());//EL HIJO ES UN HIJOFRUTA
        //if(write(1,mensajes,strlen(mensajes))==-1) perror("");
      }
      //Cuando un hijo muere, el padre haga un wait sin bloquearse.
      wait(&estado_retorno); //Solo el padre espera
    } // Fin del for
  } // Fin del while
} // Fin del main


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
    if(getpid() == PID_BASH){
      sprintf(mensajes,"\nPrograma acabado correctamente.\n");
      if(write(1,mensajes,strlen(mensajes))==-1) perror("");
      raise(SIGTERM);
      exit(0);
    }
  }

/*-----------------------------------*/
  void muerte_hijo(int signum){
/*-----------------------------------*/
  	pid_t p;
  	int status;
    char mensajes[200];
    int contador1;
    sprintf(mensajes,"M [\e[1;31m%d\e[0m] P [\e[1;33m%d\e[0m].\n",HIJO,getpid());//EL HIJO ES UN HIJOFRUTA
    if(write(1,mensajes,strlen(mensajes))==-1) perror("");
    //// FICHERO
    int fd = open("contador.txt", O_RDWR); // Abrimos el archivo
    if(fd == -1) perror("Error al crear archivo: ");
    lseek(fd,0,SEEK_SET);
    if(lockf(fd,F_LOCK,0)==0){
      if (read(fd,&contador1,sizeof(contador1))==-1) perror("Error al leer el archivo: ");
      lseek(fd,0,SEEK_SET);
      contador1--;
      if (write(fd,&contador1,sizeof(contador1))==-1) perror("Error al escribir en archivo: "); //Escribe un 0
      lseek(fd,0,SEEK_SET);
      lockf(fd,F_ULOCK,0);
    }
    if (close(fd) < 0) perror("Error al cerrar el archivo");
    wait(NULL); //Cierra el archivo
      ////////////
  }
