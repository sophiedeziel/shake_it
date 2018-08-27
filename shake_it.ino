#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

Adafruit_SSD1306 display(4);

const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

float gravity = 18000.0;
float friction = 0.1;

// game modes:
// 0: start
// 1: jeu
// 2: dead
int gameMode = 0;

class Ball {
    int x = 64;
    int y = 32;

    float inertiaX = 0;
    float inertiaY = 0;

  public:

    void updatePosition(int accX, int accY) {
      inertiaX -= (accX / gravity);
      inertiaY -= (accY / gravity);

      if (inertiaX > 0) {
        inertiaX -= friction;
      } else {
        inertiaX += friction;
      }

      if (inertiaY > 0) {
        inertiaY -= friction;
      } else {
        inertiaY += friction;
      }

      x += inertiaX;
      y += inertiaY;
    }

    void draw() {
      display.fillCircle(x, y, 5, WHITE);
    }
};

Ball ball;

void setup() {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);

  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Serial.begin(115200);
}

void loop() {
  display.clearDisplay();
  getAccData();
  switch (gameMode) {
    case 0:
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 25);
      display.println("ShakeIT to begin");

      detectShake();
      break;
    case 1:
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(0, 25);

      // ici, notre accéléromètre a une rotation de 90 degrés par rapport a notre écran.
      // Donc, l'axe y de l'accélérometre correspond a l'axe x de l'écran

      // gauche: y+
      // droite: y-
      // haut:   x+
      // bas:    x-

      ball.updatePosition(AcY, AcX);
      ball.draw();

      break;
    case 2:

      break;
  }

  display.display();


}

void getAccData() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);

  Wire.requestFrom(MPU_addr, 14, true); // request a total of 14 registers
  AcX = Wire.read() << 8 | Wire.read(); // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)
  AcY = Wire.read() << 8 | Wire.read(); // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ = Wire.read() << 8 | Wire.read(); // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp = Wire.read() << 8 | Wire.read(); // 0x41 (TEMP_OUT_H)   & 0x42 (TEMP_OUT_L)
  GyX = Wire.read() << 8 | Wire.read(); // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY = Wire.read() << 8 | Wire.read(); // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ = Wire.read() << 8 | Wire.read(); // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  Serial.print(AcX);
  Serial.print(" ");

  Serial.print(AcY);
  Serial.print(" ");

  Serial.println(AcZ);
}

void detectShake() {
  static int last_z;

  if (last_z - AcZ > 1000) {
    gameMode = 1;
  }
}
