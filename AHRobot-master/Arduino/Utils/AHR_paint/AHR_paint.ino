// AHR AIR HOCKEY ROBOT PROJECT
// VERION FOR PAINT A GCODE WITH THE ROBOT!!!!!!

// Author: Jose Julio (@jjdrones)
// Hardware: Arduino MEGA + Ramps 1.4
// Date: 21/01/2014
// Updated: 
// Version: 1.01
// Project page : Spanish: English:
// GIT repository: 
// License: Open Software GPL License

// ROBOT and USER configuration parameters
#include "Configuration.h"
#include "Definitions.h"   // Variable definitions

// Array with the Gcode
#include "GCODE.h"  

void setup() 
{ 
  // STEPPER MOTOR PINS (SAME AS RAMPS 1.4)
  // X MOTOR
  //     X-STEP: A0    (PF0)
  //     X-DIR:  A1    (PF1)
  //     X-ENABLE: D38 (PD7)
  // Y MOTOR (Y-LEFT)
  //     Y-STEP: A6    (PF6)
  //     Y-DIR:  A7    (PF7)
  //     Y-ENABLE: A2  (PF2)
  // Z MOTOR (Y-RIGHT)
  //     Z-STEP: D46   (PL3)
  //     Z-DIR:  D48   (PL1)
  //     Z-ENABLE: A8  (PK0)

  // STEPPER PINS 
  // X_AXIS
  pinMode(38,OUTPUT);  // ENABLE MOTOR
  pinMode(A0,OUTPUT);  // STEP MOTOR
  pinMode(A1,OUTPUT);  // DIR MOTOR
  // Y_AXIS (Y-LEFT)
  pinMode(A2,OUTPUT);  // ENABLE MOTOR
  pinMode(A6,OUTPUT);  // STEP MOTOR
  pinMode(A7,OUTPUT);  // DIR MOTOR
  // Z_AXIS (Y-RIGHT)
  pinMode(A8,OUTPUT);  // ENABLE MOTOR
  pinMode(46,OUTPUT);  // STEP MOTOR
  pinMode(48,OUTPUT);  // DIR MOTOR 

  pinMode(A3,OUTPUT);  // DEBUG PIN FOR OSCILLOSCOPE TIME MEASURES

  pinMode(19,INPUT);  // RX1 Serial Port 1
  pinMode(18,OUTPUT); // TX1

  //FANS and LEDS
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(13,OUTPUT);

  // Disable Motors
  digitalWrite(38,HIGH);
  digitalWrite(A2,HIGH);
  digitalWrite(A8,HIGH);

  Serial.begin(115200);
  Serial.println("AHR Robot GCODE paint version v1.01");
  Serial.println("Initializing robot...");
  Serial.print("Free Memory: ");
  Serial.print(freeRam());
  Serial.println();
  delay(2500);

  //LED blink
  for (uint8_t k=0;k<4;k++)
  {
    digitalWrite(13,HIGH);
    delay(300);
    digitalWrite(13,LOW);
    delay(300);
  } 

  // We use TIMER 1 for stepper motor X AXIS and Timer 3 for Y AXIS
  // STEPPER MOTORS INITIALIZATION
  // TIMER1 CTC MODE
  TCCR1B &= ~(1<<WGM13);
  TCCR1B |=  (1<<WGM12);
  TCCR1A &= ~(1<<WGM11); 
  TCCR1A &= ~(1<<WGM10);

  // output mode = 00 (disconnected)
  TCCR1A &= ~(3<<COM1A0); 
  TCCR1A &= ~(3<<COM1B0); 

  // Set the timer pre-scaler
  // Generally we use a divider of 8, resulting in a 2MHz timer on 16MHz CPU
  TCCR1B = (TCCR1B & ~(0x07<<CS10)) | (2<<CS10);

  OCR1A = ZERO_SPEED;   // Motor stopped
  dir_x = 0;
  TCNT1 = 0;

  // We use TIMER 3 for stepper motor Y AXIS 
  // STEPPER MOTORS INITIALIZATION
  // TIMER3 CTC MODE
  TCCR3B &= ~(1<<WGM13);
  TCCR3B |=  (1<<WGM12);
  TCCR3A &= ~(1<<WGM11); 
  TCCR3A &= ~(1<<WGM10);

  // output mode = 00 (disconnected)
  TCCR3A &= ~(3<<COM1A0); 
  TCCR3A &= ~(3<<COM1B0); 

  // Set the timer pre-scaler
  // Generally we use a divider of 8, resulting in a 2MHz timer on 16MHz CPU
  TCCR3B = (TCCR3B & ~(0x07<<CS10)) | (2<<CS10);

  OCR3A = ZERO_SPEED;   // Motor stopped
  dir_y = 0;
  TCNT3 = 0;

  //Initializing init position
  position_x = ROBOT_INITIAL_POSITION_X*X_AXIS_STEPS_PER_UNIT;
  position_y = ROBOT_INITIAL_POSITION_Y*Y_AXIS_STEPS_PER_UNIT;

  delay(1000);

  Serial.println("Initializing Stepper motors...");
  delay(1000);
  TIMSK1 |= (1<<OCIE1A);  // Enable Timer1 interrupt
  TIMSK3 |= (1<<OCIE1A);  // Enable Timer1 interrupt

  // Enable steppers
  digitalWrite(38,LOW);   // X-axis
  digitalWrite(A2,LOW);   // Y-axis left
  digitalWrite(A8,LOW);   // Z-axis (Y-axis right)

  Serial.print("Max_acceleration_x: ");
  Serial.println(max_acceleration_x);
  Serial.print("Max_acceleration_y: ");
  Serial.println(max_acceleration_y);
  Serial.println("Moving to initial position...");
  Serial.println("Ready to paint!!");
  delay(1000);

  // Initializing Robot command variables
  com_pos_x = ROBOT_CENTER_X;
  com_pos_y = ROBOT_MIN_Y;
  com_speed_x = MAX_SPEED_X;
  com_speed_y = MAX_SPEED_Y;

  setSpeedS(com_speed_x,com_speed_y);
  setPosition(com_pos_x,com_pos_y);

  timer_old = micros();
  timer_packet_old = timer_old;
  micros_old = timer_old;

  gcode_index = 0;

  // Enable Air Hockey FANS
  //digitalWrite(9,HIGH);
  //digitalWrite(10,HIGH);

}

int stopped_counter=0;

// Main loop
void loop() 
{ 
  int dt;
  uint8_t logOutput=0;

  debug_counter++;
  timer_value = micros();
  if ((timer_value-timer_old)>=1000)  // 1Khz loop
  {
    //dt = (timer_value-timer_old)/1000;
    timer_old = timer_value;
    loop_counter++;
    
    if ((myAbs(speed_x)<2)&&(myAbs(speed_y)<2))
      stopped_counter++;
    else
      stopped_counter=0;
    
    if (stopped_counter>=2) // Move done? 50ms stopped
      {
      if (gcode_index < gcode_lines)
        {
        int new_pos_x_mm10 = gcode[gcode_index][0]*3;
        int new_pos_y_mm10 = gcode[gcode_index][1]*4 + 000;
        Serial.print(gcode_index);
        Serial.print(":");
        Serial.print(new_pos_x_mm10);
        Serial.print(",");
        Serial.print(new_pos_y_mm10);
        Serial.print(" ");
        Serial.print(position_x);
        Serial.print(";");
        Serial.print(position_y);
        Serial.print(" ");
        
        if (gcode_index==0)
          {
          Serial.print("FIRST MOVE");
          max_speed_x = MAX_SPEED_X;
          max_speed_y = MAX_SPEED_Y;
          }
        else
          {
          // We use a much lower speed for paint
          max_speed_x = SPEED_PAINT;
          max_speed_y = SPEED_PAINT;
          }
        
        // Send coordinates to robot in mm/10 units
        setPosition_mm10(new_pos_x_mm10,new_pos_y_mm10);
        gcode_index++;
        Serial.println();
        }
      else
        {
        Serial.print("RETURN HOME ");
        com_pos_x = ROBOT_CENTER_X;
        com_pos_y = ROBOT_MIN_Y;
        setPosition_mm10(com_pos_x*10,com_pos_y*10);
        Serial.println();  
        }
      }

    positionControl();
  } // 1Khz loop
}





