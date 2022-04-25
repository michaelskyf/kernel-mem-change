# kernel-memory-change
## _Find and replace process memory_

<img alt="Gif" src="https://raw.githubusercontent.com/michaelskyf/kernel-mem-change/master/.github/images/example.gif" width="60%" />

## What it does?
This little, hacked-together Linux module finds pattern in user-space process and replaces it with given sequence of bytes.

## How to run
⚠️ You should run this module in a VM, otherwise You may break Your system ⚠️
- Run `make` (make sure to install linux-headers)
- Run `./loop`
- Run `insmod mem-change.ko`
- If everything worked correctly, "loop" should display a message and exit (If not see [Hacking](<#hacking>))
- ⚠️ Due to page caching, you need to recompile loop.c to restore it to normal behaviour

## Hacking
If You want to find another pattern to replace, run `objdump -d ./loop` <br>
This module is really basic and doesn't support patterns spanning across pages

## Tested on
- x86_64 Arch Linux (kernel 5.17) with gcc (11.2.0)
