#include "util.h"

#define MOTOR_0_PIN 12 // rear left
#define MOTOR_1_PIN 13 // rear right
#define MOTOR_2_PIN 14 // front right
#define MOTOR_3_PIN 15 // front left

// Motors array indexes:
const int MOTOR_REAR_LEFT = 0;
const int MOTOR_REAR_RIGHT = 1;
const int MOTOR_FRONT_RIGHT = 2;
const int MOTOR_FRONT_LEFT = 3;

const int MOTOR_PINS[] = {MOTOR_0_PIN, MOTOR_1_PIN, MOTOR_2_PIN, MOTOR_3_PIN};

#define PWM_FREQUENCY 50          // Частота PWM (50 Гц — безопасная для ESC)
#define PWM_RESOLUTION 12         // Разрешение PWM (0-4095)
#define PWM_STOP 1000             // ШИМ-значение для остановки мотора
#define PWM_MIN 1000              // Минимальное значение для ESC
#define PWM_MAX 2000              // Максимальное значение для ESC

void setupMotors() {
    print("Setup Motors\n");

    // Настройка PWM каналов для моторов
    for (int i = 0; i < 4; i++) {
        ledcAttach(MOTOR_PINS[i], PWM_FREQUENCY, PWM_RESOLUTION);
    }

    // Арминг ESC
    for (int i = 0; i < 4; i++) {
        ledcWrite(MOTOR_PINS[i], getDutyCycle(1)); // PWM_MAX для начала арминга
    }
    delay(3000);

    for (int i = 0; i < 4; i++) {
        ledcWrite(MOTOR_PINS[i], getDutyCycle(0)); // PWM_MIN для завершения арминга
    }
    delay(3000);

    sendMotors();
    print("Motors initialized\n");
}

int getDutyCycle(float value) {
    value = constrain(value, 0, 1);
    float pwm = mapff(value, 0, 1, PWM_MIN, PWM_MAX);
    if (value == 0) pwm = PWM_STOP;
    float duty = mapff(pwm, PWM_MIN, PWM_MAX, 0, (1 << PWM_RESOLUTION) - 1);
    return round(duty);
}

void sendMotors() {
    for (int i = 0; i < 4; i++) {
        ledcWrite(MOTOR_PINS[i], getDutyCycle(motors[i]));
    }
}

bool motorsActive() {
    bool hasMotor = motors[0] != 0 || motors[1] != 0 || motors[2] != 0 || motors[3] != 0;
    return hasMotor;
}

void testMotor(uint8_t n) {
    print("Testing motor %d\n", n);

    bool hasMotor = motorsActive();
    print("Motors initialized %d\n", hasMotor);

    motors[n] = 1;
    delay(50);
    sendMotors();
    delay(3000);
    motors[n] = 0;
    sendMotors();
    print("Done\n");
}