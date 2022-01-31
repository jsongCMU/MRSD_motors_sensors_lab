/*
 * Commands -> response:
 * ledtoggle -> "test"
 * get_pot -> val         # val: potentiometer value, 0-1023
 * set_servo,val -> val   # val: servo angle in degrees, 0-180
 * get_slot_sensor -> val        # val: slot sensor, 1 if open, 0 if blocked
 */

#include <Servo.h>
Servo myservo;
const int LED_pin = 13;
const int pot_pin = A0;
const int servo_pin = 9;
const int slot_sensor_pin = A3;

void setup() {
  pinMode(LED_pin,OUTPUT);
  pinMode(pot_pin,INPUT);
  pinMode(slot_sensor_pin,INPUT);
  myservo.attach(servo_pin);
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}

void loop() {
  // check if data is available
  if (Serial.available() > 0) {
    // read the incoming string:
    String incomingString = Serial.readStringUntil('\n');
    const char* serial_string = incomingString.c_str();
    char* token = strtok(serial_string, ",");
    if(strcmp(token, "ledtoggle")==0){
      // Test
      digitalWrite(LED_pin,!digitalRead(LED_pin));
      Serial.println("test");
    }else if(strcmp(token,"get_pot")==0){
      // get potentiometer
      int pot_val = analogRead(pot_pin);
      Serial.println(pot_val);
    }else if(strcmp(token,"set_servo")==0){
      // Set servo
      token = strtok(NULL, ",");
      if(token != NULL){
        int servo_val = atoi(token);
        myservo.write(servo_val);
        Serial.println(servo_val);
      }
    }else if(strcmp(token,"get_slot_sensor")==0){
      int slot_sensor_val = digitalRead(slot_sensor_pin);
      Serial.println(slot_sensor_val);
    }
  }
}
