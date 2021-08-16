% Índice de ayuda

## Introducción

Esta aplicación es un reproductor de archivos MIDI multiplataforma para Linux, Windows y 
macOS. Lee formatos de archivo .MID (archivos MIDI estándar), .KAR (Karaoke) y .WRK (Cakewalk)
y produce eventos MIDI en puertos MIDI de hardware y también sintetizadores software.

[Drumstick](https://drumstick.sourceforge.io) es un conjunto de
bibliotecas C ++ / Qt con licencia GPLv3 para aplicaciones MIDI. El proyecto incluye varias
herramientas como ejemplos, y entre ellas se encuentra la utilidad drumstick-guiplayer que 
aprovecha la biblioteca Drumstick::ALSA, por lo que solo está disponible para Linux 
(porque el secuenciador ALSA es una tecnología exclusiva de Linux). Algunas personas 
han pedido un programa con las mismas funcionalidades en Windows y macOS, y 
aquí está. Pero este programa es mucho más que eso y también funciona en Linux ...

Algunas características clave:

* Salida MIDI a puertos MIDI de hardware, o cualquier otro backend de Drumstick 
  como sintetizadores software
* [Transposición](https://es.wikipedia.org/wiki/Transposici%C3%B3n_(m%C3%BAsica)) 
  de la tonalidad entre -12 y +12 semitonos
* Cambiar el nivel de volumen MIDI (usando MIDI CC7)
* Escala la velocidad de la canción entre la mitad y doble 
  [tempo](https://es.wikipedia.org/wiki/Tempo)
* Vistas de letras, pianola y canales MIDI
* Admite formatos de archivo MID / KAR (archivos MIDI estándar) y WRK (Cakewalk)

## Salida MIDI

Cualquier reproductor MIDI solo emite eventos MIDI, pero no sonido. Para producir sonido, 
se necesita un sintetizador para convertir los eventos MIDI en audio digital que puede ser
reproducido por los altavoces de la computadora (o externos). Los eventos también pueden
ser renderizados por un instrumento musical MIDI externo conectado a la computadora, 
por ejemplo, un órgano MIDI, piano o sintetizador con una interfaz USB. Si esa 
opción no está disponible, otra alternativa habitual es un sintetizador de software 
(un programa).

Hay sintetizadores de software incluidos en los sistemas operativos de la computadora, 
por ejemplo, Microsoft Windows incluye "Microsoft GS Wavetable Synth", y
Apple macOS incluye el "Apple DLS Synthesizer". Para usarlos, debe abrir
el cuadro de diálogo "Configuración MIDI" y luego:

* En **Windows**, seleccione la opción "Windows MM" en el primer cuadro de lista y 
  "Microsoft GS Wavetable Synth" en el segundo cuadro de lista. En lugar de este sintetizador,
  que es muy básico y limitado, es posible que prefiera una alternativa mejor / gratuita como 
  [Sintetizador MIDI virtual](https://coolsoft.altervista.org/en/virtualmidisynth)
  que se mostrará como "VirtualMIDISynth #n" en el segundo cuadro de lista cuando se ejecute.
  Si tiene un instrumento MIDI externo conectado a la computadora, su nombre
  también debería aparecer en el segundo cuadro de lista.
* En **macOS**, seleccione "DLS Synth" en el primer y segundo cuadro de lista. 
  Este sintetizador tiene una opción de configuración para usar archivos de fuente de sonido DLS o SF2 si 
  quieres. Para usar otro dispositivo MIDI o sintetizador de software conectado,
  debe seleccionar "CoreMIDI" en el primer cuadro de lista, y el nombre de su
  sintetizador en el segundo.
* En **Linux** y otros sistemas operativos **Unix**, "SonivoxEAS" es un software 
  sintetizador listo para ser utilizado. Es un sintetizador GM muy básico que no usa archivos
  y depende únicamente del sistema de sonido PulseAudio. Para dispositivos externos conectados
  a la computadora o sintetizadores de software arbitrarios, debe seleccionar "ALSA" (o "OSS") 
  en el primer cuadro de lista.
* **FluidSynth** es otro sintetizador de software abierto incluido que es multiplataforma 
  y disponible en todos los sistemas operativos. Es muy configurable, aceptando
  archivos de fuente de sonido y muchos sistemas de sonido de audio digital. Seleccione
  [FluidSynth](https://github.com/FluidSynth/fluidsynth) en ambos cuadros de lista 
  para usarlo.
* **Network** es una salida de multidifusión UDP multiplataforma, que se puede utilizar para enviar 
  Eventos MIDI como paquetes de red a otras computadoras y dispositivos en la misma 
  red de área local. Es compatible con 
  [ipMIDI](https://www.nerds.de/en/ipmidi.html), 
  [QmidiNet](https://qmidinet.sourceforge.io/) y 
  [multidifusión](https://llg.cubic.org/tools/multimidicast/) 
  entre otro software.

## Controles de reproducción

Las siguientes acciones estándar están disponibles en el menú `Archivo` y 
también en la barra de herramientas, con las funcionalidades convencionales en 
cualquier reproductor multimedia: 

* `Reproducir` 
* `Pausa` (y continuar)
* `Parar`
* `Avanzar` (1 compás)
* `Retroceder` (1 compás)

Por defecto, `Reproducir` se activa automáticamente al cargar un archivo MIDI, 
excepto cuando esta función está desactivada en el cuadro de diálogo de 
configuración.

La `barra de estado`, ubicada opcionalmente en la parte inferior de la ventana 
principal, muestra el estado de reproducción actual como "Reproduciendo", 
"Detenido", "En pausa", etc.

### Transporte

El "Control de tono" se encuentra en la ventana principal. Es un cuadro de giro 
inicializado a cero, con un rango válido de -12 a +12 semitonos. Los límites son 
equivalentes a una octava hacia arriba o hacia abajo del tono original. Esto 
debería ser suficiente para los cantantes o instrumentistas que necesitan 
ajustar la tonalidad para su comodidad.

Todos los canales MIDI se transponen cromáticamente, excepto el canal de 
percusión (Canal GM 10 por omisión).

### Nivel de volumen
 
El "Control de volumen" se encuentra en la ventana principal. Es un control 
deslizante, inicializado al 100%, con un rango válido de 0% a 200%. El límite 
superior efectivo puede ser menor porque el MIDI CC 7 tiene un rango válido 
desde 0 a 127. También hay un botón para `Restablecer volumen`.
 
### Tempo

El `Control de tempo` se encuentra en la ventana principal. Es un control 
deslizante inicializado al 100% con un rango válido del 50% al 200%. Hay un 
botón para `Restablecer Tempo` y también una etiqueta de tempo, que muestra el 
tempo efectivo en pulsos por minuto con un valor inicial de 120 bpm. Este es el 
tempo predeterminado para un archivo MIDI si no contiene un cambio de tempo. 
Este valor se actualiza al cambiar el control de tempo, y también mientras se 
reproduce un archivo MIDI que contiene cambios de tempo.

### Salto, bucle y posicionamiento

La acción `Salto` (ubicada en el menú `Archivo` y en la barra de herramientas 
predeterminada), solicita un número de compás, entre 1 y el último compás de la 
canción cargada, para cambiar la posición actual de reproducción en la canción.

La acción `Bucle` (ubicada en el menú `Archivo` y la barra de herramientas 
predeterminada), solicita dos números de compás. La canción se reproducirá entre 
los dos compases dados cuando esté activado. Puede apagar el bucle en cualquier 
momento seleccionando la acción nuevamente.

El "Control de posicionamiento" se encuentra en la ventana principal. Es un 
control deslizante que se mueve mientras cambia la posición actual de 
reproducción, y se puede mover manualmente para cambiar la posición a cualquier 
punto arbitrario dentro de la canción.

## Vistas

Las siguientes opciones están habilitadas en el menú `Ver`:

Tres vistas son ventanas independientes que puede abrir y cerrar a su 
conveniencia: Canales, Letra y Pianola.

Por otro lado, también puede ocultar o mostrar tres vistas más que están 
incrustadas en la ventana principal: barra de herramientas, barra de estado y 
ritmo.

### Canales

Esta ventana muestra hasta 16 filas, una para cada canal MIDI utilizado por el
Archivo MIDI. Las columnas son:

* Número de canal MIDI y texto (editable). Por lo general, contiene el nombre 
  del instrumento o una breve descripción extraída de los metadatos del archivo.
* Botón de silencio. Con esta opción, el canal actual puede silenciarse.
* Botón "Solo". Sube el volumen del canal actual, mientras baja el de los otros 
  canales.
* Nivel. Indicador de la actividad en el canal actual.
* Bloquear. Este botón fija el Patch en el canal actual, evitando los cambios
  almacenados en el archivo.
* Parche. Corresponde a los eventos de cambio de programa MIDI de acuerdo con las
  Lista MIDI de nombres de instrumentos.

### Pianola

Esta ventana muestra hasta 16 filas, una para cada canal MIDI utilizado por el
Archivo MIDI. Cada fila contiene:

* Un número de canal y un texto (los mismos datos que la ventana Canales)
* Un teclado de piano. Las teclas se resaltarán con un color, que puede ser 
  completamente personalizado, por los eventos de nota MIDI reproducidos por el 
  archivo MIDI actual. Puede mostrar también opcionalmente los nombres de las 
  notas. Las teclas pueden ser activadas manualmente usando el teclado de la 
  computadora y el ratón

Hay un menú de ventana con las siguientes opciones:

* Ver en pantalla completa. Agranda la ventana llenando el monitor actual
* Mostrar todos los canales
* Ocultar todos los canales
* Reducir el número de teclas. De forma predeterminada cada teclado muestra 88 
  teclas, desde La-0 a Do-8. Usando esta opción los teclados muestran solo el 
  número de octavas realmente utilizadas por el archivo MIDI, que suele ser un 
  número menor
* Hasta 16 opciones de canal, para ocultar o mostrar cada canal individualmente. 
  Por omisión, todos los canales MIDI utilizados en el archivo MIDI actual 
  están seleccionados

### Letra

Esta ventana muestra los metadatos de texto del archivo MIDI cargado, filtrando 
los datos con una serie de controles ubicados en la barra de herramientas de la 
ventana:

* Pista: un cuadro de lista para elegir entre "Todas las pistas", o un elemento 
  correspondiente a cada pista utilizada en el archivo MIDI cargado. Este número 
  de pista reemplaza al concepto de canal MIDI en esta ventana, porque los 
  metadatos de texto no tienen un atributo de canal, pero siempre pertenecen a 
  alguna pista. Por defecto, la pista que contiene más datos se selecciona 
  automáticamente
* Tipo: un cuadro de lista para elegir uno de los tipos de texto de metadatos o 
  todos ellos. El tipo "Eventos de letra" es la opción más probable
* Codificación: la codificación del texto se detecta automáticamente, pero en 
  algunos casos la detección puede ser incorrecta por lo que es posible 
  seleccionar manualmente la correcta

Hay un menú de ventana con las siguientes opciones:

* Copiar al portapapeles: copia los metadatos que se muestran actualmente en la 
  ventana.
* Guardar en archivo... Después de abrir un cuadro de diálogo Guardar archivo, 
  esta opción crea un archivo de texto en el disco, con el texto mostrado por la 
  ventana y la codificación seleccionada
* Imprimir... Después de abrir un cuadro de diálogo Imprimir, esta opción 
  imprime el texto mostrado por la ventana de la impresora seleccionada
* Pantalla completa: agranda la ventana para llenar el monitor actual
* Fuente... Abre una ventana de Selección de fuente para elegir tipografía y 
  tamaño para los textos de la ventana

## Listas de reproducción

Las listas de reproducción son colecciones de nombres de archivos que se pueden 
usar para reproducir una serie de piezas MIDI, como un repertorio musical.

La acción `Archivo`->`Lista de reproducción...` gestiona la creación, 
modificación, ordenación, apertura y almacenamiento de los archivos de listas de 
reproducción. Esta ventana muestra el nombre de archivo de la lista de 
reproducción actual en el título de la ventana.

Las acciones `Archivo`->`Siguiente` y `Archivo`->`Anterior` se utilizan para 
navegar por la lista de reproducción manualmente hacia adelante y hacia atrás. 
Hay opciones de configuración para iniciar la reproducción automáticamente 
después de cargar archivos MIDI, y avanzar automáticamente al siguiente 
elemento de la lista de reproducción cuando termine de reproducir el elemento 
actual. Ambas opciones de configuración están habilitadas de forma 
predeterminada.

La lista de reproducción predeterminada inicial contiene una colección de 
archivos MIDI de ejemplo. Si prefiere comenzar con una lista vacía, debe usar el 
botón `Borrar` y luego guardar la lista de reproducción vacía con un nombre 
significativo. Esta sera la nueva lista de reproducción inicial en el futuro.

El último archivo de lista de reproducción abierto o guardado se recordará la 
próxima vez que se inicie el programa, pero las listas de reproducción no se 
guardan automáticamente.

Las listas de reproducción son archivos de texto simples, con un nombre de 
archivo en cada línea de texto. Los nombres de archivo pueden ser relativos a la 
ubicación del archivo de la lista de reproducción en sí (por lo que no llevan 
una ruta absoluta antepuesta) o cada archivo puede tener su propia ruta absoluta.

Puede tener varias listas de reproducción que se pueden crear fácilmente fuera 
del programa, por ejemplo: en la línea de comandos de Linux, este comando crea 
una lista de reproducción en un archivo llamado "my_playlist.lst" con todos los 
archivos MIDI ubicados en el directorio de trabajo:

~~~
$ ls -1 *.mid > my_playlist.lst
~~~

## Abrir archivos MIDI

Puede abrir archivos individuales ...

* Usando la acción estándar `Archivo`->`Abrir`
* Usando el menú `Archivo`->`Abrir archivos recientes`, que recuerda hasta diez 
  archivos abiertos recientemente
* Proporcionar nombre(s) de archivo en la línea de comando. El conjunto de 
  archivos se convierte en una lista de reproducción temporal. Esto permite una 
  fácil integración del programa con los administradores de archivos que 
  utilizan acciones de tipo "Abrir con..."
* Arrastrar y soltar archivos desde un administrador de archivos a la ventana 
  principal. El conjunto de archivos se convierte en una lista de reproducción 
  temporal.

## Personalización

### Preferencias

Hay un botón "Restaurar valores predeterminados" para inicializar todas las 
opciones en este cuadro de diálogo a sus valores predeterminados.

Hay tres pestañas que agrupan los ajustes de configuración

#### General

* Canal MIDI de percusión: número del 1 al 16. Por defecto 10, correspondiente 
  al canal de percusión estándar General MIDI
* Iniciar la reproducción automáticamente después de la carga. Habilitado por 
  omisión
* Avanzar automáticamente al siguiente elemento de la lista de reproducción. 
  Habilitado por omisión
* Forzar el modo oscuro. Puede resultar útil en Windows. En Linux y macOS, el 
  modo oscuro se aplica automáticamente cuando está configurado en el sistema 
  operativo
* Utilizar el tema de iconos interno. Esta configuración es necesaria en Windows 
  y macOS.
* Estilo de widgets de Qt. Dependiendo del sistema operativo, afecta a la 
  integración visual con otros programas
* Restablecimiento por sistema exclusivo MIDI. El reproductor envía este mensaje 
  justo antes de comenzar a reproducir cada archivo MIDI 

#### Letra

* Tipografía del texto. Abre una ventana de selección de fuente para elegir la 
  tipografía y el tamaño de los textos de la ventana. Esta configuración también 
  está disponible en el menú de la ventana Letras 
* Color de texto futuro. Esta configuración se aplica al texto que no se ha 
  reproducido
* Color de texto anterior. Esta configuración se aplica al texto que ya se ha 
  reproducido

#### Pianola

* Resaltado de nota. Hay varias paletas de resaltado disponibles.
* Color de resaltado único. Cuando se selecciona la paleta de resaltado de un 
  solo color, esta configuración indica el color de resaltado de la nota
* Trasladar la velocidad MIDI a tono de color. Con esta opción, la velocidad de 
  los eventos de nota MIDI se utiliza para dar al color de resaltado un tono más 
  claro o más oscuro
* Fuente de nombres de notas. Abre una ventana de selección de fuente para 
  elegir la tipografía y tamaño de los nombres de las notas
* Mostrar nombres de notas: las opciones disponibles son: Nunca, Mínimo, Cuando 
  se activa, y Siempre. La opción Mínimo muestra solo los nombres de las teclas 
  Do

### Personalización de la barra de herramientas

La barra de herramientas se encuentra en la ventana principal, que normalmente 
tiene un espacio disponible muy reducido, y se puede mover a la parte superior o 
inferior de la ventana, y también fuera de ella. Las acciones disponibles como 
botones de herramientas se pueden configurar en esta ventana de diálogo. 
Todas las acciones también son elementos del menú principal.

* Acciones disponibles: muestra la lista de acciones que no se muestran 
  actualmente como botones de herramientas
* Acciones seleccionadas: es la lista de botones de herramientas seleccionados 
  actualmente

Para seleccionar una acción de la lista Disponible, primero selecciónela con el 
ratón y luego presione el botón "agregar". La acción se elimina de la lista 
Disponible y se coloca al final de la lista seleccionada.

Para eliminar un botón de la barra de herramientas, selecciónelo con el ratón en 
la lista Seleccionada y luego presione el botón "eliminar".

Para reorganizar el orden de los botones en la barra de herramientas, use los 
botones "arriba" y "abajo" después de seleccionar un elemento en la lista 
Seleccionada.

Estilo de los botones de herramientas: contiene las siguientes opciones que se 
aplican a todos los botones de la barra de herramientas

* Solo icono
* Solo texto
* Texto junto al icono
* Texto bajo el icono
* Seguir estilo: la configuración está determinada por la configuración del 
  "Estilo de widgets de Qt" en el cuadro de diálogo Preferencias
