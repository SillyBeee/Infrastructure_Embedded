project (ARM_KINEMATICS)
cmake_minimum_required(VERSION 3.5)

find_package(Eigen3 REQUIRED)
include_directories(${CMAKE_SOURCE_DIR}/../Transform)
add_executable(
  arm_kinematics
  Arm_Kinematics.cpp
  ${CMAKE_SOURCE_DIR}/../Transform/Transform.cpp
)

target_include_directories(arm_kinematics PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${EIGEN3_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/../Transform/*.hpp)
target_link_libraries(arm_kinematics Eigen3::Eigen)