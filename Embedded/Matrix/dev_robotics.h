/**
*   @file dev_robotics.h
*   @brief 该文件是对SJTU的机器人库的整理,封装与拓展
*   @author HuaCheng Ma
*   @date 2026/1/20
*   @version 0.1
*   @note
*/


#ifndef DEV_ROBOTICS_H
#define DEV_ROBOTICS_H
#include "dev_matrix.h"

namespace robotics {
//旋转矩阵->rpy  rotation matrix(R) -> RPY([yaw;pitch;roll])
Matrixf<3, 1> r2rpy(Matrixf<3, 3> R);
//rpy->旋转矩阵 RPY([yaw;pitch;roll]) -> rotation matrix(R)
Matrixf<3, 3> rpy2r(Matrixf<3, 1> rpy);
//旋转矩阵->轴角 rotation matrix(R) -> angle vector([r;θ])
Matrixf<4, 1> r2angvec(Matrixf<3, 3> R);
//轴角->旋转矩阵 angle vector([r;θ]) -> rotation matrix(R)
Matrixf<3, 3> angvec2r(Matrixf<4, 1> angvec);
//旋转矩阵->四元数 rotation matrix(R) -> quaternion, [q0;q1;q2;q3]=[cos(θ/2);rsin(θ/2)]
Matrixf<4, 1> r2quat(Matrixf<3, 3> R);
//四元数->旋转矩阵 quaternion, [q0;q1;q2;q3]=[cos(θ/2);rsin(θ/2)] -> rotation matrix(R)
Matrixf<3, 3> quat2r(Matrixf<4, 1> quat);
//四元数->rpy quaternion, [q0;q1;q2;q3]=[cos(θ/2);rsin(θ/2)] -> RPY([yaw;pitch;roll])
Matrixf<3, 1> quat2rpy(Matrixf<4, 1> q);
//rpy->四元数 RPY([yaw;pitch;roll]) -> quaternion, [q0;q1;q2;q3]=[cos(θ/2);rsin(θ/2)]
Matrixf<4, 1> rpy2quat(Matrixf<3, 1> rpy);
//四元数->轴角 quaternion, [q0;q1;q2;q3]=[cos(θ/2);rsin(θ/2)] -> angle vector([r;θ])
Matrixf<4, 1> quat2angvec(Matrixf<4, 1> q);
//轴角->四元数 angle vector([r;θ]) -> quaternion, [q0;q1;q2;q3]=[cos(θ/2);rsin(θ/2)]
Matrixf<4, 1> angvec2quat(Matrixf<4, 1> angvec);
//齐次变换矩阵->旋转矩阵 homogeneous transformation matrix(T) -> rotation matrix(R)
Matrixf<3, 3> t2r(Matrixf<4, 4> T);
//齐次变换矩阵->旋转矩阵 rotation matrix(R) -> homogeneous transformation matrix(T)
Matrixf<4, 4> r2t(Matrixf<3, 3> R);
//齐次变换矩阵->平移向量 homogeneous transformation matrix(T) -> translation vector(p)
Matrixf<3, 1> t2p(Matrixf<4, 4> T);

//平移向量->齐次变换矩阵 translation vector(p) -> homogeneous transformation matrix(T)
Matrixf<4, 4> p2t(Matrixf<3, 1> p);

//平移加旋转->齐次变换矩阵 rotation matrix(R) & translation vector(p) -> homogeneous transformation
Matrixf<4, 4> rp2t(Matrixf<3, 3> R, Matrixf<3, 1> p);
//齐次变换矩阵->rpy homogeneous transformation matrix(T) -> RPY([yaw;pitch;roll])
Matrixf<3, 1> t2rpy(Matrixf<4, 4> T);
//齐次变换矩阵求逆 inverse of homogeneous transformation matrix(T^-1=[R',-R'P;0,1])
Matrixf<4, 4> invT(Matrixf<4, 4> T);
//rpy->齐次变换矩阵 RPY([yaw;pitch;roll]) -> homogeneous transformation matrix(T)
Matrixf<4, 4> rpy2t(Matrixf<3, 1> rpy);
//齐次变换矩阵->角度向量 homogeneous transformation matrix(T) -> angle vector([r;θ])
Matrixf<4, 1> t2angvec(Matrixf<4, 4> T);
//角度向量->齐次变换矩阵 angle vector([r;θ]) -> homogeneous transformation matrix(T)
Matrixf<4, 4> angvec2t(Matrixf<4, 1> angvec);
//齐次变换矩阵->四元数 homogeneous transformation matrix(T) -> quaternion,[q0;q1;q2;q3]=[cos(θ/2);rsin(θ/2)]
Matrixf<4, 1> t2quat(Matrixf<4, 4> T);
//四元数->齐次变换矩阵 quaternion, [q0;q1;q2;q3]=[cos(θ/2);rsin(θ/2)] -> homogeneous transformation
Matrixf<4, 4> quat2t(Matrixf<4, 1> quat);
//齐次变换矩阵->twist坐标向量 homogeneous transformation matrix(T) -> twist coordinates vector ([p;rθ])
Matrixf<6, 1> t2twist(Matrixf<4, 4> T);
//twist坐标向量->齐次变换矩阵 twist coordinates vector ([p;rθ]) -> homogeneous transformation matrix(T)
Matrixf<4, 4> twist2t(Matrixf<6, 1> twist);



//关节类型: 旋转关节(R-revolute joint), 滑动关节(P-prismatic joint)
typedef enum joint_type {
  R = 0,
  P = 1,
} Joint_Type_e;

//DH法 Denavit–Hartenberg(DH) method
struct DH_t {
  //正向运动学解算 forward kinematic
  Matrixf<4, 4> fkine();
  //DH参数 DH parameter
  float theta;
  float d;
  float a;
  float alpha;
  Matrixf<4, 4> T;
};

//连杆类
class Link {
 public:
  Link()= default;
  Link(float theta, float d, float a, float alpha, Joint_Type_e type = R,
       float offset = 0, float qmin = 0, float qmax = 0, float m = 1,
       Matrixf<3, 1> rc = matrixf::zeros<3, 1>(),
       Matrixf<3, 3> I = matrixf::zeros<3, 3>());
  Link(const Link& link);

  Link& operator=(Link link);

  //获取关节限位
  float qmin() { return qmin_; }
  float qmax() { return qmax_; }
  //获取关节类型
  Joint_Type_e type() { return type_; }
  //获取动力学参数
  float m() { return m_; }
  Matrixf<3, 1> rc() { return rc_; }
  Matrixf<3, 3> I() { return I_; }
  //获取当前连杆末端的齐次变换矩阵(正运动学解算)
  Matrixf<4, 4> T(float q);

 public:
  //运动学参数
  DH_t dh_;
  float offset_;
  //关节限位  limit(qmin,qmax), no limit if qmin<=qmax
  float qmin_;
  float qmax_;
  //关节类型
  Joint_Type_e type_;
  //动力学参数
  float m_;           //连杆质量 mass
  Matrixf<3, 1> rc_;  //连杆质心在连杆局部坐标系下的坐标 centroid(link coordinate)
  Matrixf<3, 3> I_;   //连杆在连杆坐标系下的惯性张量 inertia tensor(3*3)
};


//模板类,用于描述串联的一个关节组(机械臂)
template <uint16_t _n = 1>
class Serial_Link {
 public:
  Serial_Link(Link links[_n]) {
    for (int i = 0; i < _n; i++)
      links_[i] = links[i];
    gravity_ = matrixf::zeros<3, 1>();
    gravity_[2][0] = -9.81f;
  }

  Serial_Link(Link links[_n], Matrixf<3, 1> gravity) {
    for (int i = 0; i < _n; i++)
      links_[i] = links[i];
    gravity_ = gravity;
  }

  /**
   * @brief 机械臂正运动学求解
   * @param q  当前所有关节角度构成的向量
   * @return 末端齐次变换矩阵(相对于基坐标系)
   */
  Matrixf<4, 4> fkine(Matrixf<_n, 1> q) {
    T_ = matrixf::eye<4, 4>();
    for (int iminus1 = 0; iminus1 < _n; iminus1++)
      T_ = T_ * links_[iminus1].T(q[iminus1][0]);
    return T_;
  }


  /**
   * @brief 对机械臂指定关节进行正运动学求解
   * @param q  当前所有关节角度构成的向量
   * @param k  所需的关节序号
   * @return 指定关节相对于基坐标系的齐次变换矩阵
   */
  Matrixf<4, 4> fkine(Matrixf<_n, 1> q, uint16_t k) {
    if (k > _n)
      k = _n;
    Matrixf<4, 4> T = matrixf::eye<4, 4>();
    for (int iminus1 = 0; iminus1 < k; iminus1++)
      T = T * links_[iminus1].T(q[iminus1][0]);
    return T;
  }

  /**
   * @brief 对机械臂指定关节进行正运动学求解(关节相对于前一关节的齐次变换矩阵)
   * @param q 当前所有关节角度构成的向量
   * @param kminus1 所需的关节序号-1
   * @return 指定关节相对于前一关节的齐次变换矩阵
   */
  Matrixf<4, 4> T(Matrixf<_n, 1> q, uint16_t kminus1) {
    if (kminus1 >= _n)
      kminus1 = _n - 1;
    return links_[kminus1].T(q[kminus1][0]);
  }


  /**
   * @brief 计算机械臂末端雅可比矩阵(即关节速度与末端速度的映射矩阵)
   * @param q 当前所有关节角度构成的向量
   * @return 机械臂末端雅可比矩阵 上3行表示线速度部分,下3行表示角速度部分
   */
  Matrixf<6, _n> jacob(Matrixf<_n, 1> q) {
    Matrixf<3, 1> p_e = t2p(fkine(q));               // p_e
    Matrixf<4, 4> T_iminus1 = matrixf::eye<4, 4>();  // T_i-1^0
    Matrixf<3, 1> z_iminus1;                         // z_i-1^0
    Matrixf<3, 1> p_iminus1;                         // p_i-1^0
    Matrixf<3, 1> J_pi;
    Matrixf<3, 1> J_oi;
    for (int iminus1 = 0; iminus1 < _n; iminus1++) {
      // revolute joint: J_pi = z_i-1x(p_e-p_i-1), J_oi = z_i-1
      if (links_[iminus1].type() == R) {
        z_iminus1 = T_iminus1.block<3, 1>(0, 2);
        p_iminus1 = t2p(T_iminus1);
        T_iminus1 = T_iminus1 * links_[iminus1].T(q[iminus1][0]);
        J_pi = vector3f::cross(z_iminus1, p_e - p_iminus1);
        J_oi = z_iminus1;
      }
      // prismatic joint: J_pi = z_i-1, J_oi = 0
      else {
        z_iminus1 = T_iminus1.block<3, 1>(0, 2);
        T_iminus1 = T_iminus1 * links_[iminus1].T(q[iminus1][0]);
        J_pi = z_iminus1;
        J_oi = matrixf::zeros<3, 1>();
      }
      J_[0][iminus1] = J_pi[0][0];
      J_[1][iminus1] = J_pi[1][0];
      J_[2][iminus1] = J_pi[2][0];
      J_[3][iminus1] = J_oi[0][0];
      J_[4][iminus1] = J_oi[1][0];
      J_[5][iminus1] = J_oi[2][0];
    }
    return J_;
  }


  /**
   * @brief 机械臂逆运动学求解
   * @param Td 末端齐次变换矩阵目标值
   * @param q 初始关节变量向量(牛顿法迭代初值)
   * @param tol 误差容限(扭转向量误差的范数)
   * @param max_iter 最大迭代次数，默认50
   * @return 关节变量向量
   */
  Matrixf<_n, 1> ikine(Matrixf<4, 4> Td,
                       Matrixf<_n, 1> q = matrixf::zeros<_n, 1>(),
                       float tol = 1e-4f, uint16_t max_iter = 50) {
    Matrixf<4, 4> T;
    Matrixf<3, 1> pe, we;
    Matrixf<6, 1> err, new_err;
    Matrixf<_n, 1> dq;
    float step = 1;
    for (int i = 0; i < max_iter; i++) {
      T = fkine(q);
      pe = t2p(Td) - t2p(T);
      // angvec(Td*T^-1), transform angular vector(T->Td) in world coordinate
      we = t2twist(Td * invT(T)).block<3, 1>(3, 0);
      for (int i = 0; i < 3; i++) {
        err[i][0] = pe[i][0];
        err[i + 3][0] = we[i][0];
      }
      if (err.norm() < tol)
        return q;
      // adjust iteration step
      Matrixf<6, _n> J = jacob(q);
      for (int j = 0; j < 5; j++) {
        dq = matrixf::inv(J.trans() * J) * (J.trans() * err) * step;
        if (dq[0][0] == INFINITY)  // J'*J singular
        {
          dq = matrixf::inv(J.trans() * J + 0.1f * matrixf::eye<_n, _n>()) *
               J.trans() * err * step;
          // SVD<6, _n> JTJ_svd(J.trans() * J);
          // dq = JTJ_svd.solve(err) * step * 5e-2f;
          q += dq;
          for (int i = 0; i < _n; i++) {
            if (links_[i].type() == R)
              q[i][0] = math::loopLimit(q[i][0], -PI, PI);
          }
          break;
        }
        T = fkine(q + dq);
        pe = t2p(Td) - t2p(T);
        we = t2twist(Td * invT(T)).block<3, 1>(3, 0);
        for (int i = 0; i < 3; i++) {
          new_err[i][0] = pe[i][0];
          new_err[i + 3][0] = we[i][0];
        }
        if (new_err.norm() < err.norm()) {
          q += dq;
          for (int i = 0; i < _n; i++) {
            if (links_[i].type() == robotics::Joint_Type_e::R) {
              q[i][0] = math::loopLimit(q[i][0], -PI, PI);
            }
          }
          break;
        } else {
          step /= 2.0f;
        }
      }
      if (step < 1e-3f)
        return q;
    }
    return q;
  }


  //(保留功能)运动学逆解,解析解(几何法)
  Matrixf<_n, 1> (*ikine_analytic)(Matrixf<4, 4> T);



  /**
   * @brief 机械臂逆动力学求解(牛顿-欧拉法)
   * @param q 当前关节角度向量
   * @param qv 当前关节速度向量
   * @param qa 当前关节加速度向量
   * @param he 末端负荷向量([Fx;Fy;Fz;Mx;My;Mz])
   * @return
   */
  Matrixf<_n, 1> rne(Matrixf<_n, 1> q,
                     Matrixf<_n, 1> qv = matrixf::zeros<_n, 1>(),
                     Matrixf<_n, 1> qa = matrixf::zeros<_n, 1>(),
                     Matrixf<6, 1> he = matrixf::zeros<6, 1>()) {
    // forward propagation
    // record each links' motion state in matrices
    // [ωi] angular velocity
    Matrixf<3, _n + 1> w = matrixf::zeros<3, _n + 1>();
    // [βi] angular acceleration
    Matrixf<3, _n + 1> b = matrixf::zeros<3, _n + 1>();
    // [pi] position of joint
    Matrixf<3, _n + 1> p = matrixf::zeros<3, _n + 1>();
    // [vi] velocity of joint
    Matrixf<3, _n + 1> v = matrixf::zeros<3, _n + 1>();
    // [ai] acceleration of joint
    Matrixf<3, _n + 1> a = matrixf::zeros<3, _n + 1>();
    // [aci] acceleration of mass center
    Matrixf<3, _n + 1> ac = matrixf::zeros<3, _n + 1>();
    // temperary vectors
    Matrixf<3, 1> w_i, b_i, p_i, v_i, ai, ac_i;
    // i & i-1 coordinate convert to 0 coordinate
    Matrixf<4, 4> T_0i = matrixf::eye<4, 4>();
    Matrixf<4, 4> T_0iminus1 = matrixf::eye<4, 4>();
    Matrixf<3, 3> R_0i = matrixf::eye<3, 3>();
    Matrixf<3, 3> R_0iminus1 = matrixf::eye<3, 3>();
    // unit vector of z-axis
    Matrixf<3, 1> ez = matrixf::zeros<3, 1>();
    ez[2][0] = 1;

    for (int i = 1; i <= _n; i++) {
      T_0i = T_0i * T(q, i - 1);     // T_i^0
      R_0i = t2r(T_0i);              // R_i^0
      R_0iminus1 = t2r(T_0iminus1);  // R_i-1^0
      // ω_i = ω_i-1+qv_i*R_i-1^0*ez
      w_i = w.col(i - 1) + qv[i - 1][0] * R_0iminus1 * ez;
      // β_i = β_i-1+ω_i-1x(qv_i*R_i-1^0*ez)+qa_i*R_i-1^0*ez
      b_i = b.col(i - 1) +
            vector3f::cross(w.col(i - 1), qv[i - 1][0] * R_0iminus1 * ez) +
            qa[i - 1][0] * R_0iminus1 * ez;
      p_i = t2p(T_0i);  // p_i = T_i^0(1:3,4)
      // v_i = v_i-1+ω_ix(p_i-p_i-1)
      v_i = v.col(i - 1) + vector3f::cross(w_i, p_i - p.col(i - 1));
      // a_i = a_i-1+β_ix(p_i-p_i-1)+ω_ix(ω_ix(p_i-p_i-1))
      ai = a.col(i - 1) + vector3f::cross(b_i, p_i - p.col(i - 1)) +
           vector3f::cross(w_i, vector3f::cross(w_i, p_i - p.col(i - 1)));
      // ac_i = a_i+β_ix(R_0^i*rc_i^i)+ω_ix(ω_ix(R_0^i*rc_i^i))
      ac_i =
          ai + vector3f::cross(b_i, R_0i * links_[i - 1].rc()) +
          vector3f::cross(w_i, vector3f::cross(w_i, R_0i * links_[i - 1].rc()));
      for (int row = 0; row < 3; row++) {
        w[row][i] = w_i[row][0];
        b[row][i] = b_i[row][0];
        p[row][i] = p_i[row][0];
        v[row][i] = v_i[row][0];
        a[row][i] = ai[row][0];
        ac[row][i] = ac_i[row][0];
      }
      T_0iminus1 = T_0i;  // T_i-1^0
    }

    // backward propagation
    // record each links' force
    Matrixf<3, _n + 1> f = matrixf::zeros<3, _n + 1>();   // joint force
    Matrixf<3, _n + 1> mu = matrixf::zeros<3, _n + 1>();  // joint moment
    // temperary vector
    Matrixf<3, 1> f_iminus1, mu_iminus1;
    // {T,R',P}_i^i-1
    Matrixf<4, 4> T_iminus1i;
    Matrixf<3, 3> RT_iminus1i;
    Matrixf<3, 1> P_iminus1i;
    // I_i-1(in 0 coordinate)
    Matrixf<3, 3> I_i;
    // joint torque
    Matrixf<_n, 1> torq;

    // load on end effector
    for (int row = 0; row < 3; row++) {
      f[row][_n] = he.block<3, 1>(0, 0)[row][0];
      mu[row][_n] = he.block<3, 1>(3, 0)[row][0];
    }
    for (int i = _n; i > 0; i--) {
      T_iminus1i = T(q, i - 1);               // T_i^i-1
      P_iminus1i = t2p(T_iminus1i);           // P_i^i-1
      RT_iminus1i = t2r(T_iminus1i).trans();  // R_i^i-1'
      R_0iminus1 = R_0i * RT_iminus1i;        // R_i-1^0
      // I_i^0 = R_i^0*I_i^i*(R_i^0)'
      I_i = R_0i * links_[i - 1].I() * R_0i.trans();
      // f_i-1 = f_i+m_i*ac_i-m_i*g
      f_iminus1 = f.col(i) + links_[i - 1].m() * ac.col(i) -
                  links_[i - 1].m() * gravity_;
      // μ_i-1 = μ_i+f_ixrc_i-f_i-1xrc_i-1->ci+I_i*b_i+ω_ix(I_i*ω_i)
      mu_iminus1 = mu.col(i) +
                   vector3f::cross(f.col(i), R_0i * links_[i - 1].rc()) -
                   vector3f::cross(f_iminus1, R_0i * (RT_iminus1i * P_iminus1i +
                                                      links_[i - 1].rc())) +
                   I_i * b.col(i) + vector3f::cross(w.col(i), I_i * w.col(i));
      // τ_i = μ_i-1'*(R_i-1^0*ez)
      torq[i - 1][0] = (mu_iminus1.trans() * R_0iminus1 * ez)[0][0];
      for (int row = 0; row < 3; row++) {
        f[row][i - 1] = f_iminus1[row][0];
        mu[row][i - 1] = mu_iminus1[row][0];
      }
      R_0i = R_0iminus1;
    }

    return torq;
  }

 private:
  Link links_[_n];  //连杆组
  Matrixf<3, 1> gravity_; //重力加速度向量

  Matrixf<4, 4> T_;         //末端齐次变换矩阵
  Matrixf<6, _n> J_;        //末端雅可比矩阵
};
};  // namespace robotics


#endif //DEV_ROBOTICS_H