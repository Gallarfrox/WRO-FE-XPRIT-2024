#include <Wire.h>
#include <VL53L0X.h>

// Definir direcciones I2C
#define VL53L0X_DEFAULT_ADDRESS 0x29 // Dirección I2C por defecto del VL53L0X
#define ESP32_SLAVE_ADDRESS 0x08     // Dirección I2C del ESP32 esclavo

#define SENSOR1_NEW_ADDRESS 0x30
#define SENSOR2_NEW_ADDRESS 0x31
#define SENSOR3_NEW_ADDRESS 0x32

VL53L0X sensor1;
VL53L0X sensor2;
VL53L0X sensor3;

//Pin Declarations
#define buttonSO 20
#define buttonCO 21

const int XSHUT1 = 4;  // Sensor1 Pin XSHUT
const int XSHUT2 = 3;  // Sensor2 Pin XSHUT
const int XSHUT3 = 2;  // Sensor3 Pin XSHUT

// Adjustable Variables
int interWall = 3200;       //Modificate the time between corners 
int TurnDistance = 380;     //This Variable regulate what distance between front wall and robot you need to Turn
int TurnTime = 1000;        //This variable regulate how much time the robot Turn

// Non-Adjustable Variables
int servoAngle = 90;
int motorSpeed = 0;
int wallSen = 0;
int TurnDirection = 0;
int Mode = 0;
long int wallTime = 0;
int wallCounter = 0;
long int turningTime = 0;

void setup() {
  Serial.begin(115200);

  // PinMode Declarations

  pinMode(buttonSO, INPUT_PULLUP);
  pinMode(buttonCO, INPUT_PULLUP);
  pinMode(XSHUT1, OUTPUT);
  pinMode(XSHUT2, OUTPUT);
  pinMode(XSHUT3, OUTPUT);

  // Inicializar los pines XSHUT
  digitalWrite(XSHUT1, LOW);
  digitalWrite(XSHUT2, LOW);
  digitalWrite(XSHUT3, LOW);
  delay(10);

  // Inicializar I2C
  Wire.begin();

  // Inicializar los sensores VL53L0X

  // Sensor 1
  digitalWrite(XSHUT1, HIGH);
  delay(10);
  sensor1.init(true);
  sensor1.setAddress(SENSOR1_NEW_ADDRESS);
  sensor1.startContinuous();

  // Sensor 2
  digitalWrite(XSHUT2, HIGH);
  delay(10);
  sensor2.init(true);
  sensor2.setAddress(SENSOR2_NEW_ADDRESS);
  sensor2.startContinuous();

  // Sensor 3
  digitalWrite(XSHUT3, HIGH);
  delay(10);
  sensor3.init(true);
  sensor3.setAddress(SENSOR3_NEW_ADDRESS);
  sensor3.startContinuous();

  // Comprobar la conexión con el ESP32 esclavo
  Wire.beginTransmission(ESP32_SLAVE_ADDRESS);
  if (Wire.endTransmission() == 0) {
    Serial.println("Conexión con ESP32 esclavo exitosa.");
  } else {
    Serial.println("No se pudo conectar con el ESP32 esclavo.");
  }
}

void loop() {
  // Leer las distancias de los sensores VL53L0X
  int distance1 = sensor1.readRangeContinuousMillimeters();
  int distance2 = sensor2.readRangeContinuousMillimeters();
  int distance3 = sensor3.readRangeContinuousMillimeters();

  // Verificar si ocurrió un timeout
  if (sensor1.timeoutOccurred()) Serial.println("Timeout en el sensor 1");
  if (sensor2.timeoutOccurred()) Serial.println("Timeout en el sensor 2");
  if (sensor3.timeoutOccurred()) Serial.println("Timeout en el sensor 3");

  // Mostrar las distancias medidas
  Serial.print("Distancia Sensor 1: ");
  Serial.print(distance1);
  Serial.println(" mm");

  Serial.print("Distancia Sensor 2: ");
  Serial.print(distance2);
  Serial.println(" mm");

  Serial.print("Distancia Sensor 3: ");
  Serial.print(distance3);
  Serial.println(" mm");
  selLado(distance1, distance3, distance2);
  Serial.print(Mode);
  Serial.print("  ");
  Serial.print(TurnDirection);
  // Paso 2: Enviar datos al ESP32 esclavo
  String mensaje = String(servoAngle) + "," + String(motorSpeed);
  Wire.beginTransmission(ESP32_SLAVE_ADDRESS);
  Wire.print(mensaje);
  Wire.endTransmission();

  Serial.print("Mensaje enviado al esclavo: ");
  Serial.println(mensaje);

  // Esperar un momento antes de repetir el ciclo
  delay(10);
}

void selLado (int sen1, int sen2, int sen3) { //the order of Sensors are Right, Left, Middle :)
  if ((digitalRead(buttonSO) == LOW) && (Mode == 0) && (digitalRead(buttonCO) == HIGH)) {
    Mode = 1;
  } else if ((digitalRead(buttonSO) == HIGH) && (Mode == 0) && (digitalRead(buttonCO) == LOW)) {
    Mode = 2;
  }

  if (Mode == 1) {

    if ((digitalRead(buttonSO) == LOW) && (sen1 < 100) && (sen2 > 100)) {
      TurnDirection = 1;
    }
    if (TurnDirection == 1) {
      if ((wallSen == 0) && (sen3 > TurnDistance)) {
        motorSpeed = 250;
        servoAngle = 90;
      } else if ((sen3 < TurnDistance)) {
        wallSen = 1;
        motorSpeed = 250;
        turningTime = millis() + TurnTime;
        while (turningTime > millis()){
           servoAngle = 45;
           String mensaje = String(servoAngle) + "," + String(motorSpeed);
  Wire.beginTransmission(ESP32_SLAVE_ADDRESS);
  Wire.print(mensaje);
  Wire.endTransmission();
        }
      }
      if ((wallSen == 1) && (sen3 > TurnDistance)) {
        servoAngle = map(sen1, 50, 350, 65, 115);
        if (servoAngle < 55) servoAngle = 55;
        if (servoAngle > 125) servoAngle = 125;
      }
    }

    if ((digitalRead(buttonSO) == LOW) && (sen2 < 100) && (sen1 > 100)) {
      TurnDirection = 2;
    }
    if (TurnDirection == 2) {
      if ((wallSen == 0) && (sen3 > TurnDistance)) {
        motorSpeed = 255;
        servoAngle = 90;
      } else if ((sen3 < TurnDistance)) {
        wallSen = 1;
        motorSpeed = 255;
        turningTime = millis() + TurnTime;
        while (turningTime > millis()){
           servoAngle = 135;
           String mensaje = String(servoAngle) + "," + String(motorSpeed);
  Wire.beginTransmission(ESP32_SLAVE_ADDRESS);
  Wire.print(mensaje);
  Wire.endTransmission();
        }
      }
      if ((wallSen == 1) && (sen3 > TurnDistance)) {
        servoAngle = map(sen2, 50, 350, 115, 65);
        if (servoAngle < 55) servoAngle = 55;
        if (servoAngle > 125) servoAngle = 125;
      }
    }

    if ((sen3 < TurnDistance) && (wallTime < millis())) {
      wallCounter += 1;
      wallTime = millis() + interWall;
    }


  }

  if (Mode == 2) {

  }
  if (wallCounter > 12){
    TurnDistance =  TurnDistance * 2;
    }
  if (wallCounter >= 13) {
    motorSpeed = 0;
    servoAngle = 90;
  }

}
