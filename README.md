English [Français](README-fr.md)

# NSub

Converts between Subtitle/Lyrics formats (webvtt/srt/lrc).

## Synopsis

- `nsub --help`
- `nsub` (`--from FMT`) (`--to FMT`) (`--apply-offset`) (--output `OUT`) (`IN`)
- `nsub` (`-f FMT`) (`-t FMT`) (`-a`) (`-o OUT`) (`IN`)

## Description

This program is a very simple thing.  
It just converts between different subtitle/lyrics formats.

It does not require external libraries.

(I.E., I needed to convert some files and was surprised to see that it was not available via `apt-get install`.)

### Supported formats

- `LRC` lyrics files: usually distributed with music
- `SRT` SubRip subtitle files, usually distributed with films
- `WebVTT` Web Video Text Tracks, a new W3C standard

## Options

- **--help** (or **-h**): information about the syntax
- **--from** (or **-f**) **FMT**: select the input format FMT
- **--to** (or **-t**) **FMT**: select the output format FMT
- **--apply-offset** (or **-a**): apply the offset tag value to the lyrics
- **--output** (or **-o**) **OUT**: the output file or '-' for stdout (which is the default)
- **IN**: the input file or '-' for stdin (which is the default)

Note: the in/out formats will be guessed from the extension if needed/possible
Note: to specify a file named dash (-), prefix it with a path (e.g., './-')

### Supported formats

- **lrc**: lyrics files
- **srt**: SubRip subtitles files
- **vtt** (or **webvtt**): Web Video Text Tracks

## Compilation

Just run `make`.  

You can also use those make targets:

- `make doc`: build the Doxygen documentation (`doxygen` required)
- `make man`: build the man page (`pandoc` required)
- `make install PREFIX=/usr/local`: install the program into PREFIX (default is `/usr/local`) and the manual if built
- `make uninstall`: uninstall the program from the given PREFIX
- `make clear`: clear the temporary files
- `make mrpropre`: clear everything, including the main executable and the documentation
- `make test`: build the unit tests (`check` required)
- `make run-test`: start the unit tests
- `make run-test-more`: start the extra unit tests (can be long)

## Author

NSub was written by Niki Roo <niki@nikiroo.be>

