const int pinA = 12;
const int pinB = 8;
const int pinC = 5;
const int pinD = 3;
const int pinE = 2;
const int pinF = 11;
const int pinG = 6;
const int pinDP = 4;
const int pinD1 = 7;
const int pinD2 = 9;
const int pinD3 = 10;
const int pinD4 = 13;

const int pinSW = A2; // digital pin connected to switch output
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output

const int segSize = 8;

const int noOfDisplays = 4;
const int noOfDigits = 10;

byte stateDP = LOW;
byte lockedIN = 0; //if the selected digit  is locked in or not
int selected = 0; //which digit is selected 0 is default
int xValue = 0;
int yValue = 0;
int switchValue;
int showSelected = 1;
unsigned long previousSelectedTime = 0;
unsigned long previousDigitSwitchTime = 0;
unsigned long previousNumberSwitchTime = 0;
unsigned long previousSwitchTime = 0;
unsigned long currentTime;


// segments array, similar to before
int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

// digits array, to switch between them easily
int digits[noOfDisplays] = {
  pinD1, pinD2, pinD3, pinD4
};

int digitValue[noOfDisplays] = {//digit values at the current time
  7, 3, 5, 4
};

byte digitMatrix[noOfDigits + 1][segSize - 1] = {
// a b c d e f g NOTHING
{1, 1, 1, 1, 1, 1, 0}, // 0
{0, 1, 1, 0, 0, 0, 0}, // 1
{1, 1, 0, 1, 1, 0, 1}, // 2
{1, 1, 1, 1, 0, 0, 1}, // 3
{0, 1, 1, 0, 0, 1, 1}, // 4
{1, 0, 1, 1, 0, 1, 1}, // 5
{1, 0, 1, 1, 1, 1, 1}, // 6
{1, 1, 1, 0, 0, 0, 0}, // 7
{1, 1, 1, 1, 1, 1, 1}, // 8
{1, 1, 1, 1, 0, 1, 1}, // 9
{0, 0, 0, 0, 0, 0, 0} //EMPTY
};



void displayNumber(byte digit, byte decimalPoint) {
  for (int i = 0; i < segSize - 1; i++) 
    digitalWrite(segments[i], digitMatrix[digit][i]);
    
// write the decimalPoint to DP pin
  digitalWrite(segments[segSize - 1], decimalPoint);
}



// activate the display no. received as param
void showDigit(int num) {
  for (int i = 0; i < noOfDisplays; i++) {
    digitalWrite(digits[i], HIGH);
  }
  digitalWrite(digits[num], LOW); //LOW means activated in this config
}



void setup() {
  for (int i = 0; i < segSize; i++){
    pinMode(segments[i], OUTPUT);
  }
  
  for (int i = 0; i < noOfDisplays; i++){
    pinMode(digits[i], OUTPUT);
  }
  
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);
  pinMode(pinSW, INPUT_PULLUP);
  Serial.begin(9600);
}



void routineShow(){
  
  for(int i = 0; i < noOfDisplays; ++i){
      int show = 1;
      stateDP = 0;
      if(currentTime > previousSelectedTime + 500 ){ //the selected digit blinker
        showSelected = !showSelected;
        previousSelectedTime = currentTime;
      }
      
      if(i == selected ){ // if we reached the selected digit we show it's own separate status
        if(!lockedIN) //if the digit is not locked in we have to make it blink
          show = showSelected;
        stateDP = HIGH;
      }
        
      showDigit(i);
      if(show)
        displayNumber(digitValue[i], stateDP);
      else
        displayNumber(10, stateDP);// 10 is the codenumber for an empty digit
      delay(5);
  }

  
}



//changes the current digit if required
void digitChange(){
    xValue = analogRead(pinX);
  
    if(xValue < 400 && previousDigitSwitchTime + 400 < currentTime){
      if(selected + 1 > 3)
        selected = 0;
      else
        ++selected;
      previousDigitSwitchTime = currentTime;
    }
    else
      if(xValue > 600 &&  previousDigitSwitchTime + 400 < currentTime){
      if(selected - 1 < 0)
        selected = 3;
      else
        --selected;
      previousDigitSwitchTime = currentTime;
    }
}



//sense a button press and changes the lockedIN variable accrdingly
void switchLock(){
  switchValue = digitalRead(pinSW);
  if(!switchValue && previousSwitchTime + 1000 < currentTime){
    lockedIN = !lockedIN;
    previousSwitchTime = currentTime;
  } 
}



//changes the number of the current selected physical digit
void numberChange(){

    yValue = analogRead(pinY);
    if(yValue < 400 && previousNumberSwitchTime + 400 < currentTime){
      if(digitValue[selected] - 1 < 0)
        digitValue[selected] = 9;
      else
        --digitValue[selected];
      previousNumberSwitchTime = currentTime;
    }
    else
      if(yValue > 600 &&  previousNumberSwitchTime + 400 < currentTime){
      if(digitValue[selected] + 1 > 9)
        digitValue[selected] = 0;
      else
        ++digitValue[selected];
      previousNumberSwitchTime = currentTime;
    }
}



void loop() {
  currentTime = millis();

  routineShow();

  if(!lockedIN)
    digitChange();
    
  if(lockedIN)
    numberChange();
  
  switchLock();
 
}
