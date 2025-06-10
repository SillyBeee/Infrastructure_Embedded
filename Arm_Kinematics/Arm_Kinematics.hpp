#include "Eigen/Dense"
#include "Transform.hpp"
#include "iostream"


class DH_Transform{
    public:
        DH_Transform(double a, double alpha, double d, double theta);
        Eigen::Matrix<double, 4, 4> DH_Transform_Matrix;
};


inline Eigen::Matrix<double, 4, 4> DO_DH_Transform(std::vector<DH_Transform> &DH_Transform_Matrices) {
    
    Eigen::Matrix<double, 4, 4> result = Eigen::Matrix<double, 4, 4>::Identity();
    for (const auto& mat : DH_Transform_Matrices) {
        result *= mat.DH_Transform_Matrix;
    }
    return result;
}