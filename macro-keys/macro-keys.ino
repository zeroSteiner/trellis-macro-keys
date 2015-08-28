#include <Wire.h>

// https://github.com/adafruit/Adafruit_Trellis_Library/archive/master.zip
#include <Adafruit_Trellis.h>

// https://github.com/adafruit/Pro_Trinket_USB_Keyboard_Library/archive/master.zip
#include <ProTrinketKeyboard.h>

#define PIN_SWITCH 4
#define PIN_RST_LED 13
#define PIN_TRE_INT A2
#define TRE_NUM_KEYS 16

Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_TrellisSet trellis = Adafruit_TrellisSet(&matrix0);

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

void setup() {
  // initialize pins
  pinMode(PIN_SWITCH, INPUT);
  pinMode(PIN_RST_LED, OUTPUT);
  pinMode(PIN_TRE_INT, INPUT);
  digitalWrite(PIN_TRE_INT, HIGH);

  trellis.begin(0x70);
  trellis_set_lights();

  TrinketKeyboard.begin();
}

// the loop function runs over and over again forever
void loop() {
  uint8_t iCtr = 0;

  for (iCtr=0; iCtr<6; iCtr++) {
    TrinketKeyboard.poll();
    delay(5);
  }

  trellis_set_lights();

  if (digitalRead(PIN_TRE_INT)) {
    return;
  }

  if (!trellis.readSwitches()) {
    return;
  }

  for (uint8_t iKey=0; iKey<16; iKey++) {
    if (!trellis.justPressed(iKey)) {
      continue;
    }

    switch (iKey) {
      case 0:
        TrinketKeyboard.print("KEY 0");
        break;
      case 1:
        TrinketKeyboard.print("KEY 1");
        break;
    }
  }
}
