#include <Wire.h>
#include <VL53L0X.h>

// Definir direcciones I2C
#define VL53L0X_ADDRESS 0x29 // Dirección I2C por defecto del VL53L0X
#define ESP32_SLAVE_ADDRESS 0x08 // Dirección I2C del ESP32 esclavo

VL53L0X sensor;

//Pin Declarations
#define buttonSO 0
#define buttonCO 1



//Adjustable Variables    This variables are maked to adjust some parameters of the robot
int TurnDistance = 300; //this variable affect the distance to turn, reading how much nerby to the wall are the robot
int servoAngle = 90;   //This variable is the Servo Position
int motorSpeed = 250;  //This variable is the Motor Velocity}



//Non-Adjustable Variables    this are variables who are used to Adjust things automatically
int wallSen = 0;
int TurnDirection = 0;
int Mode = 0;

void setup() {
  Serial.begin(115200);

  //--------------- PinMode Declarations ---------------//

  pinMode(buttonSO, INPUT_PULLUP);


  Wire.begin(); // Pines SDA y SCL en el ESP32-C3 maestro
  // Comprobar la conexión con el ESP32 esclavo
  Wire.beginTransmission(ESP32_SLAVE_ADDRESS);
  if (Wire.endTransmission() == 0) {
    Serial.println("Conexión con ESP32 esclavo exitosa.");
  } else {
    Serial.println("No se pudo conectar con el ESP32 esclavo.");
  }

  // Inicializar el sensor VL53L0X
  Wire.beginTransmission(VL53L0X_ADDRESS);
  if (Wire.endTransmission() == 0) {
    sensor.setTimeout(500);
    if (!sensor.init(true)) {
      Serial.println("Fallo en la inicialización del sensor VL53L0X");
      while (1);
    }
    sensor.startContinuous();
  } else {
    Serial.println("Sensor VL53L0X no encontrado.");
  }


}

void loop() {
  // Paso 1: Lectura del sensor VL53L0X
  int distance = -1;
  Wire.beginTransmission(VL53L0X_ADDRESS);
  if (Wire.endTransmission() == 0) {
    distance = sensor.readRangeContinuousMillimeters();
    if (!sensor.timeoutOccurred()) {
      Serial.print("Distancia medida: ");
      Serial.print(distance);
      Serial.println(" mm");
    } else {
      Serial.println("Timeout en el sensor VL53L0X");
    }
  }

  // Esperar un momento para evitar choque de datos
  delay(10);
  //
  //  servoAngle = map(distance, 30, 500, 10, 150);
  //  if (servoAngle < 10) servoAngle = 10;
  //  if (servoAngle > 150) servoAngle = 150;
  //

  // Paso 2: Enviar datos al ESP32 esclavo
  String mensaje = String(servoAngle) + "," + String(motorSpeed);
  Wire.beginTransmission(ESP32_SLAVE_ADDRESS);
  Wire.print(mensaje);
  Wire.endTransmission();

  Serial.print("Mensaje enviado al esclavo: "); Serial.println(mensaje);

  // Esperar otro momento antes de volver al sensor
  delay(10); // Esperar 10 milisegundos antes de repetir el ciclo
}


void selLado (int sen1, int sen2, int sen3) { //the order of Sensors are Right, Left, Middle :)
  if ((buttonSO == LOW) && (Mode == 0) && (buttonCO == HIGH)) {
    Mode = 1;
  } else if ((buttonSO == HIGH) && (Mode == 0) && (buttonCO == LOW)) {
    Mode = 2;
  }

  if (Mode == 1) {

    if ((buttonSO == LOW) && (sen1 < 100) && (sen2 > 100)) {
      TurnDirection = 1;
      if (TurnDirection == 1) {
        if ((wallSen == 0) && (sen3 > TurnDistance)) {
          motorSpeed = 250;
          servoAngle = 90;
        } else if ((sen3 < TurnDistance)) {
          wallSen = 1;
          motorSpeed = 250;
          servoAngle = 45;
          delay(3000);
        }
        if ((wallSen == 1) && (sen3 > TurnDistance)) {
          servoAngle = map(sen1, 30, 500, 45, 135);
          if (servoAngle < 45) servoAngle = 45;
          if (servoAngle > 135) servoAngle = 135;
        }
      }
    }
    if ((buttonSO == LOW) && (sen2 < 100) && (sen1 > 100)) {
      TurnDirection = 2;
    }
    if (TurnDirection == 2) {
      if ((wallSen == 0) && (sen3 > TurnDistance)) {
        motorSpeed = 250;
        servoAngle = 90;
      } else if ((sen3 < TurnDistance)) {
        wallSen = 1;
        motorSpeed = 250;
        servoAngle = 135;
        delay(3000);
      }
      if ((wallSen == 1) && (sen3 > TurnDistance)) {
        servoAngle = map(sen2, 30, 500, 135, 45);
        if (servoAngle < 45) servoAngle = 45;
        if (servoAngle > 135) servoAngle = 135;
      }
    }
  }
  if (Mode == 2) {

  }


}
