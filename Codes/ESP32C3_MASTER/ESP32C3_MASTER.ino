#include <Wire.h>
#include <VL53L0X.h>
#define I2C_SLAVE_ADDR 0x08

VL53L0X sensor1;  //Sensor Izquierdo
VL53L0X sensor2;  //sensor Derecho
VL53L0X sensor3;  //Sensor Delantero

const int XSHUT1 = 11;  //Sensor1 Pin XSHUT
const int XSHUT2 = 12;  //Sensor2 Pin XSHUT
const int XSHUT3 = 13;  //Sensor3 Pin XSHUT


//Iniciamos Variables

int servoAngle = 90;   // Grados del servomotor  max -> 145 min -> 35
int motorSpeed = 250;  // Velocidad del motor

void setup() {
  Wire.begin(); // Inicia el maestro I2C
  Serial.begin(115200);

  pinMode(XSHUT1, OUTPUT);
  pinMode(XSHUT2, OUTPUT);
  pinMode(XSHUT3, OUTPUT);

  digitalWrite(XSHUT1, LOW);
  digitalWrite(XSHUT2, LOW);
  digitalWrite(XSHUT3, LOW);
  delay(10);


  // Enciende el primer sensor y configúralo con la dirección 0x30
  digitalWrite(XSHUT1, HIGH);
  delay(10);
  sensor1.setAddress(0x30);
  sensor1.init();
  
  // Enciende el segundo sensor y configúralo con la dirección 0x31
  digitalWrite(XSHUT2, HIGH);
  delay(10);
  sensor2.setAddress(0x31);
  sensor2.init();
  
  // Enciende el tercer sensor y configúralo con la dirección 0x32
  digitalWrite(XSHUT3, HIGH);
  delay(10);
  sensor3.setAddress(0x32);
  sensor3.init();
  
}

void loop() {

  // Lee las distancias de los tres sensores
  sensor1.startContinuous();
  sensor2.startContinuous();
  sensor3.startContinuous();

  int sen1 = sensor1.readRangeContinuousMillimeters();
  int sen2 = sensor2.readRangeContinuousMillimeters();
  int sen3 = sensor3.readRangeContinuousMillimeters();

  Serial.print("Right Sensor: ");
  Serial.print(sen1);
  Serial.print("Middle Sensor: ");
  Serial.print(sen3);
  Serial.print("Left Sensor: ");
  Serial.println(sen2);
  
  String dataToSend = String(ControlAngular(sen1, sen2, sen3)) + "," + String(motorSpeed);
  
  Wire.beginTransmission(I2C_SLAVE_ADDR); // Inicia la transmisión al esclavo
  Wire.write(dataToSend.c_str());         // Envía la cadena de datos al esclavo
  Wire.endTransmission();                 // Finaliza la transmisión

  Serial.println("Datos enviados al esclavo: " + dataToSend);
  
  delay(10); // Espera 1 segundo antes de enviar el siguiente mensaje
}




int ControlAngular(int Sen1, int Sen2, int Sen3){
  
}
