.. default-role:: math

Decisiones de diseño
====================

Arranque
--------

Como se explica en el documento :doc:`background`, generalmente los
BIOS de las IBM PC se encargan de cargar el primer sector de nuestro
dispositivo a partir de una cierta dirección. El primer sector de un
disco floppy o un disco rígido equivale a 512 bytes.

512 bytes (510, en realidad, porque los últimos 2 bytes son ocupados
por la marca ``0x55AA``) no alcanzan si se quiere hacer un sistema
operativo con un mínimo de funcionalidad, por mucho que uno quiera.
Por lo tanto, la alternativa más sensata parece ser que esos 512 bytes
lean de nuestro dispositivo y se encarguen de cargar el resto de
nuestro sistema operativo en memoria.

Para poder hacer lo anterior, esos 512 bytes tienen que saber
interpretar el sistema de archivos del dispositivo, que en el caso
de los discos floppy es comúnmente algún FAT.

Como nuestro foco principal es el sistema operativo en sí, vamos a
utilizar, de momento, una imagen de disco floppy cuyo sector de
arranque realiza lo siguiente:

1. Copiar el sector de arranque a la dirección ``0x1000`` de
   la memoria principal.
2. Buscar en el mismo disco, que utiliza el sistema de archivos
   FAT12 de DOS, un archivo llamado ``KERNEL.BIN``.
3. Copiar el contenido del archivo ``KERNEL.BIN`` a la memoria
   principal a partir de la dirección ``0x1200``.
4. Saltar a la posición de memoria ``0x1200``.

Por ahora, con los 1.44MB de esta imagen de disco, podemos arreglarnos
para tener un sistema operativo minimal.
