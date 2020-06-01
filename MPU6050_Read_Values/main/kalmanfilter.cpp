#include "kalmanfilter.hpp"

KALMAN::KALMAN(float dt) {
    this -> dt = dt;
}

float KALMAN::filter(float accel, float gyro) {
    angle += (gyro - q_bias) * dt;
    angle_err = accel - angle;

    pdot[0] = q_angle - pp[0][1] - pp[1][0];
    pdot[1] = -pp[1][1];
    pdot[2] = -pp[1][1];
    pdot[3] = q_gyro;
    pp[0][0] += pdot[0] * dt;
    pp[0][1] += pdot[1] * dt;
    pp[1][0] += pdot[2] * dt;
    pp[1][1] += pdot[3] * dt;

    pct_0 = c_0 * pp[0][0];
    pct_1 = c_0 * pp[1][0];

    e = r_angle + c_0 * pct_0;

    k_0 = pct_0 / e;
    k_1 = pct_1 / e;

    t_0 = pct_0;
    t_1 = c_0 * pp[0][1];

    pp[0][0] -= k_0 * t_0;
    pp[0][1] -= k_0 * t_1;
    pp[1][0] -= k_1 * t_0;
    pp[1][1] -= k_1 * t_1;

    angle += k_0 * angle_err;
    q_bias += k_1 * angle_err;
    gyro_y= gyro - q_bias;
    
    return angle;
}
