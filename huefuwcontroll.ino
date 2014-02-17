#define pwmPin 11 //PIN to the 2 enable ports (pin 3 and 4)
#define rePin 4 //reversePIN (pin 1)
#define fwPin 5 //forwardPIN (pin 2)
#define FORWARD true
#define REVERSE false
#define potiPin A0
const int numReadings = 64;
int total = 0;
int index = 0;
int readings[numReadings];
int average = 0;
int veloc = 0;
int cspeed = 0;


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
    case -255 ... -11:
      setDir(REVERSE);
      analogWrite(pwmPin, -veloc);
      break;
    case -10 ... 10:
      setBrake();
      analogWrite(pwmPin, 16);
      delay(12);
      break;
    case 11 ... 255:
      setDir(FORWARD);
      analogWrite(pwmPin, veloc);
      break;  
  }
}


void setup()  { 
  TCCR2B = TCCR2B & 0b11111000 | 0x06; //set PWM requency of PIN 3 and 11 to ~3kHz
  pinMode(fwPin, OUTPUT);
  pinMode(rePin, OUTPUT);
  setDir(FORWARD);
  for (int thisReading = 0; thisReading < numReadings; thisReading++)
    readings[thisReading] = 0;
} 

void loop()  { 
  total = total - readings[index];
  readings[index] = analogRead(potiPin)/2;
  total = total + readings[index];
  index++;
  if (index >= numReadings){
    index = 0;
  }
  average = total / numReadings;
  cspeed = -255+average;
  setSpeed();
}


