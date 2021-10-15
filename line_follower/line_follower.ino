
#include <Adafruit_MotorShield.h>

// SENSOR CONSTANTS
const uint8_t SENSOR_OUT_LEFT = 0;
const uint8_t SENSOR_IN_LEFT = 1;
const uint8_t SENSOR_OUT_RIGHT = 2;
const uint8_t SENSOR_IN_RIGHT = 3;
const uint8_t SENSORS[] = {SENSOR_OUT_LEFT, SENSOR_IN_LEFT, SENSOR_IN_RIGHT, SENSOR_OUT_RIGHT};
const uint8_t NUM_SENSORS = 4;

// MISC CONSTANTS
uint8_t MOTOR_SPEED = 100;
int ERROR_COEF = 20;
int SENSOR_THRESH = 400;

// MOTOR CONSTANTS
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor_right = AFMS.getMotor(1);
Adafruit_DCMotor *motor_left = AFMS.getMotor(2);

// SIDES
typedef struct {
  int coefs[4];
  Adafruit_DCMotor *motor;
  uint8_t dir;
} Side;

Side RIGHT = {
  .coefs = {3, 1, -1, -3},
  .motor = motor_right,
  .dir = FORWARD
};
Side LEFT = {
  .coefs = {-3, -1, 1, 3},
  .motor = motor_right,
  .dir = FORWARD
};
Side SIDES[2] = {RIGHT, LEFT};

void set_motor(Adafruit_DCMotor *motor, uint8_t mode, int motor_speed=MOTOR_SPEED) {
  motor->run(mode);
  if (mode != RELEASE) {
    motor->setSpeed(motor_speed);
  }
}

// MEASURE FUNCS
int get_measure(uint8_t sensor) {
  return analogRead(sensor);
}

bool is_line(uint8_t sensor) {
  int measure = get_measure(sensor);
  return measure < SENSOR_THRESH;
}

int * get_measures() {
  int measures[NUM_SENSORS];
  for (int i = 0; i < NUM_SENSORS; i++) {
    measures[i] = is_line(SENSORS[i]);
  }
  return measures;
}

int get_error(Side side, int measures[4]) {
  int total_error = 0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    total_error += measures[i] * side.coefs[i];
  }
  return total_error;
}

// MOTOR SPEED FUNCS
int get_motor_speed(Side side, int measures[4]) {
  int error = get_error(side, measures);
  int prop_adj = error * ERROR_COEF;
  return MOTOR_SPEED + prop_adj;
}

void set_motor_speeds() {
  int *measures = get_measures();
  for (int i = 0; i < 2; i++) {
    Side side = SIDES[i];
    int motor_speed = get_motor_speed(side, measures);
    motor_speed = max(min(motor_speed, 0), 255);
    set_motor(side.motor, side.dir, motor_speed);
  }
}

// MODIFY CONSTANTS FUNCS
void set_single_constant(char *constant, int value) {
}

void set_array_constant(char *constant, char *values) {
  
}

void setup() {
  Serial.begin(9600);
  set_motor(RIGHT.motor, RIGHT.dir);
  set_motor(RIGHT.motor, RIGHT.dir);
}

void loop() {
  set_motor_speeds();
}
