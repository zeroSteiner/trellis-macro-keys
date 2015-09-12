#ifndef MACRO_KEYS_H
#define MACRO_KEYS_H

#define PIN_SWITCH 4
#define PIN_RST_LED 13
#define PIN_TRE_INT A2
#define TRE_DFLT_BRIGHTNESS 3
#define TRE_MAX_BRIGHTNESS 15
#define TRE_NUM_KEYS 16
#define NOT_SET -1

#define MACRO_MAX_SIZE 22
#define MACRO_NUM 4
/* define macro types */
#define MACRO_T_NOT_SET NOT_SET
#define MACRO_T_KEYS 0
#define MACRO_T_STR  1
#define MACRO_T_TRE_BRIGHT 2
#define MACRO_T_TRE_DIM 3
#define MACRO_T_MODE_RST 4

/* define common macros */
#define MACRO_NOT_SET { MACRO_T_NOT_SET, 0, 0, { } }

typedef struct {
  int8_t  iType;
  uint8_t iMod;
  uint8_t iSize;
  union {
    uint8_t iKeys[MACRO_MAX_SIZE];
    char    cStr[MACRO_MAX_SIZE];
  };
} Macro;

#endif
