/*
1 1/2EN
  D9
2 1A
  D8
7 2A
  D7
9 3/4 EM
  D6
15 4A
  D5
10 3A
  D4
*/

#define pin1A (9)//8
#define pin2A (6)//7
#define pin3A (5)//4
#define pin4A (3)//5
#define pin12En (7)//9
#define pin34En (4)//6

#define POWER_PERCENT   (30)//30 minium
#define PWM_DUTY        (255 * POWER_PERCENT / 100)

void Left_Forward(void) {
  digitalWrite(pin12En, LOW);
  analogWrite(pin1A, 0);
  analogWrite(pin2A, PWM_DUTY);
  digitalWrite(pin12En, HIGH);
}

void Left_Backward(void) {
  digitalWrite(pin12En, LOW);
  analogWrite(pin1A, PWM_DUTY);
  analogWrite(pin2A, 0);
  digitalWrite(pin12En, HIGH);
}

void Right_Forward(void) {
  digitalWrite(pin34En, LOW);
  analogWrite(pin3A, 0);
  analogWrite(pin4A, PWM_DUTY);
  digitalWrite(pin34En, HIGH);
}

void Right_Backward(void) {
  digitalWrite(pin34En, LOW);
  analogWrite(pin3A, PWM_DUTY);
  analogWrite(pin4A, 0);
  digitalWrite(pin34En, HIGH);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(pin1A, OUTPUT);
  pinMode(pin2A, OUTPUT);
  pinMode(pin3A, OUTPUT);
  pinMode(pin4A, OUTPUT);
  pinMode(pin12En, OUTPUT);
  pinMode(pin34En, OUTPUT);

  analogWrite(pin1A, 0);
  analogWrite(pin2A, 0);
  analogWrite(pin3A, 0);
  analogWrite(pin4A, 0);

  digitalWrite(pin12En, HIGH);
  digitalWrite(pin34En, HIGH);

  //Left_Forward();
  //Left_Backward();
  //Right_Forward();
  //Right_Backward();
}

void loop() {
  // put your main code here, to run repeatedly:

}
