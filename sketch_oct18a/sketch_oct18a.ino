
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
float ERROR_COEF = 1;
int PROP_COEF = 40;
int SENSOR_THRESH = 600;
int ERR = 0;
float DECAY = 0.99;
int COEFS[4] = {4, 2, -2, -4};

// MOTOR CONSTANTS
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor_right = AFMS.getMotor(2);
Adafruit_DCMotor *motor_left = AFMS.getMotor(1);

// SIDES
typedef struct {
  float coef;
  Adafruit_DCMotor *motor;
  uint8_t dir;
} Side;

Side RIGHT = {
  .coef= ERROR_COEF,
  .motor = motor_right,
  .dir = FORWARD,
};
Side LEFT = {
  .coef = ERROR_COEF * -1,
  .motor = motor_left,
  .dir = FORWARD,
};
Side *SIDES[2] = {&RIGHT, &LEFT};

void set_motor(Adafruit_DCMotor *motor, uint8_t mode, int motor_speed=MOTOR_SPEED) {
  motor->run(mode);
  if (mode != RELEASE) {
    Serial.print("Speed set: "); Serial.println(motor_speed);
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

int get_error(uint8_t *measures) {
  int total_error = 0;
  for (int i = 0; i < NUM_SENSORS; i++) {
    total_error += measures[i] * COEFS[i];
  }
  return total_error;
}

// MOTOR SPEED FUNCS
int get_motor_speed(Side *side, uint8_t *measures) {
  int error = get_error(measures);
  ERR = ((int)((float)ERR) * DECAY) + error;
  //Serial.print(measures[0]); Serial.print(measures[1]); Serial.print(measures[2]); Serial.println(measures[3]);
  Serial.print(error); Serial.print(" "); Serial.println(ERR);
  int intergral_adj = ((int)((float)ERR * side->coef));
  int prop_adj = error * PROP_COEF;
  return MOTOR_SPEED + intergral_adj + prop_adj;
}

void set_motor_speeds() {
  uint8_t *measures = get_measures();
  for (int i = 0; i < 2; i++) {
    Side *side = SIDES[i];
    int motor_speed = get_motor_speed(side, measures);
    motor_speed = max(min(motor_speed, 255), -255);
    uint8_t mode;
    if (motor_speed >= 0) {
      mode = FORWARD;
    } else {
      mode = BACKWARD;
    }
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
