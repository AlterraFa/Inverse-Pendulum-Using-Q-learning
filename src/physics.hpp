#pragma once
#include <eigen3/Eigen/Dense>
#include <cmath>

Eigen::Vector4f funct(const Eigen::Vector4f x, float M, float m, float length, float forcec, float forceS);
void stateUpdate(Eigen::Vector4f& x, float M, float m, float length, float force, float forceS, float timeStep);