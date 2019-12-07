#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>

/* -------------------------- PROTOTIPOS ----------------------- */
  int isNumber(char number[]);
  void uso(char mensajes[],int MIN_PROCS,int MAX_PROCS);
  unsigned int dormir(unsigned int nsecs);
  static void sig_alrm(int signo);
  static void muerto(int signo);
/* ------------------------------------------------------------- */

/* ----------------------------------- */
  int main(int argc, char *argv[]){
/* ----------------------------------- */
  setlocale(LC_ALL, "spanish");
  //VARIABILITAS
  const int MIN_PROCS = 3;
  const int MAX_PROCS = 33;
  char velocidad[2][7] = {"normal", "veloz"};
  char mensajes[500];
  pid_t return_child;
  int arg1;

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
  int pid = getpid();
  // Luego hay que hacer free()
  pid_t *hijos = (pid_t *) malloc (arg1 * sizeof(pid_t));
  //
  printf("Soy el padre de todos PID: %d\n\n",pid);
  for (int i = 0; i < arg1; i++) {
    srand(time(NULL));
    dormir(rand() % 5 + 1);
    hijos[i] = fork();
    return_child = wait(hijos[i]);
    if (hijos[i] == -1){
        //Si no se ha creado correctamente
        perror("Fork() error ");
        exit(1);
    }
    else if (hijos[i] ==  0) {
      //Si se ha creado correctamente
      printf("Soy un \e[1;32mhijo\e[0m (\e[1;32m%d\e[0m) y mi \e[1;31mpadre\e[0m es (\e[1;31m%d\e[0m).\n",getpid(),getppid());
    }
    else if (hijos[i] != 0){
      if (signal(SIGCHLD, muerto)){
        //Esto se ejecuta cuando se han muerto todos los hijos que había creado el padre original.
        printf("Soy un \e[1;34mpadre\e[0m (\e[1;33m%d\e[0m) y mis hijos \e[1;32m↑\e[0m las han palmao todos.\n",getpid());
      }
    }
  }
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
  static void sig_alrm(int signo){}
/*-----------------------------------*/

/*----------------------------------------------*/
  static void muerto(int signo){wait(NULL);}
/*----------------------------------------------*/

/* ---------------------------------------------- */
  unsigned int dormir(unsigned int nsecs){
/* ---------------------------------------------- */
	if (signal(SIGALRM, sig_alrm) == SIG_ERR) return(nsecs);
	alarm(nsecs);
	pause();
	return(alarm(0));
}
