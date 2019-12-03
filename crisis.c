#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>

const int MIN_PROCS = 3;
const int MAX_PROCS = 33;

/* -- PROTOTIPOS -- */
  void uso();
  int isNumber(char number[]);
/* ---------------- */

/* ----------------------------------- */
  int main(int argc, char *argv[]){
/* ----------------------------------- */
  setlocale(LC_ALL, "spanish");
  char velocidad[2][7] = {"normal", "veloz"};

  if(argc != 2){
    if(argc == 1){
      perror(" ERROR: no se ha recibido el argumento necesario. (Numero de procesos)");
      uso();
    }
    else if(argc > 3){
        perror(" ERROR: se han recibido demasiados argumentos. (Minimo 1, maximo 2).");
        uso();
    }
  }
  else if (argc == 2 || argc == 3){
    // Comprobaciones del segundo argumento
    if(isNumber(argv[1]) == 1){
      perror("\n ERROR: el argumento recibido no es un número.");
      uso();
    }
    else if(argv[1] < MIN_PROCS && argv[1] > MAX_PROCS){
        perror(" ERROR: el número de procesos no está en el rango de valores posibles.");
        uso();
    }
    if(argc == 3){
      //Si hay 3 argumentos
      if(isNumber(argv[1]) == 0){
        perror("\n ERROR: el argumento <velocidad> es un número.");
        uso();
      }
      else if(strcmp(velocidad[0],tolower(argv[2])) != 0 || strcmp(velocidad[1],tolower(argv[2])) != 0){
        perror(" ERROR: el valor para la velocidad es incorrecto.");
        uso();
      }
    }
  }
}

/* --------------- */
  void uso(){
/* --------------- */
    printf("\n USO: ./crisis <numero de procesos> [<velocidad>]");
    printf("\n El número de procesos debe ser un NÚMERO ENTERO entre %d y %d.", MIN_PROCS, MAX_PROCS);
    printf("\n La velocidad puede ser 'normal' o 'veloz'.\n");
}

/* ------------------------------------- */
  int isNumber(char number[]){
/* ------------------------------------- */
    int i = 0;
    if (number[0] == '-')
      i = 1;
    for (; number[i] != 0; i++){
        if (!isdigit(number[i]))
          return 1;
    } return 0;
}
