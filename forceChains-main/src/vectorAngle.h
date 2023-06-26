#ifndef VECTORANGLE_H
#define VECTORANGLE_H


#include <Eigen/Dense>

namespace ForceChain
{   
    // Angles are reported in [0,180]
    // The order of vectors is not important: f(a,b) = f(b,a)
    auto angleBetweenVectors(Eigen::Vector3d a, Eigen::Vector3d b)
    {
        return std::atan2(a.cross(b).norm(), a.dot(b));
    }
}

#endif