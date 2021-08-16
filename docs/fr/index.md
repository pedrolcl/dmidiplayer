% de l'index de l'aide

## Introduction

Cette application est un lecteur de fichiers MIDI multiplateforme pour Linux, Windows et 
macOS. Il lit .MID (fichiers MIDI standard), .KAR (karaoké) et .WRK (Cakewalk)
formats de fichiers et sorties des événements MIDI vers les ports MIDI matériels et également logiciels 
synthés.

[Drumstick](https://drumstick.sourceforge.io) est un ensemble de GPLv3 
bibliothèques C++/Qt sous licence pour les applications MIDI. Le projet comprend plusieurs
outils comme exemples, et parmi eux se trouve l'utilitaire drumstick-guiplayer qui 
exploite la bibliothèque Drumstick::ALSA, elle n'est donc disponible que pour Linux 
(car le séquenceur ALSA est une technologie Linux uniquement). Certaines personnes ont
demandé un programme avec les mêmes fonctionnalités sur Windows et macOS, et 
C'est ici. Mais ce programme est bien plus que cela, et il fonctionne aussi sur
Linux...

Quelques fonctionnalités clés :

* Sortie MIDI vers les ports MIDI matériels, ou tout autre backend Drumstick comme soft
  synthés
* [Transposer](https://en.wikipedia.org/wiki/Transposition_(music)) tonalité de la chanson 
  entre -12 et +12 demi-tons
* Changer le niveau de volume MIDI (en utilisant MIDI CC7)
* Échelle la vitesse de la chanson entre la moitié et le double 
  [tempo](https://en.wikipedia.org/wiki/Tempo)
* Vues Paroles, Piano Player et Canaux MIDI
* Prend en charge les formats de fichiers MID/KAR (fichiers MIDI standard) et WRK (Cakewalk)

## Sortie MIDI

Tout lecteur MIDI ne produit que des événements MIDI, mais aucun son. Pour produire du son, un MIDI
Un synthétiseur est nécessaire pour restituer les événements en audio numérique qui peut être 
reproduit par les haut-parleurs audio de l'ordinateur (ou externes). Les événements peuvent également
être rendu par un instrument de musique MIDI externe connecté à l'ordinateur, 
par exemple un orgue MIDI, un piano ou un synthétiseur avec une interface USB. Si ce
option n'est pas disponible, une autre alternative habituelle est un synthétiseur logiciel 
(un programme).

Il existe des synthétiseurs logiciels inclus dans les systèmes d'exploitation de l'ordinateur, 
par exemple, Microsoft Windows inclut le "Microsoft GS Wavetable Synth", et
Apple macOS inclut le "synthétiseur Apple DLS". Pour les utiliser, vous devez ouvrir
la boîte de dialogue "MIDI Setup" puis :

* Dans **Windows**, sélectionnez l'option "Windows MM" dans la première zone de liste, et 
  "Microsoft GS Wavetable Synth" dans la deuxième zone de liste. A la place de ce synthé,
  qui est très basique et limité, vous préférerez peut-être une alternative meilleure/gratuite comme 
  [Synthé MIDI virtuel](https://coolsoft.altervista.org/en/virtualmidisynth)
  qui apparaîtra comme "VirtualMIDISynth #n" dans la deuxième zone de liste lors de l'exécution.
  Si vous avez un instrument MIDI externe connecté à l'ordinateur, son nom
  devrait également apparaître dans la deuxième zone de liste.
* Dans **macOS**, sélectionnez "DLS Synth" dans les première et deuxième zones de liste. 
  Ce synthé a une option de configuration pour utiliser les fichiers soundfont DLS ou SF2 si 
  tu veux. Pour utiliser un autre appareil MIDI connecté ou un synthétiseur logiciel,
  vous devez sélectionner "CoreMIDI" dans la première zone de liste, et le nom de votre
  synthé dans le second.
* Dans **Linux** et autres systèmes d'exploitation **Unix**, "SonivoxEAS" est un logiciel 
  synthé prêt à être utilisé. C'est un synthé GM très basique qui n'utilise pas de fichiers
  et ne dépend que du système audio PulseAudio. Pour les périphériques externes connectés
  à l'ordinateur ou à des synthés logiciels arbitraires, vous devez sélectionner "ALSA" (ou "OSS") 
  dans la première zone de liste.
* **FluidSynth** est un autre synthétiseur logiciel ouvert inclus qui est multiplateforme 
  et disponible sur tous les systèmes d'exploitation. Il est très configurable, acceptant
  soundfont et de nombreux systèmes audio numériques. Sélectionner
  [FluidSynth](https://github.com/FluidSynth/fluidsynth) sur les deux zones de liste 
  pour l'utiliser.
* **Réseau** est une sortie UDP Multicast multiplateforme, qui peut être utilisée pour envoyer 
  événements MIDI sous forme de paquets réseau vers d'autres ordinateurs et appareils dans le même 
  réseau local. Il est compatible avec
  [ipMIDI](https://www.nerds.de/en/ipmidi.html),
  [QmidiNet](https://qmidinet.sourceforge.io/), et 
  [multimidicast](https://llg.cubic.org/tools/multimidicast/) 
  entre autres logiciels.

## Contrôles de lecture

Les actions standard suivantes sont disponibles dans le menu `Fichier` ainsi que dans le
barre d'outils, avec les fonctionnalités classiques sur n'importe quel lecteur multimédia : 

* 'Jouer' 
* `Pause` (et continuer)
* 'Arrêter'
* `Avance rapide` (par 1 barre)
* `Rewind` (par 1 barre)

Par défaut, `Play` est activé automatiquement lors du chargement d'un fichier MIDI sauf 
lorsque cette fonctionnalité est désactivée dans la boîte de dialogue de configuration.

La "barre d'état", éventuellement située en bas de la fenêtre principale, affiche 
l'état de lecture actuel comme "Lecture", "Arrêté", "Pause", etc.

### Transposer

Le « contrôle de hauteur » est situé dans la fenêtre principale. C'est un spin box initialisé
à zéro, avec une plage valide de -12 à +12 demi-tons. Les limites sont équivalentes
à une octave vers le haut ou vers le bas de la tonalité d'origine. Cela devrait suffire aux chanteurs
ou des instrumentistes ayant besoin d'ajuster la tonalité pour leur confort.

Tous les canaux MIDI sont transposés chromatiquement, à l'exception du canal de percussion 
(GM canal 10 par défaut).

### Niveau de volume
 
Le « contrôle du volume » est situé dans la fenêtre principale. C'est un curseur, initialisé
à 100 %, avec une plage valide de 0 % à 200 %. L'effet de limite supérieure peut être inférieur
car le MIDI CC 7 a une plage valide de 0 à 127. Il y a aussi un bouton pour
« Réinitialiser le volume ».
 
### Tempo

Le `Contrôle du tempo` est situé dans la fenêtre principale. C'est un curseur initialisé
à 100 % avec une plage valide de 50 % à 200 %. Il y a un bouton pour « Réinitialiser le tempo »
et aussi un affichage du tempo, montrant le tempo effectif en battements par minute avec
une valeur initiale de 120 bpm. Il s'agit du tempo par défaut d'un fichier MIDI s'il
ne contient pas de changement de tempo. Cette valeur est mise à jour lors du changement de
contrôle du tempo, et aussi pendant la lecture d'un fichier MIDI contenant des changements de tempo.

### Saut, boucle et positionnement

Action « Jump » (située dans le menu « Fichier » et la barre d'outils par défaut), demande
un numéro de mesure, entre 1 et la dernière mesure du morceau chargé, pour changer le 
position actuelle de la chanson à la mesure donnée.

L'action `Loop` (située dans le menu `File` et la barre d'outils par défaut), demande
deux numéros de barre. La chanson sera jouée entre les deux mesures données lorsqu'elle est
activé. Vous pouvez désactiver la boucle à tout moment en sélectionnant à nouveau l'action.

Le `Contrôle de positionnement` est situé dans la fenêtre principale. C'est un curseur qui
se déplace pendant que la position de lecture actuelle change, et peut être déplacé à la main pour
changer la position à n'importe quel point arbitraire dans la chanson.

## vues

Les options suivantes sont activées dans le menu « Affichage » :

Trois vues sont des fenêtres indépendantes que vous pouvez ouvrir et fermer à votre guise. 
commodité : canaux, paroles et pianiste.

D'autre part, vous pouvez également masquer ou afficher trois autres vues intégrées 
dans la fenêtre principale : barre d'outils, barre d'état et rythme.

### Canaux

Cette fenêtre affiche jusqu'à 16 lignes, une pour chaque canal MIDI utilisé par le
fichier MIDI. Les colonnes sont :

* Numéro de canal MIDI et texte (modifiable). Habituellement, le nom de l'instrument, ou un
  brève description tirée des métadonnées du fichier.
* Bouton de sourdine. Avec cette option, le canal actuel peut être coupé.
* Bouton solo. Augmente le volume du canal actuel, tout en baissant le
  d'autres canaux.
* Niveau. Indicateur de l'activité sur le canal en cours.
* Serrure. Ce bouton fixe le Patch sur le canal actuel, empêchant les changements
  stocké dans le fichier.
* Patch. Correspond aux événements de changement de programme MIDI selon le
  Liste MIDI des noms d'instruments.

### Joueur de piano

Cette fenêtre affiche jusqu'à 16 lignes, une pour chaque canal MIDI utilisé par le
fichier MIDI. Chaque ligne contient :

* Un numéro de chaîne et du texte (les mêmes données que la fenêtre Chaînes)
* Un clavier de piano. Les touches seront mises en évidence avec une couleur, qui peut être entièrement
  personnalisé, par les événements de note MIDI joués par le fichier MIDI actuel. Cela pourrait
  également éventuellement afficher les noms des notes. Les touches peuvent être activées
  manuellement à l'aide du clavier et de la souris de l'ordinateur

Il y a un menu fenêtre, avec les options suivantes :

* Voir plein écran. Agrandit la fenêtre remplissant le moniteur actuel
* Afficher toutes les chaînes
* Masquer toutes les chaînes
* Serrez le nombre de clés. Par défaut, chaque clavier affiche 88 touches, de A0
  à C8. En utilisant cette option, les claviers n'utilisent que le nombre d'octaves réellement
  utilisé par le fichier MIDI, qui est généralement un plus petit nombre
* Jusqu'à 16 options de canal, pour masquer ou afficher chaque canal individuellement. Par
  par défaut, tous les canaux MIDI utilisés dans le fichier MIDI actuel sont sélectionnés

### Paroles

Cette fenêtre affiche les métadonnées de texte du fichier MIDI chargé, en filtrant les 
données avec une série de contrôles situés sur la barre d'outils de la fenêtre :

* Piste : une zone de liste à choisir parmi "Toutes les pistes", ou un élément Correspondant à
  chaque piste utilisée dans le fichier MIDI chargé. Ce numéro de piste remplace le
  concept de canal MIDI dans cette fenêtre, car les métadonnées de texte n'ont pas 
  un attribut de canal, mais il appartient toujours à une piste. Par défaut, le
  la piste contenant plus de données est automatiquement sélectionnée
* Type : une zone de liste pour choisir l'un des types de texte de métadonnées ou tous. Le
  Le type « Lyric Events » est le choix le plus probable
* Encodage : l'encodage du texte est détecté automatiquement, mais dans certains cas le
  la détection peut être incorrecte, donc la bonne peut être sélectionnée manuellement

Il y a un menu fenêtre, avec les options suivantes :

* Copier dans le presse-papiers : copie les métadonnées actuellement affichées par la fenêtre
* Enregistrer dans un fichier... Après avoir ouvert une boîte de dialogue Enregistrer le fichier, cette option crée un texte 
  fichier sur disque, avec le texte affiché par la fenêtre et l'encodage sélectionné
* Imprimer... Après avoir ouvert une boîte de dialogue d'impression, cette option imprime le texte affiché par
  la fenêtre sur l'imprimante sélectionnée
* Plein écran : agrandit la fenêtre remplissant le moniteur actuel
* Police... Ouvre une fenêtre de sélection de police pour choisir la typographie et la taille de
  les textes de la fenêtre

## Listes de lecture

Les listes de lecture sont des collections de noms de fichiers, qui peuvent être utilisées pour lire une série 
de pièces MIDI, comme un "répertoire" musical.

L'action `Fichier`->`Liste de lecture...` gère la création, la modification, le classement, 
l'ouverture et l'enregistrement des fichiers de liste de lecture. Cette fenêtre affiche la liste de lecture actuelle
nom du fichier dans le titre de la fenêtre.

Les actions `Fichier`->`Suivant` et `Fichier`->`Préc` sont utilisées pour naviguer dans la liste de lecture
manuellement en avant et en arrière. Il existe des options de configuration pour démarrer la lecture
automatiquement après le chargement des fichiers MIDI, et passer automatiquement au suivant 
élément de la liste de lecture lorsque la lecture de l'élément en cours est terminée. Les deux options de configuration
sont activés par défaut.

La liste de lecture par défaut initiale contient une collection d'exemples de fichiers MIDI. Si
vous préférez commencer avec une liste vide, vous devez utiliser le bouton « Effacer » et
puis enregistrez la liste de lecture vide avec un nom significatif. Ce sera le nouveau
playlist initiale dans le futur.

Le dernier fichier de liste de lecture ouvert ou enregistré sera mémorisé la prochaine fois que le 
programme est lancé, mais les listes de lecture ne sont pas enregistrées automatiquement.

Les listes de lecture sont de simples fichiers texte, avec un nom de fichier sur chaque ligne de texte. Le fichier
les noms peuvent être relatifs à l'emplacement du fichier de liste de lecture lui-même (ils le font donc 
pas de chemin absolu préfixé) ou chaque fichier peut avoir un chemin absolu sur 
sa propre.

Vous pouvez avoir plusieurs listes de lecture qui peuvent être créées facilement en dehors du 
programme, par exemple dans la ligne de commande Linux, cette commande crée une liste de lecture 
fichier nommé "my_playlist.lst" avec tous les fichiers MIDI situés sur le 
annuaire:

~~~
$ ls -1 *.mid > ma_playlist.lst
~~~

## Ouverture de fichiers MIDI

Vous pouvez ouvrir des fichiers individuels...

* Utilisation de l'action standard `Fichier`->`Ouvrir`
* En utilisant le menu `Fichier`->`Ouvrir les fichiers récents`, qui mémorise jusqu'à dix fichiers 
  récemment ouvert.
* Fournir le(s) nom(s) de fichier dans la ligne de commande. L'ensemble de fichiers devient un
  liste de lecture temporaire. Cela permet une intégration facile du programme avec le fichier
  gestionnaires utilisant les actions « Ouvrir avec... ».
* Glisser-déposer des fichiers depuis un gestionnaire de fichiers dans la fenêtre principale. Le
  ensemble de fichiers devient une liste de lecture temporaire.

## Personnalisation

### Préférences

Il y a un bouton "Restaurer les valeurs par défaut" pour initialiser toutes les options dans cette boîte de dialogue
à leurs valeurs par défaut.

Il y a trois onglets, regroupant les paramètres de configuration

#### Général

* Canal MIDI Percussion : Numéro de 1 à 16. Par défaut 10, correspondant au
  Canal de percussion standard General MIDI
* Démarrer la lecture automatiquement après le chargement. Activé par défaut
* Avancez automatiquement jusqu'à l'élément suivant de la liste de lecture. Activé par défaut
* Forcer le mode sombre. Peut être utile sous Windows. Sous Linux et macOS, le mode sombre est
  appliqué automatiquement lorsqu'il est configuré dans le système d'exploitation
* Utilisez le thème d'icône interne. Ce paramètre est nécessaire sous Windows et macOS.
* Style de widgets Qt. Selon le système d'exploitation, affecte le visuel
  intégration avec d'autres programmes
* Réinitialisation exclusive du système MIDI. Le joueur envoie ce message juste avant de commencer
  jouer chaque fichier MIDI 

#### Paroles

* Police de texte. Ouvre une fenêtre de sélection de police pour choisir la typographie et la taille de
  les textes de la fenêtre. Ce paramètre est également disponible dans le menu de la fenêtre Paroles
* Future couleur du texte. Ce réglage s'applique au texte qui n'a pas été lu
* Couleur du texte passé. Ce réglage s'applique au texte qui a déjà été lu

#### Joueur de piano

* Remarque en surbrillance. Il y a quelques palettes de surlignage disponibles
* Couleur de surbrillance unique. Lorsque la palette de surbrillance d'une seule couleur est sélectionnée,
  ce paramètre est la couleur de surbrillance de la note
* Notez la vitesse à la teinte de couleur. Avec cette option, la vélocité des événements de note MIDI
  L'attribut est utilisé pour donner à la couleur de surbrillance un ton plus clair ou plus foncé
* Notez la police des noms. Ouvre une fenêtre de sélection de police pour choisir la typographie et
  taille des noms de notes
* Afficher les noms des notes : les choix disponibles sont Jamais, Minimal, Lorsqu'il est activé, toujours.
  L'option Minimal affiche uniquement les noms des touches C.

### Personnalisation de la barre d'outils

La barre d'outils est située sur la fenêtre principale, qui a généralement de très petites 
espace disponible, et peut être déplacé vers le haut ou le bas de la fenêtre, et 
aussi à l'extérieur. Les actions disponibles sous forme de boutons d'outils peuvent être configurées dans
cette fenêtre de dialogue. Toutes les actions sont également des éléments du menu principal.

* Actions disponibles : affiche la liste des actions qui ne sont pas actuellement affichées comme
  boutons d'outils
* Actions sélectionnées : est la liste des boutons d'outils actuellement sélectionnés

Pour sélectionner une action dans la liste Disponible, sélectionnez-la d'abord avec la souris et 
puis appuyez sur le bouton "ajouter". L'action est supprimée de la liste Disponible et
placé à la fin de la liste sélectionnée.

Pour supprimer un bouton de la barre d'outils, sélectionnez-le avec la souris dans le 
liste, puis appuyez sur le bouton « supprimer ».

Pour réorganiser l'ordre des boutons dans la barre d'outils, utilisez les boutons "haut" et "bas" 
boutons après avoir sélectionné un élément dans la liste Sélectionné.

Style des boutons d'outils : contient les options suivantes qui s'appliquent à tous 
les boutons de la barre d'outils

* Icône seulement
* Texte seulement
* Texte à côté de l'icône
* Texte sous l'icône
* Follow Style : le paramètre est déterminé par le paramètre « Qt Widgets Style »
  dans la boîte de dialogue Préférences.

