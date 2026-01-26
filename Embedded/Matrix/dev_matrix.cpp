/**
*   @file dev_matrix.cpp
*   @brief 该文件是对SJTU的机器人库的整理,封装与拓展
*   @author HuaCheng Ma
*   @date 2026/1/20
*   @version 0.1
*   @note
*/

#include "dev_matrix.h"

//反对称矩阵实现
Matrixf<3, 3> vector3f::hat(Matrixf<3, 1> vec) {
    float hat[9] = {    0     , -vec[2][0],vec[1][0] ,
                     vec[2][0],     0     ,-vec[0][0],
                    -vec[1][0], vec[0][0] ,    0      };
    return {hat};
}

//向量叉积计算
Matrixf<3, 1> vector3f::cross(const Matrixf<3, 1>& vec1,  const Matrixf<3, 1>& vec2) {
    return vector3f::hat(vec1) * vec2;
}



float math::limit(float val, const float& min, const float& max) {
    if (min > max)
        return val;
    else if (val < min)
        val = min;
    else if (val > max)
        val = max;
    return val;
}

float math::limitMin(float val, const float& min) {
    if (val < min)
        val = min;
    return val;
}

float math::limitMax(float val, const float& max) {
    if (val > max)
        val = max;
    return val;
}

float math::loopLimit(float val, const float& min, const float& max) {
    if (min >= max)
        return val;
    if (val > max) {
        while (val > max)
            val -= (max - min);
    } else if (val < min) {
        while (val < min)
            val += (max - min);
    }
    return val;
}

float math::sign(const float& val) {
    if (val > 0)
        return 1;
    else if (val < 0)
        return -1;
    return 0;
}
