% Index nápovědy

## Úvod

Tato aplikace je multiplatformní MIDI přehrávač souborů pro Linux, Windows a 
Operační Systém Mac. Čte .MID (standardní MIDI soubory), .KAR (Karaoke) a .WRK (Cakewalk)
formáty souborů a výstupy MIDI událostí do hardwarových MIDI portů a také softwaru 
syntezátory.

[Drumstick](https://drumstick.sourceforge.io) je sada GPLv3 
licencované C ++/Qt knihovny pro MIDI aplikace. Projekt zahrnuje několik
nástroje jako příklady, a mezi nimi je i nástroj palička-guiplayer, který 
využívá knihovnu Drumstick :: ALSA, takže je k dispozici pouze pro Linux 
(protože sekvencer ALSA je technologie pouze pro Linux). Někteří lidé mají
požadoval program se stejnými funkcemi ve Windows a macOS a 
tady to je. Ale tento program je mnohem víc než to a také funguje
Linux ...

Některé klíčové vlastnosti:

* MIDI výstup na hardwarové MIDI porty nebo jakýkoli jiný Drumstick backend jako soft
  syntezátory
* [Transpozice](https://cs.wikipedia.org/wiki/Transpozice_(hudba)) tonalita písně 
  mezi -12 a +12 půltónů
* Změňte úroveň hlasitosti MIDI (pomocí MIDI CC7)
* Měřítko rychlosti skladby mezi polovinou a dvojnásobkem 
  [tempo](https://cs.wikipedia.org/wiki/Tempo)
* Zobrazení textů, klavírního přehrávače a MIDI kanálů
* Podporuje formáty souborů MID/KAR (Standard MIDI Files) a WRK (Cakewalk)

## MIDI výstup

Jakýkoli přehrávač MIDI vydává pouze události MIDI, ale žádný zvuk. Chcete -li vytvářet zvuk, MIDI
K vykreslení událostí do digitálního zvuku, který může být, je zapotřebí syntetizátor 
reprodukovány zvukovými reproduktory počítače (nebo externími). Události také mohou
vykreslení externím hudebním MIDI nástrojem připojeným k počítači, 
například MIDI varhany, piano nebo syntezátor s rozhraním USB. Pokud to
možnost není k dispozici, další obvyklou alternativou je softwarový syntetizátor 
(program).

V operačních systémech počítače jsou softwarové syntetizéry, 
například Microsoft Windows obsahuje „Microsoft GS Wavetable Synth“ a
Apple macOS obsahuje „Apple DLS Synthesizer“. Chcete -li je použít, musíte otevřít
dialog „Nastavení MIDI“ a poté:

* V **Windows** vyberte v prvním seznamu možnost „Windows MM“ a 
  „Microsoft GS Wavetable Synth“ ve druhém seznamu. Místo tohoto syntetizátoru
  což je velmi základní a omezené, můžete upřednostnit lepší/bezplatnou alternativu jako 
  [Virtual MIDI Synth](https://coolsoft.altervista.org/en/virtualmidisynth)
  které se při spuštění zobrazí jako „VirtualMIDISynth #n“ ve druhém seznamu.
  Pokud máte k počítači připojen externí MIDI nástroj, jeho název
  by se měl objevit i ve druhém seznamu.
* V **macOS** vyberte „DLS Synth“ v prvním i druhém seznamu. 
  Tento syntetizátor má možnost konfigurace pro použití souborů zvukových písem DLS nebo SF2, pokud 
  ty chceš. Chcete -li použít jiné připojené MIDI zařízení nebo softwarový syntetizátor,
  v prvním seznamu musíte vybrat „CoreMIDI“ a název vašeho
  syntetizátor ve druhém.
* V **Linux** a dalších **Unix** operačních systémech je „SonivoxEAS“ měkký 
  synth připraven k použití. Je to velmi základní GM syntetizátor, který nepoužívá soubory
  a závisí pouze na zvukovém systému PulseAudio. Pro externí zařízení připojeno
  k počítači nebo libovolným softwarovým syntezátorům musíte vybrat „ALSA“ (nebo „OSS“) 
  v prvním seznamu.
* **FluidSynth** je další otevřený syntetizátor softwaru, který je multiplatformní 
  a je k dispozici ve všech operačních systémech. Je to velmi konfigurovatelné, přijímající
  zvukové soubory a mnoho digitálních zvukových zvukových systémů. Vybrat
  [FluidSynth](https://github.com/FluidSynth/fluidsynth) v obou seznamech 
  použít to.
* **Network** je multiplatformní výstup UDP Multicast, který lze použít k odeslání 
  Události MIDI jako síťové pakety k jiným počítačům a zařízením ve stejném 
  místní síť. Je kompatibilní s
  [ipMIDI](https://www.nerds.de/en/ipmidi.html),
  [QmidiNet](https://qmidinet.sourceforge.io/) a 
  [multimidicast](https://llg.cubic.org/tools/multimidicast/) 
  mimo jiné software.

## Ovládání přehrávání

Následující standardní akce jsou k dispozici v nabídce `Soubor 'a také
panel nástrojů s konvenčními funkcemi v libovolném přehrávači médií: 

* `Hrát` 
* `Pozastavit` (a pokračovat)
* "Zastavit", 
* „Rychle vpřed“ (o 1 bar)
* "Převinout zpět" (o 1 bar)

Ve výchozím nastavení se „Play“ aktivuje automaticky po načtení souboru MIDI kromě 
když je tato funkce v konfiguračním dialogu zakázána.

Zobrazí se `stavový řádek`, který je volitelně umístěn ve spodní části hlavního okna 
aktuální stav přehrávání jako „Přehrávání“, „Zastaveno“, „Pozastaveno“ atd.

### Transponovat

V hlavním okně se nachází `Pitch control`. Je to inicializovaný spin box
na nulu, s platným rozsahem od -12 do +12 půltónů. Limity jsou ekvivalentní
o jednu oktávu nahoru nebo dolů původní tón. To by zpěvákům mělo stačit
nebo instrumentalisté, kteří potřebují upravit tonalitu pro své pohodlí.

Všechny MIDI kanály jsou chromaticky transponovány, kromě perkusního kanálu 
(GM kanál 10 ve výchozím nastavení).

### Úroveň hlasitosti
 
"Ovládání hlasitosti" se nachází v hlavním okně. Je to posuvník, inicializovaný
až 100%, s platným rozsahem od 0%do 200%. Účinek horní hranice může být nižší
protože MIDI CC 7 má platný rozsah od 0 do 127. K dispozici je také tlačítko
`Obnovit hlasitost`.
 
### Tempo

`Tempo control` se nachází v hlavním okně. Je to posuvník inicializovaný
do 100% s platným rozsahem od 50% do 200%. K dispozici je tlačítko „Obnovit tempo“
a také zobrazení tempa, ukazující efektivní tempo v taktech za minutu s
počáteční hodnota 120 tepů za minutu. Toto je výchozí tempo pro MIDI soubor
neobsahuje změnu tempa. Tato hodnota se aktualizuje při změně
ovládání tempa a také při přehrávání souboru MIDI obsahující změny tempa.

### Skok, smyčka a polohování

Akce „Přejít“ (umístěná v nabídce „Soubor“ a na výchozí liště nástrojů), vyžaduje
číslo taktu, mezi 1 a posledním pruhem načtené skladby, pro změnu 
aktuální poloha skladby na daném pruhu.

Akce „Smyčka“ (umístěná v nabídce „Soubor“ a na výchozí liště nástrojů), vyžaduje
dvě čárková čísla. Píseň se bude hrát mezi dvěma danými takty, pokud je
aktivováno. Smyčku můžete kdykoli vypnout výběrem akce znovu.

`Ovládací prvek polohování 'se nachází v hlavním okně. Je to posuvník, který
se pohybuje, zatímco se mění aktuální hraná pozice, a lze jej ručně přesunout do
změňte polohu na libovolný bod v rámci skladby.

## Zobrazení

V nabídce `Zobrazit 'jsou povoleny následující možnosti:

Tři zobrazení jsou nezávislá okna, která můžete otevřít a zavřít ve svém 
pohodlí: Kanály, texty a klavírní přehrávač.

Na druhou stranu můžete také skrýt nebo zobrazit další tři zobrazení, která jsou vložená 
v hlavním okně: Panel nástrojů, Stavový řádek a Rytmus.

### Kanály

Toto okno zobrazuje až 16 řádků, jeden pro každý MIDI kanál používaný proudem
MIDI soubor. Sloupce jsou:

* Číslo a text MIDI kanálu (upravitelné). Obvykle název nástroje, nebo a
  stručný popis převzat z metadat souboru.
* Tlačítko ztlumení. Pomocí této možnosti lze aktuální kanál ztlumit.
* Tlačítko Solo. Zvyšuje hlasitost aktuálního kanálu a zároveň snižuje hlasitost
  jiné kanály.
* Úroveň. Indikátor aktivity na aktuálním kanálu.
* Zamknout. Toto tlačítko opraví Patch na aktuálním kanálu a zabrání změnám
  uloženy v souboru.
* Náplast. Odpovídá událostem MIDI Program Change podle General
  MIDI seznam názvů nástrojů.

### Pianola

Toto okno zobrazuje až 16 řádků, jeden pro každý MIDI kanál používaný proudem
MIDI soubor. Každý řádek obsahuje:

* Číslo kanálu a text (stejná data jako v okně Kanály)
* Klavírní klávesnice. Klávesy budou zvýrazněny barvou, která může být plně
  přizpůsobené událostmi MIDI noty hranými aktuálním souborem MIDI. Může
  volitelně také zobrazit názvy poznámek. Klíče mohou být aktivovány
  ručně pomocí počítačové klávesnice a myši

K dispozici je nabídka okna s následujícími možnostmi:

* Zobrazit na celou obrazovku. Zvětší okno vyplňující aktuální monitor
* Zobrazit všechny kanály
* Skrýt všechny kanály
* Utáhněte počet klíčů. Ve výchozím nastavení každá klávesnice zobrazuje 88 kláves od A0
  do C8. Pomocí této možnosti klávesnice skutečně používají pouze počet oktáv
  používá soubor MIDI, což je obvykle menší číslo
* Až 16 možností kanálu, pro skrytí nebo zobrazení každého kanálu jednotlivě. Podle
  výchozí jsou vybrány všechny MIDI kanály použité v aktuálním MIDI souboru

### Texty

Toto okno zobrazuje textová metadata z načteného souboru MIDI a filtruje 
data s řadou ovládacích prvků umístěných na panelu nástrojů okna:

* Stopa: pole se seznamem na výběr mezi „Všechny skladby“ nebo jednou položkou, které odpovídá
  každá použitá stopa v načteném souboru MIDI. Toto číslo stopy nahrazuje
  v tomto okně koncept MIDI kanálu, protože textová metadata nemají 
  atribut kanálu, ale vždy patří k nějaké stopě. Standardně je
  automaticky se vybere stopa obsahující více dat
* Typ: pole seznamu pro výběr jednoho z typů textu metadat nebo všech. The
  Typ „Lyric Events“ je nejpravděpodobnější volbou
* Kódování: kódování textu je detekováno automaticky, ale v některých případech
  detekce může být nesprávná, takže správnou lze vybrat ručně

K dispozici je nabídka okna s následujícími možnostmi:

* Kopírovat do schránky: zkopíruje metadata aktuálně zobrazená v okně
* Uložit do souboru ... Po otevření dialogového okna Uložit soubor vytvoří tato možnost text 
  soubor na disku s textem zobrazeným oknem a zvoleným kódováním
* Tisk ... Po otevření dialogového okna Tisk vytiskne tato možnost text zobrazený pomocí
  okno na vybrané tiskárně
* Celá obrazovka: Zvětší okno vyplňující aktuální monitor
* Písmo ... Otevírá okno Výběr písma, kde můžete vybrat typografii a velikost
  texty oken

## Seznamy skladeb

Seznamy skladeb jsou kolekce názvů souborů, které lze použít k přehrání série 
MIDI skladeb, jako hudební „repertoár“.

Akce „Soubor“-> „Seznam přehrání ...“ spravuje vytváření, úpravy, objednávání, 
otevírání a ukládání souborů seznamu Play. Toto okno zobrazuje aktuální seznam skladeb
název souboru v názvu okna.

K procházení seznamu skladeb slouží akce „Soubor“-> „Další“ a „Soubor“-> „Předchozí“
ručně dopředu a dozadu. Existují možnosti konfigurace pro spuštění přehrávání
automaticky po načtení MIDI souborů a automaticky přejít na další 
položka seznamu skladeb po dokončení přehrávání aktuální položky. Obě možnosti konfigurace
jsou ve výchozím nastavení povoleny.

Počáteční výchozí seznam skladeb obsahuje sbírku příkladů souborů MIDI. Li
dáváte přednost prázdnému seznamu, měli byste použít tlačítko „Vymazat“ a
poté uložte prázdný seznam skladeb se smysluplným názvem. To bude novinka
počáteční seznam skladeb v budoucnosti.

Poslední otevřený nebo uložený soubor seznamu skladeb si bude pamatovat při příštím 
program se spustí, ale seznamy skladeb se automaticky neukládají.

Seznamy skladeb jsou jednoduché textové soubory s názvem souboru na každém textovém řádku. Soubor
názvy mohou být relativní k umístění samotného souboru seznamu skladeb (takže ano 
nemá předpřipravenou absolutní cestu) nebo každý soubor může mít zapnutou absolutní cestu 
jeho vlastní.

Můžete mít několik seznamů skladeb, které lze snadno vytvořit mimo 
program, například v příkazovém řádku Linux tento příkaz vytvoří seznam skladeb 
soubor s názvem "my_playlist.lst" se všemi MIDI soubory umístěnými na pracovní ploše 
adresář:

~~~
$ ls -1 *.mid> my_playlist.lst
~~~

## Otevírání souborů MIDI

Jednotlivé soubory můžete otevřít ...

* Pomocí standardní akce „Soubor“-> „Otevřít“
* Pomocí nabídky „Soubor“-> „Otevřít poslední soubory“, která si pamatuje až deset souborů 
  nedávno otevřeno.
* Zadání názvů souborů na příkazovém řádku. Sada souborů se stává a
  dočasný seznam skladeb. To umožňuje snadnou integraci programu se souborem
  manažeři pomocí akcí „Otevřít s ...“.
* Přetahování souborů ze správce souborů do hlavního okna. The
  sada souborů se stane dočasným seznamem skladeb.

## Přizpůsobení

### Předvolby

V tomto dialogu je tlačítko „Obnovit výchozí“ pro inicializaci všech možností
na jejich výchozí hodnoty.

K dispozici jsou tři karty, které seskupují nastavení konfigurace

#### Všeobecné

* Perkusní MIDI kanál: Číslo od 1 do 16. Výchozí 10, odpovídající
  Obecný MIDI standardní bicí kanál
* Spustit přehrávání automaticky po načtení. Ve výchozím nastavení povoleno
* Automaticky postoupit na další položku seznamu stop. Ve výchozím nastavení povoleno
* Vynutit tmavý režim. Může být užitečné v systému Windows. V Linuxu a macOS je tmavý režim
  automaticky se použije při konfiguraci v operačním systému
* Použijte vnitřní motiv ikon. Toto nastavení je potřeba ve Windows a macOS.
* Styl widgetů Qt. V závislosti na operačním systému ovlivňuje vizuální
  integrace s jinými programy
* Reset systému exkluzivní pro MIDI. Tuto zprávu hráč odešle těsně před startem
  přehrávání každého MIDI souboru 

#### Texty

* Písmo textu. Otevře okno Výběr písma, kde můžete vybrat typografii a velikost
  texty oken. Toto nastavení je k dispozici také v nabídce okna Texty
* Budoucí barva textu. Toto nastavení platí pro text, který nebyl přehrán
* Předchozí barva textu. Toto nastavení platí pro již přehrávaný text

#### Hráčský klavír

* Zvýraznění poznámky. K dispozici je několik palet zvýraznění
* Barva jednoho zvýraznění. Když je vybrána paleta zvýraznění jedné barvy,
  toto nastavení je barva zvýrazňující notu
* Všimněte si rychlosti barevného odstínu. S touto možností MIDI zaznamenává rychlost událostí
  Atribut se používá k zesvětlení nebo ztmavení zvýrazňující barvy
* Písmo názvů poznámek. Otevře okno Výběr písma, kde vyberete typografii a
  velikost názvů not
* Zobrazit názvy poznámek: dostupné možnosti jsou Nikdy, Minimální, Při aktivaci vždy.
  Možnost Minimální zobrazuje pouze názvy kláves C.

### Přizpůsobení lišty nástrojů

Panel nástrojů se nachází v hlavním okně, které je obvykle velmi malé 
volného místa a lze jej přesunout do horní nebo dolní části okna a 
také mimo něj. Akce dostupné jako tlačítka nástrojů lze konfigurovat v
toto dialogové okno. Všechny akce jsou také položkami hlavní nabídky.

* Dostupné akce: zobrazuje seznam akcí, které se aktuálně nezobrazují jako
  tlačítka nástrojů
* Vybrané akce: je seznam aktuálně vybraných tlačítek nástrojů

Chcete -li vybrat jednu akci ze seznamu Dostupné, nejprve ji vyberte myší a 
poté stiskněte tlačítko „přidat“. Akce je odebrána ze seznamu Available a
umístěné na konci vybraného seznamu.

Chcete -li odebrat jedno tlačítko z panelu nástrojů, vyberte jej myší v poli Vybráno 
seznamu a poté stiskněte tlačítko „odebrat“.

Chcete -li uspořádat pořadí tlačítek na panelu nástrojů, použijte tlačítka „nahoru“ a „dolů“ 
po výběru položky v seznamu Vybrané.

Styl tlačítek nástrojů: obsahuje následující možnosti, které platí pro všechny 
tlačítka na panelu nástrojů

* Pouze ikona
* Pouze text
* Text vedle ikony
* Text pod ikonou
* Sledujte styl: Nastavení je určeno nastavením „Styl widgetů Qt“
  v dialogu Předvolby.
