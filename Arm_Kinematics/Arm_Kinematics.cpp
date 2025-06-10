#include  "Arm_Kinematics.hpp"

DH_Transform::DH_Transform(double a, double alpha, double d, double theta) {
    // Initialize the transformation matrix as an identity matrix
    DH_Transform_Matrix = Eigen::Matrix<double, 4, 4>::Identity();

    // Set the rotation and translation components based on the DH parameters
    DH_Transform_Matrix(0, 0) = cos(theta);
    DH_Transform_Matrix(0, 1) = -sin(theta) * cos(alpha);
    DH_Transform_Matrix(0, 2) = sin(theta) * sin(alpha);
    DH_Transform_Matrix(0, 3) = a * cos(theta);

    DH_Transform_Matrix(1, 0) = sin(theta);
    DH_Transform_Matrix(1, 1) = cos(theta) * cos(alpha);
    DH_Transform_Matrix(1, 2) = -cos(theta) * sin(alpha);
    DH_Transform_Matrix(1, 3) = a * sin(theta);

    DH_Transform_Matrix(2, 1) = sin(alpha);
    DH_Transform_Matrix(2, 2) = cos(alpha);
    DH_Transform_Matrix(2, 3) = d;


    // std::cout<<"DH_Transform_Matrix\n"<<DH_Transform_Matrix<<std::endl;

    // std::cout<<"Transform_Matrix\n"<<Transform(a,0,d,alpha,0,theta).Transform_Matrix<<std::endl;
}

int main()
{
    std::vector<DH_Transform> DH_Transform_Matrices;
    DH_Transform_Matrices.emplace_back(0.5, 0, 0.3, M_PI / 4);
    DH_Transform_Matrices.emplace_back(0.5, 0, 0.3, M_PI / 4);
    std::cout<< DO_DH_Transform(DH_Transform_Matrices) << std::endl;
    return 0;
}