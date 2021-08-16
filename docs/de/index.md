% Hilfeindex

## Einführung

Diese Anwendung ist ein Multiplattform-MIDI-Dateiplayer für Linux, Windows und 
Mac OS. Es liest .MID (Standard MIDI Files), .KAR (Karaoke) und .WRK (Cakewalk)
Dateiformate und gibt MIDI-Events an Hardware-MIDI-Ports und auch Software aus 
Synthesizer.

[Drumstick](https://drumstick.sourceforge.io) ist ein Satz von GPLv3 
lizenzierte C++/Qt-Bibliotheken für MIDI-Anwendungen. Das Projekt umfasst mehrere
Tools als Beispiele, darunter das Dienstprogramm drumstick-guiplayer, das 
nutzt die Drumstick::ALSA-Bibliothek, ist also nur für Linux verfügbar 
(weil der ALSA-Sequenzer eine reine Linux-Technologie ist). Manche Leute haben
ein Programm mit den gleichen Funktionalitäten unter Windows und macOS angefordert, und 
hier ist es. Aber dieses Programm ist viel mehr als das und es funktioniert auch weiter
Linux...

Einige wichtige Funktionen:

* MIDI-Ausgabe an Hardware-MIDI-Ports oder jedes andere Drumstick-Backend wie Soft
  Synthesizer
* [Transpose](https://en.wikipedia.org/wiki/Transposition_(music)) Song-Tonalität 
  zwischen -12 und +12 Halbtönen
* MIDI-Lautstärke ändern (mit MIDI CC7)
* Skaliere die Songgeschwindigkeit zwischen halb und doppelt 
  [Tempo](https://en.wikipedia.org/wiki/Tempo)
* Ansichten von Songtexten, Piano-Player und MIDI-Kanälen
* Unterstützt die Dateiformate MID/KAR (Standard MIDI Files) und WRK (Cakewalk)

## MIDI-Ausgang

Jeder MIDI-Player gibt nur MIDI-Events aus, aber keinen Ton. Zur Klangerzeugung wird ein MIDI
Synthesizer wird benötigt, um die Events in digitales Audio zu rendern, das 
von den (oder externen) Lautsprechern des Computers wiedergegeben werden. Die Veranstaltungen können auch
von einem an den Computer angeschlossenen externen Musik-MIDI-Instrument wiedergegeben werden, 
B. eine MIDI-Orgel, ein Klavier oder ein Synthesizer mit USB-Schnittstelle. Wenn das
Option ist nicht verfügbar, eine andere übliche Alternative ist ein Software-Synthesizer 
(ein Programm).

In den Betriebssystemen des Computers sind Software-Synthesizer enthalten, 
Microsoft Windows enthält beispielsweise den "Microsoft GS Wavetable Synth", und
Apple macOS enthält den "Apple DLS Synthesizer". Um sie zu verwenden, müssen Sie öffnen
den Dialog "MIDI Setup" und dann:

* Wählen Sie in **Windows** im ersten Listenfeld die Option "Windows MM" und 
  "Microsoft GS Wavetable Synth" im zweiten Listenfeld. Anstelle dieses Synthesizers
  was sehr einfach und begrenzt ist, bevorzugen Sie vielleicht eine bessere/kostenlose Alternative wie 
  [Virtueller MIDI-Synth](https://coolsoft.altervista.org/en/virtualmidisynth)
  das wird beim Ausführen im zweiten Listenfeld als "VirtualMIDISynth #n" angezeigt.
  Wenn Sie ein externes MIDI-Instrument an den Computer angeschlossen haben, wird dessen Name
  sollte auch im zweiten Listenfeld erscheinen.
* Wählen Sie in **macOS** sowohl im ersten als auch im zweiten Listenfeld "DLS Synth". 
  Dieser Synth hat eine Konfigurationsoption für die Verwendung von DLS- oder SF2-Soundfont-Dateien, wenn 
  Sie wollen. Um ein anderes angeschlossenes MIDI-Gerät oder einen Software-Synth zu verwenden,
  Sie müssen im ersten Listenfeld "CoreMIDI" auswählen und den Namen Ihres
  Synthesizer im zweiten.
* In **Linux** und anderen **Unix**-Betriebssystemen ist "SonivoxEAS" ein Soft 
  Synthesizer einsatzbereit. Es ist ein sehr einfacher GM-Synth, der keine Dateien verwendet
  und hängt nur vom PulseAudio-Soundsystem ab. Für angeschlossene externe Geräte
  an den Computer oder beliebige Softsynths, müssen Sie "ALSA" (oder "OSS") auswählen 
  im ersten Listenfeld.
* **FluidSynth** ist ein weiterer enthaltener Open-Software-Synth, der plattformübergreifend ist 
  und auf allen Betriebssystemen verfügbar. Es ist sehr konfigurierbar und akzeptiert
  Soundfont-Dateien und viele digitale Audio-Soundsysteme. Auswählen
  [FluidSynth](https://github.com/FluidSynth/fluidsynth) in beiden Listenfeldern 
  um es zu benutzen.
* **Netzwerk** ist eine Multiplattform-UDP-Multicast-Ausgabe, die zum Senden verwendet werden kann 
  MIDI-Events als Netzwerkpakete an andere Computer und Geräte im selben 
  lokales Netzwerk. Es ist kompatibel mit
  [ipMIDI](https://www.nerds.de/de/ipmidi.html),
  [QmidiNet](https://qmidinet.sourceforge.io/), und 
  [multimidicast](https://llg.cubic.org/tools/multimidicast/) 
  unter anderem Software.

## Wiedergabesteuerung

Die folgenden Standardaktionen stehen im Menü `Datei` zur Verfügung und auch die
Symbolleiste, mit den üblichen Funktionalitäten auf jedem Mediaplayer: 

* `Spielen` 
* `Pause` (und weiter)
* `Stopp`
* `Schneller Vorlauf` (um 1 bar)
* `Zurückspulen` (um 1 bar)

Standardmäßig wird `Play` beim Laden einer MIDI-Datei automatisch aktiviert, außer 
wenn diese Funktion im Konfigurationsdialog deaktiviert ist.

Die `Statusleiste`, die sich optional am unteren Rand des Hauptfensters befindet, zeigt 
den aktuellen Wiedergabestatus als "Wiedergabe", "Gestoppt", "Pausiert" usw.

###Transponieren

Die `Pitch Control` befindet sich im Hauptfenster. Es ist eine Spinbox, die initialisiert wurde
auf Null, mit einem gültigen Bereich von -12 bis +12 Halbtönen. Die Grenzen sind gleichwertig
um eine Oktave höher oder tiefer als der Originalton. Das sollte für Sänger reichen
oder Instrumentalisten, die die Tonalität für ihren Komfort anpassen müssen.

Alle MIDI-Kanäle sind chromatisch transponiert, mit Ausnahme des Percussion-Kanals 
(GM-Kanal 10 standardmäßig).

### Lautstärkepegel
 
Die `Lautstärkeregelung` befindet sich im Hauptfenster. Es ist ein Schieberegler, initialisiert
bis 100 %, mit einem gültigen Bereich von 0 % bis 200 %. Der Effekt der oberen Grenze kann niedriger sein
da MIDI CC 7 einen gültigen Bereich von 0 bis 127 hat. Es gibt auch eine Taste zum
`Lautstärke zurücksetzen`.
 
### Tempo

Die `Temposteuerung` befindet sich im Hauptfenster. Es ist ein Slider initialisiert
bis 100 % mit einem gültigen Bereich von 50 % bis 200 %. Es gibt eine Taste zum `Reset Tempo`
und auch eine Tempoanzeige, die das effektive Tempo in Schlägen pro Minute mit . anzeigt
ein Anfangswert von 120 bpm. Dies ist das Standardtempo für eine MIDI-Datei, wenn sie
enthält keine Tempoänderung. Dieser Wert wird aktualisiert, wenn die
Temposteuerung, und auch während eine MIDI-Datei mit Tempoänderungen abgespielt wird.

### Jump, Loop und Positionierung

Aktion `Sprung` (im Menü `Datei` und in der Standard-Symbolleiste) fragt nach
eine Taktnummer zwischen 1 und dem letzten Takt des geladenen Songs, um die 
aktuelle Position des Songs zum angegebenen Takt.

Aktion `Loop` (befindet sich im Menü `Datei` und in der Standardsymbolleiste), fragt nach
zwei Taktzahlen. Das Lied wird zwischen den beiden angegebenen Takten abgespielt, wenn es ist
aktiviert. Sie können die Schleife jederzeit ausschalten, indem Sie die Aktion erneut auswählen.

Die `Positionierungskontrolle` befindet sich im Hauptfenster. Es ist ein Schieberegler, der
bewegt sich, während sich die aktuell gespielte Position ändert, und kann von Hand auf verschoben werden
Ändern Sie die Position an einen beliebigen Punkt innerhalb des Songs.

## Aufrufe

Die folgenden Optionen sind im Menü "Ansicht" aktiviert:

Drei Ansichten sind unabhängige Fenster, die Sie nach Belieben öffnen und schließen können 
Bequemlichkeit: Kanäle, Liedtexte und Klavierspieler.

Auf der anderen Seite können Sie auch drei weitere Ansichten ein- oder ausblenden, die eingebettet sind 
im Hauptfenster: Symbolleiste, Statusleiste und Rhythmus.

### Kanäle

Dieses Fenster zeigt bis zu 16 Zeilen an, eine für jeden MIDI-Kanal, der vom aktuellen . verwendet wird
MIDI-Datei. Die Spalten sind:

* MIDI-Kanalnummer und Text (bearbeitbar). Normalerweise der Instrumentenname oder a
  kurze Beschreibung aus den Dateimetadaten.
* Stummschalttaste. Mit dieser Option kann der aktuelle Kanal stummgeschaltet werden.
* Solo-Taste. Erhöht die Lautstärke des aktuellen Kanals, während die Lautstärke verringert wird
  andere Kanäle.
* Niveau. Anzeige der Aktivität auf dem aktuellen Kanal.
* Sperren. Diese Schaltfläche fixiert das Patch auf dem aktuellen Kanal und verhindert Änderungen
  in der Datei gespeichert.
* Patch. Entspricht den MIDI-Programmwechsel-Events gemäß dem General
  MIDI-Liste der Instrumentennamen.

### Piano Spieler

Dieses Fenster zeigt bis zu 16 Zeilen an, eine für jeden MIDI-Kanal, der vom aktuellen . verwendet wird
MIDI-Datei. Jede Zeile enthält:

* Eine Kanalnummer und ein Text (die gleichen Daten wie im Kanalfenster)
* Eine Klaviertastatur. Die Tasten werden mit einer Farbe hervorgehoben, die vollständig sein kann
  angepasst, durch die MIDI-Noten-Events, die von der aktuellen MIDI-Datei gespielt werden. Es kann
  optional auch die Notennamen anzeigen. Die Tasten können aktiviert werden
  manuell über die Computertastatur und die Maus

Es gibt ein Fenstermenü mit den folgenden Optionen:

* Vollbild anzeigen. Vergrößert das Fenster, das den aktuellen Monitor ausfüllt
* Alle Kanäle anzeigen
* Alle Kanäle ausblenden
* Ziehen Sie die Anzahl der Schlüssel fest. Standardmäßig zeigt jede Tastatur 88 Tasten an, von A0
  bis C8. Mit dieser Option verwenden die Keyboards nur die Anzahl der Oktaven wirklich
  wird von der MIDI-Datei verwendet, die normalerweise eine kleinere Zahl ist
* Bis zu 16 Kanaloptionen, um jeden Kanal einzeln auszublenden oder anzuzeigen. Von
  Standardmäßig sind alle in der aktuellen MIDI-Datei verwendeten MIDI-Kanäle ausgewählt

### Songtexte

Dieses Fenster zeigt die Text-Metadaten der geladenen MIDI-Datei an und filtert die 
Daten mit einer Reihe von Steuerelementen in der Symbolleiste des Fensters:

* Track: ein Listenfeld zur Auswahl zwischen "Alle Tracks" oder einem entsprechenden Eintrag
  jede verwendete Spur in der geladenen MIDI-Datei. Diese Titelnummer ersetzt die
  Konzept des MIDI-Kanals in diesem Fenster, da die Text-Metadaten kein 
  ein Kanalattribut, aber es gehört immer zu einer Spur. Standardmäßig ist die
  Track mit mehr Daten wird automatisch ausgewählt
* Typ: ein Listenfeld, um einen der Metadaten-Texttypen oder alle auszuwählen. Das
  Der Typ "Lyric Events" ist die wahrscheinlichste Wahl
* Kodierung: Die Textkodierung wird automatisch erkannt, in einigen Fällen jedoch die
  Erkennung kann falsch sein, daher kann die richtige manuell ausgewählt werden

Es gibt ein Fenstermenü mit den folgenden Optionen:

* In Zwischenablage kopieren: kopiert die aktuell im Fenster angezeigten Metadaten
* In Datei speichern... Nach dem Öffnen des Dialogs Datei speichern erstellt diese Option einen Text 
  Datei auf der Festplatte, mit dem im Fenster angezeigten Text und der ausgewählten Kodierung
* Drucken... Nach dem Öffnen eines Druckdialogs druckt diese Option den angezeigten Text aus
  das Fenster auf dem ausgewählten Drucker
* Vollbild: Vergrößert das Fenster, das den aktuellen Monitor ausfüllt
* Schriftart... Öffnet ein Fenster zur Auswahl von Schriftarten, um die Typografie und Größe für auszuwählen
  die Fenstertexte

##Wiedergabelisten

Playlists sind Sammlungen von Dateinamen, die zum Abspielen einer Serie verwendet werden können 
von MIDI-Stücken, wie ein musikalisches "Repertoire".

Die Aktion `File`->`Play List...` verwaltet das Erstellen, Ändern, Ordnen, 
Öffnen und Speichern von Playlist-Dateien. Dieses Fenster zeigt die aktuelle Playlist
Dateiname im Fenstertitel.

Die Aktionen `File`->`Next` und `File`->`Prev` werden verwendet, um in der Playlist zu navigieren
manuell vorwärts und rückwärts. Es gibt Konfigurationsoptionen, um die Wiedergabe zu starten
automatisch nach dem Laden von MIDI-Dateien und automatisch zum nächsten 
Playlist-Element, wenn die Wiedergabe des aktuellen Elements beendet ist. Beide Konfigurationsmöglichkeiten
sind standardmäßig aktiviert.

Die anfängliche Standard-Playlist enthält eine Sammlung von Beispiel-MIDI-Dateien. Ob
Wenn Sie lieber mit einer leeren Liste beginnen möchten, verwenden Sie die Schaltfläche `Löschen` und
Speichern Sie dann die leere Playlist unter einem aussagekräftigen Namen. Das wird das Neue
erste Playlist in der Zukunft.

Die zuletzt geöffnete oder gespeicherte Playlist-Datei soll beim nächsten Mal gespeichert werden 
Programm gestartet, aber Playlists werden nicht automatisch gespeichert.

Playlists sind einfache Textdateien mit einem Dateinamen in jeder Textzeile. Die Datei
Namen können relativ zum Speicherort der Playlist-Datei selbst sein (also 
kein absoluter Pfad vorangestellt) oder jede Datei kann einen absoluten Pfad haben 
seine eigene.

Möglicherweise haben Sie mehrere Playlists, die Sie ganz einfach außerhalb des 
Programm, zum Beispiel in der Linux-Befehlszeile erstellt dieser Befehl eine Wiedergabeliste 
Datei namens "my_playlist.lst" mit allen MIDI-Dateien, die sich auf der Arbeitsumgebung befinden 
Verzeichnis:

~~~
$ ls -1 *.mid > my_playlist.lst
~~~

## MIDI-Dateien öffnen

Sie können einzelne Dateien öffnen...

* Verwenden der Standardaktion `Datei`->`Öffnen`
* Verwenden des Menüs `Datei`->`Zuletzt geöffnete Dateien`, das bis zu zehn Dateien speichert 
  vor kurzem eröffnet.
* Geben Sie den/die Dateinamen in der Befehlszeile an. Der Satz von Dateien wird a
  temporäre Playlist. Dies ermöglicht eine einfache Integration des Programms mit Datei
  Manager, die "Öffnen mit..."-Aktionen verwenden.
* Ziehen und Ablegen von Dateien aus einem Dateimanager in das Hauptfenster. Das
  Satz von Dateien wird zu einer temporären Wiedergabeliste.

## Anpassung

### Einstellungen

Es gibt eine Schaltfläche "Standardeinstellungen wiederherstellen", um alle Optionen in diesem Dialogfeld zu initialisieren
auf ihre Standardwerte.

Es gibt drei Registerkarten, die die Konfigurationseinstellungen gruppieren

#### Allgemein

* MIDI-Kanal für Percussion: Nummer von 1 bis 16. Standard 10, entsprechend dem
  General-MIDI-Standard-Percussion-Kanal
* Starten Sie die Wiedergabe automatisch nach dem Laden. Standardmäßig aktiviert
* Automatisch zum nächsten Playlist-Element vorrücken. Standardmäßig aktiviert
* Dunklen Modus erzwingen. Kann unter Windows nützlich sein. Unter Linux und macOS ist der Dunkelmodus
  wird automatisch angewendet, wenn im Betriebssystem konfiguriert
* Verwenden Sie das interne Symboldesign. Diese Einstellung wird unter Windows und macOS benötigt.
* Qt-Widgets-Stil. Beeinflusst je nach Betriebssystem die visuelle
  Integration mit anderen Programmen
* MIDI System Exclusive Reset. Der Spieler sendet diese Nachricht kurz vor dem Start
  Abspielen jeder MIDI-Datei 

#### Songtexte

* Schriftart. Öffnet ein Fenster zur Schriftartenauswahl, um die Typografie und Größe für auszuwählen
  die Fenstertexte. Diese Einstellung ist auch im Menü des Songtext-Fensters verfügbar
* Zukünftige Textfarbe. Diese Einstellung gilt für den nicht abgespielten Text
* Frühere Textfarbe. Diese Einstellung gilt für den bereits abgespielten Text

#### Spieler Klavier

* Hervorhebung beachten. Es stehen einige Hervorhebungspaletten zur Verfügung
* Einzelne Hervorhebungsfarbe. Wenn die einfarbige Hervorhebungspalette ausgewählt ist,
  Diese Einstellung ist die Farbe zur Hervorhebung von Notizen
* Beachten Sie die Velocity zum Farbton. Mit dieser Option werden MIDI-Noten-Events Velocity
  Attribut wird verwendet, um der Hervorhebungsfarbe einen helleren oder dunkleren Ton zu geben
* Beachten Sie die Schriftart der Namen. Öffnet ein Fenster zur Schriftartenauswahl, um die Typografie auszuwählen und
  Größe der Notennamen
* Notennamen anzeigen: Verfügbare Optionen sind Nie, Minimal, Wenn aktiviert, immer.
  Die Option Minimal zeigt nur die Namen der C-Schlüssel an.

### Anpassung der Symbolleiste

Die Symbolleiste befindet sich im Hauptfenster, das typischerweise sehr kleine 
verfügbaren Platz und kann an den oberen oder unteren Rand des Fensters verschoben werden, und 
auch außerhalb davon. Die als Werkzeugschaltflächen verfügbaren Aktionen können in . konfiguriert werden
dieses Dialogfenster. Alle Aktionen sind auch Hauptmenüpunkte.

* Verfügbare Aktionen: Zeigt die Liste der Aktionen an, die derzeit nicht angezeigt werden als
  Werkzeugschaltflächen
* Ausgewählte Aktionen: ist die Liste der aktuell ausgewählten Werkzeugschaltflächen

Um eine Aktion aus der Liste Verfügbar auszuwählen, wählen Sie sie zuerst mit der Maus aus und 
drücken Sie dann die Schaltfläche "Hinzufügen". Die Aktion wird aus der Liste Verfügbar entfernt und
am Ende der ausgewählten Liste platziert.

Um eine Schaltfläche aus der Symbolleiste zu entfernen, wählen Sie sie mit der Maus im Ausgewählten 
Liste und klicken Sie dann auf die Schaltfläche "Entfernen".

Um die Reihenfolge der Schaltflächen in der Symbolleiste neu zu organisieren, verwenden Sie die Tasten "Auf" und "Ab" 
Schaltflächen, nachdem Sie ein Element in der Liste Ausgewählt ausgewählt haben.

Stil der Werkzeugschaltflächen: enthält die folgenden Optionen, die für alle gelten 
die Schaltflächen in der Werkzeugleiste

* Nur Symbol
* Nur Text
* Text neben dem Symbol
* Text unter Symbol
* Style folgen: Die Einstellung wird durch die Einstellung "Qt Widgets Style" bestimmt
  im Dialog Einstellungen.

