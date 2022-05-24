% Indice di aiuto

## Introduzione

Questa applicazione è un riproduttore di file MIDI multi piattaforma per Linux, Windows e
Mac OS. Legge i formati di file .MID (file MIDI standard), .KAR (Karaoke) e .WRK (Cakewalk)
e invia gli eventi MIDI alle porte MIDI hardware e anche a sintetizzatori software.

[Drumstick](https://drumstick.sourceforge.io) è un set di librerie C++/Qt con licenza GPLv3 per applicazioni MIDI.
Il progetto include diversi strumenti come esempi, e tra questi c'è l'utility drumstick-guiplayer che
sfrutta la libreria Drumstick::ALSA; l'utility è quindi disponibile solo per Linux (perché il sequencer ALSA è una
tecnologia presente solo su Linux).
Alcune persone hanno richiesto un programma per Windows e macOS con le stesse funzionalità di drumstick-guiplayer,
ed eccolo qui. Tuttavia dmidiplayer è molto di più dell'utility compresa in Drumstick e funziona anche su Linux...

Alcune caratteristiche chiave:

* Uscita MIDI su porte MIDI hardware, o su qualsiasi altro backend Drumstick, come ad esempio un
  sintetizzatore software.
* [Trasposizione](https://it.wikipedia.org/wiki/Trasposizione_(musica)) della tonalità della musica
  tra -12 e +12 semitoni.
* Cambio del livello del volume MIDI (usando il comando MIDI CC7).
* Variazione della velocità della musica tra la metà e il doppio del [tempo](https://it.wikipedia.org/wiki/Tempo_(musica)).
* Visualizzazione dei testi, della tastiera e dei canali MIDI.
* Supporto dei formati di file MID/KAR (file MIDI standard) e WRK (Cakewalk).

## Selezionare l'uscita MIDI

Ogni riproduttore MIDI emette solo eventi MIDI, non suoni ascoltabili. Per produrre un suono effettivo, è necessario
un sintetizzatore MIDI: esso trasforma gli eventi MIDI in ingresso in audio digitale che può essere
riprodotto dagli altoparlanti del computer (o esterni).
Questo ruolo può essere svolto o da uno strumento musicale MIDI esterno collegato al computer (ad esempio un organo,
un pianoforte o un sintetizzatore MIDI con un'interfaccia USB) oppure, se l'opzione precedente non è disponibile,
da un sintetizzatore software (cioè da un programma).

Vi sono sintetizzatori software già inclusi nel sistema operativo del computer:
ad esempio Microsoft Windows include "Microsoft GS Wavetable Synth" e Apple macOS include "Apple DLS Synthesizer".
Per usare uno di questi, bisogna aprire la finestra di dialogo "Impostazione MIDI" di dmidiplayer e quindi:

* In **Windows**, selezionare l'opzione "Windows MM" nella prima casella di riepilogo e "Microsoft GS Wavetable Synth" nella seconda.
  A questo programma, che è molto semplice e con limitate capacità, è preferibile un'alternativa migliore e gratuita come
  [Virtual MIDI Synth](https://coolsoft.altervista.org/it/virtualmidisynth); se installato, esso verrà visualizzato come "VirtualMIDISynth #n" nella seconda casella di riepilogo.
  Se invece si ha disposizione uno strumento MIDI esterno collegato al computer, anche il suo nome dovrebbe apparire stessa casella.

* In **macOS**, selezionare "DLS Synth" sia nella prima che nella seconda casella di riepilogo.
  Questo programma ha un'opzione di configurazione per l'utilizzo di file sound font DLS o SF2 che, se
  lo si vuole, può essere utilizzata. Invece, per utilizzare un dispositivo MIDI esterno collegato, oppure un sintetizzatore software,
  basta selezionare "CoreMIDI" nella prima casella di riepilogo e il nome del tuo hardware o del programma sintetizzatore nel secondo.

* **Linux** e altri sistemi operativi **Unix** non dispongono di sintetizzatori software di default; è tuttavia possibile installarli.
  Tra i tanti disponibili si possono segnalare: "SonivoxEAS": un sintetizzatore GM pronto per essere utilizzato, subito dopo l'installazione; è molto semplice, non usa file e dipende solo dal server audio di sistema PulseAudio. Per utilizzarlo (come pure per usare dispositivi esterni collegati al computer o sintetizzatori software arbitrari) è necessario selezionare "ALSA" (o "OSS") nella prima casella di riepilogo.

* [**FluidSynth**](https://github.com/FluidSynth/fluidsynth) è un altro sintetizzatore open software, multi piattaforma e disponibile su tutti
  i sistemi operativi. È molto configurabile, accetta file sound font e può dialogare con molti sistemi audio digitali.
  Per utilizzarlo bisogna selezionare "FluidSynth" in entrambe le caselle di riepilogo.

* **Network**: dmidiplayer dispone anche di un'uscita UDP Multicast multi piattaforma, che può essere utilizzata per inviare eventi MIDI come
  pacchetti di rete verso altri computer e dispositivi nella stessa rete locale. È compatibile con ["ipMIDI"](https://www.nerds.de/en/ipmidi.html),
  ["QmidiNet"](https://qmidinet.sourceforge.io/) e ["multimidicast"](https://llg.cubic.org/tools/multimidicast/).


## Controlli di riproduzione

Le seguenti azioni standard sono disponibili nel menù `File`, ed anche nella barra degli strumenti,
che contiene le funzionalità convenzionali su qualsiasi lettore multimediale:

* `Riproduci`
* `Pausa` (e continua)
* `Ferma`
* `Avanti veloce` (di 1 posizione)
* `Riavvolgi` (di 1 posizione)

Per impostazione predefinita, "Riproduci" viene attivato automaticamente al caricamento di un file MIDI tranne
quando questa funzione è stata disabilitata nella finestra di dialogo di configurazione.

La `barra di stato`, facoltativamente situata nella parte inferiore della finestra principale, mostra
lo stato di riproduzione corrente come "Riproduzione", "Interrotto", "In pausa", ecc.


### Trasposizione

Il `Controllo del passo` si trova nella finestra principale. È una casella di selezione inizializzata
a zero, con un range valido da -12 a +12 semitoni. I limiti sono equivalenti ad un'ottava sopra o sotto il tono originale.
Questo dovrebbe essere sufficiente per cantanti o strumentisti che abbiano bisogno di regolare la tonalità per il loro comfort.

Tutti i canali MIDI sono trasposti cromaticamente, eccetto il canale percussioni (canale GM 10 per impostazione predefinita).

### Livello del volume
 
Il `Controllo del volume` si trova nella finestra principale. È un cursore, inizializzato al 100%, con un intervallo valido che va
da 0% a 200%. L'effetto del limite superiore potrebbe essere inferiore perché il MIDI CC 7 ha un range valido da 0 a 127.
C'è anche un pulsante per "Ripristina volume".

### Tempo

Il `Controllo del tempo` si trova nella finestra principale. È un cursore inizializzato al 100%, con un intervallo valido che va dal 50%
al 200%. Ci sono inoltre un pulsante per "Ripristina tempo" ed un display, che mostra il tempo effettivo in battiti al minuto con un valore
iniziale di 120 bpm. Questo è il tempo predefinito per un file MIDI (se non contiene un cambio di tempo).
Il valore mostrato viene aggiornato quando si cambia il controllo del tempo e anche durante la riproduzione di un file MIDI contenente cambiamenti di tempo.

### Salto, Ripetizione e posizionamento

L'azione `Salta` (che si trova nel menù `File` e nella barra degli strumenti predefinita), richiede un numero di posizione, compreso tra 1 e l'ultima battuta del brano caricato, per cambiare la posizione del brano da quella corrente a quella della battuta indicata.

L'azione `Ripetizione` (che si trova nel menù `File` e nella barra degli strumenti predefinita), richiede due numeri di posizione.
La musica verrà riprodotta tra le due posizioni indicate quando il Loop è attivato. Si può disattivarlo in qualsiasi momento selezionando nuovamente l'azione.

Il `Controllo del posizionamento` si trova nella finestra principale. È un cursore che si muove mentre cambia la posizione di riproduzione corrente e può essere spostato manualmente per cambiare la posizione in qualsiasi punto arbitrario all'interno della musica.

## Visualizzazioni

Le seguenti opzioni sono abilitate nel menù `Visualizza`:

Tre sono finestre indipendenti che si possono aprire e chiudere a piacimento: Canali, Testi e Tastiera.
Vi sono poi altre tre, incorporate nella finestra principale: Barra degli strumenti, Barra di stato e Ritmo.
Anch'esse possono essere nascoste o mostrate a piacere.

### Canali

Questa finestra mostra fino a 16 righe, una per ogni canale MIDI utilizzato dal corrente File MIDI.
Le colonne sono:

* Numero di canale MIDI e del testo (modificabile). Di solito il nome dello strumento, o una breve descrizione tratta dai meta dati del file.
* Pulsante "Muto"". Con questa opzione il canale correntemente selezionato può essere silenziato.
* Pulsante "Solo". Alza il volume del canale selezionato, mentre abbassa quello di tutti gli altri canali.
* Livello. Indicatore dell'attività sul canale correntemente selezionato.
* Pulsante "Blocca". Questo pulsante corregge la Patch sul canale corrente, impedendo modifiche eventualmente memorizzate nel file.
* Patch. Corrisponde agli eventi di Program Change MIDI secondo l'Elenco Generale dei nomi degli strumenti MIDI.

### Tastiera

Questa finestra mostra fino a 16 righe, una per ogni canale MIDI utilizzato dal corrente file MIDI.
Ogni riga contiene:

* Un numero di canale e del testo (gli stessi dati della finestra Canali).
* Una tastiera di pianoforte interattiva. I tasti saranno evidenziati con un colore, che può essere completamente personalizzato, a partire dagli eventi
  di nota MIDI suonati dal file MIDI corrente. La tastiera può facoltativamente anche i nomi delle note suonate.
  I tasti possono inoltre essere attivati manualmente utilizzando la tastiera del computer e il mouse.

La finestra ha anche un menù, contenente le seguenti opzioni:

* Visualizza a schermo intero. Ingrandisce la finestra riempiendo completamente il monitor corrente.
* Mostra tutti i canali.
* Nascondi tutti i canali.
* Riduci il numero di tasti. Per impostazione predefinita, ogni tastiera mostra 88 tasti, da A0 a C8. Usando questa opzione le tastiere usano solo
  il numero di ottave realmente utilizzato dal file MIDI, che di solito è più piccolo.
* Fino a 16 Opzioni di canale, per nascondere o mostrare ogni canale individualmente. Di default tutti i canali MIDI utilizzati nel file MIDI corrente
  sono selezionati.

### Testi

Questa finestra mostra i meta dati di testo dal file MIDI caricato, filtrando i dati con una serie di controlli posti sulla
barra degli strumenti della finestra:

* Traccia: una casella di riepilogo per scegliere tra "Tutte le tracce" o un elemento corrispondente ad una traccia fra quelle utilizzate nel file
  MIDI caricato. In questa finestra il numero di traccia sostituisce il concetto di canale MIDI, perché i meta dati di testo non hanno un attributo
  di canale, pur appartenendo sempre a qualche traccia. Per impostazione predefinita, la traccia contenente più dati viene selezionata automaticamente.
* Tipo: una casella di riepilogo per scegliere uno dei tipi di testo presenti nei meta dati, oppure tutti. Il tipo "Lyric Events" è la scelta più
  probabile.
* Codifica: la codifica del testo viene rilevata automaticamente, ma in alcuni casi il rilevamento potrebbe rivelarsi errato, quindi quello corretto
  può essere selezionato manualmente qui.

Anche questa finestra ha un menù, con le seguenti opzioni:

* Copia negli appunti: copia i meta dati attualmente mostrati nella finestra
* Salva nel file...: Dopo aver aperto una finestra di dialogo Salva file, questa opzione crea un file di testo sul disco, contenente il testo mostrato
  nella finestra e la codifica selezionata.
* Stampa...: Dopo aver aperto una finestra di dialogo Stampa, questa opzione stampa il testo mostrato nella finestra sulla stampante selezionata.
* Schermo intero: ingrandisce la finestra riempiendo completamente il monitor corrente.
* Carattere...: Apre una finestra di selezione dei caratteri per scegliere la tipo e la dimensione dei caratteri per i testi della finestra.

## Playlist

Le playlist sono raccolte di nomi di file che possono essere utilizzate per riprodurre una serie
di brani MIDI, come un "repertorio" musicale: sono semplici file di testo, con il nome di un file MIDI su ogni riga.
I nomi dei file da riprodurre possono essere relativi alla posizione del file della playlist (in questo caso non iniziano con un
percorso assoluto), oppure ognuno può avere un proprio percorso assoluto.

L'azione `File`->`Play List...` gestisce la creazione, la modifica, l'ordinamento, l'apertura e il salvataggio dei file dell'elenco di riproduzione.
Questa finestra mostra il nome del file della playlist attuale nel suo titolo.

Le azioni `File`->`Successivo` e `File`->`Prec` sono utilizzate per navigare manualmente avanti e indietro nella playlist corrente.
Vi sono opzioni di configurazione per avviare automaticamente la riproduzione della playlist, dopo aver caricato i file MIDI,
e per avanzare automaticamente al successivo elemento della playlist al termine della riproduzione dell'elemento corrente.
Entrambe le opzioni sono abilitate per impostazione predefinita.

La playlist predefinita iniziale contiene una raccolta di file MIDI di esempio.
Se si preferisce iniziare con un elenco vuoto, si deve usare il pulsante `Cancella` e 
quindi salvare la playlist vuota con un nome differente. Questa sarà la nuova playlist iniziale in futuro.

L'ultimo file di playlist aperto o salvato verrà ricordato la prossima volta che
il programma viene avviato; tuttavia, le playlist come tali non vengono salvate automaticamente.

Si possono avere diverse playlist da caricare; esse possono essere create facilmente anche al di fuori del programma.
Ad esempio in una finestra terminale di Linux il seguente comando crea un file di playlist, denominato "my_playlist.lst", che contiene
tutti i file MIDI che si trovano nella directory corrente:

~~~
$ ls -1 *.mid > mia_playlist.lst
~~~

## Apertura di file MIDI

Si possono aprire singoli file in vari modi:

* Usando l'azione standard `File`->`Apri`.
* Usando il menù `File`->`Apri file recenti`, che ricorda fino a dieci file aperti di recente.
* Fornendo dei nomi di file nella riga di comando. L'insieme di file così indicati diventa una playlist temporanea.
  Ciò consente una facile integrazione del programma con file manager che utilizzano le azioni "Apri con...".
* Trascinando e rilasciando uno o più file da un file manager nella finestra principale del programma. Il set di file diventa una playlist temporanea.

## Personalizzazione

### Preferenze

C'è un pulsante "Ripristina impostazioni predefinite" per inizializzare tutte le opzioni di questa finestra di dialogo ai loro valori predefiniti.

Ci sono tre schede, che raggruppano le impostazioni di configurazione.

#### Generale

* Canale MIDI delle percussioni: Numero da 1 a 16. Di default 10, corrispondente al Canale di percussioni standard MIDI generale.
* Avvia la riproduzione automaticamente dopo il caricamento. Abilitato per impostazione predefinita.
* Avanza automaticamente all'elemento successivo della playlist. Abilitato per impostazione predefinita.
* Forza Dark Mode. Può essere utile su Windows. Su Linux e macOS la modalità visuale è applicata automaticamente quando essa è configurata
  nel sistema operativo.
* Usa tema interno per le icone. Questa impostazione è necessaria su Windows e macOS.
* Stile widget Qt. A seconda del sistema operativo, influisce sulla integrazione visuale con altri programmi.
* Ripristino esclusivo del sistema MIDI. Il programma invia questo messaggio appena prima di iniziare riprodurre ogni file MIDI.

#### Testi

* Carattere del testo. Apre una finestra di selezione dei caratteri per scegliere il tipo e la dimensione per i testi delle finestre.
  Questa impostazione è disponibile anche nel menù della finestra Testi.
* Colore del testo da riprodurre. Questa impostazione si applica al testo che non è ancora stato riprodotto.
* Colore del testo riprodotto. Questa impostazione si applica al testo che è già stato riprodotto.

#### Tastiera

* Evidenziazione delle note. Sono disponibili alcune tavolozze di evidenziazione.
* Colore di evidenziazione singolo. Quando è selezionata la tavolozza di evidenziazione a colore singolo, questa impostazione governa il colore di
  evidenziazione della nota riprodotta.
* Tinta di colore secondo la velocità della nota. Con questa opzione la tinta viene utilizzata per conferire al colore di evidenziazione un tono
  più chiaro o più scuro, in funzione della la velocità degli eventi delle note MIDI.
* Carattere del nome delle note. Apre una finestra di selezione dei caratteri per scegliere il tipo e la dimensione dei nomi dei font usati per
  i nomi delle note.
* Mostra il nome delle note: le scelte disponibili sono Mai, Minimo, Quando attivato, Sempre. L'opzione Minimo mostra solo i nomi dei tasti C.

### Personalizzazione della barra degli strumenti

La barra degli strumenti si trova nella finestra principale, che in genere ha molto poco spazio disponibile, e per questo può essere spostata
nella sua parte superiore o inferiore, od anche al di fuori di essa.
Le azioni disponibili come pulsanti-strumento possono essere configurate in questa finestra di dialogo.
Tutte le azioni sono anche voci del menù principale.

* Azioni Disponibili: mostra l'elenco delle azioni attualmente non visualizzate come pulsanti-strumento.
* Azioni Selezionate: è l'elenco dei pulsanti-strumento attualmente selezionati e visualizzati nella barra degli strumenti.

Per aggiungere un'azione alla barra degli strumenti, bisogna prima selezionarla con il mouse nell'elenco Disponibili, e quindi premere il pulsante "Aggiungi". L'azione verrà rimossa dall'elenco Disponibili e posta alla fine dell'elenco Selezionate.

Per rimuovere un pulsante dalla barra degli strumenti, bisogna selezionarlo con il mouse nel riquadro Selezionati e quindi premere il pulsante
"Rimuovi".

Per riorganizzare l'ordine dei pulsanti nella barra degli strumenti, si utilizzano i pulsanti "su" e "giù", dopo aver selezionato un elemento nell'elenco Selezionati.

Stile dei pulsanti degli strumenti: contiene le seguenti opzioni, che si applicano a tutti i pulsanti nella barra degli strumenti.

* Solo icona.
* Solo testo.
* Testo accanto all'icona.
* Testo sotto l'icona.
* Segui stile: segue l'impostazione "Stile widget Qt" nella finestra di dialogo Preferenze. 
