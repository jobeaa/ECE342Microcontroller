#include "pid_controller.h"

void pid_controller_init(pid_controller_t *pid) {
  // Clear controller variables
  pid->proportion = 0.0f;
  pid->integrator = 0.0f;
  pid->previous_error = 0.0f;
  pid->differentiator = 0.0f;
  pid->previous_measurement = 0.0f;
}

float pid_controller_update(pid_controller_t *pid, float setpoint,
                            float measurement) {
  float error = setpoint - measurement;

  pid->proportion = pid->proportion_gain * error;

  // https://en.wikipedia.org/wiki/Riemann_sum#Midpoint_rule
  pid->integrator =
      pid->integrator + pid->integration_gain * pid->sample_time_seconds *
                            (pid->previous_error + error) * (0.5f);
  // Anti-wind-up via integrator clamping
  if (pid->integrator > pid->integral_limit_max) {
    pid->integrator = pid->integral_limit_max;
  } else if (pid->integrator < pid->integral_limit_min) {
    pid->integrator = pid->integral_limit_min;
  }
  pid->previous_error = error;

//  pid->differentiator =
//      pid->derivative_gain *
//      ((measurement - pid->previous_measurement) / (pid->sample_time_seconds));
//  pid->previous_measurement = measurement;

  float output = pid->proportion + pid->integrator + pid->differentiator;
  if (output > pid->output_limit_max) {
    output = pid->output_limit_max;
  } else if (output < pid->output_limit_min) {
    output = pid->output_limit_min;
  }
  return output;
}
