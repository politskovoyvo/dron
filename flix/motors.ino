// Motors output control using BLHeli_S ESCs
// PWM settings for BLHeli_S:
// - Frequency: 400 Hz
// - Min pulse: 1000 μs
// - Max pulse: 2000 μs
// - Stop pulse: 1000 μs

#include "util.h"

#define MOTOR_0_PIN 12 // rear left
#define MOTOR_1_PIN 13 // rear right
#define MOTOR_2_PIN 14 // front right
#define MOTOR_3_PIN 15 // front left

#define PWM_FREQUENCY 400
#define PWM_RESOLUTION 12
#define PWM_STOP 1000
#define PWM_MIN 1000
#define PWM_MAX 2000

// Motors array indexes:
const int MOTOR_REAR_LEFT = 0;
const int MOTOR_REAR_RIGHT = 1;
const int MOTOR_FRONT_RIGHT = 2;
const int MOTOR_FRONT_LEFT = 3;

// Массив пинов для удобства
const int MOTOR_PINS[4] = {MOTOR_0_PIN, MOTOR_1_PIN, MOTOR_2_PIN, MOTOR_3_PIN};

void setupMotors() {
	print("Setup Motors\n");

	// Настройка PWM каналов для моторов
	for (int i = 0; i < 4; i++) {
		ledcAttach(MOTOR_PINS[i], PWM_FREQUENCY, PWM_RESOLUTION);
	}

	// Выполняем арминг ESC
    armMotors();

	// Инициализация моторов с минимальным сигналом
    for (int i = 0; i < 4; i++) {
        motors[i] = 0; // Устанавливаем минимальный сигнал (PWM_STOP)
    }

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
	if (!armed) {
		armMotors();
	}
	
	for (int i = 0; i < 4; i++) {
		ledcWrite(MOTOR_PINS[i], getDutyCycle(motors[i]));
	}
}

bool motorsActive() {
	return armed && (motors[0] != 0 || motors[1] != 0 || motors[2] != 0 || motors[3] != 0);
}

void armMotors() {
	if (armed) return;
	
	print("Arming ESCs...\n");
	
	// Отправляем максимальный сигнал
	print("Sending max signal...\n");
	for (int i = 0; i < 4; i++) {
		ledcWrite(MOTOR_PINS[i], getDutyCycle(1)); // PWM_MAX для начала арминга
	}

	delay(3000); // Ждем 3 секунды

	// Отправляем минимальный сигнал
	print("Sending min signal...\n");
	for (int i = 0; i < 4; i++) {
		ledcWrite(MOTOR_PINS[i], getDutyCycle(0)); // PWM_MIN для завершения арминга
	}

	delay(3000); // Ждем 3 секунды
	
	armed = true;
	
	print("Motors armed\n");
}

void disarmMotors() {
	if (!armed) return;
	
	print("Disarming motors...\n");
	
	armed = false;
	
	// Отправляем минимальный сигнал
	for (int i = 0; i < 4; i++) {
		motors[i] = 0;
	}

	sendMotors();
	
	print("Motors disarmed\n");
}

void testMotor(uint8_t n) {
	print("Testing motor %d (pin %d)\n", n, MOTOR_PINS[n]);
	
	// Отправляем 50% мощности
	motors[n] = 0.1;
	
	// Выводим отладочную информацию
	int duty = getDutyCycle(motors[n]);
	print("Motor %d: value=%.2f, duty=%d\n", n, motors[n], duty);
	
	// Отправляем сигнал
	sendMotors();
	delay(50); // ESP32 may need to wait until the end of the current cycle to change duty
	
	print("Motor %d running for 2 seconds...\n", n);
	pause(2);
	
	// Останавливаем мотор
	motors[n] = 0;
	sendMotors();
	print("Motor %d stopped\n", n);
}
