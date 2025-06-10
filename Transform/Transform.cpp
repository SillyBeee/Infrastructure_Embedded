#include "Transform.hpp"



Transform::Transform(Rotate rotate, Move move) {
    Transform_Matrix = Eigen::Matrix<double, 4, 4>::Identity();
    // 1. 复制旋转部分 (左上角的 3x3 块)
    // 从 rotate.Rotate_Matrix 的 (0,0) 开始的 3x3 块
    // 复制到 Transform_Matrix 的 (0,0) 开始的 3x3 块
    Transform_Matrix.block<3,3>(0,0) = rotate.Rotate_Matrix.block<3,3>(0,0);

    // 2. 复制平移部分 (最后一列的前三个元素，即一个 3x1 块)
    // 从 move.Move_Matrix 的 (0,3) 开始的 3x1 块
    // 复制到 Transform_Matrix 的 (0,3) 开始的 3x1 块
    Transform_Matrix.block<3,1>(0,3) = move.Move_Matrix.block<3,1>(0,3);

    // Transform_Matrix 的右下角元素 (3,3) 保持为 1 (来自 Identity())
    // Transform_Matrix 的最后一行前三个元素 (3,0), (3,1), (3,2) 保持为 0 (来自 Identity())
}

Transform::Transform(double x, double y, double z, double roll, double pitch, double yaw) {
    Move move(x, y, z);
    Rotate rotate(roll, pitch, yaw);
    Transform_Matrix = Transform(rotate, move).Transform_Matrix;
}


Move::Move(double x, double y, double z) {
    Move_Matrix = Eigen::Matrix<double, 4, 4>::Identity();
    Move_Matrix(0, 3) = x;
    Move_Matrix(1, 3) = y;
    Move_Matrix(2, 3) = z;
}

Rotate::Rotate(double roll, double pitch, double yaw) {
    Rotate_Matrix = Eigen::Matrix<double, 4, 4>::Identity();
    double cr = cos(roll);
    double sr = sin(roll);
    double cp = cos(pitch);
    double sp = sin(pitch);
    double cy = cos(yaw);
    double sy = sin(yaw);


    Eigen::Matrix<double, 4, 4> roll_rotate = Eigen::Matrix<double, 4, 4>::Identity();
    roll_rotate(1, 1) = cr;
    roll_rotate(1, 2) = -sr;
    roll_rotate(2, 1) = sr;
    roll_rotate(2, 2) = cr;

    Eigen::Matrix<double, 4, 4> pitch_rotate = Eigen::Matrix<double, 4, 4>::Identity();
    pitch_rotate(0, 0) = cp;
    pitch_rotate(0,2) = sp;
    pitch_rotate(2,0) = -sp;
    pitch_rotate(2,2) = cp;

    Eigen::Matrix<double, 4, 4> yaw_rotate = Eigen::Matrix<double, 4, 4>::Identity();
    yaw_rotate(0, 0) = cy;
    yaw_rotate(0, 1) = -sy;
    yaw_rotate(1, 0) = sy;
    yaw_rotate(1, 1) = cy;

    // Rotate = roll_rotate * pitch_rotate * yaw_rotate;
    Rotate_Matrix = yaw_rotate * pitch_rotate * roll_rotate; 
}