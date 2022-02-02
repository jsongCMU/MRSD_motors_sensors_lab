import processing.serial.*;

import processing.serial.*;

import cc.arduino.*;

Arduino arduino;
Serial myPort;
String val;
int rectX, rectY;      // Position of square button
int rectSizeX = 180, rectSizeY = 40;
color rectColor;
color currentColor;
boolean rectOver = false;
boolean mode = true;
int pot;
int pot_pin = 0;
int stepper_pin = 9;
PFont mono;
String[] readings = {"null",  "null", "null", "null"};

ArrayList<TEXTBOX> textboxes = new ArrayList<TEXTBOX>();

void setup() {
  size(1000, 700);
  
  // Prints out the available serial ports.
  println(Arduino.list());
  
  // Modify this line, by changing the "0" to the index of the serial
  // port corresponding to your Arduino board (as it appears in the list
  // printed by the line above).
  String portName = Serial.list()[3];
  myPort = new Serial(this, portName, 9600);
  //arduino = new Arduino(this, Arduino.list()[3], 57600);
  //arduino.pinMode(stepper_pin, Arduino.OUTPUT);
  
  mono = loadFont("AndaleMono-24.vlw");
  textFont(mono);
  currentColor = color(255);
  rectColor = color(150);
  rectX = width/2-rectSizeX/2;
  rectY = 10;
  
  TEXTBOX servoIn = new TEXTBOX(750, 25, 200, 35);
  textboxes.add(servoIn); 
  TEXTBOX stepperIn = new TEXTBOX(750, 125, 200, 35);
  textboxes.add(stepperIn); 
  TEXTBOX DCIn = new TEXTBOX(750, 225, 200, 35);
  textboxes.add(DCIn); 
}

void draw() {
  update(mouseX, mouseY);
  background(currentColor);
  //fill(rectColor);
  //rect(rectX, rectY, rectSizeX, rectSizeY);
  
  for (TEXTBOX t : textboxes) {
      t.DRAW();
  }
  
  /**** SENSORS ****/
  myPort.write("get_all_sensors\n");
  readPort();
  System.out.println(val);
  // potentiometer
  //myPort.write("get_pot\n");
  //readPort();
  text(String.format("Potentiometer: %s", readings[0]), 30, 50);
  fill(0);
  //int pot_map = map(pot, 0, 1023, 0, 180); // convert 10 bit pot reading to degrees
  //arduino.analogWrite(stepper_pin, pot_map);
  
  // slot 
  //myPort.write("get_slot_sensor\n");
  //readPort();
  text(String.format("Slot: %s", readings[1]), 30, 150);
  
  //infrared
  //myPort.write("get_ir\n");
  //readPort();
  text(String.format("Infrared: %s", readings[2]), 30, 250);
  
  // ultrasonic
  //myPort.write("get_us\n");
  //readPort();
  text(String.format("Ultrasonic: %s", readings[3]), 30, 350);
  
  /**** MOTORS ****/
  // servo
  text("Servo Motor:", 500, 50);
  
  // stepper
  text("Stepper Motor:", 500, 150);
  
  // dc
  text("DC Motor:", 500, 250);
  
  delay(100);
}

void readPort() {
  if ( myPort.available() > 0) {
    val = myPort.readStringUntil('\n');
    String[] temp = val.split(",");
    if (temp.length == 4) readings = temp;
  }
}

void update(int x, int y) {
  if ( overRect(rectX, rectY, rectSizeX, rectSizeY) ) {
    rectOver = true;
  } else {
    rectOver = false;
  }
}

boolean overRect(int x, int y, int width, int height)  {
  if (mouseX >= x && mouseX <= x+width && 
      mouseY >= y && mouseY <= y+height) {
    return true;
  } else {
    return false;
  }
}

void mousePressed()
{
  for (TEXTBOX t : textboxes) {
      t.PRESSED(mouseX, mouseY);
   }
   
  if (rectOver) {
    mode = mode ^ rectOver;
    if (!mode) {
      currentColor = color(200,200,0);
      rectColor = color(70);
      rect(rectX, rectY, rectSizeX, rectSizeY);
      
    } else {
      currentColor = color(255);
      rectColor = color(150);
      rect(rectX, rectY, rectSizeX, rectSizeY);
    }
  }
}

void keyPressed() {
  int motor = 0;
  String[] motornames = {"servo","stepper","dc"};
   for (TEXTBOX t : textboxes) {
      if (t.KEYPRESSED(key, (int)keyCode)) {
        //myPort.write("ledtoggle\n");
        myPort.write(String.format("set_%s,%d", motornames[motor], Integer.parseInt(t.Text)));
        //System.out.println(String.format("set_%s,%d", motornames[motor], Integer.parseInt(t.Text)));
      }
      motor++;
   }
}
