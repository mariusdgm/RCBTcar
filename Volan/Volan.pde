import processing.serial.*;
import org.gamecontrolplus.gui.*;
import org.gamecontrolplus.*;
import net.java.games.input.*;

ControlIO control;
ControlDevice volan;
float px, py;
float BTpx, BTpy;
Serial myPort;
int rndPx, rndPy;
boolean firstContact = false;
String val;


public void setup() {
  size(500, 500);
  // Initialise the ControlIO
  control = ControlIO.getInstance(this);
  // Find a device that matches the configuration file
  volan = control.getMatchedDevice("Volan_Pedale");
  if (volan == null) {
    println("No suitable device configured");
    System.exit(-1); // End the program NOW!
  }
  println(Serial.list());
  String portName = Serial.list()[0];
  myPort = new Serial(this, portName, 9600);
  myPort.bufferUntil('\n'); 
  serialEvent(myPort);
}

// Poll for user input called from the draw() method.
public void getUserInput() {
  px = map(volan.getSlider("X_axis").getValue(), -1, 1, 0, width);
  py = map(volan.getSlider("Y_axis").getValue(), -1, 1, 0, height);
}

// Poll for BluetoothControl
public void setBluetoothData() {
  BTpx = map(volan.getSlider("X_axis").getValue(), -1, 1, 0, 200);
  BTpy = map(volan.getSlider("Y_axis").getValue(), -1, 1, 200, 0);
  rndPx = round(BTpx);
  rndPy = round(BTpy);
}


public void draw() {
  getUserInput(); // Polling
  setBluetoothData();
 
    print(rndPx);
    println(" " + rndPy);
    myPort.write('s');
    myPort.write(rndPx);
    myPort.write(rndPy);
  
  delay(1);
  background(255, 255, 240);


  // Show position
  noStroke();
  fill(255, 64, 64, 64);
  ellipse(px, py, 20, 20);
}

void serialEvent( Serial myPort) {
//put the incoming data into a String - 
//the '\n' is our end delimiter indicating the end of a complete packet
val = myPort.readStringUntil('\n');
//make sure our data isn't empty before continuing
if (val != null) {
  //trim whitespace and formatting characters (like carriage return)
  val = trim(val);
  println(val);

  //look for our 'A' string to start the handshake
  //if it's there, clear the buffer, and send a request for data
  if (firstContact == false) {
    if (val.equals("A")) {
      myPort.clear();
      firstContact = true;
      myPort.write("A");
      println("contact");
    }
  }
  else { //if we've already established contact, keep getting and parsing data
    println(val);

    // when you've parsed the data you have, ask for more:
    myPort.write("A");
    }
  }
}