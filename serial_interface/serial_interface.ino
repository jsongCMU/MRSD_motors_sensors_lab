/*
 * Commands -> response:
 * ledtoggle -> "test"
 * get_state -> val       # val: get state of FSM
 * get_pot -> val         # val: potentiometer value, 0-1023
 * set_servo,val -> val   # val: servo angle in degrees, 0-180
 * set_dc,val -> val      # val: encoder position, 0 to 1000
 * set_step_speed,val -> val # val: delay for motor in ms
 * get_slot_sensor -> val # val: slot sensor, 1 if open, 0 if blocked
 * get_ir -> val          # val: IR sensor, 0-1023
 * get_us -> val          # val: US sensor, 0-1023
 * get_all_sensors -> val1,val2,val3,val4 # lists potentiometer, slot sensor, IR and US values, in that order
 */

#include <Servo.h>
#include <SharpIR.h>
#include <Encoder.h>

// Global declarations
Servo myservo;
const int LED_pin = 13;
const int pot_pin = A0;
const int servo_pin = 9;
const int US_pin = A2;
int switchPin = 3;    // pushbutton connected to digital pin 3
#define IR_sensor A1
SharpIR sensor( SharpIR::GP2Y0A02YK0F , IR_sensor );
const int stepperDirPin = 5;
const int stepperStepPin = 4;
const int stepsPerRevolution = 200;
int stepperDelay = 10;
int servo_val = 10;

const int slot = A3, encoderA = 2, encoderB = 12;
int slotValue, pos, ultrasonic_sensor, target;
int encoderA_data, encoderB_data;
const int enable = 10, l1 = 7, l2 = 6;
long prevTime = 0, duration, PWMforMotor;
float prevError = 0, integral = 0;
float cm;
const int ultrasonic = A2, MotorIN1 = 7 , MotorIN2 = 8;
Encoder myEnc(encoderA, encoderB);

enum appState_e {
  STATE_0,
  STATE_1,
  STATE_2,
  STATES_NUM
}app_state = STATE_2;


enum appState_e app_state_local = STATES_NUM;

void switch_handler() {
 
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  // If interrupts come faster than 200ms, assume it's a bounce and ignore
  if (interrupt_time - last_interrupt_time > 200)
  {
    app_state = app_state+1;
    app_state = app_state % STATES_NUM;
  }
  last_interrupt_time = interrupt_time;
}

int getIRSensorData()
{
  int distance = sensor.getDistance(); // in cms
  if(distance<19)
    distance = 19;
  else if(distance>70)
    distance = 70;

   return distance;
}

void spinStepperMotor(int customDelay)
{
    digitalWrite(stepperStepPin, HIGH);
    delay(customDelay);
    digitalWrite(stepperStepPin, LOW);
    delay(customDelay);
}

// Function for reading the Potentiometer
int stepperSpeedMap() {
  int customDelay = getIRSensorData(); // Reads the IR sensor
  int newCustom = map(customDelay, 19, 70, 1,100); // Convrests the read values of the ir sensor into desireded delay values (300 to 4000)
  return newCustom;  
}

void setTargetvalue(int val)
{
  target = val;
}

void setMotor(int clockwise, int pwmVal, int pwm, int l1, int l2) {
  analogWrite(pwm, pwmVal);
  if (clockwise == 1) {
    digitalWrite(l1, HIGH);
    digitalWrite(l2, LOW);
  }
  else if (clockwise == -1) {
    digitalWrite(l1, LOW);
    digitalWrite(l2, HIGH);
  }
  else {
    digitalWrite(l1, LOW);
    digitalWrite(l2, LOW);
  }
}
void active_state_functions()
{
  // Drive stepper
 
  spinStepperMotor(stepperDelay);
  // Driver servo
  myservo.write(servo_val);
  pos = myEnc.read();
  float kp = 1;
  float kd = 0;
  float ki = 0;
  long currTime = micros();
  float deltaTime = ((float) (currTime - prevTime)) / ( 1.0e6 );
  prevTime = currTime;
  int error = pos - target;
  float derivative = (error - prevError) / (deltaTime);
  integral = integral + error * deltaTime;
  float control_signal = kp * error + kd * derivative + ki * integral;
  float power = fabs(control_signal);
  if ( power > 255 ) {
    power = 255;
  }
  int clockwise = 1;
  if (control_signal < 0) {
    clockwise = -1;
  }
  prevError = error;
  setMotor(clockwise, power, enable, l1, l2);
}

void setup() {
  pinMode(LED_pin,OUTPUT);
  pinMode(pot_pin,INPUT);
  pinMode(US_pin,INPUT);
  myservo.attach(servo_pin);

  pinMode(stepperDirPin, OUTPUT);
  pinMode(stepperStepPin, OUTPUT);
  // Set stepper motor direction clockwise
  digitalWrite(stepperDirPin, HIGH);
  attachInterrupt(digitalPinToInterrupt(switchPin), switch_handler, RISING);

  pinMode(slot, INPUT);
  pinMode(encoderA, INPUT_PULLUP);
  pinMode(encoderB, INPUT_PULLUP);
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}


void loop() {

  if(app_state_local!=app_state)
  {
    app_state_local = app_state;
//    Serial.print("In State: ");
//    Serial.println(app_state_local);
  }

  switch (app_state)
  {
   case STATE_0:
   {
    // Do nothing
    break;
   }
   case STATE_1:
   {
    // Sensors control motors
    // Get stepper speed from IR sensor
    stepperDelay = stepperSpeedMap();
    // Get servo position from pot
    servo_val = map(analogRead(pot_pin), 0, 1023, 10,180);
    slotValue = analogRead(slot);
    pos = myEnc.read();
    ultrasonic_sensor = analogRead(ultrasonic);
    cm = (ultrasonic_sensor/2)*2.54;
    PWMforMotor = map(cm, 2, 500, 0, 255);
    PWMforMotor = 255;
    if (slotValue > 500) {
    setTargetvalue(PWMforMotor);
    }
    else {
    setTargetvalue(-PWMforMotor);
    }
 
    active_state_functions();
    break;
   }
   case STATE_2:
   {
    // GUI reads and writes to system
    // check if data is available
    active_state_functions();
   
    if (Serial.available() > 0) {
      // read the incoming string:
      String incomingString = Serial.readStringUntil('\n');
      const char* serial_string = incomingString.c_str();
      char* token = strtok(serial_string, ",");
      printf(serial_string);
      if(strcmp(token, "ledtoggle")==0){
        // Test
        digitalWrite(LED_pin,!digitalRead(LED_pin));
      }else if(strcmp(token,"get_state")==0){
        // get state
        Serial.println(app_state);
      }else if(strcmp(token,"get_pot")==0){
        // get potentiometer
        int pot_val = analogRead(pot_pin);
        Serial.println(pot_val);
      }else if(strcmp(token,"set_servo")==0){
        // Set servo
        token = strtok(NULL, ",");
        if(token != NULL){
          servo_val = atoi(token);
          if(servo_val < 10){
            servo_val = 10;
          }else if(servo_val > 180){
            servo_val = 180;
          }
          Serial.println(servo_val);
        }
      }else if(strcmp(token,"set_dc")==0){
        // Set servo
        token = strtok(NULL, ",");
        if(token != NULL){
          int targ = atoi(token);
          setTargetvalue(targ);
          Serial.println(targ);
        }
      }
      else if(strcmp(token,"set_step_speed")==0){
        // Set stepper speed
        token = strtok(NULL, ",");
        if(token != NULL){
          stepperDelay = atoi(token);
          Serial.println(stepperDelay);
        }
      }else if(strcmp(token,"get_slot_sensor")==0){
        slotValue = analogRead(slot);
        Serial.println(slotValue);
      }else if(strcmp(token,"get_ir")==0){
        int ir_val = getIRSensorData();
        Serial.println(ir_val);
      }else if(strcmp(token,"get_us")==0){
        int us_val = analogRead(US_pin);
        Serial.println(us_val);
      }else if(strcmp(token,"get_all_sensors")==0){
        int pot_val = analogRead(pot_pin);
        slotValue = digitalRead(slot);
        int ir_val = getIRSensorData();
        int us_val = analogRead(US_pin);
        Serial.print(pot_val);Serial.print(",");
        Serial.print(slotValue);Serial.print(",");
        Serial.print(ir_val);Serial.print(",");
        Serial.println(us_val);
      }
    }
    break;
   }
   default:
   {
    // Should never reach here
    Serial.println("Poop");
   }
   
  }
}
