#include "main.h"
#include "MPU6050.h"

bool mpu_init(void){
    //Initialises MPU and i2c connection, checks if connection is successful
    MPU6050_Initialize();
    MPU6050_i2c_init();

    if(MPU6050_TestConnection()){
        return true;
    } else {
        return false;
    }
}

void mpu_calibrate(){
    //Calibrates the MPU by setting the offsets for the accelerometer and gyroscope
    //This function should be called when the robot is in a stable position, not moving

    int16_t accel_data[3] = {0, 0, 0};
    int16_t gyro_data[3] = {0, 0, 0};

    for(int i = 0; i < 100; i++){
        mpu_read_data(accel_data, gyro_data);
        MPU6050_SetXAccelOffset(-accel_data[0]);
        MPU6050_SetYAccelOffset(-accel_data[1]);
        MPU6050_SetZAccelOffset(-accel_data[2]);
        MPU6050_SetXGyroOffset(-gyro_data[0]);
        MPU6050_SetYGyroOffset(-gyro_data[1]);
        MPU6050_SetZGyroOffset(-gyro_data[2]);
    }

}


void mpu_read_data(int16_t *accel_data, int16_t *gyro_data){
    //Reads data from MPU and stores it in the provided arrays
    //These axises should be checked and calibrated depending on the orientation of the MPU on the redoubot
    int16_t raw_data[6];

    MPU6050_GetRawAccelGyro(raw_data);
    accel_data[0] = raw_data[0]; // X-axis accelerometer data
    accel_data[1] = raw_data[1]; // Y-axis accelerometer data
    accel_data[2] = raw_data[2]; // Z-axis accelerometer data
    gyro_data[0] = raw_data[3];  // X-axis gyroscope data
    gyro_data[1] = raw_data[4];  // Y-axis gyroscope data
    gyro_data[2] = raw_data[5];  // Z-axis gyroscope data
}

void filter_data(int16_t *accel_data, int16_t *gyro_data, float *true_angle_rad){
    // Applies a simple low-pass filter to the accelerometer and gyroscope data given in the form of arrays
    // Returns the filtered data in the true_angle_rad array, which contains the angles in radians for each axis

    float Te = 10;          //sampling speed in ms
    float Tau = 1000;       //time constant in ms, may be incorrect

    float *filtered_accel;  //filtered accelerometer data
    float *filtered_gyro;   //filtered gyroscope data
    float *angle_rad;       //angle calculated from accelerometer data
    float *angle_radF;      //filtered angle calculated from accelerometer data

    float A, B;             //filter coefficients
    A = 1 / (1 + Tau / Te); 
    B = Tau / Te;


    for(int i = 0; i < 3; i++){
        gyro_data[i] = (gyro_data[i] * Tau) * 1e-3;
        filtered_gyro[i] = A * gyro_data[i] + B * filtered_gyro[i];

        angle_rad[i] = atan2(accel_data[i + 1], accel_data[i]) ; //angle Y, angle X, TODO Check angles //+offset if mpu calibrate doesnt work
        angle_radF[i] = A * (angle_rad[i] + B * angle_radF[i]);

        true_angle_rad[i] = angle_radF[i] + filtered_gyro[i]; //+offset
    }
}


