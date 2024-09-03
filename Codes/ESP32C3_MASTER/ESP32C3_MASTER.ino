#include <Wire.h>

#define I2C_SLAVE_ADDR 0x08

int servoAngle = 90;   // Grados del servomotor
int motorSpeed = 150;  // Velocidad del motor

void setup() {
  Wire.begin(); // Inicia el maestro I2C
  Serial.begin(115200);
}

void loop() {
  String dataToSend = String(servoAngle) + "," + String(motorSpeed);
  
  Wire.beginTransmission(I2C_SLAVE_ADDR); // Inicia la transmisión al esclavo
  Wire.write(dataToSend.c_str());         // Envía la cadena de datos al esclavo
  Wire.endTransmission();                 // Finaliza la transmisión

  Serial.println("Datos enviados al esclavo: " + dataToSend);
  
  delay(1000); // Espera 1 segundo antes de enviar el siguiente mensaje
}
