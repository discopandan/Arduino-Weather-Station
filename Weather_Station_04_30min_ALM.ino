#include <SPI.h>
#include "SdFat.h"
#include <Wire.h>
#include <DS3232RTC.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <BME280I2C.h>
#include <LowPower.h>



SdFat SD;
File myFile;
BME280I2C bme;

#define ONE_WIRE_BUS 7
#define SPI_SPEED SD_SCK_MHZ(50)
#define SD_CS_PIN SS

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempprobe(&oneWire);

const uint8_t SQW_PIN(2);

int number = 0;

void setup() {

  

  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  
   /*

  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_NONE);

  */
  
  setSyncProvider(RTC.get);
  
  pinMode(SQW_PIN, INPUT_PULLUP);
  attachInterrupt(INT0, alarmIsr, FALLING);

  RTC.setAlarm(ALM1_MATCH_MINUTES, 0, 30, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarmInterrupt(ALARM_1, true);

   
  RTC.setAlarm(ALM2_MATCH_MINUTES, 0, 0, 0, 1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_2, true);

    
  Wire.begin();
  
  digitalWrite(SDA, LOW); // disable internal pullup
  digitalWrite(SCL, LOW);
  
   while(!bme.begin())
  {
    ;
  }
  

  
  if (!SD.begin(SD_CS_PIN, SPI_SPEED)) {
    return;
  }
  
  //SD.remove("test.txt");

  myFile = SD.open("test.txt", FILE_WRITE);
  if (myFile) {
    myFile.print("Index");
    myFile.print(',');
    myFile.print("Date");
    myFile.print(',');
    myFile.print("Time");
    myFile.print(',');
    myFile.print("Unixtime");
    myFile.print(',');
    myFile.print("TemperatureRTC");
    myFile.print(',');
    myFile.print("TemperatureProbe");
    myFile.print(',');
    myFile.print("BME280 Temp C");
    myFile.print(',');
    myFile.print("BME280 Humidity % RH");
    myFile.print(',');
    myFile.print("BME280 Pressure Pa");
    //myFile.print(',');
    myFile.print('\n');
    myFile.close();
    Serial.println();
  }
  
  tempprobe.begin();

  digitalWrite(4, LOW);
  //pinMode(4, INPUT);
 
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
  
}

volatile boolean alarmIsrWasCalled = false;

void alarmIsr()
{
    alarmIsrWasCalled = true;
}

void loop() {

  if (alarmIsrWasCalled)
    { 
        digitalWrite(LED_BUILTIN, HIGH);
        //pinMode(4, OUTPUT);
        digitalWrite(4, HIGH);
       
        
        if (!SD.begin(SD_CS_PIN, SPI_SPEED)) {
          return;
        }
        

        while(!bme.begin())
        {
          ;
        }
        

        tempprobe.begin();
                     
        detachInterrupt(0);
        
        if (RTC.alarm(ALARM_1))
        {
            
        }
        if (RTC.alarm(ALARM_2))
        {
            
        }
        
        alarmIsrWasCalled = false;

        setSyncProvider(RTC.get);    
  
        tempprobe.requestTemperatures();
      
        float temp(NAN), hum(NAN), pres(NAN);
      
        BME280::TempUnit tempUnit(BME280::TempUnit_Celsius);
        BME280::PresUnit presUnit(BME280::PresUnit_Pa);
      
        bme.read(pres, temp, hum, tempUnit, presUnit);
      
        myFile = SD.open("test.txt", FILE_WRITE);
        if (myFile) {
           
          myFile.print(number);
          myFile.print(',');
          myFile.print(year(), DEC);
          myFile.print('-');
          myFile.print(month(), DEC);
          myFile.print('-');
          myFile.print(day(), DEC);
          myFile.print(',');
          myFile.print(hour(), DEC);
          myFile.print(':');
          myFile.print(minute(), DEC);
          myFile.print(':');
          myFile.print(second(), DEC);
          myFile.print(',');
          myFile.print(now());
          myFile.print(',');
          myFile.print(RTC.temperature()/4.0);
          myFile.print(',');
          myFile.print(tempprobe.getTempCByIndex(0));
          myFile.print(',');
          myFile.print(temp);
          myFile.print(',');
          myFile.print(hum);
          myFile.print(',');
          myFile.print(pres);
          myFile.print('\n'); 
          
          number++;
          
          myFile.close();
          
        }
        
      
       
        attachInterrupt(INT0, alarmIsr, FALLING);
        digitalWrite(4, LOW);
        
        //pinMode(4, INPUT);
        
        digitalWrite(LED_BUILTIN, LOW);
        
        LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
        
  }
}
