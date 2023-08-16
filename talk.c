#include <stdio.h>
#include "speech.h"

void SayText(char *s) {
  /* display text */
  puts(s);
  /* say it */
  Say(s);
}

int main(int argc, char *argv[]) {
int i;
  /* silent, do not output anything */
  if (argc <= 1) {
    puts(
      "Sound Blaster Talker Text-to-speech example test program.\n"
      "(c) SysTools 2023\n"
      "http://systools.losthost.org/\n\n"
      "Usage: talk [words] [to] [say] ...\n"
    );
  }
  if (!DetectSpeech()) {
    puts("Error: SmoothTalker (SBTALKER) is not installed.\n");
    return(1);
  }
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

    /* since all settings are global - it restore before exit */
    ResetSpeech();
  }
  return(0);
}
