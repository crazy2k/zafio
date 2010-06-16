.. default-role:: math

Un poco de *background*
=======================

A continuación, una breve reseña de algunas de las características de
la arquitectura IA-32. Como este proyecto se va a basar en dicha
arquitectura, voy a omitir cualquier detalle relacionado con
arquitecturas de 64 bits.

Modos de operación
------------------

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
  de un segmento, pero no imponen un límite. (CHEQUEAR) El espacio de
  direcciones en modo real es entonces de `2^{20}` direcciones.

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
