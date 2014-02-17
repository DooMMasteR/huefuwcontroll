#include <avr/wdt.h>

#define pwmPin 11 //PIN to the 2 enable ports (pin 3 and 4)
#define rePin 4 //reversePIN (pin 1)
#define fwPin 5 //forwardPIN (pin 2)
#define FORWARD true //FORWAD is true
#define REVERSE false //REVERSE is false
#define potiPin A0 //0..5V PIN for potentiometer
const int numReadings = 64; //arraysize to take average from
int total = 0; //average arrays total (sum)
int index = 0; //current array field
int readings[numReadings]; //array to store readings in
int average = 0; //current average of the array
int veloc = 0; //curent absolute velocity (is)
int cspeed = 0; //current set speed (wanted)


void setDir(boolean dir){
  if (dir) {
    digitalWrite(fwPin, LOW);
    digitalWrite(rePin, HIGH);  
  } else {
    digitalWrite(rePin, LOW);
    digitalWrite(fwPin, HIGH);
  }
}

void setBrake(void){
  digitalWrite(rePin, LOW);
  digitalWrite(fwPin, LOW);
}

void setSpeed(void){
  if (cspeed > veloc){
    veloc++;
  } else if (cspeed < veloc){
    veloc--;
  }
  switch (veloc) {
    case -255 ... -51:
      setDir(REVERSE);
      analogWrite(pwmPin, -veloc+50);
      break;
    case -50 ... 10:
      setBrake();
      analogWrite(pwmPin, -veloc+10);
      break;
    case 11 ... 255:
      setDir(FORWARD);
      analogWrite(pwmPin, veloc);
      break;  
  }
}


void setup()  { 
  wdt_enable(WDTO_500MS); //enable the watchdog at 500ms triptime
  TCCR2B = TCCR2B & 0b11111000 | 0x06; //set PWM requency of PIN 3 and 11 to desired value
  pinMode(fwPin, OUTPUT); //enable the fwDrive enable PINout
  pinMode(rePin, OUTPUT); //enable the reDrive enable PINout
  setDir(FORWARD); //set the direction to FW (just precautious)
  setSpeed(); //set the speed (to 0 in this case)
  
  //initalize the averager to 0
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;
} 

void loop()  {
  wdt_reset(); //reset the WD so it won't trip
  
  //get an average readout value of the potentiometer
  total = total - readings[index]; //substract the now to read value from the arrays total
  readings[index] = analogRead(potiPin)/2; //read the Potentiometer and save half (0-511) in the array
  total = total + readings[index]; //add the read value to the total amount
  index++; //increase array field index for next round
  if (index >= numReadings){ //check if out of bounds
    index = 0; //reset if
  }
  average = total / numReadings; //create average over the arrays total value
  
  cspeed = -255+average; //create feasable current speed (-255..255)
  setSpeed(); //set the current speed
}


