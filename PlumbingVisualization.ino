#include <avr/interrupt.h>

typedef const int pin;

pin dataPin = 11;
pin clockPin = 13;
pin latchPin = 9;
pin valvePins[5] = {0}; //TODO: actually fill
const int interruptNumber = 0;  // == dp 2

volatile bool valveStatuses[5] = {0};
byte lightStatuses[7] = {0};
int[] lines[] {
  int white1[2] = {0};
}

int test_lightnumber = 0;

void setup(){
  attachInterrupt(interruptNumber, updateValveStatuses, FALLING);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  Serial.begin(9600);
}

void updateValveStatuses(){
  for(int eger = 0; eger < 5; eger++){
    valveStatuses[eger] = digitalRead(valvePins[eger]);
  } 
}

void lightLights(){
  digitalWrite(latchPin, LOW);
  for(int eger = 6; eger >= 0; eger--){
    shiftOut(dataPin, clockPin, MSBFIRST, lightStatuses[eger]);
  }
  digitalWrite(latchPin, HIGH);
}

void addLight(int light){
  lightStatuses[light/8] = (1 << (light % 8)) | lightStatuses[light/8];
}

void removeLight(int light){
  lightStatuses[light/8] = ~(1 << (light % 8)) & lightStatuses[light/8];
}

void changeLine(int startLight, int endLight, void (*action)(int), int delayTime = 100){
  for(int counter = startLight; counter < (endLight + 1); counter = (counter + 1) % (endLight + 2)){
    action(counter);
    lightLights();
    Serial.println(counter);
    delay(delayTime);
  }
}

void loop(){
/*  fillLine(33,36);
  delay(2000);
  fillLine(36, 37);
  delay(2000);
  clearLine(33, 39);
  delay(2000);*/
  
  changeLine(0, 10, addLight);
  delay(1000);
  changeLine(0, 10, removeLight, 300);
  delay(1000);
}

void test_loop(){
  lightStatuses[test_lightnumber/8] = 1 << (test_lightnumber % 8);
  Serial.println(test_lightnumber);
  lightLights();
  delay(500);
  lightStatuses[test_lightnumber/8] = 0;
  test_lightnumber = (test_lightnumber + 1) % 56;

}
