
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
uint8_t ERROR_COEF = 20;
uint8_t SENSOR_THRESH = 400;
uint8_t CAR_RUNNING = 0; 


// MOTOR CONSTANTS
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor_right = AFMS.getMotor(1);
Adafruit_DCMotor *motor_left = AFMS.getMotor(2);


const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

      // variables to hold the parsed data
char messageFromPC[numChars] = {0};
int integerFromPC = 0;
float floatFromPC = 0.0;
String data; 

boolean newData = false;

uint8_t *consts[4] = {&CAR_RUNNING, &MOTOR_SPEED, &ERROR_COEF, &SENSOR_THRESH}; 


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
  return CAR_RUNNING * (MOTOR_SPEED + prop_adj); 

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

void set_array_constant(char *constant, char *values) {
  
}


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

void setNewVal(uint8_t index, uint8_t new_val){
  Serial.println("ere"); 
  if(index < 4 ){

    *consts[index] = new_val;
    //Not working rn
    Serial.println("Set indivdiual constant"); 
    Serial.println(CAR_RUNNING);
    //If car is to stop, set car running to 0, if car is to run, set car running to 1
    Serial.println(MOTOR_SPEED); 
    Serial.println(ERROR_COEF); 
    Serial.println(SENSOR_THRESH); 
  } else if(index == 4) {
    Serial.println("Setting array constant"); 
    set_array_constant(new_val, NULL); 
  }
}

void send_current_consts(){
  String motorspeed = String(MOTOR_SPEED); 
  Serial.print(motorspeed + "," + motorspeed + "," + motorspeed + "<>");
  Serial.flush();
}

void setup() {
  AFMS.begin(); 
  Serial.begin(115200);
  set_motor(RIGHT.motor, RIGHT.dir);
  set_motor(RIGHT.motor, RIGHT.dir);
}

void loop() {

  set_motor_speeds(); 
//  send_current_consts(); 
  

  recvWithStartEndMarkers();
  if (newData == true) {
    String data = receivedChars;
    int const_index; 
    int new_val; 

    const_index = data.substring(0, 1).toInt(); 
    //assuming first value is only one digit 
    new_val = data.substring(2).toInt(); 
    
    setNewVal(const_index, new_val); 

    newData = false;
  }
}
