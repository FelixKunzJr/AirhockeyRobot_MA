
#define VERSION "2.17"


#include "Configuration.h"
#include "Definitions.h"  
void setup()
{
  pinMode(4, OUTPUT); // ENABLE MOTORS
  pinMode(7, OUTPUT); // STEP MOTOR 1 PORTE,6
  pinMode(8, OUTPUT); // DIR MOTOR 1  PORTB,4
  pinMode(12, OUTPUT); // STEP MOTOR 2 PORTD,6
  pinMode(5, OUTPUT); // DIR MOTOR 2  PORTC,6
  digitalWrite(4, HIGH);  // Disbale stepper motors

  pinMode(10, OUTPUT);  // Servo1 (arm)
  pinMode(13, OUTPUT);  // Servo2

  pinMode(A4, OUTPUT); 
  digitalWrite(A4, LOW); 

  pinMode(A3,INPUT);   // User puch button
  digitalWrite(A3,HIGH); // Enable pullup

  Serial.begin(115200);   // PC debug connection
  Serial1.begin(115200);  // ESP serial connection
  delay(5000);
  Serial.print("Maturarbeit Felix Kunz 2020 ");
  Serial.println(VERSION);
  delay(1000);

  //LED blink
  for (uint8_t k = 0; k < 5; k++)
  {
    digitalWrite(13, HIGH);
    delay(300);
    digitalWrite(13, LOW);
    delay(300);
  }

  Serial.println("Initializing Wifi module...");

  ESPInit();

  Serial.println("Initializing Stepper motors...");

  TCCR1B &= ~(1 << WGM13);
  TCCR1B |=  (1 << WGM12);
  TCCR1A &= ~(1 << WGM11);
  TCCR1A &= ~(1 << WGM10);

  TCCR1A &= ~(3 << COM1A0);
  TCCR1A &= ~(3 << COM1B0);


  TCCR1B = (TCCR1B & ~(0x07 << CS10)) | (2 << CS10);

  OCR1A = ZERO_SPEED;   // Motor stopped
  dir_M1 = 0;
  TCNT1 = 0;

  TCCR3B &= ~(1 << WGM13);
  TCCR3B |=  (1 << WGM12);
  TCCR3A &= ~(1 << WGM11);
  TCCR3A &= ~(1 << WGM10);

  TCCR3A &= ~(3 << COM1A0);
  TCCR3A &= ~(3 << COM1B0);

  TCCR3B = (TCCR3B & ~(0x07 << CS10)) | (2 << CS10);

  OCR3A = ZERO_SPEED;   // Motor stopped
  dir_M2 = 0;
  TCNT3 = 0;

  delay(1000);
  TIMSK1 |= (1 << OCIE1A); // Enable Timer1 interrupt
  TIMSK3 |= (1 << OCIE1A); // Enable Timer1 interrupt

  // Enable steppers
  digitalWrite(4, LOW);

  delay(1000);

  position_M1 = (ROBOT_INITIAL_POSITION_X + ROBOT_INITIAL_POSITION_Y) * X_AXIS_STEPS_PER_UNIT;
  position_M2 = (ROBOT_INITIAL_POSITION_X - ROBOT_INITIAL_POSITION_Y) * Y_AXIS_STEPS_PER_UNIT;
  target_position_M1 = position_M1;
  target_position_M2 = position_M2;

  user_max_speed = MAX_SPEED;
  user_max_accel = MAX_ACCEL;
  max_acceleration = user_max_accel;
  max_speed = user_max_speed / 2;
  user_robot_defense_position = ROBOT_DEFENSE_POSITION_DEFAULT;
  user_robot_defense_attack_position = ROBOT_DEFENSE_ATTACK_POSITION_DEFAULT;
  defense_position = ROBOT_DEFENSE_POSITION_DEFAULT;   // Robot y axis defense position
  attack_position = ROBOT_DEFENSE_ATTACK_POSITION_DEFAULT;   // Robot y axis position for defense+attack


  Serial.print("Max_acceleration: ");
  Serial.println(user_max_accel);
  Serial.print("Max speed: ");
  Serial.println(user_max_speed);
  Serial.println("Moving to initial position...");
  Serial.println("Ready!!");

  setPosition_straight(ROBOT_CENTER_X, ROBOT_DEFENSE_POSITION_DEFAULT);

  Serial.print("Target_position:");
  Serial.print(ROBOT_CENTER_X);
  Serial.print(",");
  Serial.println(ROBOT_DEFENSE_POSITION_DEFAULT);

  digitalWrite(13, HIGH);
  Serial.print("AHR JJRobots Air Hockey Robot EVO version ");
  Serial.println(VERSION);
  Serial.println("ready...");

  testmode=false;
  robot_status = 0;
  timer_old = micros();
  timer_packet_old = timer_old;
  micros_old = timer_old;
}


void loop()
{
  int dt;
  uint8_t logOutput = 0;

  debug_counter++;

  if (analogRead(A3)<100) 
  {
    loop_counter=0;
    testmode=true;
  }
  
  timer_value = micros();
  if ((timer_value - timer_old) >= 1000) // MAIN 1Khz loop
  {
    timer_old = timer_value;
    loop_counter++;

    packetRead();  // Check for new packets...
    if (newPacket > 0)
    {
      dt = (timer_value - timer_packet_old) / 1000.0;
      //Serial.println(dt);
      //dt = 16;  //60 Hz = 16.66ms
      timer_packet_old = timer_value;
      logOutput = 0;

      Serial.print("P");
      Serial.print(newPacket);

      if (newPacket == 1) // Camera packet
      {
        // Puck detection and trayectory prediction
        cameraProcess(dt);
        // Strategy based on puck prediction
        newDataStrategy();

      }
      else if (newPacket == 2) { // User sends a manual move
        robot_status = 5;    // Manual mode
        Serial.print(" ");
        Serial.print(user_target_x);
        Serial.print(",");
        Serial.print(user_target_y);
      }
      else if (newPacket == 3) {
        Serial.print(" USER MAX SPEED:");
        Serial.println(user_max_speed);
        Serial.print("USER MAX ACCEL:");
        Serial.println(user_max_accel);
        Serial.print("USER DEFENSE POSITION:");
        Serial.println(user_robot_defense_position);
        Serial.print("USER ATTACK POSITION:");
        Serial.print(user_robot_defense_attack_position);
      }

      robotStrategy();

      missingStepsDetection();

      Serial.println();
      newPacket = 0;
    }  

    if (testmode)
      testMovements();

#ifdef DEBUG
    if ((loop_counter % 1293) == 0) {
      Serial.print("ROBOT MSD: ");
      Serial.print(robotMissingStepsErrorX);
      Serial.print(",");
      Serial.println(robotMissingStepsErrorY);
    }
#endif

    if ((loop_counter % 10) == 0)
      updatePosition_straight();  

    positionControl();
  } // 1Khz loop
}
