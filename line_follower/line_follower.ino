
#include <Adafruit_MotorShield.h>

// SENSOR CONSTANTS
const uint8_t SENSOR_OUT_LEFT = 0;
const uint8_t SENSOR_IN_LEFT = 1;
const uint8_t SENSOR_IN_RIGHT = 2;
const uint8_t SENSOR_OUT_RIGHT = 3;
const uint8_t SENSORS[] = {SENSOR_OUT_LEFT, SENSOR_IN_LEFT, SENSOR_IN_RIGHT, SENSOR_OUT_RIGHT};
const uint8_t NUM_SENSORS = 4;

// MISC CONSTANTS
uint8_t MOTOR_SPEED = 75;
int ERROR_COEF = 40;
float DERIV_COEF = 0.5;
int SENSOR_THRESH = 500;

// MOTOR CONSTANTS
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor_right = AFMS.getMotor(2);
Adafruit_DCMotor *motor_left = AFMS.getMotor(1);

// SIDES
typedef struct {
  int coefs[4];
  Adafruit_DCMotor *motor;
  uint8_t dir;
  int last_speed;
} Side;

Side RIGHT = {
  .coefs = {4, 1, -1, -4},
  .motor = motor_right,
  .dir = FORWARD,
  .last_speed = 0
};
Side LEFT = {
  .coefs = {-4, -1, 1, 4},
  .motor = motor_left,
  .dir = FORWARD,
  .last_speed = 0
};
Side *SIDES[2] = {&RIGHT, &LEFT};

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

uint8_t is_line(uint8_t sensor) {
  int measure = get_measure(sensor);
  return (measure > SENSOR_THRESH);
}

uint8_t * get_measures() {
  uint8_t *measures = malloc(sizeof(uint8_t) * NUM_SENSORS);
  for (int i = 0; i < NUM_SENSORS; i++) {
    measures[i] = is_line(SENSORS[i]);
  }
  return measures;
}

int get_error(Side side, uint8_t *measures) {
  int total_error = 0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    total_error += measures[i] * side.coefs[i];
  }
  return total_error;
}

// MOTOR SPEED FUNCS
int get_motor_speed(Side *side, uint8_t *measures) {
  int error = get_error(*side, measures);
  int prop_adj = error * ERROR_COEF;
  int deriv_adj = (int)(((float)(prop_adj - side->last_speed)) * DERIV_COEF);
  side->last_speed = prop_adj;
  return MOTOR_SPEED + prop_adj + deriv_adj;
}

void set_motor_speeds() {
  uint8_t *measures = get_measures();
  Serial.print(measures[0]); Serial.print(measures[1]); Serial.print(measures[2]); Serial.println(measures[3]);
  for (int i = 0; i < 2; i++) {
    Side *side = SIDES[i];
    int motor_speed = get_motor_speed(side, measures);
    motor_speed = max(min(motor_speed, 255), 0);
    //set_motor(side.motor, side.dir, motor_speed);
    side->motor->setSpeed(motor_speed);
  }
  free(measures);
}

void setup() {
  Serial.begin(9600);
  AFMS.begin(); 
  set_motor(RIGHT.motor, RIGHT.dir);
  set_motor(LEFT.motor, LEFT.dir);
}

void loop() {
  set_motor_speeds();
}