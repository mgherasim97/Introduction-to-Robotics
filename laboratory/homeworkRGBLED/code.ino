
// I had a common cathode RGB LED so the code may differ from my fellow students 
const int pinLEDR = 6;
const int pinLEDG = 5;
const int pinLEDB = 3;
const int pinPOTR = A0;
const int pinPOTG = A1;
const int pinPOTB = A2;


void setup() {

  pinMode(pinLEDR, OUTPUT);
  pinMode(pinLEDG, OUTPUT);
  pinMode(pinLEDB, OUTPUT);
  pinMode(pinPOTR, INPUT);
  pinMode(pinPOTG, INPUT);
  pinMode(pinPOTB, INPUT);
}


void loop() {

int valueR = analogRead(pinPOTR) / 4;
int valueG = analogRead(pinPOTG) / 4;
int valueB = analogRead(pinPOTB) / 4;

analogWrite(pinLEDR, valueR);
analogWrite(pinLEDG, valueG);
analogWrite(pinLEDB, valueB);

}
