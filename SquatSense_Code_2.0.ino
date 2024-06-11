#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include <Ticker.h>

Adafruit_BNO055 bno = Adafruit_BNO055(55);

int ledPin = 2;

int NUM_DATA_POINTS = 5;
int idx = 0;
double y_angle_avg = 0;
double y_angle_buffer[5] = {0};
int ledState = LOW;


Ticker toggler;
Ticker tick;

float rolling_avg(double *y_angle_list) {
  
  if (idx < NUM_DATA_POINTS) {
      y_angle_buffer[idx] = *y_angle_list;
      idx++;
      return NAN; // Not enough data yet
  } else {
      // Calculate averages
      y_angle_avg = 0;
      for (int i = 0; i < NUM_DATA_POINTS; i++) {
          y_angle_avg += y_angle_buffer[i];
      }
      y_angle_avg /= NUM_DATA_POINTS;

      // Shift elements in the buffer to remove the oldest data point
      for (int i = 0; i < NUM_DATA_POINTS - 1; i++) {
          y_angle_buffer[i] = y_angle_buffer[i + 1];
      }
      // Add the newest data point to the buffer
      y_angle_buffer[NUM_DATA_POINTS - 1] = *y_angle_list;

      return y_angle_avg;
  }
}

void blink() {
  digitalWrite(ledPin, !digitalRead(ledPin));
}

void ledToggle(double angle) {
  static bool tick_attach = false;
  // normal squat
  if (angle > 80 && angle < 100) {
    if (!tick_attach) {
      tick.attach(1, blink);
      tick_attach = true;
    }
  }
  // Shallow squat
  else if (angle > 40 && angle < 60) {
    if (!tick_attach) {
      tick.attach(0.5, blink);
      tick_attach = true;
    }
  }
  // Deep squat
  else if (angle > 105 && angle < 120) {
    if (!tick_attach) {
      tick.attach(0.25, blink);
      tick_attach = true;
    }
  }
  else {
    tick.detach();
    digitalWrite(ledPin, LOW);
    tick_attach = false;
  }  
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("Orientation Sensor Test"); Serial.println("");
  
  /* Initialise the sensor */
  if(!bno.begin())
  {
    /* There was a problem detecting the BNO055 ... check your connections */
    Serial.print("Ooops, no BNO055 detected ... Check your wiring or I2C ADDR!");
    while(1);
  }
  
  delay(1000);
    
  bno.setExtCrystalUse(true);

  pinMode(ledPin, OUTPUT);
}


void loop() {
  // put your main code here, to run repeatedly:
  /* Get a new sensor event */ 
  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  double current_angle = rolling_avg(&euler.y());

  current_angle = current_angle + 90;

  Serial.print("Output Angle: ");
  Serial.print(current_angle);
  Serial.println("");

  ledToggle(current_angle);
 
  delay(100);

}
