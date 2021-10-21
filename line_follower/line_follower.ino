#include <Adafruit_MotorShield.h>

// SENSOR CONSTANTS
const uint8_t SENSOR_OUT_LEFT = 0;
const uint8_t SENSOR_IN_LEFT = 1;
const uint8_t SENSOR_IN_RIGHT = 2;
const uint8_t SENSOR_OUT_RIGHT = 3;
const uint8_t SENSORS[] = {SENSOR_OUT_LEFT, SENSOR_IN_LEFT, SENSOR_IN_RIGHT, SENSOR_OUT_RIGHT};
const uint8_t NUM_SENSORS = 4;

int SENSOR_READINGS[4] = {0, 0, 0, 0};

// MISC CONSTANTS
float DERIV_COEF = 0;
int MOTOR_SPEED = 100;
int TURN_SPEED = 10;
int ERROR_COEF = 20;
int CAR_RUNNING = 0; 
int SENSOR_THRESH = 600;
int TURNING = -1;

Adafruit_DCMotor *turning_motor;

// MOTOR CONSTANTS
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor_right = AFMS.getMotor(2);
Adafruit_DCMotor *motor_left = AFMS.getMotor(1);

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];  // temporary array for use when parsing

// variables to hold the parsed data
char messageFromPC[numChars] = {0};
int integerFromPC = 0;
float floatFromPC = 0.0;
String data; 

boolean newData = false;
boolean collectData = false;

int new_val; 

int *consts[4] = {&CAR_RUNNING, &MOTOR_SPEED, &ERROR_COEF, &SENSOR_THRESH}; 

unsigned long last_time;
unsigned long WAIT_FOR = 100;

// SIDES
typedef struct {
  int coefs[4];
  Adafruit_DCMotor *motor;
  uint8_t dir;
  int last_adj;
  int last_speed;
} Side;

Side RIGHT = {
  .coefs = {1, 1, -1, -1},
  .motor = motor_right,
  .dir = FORWARD,
  .last_adj = 0
  .last_speed = MOTOR_SPEED;
};
Side LEFT = {
  .coefs = {-1, -1, 1, 1},
  .motor = motor_left,
  .dir = FORWARD,
  .last_adj = 0
  .last_speed = MOTOR_SPEED;
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
  SENSOR_READINGS[sensor] = measure;
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
  int deriv_adj = (int)(((float)(prop_adj - side->last_adj)) * DERIV_COEF);
  side->last_adj = prop_adj;
  return (MOTOR_SPEED + prop_adj + deriv_adj) * CAR_RUNNING;
}

void set_motor_speeds() {
  uint8_t *measures = get_measures();
  int *speeds = malloc(sizeof(int) * 2);

  if (TURNING == -1) {
		if (measures[0] == 1 && measures[1] == 1) {
			TURNING = 2;
			SIDES[0]->last_speed = MOTOR_SPEED * CAR_RUNNING;
			SIDES[1]->last_speed = MOTOR_SPEED / 10 * CAR_RUNNING;
			turning_motor = motor_right;
		} else if (measures[2] == 1 && measures[3] == 1) {
			TURNING = 1;
			SIDES[0]->last_speed = MOTOR_SPEED / 10 * CAR_RUNNING;
			SIDES[1]->last_speed = MOTOR_SPEED * CAR_RUNNING;
		}
		motor_right->setSpeed(SIDES[0]->last_speed);
		motor_left->setSpeed(SIDES[1]->last_speed);
  }
  
  if (TURNING != -1) {
    if (measures[TURNING] == 1) {
      TURNING = -1;
    }
  } else {
    for (int i = 0; i < 2; i++) {
      Side *side = SIDES[i];
      int motor_speed = get_motor_speed(side, measures);
      motor_speed = max(min(motor_speed, 255), 0);
      side->motor->setSpeed(motor_speed);
			side->last_speed = motor_speed;
  }

  unsigned long current_time = millis();
  if (collectData && (current_time - last_time) > WAIT_FOR) {
    send_data(SIDES[0]->last_speed, SIDES[1]->last_speed);
    last_time = current_time;
  }

  free(speeds);
  free(measures);
  
}

// MODIFY CONSTANTS FUNCS
void recvWithStartEndMarkers() {
    static boolean recvInProgress = false;
    static byte ndx = 0;
    char startMarker = '<';
    char endMarker = '>';
    char rc;
    while (Serial.available() > 0 && newData == false) {
        rc = Serial.read();
 
        if (recvInProgress == true) {
            if (rc != endMarker) {
                receivedChars[ndx] = rc;
                ndx++;
                if (ndx >= numChars) {
                    ndx = numChars - 1;
                }
            }
            else {
                receivedChars[ndx] = '\0'; // terminate the string
                recvInProgress = false;
                ndx = 0;
                newData = true;
            }
        }
        else if (rc == startMarker) {
            recvInProgress = true;
        }
    }
}

void set_array_constant(char *constant, char *values) {
  
}

void setNewVal(uint8_t index, int new_val){
  if(index < 4 ){
    *consts[index] = new_val;
  } else if(index == 4) { 
    collectData = true; 
  } else if(index == 5) {
    collectData = false;
  }
}

void send_current_consts(int flag){
  Serial.print("{");
  Serial.print(MOTOR_SPEED); Serial.print(","); 
  Serial.print(ERROR_COEF); Serial.print(",");
  Serial.print(SENSOR_THRESH);
  Serial.println("}");
  Serial.flush();
}

void send_data(int rspeed, int lspeed){
  Serial.print("{");
  Serial.print(millis()); Serial.print(","); 
  Serial.print(lspeed); Serial.print(","); Serial.print(rspeed); Serial.print(",");
  Serial.print(SENSOR_READINGS[0]); Serial.print(","); Serial.print(SENSOR_READINGS[1]); Serial.print(",");
  Serial.print(SENSOR_READINGS[2]); Serial.print(","); Serial.print(SENSOR_READINGS[3]);
  Serial.println("}");
  Serial.flush();
}

void setup() {
  AFMS.begin(); 
  Serial.begin(115200);
  set_motor(RIGHT.motor, RIGHT.dir);
  set_motor(LEFT.motor, LEFT.dir);
  send_current_consts(0);
  send_current_consts(0);
  send_current_consts(0);
  last_time = millis();
}

void loop() {
  set_motor_speeds();
  
  recvWithStartEndMarkers();
  
  if (newData) {
    String data = receivedChars;
    int const_index; 
   
    const_index = data.substring(0, 1).toInt(); 
    //assuming first value is only one digit 
    new_val = data.substring(2).toInt(); 

    setNewVal(const_index, new_val); 

    send_current_consts(1);
    newData = false;
  }
}
