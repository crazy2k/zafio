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

    digraph foo {
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
