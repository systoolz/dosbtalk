# dosbtalk
This is unofficial API implementation for Text-to-Speech Engine by First Byte.

You can obtain this Engine here (V3, SmoothTalker, Sound Blaster only):

https://winworldpc.com/product/dr-sbaitso/2x

Or there (V4, Text-to-Speech Engine, a lot of other sound devices supported):

https://winworldpc.com/product/monologue/for-dos-v-31

Please note that this file based on disassembled and cleaned code.
There is no documentation on functions so use anything except functions
declared in "speech.h" at your own risk. See "talk.c" code for example.

This code compiled and tested with [Borland Turbo C 2.01 (1988)](https://web.archive.org/web/20060614091700/http://bdn.borland.com/article/20841).

Comments, questions and suggestions about this API implementation accepted here:

https://github.com/systoolz/dosbtalk

This code licensed under the Apache License, Version 2.0 (the "License") - see LICENSE file.


V3 Sound Blaster initialization:

SBTALKER.EXE /dBLASTER

Required files: SBTALKER.EXE and BLASTER.DRV


V4 Sound Blaster initialization (run in this order):

SOUNDBST.EXE

SPEECH.EXE

Required files: SOUNDBST.EXE, SPEECH.EXE, KERNEL.DIC and all V4ENG*.*

Exchange SOUNDBST.EXE with any other sound driver you may want from this list:

SOUNDACP.EXE - Driver for the ACPA card

SOUNDADD.EXE - Driver for the ADLIB card

SOUNDBST.EXE - Driver for the Sound Blaster

SOUNDCMD.EXE - Driver for the Sound Commander (MediaSonic)

SOUNDCVX.EXE - Driver for the Covox Speech Thing and the Hearsay 100, 500, & 1000

SOUNDDIS.EXE - Driver for The Sound Source (Disney)

SOUNDEC2.EXE - Driver for the Echo PC II

SOUNDIBM.EXE - Driver for the IBM Speech Adapter

SOUNDPS1.EXE - Driver for PS/1 Audio Card

SOUNDPWM.EXE - Driver for the standard speaker

SOUNDTHD.EXE - Driver for the Thunder Board

SOUNDTND.EXE - Driver for a Tandy Sound System


Original copyrights follows below.


(READ.EXE)

SOUND BLASTER Text-to-Speech Text Reader Version 1.00

Copyright (c) CREATIVE LABS, INC., 1989-1991. All rights reserved.

Copyright (c) CREATIVE Technology Pte Ltd, 1989-1991. All rights reserved.


(V3)

SmoothTalker (R), Version 3.5, male voice

Copyright (c) 1983-1990 First Byte, All Rights Reserved.

Protected by U.S. Patents #4,692,941 and #4,617,645


(V4)

First Byte Text-to-Speech Engine Version 4.1

Copyright (c) 1991, All Rights Reserved.

Technology contained herein protected by one or more of the following U.S. patents:

#4,692,941 #4,617,645 #4,852,168 #4,805,220 #4,833,718
