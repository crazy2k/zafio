.. default-role:: math

Zafio por dentro
================

El *bootloader*
---------------

Decidimos utilizar GRUB ("Legacy") como nuestro *bootloader* de
referencia, aunque nuestro sistema operativo debería poder ser
arrancado por cualquier *bootloader* que cumpla la especificación
Multiboot [Multiboot]_.

GRUB es un *bootloader* muy completo, y la especificación Multiboot nos
parece relativamente sencilla y razonable. Como entendemos que nuestro
foco principal está puesto en el sistema operativo en sí, y tenemos a
nuestro alcance una herramienta como esta, optamos por aprovecharla en
lugar de escribir nuestro propio *bootloader* desde cero.

Convenciones
------------

.. TODO: Agregar las convenciones que usamos para las macros, los
   nombres de las funciones, etc.


