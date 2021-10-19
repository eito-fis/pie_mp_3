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
int ERROR_COEF = 20;
int CAR_RUNNING = 0; 
int SENSOR_THRESH = 600;

// MOTOR CONSTANTS
Adafruit_MotorShield AFMS = Adafruit_MotorShield();
Adafruit_DCMotor *motor_right = AFMS.getMotor(2);
Adafruit_DCMotor *motor_left = AFMS.getMotor(1);


const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

      // variables to hold the parsed data
char messageFromPC[numChars] = {0};
int integerFromPC = 0;
float floatFromPC = 0.0;
String data; 

boolean newData = false;
boolean collectData = false;

int new_val; 

int *consts[4] = {&CAR_RUNNING, &MOTOR_SPEED, &ERROR_COEF, &SENSOR_THRESH}; 


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
  int deriv_adj = (int)(((float)(prop_adj - side->last_speed)) * DERIV_COEF);
  side->last_speed = prop_adj;
  return (MOTOR_SPEED + prop_adj + deriv_adj) * CAR_RUNNING;
}

void set_motor_speeds() {
  uint8_t *measures = get_measures();
  //Serial.print(measures[0]); Serial.print(measures[1]); Serial.print(measures[2]); Serial.println(measures[3]);
  for (int i = 0; i < 2; i++) {
    Side *side = SIDES[i];
    int motor_speed = get_motor_speed(side, measures);
    motor_speed = max(min(motor_speed, 255), 0);
    //set_motor(side.motor, side.dir, motor_speed);
    side->motor->setSpeed(motor_speed);
  }
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
    //Not working rn
//    Serial.println("Set indivdiual constant"); 
//    Serial.println(CAR_RUNNING);
//    //If car is to stop, set car running to 0, if car is to run, set car running to 1
//    Serial.println(MOTOR_SPEED); 
//    Serial.println(ERROR_COEF); 
//    Serial.println(SENSOR_THRESH); 
  } else if(index == 4) { 
    collectData = true; 
  } else if(index == 5) {
    collectData = false;
  }
}

void send_current_consts(){
  String motorspeed = String(MOTOR_SPEED); 
  String errorcoef = String(ERROR_COEF); 
  String sensorthresh = String(SENSOR_THRESH); 
  Serial.println("{" + motorspeed + "," + errorcoef + "," + sensorthresh + "}");
  Serial.flush();
}

void send_data(){
  Serial.println("{data, data, data}"); 
  Serial.flush();
  
}

void setup() {
  AFMS.begin(); 
  Serial.begin(115200);
  set_motor(RIGHT.motor, RIGHT.dir);
  set_motor(LEFT.motor, LEFT.dir);
}

void loop() {
  set_motor_speeds();
  
  
  if (collectData){
    send_data();
  
  } else {
    send_current_consts(); 
    //Serial.print(new_val);

  }
  
  recvWithStartEndMarkers();
  
  if (newData) {
    String data = receivedChars;
    int const_index; 
   

    const_index = data.substring(0, 1).toInt(); 
    //assuming first value is only one digit 
    new_val = data.substring(2).toInt(); 

    setNewVal(const_index, new_val); 

    newData = false;
  }
}
