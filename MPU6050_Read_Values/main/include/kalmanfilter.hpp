#pragma once

class KALMAN {
private:
    float gyro_y;
    float angle = 0;
    float q_bias = 0;
    float angle_err = 0;
    float q_angle = 0.1;
    float q_gyro = 0.1;
    float r_angle = 0.5;
    float dt = 0.005;
    char c_0 = 1;
    float pct_0=0, pct_1=0, e=0;
    float k_0=0, k_1=0, t_0=0, t_1=0;
    float pdot[4] = {0, 0, 0, 0};
    float pp[2][2] = {{1, 0}, {0, 1}};
public:
    KALMAN(float dt);
    float filter(float accel, float gyro);
};
