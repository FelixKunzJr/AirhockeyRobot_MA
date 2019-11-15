
// Puck trayectory prediction

void cameraProcess(int time)
{
  int vectorX;
  int vectorY;
  double slope;

  int bounce_x;
  int bounce_y;

  // Speed calculation on each axis
  vectorX = (puckCoordX - puckOldCoordX);
  vectorY = (puckCoordY - puckOldCoordY);

  puckOldSpeedX = puckSpeedX;
  puckOldSpeedY = puckSpeedY;
  puckSpeedX = (long)vectorX * 100 / time; // speed in dm/ms (we use this units to not overflow the variable)
  puckSpeedY = (long)vectorY * 100 / time;

  // Noise detection, if there are a big speeds this should be noise
  if ((puckSpeedX < -1000) || (puckSpeedX > 1000) || (puckSpeedY < -1000) || (puckSpeedY > 1000))
  {
    Serial.println("NOISE");
    predict_status = -1;
    predict_x_old = -1;
    return;
  }

  if (predict_status == -1)  // Noise on last reading?
  {
    puckSpeedXAverage = puckSpeedX;
    puckSpeedYAverage = puckSpeedY;
  }
  else
  {
    // if there are low accelerations (similar speeds on readings) we apply an average filtering with the previous value...
    if (myAbs(puckSpeedX - puckOldSpeedX) < 50)
      puckSpeedXAverage = (puckSpeedX + puckOldSpeedX) >> 1;
    else
      puckSpeedXAverage = puckSpeedX;
    if (myAbs(puckSpeedY - puckOldSpeedY) < 50)
      puckSpeedYAverage = (puckSpeedY + puckOldSpeedY) >> 1;
    else
      puckSpeedYAverage = puckSpeedY;
  }

  //puckSpeed = sqrt(vectorX*vectorX + vectorY*vectorY)*1000.0/time;
  //puckDirection = atan2(vectorY,vectorX);

  predict_x_attack = -1;


  if (puckSpeedYAverage < -50)  //-25
  {
    predict_status = 1;

    // slope formula: m = (y2-y1)/(x2-x1)
    if (vectorX == 0)  // To avoid division by 0
      slope = 9999999;
    else
      slope = (float)vectorY / (float)vectorX;

    predict_y = defense_position + PUCK_SIZE;
    predict_x = (predict_y - puckCoordY) / slope;
    predict_x += puckCoordX;
    predict_x_attack = ((attack_position + PUCK_SIZE) - puckCoordY) / slope;
    predict_x_attack += puckCoordX;

    if ((predict_x < PUCK_SIZE) || (predict_x > (TABLE_WIDTH - PUCK_SIZE)))
    {
      predict_status = 2;
      predict_bounce = 1;
      predict_bounce_status = 1;
      // Wich side?
      if (predict_x < PUCK_SIZE)
      {
        bounce_x = PUCK_SIZE;
      }
      else
      {
        bounce_x = (TABLE_WIDTH - PUCK_SIZE);
      }
      bounce_y = (bounce_x - puckCoordX) * slope + puckCoordY;
      predict_time = (bounce_y - puckCoordY) * 100L / puckSpeedY; // time until bouce
      slope = -slope;
      predict_y = defense_position + PUCK_SIZE;
      predict_x = (predict_y - bounce_y) / slope;
      predict_x += bounce_x;

      if ((predict_x < PUCK_SIZE) || (predict_x > (TABLE_WIDTH - PUCK_SIZE))) // New bounce with side wall?
      {
        predict_x_old = -1;
        predict_status = 0;
      }
      else
      {
         if (myAbs(puckSpeedY - puckOldSpeedY) > 50)
        {

          predict_x_old = -1;
        }
        else
        {

          if (predict_x_old != -1)
            predict_x = (predict_x_old + predict_x) >> 1;
          predict_x_old = predict_x;
          // We introduce a factor (120 instead of 100) to model the bounce (20% loss in speed)(to improcve...)
          predict_time = predict_time + (predict_y - puckCoordY) * 120L / puckSpeedY; // in ms
          predict_time -= VISION_SYSTEM_LAG;
        }
      }
    }
    else  // No bounce, direct impact
    {
      if (predict_bounce_status == 1)  // This is the first direct impact trajectory after a bounce
      {
        // We dont predict nothing new...
        predict_bounce_status = 0;
      }
      else
      {
        if (predict_x_old > 0)
          predict_x = (predict_x_old + predict_x) >> 1;
        predict_x_old = predict_x;

        predict_time = ((defense_position + PUCK_SIZE) - puckCoordY) * 100L / puckSpeedY; // in ms
        predict_time_attack = ((attack_position + PUCK_SIZE) - puckCoordY) * 100L / puckSpeedY; // in ms
        predict_time -= VISION_SYSTEM_LAG;
        predict_time_attack -= VISION_SYSTEM_LAG;
      }
    }
  }
  else // Puck is moving slowly or to the other side
  {
    predict_x_old = -1;
    predict_status = 0;
    predict_bounce = 0;
    predict_bounce_status = 0;
  }
}

int predictPuckXPosition(int predict_time)
{
  predict_time += VISION_SYSTEM_LAG;
  return (puckCoordX + (long)puckSpeedXAverage * predict_time / 100L);
}

int predictPuckYPosition(int predict_time)
{
  predict_time += VISION_SYSTEM_LAG;
  return (puckCoordY + (long)puckSpeedYAverage * predict_time / 100L);
}


void missingStepsDetection()
{
  int robot_position_x_mm;
  int robot_position_y_mm;


  
  if ((robotCoordX==0)||(robotCoordY==0))
    return;
  
  if ((myAbs(speed_M1) < 400) && (myAbs(speed_M2) < 400) && (robotCoordX < TABLE_WIDTH) && (robotCoordY < (TABLE_LENGTH / 2)))
  {
    if ((real_position_x  > (ROBOT_CENTER_X - 50)) && (real_position_x  < (ROBOT_CENTER_X + 50)) && (real_position_y  >= ROBOT_MIN_Y) && (real_position_y  < (ROBOT_DEFENSE_POSITION_DEFAULT + 60)))
    {
      robotCoordSamples++;
      robotCoordXAverage += robotCoordX;
      robotCoordYAverage += robotCoordY;
      // When we collect 10 samples we make the correction
      if (robotCoordSamples == 10)
      {
        // X axis
        robotCoordXAverage = robotCoordXAverage / robotCoordSamples;
        robotMissingStepsErrorX = myAbs(real_position_x - robotCoordXAverage);  // in milimeters)
        // Y AXIS
        robotCoordYAverage = robotCoordYAverage / robotCoordSamples;
        robot_position_y_mm += ROBOT_POSITION_CAMERA_CORRECTION_Y;   // correction because camera point of view and robot mark
        robotMissingStepsErrorY = myAbs(real_position_y - robotCoordYAverage);
        if ((robotMissingStepsErrorX > MISSING_STEPS_MAX_ERROR_X) || (robotMissingStepsErrorY > MISSING_STEPS_MAX_ERROR_Y))
        {
#ifdef CORRECT_MISSING_STEPS
          position_M1 = (robotCoordXAverage + robotCoordYAverage) * X_AXIS_STEPS_PER_UNIT;
          position_M2 = (robotCoordXAverage - robotCoordYAverage) * X_AXIS_STEPS_PER_UNIT;
          Serial.print("MSX:");
          Serial.println(robotMissingStepsErrorX);
          Serial.print("MSY:");
          Serial.println(robotMissingStepsErrorY);
          //max_acceleration = user_max_accel / 2;          
#endif
        }
        robotCoordSamples = 0; 
        robotCoordXAverage = 0;
        robotCoordYAverage = 0;
      }
    }
    else
    {
      robotCoordSamples = 0;
      robotCoordXAverage = 0;
      robotCoordYAverage = 0;
      robotMissingStepsErrorX = 0;
      robotMissingStepsErrorY = 0;
    }
  }
  else
  {
    robotCoordSamples = 0;
    robotCoordXAverage = 0;
    robotCoordYAverage = 0;
    robotMissingStepsErrorX = 0;
    robotMissingStepsErrorY = 0;
  }
}
