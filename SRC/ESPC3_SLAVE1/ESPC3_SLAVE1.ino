#include <Servo.h>

#include <Wire.h>

Servo direccion;

int servoAngle = 0;
int motorSpeed = 0;

void setup() {
  direccion.attach(5);
  Wire.begin(0x08);              // Configura el esclavo I2C con la dirección 0x08
  Wire.onReceive(receiveEvent);  // Asocia la función de recepción de datos
  Serial.begin(115200);
}

void loop() {
  // Aquí podrías agregar el código para controlar el servomotor y el motor utilizando los valores recibidos
  Serial.print("Ángulo del servomotor: ");
  Serial.println(servoAngle);
  Serial.print("Velocidad del motor: ");
  Serial.println(motorSpeed);
  
  delay(10); // Espera para evitar bloquear el programa

  Motor(motorSpeed);
  direccion.write(servoAngle);
  
}

void receiveEvent(int howMany) {
  String receivedData = "";
  while (Wire.available()) {         // Lee todos los bytes recibidos
    char c = Wire.read();            // Lee un byte
    receivedData += c;               // Agrega el byte a la cadena de datos recibidos
  }

  // Procesa la cadena recibida
  if (receivedData.length() > 0) {
    int delimiterIndex = receivedData.indexOf(','); // Encuentra el delimitador
    
    if (delimiterIndex != -1) {
      servoAngle = receivedData.substring(0, delimiterIndex).toInt(); // Extrae y convierte el ángulo
      motorSpeed = receivedData.substring(delimiterIndex + 1).toInt(); // Extrae y convierte la velocidad
    }
  }
}
