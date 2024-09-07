#include "physics.hpp"
#include "config.hpp"

Eigen::Vector4f funct(Eigen::Vector4f x, float M, float m, float length, float force, float forceS){
    float xdot = x[1];
    float thetadot = x[3];

    float a = M + m;
    float b = m * length * cosf(x[2]);
    float c = force - m * length * powf(thetadot, 2) * sinf(x[2]);
    float d = length;
    float e = cosf(x[2]);
    float f = conf::cgravity * sinf(x[2]) + forceS;

    float xddot = ((c / a) + ((b * f) / (a * d))) / (1 - ((b * e) / (a * d)));
    float thetaddot = (f + e * xddot) / d;

    return {xdot, xddot, thetadot, thetaddot};
}


void stateUpdate(Eigen::Vector4f& x, float M, float m, float length, float force, float forceS, float timeStep){

    Eigen::Vector4f f1 = funct(x, M, m, length, force, forceS);

    Eigen::Vector4f x2 = x + f1 * timeStep * .5f;
    Eigen::Vector4f f2 = funct(x2, M, m, length, force, forceS);

    Eigen::Vector4f x3 = x + f2 * timeStep * .5f;
    Eigen::Vector4f f3 = funct(x3, M, m, length, force, forceS);

    Eigen::Vector4f x4 = x + f3 * timeStep;
    Eigen::Vector4f f4 = funct(x4, M, m, length, force, forceS);

    x += (1.f / 6.f) * timeStep * (f1 + 2 * f2 + 2 * f3 + f4);
}