#include <SoftwareSerial.h>
const int RxPin = 2;
const int TxPin = 4;
const int forwardPin = 6;
const int reversePin = 9;
const int rightPin = 5;
const int leftPin = 3;



#define    STX          0x02
#define    ETX          0x03
#define    SLOW         750                            // Datafields refresh rate (ms)
#define    FAST         250                             // Datafields refresh rate (ms)
#define    ledPin       13

SoftwareSerial BlueTooth(TxPin, RxPin);
byte cmd[8] = {0, 0, 0, 0, 0, 0, 0, 0};                 // bytes received
byte buttonStatus = 0;
long previousMillis = 0;                                // will store last time Buttons status was updated
long sendInterval = FAST;                               // interval between Buttons status transmission (milliseconds)
String displayStatus = "xxxx";                          // message to Android device


void setup() {
  // put your setup code here, to run once:

  pinMode(forwardPin, OUTPUT);
  pinMode(reversePin, OUTPUT);
  pinMode(leftPin, OUTPUT);
  pinMode(rightPin, OUTPUT);

  /* For hardware joystick
     pinMode(X_pin, INPUT);
     pinMode(Y_pin, INPUT);*/

  digitalWrite(forwardPin, LOW);
  digitalWrite(reversePin, LOW);
  digitalWrite(rightPin, LOW);
  digitalWrite(leftPin, LOW);

  Serial.begin(9600);
  BlueTooth.begin(9600);

  Serial.println("Bluetooth On ");
  BlueTooth.println("Bluetooth On ");

}
void loop() {
  // put your main code here, to run repeatedly:
  BlueToothCommander();

}

void BlueToothCommander() {
  if (BlueTooth.available() > 0)
  {
    delay(2);
    cmd[0] =  BlueTooth.read();
    if (cmd[0] == STX)  {
      int i = 1;
      while (BlueTooth.available())  {
        delay(1);
        cmd[i] = BlueTooth.read();
        if (cmd[i] > 127 || i > 7)                 break; // Communication error
        if ((cmd[i] == ETX) && (i == 2 || i == 7))   break; // Button or Joystick data
        i++;
      }
      if     (i == 2)          getButtonState(cmd[1]);  // 3 Bytes  ex: < STX "C" ETX >
      else if (i == 7)          getJoystickState(cmd);  // 6 Bytes  ex: < STX "200" "180" ETX >
    }
  }
  sendBlueToothData();
}

void sendBlueToothData()  {
  static long previousMillis = 0;
  long currentMillis = millis();
  if (currentMillis - previousMillis > sendInterval) {  // send data back to smartphone
    previousMillis = currentMillis;

    // Data frame transmitted back from Arduino to Android device:
    // < 0X02   Buttons state   0X01   DataField#1   0x04   DataField#2   0x05   DataField#3    0x03 >
    // < 0X02      "01011"      0X01     "120.00"    0x04     "-4500"     0x05  "Motor enabled" 0x03 >    // example

    BlueTooth.print((char)STX);                                             // Start of Transmission
    BlueTooth.print(getButtonStatusString());  BlueTooth.print((char)0x1);   // buttons status feedback
    BlueTooth.print(GetdataInt1());            BlueTooth.print((char)0x4);   // datafield #1
    BlueTooth.print(GetdataFloat2());          BlueTooth.print((char)0x5);   // datafield #2
    BlueTooth.print(displayStatus);                                         // datafield #3
    BlueTooth.print((char)ETX);                                             // End of Transmission
  }
}

String getButtonStatusString()  {
  String bStatus = "";
  for (int i = 0; i < 6; i++)  {
    if (buttonStatus & (B100000 >> i))      bStatus += "1";
    else                                  bStatus += "0";
  }
  return bStatus;
}

int GetdataInt1()  {              // Data dummy values sent to Android device for demo purpose
  static int i = -30;             // Replace with your own code
  i ++;
  if (i > 0)    i = -30;
  return i;
}

float GetdataFloat2()  {           // Data dummy values sent to Android device for demo purpose
  static float i = 50;             // Replace with your own code
  i -= .5;
  if (i < -50)    i = 50;
  return i;
}

void getJoystickState(byte data[8])    {
  int joyX = (data[1] - 48) * 100 + (data[2] - 48) * 10 + (data[3] - 48); // obtain the Int from the ASCII representation
  int joyY = (data[4] - 48) * 100 + (data[5] - 48) * 10 + (data[6] - 48);
  joyX = joyX - 200;                                                  // Offset to avoid
  joyY = joyY - 200;                                                  // transmitting negative numbers

  if (joyX < -100 || joyX > 100 || joyY < -100 || joyY > 100)     return; // commmunication error

  // Your code here ...
  Serial.print("Joystick position:  ");
  Serial.print(joyX);
  Serial.print(", ");
  Serial.println(joyY);

  if (-5 < joyY < 5) {
    moveCoasting();
  }

  if (-5 < joyX < 5) {
    wheelsNeutral();
  }

  if (joyY > 5) {
    digitalWrite(reversePin, LOW);
    analogMove(joyY, forwardPin);
  }

  if (joyY < -5) {
    digitalWrite(forwardPin, LOW);
    analogMoveNegative(joyY, reversePin);
  }

  if (joyX > 5) {
    digitalWrite(leftPin, LOW);
    analogMove(joyX, rightPin);
  }

  if (joyX < -5) {
    digitalWrite(rightPin, LOW);
    analogMoveNegative(joyX, leftPin);
  }

}

void getButtonState(int bStatus)  {
  switch (bStatus) {
    // -----------------  BUTTON #1  -----------------------
    case 'A':
      
      buttonStatus |= B000001;        // ON
      Serial.println("\n** Button_1: ON **");
      // your code...
     
      displayStatus = "Left";
      Serial.println(displayStatus);
      break;
    case 'B':
      buttonStatus &= B111110;        // OFF
      Serial.println("\n** Button_1: OFF **");
      // your code...
      
      displayStatus = "Straight";
      Serial.println(displayStatus);
      break;

    // -----------------  BUTTON #2  -----------------------
    case 'C':
      buttonStatus |= B000010;        // ON
      Serial.println("\n** Button_2: ON **");
      // your code...
      displayStatus = "Button2 <ON>";
      Serial.println(displayStatus);
      break;
    case 'D':
      buttonStatus &= B111101;        // OFF
      Serial.println("\n** Button_2: OFF **");
      // your code...
      displayStatus = "Button2 <OFF>";
      Serial.println(displayStatus);
      break;

    // -----------------  BUTTON #3  -----------------------
    case 'E':
    
      buttonStatus |= B000100;        // ON
      Serial.println("\n** Button_3: ON **");
      // your code...
     
      displayStatus = "Right";
      Serial.println(displayStatus);
      break;
    case 'F':
      buttonStatus &= B111011;      // OFF
      Serial.println("\n** Button_3: OFF **");
      // your code...
      
      displayStatus = "Straight";
      Serial.println(displayStatus);
      break;

    // -----------------  BUTTON #4  -----------------------
    case 'G':
      buttonStatus |= B001000;       // ON
      Serial.println("\n** Button_4: ON **");
      // your code...
      displayStatus = "Button4 <ON>";
      Serial.println(displayStatus);
      break;
    case 'H':
      buttonStatus &= B110111;    // OFF
      Serial.println("\n** Button_4: OFF **");
      // your code...
      displayStatus = "Button3 <OFF>";
      Serial.println(displayStatus);
      break;

    // -----------------  BUTTON #5  -----------------------
    case 'I':           // configured as momentary button
      //      buttonStatus |= B010000;        // ON
      Serial.println("\n** Button_5: ON ++ **");
      // your code...
      displayStatus = "Button5 <ON>";
      Serial.println(displayStatus);
      break;
    case 'J':
      buttonStatus &= B101111;        // OFF
      Serial.println("\n** Button_5: OFF ++ **");
      // your code...
      displayStatus = "Button5 <OFF>";
      Serial.println(displayStatus);
      break;

    // -----------------  BUTTON #6  -----------------------
    case 'K':
      buttonStatus |= B100000;        // ON
      Serial.println("\n** Button_6: ON **");
      // your code...
      displayStatus = "Button6 <ON>";
      Serial.println(displayStatus);
      break;
    case 'L':
      buttonStatus &= B011111;        // OFF
      Serial.println("\n** Button_6: OFF **");
      // your code...
      displayStatus = "Button6 <OFF>";
      Serial.println(displayStatus);
      break;
  }
  // ---------------------------------------------------------------
}

void hardwareJoystickControl() {

  const int X_pin = A0; // analog pin connected to X output
  const int Y_pin = A1; // analog pin connected to Y output
  const int joyStickNeutralUpperLimit = 535;
  const int joyStickNeutralLowerLimit = 510;
  int X_input = 520;
  int Y_input = 520;
  X_input = analogRead(X_pin);
  Y_input = analogRead(Y_pin);


  if (joyStickNeutralLowerLimit < X_input <  joyStickNeutralUpperLimit) {
    moveCoasting();
  }
  if (joyStickNeutralLowerLimit <  Y_input <  joyStickNeutralUpperLimit) {
    wheelsNeutral();
  }

  if ( X_input >  joyStickNeutralUpperLimit) {
    digitalWrite(reversePin, LOW);
    analogMove(X_input, forwardPin);
  }

  if ( X_input < joyStickNeutralLowerLimit) {
    digitalWrite(forwardPin, LOW);
    analogMoveNegative(X_input, reversePin);
  }

  if ( Y_input >  joyStickNeutralUpperLimit) {
    digitalWrite(leftPin, LOW);
    analogMove(Y_input, rightPin);
  }

  if ( Y_input < joyStickNeutralLowerLimit) {
    digitalWrite(rightPin, LOW);
    analogMoveNegative(Y_input, leftPin);
  }
  delay(1);
}

void analogMove(int inputAcceleration, int movePin) {
  int motorAcceleration = map(inputAcceleration, 5, 100, 10, 255);
  analogWrite(movePin, motorAcceleration);
}

void analogMoveNegative(int inputAcceleration, int movePin) {
  int  motorAcceleration = map(inputAcceleration, -5, -100, 10, 255);
  analogWrite(movePin, motorAcceleration);
}

void bluetoothButtonControl() {
  char incomingChar;
  if (BlueTooth.available() > 0)
  {
    incomingChar = BlueTooth.read();
    Serial.println(incomingChar);
    switch (incomingChar) {
      case 'f':
        moveForward();
        break;
      case 'd':
        wheelsRight();
        break;
      case 's':
        wheelsLeft();
        break;
      case 'r':
        moveReverse();
        break;
      case 'n':
        wheelsNeutral();
        break;
      case 'k':
        moveCoasting();
        break;
    }

    delay(1);        // delay in between reads for stability
  }
  else {
    incomingChar = 0;
  }
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




