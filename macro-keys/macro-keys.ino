#include <Wire.h>

// https://github.com/adafruit/Adafruit_Trellis_Library/archive/master.zip
#include <Adafruit_Trellis.h>

// https://github.com/adafruit/Pro_Trinket_USB_Keyboard_Library/archive/master.zip
#include <ProTrinketKeyboard.h>

#define PIN_SWITCH 4
#define PIN_RST_LED 13
#define PIN_TRE_INT A2
#define TRE_NUM_KEYS 16
#define KEY_NOT_SET -1

Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_TrellisSet trellis = Adafruit_TrellisSet(&matrix0);

int8_t iLastKey;

uint8_t key_array_map[] = {
  12, 8,  4,  0,
  13, 9,  5,  1,
  14, 10, 6,  2,
  15, 11, 7,  3
};

/*
 * modifiers, number of keys, keys
 * profile for pycharm
 */
uint8_t key_map[][3] = {
  { KEYCODE_MOD_LEFT_SHIFT, 1, KEYCODE_A },
  { KEYCODE_MOD_LEFT_SHIFT, 1, KEYCODE_F9 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { (KEYCODE_MOD_LEFT_SHIFT | KEYCODE_MOD_LEFT_ALT), 1, KEYCODE_7 },
  { KEYCODE_MOD_LEFT_SHIFT, 1, KEYCODE_F6 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
  { 0, 0, 0 },
};

void hid_sleep(uint8_t iTime) {
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

void trellis_set_lights() {
  bool bAllLightsOn = (digitalRead(PIN_SWITCH) == HIGH);
  uint8_t iLedCursor = 0;

  for (iLedCursor=0; iLedCursor<TRE_NUM_KEYS; iLedCursor++) {
    if (bAllLightsOn) {
      trellis.setLED(iLedCursor);
    } else {
      trellis.clrLED(iLedCursor);
    }
  }

  trellis.writeDisplay();
  return;
}

void trellis_set_lights_off() {
  uint8_t iLedCursor = 0;

  for (iLedCursor=0; iLedCursor<TRE_NUM_KEYS; iLedCursor++) {
    trellis.clrLED(iLedCursor);
  }

  trellis.writeDisplay();
  return;
}

void trellis_set_lights_on() {
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
  //trellis_set_lights();
  trellis_set_lights_off();

  TrinketKeyboard.begin();
  iLastKey = KEY_NOT_SET;
}

// the loop function runs over and over again forever
void loop() {
  uint8_t iKey;
  uint8_t *pMacro;
  hid_sleep(30);
  //trellis_set_lights();

  if (!trellis.readSwitches()) {
    return;
  }

  if (iLastKey != KEY_NOT_SET) {
    if (trellis.justReleased(iLastKey)) {
      trellis.clrLED(iKey);
      iLastKey = KEY_NOT_SET;
    } else {
      return;
    }
  }

  for (iKey=0; iKey<16; iKey++) {
    if (trellis.justReleased(iKey)) {
      trellis.clrLED(iKey);
    }
    if (trellis.justPressed(iKey)) {
      trellis.setLED(iKey);
      if (iLastKey == KEY_NOT_SET) {
        iLastKey = iKey;
        iKey = key_array_map[iKey];

        //pMacro = key_map[iKey];
        //if (pMacro[1] == 0) {
        //  continue;
        //}
        //TrinketKeyboard.pressKeys(KEYCODE_MOD_LEFT_SHIFT, &pMacro[2], 1);
        //TrinketKeyboard.pressKey(0, 0);

        switch (iKey) {
          case 0:
            TrinketKeyboard.pressKey(0, KEYCODE_0);
            break;
          case 1:
            TrinketKeyboard.pressKey(0, KEYCODE_1);
            break;
          case 2:
            TrinketKeyboard.pressKey(0, KEYCODE_2);
            break;
          case 3:
            TrinketKeyboard.pressKey(0, KEYCODE_3);
            break;
        }
        TrinketKeyboard.pressKey(0, 0);
      }
    }
  }

  trellis.writeDisplay();
  return;
}
