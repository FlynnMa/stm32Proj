/******************************************************************************************
* Test Sketch for Razor AHRS v1.4.2
* 9 Degree of Measurement Attitude and Heading Reference System
* for Sparkfun "9DOF Razor IMU" and "9DOF Sensor Stick"
*
* Released under GNU GPL (General Public License) v3.0
* Copyright (C) 2013 Peter Bartz [http://ptrbrtz.net]
* Copyright (C) 2011-2012 Quality & Usability Lab, Deutsche Telekom Laboratories, TU Berlin
* Written by Peter Bartz (peter-bartz@gmx.de)
*
* Infos, updates, bug reports, contributions and feedback:
*     https://github.com/ptrbrtz/razor-9dof-ahrs
******************************************************************************************/

/*
  NOTE: There seems to be a bug with the serial library in Processing versions 1.5
  and 1.5.1: "WARNING: RXTX Version mismatch ...".
  Processing 2.0.x seems to work just fine. Later versions may too.
  Alternatively, the older version 1.2.1 also works and is still available on the web.
*/

import processing.opengl.*;
import processing.serial.*;

// IF THE SKETCH CRASHES OR HANGS ON STARTUP, MAKE SURE YOU ARE USING THE RIGHT SERIAL PORT:
// 1. Have a look at the Processing console output of this sketch.
// 2. Look for the serial port list and find the port you need (it's the same as in Arduino).
// 3. Set your port number here:
final static int SERIAL_PORT_NUM = 2;
// 4. Try again.

//PrintWriter dataLog;

boolean synched = false;

//final static int SERIAL_PORT_BAUD_RATE = 115200;
final static int SERIAL_PORT_BAUD_RATE = 460800;

int[] raw = new int[3];

PFont font;
Serial serial;

float[] ypr;
ArrayList<Axis> accData;

// Skip incoming serial stream data until token is found
boolean readToken(Serial serial, String token) {
  /*
  // Wait until enough bytes are available
  if (serial.available() < token.length())
    return false;
  
  String str = serial.readStringUntil('\n');
  if (null == str)
  {
    return false;
  }
  println("Get reply:" + str + "expect:"+token);
  // Check if incoming bytes match token
  for (int i = 0; i < token.length(); i++) {
    if (str.charAt(i) != token.charAt(i))
      return false;
  }
  
  println("sync success!");*/
  return true;
}


// Global setup
void setup() {
  // Setup graphics
  size(640, 480);
  smooth();
  frameRate(400);
  
  accData = new ArrayList<Axis>();
  
  // Setup serial port I/O
  println("AVAILABLE SERIAL PORTS:");
  println(Serial.list());
  String portName = Serial.list()[SERIAL_PORT_NUM];
  println();
  println("HAVE A LOOK AT THE LIST ABOVE AND SET THE RIGHT SERIAL PORT NUMBER IN THE CODE!");
  println("  -> Using port " + SERIAL_PORT_NUM + ": " + portName);
  serial = new Serial(this, portName, SERIAL_PORT_BAUD_RATE);
  
//  dataLog = createWriter("dataLog.txt"); 

}

void setupRazor() {
  println("Trying to setup and synch FlyDragon...");
  
  // On Mac OSX and Linux (Windows too?) the board will do a reset when we connect, which is really bad.
  // See "Automatic (Software) Reset" on http://www.arduino.cc/en/Main/ArduinoBoardProMini
  // So we have to wait until the bootloader is finished and the Razor firmware can receive commands.
  // To prevent this, disconnect/cut/unplug the DTR line going to the board. This also has the advantage,
  // that the angles you receive are stable right from the beginning. 
//  delay(1000);  // 3 seconds should be enough
  
  // Set Razor output parameters
  serial.write("binary off\r\n");  // set as binary mode
  serial.clear();  // Clear input buffer up to here
  serial.write("acclog off\r\n");  // Turn on continuous streaming output
  delay(10);
  serial.clear();  // Clear input buffer up to here
  serial.write("accon off\r\n");  // Turn on continuous streaming output
  delay(10);
  serial.clear();  // Clear input buffer up to here
  serial.write("accon on\r\n");  // Turn on continuous streaming output
  delay(10);
  serial.write("binary on\r\n");  // set as binary mode
  delay(10);
  println("enable acc...");
  serial.write("acclog on\r\n");  // Turn on continuous streaming output
  delay(100);
  println("enable acclog...");

  serial.clear();  // Clear input buffer up to here
  // Synch with Razor
//  serial.write("#sync 00\r");  // Request synch token
}

float readFloat(Serial s) {
  // Convert from little endian (Razor) to big endian (Java) and interpret as float
  return Float.intBitsToFloat(s.read() + (s.read() << 8) + (s.read() << 16) + (s.read() << 24));
}

boolean updateACC(Serial s)
{
  if (s.available() <= 0)
    return false;
  
  String str = s.readStringUntil('\n');
  if (null == str)
  {
    return false;
  }

  if (str.length() < 10)
    return false;

//  println("recv:" + str);
  String[] m = match(str, "#ACC=(.*?)");
  if (null == m)
  {
    return false;
  }
  
//  dataLog.println(str);
//  dataLog.flush();
  m = split(str, '=');
  raw = int(split(m[1], ','));
//  println(m[1]);
//  println("raw x:" + raw[0] + " y:" + raw[1] + " z:" + raw[2]);
  return true;
}

boolean updateBinary(Serial s)
{
 if (s.available() <= 0)
    return false;
  
  byte head = byte(s.read());
//  println("head is : ", hex(head));
  byte type = byte(head & 0x0f);
  head &= 0xf0;
  if (byte(0xa0) != head)
  {
//    s.clear();
    println("unknown head " + hex(head));
    return false;
  }
  if (type != byte(0x01))
  {
//    s.clear();
    println("unknown type %x" + type);
    return false;
  }

  byte len = byte(s.read());
  if (len != 12)
  {
    println("length error : " + len);
    return false;
  }
  raw[0] = s.read() + (s.read() << 8) + (s.read() << 16) + (s.read() << 24);
  raw[1] = s.read() + (s.read() << 8) + (s.read() << 16) + (s.read() << 24);
  raw[2] = s.read() + (s.read() << 8) + (s.read() << 16) + (s.read() << 24);
//  println("raw x:" + raw[0] + " y:" + raw[1] + " z:" + raw[2]);
  
  int tailMagic = s.read();
  if (tailMagic != 0xed)
  {
    println("tailMagic error: " + tailMagic);
    return false;
  }
  return true;
}

void draw() {
   // Reset scene
  background(0);
//  lights();


  // Sync with Razor 
  if (!synched) {
    textAlign(CENTER);
    fill(255);
    text("Connecting to FlyDragon...", width/2, height/2, -200);
    
    if (frameCount == 2)
      setupRazor();  // Set ouput params and request synch token
    else if (frameCount > 2)
      synched = true;//readToken(serial, "#SYNCH 00");  // Look for synch token
    return;
  }
 
  // Read angles from serial port
  /*
  while (serial.available() >= 12) {
    yaw = readFloat(serial);
    pitch = readFloat(serial);
    roll = readFloat(serial);
  }*/
//  while(false == updateACC(serial));
  while(false == updateBinary(serial));
  Axis data = new Axis();

  data.rawX = raw[0];
  data.rawY = raw[1];
  data.rawZ = raw[2];
  accData.add(data);
  while (accData.size() > width - 20)
  {
    accData.remove(0);
  }
  println("buffered:" + accData.size() + " width :" + width);
  
  Axis accCurr;
  Axis accOld;
  accOld = accData.get(0);
  for (int i = 1; i < accData.size(); i++)
  {
    accCurr = accData.get(i);
    stroke(255);
    line(i - 1, (accOld.rawX * 200) / 1024 + height/2, i, (accCurr.rawX * 200) / 1024 + height/2);
    stroke(255,0,0);
    line(i - 1, (accOld.rawY * 200)/ 1024 + height/2, i, (accCurr.rawY * 200) / 1024 + height/2);
    stroke(255,0,255);
    line(i - 1, (accOld.rawZ * 200)/ 1024 + height/2, i, (accCurr.rawZ * 200) / 1024 + height/2);
    accOld = accCurr;
  }

  
}

void keyPressed() {
  switch (key) {
    case '0':  // Turn Razor's continuous output stream off
      serial.write("accoff\r");
      break;
    case '1':  // Turn Razor's continuous output stream on
      println("enable acc");
      serial.write("accon on\r\n");
      serial.write("acclog on\r\n");
      break;
    case 'f':  // Request one single yaw/pitch/roll frame from Razor (use when continuous streaming is off)
      serial.write("#f\r");
      break;
    case 'a':  // Align screen with Razor
  }
}



