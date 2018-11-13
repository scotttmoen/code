/*This is the 2/16/15 Disc Driver*/
//i2c = 0x60,0x20,0x70
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <Adafruit_RGBLCDShield.h>
#include <AFMotor.h>

AF_DCMotor motor2(2,MOTOR12_64KHZ);
// ************************************************
// DiSplay Variables and constants
// ************************************************

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();
// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

#define BUTTON_SHIFT BUTTON_SELECT

unsigned long lastInput = 0; // last button press
const int logInterval = 10000; // log every 10 seconds
long lastLogTime = 0;

// ************************************************
// States for state machine
// ************************************************
enum operatingState { 
  OFF = 0, SETACCEL, SETDECEL, MAXSPEED, DURATION, RUN};
operatingState opState = OFF;


double up;
double down;
double fast;
double time;

void setup()
{
  Serial.begin(9600);

  // Initialize LCD DiSplay 

  lcd.begin(16, 2);
  lcd.setBacklight(RED);
  lcd.print(F("  Spinanator"));
  lcd.setCursor(0, 1);
  lcd.print(F("   3000!"));
  delay(3000);  // Splash screen
}
void loop()
{
  // wait for button release before changing state
  while(ReadButtons() != 0) {
  }

  lcd.clear();

  //Serial.println(opState);

  switch (opState)
  {
  case OFF:
    Off();
    break;
  case SETACCEL:
    SetAccel();
    break;
  case SETDECEL:
    SetDecel();
    break;
  case MAXSPEED:
    SetMaxSpeed();
    break;
  case DURATION:
    SetDuration();
    break;
  case RUN:
    Run();
    break;
  }
}
//**************************************OFF
void Off()
{
  lcd.setBacklight(0);
  lcd.print(F("    Spinanator"));
  lcd.setCursor(0, 1);
  lcd.print(F(" Hasta La Vista!"));
  uint8_t buttons = 0;

  while(!(buttons & (BUTTON_RIGHT)))
  {
    buttons = ReadButtons();
  }

  
  opState = RUN; // start control
}
//*************************************Set Accel
void SetAccel()
{
  lcd.setBacklight(RED);
  lcd.print(F("Set Accel:"));
  uint8_t buttons = 0;
  while(true)
  {
    buttons = ReadButtons();

    float increment = 0.1;
    if (buttons & BUTTON_SHIFT)
    {
      increment *= 10;
    }
    if (buttons & BUTTON_LEFT)
    {
      opState = RUN;
      return;
    }
    if (buttons & BUTTON_RIGHT)
    {
      opState = SETDECEL;
      return;
    }
    if (buttons & BUTTON_UP)
    {
      up += increment;
      delay(200);
    }
    if (buttons & BUTTON_DOWN)
    {
      up -= increment;
      delay(200);
    }

    if ((millis() - lastInput) > 3000)  // return to RUN after 3 seconds idle
    {
      opState = RUN;
      return;
    }
    lcd.setCursor(0,1);
    lcd.print(up);
    lcd.print(" ");
    DoControl();
  }
}
//************************************Set Decel
void SetDecel()
{
  lcd.setBacklight(RED);
  lcd.print(F("Set Decel:"));
  uint8_t buttons = 0;
  while(true)
  {
    buttons = ReadButtons();

    float increment = 0.1;
    if (buttons & BUTTON_SHIFT)
    {
      increment *= 10;
    }
    if (buttons & BUTTON_LEFT)
    {
      opState = SETACCEL;
      return;
    }
    if (buttons & BUTTON_RIGHT)
    {
      opState = MAXSPEED;
      return;
    }
    if (buttons & BUTTON_UP)
    {
      down += increment;
      delay(200);
    }
    if (buttons & BUTTON_DOWN)
    {
      down -= increment;
      delay(200);
    }

    if ((millis() - lastInput) > 3000)  // return to RUN after 3 seconds idle
    {
      opState = RUN;
      return;
    }
    lcd.setCursor(0,1);
    lcd.print(down);
    lcd.print(" ");
    DoControl();
  }
}
//**************************************Maxspeed
void SetMaxSpeed()
{
  lcd.setBacklight(RED);
  lcd.print(F("Set Maxspeed:"));
  uint8_t buttons = 0;
  while(true)
  {
    buttons = ReadButtons();

    float increment = 0.1;
    if (buttons & BUTTON_SHIFT)
    {
      increment *= 10;
    }
    if (buttons & BUTTON_LEFT)
    {
      opState = SETDECEL;
      return;
    }
    if (buttons & BUTTON_RIGHT)
    {
      opState = DURATION;
      return;
    }
    if (buttons & BUTTON_UP)
    {
      fast += increment;
      delay(200);
    }
    if (buttons & BUTTON_DOWN)
    {
      fast -= increment;
      delay(200);
    }

    if ((millis() - lastInput) > 3000)  // return to RUN after 3 seconds idle
    {
      opState = RUN;
      return;
    }
    lcd.setCursor(0,1);
    lcd.print(fast);
    lcd.print(" ");
    DoControl();
  }
}
//************************************Duration
void SetDuration()
{
  lcd.setBacklight(RED);
  lcd.print(F("Set Duration:"));
  uint8_t buttons = 0;
  while(true)
  {
    buttons = ReadButtons();

    float increment = 0.1;
    if (buttons & BUTTON_SHIFT)
    {
      increment *= 10;
    }
    if (buttons & BUTTON_LEFT)
    {
      opState = MAXSPEED;
      return;
    }
    if (buttons & BUTTON_RIGHT)
    {
      opState = SETACCEL;
      return;
    }
    if (buttons & BUTTON_UP)
    {
      time += increment;
      delay(200);
    }
    if (buttons & BUTTON_DOWN)
    {
      time -= increment;
      delay(200);
    }

    if ((millis() - lastInput) > 3000)  // return to RUN after 3 seconds idle
    {
      opState = RUN;
      return;
    }
    lcd.setCursor(0,1);
    lcd.print(time);
    lcd.print(" ");
    DoControl();
  }
}
//*********************************************************************THIS IS WHERE I LEFT OFF
void Run()
{
  // set up the LCD's number of rows and columns:

  lcd.print(F("Duration: "));
  lcd.print(time);
  lcd.setCursor(0, 1);
  //lcd.write(1);
  lcd.print(F("Wait Please"));

  uint8_t buttons = 0;
  while(true)
  {
 
    buttons = ReadButtons();
   if (buttons & BUTTON_RIGHT)
    {
      opState = SETACCEL;
      return;
    }
    else if (buttons & BUTTON_LEFT)
    {
      opState = OFF;
      return;
    }
    DoControl();

  }
}


// ************************************************
// Execute the control loop
// ************************************************
void DoControl()
{

  for (int v=0;v<=10;v++){
    motor2.setSpeed(v);
    motor2.run(FORWARD);
    Serial.println(v);
    delay(1000);
  }
  for(int v=50;v<=fast;v++){
    motor2.setSpeed(v);
    motor2.run(FORWARD);
    Serial.println(v);
    delay(1000*up);
  }
  delay(time*1000);


  for(int v=fast;v>=0;v--){
    motor2.setSpeed(v);
    motor2.run(FORWARD);
    Serial.println(v);
    delay(1000*down);
  }
  motor2.run(RELEASE);
  while(1) { 
  }
}
uint8_t ReadButtons()
{
  uint8_t buttons = lcd.readButtons();
  if (buttons != 0)
  {
    lastInput = millis();
  }
  return buttons;
}


