#include <Wire.h>

// https://github.com/adafruit/Adafruit_Trellis_Library/archive/master.zip
#include <Adafruit_Trellis.h>

// https://github.com/adafruit/Pro_Trinket_USB_Keyboard_Library/archive/master.zip
#include <ProTrinketKeyboard.h>

#define PIN_SWITCH 4
#define PIN_RST_LED 13
#define PIN_TRE_INT A2
#define TRE_BRIGHTNESS 3
#define TRE_MAX_BRIGHTNESS 15
#define TRE_NUM_KEYS 16
#define NOT_SET -1

#define MAX_MACRO_SIZE 32
/* define macro types */
#define MACRO_T_NOT_SET NOT_SET
#define MACRO_T_KEYS 0
#define MACRO_T_STR  1
#define MACRO_T_TRE_BRIGHT 2
#define MACRO_T_TRE_DIM 3
/* define common macros */
#define MACRO_NOT_SET { MACRO_T_NOT_SET, 0, 0, { } }

Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_TrellisSet trellis = Adafruit_TrellisSet(&matrix0);

typedef struct Macro {
  int8_t  iType;
  uint8_t iMod;
  uint8_t iSize;
  union {
    uint8_t iKeys[MAX_MACRO_SIZE];
    char    cStr[MAX_MACRO_SIZE];
  };
};

int8_t iLastKey;
int8_t iTrellisBrightness;

const uint8_t trellisToMacroKeyMap[] = {
  12, 8,  4,  0,
  13, 9,  5,  1,
  14, 10, 6,  2,
  15, 11, 7,  3
};

/*
 * modifiers, number of keys, keys
 * profile for pycharm
 */
Macro macroKeyMap[16] = {
  { MACRO_T_KEYS, KEYCODE_MOD_LEFT_SHIFT, 1, { KEYCODE_F10 } },
  { MACRO_T_KEYS, KEYCODE_MOD_LEFT_SHIFT, 1, { KEYCODE_F9 } },
  MACRO_NOT_SET,
  MACRO_NOT_SET,
  { MACRO_T_KEYS, (KEYCODE_MOD_LEFT_SHIFT | KEYCODE_MOD_LEFT_ALT), 1, { KEYCODE_7 } },
  { MACRO_T_KEYS, KEYCODE_MOD_LEFT_SHIFT, 1, { KEYCODE_F6 } },
  MACRO_NOT_SET,
  { MACRO_T_TRE_BRIGHT, 0, 0, { } },
  { MACRO_T_STR,  0, 0, "Hello World!" },
  { MACRO_T_KEYS, 0, 1, { KEYCODE_A } },
  MACRO_NOT_SET,
  { MACRO_T_TRE_DIM, 0, 0, { } },
  { MACRO_T_KEYS, KEYCODE_MOD_LEFT_ALT, 1, { KEYCODE_1 } },
  { MACRO_T_KEYS, KEYCODE_MOD_LEFT_ALT, 1, { KEYCODE_2 } },
  { MACRO_T_KEYS, KEYCODE_MOD_LEFT_ALT, 1, { KEYCODE_7 } },
  { MACRO_T_KEYS, KEYCODE_MOD_LEFT_ALT, 1, { KEYCODE_F12 } }
};

void hidSleep(uint8_t iTime) {
  uint8_t iParts = (iTime / 10);
  iTime = (iTime % 10);

  while (iParts > 0) {
    TrinketKeyboard.poll();
    delay(10);
    iParts -= 1;
  }

  TrinketKeyboard.poll();
  if (iTime) {
    delay(iTime);
  }
  return;
}

void trellisSetLights() {
  bool bAllLightsOn = (digitalRead(PIN_SWITCH) == HIGH);
  bool bKeyPressed;
  uint8_t iLedCursor = 0;

  for (iLedCursor=0; iLedCursor<TRE_NUM_KEYS; iLedCursor++) {
    bKeyPressed = trellis.isKeyPressed(iLedCursor);
    if (bAllLightsOn) {
      if (bKeyPressed) {
        trellis.clrLED(iLedCursor);
      } else {
        trellis.setLED(iLedCursor);
      }
    } else {
      if (bKeyPressed) {
        trellis.setLED(iLedCursor);
      } else {
        trellis.clrLED(iLedCursor);
      }
    }
  }

  trellis.writeDisplay();
  return;
}

void trellisSetLightsOff() {
  uint8_t iLedCursor = 0;

  for (iLedCursor=0; iLedCursor<TRE_NUM_KEYS; iLedCursor++) {
    trellis.clrLED(iLedCursor);
  }

  trellis.writeDisplay();
  return;
}

void trellisSetLightsOn() {
  uint8_t iLedCursor = 0;

  for (iLedCursor=0; iLedCursor<TRE_NUM_KEYS; iLedCursor++) {
    trellis.setLED(iLedCursor);
  }

  trellis.writeDisplay();
  return;
}

void setup() {
  // initialize pins
  pinMode(PIN_SWITCH, INPUT);
  pinMode(PIN_RST_LED, OUTPUT);
  pinMode(PIN_TRE_INT, INPUT);
  digitalWrite(PIN_TRE_INT, HIGH);

  trellis.begin(0x70);
  iTrellisBrightness = TRE_BRIGHTNESS;
  trellis.setBrightness(iTrellisBrightness);
  trellisSetLights();

  TrinketKeyboard.begin();
  iLastKey = NOT_SET;
}

// the loop function runs over and over again forever
void loop() {
  bool bAllLightsOn = (digitalRead(PIN_SWITCH) == HIGH);
  uint8_t iKey;
  Macro *pMacro;
  hidSleep(30);
  trellisSetLights();

  if (!trellis.readSwitches()) {
    return;
  }

  if (iLastKey != NOT_SET) {
    if (!trellis.justReleased(iLastKey)) {
      return;
    }

    trellis.clrLED(iLastKey);
    iLastKey = NOT_SET;
  }

  for (iKey=0; iKey<TRE_NUM_KEYS; iKey++) {
    if (trellis.justReleased(iKey)) {
      if (bAllLightsOn) {
        trellis.clrLED(iKey);
      } else {
        trellis.setLED(iKey);
      }
    }
    if (trellis.justPressed(iKey)) {
      if (bAllLightsOn) {
        trellis.setLED(iKey);
      } else {
        trellis.clrLED(iKey);
      }

      if (iLastKey == NOT_SET) {
        iLastKey = iKey;
        iKey = trellisToMacroKeyMap[iKey];

        pMacro = &macroKeyMap[iKey];
        switch (pMacro->iType) {
          case MACRO_T_NOT_SET:
            break;
          case MACRO_T_KEYS:
            TrinketKeyboard.pressKeys(pMacro->iMod, pMacro->iKeys, pMacro->iSize);
            TrinketKeyboard.pressKey(0, 0);
            break;
          case MACRO_T_STR:
            TrinketKeyboard.print(pMacro->cStr);
            break;
          case MACRO_T_TRE_BRIGHT:
            if (iTrellisBrightness < TRE_MAX_BRIGHTNESS) {
              iTrellisBrightness += 1;
              trellis.setBrightness(iTrellisBrightness);
            }
            break;
          case MACRO_T_TRE_DIM:
            if (iTrellisBrightness > 1) {
              iTrellisBrightness -= 1;
              trellis.setBrightness(iTrellisBrightness);
            }
            break;
        }
      }
    }
  }

  trellis.writeDisplay();
  return;
}
