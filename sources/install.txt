.. default-role:: math

Instalación y uso
=================

Compilación
-----------

Antes de realizar la compilación de Zafio, es preciso preparar los
programas que se pretende ejecutar una vez que el sistema sea cargado.

El directorio ``user/`` contiene tres programas de ejemplo en los
subdirectorios ``shell/``, ``hello/`` y ``loop/`` respectivamente. El
programa en ``shell/`` es un intérpete de comandos que permite, entre
otras cosas, comenzar la ejecución de nuevas tareas y examinar el
estado del sistema. El que se halla en ``hello/`` es un programa
sencillo que pide al usuario que ingrese su nombre para luego mostrarlo
por pantalla. El programa en ``loop/`` es simplemente un ciclo
infinito.

Como se puede observar, estos directorios contienen un archivo con
código C (y extensión ``.c``), que contiene el código del programa, y
un enlace simbólico a un archivo Makefile que se halla en ``user/``.

Para realizar la compilación de los programas y del kernel en un solo
paso, puede ejecutarse lo siguiente en el shell::

    $ make clean && make install_user_progs && make

Ejecución
---------

Si la compilación tuvo éxito, en ``reftest/`` (el directorio destinado
a las pruebas de referencia) debería encontrarse una imagen lista para
ser arrancada usando Bochs. En ese mismo directorio, también, se
encuentra listo un archivo de configuración de Bochs, ``bochsrc``, por
lo que alcanza con ejecutar el comando ``bochs -q`` para iniciar el
emulador y arrancar Zafio.

Uso
---

El caso común es que el Zafio se encuentre configurado para ejecutar el
programa ``shell`` al inicio. En ese caso, ni bien finalice la carga
del sistema aparecerá el *prompt* del shell::

    $

Escribiendo ``help`` y presionando ``ENTER`` puede obtenerse la ayuda del
shell::

    $ help
    Comandos disponibles:
    help                muestra esta ayuda
    ls                  muestra la lista de programas disponibles
    ps                  muestra datos sobre los procesos del sistema
    run <programa>      ejecuta un programa en foreground
    bg <programa>       ejecuta un programa en background
    nice <pid> <valor>  cambia la prioridad de un proceso
    kill <pid>          mata un proceso
    echo <palabra>      imprime una palabra

El mensaje de ``help`` es bastante descriptivo. ``<programa>``
representa el nombre de un programa (como los que se leen en la salida
de ``ls``), ``<pid>`` es el número de identificación de una tarea
(pueden obtenerse usando ``ps``) y ``<valor>``, en el caso de ``nice``,
indica el valor del quantum que se quiere asignar a la tarea
indicada. ``<palabra>`` no es más que una cadena de caracteres sin
espacios.

Es importante tener en cuenta la diferencia entre ``run`` y ``bg``. El
primero ejecuta un programa en *foreground*, es decir, hace que el
shell libere la terminal, ponga en ejecución la tarea, y se bloquee
hasta que finalice la tarea creada, para luego sí tomar nuevamente la
terminal. El segundo ejecuta un programa en *background*, por lo que el
shell retiene la terminal y sigue pudiendo ejecutarse mientras lo hace
la nueva tarea.
