/*
 * Commands -> response:
 * ledtoggle -> "test"
 * get_pot -> val
 * set_servo,val -> val
 */

#include <Servo.h>
Servo myservo;
const int LED_pin = 13;
const int pot_pin = A0;
const int servo_pin = 9;

void setup() {
  pinMode(13,OUTPUT);
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
    }
  }
}
