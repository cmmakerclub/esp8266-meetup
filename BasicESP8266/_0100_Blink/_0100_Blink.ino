const int PIN_LED = 16;

void init_hardware()
{
  Serial.begin(115200);
  
  pinMode(PIN_LED, OUTPUT);
  
  delay(10);
  Serial.println();
  Serial.println();
}

void setup() {
  init_hardware();
}

void loop() {
  digitalWrite( PIN_LED, HIGH );
  delay( 1000 );
  digitalWrite( PIN_LED, LOW );
  delay( 1000 );
}
