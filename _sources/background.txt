.. default-role:: math

Un poco de *background*
=======================

La idea de este documento no es brindar explicaciones exhaustivas ni
servir de referencia absoluta de ninguno de los temas necesarios para
poder encarar un proyecto como este. Se pretende, simplemente, ofrecer
un material de apoyo a otros documentos orientado a las necesidades de
este proyecto.

En consecuencia, se asume que el lector tiene conocimientos previos
acerca de la arquitectura IA-32, que ha tenido cierta experiencia
previa programando en algún dialecto de *assembly* para dicha
arquitectura y que está dispuesto a buscar en otros textos lo que no
pueda encontrar aquí.

La arquitectura en la que se basará este proyecto es la arquitectura
IA-32 de Intel, por lo que todos los detalles de bajo nivel se
orientarán a esta.

La secuencia de arranque
------------------------

En las computadoras compatibles con IBM PC, luego de encender la
máquina, tiene lugar la carga y ejecución de un programa, que reside en
una memoria especial de la computadora, llamado BIOS (*Basic
Input/Output System*). Generalmente, el BIOS realiza chequeos y/o
inicializaciones en el hardware y, si todo va bien, busca un
dispositivo del que pueda arrancar, siguiendo un orden de arranque que
especifica con qué dispositivo debe intentarse arrancar primero.

Para decidir si puede arrancar de un dispositivo [1]_, el BIOS verifica si
los últimos dos bytes del primer sector del dispositivo contienen la
firma ``0xAA55``. Si encontró un dispositivo del que puede arrancar,
entonces copia el primer sector de este a la memoria principal, en la
dirección física ``0x7C00``, y comienza a ejecutar desde esa dirección.

.. [1] En realidad, esta explicación es válida para dispositivos como
    discos floppy o discós rígidos. Los CDs tienen otra estructura para
    los sectores de arranque, y esta explicación no los contempla.

La arquitectura
---------------

Modos de operación
~~~~~~~~~~~~~~~~~~

La arquitectura IA-32 tiene los siguientes modos de operación:

* **Modo real**: El modo real preserva el entorno del viejo Intel 8086,
  con algunas extensiones, como las que permiten un cambio de modo. Es
  el modo en el cual arranca cualquier máquina de esta arquitectura.
  
  En este modo, los únicos registros de propósito "general" disponibles
  son ``AX``, ``BX``, ``CX``, ``DX``, ``BP``, ``SI``, ``DI`` y ``SP``,
  todos de 16 bits.

  Para el acceso a memoria, se utiliza segmentación. Las direcciones se
  resuelven a partir de un registro de `segmento` y un `offset` a través
  del siguiente cálculo: `(segmento \cdot 16) + offset`. Es decir, los
  registros de segmento sólo contienen información acerca del comienzo
  de un segmento, pero no imponen un límite ni permisos. El espacio de
  direcciones en modo real es de `2^{20}` direcciones.

* **Modo protegido**: El modo protegido es "el modo de operación nativo
  del procesador" de la arquitectura IA-32, según los manuales de
  Intel. Este modo permite trabajar con un espacio de direcciones mucho
  mayor y, en procesadores modernos, brinda mecanismos de protección de
  memoria y anillos/niveles de ejecución.

  En modo protegido se cuenta con los registros de próposito "general"
  de 32 bits ``EAX``, ``EBX``, ``ECX``, ``EDX``, ``ESI``, ``EDI``,
  ``EBP`` y ``ESP``.

  Los mecanismos de protección de memoria disponibles son segmentación
  y páginación. Para la primera, los registros de segmento representan
  selectores de segmento y proveen más información que los registros de
  segmento en modo real.

  Existe un pseudo-modo **virtual-8086**, que de momento no interesa.

* **Modo de "gestión del sistema"**: Por ahora no me interesa, pero
  básicamente es un modo que permite atender mensajes o alarmas de las
  que el sistema avisa a través de interrupciones.

El siguiente gráfico muestra, para los modos mencionados, las maneras
de pasar de uno a otro:

.. graphviz::

    digraph modos {
        "Modo real" -> "Modo protegido" [label="PE=1"];
        "Modo real" -> "Modo de 'gestión del sistema'" [label="SMI#"];
        "Modo protegido" -> "Modo real" [label="Reset o PE=0"];
        "Modo protegido" -> "Modo de 'gestión del sistema'"
            [label="SMI#"];
        "Modo de 'gestión del sistema'" -> "Modo real"
            [label="Reset o RSM"]
        "Modo de 'gestión del sistema'" -> "Modo protegido"
            [label="RSM"]
    }

Administración de memoria
~~~~~~~~~~~~~~~~~~~~~~~~~

Segmentación
~~~~~~~~~~~~

.. La administración de memoria mediante segmentación se implementó desde
.. hace mucho tiempo atrás en la familia de procesadores de Intel. En los
.. 8086 de Intel (que vieron el mercado en 1978), la segmentación, no
.. obstante, fue implementada de una manera rudimentaria, únicamente con
.. los fines de acceder a posiciones de memoria cuya dirección no cabía en
.. los registros del procesador. Por esos tiempos, en la arquitectura de
.. Intel, no se utilizaba la segmentación como un mecanismo para proteger
.. espacios de memoria.

.. Con la aparición del procesador 80286 (1982) --- inclusión del modo
.. protegido --- se hizo posible la definición de los tamaños y
.. privilegios asignados a cada uno de los segmentos. Así, la segmentación
.. pudo utilizarse para realizar protección de memoria, es decir,
.. restringir el acceso de determinadas tareas a ciertos sectores de la
.. memoria, logrando así, por ejemplo, que las aplicaciones no puedan
.. acceder o modificar datos pertenecientes al sistema operativo.

.. Sin embargo, en una gran cantidad de sistemas operativos modernos
.. (también es el caso de JOS), la segmentación fue dejada de lado como
.. mecanismo de protección de memoria. Se utiliza, para esto, únicamente
.. la paginación.

.. No obstante, la segmentación no puede ser desactivada en la
.. arquitectura IA-32, en ninguno de sus modos. ¿Cómo hacen entonces los
.. sistemas operativos modernos (que no precisan de la segmentación) para
.. desactivarla? En realidad, no la desactivan. Utilizan algo llamado
.. “Modelo flat de segmentación”, que consiste en ubicar a todos los
.. segmentos ocupando todo el espacio que se pretende direccionar, desde
.. la dirección cero. De este modo, la dirección (la parte del offset,
.. específicamente) virtual (la utilizada en el código) coincide con la
.. dirección lineal (la que toma como entrada el módulo de paginación).

Paginación
~~~~~~~~~~

.. La verdadera protección de memoria en JOS (y en muchos sistemas
.. operativos modernos) se da gracias al mecanismo de paginación. La
.. paginación se caracteriza por organizar la memoria física en bloques de
.. tamaño fijo, no solapados, llamados marcos de página.

.. Observemos las distintas etapas por las cuales pasa una dirección
.. virtual hasta que se convierte en una dirección física:

.. .. graphviz::

..     digraph direcciones {
..         "dirección virtual" [shape=box] -> "dirección lineal"
..             [label="unidad de segmentación"];
..         "dirección lineal" -> "dirección física"
..             [label="unidad de paginación"];
..     }

.. La primer traducción la realiza la unidad de segmentación, mientras
.. que la segunda traducción es realizada por la unidad de paginación.

.. En la arquitectura IA-32 la paginación puede activarse una vez hecho el
.. cambio a modo protegido. Cuando se encuentra activada, y se utilizan
.. páginas de 4KB, la dirección lineal es dividida en tres partes por la
.. unidad de paginación:

.. +------------------------------------+-------------------------------+---------+
.. | índice en el directorio de páginas | índice en la tabla de páginas | offset  |
.. +====================================+===============================+=========+
.. | (10 bits)                          | (10 bits)                     | 12 bits |
.. +------------------------------------+-------------------------------+---------+

.. El primero de los tres campos representa un índice en el directorio de
.. páginas. El directorio de páginas es una tabla que contiene 2^{10}
.. entradas (una por cada índice posible). Cada entrada, además de varios
.. atributos, contiene la dirección física de una tabla de páginas. El
.. sistema de paginación utiliza el primer campo para seleccionar una de
.. las entradas en el directorio de páginas (PDEs). Consecuentemente, se
.. obtendrá la dirección física de la tabla de páginas asociada a dicha
.. entrada.

.. El segundo campo es utilizado, entonces, para elegir una de las
.. `2^{10}` entradas de la tabla de páginas mencionada. Las entradas en
.. la tabla de páginas (PTEs) contienen, además de varios atributos, la
.. dirección física de una página en memoria. El offset es utilizado para
.. seleccionar uno de los bytes en dicha página.

Referencia de NASM
------------------

Directivas del preprocesador
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. describe:: define A x
              define f(x1, x2, ...) y

    Permite definir macros en una sola línea. Funciona de manera
    similar a las macros de C, por lo que también posibilita crear
    macros con "parámetros".


Directivas del ensamblador
~~~~~~~~~~~~~~~~~~~~~~~~~~

.. describe:: BITS m

    Indica que el código a continuación de la directiva debe generarse
    para procesadores operando en modo de ``m`` bits, donde ``m``
    puede ser 16, 32 o 64.

    Para el modo de salida ``bin``, que es el que usamos para el
    *bootloader*, se puede asumir por omisión que ``m`` es 16. Sin
    embargo, siempre es buena práctica ser explícitos con esto, en
    lugar de asumir valores por omisión.

.. describe:: ORG addr

    Hace que NASM asuma que la dirección de memoria ``addr`` es la
    dirección en la que ha sido cargado el programa. NASM utiliza esta
    dirección como base para todas las referencias internas en una
    sección de código.
