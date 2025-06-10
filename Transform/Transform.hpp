#include <Eigen/Dense>

class Rotate{
    public:
        Rotate(double roll  , double pitch , double yaw);
        Eigen::Matrix<double,4,4> Rotate_Matrix;
};

class Move{
    public:
        Move(double x, double y, double z);
        Eigen::Matrix<double,4,4> Move_Matrix;
};

class Transform{
    public:
        Transform(Rotate rotate, Move move); 
        Transform(double x, double y, double z , double roll, double pitch, double yaw);

        Eigen::Matrix<double,4,4> Transform_Matrix;
};




