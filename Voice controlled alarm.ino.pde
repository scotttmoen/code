#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
  #include "Platform.h"
  #include "SoftwareSerial.h"
#ifndef CDC_ENABLED
  // Shield Jumper on SW
  SoftwareSerial port(12,13);
#else
  // Shield Jumper on HW (for Leonardo)
  #define port Serial1
#endif
#else // Arduino 0022 - use modified NewSoftSerial
  #include "WProgram.h"
  #include "NewSoftSerial.h"
  NewSoftSerial port(12,13);
#endif

#include "EasyVR.h"

EasyVR easyvr(port);

//Groups and Commands
enum Groups
{
  GROUP_0  = 0,
  GROUP_1  = 1,
};

enum Group0 
{
  G0_RA = 0,
};

enum Group1 
{
  G1_CANDLE = 0,
  G1_LIGHT = 1,
  G1_OFF = 2,
  G1_GUARD = 3,
  G1_RELAX = 4,
};


EasyVRBridge bridge;

int8_t group, idx;

void setup()
{
#ifndef CDC_ENABLED
  // bridge mode?
  if (bridge.check())
  {
    cli();
    bridge.loop(0, 1, 12, 13);
  }
  // run normally
  Serial.begin(9600);
  Serial.println("Bridge not started!");
#else
  // bridge mode?
  if (bridge.check())
  {
    port.begin(9600);
    bridge.loop(port);
  }
  Serial.println("Bridge connection aborted!");
#endif
  port.begin(9600);

  while (!easyvr.detect())
  {
    Serial.println("EasyVR not detected!");
    delay(1000);
  }

  easyvr.setPinOutput(EasyVR::IO1, LOW);
  Serial.println("EasyVR detected!");
  easyvr.setTimeout(5);
  easyvr.setLanguage(0);

  group = EasyVR::TRIGGER; //<-- start group (customize)
}

void action();

void loop()
{
  easyvr.setPinOutput(EasyVR::IO1, HIGH); // LED on (listening)

  Serial.print("Say a command in Group ");
  Serial.println(group);
  easyvr.recognizeCommand(group);

  do
  {
    // can do some processing while waiting for a spoken command
  }
  while (!easyvr.hasFinished());
  
  easyvr.setPinOutput(EasyVR::IO1, LOW); // LED off

  idx = easyvr.getWord();
  if (idx >= 0)
  {
    // built-in trigger (ROBOT)
    // group = GROUP_X; <-- jump to another group X
    return;
  }
  idx = easyvr.getCommand();
  if (idx >= 0)
  {
    // print debug message
    uint8_t train = 0;
    char name[32];
    Serial.print("Command: ");
    Serial.print(idx);
    if (easyvr.dumpCommand(group, idx, name, train))
    {
      Serial.print(" = ");
      Serial.println(name);
    }
    else
      Serial.println();
    easyvr.playSound(0, EasyVR::VOL_FULL);
    // perform some action
    action();
  }
  else // errors or timeout
  {
    if (easyvr.isTimeout())
      Serial.println("Timed out, try again...");
    int16_t err = easyvr.getError();
    if (err >= 0)
    {
      Serial.print("Error ");
      Serial.println(err, HEX);
    }
  }
}

void action()
{
    switch (group)
    {
    case GROUP_0:
      switch (idx)
      {
      case G0_RA:
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      }
      break;
    case GROUP_1:
      switch (idx)
      {
      case G1_CANDLE:
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_LIGHT:
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_OFF:
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_GUARD:
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      case G1_RELAX:
        // write your action code here
        // group = GROUP_X; <-- or jump to another group X for composite commands
        break;
      }
      break;
    }
}
