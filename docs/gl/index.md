% Índice de axuda

## Introdución

Esta aplicación é un reprodutor de ficheiros MIDI multiplataforma para Linux, Windows e
macOS. Le formatos de ficheiro .MID (Arquivos MIDI estándar), .KAR (Karaoke) e .WRK (Cakewalk)
e envía eventos MIDI a portos MIDI de hardware e tamén a sintetizadores software.

[Drumstick](https://drumstick.sourceforge.io) é un conxunto de 
bibliotecas C++/Qt con licenza GPLv3 para aplicacións MIDI. O proxecto inclúe varias
ferramentas como exemplos, e entre elas está a utilidade drumstick-guiplayer que
aproveita a biblioteca Drumstick::ALSA, polo que só está dispoñible para Linux
(porque o secuenciador ALSA é unha tecnoloxía só para Linux). Algunhas persoas
pediron un programa coas mesmas funcionalidades en Windows e macOS, e
aquí está. Pero este programa é moito máis que iso, e tamén funciona en Linux...

Algunhas características clave:

* Saída MIDI a portos MIDI de hardware ou a calquera outro backend de Drumstick 
  como sintetizadores software
* [Traspoñer](https://en.wikipedia.org/wiki/Transposition_(music)) a tonalidade 
  da canción entre -12 e +12 semitonos
* Cambiar o nivel de volume MIDI (usando MIDI CC7)
* Escalar a velocidade da canción entre a metade e o dobre de
  [tempo](https://en.wikipedia.org/wiki/Tempo)
* Vistas de letras, pianola e canles MIDI
* Admite formatos de ficheiros MID/KAR (Arquivos MIDI estándar) e WRK (Cakewalk)

## Saída MIDI

Calquera reprodutor MIDI só produce eventos MIDI, pero non son. Para producir son,
precísase un sintetizador para renderizar os eventos en audio dixital que pode ser
reproducido polos altofalantes do ordenador (ou externos). Os eventos tamén poden
ser interpretados por un instrumento musical MIDI externo conectado ao ordenador,
por exemplo, un órgano MIDI, un piano ou un sintetizador cunha interface USB. Se isa
opción non está dispoñible, outra alternativa habitual é un sintetizador de software
(un programa).

Hai sintetizadores de software incluídos nos sistemas operativos do ordenador,
por exemplo, Microsoft Windows inclúe o "Microsoft GS Wavetable Synth" e
Apple macOS inclúe o "Apple DLS Synthesizer". Para usalos, cómpre abrir
o diálogo "Configuración MIDI" e despois:

* En **Windows**, seleccione a opción "Windows MM" na primeira caixa de lista e
  "Microsoft GS Wavetable Synth" na segunda caixa de lista. En lugar deste sintetizador,
  que é moi básico e limitado, pode preferir unha alternativa mellor/gratis como
  [VirtualMIDISynth](https://coolsoft.altervista.org/en/virtualmidisynth)
  que se mostrará como "VirtualMIDISynth #n" na segunda caixa de lista ao executarse.
  Se tes un instrumento MIDI externo conectado ao ordenador, o seu nome
  debería aparecer tamén na segunda caixa de lista.
* En **macOS**, seleccione "DLS Synth" tanto na primeira como na segunda caixa de lista.
  Este sintetizador ten unha opción de configuración para usar ficheiros de fontes de son DLS ou SF2 se
  ti queres. Para usar outro dispositivo MIDI ou sintetizador de software conectado,
  cómpre seleccionar "CoreMIDI" na primeira caixa de lista e o nome do seu
  sintetizador na segunda.
* En **Linux** e noutros sistemas operativos **Unix**, "SonivoxEAS" é un
  sintetizador listo para ser usado. É un sintetizador GM moi básico que non usa ficheiros
  e depende só do sistema de son PulseAudio. Para dispositivos externos conectados
  ao ordenador ou a sintetizadores software arbitrarios, cómpre seleccionar "ALSA"
  (ou "OSS") no primeiro cadro de lista.
* **FluidSynth** é outro sintetizador de software aberto incluído que é multiplataforma
  e dispoñible en todos os sistemas operativos. É moi configurable, aceptando
  ficheiros soundfont e moitos sistemas de son de audio dixital. Seleccione
  [FluidSynth](https://github.com/FluidSynth/fluidsynth) en ambas caixas de lista
  para usalo.
* **Rede** é unha saída multiplataforma UDP Multicast, que se pode usar para enviar
  Eventos MIDI como paquetes de rede a outros ordenadores e dispositivos na mesma
  rede de área local. É compatible con [ipMIDI](https://www.nerds.de/en/ipmidi.html),
  [QmidiNet](https://qmidinet.sourceforge.io/), e
  [multimidicast](https://llg.cubic.org/tools/multimidicast/)
  entre outros programas.

## Controis de reprodución

As seguintes accións estándar están dispoñibles no menú `Ficheiro` e tamén no menú
barra de ferramentas, coas funcionalidades convencionais de calquera reprodutor multimedia:

* `Reproducir`
* `Pausa` (e continúa)
* `Parar`
* `Avance rápido` (1 compás)
* `Rebobinar` (1 compás)

Por defecto, `Reproducir` actívase automaticamente ao cargar un ficheiro MIDI excepto
cando esta función está desactivada no diálogo de configuración.

Móstrase na "barra de estado", situada opcionalmente na parte inferior da xanela principal
o estado actual da reprodución como "Reproducindo", "Detido", "En pausa", etc.

### Traspoñer

O `Control de ton` está situado na xanela principal. É unha caixa xiratoria inicializada
a cero, cun rango válido de -12 a +12 semitonos. Os límites son equivalentes
a unha oitava arriba ou abaixo do ton orixinal. Isto debería ser suficiente para os cantantes
ou instrumentistas que necesitan axustar a tonalidade para a súa comodidade.

Todas as canles MIDI transpóñense cromáticamente, excepto a canle de percusión
(Canle 10 de GM por defecto).

### Nivel de volume
 
O `Control de volume` está situado na xanela principal. É un control deslizante, inicializado
ao 100%, cun rango válido do 0% ao 200%. O efecto límite superior pode ser inferior
porque o MIDI CC 7 ten un rango válido de 0 a 127. Tamén hai un botón para
"Restablecer volume".
 
### Tempo

O `Control Tempo` está situado na xanela principal. É un control deslizante inicializado
ao 100% cun rango válido do 50% ao 200%. Hai un botón para "Reset Tempo".
e tamén unha pantalla de tempo, que mostra o tempo efectivo en pulsacións por minuto con
un valor inicial de 120 bpm. Este é o tempo predeterminado para un ficheiro MIDI si
non contén un cambio de tempo. Este valor actualízase ao cambiar o
control de tempo, e tamén mentres se reproduce un ficheiro MIDI que contén cambios de tempo.

### Salto, bucle e posicionamento

A acción `Saltar` (situada no menú `Ficheiro` e na barra de ferramentas predeterminada), solicita
un número de compás, entre 1 e o último compás da canción cargada, para cambiar a
posición actual da canción a o compás indicado.

A acción `Bucle` (situada no menú `Ficheiro` e na barra de ferramentas predeterminada), solicita
dous números de compás. A canción tocarase entre os dous compases indicados cando sexa
activado. Podes desactivar o bucle en calquera momento seleccionando a acción de novo.

O `Control de posicionamento` está situado na xanela principal. É un control deslizante que
móvese mentres cambia a posición de reprodución actual e pódese mover a man para
cambiar a posición a calquera punto arbitrario dentro da canción.

## Configuración da canción

A configuración das cancións gárdase no subdirectorio $HOME/.dmidiplayer co
mesmo nome que as cancións e o sufixo ".cfg". Cando se carga un ficheiro de canción,
a sua configuración pódese cargar automaticamente e tamén gardarse cando a canción se detivo.
Consulte a configuración correspondente no diálogo Preferencias/Xeral.

A configuración das cancións tamén se pode cargar e gardar manualmente desde o correspondente
elemento do menú "Ficheiro"->"Configuración da canción".

Cada ficheiro .cfg de configuración de canción é un ficheiro de texto onde se almacenan os seguintes datos:

- Codificación de texto da canción, detectada ou seleccionada polo usuario na vista Letras.
- O camiño completo do ficheiro e o nome da canción.
- A alteración da altura en semitonos, tal e como se indica na xanela principal.
- A variación do tempo, tal e como se indica na xanela principal.
- A variación global do volume, tal e como se indica na xanela principal.

Para cada canle MIDI utilizada pola canción almacénanse os seguintes datos 
procedentes da vista de canles:

- A variación do volume da canle.
- A etiqueta da canle.
- O parche da canle (número de programa MIDI).
- Estado dos botóns de solo, silencio e bloqueado da canle.

## Vistas

No menú "Ver" están habilitadas as seguintes opcións:

Tres vistas son fiestras independentes que podes abrir e pechar a tua
comodidade: canles, letras e pianola.

Por outra banda, tamén pode ocultar ou mostrar tres vistas máis que están incrustadas
na xanela principal: barra de ferramentas, barra de estado e ritmo.

### Canles

Esta xanela mostra ata 16 filas, unha por cada canle MIDI utilizada polo actual
Ficheiro MIDI. As columnas son:

* Número e texto da canle MIDI (editable). Normalmente o nome do instrumento, ou
  breve descrición extraída dos metadatos do ficheiro.
* Botón de silencio. Con esta opción pódese silenciar a canle actual.
* Botón Solo. Aumenta o volume da canle actual mentres baixa
  outras canles.
* Nivel. Indicador da actividade na canle actual.
* Pechar o ferrollo. Este botón fixa o parche na canle actual, evitando cambios
  almacenados no ficheiro.
* Parche. Corresponde aos eventos de cambio de programa MIDI segundo a 
  Lista MIDI Xeral de nomes de instrumentos.

### Pianola

Esta xanela mostra ata 16 filas, unha por cada canle MIDI utilizada polo actual
Ficheiro MIDI. Cada fila contén:

* Un número de canle e texto (os mesmos datos que a xanela Canles)
* Un teclado de piano. As teclas serán resaltadas cunha cor, que pode ser totalmente
  personalizado, polos eventos de nota MIDI reproducidos polo ficheiro MIDI actual. Pode
  tamén, opcionalmente, mostrar os nomes das notas. As teclas poden estar activadas
  manualmente utilizando o teclado e o rato do ordenador

Hai un menú de xanela, coas seguintes opcións:

* Ver pantalla completa. Amplía a fiestra enchendo o monitor actual
* Mostrar todas as canles
* Ocultar todas as canles
* Apretar o número de teclas. Por defecto, cada teclado mostra 88 teclas, dende A0
  ao C8. Usando esta opción, os teclados só usan realmente o número de oitavas
  usado polo ficheiro MIDI, que adoita ser un número menor
* Ata 16 opcións de canle, para ocultar ou mostrar cada canle individualmente. Por
  por defecto todas as canles MIDI utilizadas no ficheiro MIDI actual están seleccionadas

### Letras

Esta xanela mostra os metadatos de texto do ficheiro MIDI cargado, filtrando os
datos cunha serie de controis situados na barra de ferramentas da xanela:

* Pista: unha caixa de lista para escoller entre "Todas as pistas" ou un elemento correspondente
  a cada pista utilizada no ficheiro MIDI cargado. Este número de pista substitúe ao
  concepto de canle MIDI nesta xanela, porque os metadatos do texto non teñen
  un atributo de canle, pero sempre pertence a algunha pista. Por defecto,
  selecciónase automaticamente a pista que contén máis datos
* Tipo: unha caixa de lista para escoller un dos tipos de texto de metadatos ou todos.
  O tipo "Letra" é a opción máis probable
* Codificación: a codificación do texto detéctase automaticamente, pero nalgúns casos
  a detección pode ser incorrecta, polo que pódese seleccionar manualmente

Hai un menú de xanela, coas seguintes opcións:

* Copiar no portapapeis: copia os metadatos que a xanela mostra actualmente
* Gardar no ficheiro... Despois de abrir un diálogo Gardar ficheiro, esta opción crea un
  ficheiro de texto no disco, co texto mostrado pola xanela e a codificación seleccionada
* Imprimir... Despois de abrir un diálogo de impresión, esta opción imprime o texto mostrado por
  a xanela da impresora seleccionada
* Pantalla completa: amplía a xanela enchendo o monitor actual
* Fonte... Abre unha xanela de selección de fontes para escoller a tipografía e o tamaño
  os textos da fiestra

## Listas de reprodución

As listas de reprodución son coleccións de nomes de ficheiros que se poden usar para reproducir unha serie
de pezas MIDI, como un repertorio musical.

A acción `Ficheiro`->`Lista de reprodución...` xestiona a creación, modificación, ordenación,
abrir e gardar ficheiros da lista de reprodución. Esta xanela mostra a lista de reprodución actual
nome do ficheiro no título da xanela.

As accións `Ficheiro`->`Seguinte` e `Ficheiro`->`Anterior` úsanse para navegar pola lista de reprodución
manualmente cara adiante e cara atrás. Hai opcións de configuración para iniciar a reprodución
automaticamente despois de cargar ficheiros MIDI e avanzar automaticamente ao seguinte
elemento da lista de reprodución cando remate de reproducir o elemento actual. Ambas opcións de configuración
están activadas por defecto.

A lista de reprodución predeterminada inicial contén unha colección de exemplos de ficheiros MIDI. Se
prefires comezar cunha lista baleira, deberías usar o botón `Borrar` e
despois garda a lista de reprodución baleira cun nome significativo. Esta será a nova
lista de reprodución inicial no futuro.

O último ficheiro de lista de reprodución aberto ou gardado lembrarase a próxima vez
iníciase o programa, pero as listas de reprodución non se gardan automaticamente.

As listas de reprodución son ficheiros de texto sinxelos, cun nome de ficheiro en cada liña de texto. No arquivo,
os nomes poden ser relativos á localización do propio ficheiro da lista de reprodución (polo que 
non teñen unha ruta absoluta anteposta) ou cada ficheiro pode ter unha ruta absoluta propia.

Podes ter varias listas de reprodución que se poden crear facilmente fóra de
programa, por exemplo, na liña de comandos de Linux, este comando crea unha lista de reprodución
nun ficheiro chamado "my_playlist.lst" con todos os ficheiros MIDI situados no
directorio de traballo:

~~~
$ ls -1 *.mid > my_playlist.lst
~~~

## Apertura de ficheiros MIDI

Podes abrir ficheiros individuais...

* Usando a acción estándar `Ficheiro`->`Abrir`
* Usando o menú `Ficheiro`->`Abrir ficheiros recentes`, que lembra ata dez ficheiros
  abertos recentemente.
* Proporcionando o(s) nome(s) do ficheiro na liña de comandos. O conxunto de ficheiros pasa a ser a
  lista de reprodución temporal. Isto permite unha fácil integración do programa con
  xestores de ficheiros que usan accións "Abrir con...".
* Arrastrar e soltar ficheiros desde un xestor de ficheiros á xanela principal. O
  conxunto de ficheiros convértese nunha lista de reprodución temporal.

## Personalización

### Preferencias

Hai un botón "Restaurar valores predeterminados" para inicializar todas as opcións neste diálogo
aos seus valores predeterminados.

Hai tres pestanas que agrupan os axustes de configuración

#### Xeral

* Canle MIDI de percusión: Número do 1 ao 16. Por defecto 10, correspondente á
  canle de percusión estándar MIDI xeral
* Redución de volume % do botón Solo. Predeterminado 50. O volume doutras canles MIDI
  redúcese nesta porcentaxe cando unha canle ten o botón "solo" pulsado.
* Inicia a reprodución automaticamente despois da carga. Activado por defecto
* Avanza automaticamente ao seguinte elemento da lista de reprodución. Activado por defecto
* Carga e garda automaticamente a configuración da canción. Non está activado por defecto. 
  As configuracións gárdanse no subdirectorio $HOME/.dmidiplayer co mesmo nome 
  das cancións e o sufixo ".cfg".
* Bordes apegadizos da fiestra. Só para MS Windows. Activado por defecto.
* Forzar o modo escuro. Pode ser útil en Windows. En Linux e macOS o modo escuro é
  aplicado automaticamente cando se configura no sistema operativo
* Usa o tema de iconas interno. Esta configuración é necesaria en Windows e macOS.
* Estilo de widgets Qt. Dependendo do sistema operativo, afecta a integración
  visual con outros programas
* Restablecemento do sistema exclusivo MIDI. O reproductor envía esta mensaxe xusto 
  antes de comezar reproducindo cada ficheiro MIDI

#### Letras

* Fonte do texto. Abre unha xanela de selección de fontes para escoller a tipografía e o tamaño
  dos textos da fiestra. Esta configuración tamén está dispoñible no menú da xanela Letras
* Cor do texto futuro. Esta configuración aplícase ao texto que ainda non se reproduciu
* Cor do texto anterior. Esta configuración aplícase ao texto que xa se reproduciu

#### Pianola

* Resaltar notas. Hai algunhas paletas de resaltado dispoñibles
* Unha única cor de resaltado. Cando se selecciona a paleta de resaltado dunha soa cor,
  esta configuración é a cor de resaltado da nota
* Traducir velocidade ao matiz da cor. Con esta opción, a velocidade dos eventos de nota MIDI
  utilízase para darlle á cor de resaltado un ton máis claro ou escuro
* Anota os nomes da fonte. Abre unha xanela de selección de fontes para escoller a tipografía e
  tamaño dos nomes das notas
* Mostrar os nomes das notas: as opcións dispoñibles son Nunca, Mínima, Cando está activada, sempre.
  A opción Mínimo mostra só os nomes das teclas Do.
* Designación do subíndice de oitava. Desactivada, a nota Do central MIDI #60 
  preséntase como "Do4", pero cando está activado preséntase como "Do<sub>4</sub>".
  
### Personalizar a barra de ferramentas

A barra de ferramentas está situada na xanela principal, que normalmente ten moi pouco
espazo dispoñible e pódese mover á parte superior ou á parte inferior da xanela, e
tamén fóra dela. As accións dispoñibles como botóns de ferramenta pódense configurar en
esta xanela de diálogo. Todas as accións tamén son elementos do menú principal.

* Accións dispoñibles: mostra a lista de accións que non se mostran actualmente
  botóns da ferramenta
* Accións seleccionadas: é a lista dos botóns das ferramentas actualmente seleccionados

Para seleccionar unha acción da lista Dispoñible, primeiro selecciónaa co rato e
a continuación, prema o botón "engadir". A acción elimínase da lista Dispoñible e
colócase ao final da lista seleccionada.

Para eliminar un botón da barra de ferramentas, selecciónao co rato na sección 
lista Seleccionado e, a continuación, prema o botón "eliminar".

Para reorganizar a orde dos botóns na barra de ferramentas, use qs botóns "arriba" e "abaixo".
despois de seleccionar un elemento da lista Seleccionado.

Estilo dos botóns da ferramenta: contén as seguintes opcións que se aplican a todos
os botóns da barra de ferramentas

* Só icona
* Só texto
* Texto a carón da icona
* Texto baixo a icona
* Seguir estilo: a configuración está determinada pola configuración "Estilo de widgets Qt",
  no diálogo de Preferencias.
