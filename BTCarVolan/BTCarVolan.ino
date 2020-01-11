#include <SoftwareSerial.h>
#define RxPin 2
#define TxPin 4
#define forwardPin 6
#define reversePin 9
#define rightPin 5
#define leftPin 3
#define treshold 60

int joyX ;
int joyY ;

byte readByte;
byte array[4];


SoftwareSerial BlueTooth(TxPin, RxPin);

void setup() {
  // put your setup code here, to run once:
  joyX = 100;
  joyY = 100;
  pinMode(forwardPin, OUTPUT);
  pinMode(reversePin, OUTPUT);
  pinMode(leftPin, OUTPUT);
  pinMode(rightPin, OUTPUT);

  digitalWrite(forwardPin, LOW);
  digitalWrite(reversePin, LOW);
  digitalWrite(rightPin, LOW);
  digitalWrite(leftPin, LOW);

  Serial.begin(9600);
  Serial.write("Bluetooth On ");
  BlueTooth.begin(9600);
  BlueTooth.println("Bluetooth On ");
  establishContact();  // send a byte to establish contact until receiver responds

}
void loop() {
  // put your main code here, to run repeatedly:
  if (BlueTooth.available() >= 3)
  {
    if (BlueTooth.read() == 's') {
      for (int i = 1; i < 3; i++) {
        readByte = BlueTooth.read();  //gets one byte from serial buffer
        array[i] = readByte;
      }
    }
  }
  else {
    joyX = 100;
    joyY = 100;
  }

  joyX = array[1];
  joyY = array[2];

  Serial.println(joyX);
  Serial.println(joyY);

  if (95 < joyY < 105) {
    moveCoasting();
  }

  if (95 < joyX < 105) {
    wheelsNeutral();
  }

  if (joyY > 105) {
    digitalWrite(reversePin, LOW);
    analogMove(joyY, forwardPin);
  }

  if (joyY < 95) {
    digitalWrite(forwardPin, LOW);
    analogMoveNegative(joyY, reversePin);
  }

  if (joyX > 105) {
    digitalWrite(leftPin, LOW);
    analogMove(joyX, rightPin);
  }

  if (joyX < 95) {
    digitalWrite(rightPin, LOW);
    analogMoveNegative(joyX, leftPin);
  }
}

void establishContact() {
  while (BlueTooth.available() <= 0) {
    BlueTooth.println("A");   // send a capital A
    delay(300);
  }
}

void analogMove(int inputAcceleration, int movePin) {
  int motorAcceleration = map(inputAcceleration, 105, 200, treshold, 255);
  analogWrite(movePin, motorAcceleration);
}

void analogMoveNegative(int inputAcceleration, int movePin) {
  int  motorAcceleration = map(inputAcceleration, 95, 0, treshold, 255);
  analogWrite(movePin, motorAcceleration);
}



void moveForward(float time) {
  digitalWrite(reversePin, LOW);
  if (digitalRead(reversePin) == LOW) {
    digitalWrite(forwardPin, HIGH);

  }
  delay(time * 1000);
  digitalWrite(forwardPin, LOW);
}

void moveForward() {
  digitalWrite(reversePin, LOW);
  if (digitalRead(reversePin) == LOW) {
    digitalWrite(forwardPin, HIGH);

  }
}

void moveReverse(float time) {
  digitalWrite(forwardPin, LOW);
  if (digitalRead(forwardPin) == LOW) {
    digitalWrite(reversePin, HIGH);

  }
  delay(time * 1000);
  digitalWrite(reversePin, LOW);
}

void moveReverse() {
  digitalWrite(forwardPin, LOW);
  if (digitalRead(forwardPin) == LOW) {
    digitalWrite(reversePin, HIGH);

  }
}

void moveCoasting() {
  digitalWrite(forwardPin, LOW);
  digitalWrite(reversePin, LOW);
}

void wheelsRight() {
  digitalWrite(leftPin, LOW);
  if (digitalRead(leftPin) == LOW) {
    digitalWrite(rightPin, HIGH);
  }
}

void wheelsNeutral() {
  digitalWrite(rightPin, LOW);
  digitalWrite(leftPin, LOW);

}

void wheelsLeft() {
  digitalWrite(rightPin, LOW);
  if (digitalRead(rightPin) == LOW) {
    digitalWrite(leftPin, HIGH);
  }

}

void testDirection() {
  wheelsRight();
  delay(1000);
  wheelsNeutral();
  delay(1000);
  wheelsLeft();
  delay(1000);
  wheelsNeutral();
  delay(1000);
}

void testTraction(float time) {
  moveForward(time);
  delay(2000);
  moveReverse(time + 0.1);
  delay(1000);
}

void testGeneralMove() {
  wheelsRight();
  delay(1000);
  moveForward(0.2);
  delay(1000);
  moveReverse(0.3);
  delay(1000);
  wheelsNeutral();
  delay(1000);
  wheelsLeft();
  delay(1000);
  moveForward(0.2);
  delay(1000);
  moveReverse(0.3);
  delay(1000);
  wheelsNeutral();
  delay(1000);
}




