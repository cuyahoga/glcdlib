// Display latest home power consumption info.
// 2012-10-21 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <GLCD_ST7565.h>
#include <JeeLib.h>
#include "utility/font_clR6x6.h"
#include "utility/font_8x13B.h"

#define FREQ    RF12_868MHZ
#define GROUP   5
#define SEND_ID 9 // ndoe 9 is the homePower.ino node

#define NUMPINS 3

struct Payload {
  word count;
  word tdiff;
};

GLCD_ST7565 glcd;

// show a single power value on the graphics display
static void showPower (word value, byte ypos) {
  char buf[5];
  if (value == 0 || value >= 65000)
    strcpy(buf, "   -");
  else {
    long ms = value;
    if (value > 60000)
      ms = 1000L * (value - 60000);
    sprintf(buf, "%4lu", 1800000L / (long) value);
  }
  glcd.drawString(90,  ypos, buf);
}

// redraw entire display with the latest info
static void showPowerInfo (const struct Payload* payload) {
  glcd.clear();
  glcd.setFont(font_clR6x6);
  glcd.drawString_P(84,  0, PSTR("Solar W"));
  glcd.drawString_P(84, 22, PSTR(" Home W"));
  glcd.drawString_P(84, 44, PSTR("Cooking"));
  glcd.setFont(font_8x13B);
  showPower(payload[0].tdiff, 52);  // cooking
  showPower(payload[1].tdiff, 8);   // solar
  showPower(payload[2].tdiff, 30);  // home
  glcd.refresh();
}

void setup () {
  glcd.begin();
  glcd.backLight(255);
  rf12_initialize(1, FREQ, GROUP);
}

void loop () {
  if (rf12_recvDone() && rf12_crc == 0 && rf12_hdr == SEND_ID &&
      rf12_len >= NUMPINS * sizeof (struct Payload))
    showPowerInfo((const struct Payload*) rf12_data);
}
