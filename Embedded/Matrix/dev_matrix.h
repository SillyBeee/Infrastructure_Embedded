/**
*   @file dev_matrix.h
*   @brief  该文件是对SJTU的机器人库的整理,封装与拓展
*   @author HuaCheng Ma
*   @date 2026/1/20
*   @version 0.1
*   @note
*/

#ifndef DEV_MATRIX_H
#define DEV_MATRIX_H
#include "arm_math.h"

//矩阵模板类
template <int _rows, int _cols>
class Matrixf {
 public:
  //获取矩阵行数
  int rows() { return _rows; }
  //获取矩阵列数
  int cols() { return _cols; }

  //获取指定行数据
  float* operator[](const int& row) { return &this->data_[row * _cols]; }

  //运算符重载
  Matrixf<_rows, _cols>& operator=(const Matrixf<_rows, _cols> mat) {
    memcpy(this->data_, mat.data_, _rows * _cols * sizeof(float));
    return *this;
  }

  Matrixf<_rows, _cols>& operator+=(const Matrixf<_rows, _cols> mat) {
    // arm_status s;
    // s = arm_mat_add_f32(&this->arm_mat_, &mat.arm_mat_, &this->arm_mat_);
    arm_mat_add_f32(&this->arm_mat_, &mat.arm_mat_, &this->arm_mat_);
    return *this;
  }

  Matrixf<_rows, _cols>& operator-=(const Matrixf<_rows, _cols> mat) {
    // arm_status s;
    // s = arm_mat_sub_f32(&this->arm_mat_, &mat.arm_mat_, &this->arm_mat_);
    arm_mat_sub_f32(&this->arm_mat_, &mat.arm_mat_, &this->arm_mat_);
    return *this;
  }

  Matrixf<_rows, _cols>& operator*=(const float& val) {
    // arm_status s;
    // s = arm_mat_scale_f32(&this->arm_mat_, val, &this->arm_mat_);
    arm_mat_scale_f32(&this->arm_mat_, val, &this->arm_mat_);
    return *this;
  }

  Matrixf<_rows, _cols>& operator/=(const float& val) {
    // arm_status s;
    // s = arm_mat_scale_f32(&this->arm_mat_, 1.f / val, &this->arm_mat_);
    arm_mat_scale_f32(&this->arm_mat_, 1.f / val, &this->arm_mat_);
    return *this;
  }

  Matrixf<_rows, _cols> operator+(const Matrixf<_rows, _cols>& mat) {
    // arm_status s;
    Matrixf<_rows, _cols> res;
    // s = arm_mat_add_f32(&this->arm_mat_, &mat.arm_mat_, &res.arm_mat_);
    arm_mat_add_f32(&this->arm_mat_, &mat.arm_mat_, &res.arm_mat_);
    return res;
  }
  Matrixf<_rows, _cols> operator-(const Matrixf<_rows, _cols>& mat) {
    // arm_status s;
    Matrixf<_rows, _cols> res;
    // s = arm_mat_sub_f32(&this->arm_mat_, &mat.arm_mat_, &res.arm_mat_);
    arm_mat_sub_f32(&this->arm_mat_, &mat.arm_mat_, &res.arm_mat_);
    return res;
  }
  Matrixf<_rows, _cols> operator*(const float& val) {
    // arm_status s;
    Matrixf<_rows, _cols> res;
    // s = arm_mat_scale_f32(&this->arm_mat_, val, &res.arm_mat_);
    arm_mat_scale_f32(&this->arm_mat_, val, &res.arm_mat_);
    return res;
  }

  friend Matrixf<_rows, _cols> operator*(const float& val,
                                         const Matrixf<_rows, _cols>& mat) {
    // arm_status s;
    Matrixf<_rows, _cols> res;
    // s = arm_mat_scale_f32(&mat.arm_mat_, val, &res.arm_mat_);
    arm_mat_scale_f32(&mat.arm_mat_, val, &res.arm_mat_);
    return res;
  }

  Matrixf<_rows, _cols> operator/(const float& val) {
    // arm_status s;
    Matrixf<_rows, _cols> res;
    // s = arm_mat_scale_f32(&this->arm_mat_, 1.f / val, &res.arm_mat_);
    arm_mat_scale_f32(&this->arm_mat_, 1.f / val, &res.arm_mat_);
    return res;
  }

  //矩阵乘法
  template <int cols>
  friend Matrixf<_rows, cols> operator*(const Matrixf<_rows, _cols>& mat1,
                                        const Matrixf<_cols, cols>& mat2) {
    // arm_status s;
    Matrixf<_rows, cols> res;
    // s = arm_mat_mult_f32(&mat1.arm_mat_, &mat2.arm_mat_, &res.arm_mat_);
    arm_mat_mult_f32(&mat1.arm_mat_, &mat2.arm_mat_, &res.arm_mat_);
    return res;
  }

  //子矩阵
  template <int rows, int cols>
  Matrixf<rows, cols> block(const int& start_row, const int& start_col) {
    Matrixf<rows, cols> res;
    for (int row = start_row; row < start_row + rows; row++) {
      memcpy((float*)res[0] + (row - start_row) * cols,
             (float*)this->data_ + row * _cols + start_col,
             cols * sizeof(float));
    }
    return res;
  }

  //获取指定行
  Matrixf<1, _cols> row(const int& row) { return block<1, _cols>(row, 0); }
  //获取指定列
  Matrixf<_rows, 1> col(const int& col) { return block<_rows, 1>(0, col); }

  //转置
  Matrixf<_cols, _rows> trans() {
    Matrixf<_cols, _rows> res;
    arm_mat_trans_f32(&arm_mat_, &res.arm_mat_);
    return res;
  }

  //迹
  float trace() {
    float res = 0;
    for (int i = 0; i < fmin(_rows, _cols); i++) {
      res += (*this)[i][i];
    }
    return res;
  }

  //L2范数
  float norm() { return sqrtf((this->trans() * *this)[0][0]); }


  //无输入构造
  Matrixf() : rows_(_rows), cols_(_cols) {
    arm_mat_init_f32(&this->arm_mat_, _rows, _cols, this->data_);
  }
  //含输入构造
  Matrixf(float data[_rows * _cols]) : Matrixf() {
    memcpy(this->data_, data, _rows * _cols * sizeof(float));
  }
  //拷贝构造
  Matrixf(const Matrixf<_rows, _cols>& mat) : Matrixf() {
    memcpy(this->data_, mat.data_, _rows * _cols * sizeof(float));
  }
  //析构
  ~Matrixf(void) {}

public:
  //DSP库封装的矩阵结构
  arm_matrix_instance_f32 arm_mat_;
protected:
  //矩阵大小
  int rows_, cols_;
  //矩阵内部数据
  float data_[_rows * _cols];
};


// Matrix相关函数
namespace matrixf {

//特殊矩阵

//全零矩阵
template <int _rows, int _cols>
Matrixf<_rows, _cols> zeros() {
  float data[_rows * _cols] = {0};
  return Matrixf<_rows, _cols>(data);
}

//全一矩阵
template <int _rows, int _cols>
Matrixf<_rows, _cols> ones() {
  float data[_rows * _cols] = {0};
  for (int i = 0; i < _rows * _cols; i++) {
    data[i] = 1;
  }
  return Matrixf<_rows, _cols>(data);
}

//单位矩阵
template <int _rows, int _cols>
Matrixf<_rows, _cols> eye() {
  float data[_rows * _cols] = {0};
  for (int i = 0; i < fmin(_rows, _cols); i++) {
    data[i * _cols + i] = 1;
  }
  return Matrixf<_rows, _cols>(data);
}

//对角矩阵
template <int _rows, int _cols>
Matrixf<_rows, _cols> diag(Matrixf<_rows, 1> vec) {
  Matrixf<_rows, _cols> res = matrixf::zeros<_rows, _cols>();
  for (int i = 0; i < fmin(_rows, _cols); i++) {
    res[i][i] = vec[i][0];
  }
  return res;
}

//矩阵求逆
template <int _dim>
Matrixf<_dim, _dim> inv(Matrixf<_dim, _dim> mat) {
  // extended matrix [A|I]
  Matrixf<_dim, 2 * _dim> ext_mat = matrixf::zeros<_dim, 2 * _dim>();
  for (int i = 0; i < _dim; i++) {
    memcpy(ext_mat[i], mat[i], _dim * sizeof(float));
    ext_mat[i][_dim + i] = 1;
  }
  // elimination
  for (int i = 0; i < _dim; i++) {
    // find maximum absolute value in the first column in lower right block
    float abs_max = fabs(ext_mat[i][i]);
    int abs_max_row = i;
    for (int row = i; row < _dim; row++) {
      if (abs_max < fabs(ext_mat[row][i])) {
        abs_max = fabs(ext_mat[row][i]);
        abs_max_row = row;
      }
    }
    if (abs_max < 1e-12f) {  // singular
      return matrixf::zeros<_dim, _dim>();
    }
    if (abs_max_row != i) {  // row exchange
      float tmp;
      Matrixf<1, 2 * _dim> row_i = ext_mat.row(i);
      Matrixf<1, 2 * _dim> row_abs_max = ext_mat.row(abs_max_row);
      memcpy(ext_mat[i], row_abs_max[0], 2 * _dim * sizeof(float));
      memcpy(ext_mat[abs_max_row], row_i[0], 2 * _dim * sizeof(float));
    }
    float k = 1.f / ext_mat[i][i];
    for (int col = i; col < 2 * _dim; col++) {
      ext_mat[i][col] *= k;
    }
    for (int row = 0; row < _dim; row++) {
      if (row == i) {
        continue;
      }
      k = ext_mat[row][i];
      for (int j = i; j < 2 * _dim; j++) {
        ext_mat[row][j] -= k * ext_mat[i][j];
      }
    }
  }
  // inv = ext_mat(:,n+1:2n)

  Matrixf<_dim, _dim> res;
  for (int i = 0; i < _dim; i++) {
    memcpy(res[i], &ext_mat[i][_dim], _dim * sizeof(float));
  }
  return res;
}

}// namespace matrixf



namespace vector3f {

//反对称矩阵
Matrixf<3, 3> hat(Matrixf<3, 1> vec);

//向量叉积计算
Matrixf<3, 1> cross(const Matrixf<3, 1>& vec1, const Matrixf<3, 1>& vec2);

} // namespace vector3f


namespace math {
  float limit(float val, const float& min, const float& max);
  float limitMin(float val, const float& min);
  float limitMax(float val, const float& max);
  float loopLimit(float val, const float& min, const float& max);
  float sign(const float& val);
}  // namespace math

#endif //DEV_MATRIX_H