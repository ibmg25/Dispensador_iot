//Definicion de pines de entrada del H
int IN1 = 32;
int IN2 = 35;
int ENA = 34;
 
void setup(){
  //Definicion de pines como salida
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  //pinMode(ENA, OUTPUT);
}

void loop(){
  //Gira Motor A sentido horario
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  //analogWrite(ENA, 255);
  delay(10000);
  //Para motor A
  //analogWrite(ENA, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  delay(3000);
}