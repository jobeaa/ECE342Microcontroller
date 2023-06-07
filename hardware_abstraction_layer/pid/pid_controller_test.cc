#include <gtest/gtest.h>

#include "pid_controller.h"

class PIDTest : public testing::TestWithParam<float> {
public:
  pid_controller_t pid;

protected:
  PIDTest() {
    pid.proportion_gain = 0.2f;
    pid.integration_gain = 0.3f;
    pid.derivative_gain = 0.2f;

    pid.output_limit_max = 100;
    pid.output_limit_min = -100;

    pid.integral_limit_max = 100;
    pid.integral_limit_min = -100;

    pid.sample_time_ms = 1;

    pid_controller_init(&pid);
  }

  ~PIDTest() override {
    // You can do clean-up work that doesn't throw exceptions here.
  }
};

TEST_F(PIDTest, FORWARD_MOVE_NO_MOVEMENT_GOES_TO_MAX) {
  // let the pid controller accumulate
  float output;
  for (int i = 0; i < 1000; i++) {
    output = pid_controller_update(&pid, 20, 0);
  }

  EXPECT_FLOAT_EQ(output, pid.output_limit_max);
}

TEST_P(PIDTest, MOVE_TO_SETPOINT_APPROACHES_SET_POINT) {
  const unsigned int test_setpoint = GetParam();
  const unsigned int pid_update_count = 20;
  const unsigned int motor_maximum_magnitude = 1024;

  const unsigned int scalar_transfer_function = 1;

  float pid_output = 0;
  float measured_position = 0;
  float motor_output = 0;

  for (int i = 0; i < pid_update_count; i++) {
    measured_position = scalar_transfer_function*pid_output;
    pid_output = pid_controller_update(&pid, test_setpoint, measured_position);

    // DEBUGGING
    printf("measured_position: %f, set-point: %i, pid_output: %f, "
           "P_term: %f, I_term: %f, D_term: %f\n",
           measured_position, test_setpoint, pid_output, pid.proportion, pid.integrator,
           pid.differentiator);
  }
  FAIL();
  //EXPECT_NEAR(tracked_output, test_setpoint, (0.01f * test_setpoint));
}

INSTANTIATE_TEST_SUITE_P(Parametric_PID_Set_Point, PIDTest,
                         ::testing::Range(((float)5.0), ((float)100.0),
                                          ((float)5.0)));

TEST_F(PIDTest, MOVE_TO_SETPOINT_APPROACHES_SET_POINT_VARIABLE_SAMPLING) {
  const unsigned int pid_update_count = 20;
  const unsigned int test_setpoint = 10;

  const float sampling_rates[] = {1,   1.2, 1.4, 1.3, 0.8,
                                  0.5, 0.5, 0.4, 0.6, 0.9};

  float tracked_output = 0;

  for (int i = 0; i < pid_update_count; i++) {
    // DEBUGGING
    printf("position: %i, set-point: %i, output: %f, proportion: %f, "
           "integration: %f, derivative: %f\n",
           i, test_setpoint, tracked_output, pid.proportion, pid.integrator,
           pid.differentiator);

    pid.sample_time_ms = sampling_rates[i % 10];
    tracked_output = pid_controller_update(&pid, test_setpoint, tracked_output);
  }
  EXPECT_NEAR(tracked_output, test_setpoint, (0.01f * test_setpoint));
}

