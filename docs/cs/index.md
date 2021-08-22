% Index nápovědy

## Úvod

Tato aplikace je multiplatformní MIDI přehrávač souborů pro operační systémy Linux, Windows a 
macOS. Čte formáty souborů .MID (standardní MIDI soubory), .KAR (Karaoke) a .WRK (Cakewalk),
a dělá výstup událostí MIDI do hardwarových přípojek MIDI (portů) a také softwarových 
syntezátorů.

[Drumstick](https://drumstick.sourceforge.io) je sada GPLv3 
licencovaných C ++/Qt knihoven pro programy MIDI. Projekt zahrnuje několik
nástrojů jako příkladů. Mezi nimi je i nástroj drumstick-guiplayer, který 
využívá knihovnu Drumstick::ALSA, takže je dostupný pouze pro Linux 
(protože sekvencer ALSA je technologie pouze pro Linux). Někteří lidé
požadovali program se stejnými funkcemi ve Windows a macOS. A toto 
je ten program. Ale tento program je mnohem víc než to, a také funguje
v Linuxu...

Některé klíčové vlastnosti:

* MIDI výstup do hardwarových přípojek MIDI (portů) nebo do jakékoli jiné podpůrné vrstvy Drumstick, jako jsou softwarové syntetizátory
  syntezátory
* [Transpozice](https://cs.wikipedia.org/wiki/Transpozice_(hudba)) tóniny písně 
  mezi -12 a +12 půltónů
* Změna úrovně hlasitosti MIDI (pomocí MIDI CC7)
* Změna rychlosti skladby mezi polovičním a dvojnásobným 
  [tempem](https://cs.wikipedia.org/wiki/Tempo)
* Zobrazení zpěvních textů, klaviatury přehrávače a kanálů MIDI
* Podporuje formáty souborů MID/KAR (standardní soubory MIDI) a WRK (Cakewalk)

## Výstup MIDI

Všechny přehrávače MIDI provádí pouze výstup událostí MIDI, ale nepřehrávají zvuk. Chcete-li udělat zvuk,
pro zpracování událostí do digitálního zvuku, který může být slyšet ze zvukových reproduktorů počítače (nebo vnějších), je zapotřebí syntetizátor MIDI. Události také mohou
být zpracovány vnějším hudebním nástrojem MIDI připojeným k počítači, 
například MIDI varhany, klavír nebo syntezátor s rozhraním USB. Pokud tato
možnost není dostupná, další obvyklou alternativou je softwarový syntetizátor 
(program).

V operačních systémech počítače jsou přítomny softwarové syntetizátory, 
například Microsoft Windows obsahuje „Microsoft GS Wavetable Synth“ a
Apple macOS obsahuje „Apple DLS Synthesizer“. Chcete -li je použít, musíte otevřít
dialog „Nastavení MIDI“, a poté:

* V operačním systému **Windows** vyberte v prvním seznamu voleb „Windows MM“ a 
  „Microsoft GS Wavetable Synth“ ve druhém seznamu. Místo tohoto syntetizátoru,
  jenž je velmi základní a omezený, můžete upřednostnit jinou lepší/bezplatnou možnost, jakou je 
  [Virtual MIDI Synth](https://coolsoft.altervista.org/en/virtualmidisynth)
  která se při spuštění zobrazí ve druhém seznamu jako „VirtualMIDISynth #n“.
  Pokud máte k počítači připojen vnější nástroj MIDI, jeho název
  by se měl objevit i ve druhém seznamu.
* V **macOS** vyberte „DLS Synth“ v prvním i druhém seznamu. 
  Pokud ji budete chtít, tento syntetizátor má v nastavení volbu pro použití souborů zvukových bank
  DLS nebo SF2. Chcete-li použít jiné připojené zařízení MIDI nebo softwarový syntetizátor,
  v prvním seznamu musíte vybrat „CoreMIDI“ a název vašeho
  syntetizátoru ve druhém.
* V **Linuxu** a dalších **Unixových** operačních systémech je k použití připraven 
  softwarový syntetizátor „SonivoxEAS“. Je to naprosto nejzákladnější GM syntetizátor, který nepoužívá soubory
  a závisí pouze na zvukovém systému PulseAudio. Pro vnější zařízení připojená
  k počítači nebo libovolné softwarové syntezátory musítev prvním seznamu vybrat „ALSA“ 
  (nebo „OSS“).
* **FluidSynth** je další zahrnutý syntetizátor softwaru s otevřeným zdrojovým kódem, který je multiplatformní 
  a je dostupný pro všechny operační systémy. Je velmi nastavitelný, přijímající
  soubory zvukových bank a mnoho zvukových systémů digitálního zvuku. Pro jeho použití v obou seznamech 
  vyberte [FluidSynth](https://github.com/FluidSynth/fluidsynth).
* **Network** je výstup UDP Multicast určený pro více operačních systémů, který lze použít k odesílání 
  událostí MIDI, jako jsou síťové pakety k jiným počítačům a zařízením ve stejné 
  místní síti. Rozumí si mimo jiného software s
  [ipMIDI](https://www.nerds.de/en/ipmidi.html),
  [QmidiNet](https://qmidinet.sourceforge.io/) a 
  [multimidicast](https://llg.cubic.org/tools/multimidicast/).

## Ovládání přehrávání

Následující běžné činnosti jsou dostupné v nabídce `Soubor` a také
v panelu nástrojů s obecnými funkcemi v libovolném přehrávači záznamů: 

* `Přehrávat` 
* `Pozastavit` (a pokračovat)
* `Zastavit`, 
* `Rychle vpřed` (o 1 takt)
* `Přetočit zpět` (o 1 takt)

Ve výchozím nastavení se `Přehrávat` spustí automaticky po načtení souboru MIDI, kromě případu,
kdy je tato funkce v nastavovacím dialogu zakázána.

`Stavový řádek`, který je volitelně umístěn ve spodní části hlavního okna, 
ukazuje nynější stav přehrávání jako „Přehrává se“, „Zastaveno“, „Pozastaveno“ atd.

### Transponování

V hlavním okně se nachází `Ovládání výšky tónu`. Je to kolečko
ve výchozím stavu na nule, s platným rozsahem od -12 do +12 půltónů. Meze odpovídají
rozsahu o jednu oktávu nahoru nebo dolů od původního tónu. To by mělo stačit
zpěvákům nebo hráčům na hudební nástroje, kteří pro své pohodlí potřebují upravit tóninu.

Všechny kanály MIDI jsou chromaticky transponovány, kromě kanálu bicích 
(ve výchozím nastavení GM kanál 10).

### Úroveň hlasitosti
 
`Ovládání hlasitosti` se nachází v hlavním okně. Je to posuvník, inicializovaný
na 100 %, s platným rozsahem od 0 % do 200 %. Účinek horní hranice může být nižší
protože MIDI CC 7 má platný rozsah od 0 do 127. Kromě toho je tu tlačítko
`Obnovit výchozí hlasitost`.
 
### Tempo

`Ovládání tempa` se nachází v hlavním okně. Je to posuvník inicializovaný
na 100 %, s platným rozsahem od 50 % do 200 %. Je tu rovněž tlačítko `Obnovit výchozí tempo`
a také zobrazení tempa, ukazující skutečné tempo v úderech za minutu (M.M.) s
počáteční hodnotou 120 M.M. Toto je výchozí tempo pro soubor MIDI, pakliže
neobsahuje změnu tempa. Tato hodnota se obnovuje při změně
ovládání tempa, a také při přehrávání souboru MIDI obsahujícího změny tempa.

### Skok, přehrávání ve smyčce a polohování

Činnost `Skočit` (umístěná v nabídce `Soubor` a v pruhu s výchozími nástroji) vyžaduje
pro změnu současné polohy ve skladbě na daný takt zadat číslo taktu mezi 1 a posledním taktem nahrané skladby.

Činnost `Smyčkování` (umístěná v nabídce `Soubor` a v pruhu s výchozími nástroji) vyžaduje
zadání čísel dvou taktů. Skladba se bude, když je zapnuto, přehrávat mezi dvěma danými takty.
Smyčku můžete kdykoli vypnout opětovným výběrem činnosti.

`Ovládání polohování` se nachází v hlavním okně. Je to posuvník, který
se pohybuje, zatímco se mění nyní hrané místo, a lze jej ručně přesunout a tím
změnit polohu na libovolný časový bod skladby.

## Zobrazení

V nabídce `Zobrazit` jsou dostupné následující volby:

Tři zobrazení jsou nezávislá okna, která můžete pro své pohodlí otevřít a zavřít:  
Kanály, zpěvní texty a přehrávání klavíru.

Na druhou stranu můžete skrýt nebo zobrazit i další tři zobrazení, která jsou vložena 
v hlavním okně: panel nástrojů, stavový řádek a rytmus.

### Kanály

Toto okno zobrazuje až 16 řádků, jeden pro každý kanál MIDI používaný nynějším
souborem MIDI. Sloupce jsou:

* Číslo a text kanálu MIDI (upravitelné). Obvykle název nástroje, nebo
  stručný popis převzatý z popisných dat souboru.
* Tlačítko pro ztlumení. Pomocí této volby lze nynější kanál ztlumit.
* Tlačítko pro sólo. Zvyšuje hlasitost nynějšího kanálu a zároveň snižuje hlasitost
  jiných kanálů.
* Úroveň. Ukazatel činnosti v nynějším kanálu.
* Zámek. Toto tlačítko opraví zapojení v nynějším kanálu a zabrání uložení změn
  do souboru.
* Zapojení. Odpovídá událostem změn programů MIDI podle názvů nástrojů v seznamu General
  MIDI.

### Přehrávač klavíru

Toto okno zobrazuje až 16 řádků, jeden pro každý kanál MIDI používaný nynějším
souborem MIDI. Každý řádek obsahuje:

* Číslo kanálu a text (stejná data jako v okně s kanály)
* Klavírní klávesnice. Klávesy budou zvýrazněny barvou, která může být plně
  přizpůsobena, podle událostí not MIDI hraných nynějším souborem MIDI. Volitelně také může
  zobrazit názvy not. Klávesy mohou být spuštěny
  ručně pomocí počítačové klávesnice a myši

Okenní nabídka obsahuje následující volby:

* Zobrazit na celou obrazovku. Zvětší okno vyplňující nynější obrazovku
* Zobrazit všechny kanály
* Skrýt všechny kanály
* Zmenšit počet kláves. Ve výchozím nastavení každá klávesnice zobrazuje 88 kláves od A0
  do C8. Pomocí této volby klávesnice používají pouze počet oktáv, jejž soubor MIDI skutečně
  používá, což je obvykle menší číslo
* Až 16 voleb kanálů, pro jednotlivé skrytí nebo zobrazení každého kanálu. Ve
  výchozím nastavení jsou vybrány všechny kanály MIDI použité v nynějším souboru MIDI

### Zpěvní texty

Toto okno zobrazuje textová popisná data z nahraného souboru MIDI a data filtruje 
řadou ovládacích prvků umístěných v okně v panelu nástrojů:

* Skladba: pole se seznamem s výběrem mezi „Všechny skladby“ a jednou položkou, které odpovídá
  každá použitá skladba v nahraném souboru MIDI. Toto číslo skladby nahrazuje
  v tomto okně pojetí kanálu MIDI, protože textová popisná data nemají 
  vlastnost kanálu, ale vždy patří k nějaké skladbě. Ve výchozím nastavení se
  automaticky vybere skladba obsahující více dat
* Typ: pole seznamu pro výběr jednoho z typů textu popisných dat nebo všech.
  Typ `Události zpěvních textů` je nejpravděpodobnější volbou
* Kódování: kódování textu je rozpoznáváno automaticky, ale v některých případech
  může být zjištění nesprávné, takže správné lze vybrat ručně

Okenní nabídka obsahuje následující volby:

* Kopírovat do schránky: Zkopíruje popisná data nyní zobrazená v okně
* Uložit do souboru... Po otevření dialogového okna pro uložení souboru tato volba na disku vytvoří textový 
  soubor s textem zobrazeným oknem a zvoleným kódováním
* Tisk... Po otevření dialogového okna pro tisk tato volba vytiskne text zobrazený v
  okně na vybrané tiskárně
* Celá obrazovka: Zvětší okno vyplňující nynější obrazovku
* Písmo ... Otevírá okno pro výběr písma, kde můžete vybrat písmo a jeho velikost
  v textech oken

## Seznamy skladeb

Seznamy skladeb jsou sbírky názvů souborů, které lze použít k přehrání řady 
skladeb MIDI, jako hudební „zásobu“.

Činnost `Soubor` → `Seznam skladeb...` spravuje vytváření, úpravy, řazení, 
otevírání a ukládání souborů se seznamy skladeb. Toto okno zobrazuje nynější seznam skladeb
název souboru v názvu okna.

K vlastnoručnímu procházení seznamu skladeb dopředu a dozadu slouží činnost `Soubor` → `Další` a `Soubor` → `Předchozí`.
Jsou volby nastavení pro automatické spuštění přehrávání
po nahrání souborů MIDI, pro a automatický přechod na další 
položku v seznamu skladeb po dokončení přehrávání nynější položky. Obě volby nastavení
jsou povoleny ve výchozím nastavení.

Počáteční výchozí seznam skladeb obsahuje sbírku ukázkových souborů MIDI.
Pokud dáváte přednost zahájení s prázdným seznamem, použijte tlačítko `Vyprázdnit`, a
poté prázdný seznam skladeb uložte se smysluplným názvem. Teno bude v budoucnosti novým
počátečním seznamem skladeb.

Poslední otevřený nebo uložený soubor se seznamem skladeb si program bude pamatovat, až 
se příště spustí, ale seznamy skladeb se automaticky neukládají.

Seznamy skladeb jsou jednoduché textové soubory s názvem souboru na každém textovém řádku. Souborové
názvy mohou být relativní k umístění samotného souboru seznamu skladeb (takže 
nemají předpřipravenou absolutní cestu) nebo může mít každý soubor zapnutou vlastní absolutní cestu.

Můžete mít několik seznamů skladeb, které lze snadno vytvořit mimo 
program, například v příkazovém řádku Linuxu příkaz níže vytvoří seznam skladeb 
soubor s názvem "my_playlist.lst" se všemi soubory MIDI umístěnými v pracovním 
adresáři:

~~~
$ ls -1 *.mid> my_playlist.lst
~~~

## Otevírání souborů MIDI

Jednotlivé soubory můžete otevřít:

* Pomocí běžné činnosti `Soubor` → `Otevřít`
* Pomocí nabídky `Soubor` → `Otevřít nedávné soubory`, která si pamatuje až deset nedávno otevřených souborů.
* Zadáním názvů souborů v příkazovém řádku. Sada souborů se stává
  dočasným seznamem skladeb. To umožňuje snadné propojení programu se správci souborů
  pomocí činnosti `Otevřít s...`.
* Přetahování souborů ze správce souborů do hlavního okna.
  Sada souborů se stane dočasným seznamem skladeb.

## Přizpůsobení

### Nastavení

V tomto dialogu je tlačítko `Obnovit výchozí nastavení` pro vrácení všech voleb
na jejich výchozí hodnoty.

Nastavení nastavení seskupují tři karty 

#### Obecné

* Kanál MIDI bicích: Číslo od 1 do 16. Výchozí je 10, odpovídá
  Standardu General MIDI pro bicí kanál
* Spustit přehrávání automaticky po nahrání. Ve výchozím nastavení povoleno
* Automaticky postoupit na další položku seznamu skladeb. Ve výchozím nastavení povoleno
* Vynutit tmavý režim. Může být užitečné v operačním systému Windows. V Linuxu a macOS se tmavý režim
  použije automaticky, je-li nastaven v operačním systému
* Použít vnitřní motiv ikon. Toto nastavení je potřeba ve Windows a macOS.
* Styl prvků Qt. V závislosti na operačním systému ovlivňuje vzhledové
  sjednocení s jinými programy
* Obnovení výchozího nastavení systému exkluzivní pro MIDI. Tuto zprávu přehrávač odešle těsně před spuštěním
  přehrávání každého souboru MIDI 

#### Zpěvní texty

* Písmo textu. Otevře okno Výběr písma, kde můžete vybrat typografii a velikost
  texty oken. Toto nastavení je k dispozici také v nabídce okna Texty
* Budoucí barva textu. Toto nastavení platí pro text, který nebyl přehrán
* Předchozí barva textu. Toto nastavení platí pro již přehrávaný text

#### Klaviatura v přehřávači

* Zvýraznění not. Dostupných je několik palet zvýraznění
* Jedna zvýrazňující barva. Když je vybrána paleta pro zvýraznění jednou barvou,
  toto nastavení je barva zvýrazňující notu
* Velocity noty s barevným odstínem. S touto volbou se vlastnost velocity noty MIDI používá
  používá k zesvětlení nebo ztmavení zvýrazňující barvy
* Písmo názvů not. Otevře okno pro výběr písma, kde vyberete písmo a
  velikost názvů not
* Zobrazit názvy not: dostupné možnosti jsou Nikdy, Minimální, Při zapnutí, Vždy.
  Volba Minimální zobrazuje pouze názvy kláves C.

### Přizpůsobení panelu nástrojů

Panel nástrojů se nachází v hlavním okně, které má obvykle velmi málo 
volného místa, a lze jej přesunout do horní nebo dolní části okna, a 
také mimo ně. Dostupné činnosti, jako jsou tlačítka nástrojů, lze nastavovat v
tomto dialogovém okně. Všechny činnosti jsou také položkami hlavní nabídky.

* Dostupné činnosti: zobrazuje seznam činností, které se nyní nezobrazují, jako
  jsou tlačítka nástrojů
* Vybrané činnosti: je seznam nyní vybraných tlačítek nástrojů

Pro vybrání jedné činnosti ze seznamu dostupných činností ji nejprve vyberte myší, a 
poté stiskněte tlačítko „Přidat“. Činnost je odebrána ze seznamu dostupných činností a
umístěna na konec vybraného seznamu.

Pro odebrání jednoho tlačítka z panelu nástrojů je vyberte myší v seznamu vybraných, 
a poté stiskněte tlačítko „Odebrat“.

Poro přeuspořádání pořadí tlačítek v panelu nástrojů použijte tlačítko „Nahoru“ a „Dolů“, 
poté co položku vyberete v seznamu vybraných.

Styl tlačítek nástrojů: obsahuje následující možnosti, které platí pro všechna 
tlačítka v panelu nástrojů

* Pouze ikona
* Pouze text
* Text vedle ikony
* Text pod ikonou
* Následovat styl: Nastavení je určeno nastavením „Stylu prvků Qt“
  v dialogu Nastavení.