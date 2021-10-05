
#include <Adafruit_MotorShield.h>

const uint8_t SENSOR_LEFT = 0;
const uint8_t SENSOR_RIGHT = 1;

const uint8_t MOTOR_RIGHT_DIR = FORWARD;
const uint8_t MOTOR_LEFT_DIR = FORWARD;
const uint8_t MOTOR_SPEED = 100;

const int SENSOR_THRESH = 1000;
const int LINE_BUFFER_TIME = 100;

uint8_t current_sensor;
Adafruit_DCMotor *current_motor;

Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor_right = AFMS.getMotor(1);
Adafruit_DCMotor *motor_left = AFMS.getMotor(2);

int get_measure() {
  return analogRead(current_sensor);
}

bool is_line() {
  int measure = get_measure();
  return measure > SENSOR_THRESH;
}

void swap_sensor() {
  current_sensor = (current_sensor == SENSOR_LEFT) ? SENSOR_RIGHT : SENSOR_LEFT;
}

void set_motor(Adafruit_DCMotor *motor, uint8_t mode) {
  motor->run(mode);
  if (mode != RELEASE) {
    motor->setSpeed(MOTOR_SPEED);
  }
}

void swap_motor() {
  if (current_motor == motor_right) {
    set_motor(motor_right, RELEASE);
    set_motor(motor_left, MOTOR_LEFT_DIR);
  }
  else {
    set_motor(motor_right, MOTOR_RIGHT_DIR);
    set_motor(motor_left, RELEASE);
  }
}

void swap_direction() {
  swap_sensor();
  swap_motor();
}

void setup() {
  Serial.begin(9600);
  set_motor(motor_right, MOTOR_RIGHT_DIR);
  set_motor(motor_left, RELEASE);
  current_motor = motor_right;
  current_sensor = SENSOR_LEFT;
}

void loop() {
  if (is_line()) {
    swap_direction();
  }
}
