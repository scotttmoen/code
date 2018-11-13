

#include <PCD8544.h>

//sets constants
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int motorInPin = A1;  // Analog input pin that the potentiometer is attached to
// Variables etc...
int potValue = 0;
int motorValue = 0;
float sensorValue = 1;       // value read from the pot
float hold = 1;              // hold time between iterations (sort of)
float holdFrequency = 1;
unsigned long microsPrev = 0;       // set up value for timing       
unsigned long microsCurrent = 0;    // used for timing purposes
unsigned long millisSerial =0;      //used for serial output timing
float frequency = 1;         // stores frequency
float rpm = 0;              // rpm
//static const byte sensorPin = 0;
static const byte ledPin = 13;

// The dimensions of the LCD (in pixels)...
static const byte LCD_WIDTH = 84;
static const byte LCD_HEIGHT = 48;

// The number of lines for the temperature chart...
static const byte CHART_HEIGHT = 5;

// A custom "degrees" symbol...
static const byte DEGREES_CHAR = 1;
static const byte degrees_glyph[] = { 
  0x00, 0x07, 0x05, 0x07, 0x00 };

// A bitmap graphic (10x2) of a thermometer...
static const byte THERMO_WIDTH = 10;
static const byte THERMO_HEIGHT = 1;
static const byte thermometer[] = { 
  
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static PCD8544 lcd;
const byte PWMA = 3;  // PWM control (speed) for motor A
//const byte PWMB = 11; // PWM control (speed) for motor B
const byte DIRA = 12; // Direction control for motor A
//const byte DIRB = 13; // Direction control for motor B



void setup() {
  Serial.begin(115200); 

  //initialize pins as outputs
  pinMode(25, OUTPUT);
  pinMode(26, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(28, OUTPUT);
  lcd.begin(LCD_WIDTH, LCD_HEIGHT);

  // Register the custom symbol...
  lcd.createChar(DEGREES_CHAR, degrees_glyph);

  pinMode(ledPin, OUTPUT);

  // The internal 1.1V reference provides for better
  // resolution from the LM35, and is also more stable
  // when powered from either a battery or USB...
  analogReference(INTERNAL);
  //setupArdumoto(); // Set all pins as outputs
}


void loop() {
  // Start beyond the edge of the screen...
  static byte xChart = LCD_WIDTH;

  digitalWrite(ledPin, HIGH);

  // Read the temperature (in celsius)...
  //float temp = (1.1 * analogRead(sensorPin) * 100.0) / 1024.0;
  // this while is a workaround for timer limitations using micros vs millis
  sensorValue = analogRead(analogInPin);   //take potentiometer reading
  hold = sensorValue*55 + 400;             //add some time to scale and for code execution
  microsCurrent = micros();                // collect current time
  if (microsCurrent > microsPrev + hold){  //set up timing loop
    frequency = 1000000/(microsCurrent - microsPrev);    
    microsPrev = microsPrev + hold;            //set up millis for delay stuff

    //set all 6 LEDs high (they were already plugged in)
    digitalWrite(25,HIGH);
    digitalWrite(26,HIGH);
    digitalWrite(27,HIGH);
    digitalWrite(28,HIGH);
    delayMicroseconds(.02*hold);  //leave LEDs on long enough to see

    //set all LEDs low
    digitalWrite(25,LOW);
    digitalWrite(26,LOW);
    digitalWrite(27,LOW);
    digitalWrite(28,LOW);
    if(millis() > millisSerial + 200){ 
      millisSerial=millis();    //reset timer
      holdFrequency = 1000000/hold;    
      //do some math to find values
      rpm = frequency * 60;

      // print the results to the serial monitor:
      //      Serial.print("sensor  " );      //print sensor data for debug
      //      Serial.print(sensorValue);
      //Serial.print("holdFreq = " );                       
      //Serial.print(holdFrequency);
      Serial.print("  freq = " );                       
      Serial.print(frequency);      
      Serial.print("  rpm = ");      
      Serial.println(rpm);

      // Print the temperature (using the custom "degrees" symbol)...
      lcd.setCursor(0, 0);
      lcd.print("  freq = " );
      lcd.print(frequency);
      lcd.print("  rpm = ");
      lcd.print(rpm);

      // Draw the thermometer bitmap at the bottom left corner...
      lcd.setCursor(0, LCD_HEIGHT/8 - THERMO_HEIGHT);
      lcd.drawBitmap(thermometer, THERMO_WIDTH, THERMO_HEIGHT);

      // Wrap the chart's current position...
      if (xChart >= LCD_WIDTH) {
        xChart = THERMO_WIDTH + 2;
      }

      // Update the temperature chart...  
      lcd.setCursor(xChart, 1);
      lcd.drawColumn(CHART_HEIGHT, map(sensorValue, 0, 45, 0, CHART_HEIGHT*8));  // ...clipped to the 0-45C range.
      lcd.drawColumn(CHART_HEIGHT, 0);         // ...with a clear marker to see the current chart position.

      xChart++;
      potValue = analogRead(motorInPin);
      motorValue = map(potValue, 0, 1023, 0, 255);

      analogWrite(3, motorValue); 

      //digitalWrite(ledPin, LOW);  
      delay(500);
    } 
  }
}


/* EOF - Thermometer.ino  */


