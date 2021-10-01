const uint8_t SENSOR = 0;

void setup() {
  Serial.begin(9600);
}

void loop() {
    while (!Serial.available());
    Serial.read();
    Serial.println(analogRead(SENSOR));
}
