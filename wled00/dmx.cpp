#include "wled.h"

/*
 * Support for DMX via MAX485.
 * Change the output pin in src/dependencies/ESPDMX.cpp if needed.
 * Library from:
 * https://github.com/Rickgg/ESP-Dmx
 */

#ifdef WLED_ENABLE_DMX

void handleDMX()
{
  // don't act, when in DMX Proxy mode
  if (e131ProxyUniverse != 0) return;

  uint8_t brightness = strip.getBrightness();

  uint16_t len = strip.getLengthTotal();
  for (int i = DMXStartLED; i < len; i++) {        // uses the amount of LEDs as fixture count

    uint32_t in = strip.getPixelColor(i);     // get the colors for the individual fixtures as suggested by Aircoookie in issue #462
    byte w = in >> 24 & 0xFF;
    byte r = in >> 16 & 0xFF;
    byte g = in >> 8 & 0xFF;
    byte b = in & 0xFF;

    checkDMXShutterChan();

    // If no shutter channel is set, apply brightness to RGB values 
    if(!DMXShutterChan) { 
      r = r*brightness/255;
      g = g*brightness/255;
      b = b*brightness/255;
    }

    int DMXFixtureStart = DMXStart + (DMXGap * (i - DMXStartLED));
    for (int j = 0; j < DMXChannels; j++) {
      int DMXAddr = DMXFixtureStart + j;
      switch (DMXFixtureMap[j]) {
        case 0:        // Set this channel to 0. Good way to tell strobe- and fade-functions to fuck right off.
          dmx.write(DMXAddr, 0);
          break;
        case 1:        // Red
          dmx.write(DMXAddr, r);
          break;
        case 2:        // Green
          dmx.write(DMXAddr, g);
          break;
        case 3:        // Blue
          dmx.write(DMXAddr, b);
          break;
        case 4:        // White
          dmx.write(DMXAddr, w);
          break;
        case 5:        // Shutter channel. Controls the brightness.
          dmx.write(DMXAddr, brightness);
          break;
        case 6:        // Sets this channel to 255. Like 0, but more wholesome.
          dmx.write(DMXAddr, 255);
          break;
      }
    }
  }

  dmx.update();        // update the DMX bus
}

void initDMX() {
  dmx.init(512);        // initialize with bus length
}

void checkDMXShutterChan() {
  DMXShutterChan = false;
  for (int j = 0; j < DMXChannels; j++) {
    if(DMXFixtureMap[j] == 5) {
      DMXShutterChan = true;
    }
  }
}

#else
void handleDMX() {}
void initDMX() {}
void checkDMXShutterChan() {}
#endif
