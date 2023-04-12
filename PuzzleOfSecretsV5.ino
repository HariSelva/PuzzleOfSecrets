#include <FastLED.h>
#include <Servo.h>
#include <Wire.h>
#include "rgb_lcd.h"

// Setting constants
const int button1Pin = 2; 
const int button2Pin = 3; 
const int button3Pin = 4; 
const int button4Pin = 5; 
const int button5Pin = 6; 
const int button6Pin = 7; 
const int button7Pin = 8; 
const int button8Pin = 9;  

const int limitswitch1Pin = 13;
const int limitswitch2Pin = 14;

const int ledPin = 12;

const int servo1Pin = 10;
const int servo2Pin = 11;

// Setting up LED strip
#define LED_PIN     ledPin
#define NUM_LEDS    4
#define BRIGHTNESS  70
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

// create servo object to control a servo
Servo servo1;
Servo servo2;

// Setting up RGB Backlit LCD Screen
rgb_lcd lcd;
const int colorR = 0;
const int colorG = 0;
const int colorB = 150;

// Morse Code Settings
String message = ""; 
int password = -1;
int numMessages = 14;
const String morseMessages[] = {"ONE", "TWO", "THREE", "FOUR","FIVE","SIX", "SEVEN", "EIGHT", "NINE","TEN", "YOU", "EYE", "SEE", "ARE"};
const int morsePassword[] =    {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 85, 73, 67, 82};
// In ASCII code, 'space' is 32, '0'-'9' is 48-57, and 'A'-'Z' is 65-90
const String morse[] = { ".----", "..---", "...--", "....-", ".....", 
  "-....", "--...", "---..", "----.", "-----", ".-", "-...", "-.-.", "-..", 
  ".","..-.","--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", 
  ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", 
  "--.."};
const int dotLen = 500; //delay in milliseconds
const int dashLen = 3*dotLen; // 3X longer than the dot
const int wordLen = 7*dotLen; // space between words.

bool bigClosed = true;
bool smallClosed = true;
bool reset = true;
int buttonBinary = 0;
int posBig = 80;
int posSmall = 80;
unsigned long previousMillis = 0;
int charNum = 0;
int charNum2 = 0;
char character;
bool wait = false;
bool setMessage1 = false;
bool setMessage2 = false;
bool setMessage3 = false;
bool setMessage4 = false;
bool setMessage5 = false;
bool setMessage6 = false;
bool setMessage7 = false;
bool setMessage8 = false;
bool setMessage9 = false;
bool setMessage10 = false;
bool setMessage11 = false;
bool setMessage12 = false;
int bigOpenedTime = 0;
int smallOpenedTime = 0;
int waitTime = 20000;

int servoLowLimit = 0;
int servoHighLimit = 80;

void setup() {
  // put your setup code here, to run once:
  pinMode(button1Pin, INPUT);
  pinMode(button2Pin, INPUT); 
  pinMode(button3Pin, INPUT); 
  pinMode(button4Pin, INPUT); 
  pinMode(button5Pin, INPUT); 
  pinMode(button6Pin, INPUT); 
  pinMode(button7Pin, INPUT); 
  pinMode(button8Pin, INPUT); 

  pinMode(limitswitch1Pin, INPUT_PULLUP);
  pinMode(limitswitch2Pin, INPUT_PULLUP);

  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );

  pinMode(servo1Pin, OUTPUT);
  pinMode(servo2Pin, OUTPUT);
  
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  servo1.write(posBig);
  delay(100);
  servo2.write(posSmall);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // lcd.setRGB(colorR, colorG, colorB);
  // Print a message to the LCD.
  lcd.print("Booting Up");
  delay(500);

  Serial.begin(9600); 
}

void loop() {
  // put your main code here, to run repeatedly:
  int limit1state = digitalRead(limitswitch1Pin);  
  int limit2state = digitalRead(limitswitch2Pin); 
  buttonBinary = getButtonsPressed();
  unsigned long currentMillis = millis();
  // servo1.write(posBig);
  // servo2.write(posSmall);
  
  if (bigClosed && !reset){
    setMessage2 = setMessage3 = setMessage4 = setMessage5 = setMessage6 = setMessage7 = setMessage8 = setMessage9 = setMessage10 = setMessage11 = setMessage12 = false;
    if (!setMessage1){
      LEDOff();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Reset Buttons");
      setMessage1 = true;
    }
    if(buttonBinary == 0){
      reset = true;
      setMessage1 = false;
    }
  } else if (bigClosed && reset){
    setMessage1 = setMessage5 = setMessage6 = setMessage7 = setMessage8 = setMessage9 = setMessage10 = setMessage11 = setMessage12 = false;
    if (!setMessage2){
      LEDOff();
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Input password");
      lcd.setCursor(0, 1);
      lcd.print("for Big Box");
      setMessage2 = true;
    }
    Serial.println(buttonBinary);
    if (buttonBinary == 37){ // Answer is 205 aka buttons 3, 1 and 6 (205)
      if (!setMessage3){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Opening Big Box");
        setMessage3 = true;
      }        
      currentMillis = millis();
      if(currentMillis - previousMillis > 15 && posBig > servoLowLimit) {
        previousMillis = currentMillis;
        servo1.write(posBig);    
        servo2.write(posSmall); 
        Serial.println(posBig);
        posBig--;
      }
      if (posBig <= servoLowLimit){
        if (!setMessage4){
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Big Box Opened");
          setMessage4 = true;
        }
        bigClosed = false;
        bigOpenedTime = millis();
        reset = false;
        randomSeed(analogRead(A1));
        int i = random(0, numMessages-1);
        message = morseMessages[i];
        password = morsePassword[i];
        // message = "ONE";
        // password = 1;
        Serial.println(message);
        Serial.println(password);
        delay(3000);
      }
    }
  } else if (!bigClosed && limit1state == LOW && ((currentMillis - bigOpenedTime) > waitTime)){
    setMessage1 = setMessage2 = setMessage3 = setMessage4 = setMessage7 = setMessage8 = setMessage9 = setMessage10 = setMessage11 = setMessage12 = false;
      if (!setMessage5){
        LEDOff();
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Closing Big Box");
        setMessage5 = true;
      }
      currentMillis = millis();
      if(currentMillis - previousMillis > 15 && posBig < servoHighLimit) {
        previousMillis = currentMillis;
        servo1.write(posBig);    
        servo2.write(posSmall);      
        Serial.println(posBig);
        posBig++;
      }
      if (posBig >= servoHighLimit){
        if (!setMessage6){
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Big Box closed");
          setMessage6 = true;
        }
        bigClosed = true;
      }
  } else if (smallClosed && !reset){
    setMessage1 = setMessage2 = setMessage3 = setMessage4 = setMessage5 = setMessage6 = setMessage8 = setMessage9 = setMessage10 = setMessage11 = setMessage12 = false;
    if (!setMessage7){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Reset Buttons");
      setMessage7 = true;      
    }      
    if(buttonBinary == 0){
      reset = true;
      setMessage7=false;
    }
  } else if (smallClosed && reset){
    setMessage1 = setMessage2 = setMessage3 = setMessage4 = setMessage5 = setMessage6 = setMessage7 = setMessage11 = setMessage12 = false;    
    if (!setMessage8){    
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Input password");
      lcd.setCursor(0, 1);
      lcd.print("for Small Box"); 
      setMessage8 = true;   
    }
    if (buttonBinary == password){ 
      if (!setMessage9){      
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Opening Small Box");
        LEDOff();
        setMessage9 = true;
      }
      currentMillis = millis();
      if(currentMillis - previousMillis > 15 && posSmall > servoLowLimit) {
        previousMillis = currentMillis;
        servo1.write(posBig);    
        servo2.write(posSmall);   
        Serial.println(posSmall);
        posSmall--;
      }
      if (posSmall <= servoLowLimit){
        if (!setMessage10){ 
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Small box opened");
          setMessage10 = true;
        }
        smallClosed = false;
        smallOpenedTime = millis();
        reset = false;
        delay(3000);
      }
    } else {
      currentMillis = millis();
      Serial.println(message);    
      Serial.println(message[charNum]);
      if (message[charNum]) { // fails when reaches the end of the string ("0")
        character = toupper(message[charNum]);
        if (character == ' ') {
          if(currentMillis - previousMillis > wordLen) {
            previousMillis = currentMillis;
            charNum++; // move to the next character in your message
          }     
        } 
        else if (character>47 && character<58) {  // 0-9. ASCII 48-57
          signalMorse(morse[character-48]);
        } 
        else if (character>64 && character<91) {  // A-Z, ASCII 65-90, 10-34 in morse[]
          signalMorse(morse[character-55]);
        }
        else {
          if(currentMillis - previousMillis > wordLen) {
            previousMillis = currentMillis;
            charNum++; // move to the next character in your message
          }
        }
      }
      else if(currentMillis - previousMillis > 2*wordLen) {
        previousMillis = currentMillis;
        charNum = 0;             
      }
    }
  } else if (!smallClosed && limit2state == LOW && ((currentMillis - smallOpenedTime) > waitTime)){
      setMessage1 = setMessage2 = setMessage3 = setMessage4 = setMessage5 = setMessage6 = setMessage7 = setMessage8 = setMessage9 = setMessage10 = false;
      if (!setMessage11){  
        LEDOff();   
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Closing small box");
        setMessage11 = true;
      }        
      currentMillis = millis();
      if(currentMillis - previousMillis > 15 && posSmall < servoHighLimit) {
        previousMillis = currentMillis;
        servo1.write(posBig);    
        servo2.write(posSmall);   
        Serial.println(posSmall);
        posSmall++;
      }
      if (posSmall >= servoHighLimit){
        if (!setMessage12){
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Small box closed");
          setMessage12 = true;    
        }                
        smallClosed = true;
      }
  }
}

int getButtonsPressed() {
    int Sum = 0;

  if (digitalRead(button1Pin) == HIGH){
    Sum += 1;
    // Serial.println("1");
  }     
  if (digitalRead(button2Pin) == HIGH){
    Sum += 2;
    // Serial.println("2");
  }    
  if (digitalRead(button3Pin) == HIGH){
    Sum += 4;
    // Serial.println("3");
  }      
  if (digitalRead(button4Pin) == HIGH){
    Sum += 8;
    // Serial.println("4");
  } 
  if (digitalRead(button5Pin) == HIGH){
    Sum += 16;
    // Serial.println("5");
  }   
  if (digitalRead(button6Pin) == HIGH){
    Sum += 32; 
    // Serial.println("6");
  }
  if (digitalRead(button7Pin) == HIGH){
    Sum += 64; 
    // Serial.println("7");
    }
  if (digitalRead(button8Pin) == HIGH){
    Sum += 128;
    // Serial.println("8");
  }
  // Serial.println(Sum);
  // delay(500);
  return Sum;
}

void LEDOnRed(){
  for( int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = CRGB(255, 0, 0);
  }
  FastLED.show(); 
}

void LEDOnBlue(){
  for( int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = CRGB(0, 0, 255);
  }
  FastLED.show();
}

void LEDOff(){
  for( int i = 0; i < NUM_LEDS; ++i) {
        leds[i] = CRGB(0, 0, 0);
  }
  FastLED.show(); 
}

// blink out one morse code character.
void signalMorse(String c) {
  int LEDstate = 0;
  unsigned long currentMillis = millis();
  if (c[charNum2]) { // for each dot or dash in the letter...  
    if (c[charNum2] == '.' && !wait) { 
      LEDstate = 1;
      if(currentMillis - previousMillis > dotLen) {
        LEDstate = 0;
        wait = true;
        previousMillis = currentMillis;        
      }
    } else if (c[charNum2] == '.' and wait) {
        if(currentMillis - previousMillis > dotLen) {
          previousMillis = currentMillis;
          charNum2++; // move to the next character in your message
          wait = false;
        }
    } else if (c[charNum2] == '-' && !wait) {
      LEDstate = 2;
      if(currentMillis - previousMillis > dashLen) {
        LEDstate = 0;
        wait = true;
        previousMillis = currentMillis;  
      }
    } else if (c[charNum2] == '-' && wait) {      
        if(currentMillis - previousMillis > dotLen) {
          previousMillis = currentMillis;
          charNum2++; // move to the next character in your message
          wait = false;
        }      
    }
  } else {
    if(currentMillis - previousMillis > dashLen-dotLen) {// We already delayed one dot length in the loop. A Word break = dashLen..
      previousMillis = currentMillis;
      charNum2 = 0; // move to the next character in your message
      charNum++;
    }    
  }

  if (LEDstate == 0){
    LEDOff();
  } else if (LEDstate == 1){
    LEDOnBlue();
  } else if (LEDstate == 2){
    LEDOnRed();
  }
}
