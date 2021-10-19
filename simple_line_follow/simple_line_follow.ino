
#include <Adafruit_MotorShield.h>

const uint8_t SENSOR_LEFT = 0;
const uint8_t SENSOR_RIGHT = 3;

const uint8_t MOTOR_RIGHT_DIR = FORWARD;
const uint8_t MOTOR_LEFT_DIR = FORWARD;
const uint8_t MOTOR_SPEED = 100;

const int SENSOR_THRESH = 600;

uint8_t current_sensor;
Adafruit_DCMotor *current_motor;

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor_right = AFMS.getMotor(2);
Adafruit_DCMotor *motor_left = AFMS.getMotor(1);

int get_measure() {
  Serial.print(current_sensor); Serial.print(": "); Serial.println(analogRead(current_sensor));
  return analogRead(current_sensor);
}

bool is_line() {
  int measure = get_measure();
  return measure >   SENSOR_THRESH;
}

void swap_sensor() {
  current_sensor = (current_sensor == SENSOR_LEFT) ? SENSOR_RIGHT : SENSOR_LEFT;
}

void set_motor(Adafruit_DCMotor *motor, uint8_t mode) {
  //Serial.println("Setting mode...");
  motor->run(mode);
  Serial.print("Mode set to: "); Serial.println(mode);
  if (mode != RELEASE) {
    //Serial.println("Setting speed...");
    motor->setSpeed(MOTOR_SPEED);
    Serial.print("Speed set to: "); Serial.println(MOTOR_SPEED);
  }
}

void swap_motor() {
  if (current_sensor == SENSOR_RIGHT) {
    set_motor(motor_right, RELEASE);
    set_motor(motor_left, MOTOR_LEFT_DIR);
  }
  else {
    set_motor(motor_right, MOTOR_RIGHT_DIR);
    set_motor(motor_left, RELEASE);
  }
}

void swap_direction() {
  swap_motor();
  swap_sensor();
}

void setup() {
  Serial.begin(115200);
  AFMS.begin(); 
  set_motor(motor_right, RELEASE);
  set_motor(motor_left, MOTOR_LEFT_DIR);
  current_motor = motor_left;
  current_sensor = SENSOR_LEFT;
}

void loop() {
  if (is_line()) { 
    swap_direction();
  }
}
