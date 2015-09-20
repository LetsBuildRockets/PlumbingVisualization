#include <avr/interrupt.h>

typedef const int pin;

pin pushButton = 8;//pin
pin valveInputPin = 6;
pin dataPin = 11;
pin clockPin = 13;
pin latchPin = 9;
pin valvePins[5] = {0}; //TODO: actually fill
const int interruptNumber = 0;  // == dp 2

bool oldValveStatuses[5] = {0};
volatile bool valveStatuses[5] = {0};
byte lightStatuses[7] = {0};

int test_lightnumber = 0;

const int whiteStart = 0;
const int yellowStart = 11;
const int redStart = 20;
const int greenStart = 25;

void updateValveStatuses(){
  for(int i = 0; i < 5; i++) {
    oldValveStatuses[i] = valveStatuses[i];
  }
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

void changeLight(int light, void (*action)(int), int delayTime = 100){
  action(light);
  lightLights();
  delay(delayTime);
}

void changeLine(int startLight, int endLight, void (*action)(int), int delayTime = 100){
  if(startLight < endLight) endLight++;
  else if(startLight > endLight) endLight--;
  for(int counter = startLight; counter - endLight != 0;){
    action(counter);
    lightLights();
    delay(delayTime);    
    if(startLight < endLight) counter++;
    else counter--;
  }
}
int getLightAction(int valveNumber, int (*valves)){
  if(oldValveStatuses[valveNumber] == valves[valveNumber]) return 0;
  else if(valves[valveNumber] == true) return 1; //turned on
  else return -1; //turned off
}
void play(){
  int valves[5] = {0};
  for(int i = 0; i < 5; i++) {
    valves[i] = valveStatuses[i];
  }  
  // valve 0
  if(getLightAction(0, valves) == 1) {
    changeLight(redStart, addLight);
    changeLine(whiteStart + 2, whiteStart + 10, addLight);
    delay(200);
    changeLine(yellowStart, yellowStart + 5, addLight);
    changeLine(greenStart + 15, greenStart + 3, addLight);
  } else if(getLightAction(0, valves) == -1) {
    changeLight(redStart, removeLight);
  }

  // valve 3
  if(getLightAction(3, valves) == 1) {
    changeLight(redStart + 3, addLight);
    changeLine(yellowStart + 6, yellowStart + 8, addLight);
  } else if (getLightAction(3, valves) == -1) {
    changeLight(redStart + 3, removeLight);
    changeLine(yellowStart + 6, yellowStart + 8, removeLight);
  }
  // valve 4
  if(getLightAction(4, valves) == 1) {
    changeLight(redStart + 4, addLight);
    changeLine(greenStart + 2, greenStart, addLight);
  } else if (getLightAction(4, valves) == -1) {
    changeLight(redStart + 4, removeLight);
    changeLine(greenStart + 2, greenStart, removeLight);
  }
  // valve 1
  if(getLightAction(1, valves) == 1) {
    changeLight(redStart + 1, addLight);
    changeLine(whiteStart + 8, whiteStart +2, removeLight);
    changeLine(greenStart + 3, greenStart + 15, removeLight);
  } else if (getLightAction(1, valves) == -1) {
    changeLight(redStart + 1, removeLight);
  }
  // valve 2
  if(getLightAction(2, valves) == 1) {
    changeLight(redStart + 2, addLight);
    changeLine(whiteStart + 10, whiteStart + 9, removeLight);
    changeLine(yellowStart + 5, yellowStart, removeLight);
  } else if (getLightAction(2, valves) == -1) {
    changeLight(redStart + 2, removeLight);
  }
}

void setup(){
  attachInterrupt(interruptNumber, updateValveStatuses, FALLING);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(pushButton, INPUT_PULLUP);
  pinMode(valveInputPin, INPUT);
  Serial.begin(9600);
  changeLine(whiteStart, whiteStart + 1, addLight);

}

void loop(){
  displayLoop();
//  play();
}


/* For when test stand communication doesn't work */
int stepDelayMs = 5000;
void displayLoop(){

  
  play();
  
  while(digitalRead(pushButton) && !digitalRead(valveInputPin) ); //wait for button press

  valveStatuses[0] = true;
  Serial.println("open valve 0");
  play();
  oldValveStatuses[0] = true;
  delay(stepDelayMs);

  valveStatuses[3] = true;
  Serial.println("Open valve 3");
  play();
  oldValveStatuses[3] = true;
  delay(stepDelayMs); 

  valveStatuses[4] = true;
  Serial.println("Open valve 4");
  play();
  oldValveStatuses[4] = true;
  delay(stepDelayMs);

  valveStatuses[3] = false;
  Serial.println("close valve 3");
  play();
  oldValveStatuses[3] = false;
  //delay(stepDelayMs); // not exciting enough

  valveStatuses[4] = false;
  Serial.println("close valve 4");
  play();
  delay(stepDelayMs);
  oldValveStatuses[4] = false;

  valveStatuses[0] = false;
  Serial.println("close valve 0");
  play();
  delay(stepDelayMs);
  oldValveStatuses[0] = false;
  
  valveStatuses[1] = true;
  Serial.println("open valve 1");
  play();
  delay(stepDelayMs);
  oldValveStatuses[1] = true;

  valveStatuses[2] = true;
  Serial.println("open valve 2");
  play();
  delay(stepDelayMs);
  oldValveStatuses[2] = true;
  
  valveStatuses[1] = false;
  valveStatuses[2] = false;
  Serial.println("close release valves");
  play();
  delay(stepDelayMs);
  oldValveStatuses[1] = false;
  oldValveStatuses[2] = false;
}

void test_loop(){
  lightStatuses[test_lightnumber/8] = 1 << (test_lightnumber % 8);
  Serial.println(test_lightnumber);
  lightLights();
  delay(500);
  lightStatuses[test_lightnumber/8] = 0;
  test_lightnumber = (test_lightnumber + 1) % 56;

}
