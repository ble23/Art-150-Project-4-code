#include <Wire.h>
#include "Adafruit_MCP9808.h"
#include <LiquidCrystal_I2C.h>
#include <Time.h>
#include <TimeLib.h>
#include <Keypad.h>
#include <DS3231.h>
#include "FastLED.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

const int photoResistor = A0, PIN = 12, NUMPIXELS = 12;

Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

Adafruit_MCP9808 tempsensor = Adafruit_MCP9808();
LiquidCrystal_I2C lcd(0x27,20,4); 
int fsrAnalogPin = 0;
int fsrReading;
char state = 'A';
const int trigPin = 10;
const int echoPin = 11;
double duration, inches, cm;
char customKey;
DS3231  rtc(SDA, SCL);
int value, brightness;
CRGBArray<6> led1;
CRGBArray<6> led2;

//----------------------------------------------------------------------------------------------------------------------------------------------------
void setup() {
  pinMode(photoResistor, INPUT);
  lcd.init(); 
  lcd.backlight();
  Serial.begin(115200);
  while (!Serial); 
  Serial.println("MCP9808 demo");
  if (!tempsensor.begin(0x18)) {
    Serial.println("Couldn't find MCP9808! Check your connections and verify the address is correct.");
    while (1);
  }
   Serial.println("Found MCP9808!");
  tempsensor.setResolution(3);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  rtc.begin();
  FastLED.addLeds<NEOPIXEL, 12>(led1, 6);
  FastLED.addLeds<NEOPIXEL, 13>(led2, 6);

  // The following lines can be uncommented to set the date and time
  rtc.setDOW(FRIDAY);     // Set Day-of-Week to SUNDAY
  rtc.setTime(21, 9, 00);     // Set the time to 12:00:00 (24hr format)
  rtc.setDate(10, 5, 2019);   // Set the date to January 1st, 2014
}
//----------------------------------------------------------------------------------------------------------------------------------------------------
void loop() {
  value = analogRead(photoResistor);
  Serial.println(value);
  brightness = map(value, 0, 1023, 50, 0);
  tempsensor.wake();  
  float c = tempsensor.readTempC();
  float f = tempsensor.readTempF();
  tempsensor.shutdown_wake(1);
      
  char customKey = customKeypad.getKey();
  if (customKey)
    state = customKey;
  Serial.println(state);

  lcd.clear();
  
  if(state == 'A'){
      led1 = CRGB::Black;
      led2 = CRGB::Black;
      FastLED.show();
      lcd.setCursor(0,0);
      lcd.print(rtc.getDOWStr(FORMAT_SHORT));
      lcd.print(" - ");
      lcd.print(rtc.getDateStr());
      lcd.setCursor(0,1);
      lcd.print(rtc.getTimeStr());
      delay(500);
  }
      
   if(state == 'B'){  
      led1 = CRGB::Black;
      led2 = CRGB::Black;
      FastLED.show();
      lcd.setCursor(0,0);
      lcd.print(c);
      lcd.setCursor(6,0);
      lcd.print("Celcius");
      lcd.setCursor(0,1);
      lcd.print(f);
      lcd.setCursor(6,1);
      lcd.print("Farenheit");
      delay(500);
   }
       
    if (state == 'C'){
      led1 = CRGB::Black;
      led2 = CRGB::Black;
      FastLED.show();
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      duration = pulseIn(echoPin, HIGH);

      inches = microsecondsToInches(duration);
      cm = microsecondsToCentimeters(duration);

      lcd.setCursor(0,0);
      lcd.print(cm);
      lcd.print(" cm");
      lcd.setCursor(0,1);
      lcd.print(inches);
      lcd.print(" inches");     
      delay(500);
    }  
    if (state == 'D'){
      led1 = CRGB::White;
      led2 = CRGB::White;

      lcd.print("Nightlight Mode");
      
      if (value > 700)
        FastLED.setBrightness(0);
      else
        FastLED.setBrightness(brightness);
      FastLED.show();
    }
}


double microsecondsToInches(double microseconds)
{
  return microseconds / 74 / 2;
}

double microsecondsToCentimeters(double microseconds)
{
  return microseconds / 29 / 2;
}
