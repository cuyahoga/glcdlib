/// @dir homeGraph
/// Display latest home power consumption info.
/// @see http://jeelabs.org/2012/10/25/who-needs-a-server/
// 2012-10-21 <jc@wippler.nl> http://opensource.org/licenses/mit-license.php

#include <GLCD_ST7565.h>
#include <JeeLib.h>
#include "utility/font_clR6x6.h"
#include "utility/font_8x13B.h"

#define FREQ    RF12_868MHZ
#define GROUP   5
#define SEND_ID 9 // ndoe 9 is the homePower.ino node

#define NUMPINS 3
#define NUMHIST 20

struct Payload {
  word count;
  word tdiff;
};

GLCD_ST7565 glcd;

// track some info to better limit the reception time
MilliTimer receiveTimer;
long lastWake;

// TODO uses fake data for now

word solarHist [NUMHIST] = {
  100, 150, 500, 2000, 4500, 2900, 3100, 3800, 2800, 3500,
  2500, 900, 1300, 3800, 700, 200, 1100, 900, 300, 750
};
word totalHist [NUMHIST] = {
  600, 300, 900, 2500, 120, 190, 1800, 1700, 1200, 800,
  600, 450, 280, 2900, 400, 500, 400, 830, 270, 550
};

ISR(WDT_vect) { Sleepy::watchdogEvent(); }

// scale and display hourly graph
static void showGraph () {
  // determine scale limits
  word maxSolar = 0, maxTotal = 0;
  for (byte i = 0; i < NUMHIST; ++i) {
    if (solarHist[i] > maxSolar) maxSolar = solarHist[i];
    if (totalHist[i] > maxTotal) maxTotal = totalHist[i];
  }
  if (maxSolar < 100 || maxTotal < 100)
    return;
  // scale the graph
  word wattPerTick = (maxSolar + maxTotal + 55) / 56;
  word baseline = 2 + maxSolar / wattPerTick;
  glcd.drawLine(0, baseline, 80, baseline, 1);
  // draw the hourly dots
  for (byte i = 0; i < NUMHIST/4; ++i)
    glcd.setPixel(16 * i + 2, 0, 1);
  // draw the elements
  for (byte i = 0; i < NUMHIST; ++i) {
    byte sHeight = solarHist[i] / wattPerTick;
    byte tHeight = totalHist[i] / wattPerTick;
    byte x = 4 * i + 1;
    glcd.fillRect(x, baseline - sHeight + 1, 3, sHeight, 1);
    glcd.drawRect(x, baseline, 3, tHeight, 1);
  }  
}

// show a single power value on the graphics display
static void showPower (word value, byte ypos) {
  char buf[6];
  if (value <= 100 || value >= 65000)
    strcpy(buf, "    -");
  else {
    long ms = value;
    if (value > 60000)
      ms = 1000L * (value - 60000);
    sprintf(buf, "%5lu", 1800000L / ms);
  }
  glcd.drawString(85,  ypos, buf);
}

// redraw entire display with the latest info
static void showPowerInfo (const struct Payload* payload) {
  glcd.clear();
  glcd.setFont(font_clR6x6);
  glcd.drawString_P(85,  0, PSTR("Solar W"));
  glcd.drawString_P(85, 22, PSTR(" Home W"));
  glcd.drawString_P(85, 44, PSTR("Cooking"));
  glcd.drawString_P(0, 58, PSTR("KWh +2.36 -1.89"));
  glcd.setFont(font_8x13B);
  showPower(payload[0].tdiff, 52);  // cooking
  showPower(payload[1].tdiff, 8);   // solar
  showPower(payload[2].tdiff, 30);  // home
  showGraph();
  glcd.refresh();
}

// go to sleep, wakeup again just before the next packet
static void snoozeJustEnough (bool timingWasGood) {
  const word recvWindow = 150;
  word recvOffTime = 2800;
  if (!timingWasGood)
    recvOffTime -= recvWindow;

  rf12_sleep(RF12_SLEEP);
  // the backlight offers an easy way to see when the radio is on
  // glcd.backLight(0);
  Sleepy::loseSomeTime(recvOffTime);
  // glcd.backLight(10);
  rf12_sleep(RF12_WAKEUP);
  
  lastWake = millis();
  if (timingWasGood)
    receiveTimer.set(recvWindow);
}

void setup () {
  glcd.begin();
  glcd.backLight(0);
  glcd.refresh();
  rf12_initialize(1, FREQ, GROUP);
}

void loop () {
  if (rf12_recvDone() && rf12_crc == 0 && rf12_hdr == SEND_ID &&
                rf12_len >= NUMPINS * sizeof (struct Payload)) {
    showPowerInfo((const struct Payload*) rf12_data);
    snoozeJustEnough(true);
  } else if (receiveTimer.poll())
    snoozeJustEnough(false);
}
