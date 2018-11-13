


#include <Stepper.h>
#include <Wire.h>
#include <SD.h>
#include "RTClib.h"
RTC_DS1307 RTC;
File logfile;
const int stepsPerRevolution = 200;  // number of steps per revolution
Stepper Stepp(stepsPerRevolution,4,5,6,7);
int AnalogPin = 0;//sensor input
int amppwr = 3;//ald1726 pwr
int ledPin = 8;
unsigned long cycleminutes  = 10;//time between readings,adjusted for reading time in program
int sortedValues;
int readsperwell = 250;
int wells = 25; //well number

void setup(void){
  Serial.begin(9600);
  pinMode(ledPin,OUTPUT);
  pinMode(amppwr,OUTPUT);//ald1726 pwr
  Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);//SD card power
  if (!SD.begin(10)) {
  }
  Serial.println("card initialized.");
  SD.begin(10);
  char filename[] = "LOGGER00.CSV";
  for (uint8_t p = 0; p < 100; p++) {
    filename[6] = p/10 + '0';
    filename[7] = p%10 + '0';
    if (! SD.exists(filename)) {
      logfile = SD.open(filename, FILE_WRITE);
      break;
    }
  }
  if (! logfile) {
    Serial.println("oops");
  }
  Wire.begin();
  RTC.begin();
  RTC.adjust(DateTime(__DATE__, __TIME__));
  logfile.print(",");
  logfile.print(",");
  logfile.print(",");
  for (int n=1;n<wells+1;n++){
    logfile.print(n);
    logfile.print(",");
 }
}

void loop(void) {
  DateTime now = RTC.now();
  unsigned long sta =(now.unixtime());
  Stepp.setSpeed(60);
  digitalWrite (ledPin, HIGH);
  digitalWrite (amppwr, HIGH);
  logfile.println("");
  logfile.print(millis());
  logfile.print(",");
  logfile.print(now.year(), DEC);
  logfile.print("/");
  logfile.print(now.month(), DEC);
  logfile.print("/");
  logfile.print(now.day(), DEC);
  logfile.print(",");
  logfile.print(now.hour(), DEC);
  logfile.print(":");
  logfile.print(now.minute(), DEC);
  logfile.print(":");
  logfile.print(now.second(), DEC);

  logfile.print(",");

  for(int m=0;m<wells;m++){//loop for each well

delay(1000);
    float (AnalogPin);//loop for each reading
    {
      // read multiple values and sort them to take the mode
      int sortedValues[readsperwell];
      for(int i=0;i<readsperwell;i++){
        int value = analogRead(AnalogPin);
        int j;
        if(value<sortedValues[0] || i==0){
          j=0; //insert at first position
        }
        else{
          for(j=1;j<i;j++){
            if(sortedValues[j-1]<=value && sortedValues[j]>=value){
              // j is insert position
              break;
            }
          }
        }
        for(int k=i;k>j;k--){
          // move all values higher than current reading up one position
          sortedValues[k]=sortedValues[k-1];
        }
        sortedValues[j]=value; //insert current reading
        Serial.println (value);
      }
      //return scaled mode of 10 values
      float returnval = 0;
      for(int i=readsperwell/2-5;i<(readsperwell/2+5);i++){
        returnval +=sortedValues[i];
      }
      returnval = returnval/10;
      float sensor1 = returnval;
      logfile.print(sensor1);
      logfile.print(",");
      Stepp.step(8);
      digitalWrite(4,LOW);
  digitalWrite(5,LOW);
  digitalWrite(6,LOW);
  digitalWrite(7,LOW);
      delay (1000);
    }
  }
  digitalWrite(ledPin, LOW);
  DateTime jill = RTC.now();
  unsigned long endtime =(jill.unixtime());
  Serial.println(endtime);
  Serial.println(sta);
  unsigned long runtime = endtime-sta;
  unsigned long offset = ((cycleminutes*60)-runtime);
  Serial.println(offset);
  logfile.flush();
  delay(offset*1000);
}//for void loop
