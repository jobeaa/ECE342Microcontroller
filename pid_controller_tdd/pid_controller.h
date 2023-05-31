#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  // PID term weights
  float proportion_gain;
  float integration_gain;
  float derivative_gain;

  float integral_limit_min;
  float integral_limit_max;

  float output_limit_min;
  float output_limit_max;

  float sample_time_seconds;

  // Controller "memory"
  float integrator;
  float previous_error; // Required for integrator
  float differentiator;
  float previous_measurement; // Required for differentiator
  float proportion;

} pid_controller_t;

void pid_controller_init(pid_controller_t *pid);
float pid_controller_update(pid_controller_t *pid, float setpoint,
                            float measurement);

#ifdef __cplusplus
}
#endif

#endif
