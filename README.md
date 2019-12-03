# Crisis

## Enunciado.

El programa que hay que presentar constará de un único fichero fuente de nombre `crisis.c`
La correcta compilación de dicho programa, producirá un fichero ejecutable, cuyo nombre será obligatoriamente crisis.
Respetad las mayúsculas/minúsculas de los nombres, si las hubiere.

La ejecución del programa creará una serie de procesos que simularán la vida de unas empresas.
El proceso padre representará a una empresa matriz que crece generando filiales y filiales de estas filiales (hijos, nietos, etc.).
Todas las empresas, menos la empresa matriz, pueden quebrar.

La invocación de la práctica se hará con dos argumentos obligatorios:`crisis n_max_procs [velocidad]`
`n_max_procs` es el número máximo de procesos que puede llegar a haber en cada momento de esa ejecución de la práctica (incluido el padre).

Dicho argumento es un número entero comprendido entre 3 y 33. El segundo argumento es opcional y podrá valer normal o veloz.
Si no se especifica este argumento, se entiende que su valor es normal.
La diferencia estriba en que, a velocidad veloz, no se debe ejecutar ninguna pausa por parte de los procesos, aunque se indiquen en el enunciado.
Esto es, a esa velocidad, no se invoca a sleep nunca.

En un momento de ejecución de la práctica, la situación de procesos podría ser:
```
                    .                     .---.                  .
                    .   Matriz(PADRE) ____X P X____              .
                    .                /    '---'    \             .
                    .               /       |       \            .
                    .              /        |        \           .
                    .          .---.      .---.      .---.       .
                    . Filiales X H X      X H X      X H X       .
                    .          '---'      '---'      '---'       .
                    .            |         / \                   .
                    .            |        /   \                  .
                    .          .---.   .---. .---.               .
                    .          X N X   X N X X N X    Subfiliales.
                    .          '---'   '---' '---'               .
```

La creación y destrucción de empresas obedecerá las siguientes reglas:

- La empresa matriz (proceso padre) nunca quiebra.
- Si una empresa quiebra, quiebran todas sus descendientes.
- Aunque, de vez en cuando, una empresa genera una filial (proceso hijo), en ningún momento puede haber más de n_max_procs procesos.

Para que los procesos puedan comunicarse, se debe usar un único fichero, cuyo formato es libre. Este fichero le servirá a un proceso, una vez ha decidido crear una filial, saber si puede crearla o no, es decir, si creándola se superaría el número máximo de procesos o no.

## Funcionamiento de cada proceso

Durante el período de funcionamiento de la práctica, los procesos estarán en el siguiente bucle infinito:

- Si estamos en el modo normal, duerme al azar entre 1 y 4 segundos. Se puede usar la función sleep para ello. Si estamos en el modo veloz, no hace nada.
- Si somos el proceso padre, generamos una filial, siempre que no se supere el número máximo de procesos.
- En otro caso, generamos una filial o nos morimos, al azar, con una probabilidad del 50% de cada suceso.
- En el caso de que la empresa muera, deben morir todos sus descendientes.
- Si un proceso que ha decidido tener un hijo no puede hacerlo porque se superaría el máximo número de procesos permitidos, repite otra vez desde el punto primero.

#### Matar hijos

La manera más fácil de lograr que se mueran los descendientes es registrar la señal SIGTERM de modo que cuando se mata a un proceso con ella, antes de morir, envía señales SIGTERMs a sus descendientes.

El proceso habrá tenido la precaución de guardar el PID de sus hijos cuando hizo fork para crearlos.

En el fichero compartido se puede escribir el número de procesos vivos en la actualidad.

Pero hay que tener cuidado con los problemas de concurrencia.

Por ejemplo, al crear una filial, hay primero que comprobar que podemos y, de ser el caso, incrementar el número de empresas vivas. 

Puede ocurrir que, una vez hemos comprobado que podemos crear la empresa y antes de incrementar su número, nos quiten la CPU y otro proceso también cree otra empresa y resulte que sobrepasemos el número máximo.

Para evitar esto, usaremos la llamada al sistema lockf para bloquear el fichero mientras trabajemos con él.

El proceso padre escribirá, antes de tener ningún hijo el número máximo de procesos que se le ha pasado por línea de órdenes.

#### Nacimiento hijos

Cuando nace un proceso, nada más nacer, imprime en la salida estándar una V y su PID entre paréntesis y un salto de línea.
Justo antes de morir, debe imprimir por la salida estándar una M y su PID entre paréntesis y un salto de línea.
Por ejemplo, si nace un proceso de PID 1234, que muere a continuación, la salida sería:
```
V(1234)
M(1234)
```
Respetad este formato exactamente, pues es posible que se use un programa de corrección automática.

Para que el buffer intermedio usado por printf no interfiera con la salida de los procesos, es importante usar write para la salida por pantalla en su lugar.

__Finalización ordenada__:

La práctica acabará cuando el usuario pulse CTRL-C. Los procesos deben morir y el padre, una vez hayan muerto todos imprimirá la frase: "Programa acabado correctamente".

## Restricciones

- Se deberán usar llamadas al sistema siempre que sea posible, a no ser que se especifique lo contrario.
- No está permitido usar la función de biblioteca system, salvo indicación explícita en el enunciado de la práctica.
- No se puede suponer que los PIDs de los procesos de una ristra van a aparecer consecutivos. Puestos en plan exquisito, ni siquiera podemos suponer que estarán ordenados de menor a mayor (puede ocurrir que se agoten los PIDs y se retome la cuenta partiendo de cero).
- No está permitido el uso de ficheros, tuberías u otro mecanismo externo para transmitir información entre los procesos, salvo que se indique en el enunciado.
- Supondremos un límite máximo de procesos igual a 33. Este límite os puede servir para no tener que usar memoria dinámica si no lo deseáis.

## Plazo de presentación.

Consultad la entrada de la página web de la asignatura.

## Normas de presentación.

Las tareas que tiene que realizar el programa son variadas. Os recomendamos que vayáis programándolas y comprobándolas una a una. 

No es muy productivo hacer todo el programa de seguido y corregir los errores al final. El esquema que os recomendamos seguir para facilitaros la labor se os muestra a continuación:

  - Haced un pequeño programa al que se le pase los argumentos que se especifican en el enunciado. Imprimid los argumentos para depurar y considerad las opciones de error al meterlos. Una vez controlados, comentad la depuración e imprimid el número máximo de procesos, como dice el enunciado.
  - Poned al padre en un bucle infinito en el que duerma un segundo y cree un hijo. Para no desbordar el ordenador con procesos, controlad con una variable cuántos se han creado y no crear ninguno si el número total es superior al número máximo. Dejad a los hijos en pauses.
  - Haced que el padre duerma un número de segundos al azar entre uno y cinco. Usad las funciones srand y rand. Hacedlo de modo que varíen los números al azar en cada ejecución.
  - Sustituid la variable de control por un fichero. Tened cuidado, porque si no lo hacéis bien, no controlaréis los procesos creados y se os pueden descontrolar.
  - Registrad SIGINT de modo que el padre imprima el mensaje de finalización si pulsáis CTRL-C.
  - Haced que los hijos, al nacer impriman el mensaje de su PID. También registrad en ellos SIGINT de modo que impriman el mensaje con su PID al morir.
- Programad ahora la muerte de los hijos.
  - Metedlos en un bucle infinito, haced que duerman al azar como el padre y echad otro número al azar para ver si mueren. Si tienen que morir, que se autoenvíen SIGTERM.
  - Registrad una manejadora para SIGTERM para los hijos para que decrementen el número de procesos del fichero e impriman su PID al morir.
  - En estos momentos, los procesos mueren y el padre crea a otros nuevos, sin nunca superar el máximo. Pero hay un problema, los hijos muertos se quedan zombies. Debéis evitarlo. Una manera es registrando la señal SIGCLD en los procesos para que, cuando muera un hijo, el padre pueda hacer un wait sin bloquearse. Aunque hay más posibles soluciones.
  - Llega el tiempo de que los hijos tengan también hijos si no mueren y si no se ha superado el máximo. Hay que hacerlo muy similarmente a como lo hace el padre.
  - Hay ahora que programar que un proceso al morir debe matar a todos sus descendientes. Lo más fácil es hacerlo recursivamente. Los hijos tendrán la precaución de apuntar el PID de sus hijos cuando nacen y borrarlos de esa lista cuando mueren. Antes de morir ellos, en la manejadora, manda SIGTERMs a todos sus hijos vivos.
  - Programad el modo de alta velocidad. Al ejecutarlo en dicho modo, puede que se viole la condición del límite máximo de procesos por un problema de concurrencia. Debéis colocar los bloqueos de fichero adecuadamente para que no se puedan llegar a producir nunca.
- Repasad el enunciado y acabad de pulir los últimos detalles.
  - No se puede usar sleep() o similares para sincronizar los procesos. Hay que usar otros mecanismos.
  - Sabéis que si usáis espera ocupada en lugares donde explícitamente no se haya dicho que se puede usar, la práctica está suspensa. No obstante, existe una variedad de espera ocupada que podríamos denominar espera semiocupada. Consiste en introducir una espera de algún segundo en cada iteración del bucle de espera ocupada. Con esto el proceso no consume tanta CPU como en la espera ocupada, pero persisten los demás problemas de la técnica del sondeo, en particular el relativo a la elección del periodo de espera. Aunque la práctica no estará suspensa si hacéis espera semiocupada, se penalizará en la nota bastante si la usáis. En conveniente que la evitéis.
  - Evitad, en lo posible, el uso de variables globales. Tenéis la posibilidad de declarar variables estáticas.
  - Tened cuidado con el uso de pause(). Salvo en bucles infinitos de pauses, su uso puede estar mal. Mirad la solución a la práctica propuesta en la sesión décima acerca de él o el siguiente LPE.
- El programa que he hecho se para a veces. O en mi casa se para, pero en clase, no. O en clase sí, pero en casa, no.
  - Solución: Suele provenir de un mal uso de pause
- ¿Qué hago cuando mi programa se desboca para no perjudicar el funcionamiento de la máquina?
  - Solución: Aunque pueda parecer que necesitáis manejar listas enlazadas u otras estructuras de datos de tamaño variable, en realidad considerad que no puede haber vivos más de treinta y tres procesos a la vez... Debéis tener cuidado con un efecto que se produce por el hecho de que los descriptores de fichero heredados comparten un único puntero de fichero. Si cualquiera de los procesos lo mueve con lseek el resto lo ve movido. Este efecto secundario hace que el siguiente código sea erróneo para tratar de bloquear el fichero:
    ```
    lseek(fd,0,SEEK_SET);
    lockf(fd,F_LOCK,0);
    ```
    La razón es que entre las dos instrucciones se puede perder la CPU y otro proceso nos puede mover el puntero que nosotros pensamos que está al principio. La solución pasa por no usar los descriptores heredados sino que cada hijo, al nacer haga algo similar a:
    ```
    case 0: /* COdigo del hijo */
      close(fd);   // Cerramos el descriptor heredado
      fd=open(...  // Lo volvemos a abrir
    ```
    En el LPE anterior es evidente que el open que hace el hijo no puede llevar O_TRUNC. Si lo lleva, cuandoquiera que nazca un hijo, borrará el fichero pudiendo pillar justo antes de una lectura, que no leería nada. Lo debe abrir solamente con el flag O_RDWR

