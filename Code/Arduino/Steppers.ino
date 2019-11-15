

ISR(TIMER1_COMPA_vect)
{
  if (dir_M1 == 0)
    return;

  SET(PORTE, 6); // STEP X-AXIS (MOTOR1)
  position_M1 += dir_M1;
  __asm__ __volatile__ (
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop");  // Wait for step pulse
  CLR(PORTE, 6);
}

ISR(TIMER3_COMPA_vect)
{
  if (dir_M2 == 0)
    return;

  SET(PORTD, 6); // STEP Y-AXIS (Motor2)
  position_M2 += dir_M2;
  __asm__ __volatile__ (
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop" "\n\t"
    "nop");  // Wait for step pulse
  CLR(PORTD, 6);
}

// stop_position = actual_posicion + (actual_speed*actual_speed)/(2*max_deceleration)
// Input parameters:
//    target_position_M1
//    target_speed_M1
//    max_acceleration

void positionControl()
{
  int32_t temp;
  uint32_t timer;
  int16_t dt;
  int16_t result_speed_M1, result_speed_M2;
  int16_t absSpeedM1, absSpeedM2;


  real_position_x = ((position_M1 + position_M2) / 2) / X_AXIS_STEPS_PER_UNIT;
  real_position_y = ((position_M1 - position_M2) / 2) / Y_AXIS_STEPS_PER_UNIT;
  timer = micros();

  dt = constrain(timer - micros_old, 0, 2000); 

  micros_old = timer;

  acceleration_M1 = max_acceleration;
  acceleration_M2 = max_acceleration;

  // Scurve profile implementation...
  absSpeedM1 = abs(speed_M1);
  absSpeedM2 = abs(speed_M2);
  if (absSpeedM1 < SCURVE_LOW_SPEED) {
    acceleration_M1 = map(absSpeedM1, 0, SCURVE_LOW_SPEED, MIN_ACCEL, max_acceleration);
    if (acceleration_M1 > max_acceleration)
      acceleration_M1 = max_acceleration;
  }
  if (absSpeedM2 < SCURVE_LOW_SPEED) {
    acceleration_M2 = map(absSpeedM2, 0, SCURVE_LOW_SPEED, MIN_ACCEL, max_acceleration);
    if (acceleration_M2 > max_acceleration)
      acceleration_M2 = max_acceleration;
  }
  // SCURVE FACOR 1900!!!

  // MOTOR1 M1
  temp = (long)speed_M1 * speed_M1;
  temp = temp / (1900 * (long)acceleration_M1);
  pos_stop_M1 = position_M1 + sign(speed_M1) * temp;    
  if (target_position_M1 > position_M1)    // Positive move
  {
    if (pos_stop_M1 >= target_position_M1)  
      result_speed_M1 = 0;                  
    else
      result_speed_M1 = target_speed_M1;     
  }
  else   // Negative move
  {
    if (pos_stop_M1 <= target_position_M1) 
      result_speed_M1 = 0;
    else
      result_speed_M1 = -target_speed_M1;
  }

  // MOTOR2 M2
  temp = (long)speed_M2 * speed_M2;
  temp = temp / (1900 * (long)acceleration_M2);
  pos_stop_M2 = position_M2 + sign(speed_M2) * temp;
  if (target_position_M2 > position_M2) // Positive move
  {
    if (pos_stop_M2 >= target_position_M2)  // Start decelerating?
      result_speed_M2 = 0;                  // The deceleration ramp is done inside the setSpeed function
    else
      result_speed_M2 = target_speed_M2;    // The aceleration ramp is done inside the setSpeed function
  }
  else   // Negative move
  {
    if (pos_stop_M2 <= target_position_M2)  // Start decelerating?
      result_speed_M2 = 0;
    else
      result_speed_M2 = -target_speed_M2;
  }

  setMotorSpeed(result_speed_M1, result_speed_M2, dt); // change Motors speed
}

// Speed could be positive or negative
void setMotorSpeed(int16_t m1tspeed, int16_t m2tspeed, int16_t dt)
{
  long timer_period;
  int16_t accel;

  // Limit max speeds
  m1tspeed = constrain(m1tspeed, -user_max_speed, user_max_speed);
  m2tspeed = constrain(m2tspeed, -user_max_speed, user_max_speed);

  // M1 MOTOR
  // We limit acceleration => speed ramp
  accel = ((long)acceleration_M1 * dt) / 1000; // We divide by 1000 because dt are in microseconds
  if (((long)m1tspeed - speed_M1) > accel) // We use long here to avoid overflow on the operation
    speed_M1 += accel;
  else if (((long)speed_M1 - m1tspeed) > accel)
    speed_M1 -= accel;
  else
    speed_M1 = m1tspeed;

  if ((speed_M1 == 0) && (dir_M1 != 0))
    dir_M1 = 0;
  else if ((speed_M1 > 0) && (dir_M1 != 1))
  {
    CLR(PORTB, 4);
    dir_M1 = 1;
  }
  else if ((speed_M1 < 0) && (dir_M1 != -1))
  {
    SET(PORTB, 4);
    dir_M1 = -1;
  }

  if (speed_M1 == 0)
    timer_period = ZERO_SPEED;
  else if (speed_M1 > 0)
    timer_period = 2000000 / speed_M1; // 2Mhz timer
  else
    timer_period = 2000000 / -speed_M1;

  if (timer_period > 65535)   
    timer_period = ZERO_SPEED;

  OCR1A = timer_period;
  // Check  if we need to reset the timer...
  if (TCNT1 > OCR1A)
    TCNT1 = 0;

  // M2 MOTOR
  // We limit acceleration => speed ramp
  accel = ((long)acceleration_M2 * dt) / 1000;
  if (((long)m2tspeed - speed_M2) > accel)
    speed_M2 += accel;
  else if (((long)speed_M2 - m2tspeed) > accel)
    speed_M2 -= accel;
  else
    speed_M2 = m2tspeed;

  // Check if we need to change the direction pins
  if ((speed_M2 == 0) && (dir_M2 != 0))
    dir_M2 = 0;
  else if ((speed_M2 > 0) && (dir_M2 != 1))
  {
    CLR(PORTC, 6);
    dir_M2 = 1;
  }
  else if ((speed_M2 < 0) && (dir_M2 != -1))
  {
    SET(PORTC, 6);
    dir_M2 = -1;
  }

  if (speed_M2 == 0)
    timer_period = ZERO_SPEED;
  else if (speed_M2 > 0)
    timer_period = 2000000 / speed_M2; // 2Mhz timer
  else
    timer_period = 2000000 / -speed_M2;

  if (timer_period > 65535)   
    timer_period = ZERO_SPEED;

  OCR3A = timer_period;
  if (TCNT3 > OCR3A)
    TCNT3 = 0;

}


void setPosition_no_straight(int target_x_mm_new, int target_y_mm_new)
{
  target_x_mm = constrain(target_x_mm_new, ROBOT_MIN_X, ROBOT_MAX_X);
  target_y_mm = constrain(target_y_mm_new, ROBOT_MIN_Y, ROBOT_MAX_Y);

  target_position_M1 = (target_x_mm + target_y_mm) * X_AXIS_STEPS_PER_UNIT;
  target_position_M2 = (target_x_mm - target_y_mm) * Y_AXIS_STEPS_PER_UNIT;
  com_speed_M1 = target_speed_M1;
  com_speed_M2 = target_speed_M2;
}

void setPosition_straight(int target_x_mm_new, int target_y_mm_new)
{
  int diff_M1;
  int diff_M2;
  float factor1;
  float factor2;
  long tspeed1;
  long tspeed2;
  long diffspeed1;
  long diffspeed2;
  float speedfactor1;
  float speedfactor2;

  target_x_mm = constrain(target_x_mm_new, ROBOT_MIN_X, ROBOT_MAX_X);
  target_y_mm = constrain(target_y_mm_new, ROBOT_MIN_Y, ROBOT_MAX_Y);
  target_position_M1 = (target_x_mm + target_y_mm) * X_AXIS_STEPS_PER_UNIT;
  target_position_M2 = (target_x_mm - target_y_mm) * Y_AXIS_STEPS_PER_UNIT;

  diff_M1 = myAbs(target_position_M1 - position_M1);
  diff_M2 = myAbs(target_position_M2 - position_M2);

  factor1 = 1.0;
  factor2 = 1.0;
  if (diff_M2 == 0) // to avoid division by 0
    factor2 = 0.0;
  else if (diff_M1 > diff_M2)
    factor2 = (float)diff_M2 / (float)diff_M1;
  else
    factor1 = (float)diff_M1 / (float)diff_M2;

  tspeed1 = sign(target_position_M1 - position_M1) * max_speed * factor1;
  tspeed2 = sign(target_position_M2 - position_M2) * max_speed * factor2;
  diffspeed1 = abs(speed_M1 - tspeed1);
  diffspeed2 = abs(speed_M2 - tspeed2);
  speedfactor1 = 1.05 - (diffspeed2 - diffspeed1) / (2.0 * max_speed);
  speedfactor1 = constrain(speedfactor1, 0.0, 1.0);
  speedfactor2 = 1.05 - (diffspeed1 - diffspeed2) / (2.0 * max_speed);
  speedfactor2 = constrain(speedfactor2, 0.0, 1.0);

  target_speed_M1 = max_speed * factor1 * speedfactor1 * speedfactor1;
  target_speed_M2 = max_speed * factor2 * speedfactor2 * speedfactor2;

}

void updatePosition_straight()
{
  int diff_M1;
  int diff_M2;
  float factor1;
  float factor2;
  long tspeed1;
  long tspeed2;
  long diffspeed1;
  long diffspeed2;
  float speedfactor1;
  float speedfactor2;

  // Speed adjust to draw straight lines (aproximation)
  diff_M1 = myAbs(target_position_M1 - position_M1);
  diff_M2 = myAbs(target_position_M2 - position_M2);

  factor1 = 1.0;
  factor2 = 1.0;
  if (diff_M2 == 0) // to avoid division by 0
    factor2 = 0.0;
  else if (diff_M1 > diff_M2)
    factor2 = (float)diff_M2 / (float)diff_M1;
  else
    factor1 = (float)diff_M1 / (float)diff_M2;

  // Calculate the target speed (sign) for each motor
  tspeed1 = sign(target_position_M1 - position_M1) * max_speed * factor1;
  tspeed2 = sign(target_position_M2 - position_M2) * max_speed * factor2;
  diffspeed1 = abs(speed_M1 - tspeed1);
  diffspeed2 = abs(speed_M2 - tspeed2);
  speedfactor1 = 1.05 - (diffspeed2 - diffspeed1) / (2.0 * max_speed);
  speedfactor1 = constrain(speedfactor1, 0.0, 1.0);
  speedfactor2 = 1.05 - (diffspeed1 - diffspeed2) / (2.0 * max_speed);
  speedfactor2 = constrain(speedfactor2, 0.0, 1.0);

  target_speed_M1 = max_speed * factor1 * speedfactor1 * speedfactor1;
  target_speed_M2 = max_speed * factor2 * speedfactor2 * speedfactor2;
}
