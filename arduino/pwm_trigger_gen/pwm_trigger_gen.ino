#define TRIGGER   (8u)
#define ECHO      (7u)
#define SOUND_V   (34300)
#define MAX_RANGE (400)
//max range is 4m, it will take 23.3ms for the max echo
//#define TIMEOUT   (2 * 1000 * MAX_RANGE / 34300)
#define TIMEOUT   (160)//max waiting time for echo is 160ms

void setup() {
  // put your setup code here, to run once:
  //use pin 8 as trigger of ultra sound
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  //using serial plotter
  Serial.begin(9600);
}

void loop() {
  unsigned long duration = 0;
  unsigned long distance = 0;
  // put your main code here, to run repeatedly:
  digitalWrite(TRIGGER, HIGH);

  delayMicroseconds(10); //Generate 10us pulse

  digitalWrite(TRIGGER, LOW);

  duration = pulseIn(ECHO, HIGH, TIMEOUT * 1000);
  distance = ((duration / 2) * SOUND_V) / 1000000;

  delay(10);

  //Serial.print("distance:");
  if(distance > 0 && distance < 400) {
    Serial.println(distance);
  }

}
