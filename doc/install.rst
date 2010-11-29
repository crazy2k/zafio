.. default-role:: math

Instalación y uso
=================

Antes de realizar la compilación de Zafio, es preciso preparar los
programas que se ejecutarán una vez que el sistema sea cargado.

El directorio ``user/`` contiene dos programas de ejemplo en los
subdirectorios ``bye/`` y ``hello/`` respectivamente. Ambos programas
realizan llamadas al sistema y uno de ellos eventualmente finaliza su
ejecución. Como se puede observar, estos directorios contienen un
archivo con código C (y extensión ``.c``) con el punto de entrada del
programa y un enlace simbólico a un archivo Makefile que se halla en
``user/``.

Para compilar los programas alcanza con ejecutar el comando ``make``
desde el *shell*, encontrándonos ubicados en el directorio de cada
programa. Con ``make install`` se produce la "instalación" del
programa, es decir, la ubicación de su ejecutable correspondiente en el
directorio ``progs/``, que es donde Zafio buscará los programas a ser
ejecutados.

Una vez instalados los programas que se deseen, puede ejecutarse
``make`` en el directorio raíz para compilar el kernel. Si la
compilación tuvo éxito, en ``reftest/`` (el directorio destinado a las
pruebas de referencia) debería encontrarse una imagen lista para ser
arrancada usando Bochs. En ese mismo directorio, también, se encuentra
listo un archivo de configuración de Bochs, ``bochsrc``, por lo que
alcanza con ejecutar el comando ``bochs -q`` para iniciar el emulador y
arrancar Zafio.
