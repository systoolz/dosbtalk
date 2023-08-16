#ifndef __SPEECH_H
#define __SPEECH_H

#ifdef __cplusplus
extern "C" {
#endif

short DetectSpeech(void);
short SetGlobals(short gen, short ton, short vol, short pit, short spd);
short Say(char *engstr);
short SetEcho(short parm);
short ResetSpeech(void);

#ifdef __cplusplus
}
#endif

#endif
