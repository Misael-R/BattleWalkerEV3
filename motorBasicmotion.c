#pragma config(Sensor, S1, colorSensor, sensorEV3_Color)
#pragma config(Sensor, S2, ultraSensor, sensorEV3_Ultrasonic)
#pragma config(Sensor, S3, gyroSensor, sensorEV3_Gyro)
#pragma config(Sensor, S4, touchSensor, sensorEV3_Touch)
#pragma config(Motor, motorA, leftLeg, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor, motorB, rightLeg, tmotorEV3_Large, PIDControl, encoder)
#pragma config(Motor, motorC, weaponMotor, tmotorEV3_Medium, PIDControl, encoder)

// FSM States
#define PATROL 0
#define CHASE 1
#define ATTACK 2
#define EVADE 3
#define AVOID_BOUNDARY 4

int state = PATROL;

// Function to make the robot walk forward using alternate stepping
void walkForward(int speed, int duration) {
    motor[leftLeg] = speed;
    wait1Msec(duration);
    motor[leftLeg] = 0;
    motor[rightLeg] = speed;
    wait1Msec(duration);
    motor[rightLeg] = 0;
}

// Function to detect an enemy robot using the Ultrasonic Sensor
bool enemyDetected() {
    return (SensorValue[ultraSensor] < 30); // Detect enemy within 30 cm
}

// Function to attack using weapon
void fireWeapon() {
    motor[weaponMotor] = 100;  // Activate weapon
    wait1Msec(500);
    motor[weaponMotor] = 0;
}

// Function to avoid stepping out of the arena (Color Sensor)
void checkBoundary() {
    if (SensorValue[colorSensor] == 1) { // If black line detected
        motor[leftLeg] = -50;
        motor[rightLeg] = 50; // Turn back
        wait1Msec(700);
    }
}

// Function to evade enemy when hit (Touch Sensor)
void evade() {
    motor[leftLeg] = -50;
    motor[rightLeg] = 50;  // Quick rotation to escape
    wait1Msec(700);
}

// Function to maintain balance (Gyro Sensor)
void checkBalance() {
    int angle = SensorValue[gyroSensor];
    if (abs(angle) > 20) { // If tilt > 20 degrees, correct balance
        motor[leftLeg] = -50;
        motor[rightLeg] = -50;
        wait1Msec(500);
    }
}

// Main FSM loop
task main() {
    while (true) {
        switch (state) {
            case PATROL: // Patrol Mode
                walkForward(50, 500);
                if (enemyDetected()) state = CHASE;
                if (SensorValue[colorSensor] == 1) state = AVOID_BOUNDARY;
                break;

            case CHASE: // Chase the Enemy
                motor[leftLeg] = 50;
                motor[rightLeg] = 50;
                wait1Msec(500);
                state = ATTACK;
                break;

            case ATTACK: // Attack Mode
                fireWeapon();
                if (SensorValue[touchSensor] == 1) state = EVADE;
                state = PATROL;
                break;

            case EVADE: // Evade Mode
                evade();
                state = PATROL;
                break;

            case AVOID_BOUNDARY: // Avoid Boundary Mode
                checkBoundary();
                state = PATROL;
                break;
        }
        checkBalance(); // Maintain balance at all times
    }
}
