float gravity = 18000.0;
float friction = 0.1;

class Ball {
    float inertiaX = 0;
    float inertiaY = 0;
    Adafruit_SSD1306 display;

  public:
    int x = 64;
    int y = 32;

    void updatePosition(int accX, int accY) {
      inertiaX -= (accX / gravity);
      inertiaY -= (accY / gravity);

      applyFriction();
      movement();
    }

    void reinitialize() {
      inertiaX = 0;
      inertiaY = 0;

      x = 64;
      y = 32;
    }

  private:

    void applyFriction() {
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
    }

    void movement() {
      x += inertiaX;
      y += inertiaY;
    }
};
