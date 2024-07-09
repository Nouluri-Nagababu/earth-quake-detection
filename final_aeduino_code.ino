#include <MPU6050_tockn.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

MPU6050 mpu6050(Wire);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
SoftwareSerial sim900(9, 10);

const int BUZZER_PIN = 13;
const int BUZZER_FREQ = 5000;
const String PHONE_NUMBER = "9381010253"; // Replace with your phone number
const float SCALE_FACTOR = 163.0;
void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  lcd.begin(16, 2);
  sim900.begin(9600);
  pinMode(BUZZER_PIN, OUTPUT);

  // Initialize the GSM module
  delay(2000); // Wait for the module to start up
  sim900.println("AT"); // Send AT command to check if module is responding
  delay(500);
  sim900.println("AT+CMGF=1"); // Set SMS mode to text
  delay(500);
}

void loop() {
  mpu6050.update();

  float x_raw = mpu6050.getAccX();
  float y_raw = mpu6050.getAccY();
  float z_raw = mpu6050.getAccZ();

  // Scale raw sensor values to degrees per second
  float x_dps = x_raw / SCALE_FACTOR * 1000.0;
  float y_dps = y_raw / SCALE_FACTOR * 1000.0;
  float z_dps = z_raw / SCALE_FACTOR * 1000.0;

  // Calculate earthquake magnitude
  float magnitude = sqrt(pow(x_dps, 2) + pow(y_dps, 2) + pow(z_dps, 2));
  //Serial.print("EARTHQUAKE MAGNITUDE: ");
  Serial.print("M");
  Serial.print(magnitude);
  Serial.print("\n");
  Serial.print("*");
  if (magnitude > 7.9) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Earthquake Alert!");
    lcd.setCursor(0, 1);
    lcd.print("Magnitude: ");
    lcd.print(magnitude);
    
    // Produce the tone on the buzzer pin for 1 second
    tone(BUZZER_PIN, BUZZER_FREQ);
    delay(1000);
    
    // Stop the tone after 1 second
    noTone(BUZZER_PIN);
    
    lcd.clear();

    // Send earthquake alert via SMS
    String message = "Earthquake Alert! Magnitude: " + String(magnitude);
    sim900.println("AT+CMGS=\"" + PHONE_NUMBER + "\""); // Send SMS to phone number
    delay(500);
    sim900.print(message); // Send message content
    delay(500);
    sim900.write((byte)26); // End SMS message
    delay(500);
  }
  
  lcd.setCursor(0, 0);
  lcd.print("X:");
  lcd.print(mpu6050.getAngleX());
  lcd.print(" Y:");
  lcd.print(mpu6050.getAngleY());
  lcd.setCursor(0, 1);
  lcd.print("Z:");
  lcd.print(mpu6050.getAngleZ());
  
  /*Serial.print("angleX : ");
  Serial.print(mpu6050.getAngleX());
  Serial.print("\tangleY : ");
  Serial.print(mpu6050.getAngleY());
  Serial.print("\tangleZ : ");
  Serial.println(mpu6050.getAngleZ());*/
  
  delay(200);
}
