#include "echo.h"
#include "motors.h"

// Sensor pins
static int _trigPin;
static int _echoPin;
static int _echoPin2;
static int _echoPin3;
static int _echoPin4;

void echo_init(int trigPin, int echoPin, int echoPin2, int echoPin3, int echoPin4) {
    _trigPin = trigPin;
    _echoPin = echoPin;
    _echoPin2 = echoPin2;
    _echoPin3 = echoPin3;
    _echoPin4 = echoPin4;

    pinMode(_trigPin, OUTPUT);
    digitalWrite(_trigPin, LOW);
    pinMode(_echoPin, INPUT);
    pinMode(_echoPin2, INPUT);
    pinMode(_echoPin3, INPUT);
    pinMode(_echoPin4, INPUT);
}

int echo_readDistance(int pin) {
    unsigned long duration = pulseIn(pin, HIGH, 30000);
    if (duration == 0) return 0;
    return duration * 0.034 / 2;
}

void echo_handleObstacles(int frontMid, int back, int frontLeft, int frontRight) {

    const int FRONT_STOP = 10; // cm
    const int BACK_STOP  = 10;

    bool frontBlocked = 
        (frontMid  > 0 && frontMid  <= FRONT_STOP) ||
        (frontLeft > 0 && frontLeft <= FRONT_STOP) ||
        (frontRight > 0 && frontRight <= FRONT_STOP);

    bool backBlocked =
        (back > 0 && back <= BACK_STOP);

    // ---------------------------
    // FRONT OBSTACLE HANDLING
    // ---------------------------
    if (frontBlocked)
    {
        motors_coast();

        // Move slightly back to free robot
        motors_back();
        delay(200);
        motors_coast();

        // If left side is more blocked → turn right
        // If right side is more blocked → turn left
        if (frontLeft > 0 && frontLeft < frontRight)
        {
            motors_right();
        }
        else
        {
            motors_left();
        }

        delay(300);
        motors_coast();
        return;
    }


    if (backBlocked)
    {
        motors_coast();

        // Move slightly forward to free robot
        motors_forward();
        delay(200);
        motors_coast();
        return;
    }
}


void echo_lineMode(int frontMid, int back, int frontLeft, int frontRight) {

    const int FRONT_STOP = 10;  // cm
    const int BACK_STOP  = 5;
    
    // Front danger — stop & reverse
    if ((frontMid > 0  && frontMid  <= FRONT_STOP) ||
        (frontLeft     > 0  && frontLeft      <= FRONT_STOP) ||
        (frontRight    > 0  && frontRight     <= FRONT_STOP))
    {
        motors_coast();
        delay(30);
        return;
    }

    // Back danger — stop
    if (back > 0 && back <= BACK_STOP) {
        motors_coast();
        return;
    }
}

void echo_update() {
    // Trigger the sensor
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    // Read distances from all sensors
    int distanceFrontMid = echo_readDistance(_echoPin);
    int distanceBack     = echo_readDistance(_echoPin2);
    int distanceFrontLeft  = echo_readDistance(_echoPin3);
    int distanceFrontRight = echo_readDistance(_echoPin4);

    // Handle obstacles based on readings
    echo_handleObstacles(distanceFrontMid, distanceBack, distanceFrontLeft, distanceFrontRight);
}

