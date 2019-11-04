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
  Serial.begin(9600);
}


void loop() {

analogWrite(pinLEDR,analogRead(pinPOTR)/4);Serial.println(analogRead(pinPOTR)/4);
analogWrite(pinLEDG,analogRead(pinPOTG)/4);Serial.println(analogRead(pinPOTG)/4);
analogWrite(pinLEDB,analogRead(pinPOTB)/4);Serial.println(analogRead(pinPOTB)/4);
}
