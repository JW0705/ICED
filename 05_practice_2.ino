#define PIN_LED 7
unsigned int toggle;

void setup() {
  
  pinMode(PIN_LED, OUTPUT);
  toggle = 0;
  digitalWrite(7, toggle);
  delay(1000);
}

void loop() {
  
  for(int i=0;i<11;i++){
    toggle = toggle_state(toggle);
    digitalWrite(PIN_LED, toggle);
    delay(100);
  }
  while(1){} // infinite loop
}

int toggle_state(int toggle){
  if(toggle == 0) toggle = 1;
  else toggle = 0;
  return toggle;
}
