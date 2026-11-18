//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Proportional-integral-derivative (PID) controller
//By G Vid
#include "main.h"
#include "pid.h"
#include <timers.h>

float pid_speed(float setpoint, float measurement)
{
  float sampleTime = 10;
  float timeConstant = 1000;
  float errorThreshold = 0.1;

  float Kp = ;
  float Ki = ;
  float Kd = ;

  static float previousError = 0;
  static float errorSum = 0;
  
  float error = setpoint - measurement;
  errorSum += error * sampleTime / timeConstant;
  
  float derivative = (error - previousError) / (sampleTime / timeConstant);
  previousError = error;

  return Kp * error + Ki * errorSum + Kd * derivative;
}

float pid_distance(float setpoint, float measurement, float errorThreshold)
{
  float sampleTime = 10;
  float timeConstant = 1000;

  float Kp = ;
  float Ki = ;
  float Kd = ;

  static float previousError = 0;
  static float errorSum = 0;
  
  float error = setpoint - measurement;
  errorSum += error * sampleTime / timeConstant;
  
  float derivative = (error - previousError) / (sampleTime / timeConstant);
  previousError = error;

  FLAG_TARGET_REACHED_distance = pid_check_threshold(error, errorThreshold, 100);
  
  return Kp * error + Ki * errorSum + Kd * derivative;
}

float pid_angle(float setpoint, float measurement, float errorThreshold)
{
  float sampleTime = 10;
  float timeConstant = 1000;

  float Kp = ;
  float Ki = ;
  float Kd = ;

  static float previousError = 0;
  static float errorSum = 0;
  
  float error = setpoint - measurement;
  errorSum += error * sampleTime / timeConstant;
  
  float derivative = (error - previousError) / (sampleTime / timeConstant);
  previousError = error;

  FLAG_TARGET_REACHED_angle = pid_check_threshold(error, errorThreshold, 100);
  
  return Kp * error + Ki * errorSum + Kd * derivative;
}

int pid_check_threshold(float error, float threshold, int time_interval_ms)
{
  //elseif error greater than thresh stop and reset timer
  //elseif timer greater than 100ms, raise flag FLAG_TARGET_REACHED
  //elseif error is less than thresh, start timer

  if(abs(error) > errorThreshold)
  {
    start_tick = xGetTickCount()
  }
  else if (start_tick + time_interval_ms*tick_rate_ms < xGetTickCount())
  {
    return 1; // FLAG_TARGET_REACHED
  }
  else if (abs(error) < errorThreshold)
  {
    static int start_tick = xGetTickCount() 
  }
  return 0; //no flag raised
}