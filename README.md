# NSub: Subtitle/Lyrics conversion program (webvtt/srt/lrc)

This program is a very simple thing.  
It just converts between different subtitle/lyrics formats.

It does not require external libraries.

(I.E., I needed to convert some files and was surprised to see that it was not available via `apt-get install`.)

Supported formats:
* `LRC` lyrics files: usually distributed with music
* `SRT` SubRip subtitle files, usually distributed with films
* `WebVTT` Web Video Text Tracks, a new W3C standard

Note: this program can not yet open WebVTT nor SRT files (it supports all 3 formats as output, though). 

## Compile

Just run `make`.  
You can also use those make targets:
* `make install`
* `make uninstall`
* `make clear`
* `make mrpropre`

## Syntax

`bin/nsub` (`--from FMT`) (`--to FMT`) (`--apply-offset`) (`IN_FILE` (`OUT_FILE`))
* **--help** (or **-h**): this help message
* **--from** (or **-f**) **FMT**: select the input format FMT
* **--to** (or **-t**) **FMT**: select the output format FMT
* **--apply-offset** (or **-o**): apply the offset tag value to the lyrics
* **IN_FILE**: the input file or '-' for stdin (which is the default)
* **OUT_FILE**: the output file or '-' for stdout (which is the default)

Note: the in/out formats will be guessed from the extension if needed/possible
Note: to specify a file named dash (-), prefix it with a path (e.g., './-')

Supported formats:
* **lrc**: lyrics files
* **srt**: SubRip subtitles files
* **vtt** (or **webvtt**): Web Video Text Tracks

