#include <stdio.h>
#include "speech.h"

void SayText(char *s) {
char p[256];
  /* display text */
  puts(s);
  /* show phonetical format */
  Parser(s, p, sizeof(p));
  puts(p);
  /* say it */
  Say(s);
  /*puts("SpeakM");
  SpeakM(p, -1, -1, -1, -1);
  puts("SpeakF");
  SpeakF(p, -1, -1, -1, -1);*/
}

int main(int argc, char *argv[]) {
int i;
  /* silent, do not output anything */
  if (argc <= 1) {
    puts(
      "First Byte Text-to-Speech Engine example program\n"
      "(c) SysTools 2023\n"
      "http://systools.losthost.org/\n\n"
      "Usage: talk [words] [to] [say] ...\n"
    );
  }
  if (!DetectSpeech()) {
    puts("Error: First Byte Text-to-Speech Engine is not installed.\n");
    return(1);
  }
  /* since all settings are global - restore to default if already altered */
  ResetSpeech();
  if (argc > 1) {
    /* silent say */
    for (i = 1; i < argc; i++) {
      Say(argv[i]);
    }
  } else {
    /* say something for test */
    SayText("Sound check ....."); /* more dots, more delay */
    SayText("Hello ");
    SayText("world!");

    SetEcho(2000);
    SetGlobals(-1, -1, -1, -1, 0);
    SayText("Hello slow echo world!");

    SetEcho(0);
    SetGlobals(-1, 1, 9, 9, 9);
    /* that sounds hilarious with this settings */
    SayText("Hello modified world!");
  }
  /* since all settings are global - restore before exit */
  ResetSpeech();
  return(0);
}
