#include <dos.h>

#include "speech.h"

/*
This is unofficial routine for Creative's Sound Blaster Text-to-Speech module.
You can obtain this module here:
https://winworldpc.com/product/dr-sbaitso/2x
Please note that this file based on disassembled and cleaned code from tools
"Read.exe" and "Set-echo.exe". There is no documentation on functions so use
anything except DetectSpeech(), SetGlobals(), Say() and SetEcho() on your
own risk. See "talk.c" code for example.

This code compiled and tested with Borland Turbo C 2.01 (1988):
https://web.archive.org/web/20060614091700/http://bdn.borland.com/article/20841

Any comments, questions and suggestions about this file accepted here:
https://github.com/systoolz/dosbtalk

Original copyrights follows below.

SOUND BLASTER Text-to-Speech Text Reader Version 1.00
Copyright (c) CREATIVE LABS, INC., 1989-1991. All rights reserved.
Copyright (c) CREATIVE Technology Pte Ltd, 1989-1991. All rights reserved.

SmoothTalker (R), Version 3.5, male voice
Copyright (c) 1983-1990 First Byte, All Rights Reserved
Protected by U.S. Patents #4,692,941 and #4,617,645
*/

enum {
  TTS_FUNC_PARSER = 0,
  TTS_FUNC_UNKNOWN = 1,
  TTS_FUNC_SET_GLOBALS = 2,
  TTS_FUNC_DICT_INS = 3,
  TTS_FUNC_INIT_DICT = 4,
  TTS_FUNC_DICT_DEL = 5,
  TTS_FUNC_DICT_DUMP = 6,
  TTS_FUNC_SAY = 7,
  TTS_FUNC_SPEAKM = 8,
  TTS_FUNC_SPEAKF = 9
};

enum {
  TTS_GLOB_GENDER = 0,
  TTS_GLOB_TONE = 1,
  TTS_GLOB_VOLUME = 2,
  TTS_GLOB_PITCH = 3,
  TTS_GLOB_SPEED = 4,
  TTS_GLOB_COUNT = 5,
  TTS_GLOB_ACTION = 6
};

enum {
  TTS_DATA_READ = 0,
  TTS_DATA_WRITE = 1
};

enum {
  TTS_DATA_FIRST = 0,
  TTS_DATA_SECOND = 2
};

/*
#pragma pack(push, 1)
typedef struct {
  unsigned char length;
  char string[255];
} String;
#pragma pack(pop)
*/

/* argument must be in AL register - it's not actual stack value */
typedef long (far *LPPLAYFUNC)(register unsigned char);
typedef unsigned char far * FPBYTE;
typedef unsigned short far * FPWORD;

static LPPLAYFUNC fpDrvEntry = (LPPLAYFUNC) MK_FP(0, 0); /* DRVR_ENTRY */
static FPBYTE fpStr1 = (FPBYTE) MK_FP(0, 0); /* SRECPTR */
static FPBYTE fpStr2 = (FPBYTE) MK_FP(0, 0); /* SRECPTR */
static FPWORD fpGlob = (FPWORD) MK_FP(0, 0); /* 7 globals */
static short SpeechInitialized = 0;

/* clip data value in allowed range */
short ClipData(short dval, short dmin, short dmax) {
  dval = (dmin < dval) ? dval : dmin;
  dval = (dmax > dval) ? dval : dmax;
  return(dval);
}

#include <stdio.h>
/* handy replacement for MoveTo1() / MoveTo2() / MoveFrom1() / MoveFrom2()
   note that all input and output strings must be at least 256 bytes long */
short BufferData(char *s, char t) {
FPBYTE fp;
short i;
  i = 0;
  if (SpeechInitialized) {
    fp = (t & TTS_DATA_SECOND) ? fpStr2 : fpStr1;
    if (t & TTS_DATA_WRITE) {
      /* write to internal driver buffer */
      for (i = 0; ((i < 255) && (s[i])); i++) {
        fp[i + 1] = s[i];
        /* first byte - buffer length */
        fp[0] = i + 1;
      }
    } else {
      /* read from internal driver buffer */
      i = fp[0];
      s[i] = 0;
      while (i--) {
        s[i] = fp[i + 1];
      }
    }
    i = 1;
  }
  return(i);
}

short Unload(void) {
union REGS rs;
short r;
  r = -1;
  if (SpeechInitialized) {
    rs.x.ax = 0xFBFF;
    int86(0x2F, &rs, &rs);
    r = rs.x.ax;
  }
  return(r);
}

short DetectSpeech(void) {
struct SREGS sr;
union REGS rs;
FPWORD p;
  if (!SpeechInitialized) {
    /* check multiplex interrupt vector */
    if (getvect(0x2F)) {
      /* check service */
      rs.x.ax = 0xFBFB;
      segread(&sr);
      int86x(0x2F, &rs, &rs, &sr);
      if (!rs.x.ax) {
        p = (FPWORD) MK_FP(sr.es, rs.x.bx);
        fpDrvEntry = (LPPLAYFUNC) MK_FP(p[3], p[2]);
        fpStr1 = (FPBYTE) MK_FP(sr.es, rs.x.bx + 0x20);
        fpStr2 = (FPBYTE) MK_FP(sr.es, rs.x.bx + 0x20 + 0x100);
        fpGlob = (FPWORD) MK_FP(sr.es, rs.x.bx + 0x20 + 0x200);
        SpeechInitialized = 1;
      }
    }
  }
  return(SpeechInitialized);
}

/* use -1 to keep current values */
short SetGlobals(short gen, short ton, short vol, short pit, short spd) {
short r;
  r = 0;
  if (SpeechInitialized) {
    if (gen >= 0) { fpGlob[TTS_GLOB_GENDER] = gen; }
    /* tone: 0 - bass; 1 - tremble */
    if (ton >= 0) { fpGlob[TTS_GLOB_TONE] = ClipData(ton, 0, 1); }
    if (vol >= 0) { fpGlob[TTS_GLOB_VOLUME] = ClipData(vol, 0, 9); }
    if (pit >= 0) { fpGlob[TTS_GLOB_PITCH] = ClipData(pit, 0, 9); }
    if (spd >= 0) { fpGlob[TTS_GLOB_SPEED] = ClipData(spd, 0, 9); }
    r = (short) fpDrvEntry(TTS_FUNC_SET_GLOBALS);
  }
  return(r);
}

short Parser(char *engstr, char *phonstr, short count) {
short r;
  r = -1;
  if (SpeechInitialized) {
    BufferData(engstr, TTS_DATA_WRITE | TTS_DATA_FIRST);
    fpGlob[TTS_GLOB_COUNT] = count;
    r = (short) fpDrvEntry(TTS_FUNC_PARSER);
    BufferData(phonstr, TTS_DATA_READ | TTS_DATA_SECOND);
  }
  return(r);
}

short InitDict(short act) {
short r;
  r = 0;
  if (SpeechInitialized) {
    fpGlob[TTS_GLOB_ACTION] = act;
    r = (short) fpDrvEntry(TTS_FUNC_INIT_DICT);
  }
  return(r);
}

short DictIns(char *engstr, char *phonstr) {
short r;
  r = 0;
  if (SpeechInitialized) {
    BufferData(engstr, TTS_DATA_WRITE | TTS_DATA_FIRST);
    BufferData(phonstr, TTS_DATA_WRITE | TTS_DATA_SECOND);
    r = (short) fpDrvEntry(TTS_FUNC_DICT_INS);
  }
  return(r);
}

short DictDel(char *engstr) {
short r;
  r = 0;
  if (SpeechInitialized) {
    BufferData(engstr, TTS_DATA_WRITE | TTS_DATA_FIRST);
    r = (short) fpDrvEntry(TTS_FUNC_DICT_DEL);
  }
  return(r);
}

/* returns long in DX:AX pair */
long DictDump(char *engstr, char *phonstr) {
long r;
  r = 0;
  if (SpeechInitialized) {
    BufferData(engstr, TTS_DATA_WRITE | TTS_DATA_FIRST);
    BufferData(phonstr, TTS_DATA_WRITE | TTS_DATA_SECOND);
    r = fpDrvEntry(TTS_FUNC_DICT_DUMP);
  }
  return(r);
}

short Say(char *engstr) {
short r;
  r = 0;
  if (SpeechInitialized) {
    BufferData(engstr, TTS_DATA_WRITE | TTS_DATA_FIRST);
    r = (short) fpDrvEntry(TTS_FUNC_SAY);
  }
  return(r);
}

/* male? */
short SpeakM(char *phonstr, short ton, short vol, short pit, short spd) {
short r;
  r = 0;
  if (SpeechInitialized) {
    BufferData(phonstr, TTS_DATA_WRITE | TTS_DATA_SECOND);
    /* tone: 0 - bass; 1 - tremble */
    if (ton >= 0) { fpGlob[TTS_GLOB_TONE] = ClipData(ton, 0, 1); }
    if (vol >= 0) { fpGlob[TTS_GLOB_VOLUME] = ClipData(vol, 0, 9); }
    if (pit >= 0) { fpGlob[TTS_GLOB_PITCH] = ClipData(pit, 0, 9); }
    if (spd >= 0) { fpGlob[TTS_GLOB_SPEED] = ClipData(spd, 0, 9); }
    r = (short) fpDrvEntry(TTS_FUNC_SPEAKM);
  }
  return(r);
}

/* female? */
short SpeakF(char *phonstr, short ton, short vol, short pit, short spd) {
short r;
  r = 0;
  if (SpeechInitialized) {
    BufferData(phonstr, TTS_DATA_WRITE | TTS_DATA_SECOND);
    /* tone: 0 - bass; 1 - tremble */
    if (ton >= 0) { fpGlob[TTS_GLOB_TONE] = ClipData(ton, 0, 1); }
    if (vol >= 0) { fpGlob[TTS_GLOB_VOLUME] = ClipData(vol, 0, 9); }
    if (pit >= 0) { fpGlob[TTS_GLOB_PITCH] = ClipData(pit, 0, 9); }
    if (spd >= 0) { fpGlob[TTS_GLOB_SPEED] = ClipData(spd, 0, 9); }
    r = (short) fpDrvEntry(TTS_FUNC_SPEAKF);
  }
  return(r);
}

/* code taken from Set-echo.exe */
short SetEcho(short parm) {
FPWORD fp;
short r;
  r = 0;
  if (SpeechInitialized) {
    fp = (FPWORD) getvect(0xF3);
    /* program help says from 0 to 4000 but actual code clip it at 3950 */
    *fp = ClipData(parm, 0, 3950);
    r = 1;
  }
  return(r);
}

/* restore default settings */
short ResetSpeech(void) {
short r;
  r = 0;
  if (SpeechInitialized) {
    /* reset to default settings */
    SetGlobals(0, 0, 5, 5, 5);
    SetEcho(0);
    /* for whatever reason echo program do this (clears/reinit sound buffer?) */
    Say(" .");
    r = 1;
  }
  return(r);
}
