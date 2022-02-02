/*
 * Commands -> response:
 * ledtoggle -> "test"
 * get_pot -> val         # val: potentiometer value, 0-1023
 * set_servo,val -> val   # val: servo angle in degrees, 0-180
 * get_slot_sensor -> val # val: slot sensor, 1 if open, 0 if blocked
 * get_ir -> val          # val: IR sensor, 0-1023
 * get_us -> val          # val: US sensor, 0-1023
 * get_all_sensors -> val1,val2,val3,val4 # lists potentiometer, slot sensor, IR and US values, in that order
 */

#include <Servo.h>

Servo myservo;
const int LED_pin = 13;
const int pot_pin = A0;
const int servo_pin = 9;
const int slot_sensor_pin = A3;
const int IR_pin = A1;
const int US_pin = A2;

void setup() {
  pinMode(LED_pin,OUTPUT);
  pinMode(pot_pin,INPUT);
  pinMode(slot_sensor_pin,INPUT);
  pinMode(IR_pin,INPUT);
  pinMode(US_pin,INPUT);
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
    printf(serial_string);
    if(strcmp(token, "ledtoggle")==0){
      // Test
      digitalWrite(LED_pin,!digitalRead(LED_pin));
    }else if(strcmp(token,"get_pot")==0){
      // get potentiometer
      int pot_val = analogRead(pot_pin);
    }else if(strcmp(token,"set_servo")==0){
      // Set servo
      token = strtok(NULL, ",");
      if(token != NULL){
        int servo_val = atoi(token);
        myservo.write(servo_val);
      }
    }else if(strcmp(token,"get_slot_sensor")==0){
      int slot_sensor_val = digitalRead(slot_sensor_pin);
      Serial.println(slot_sensor_val);
    }else if(strcmp(token,"get_ir")==0){
      int ir_val = analogRead(IR_pin);
      Serial.println(ir_val);
    }else if(strcmp(token,"get_us")==0){
      int us_val = analogRead(US_pin);
      Serial.println(us_val);
    }else if(strcmp(token,"get_all_sensors")==0){
      int pot_val = analogRead(pot_pin);
      int slot_sensor_val = digitalRead(slot_sensor_pin);
      int ir_val = analogRead(IR_pin);
      int us_val = analogRead(US_pin);
      Serial.print(pot_val);Serial.print(",");
      Serial.print(slot_sensor_val);Serial.print(",");
      Serial.print(ir_val);Serial.print(",");
      Serial.println(us_val);
    }
  }
}
