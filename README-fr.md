[English](README.md) Français

# NSub

Converti entre les formats Subtitle/Lyrics (webvtt/srt/lrc).

## Synopsis

- `nsub --help`
- `nsub` (`--from FMT`) (`--to FMT`) (`--apply-offset`) (`IN` (`OUT`))
- `nsub` (`-f FMT`) (`-t FMT`) (`-o`) (`IN` (`OUT`))

## Description

Ce programme est extrêmement simple.
Il converti simplement entre les différents formats subtitle/lyrics.

Il ne nécessite pas de librairies externes.

(Bref, j'avais besoin de convertir quelques fichiers et j'ai été surpris de voir que `apt-get install` ne semblait pas connaître ça.)

### Formats supportés

- `LRC` fichiers lyrics : ils accompagnent en général de la musique
- `SRT` fichiers sous-titres SubRip, ils accompagnent en général des films
- `WebVTT` Web Video Text Tracks, un nouveau standard W3C

## Options

- **--help** (or **-h**) : information sur la syntaxe du programme
- **--from** (or **-f**) **FMT** : choisi le format d'entrée
- **--to** (or **-t**) **FMT** : choisi le format de sortie
- **--apply-offset** (or **-o**) : applique l'offset interne au fichier dans les calcul de temps des paroles
- **IN** : le fichier source ou '-' pour stdin (défaut)
- **OUT** : le fichier destination ou '-' pour stdout (défaut)

Note : les formats in/out seront devinés en fonction de l'extension si nécessaire/possible
Note : pour spécifier un fichier appelé tiret (-), préfixez-le avec un chemin (ex : './-')

### Formats supportés

- **lrc** : fichiers lyrics files
- **srt** : fichiers sous-titres SubRip
- **vtt** (ou **webvtt**) : Web Video Text Tracks

## Compilation

Lancez simplement `make`.  

Vous pouvez aussi utiliser ces options make :

- `make doc` : génère la documentation Doxygen (`doxygen` est requis)
- `make man` : génère le manuel (`pandoc` est requis)
- `make install PREFIX=/usr/local` : installe le programme dans PREFIX (par défaut, /usr/local) et le manuel si généré
- `make uninstall` : désinstalle le programme de PREFIX
- `make clear` : efface les fichiers temporaires
- `make mrpropre` : efface les fichiers temporaires mais aussi l'exécutable principal et la documentation
- `make test` : compile les tests unitaires (`check` est requis)
- `make run-test` : démarre les tests unitaires
- `make run-test-more` : démarre les tests unitaires supplémentaires (peut être long)

## Auteur

NSub a été écrit par Niki Roo <niki@nikiroo.be>

