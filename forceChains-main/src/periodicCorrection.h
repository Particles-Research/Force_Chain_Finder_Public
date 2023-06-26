#include <iostream>
#include <vector>
#include "particle.h"
#include <cmath>

//   bool isPeriodicPair=1;
namespace ForceChain
{
    // This code adjusts the positions of particle x2 in periodic interaction with x1 for appropriately calculating the x12 vector  
    auto periodic_adjust(Eigen::Vector3d x1, Eigen::Vector3d x2,
                        std::vector <double> low_bound,
                        std::vector <double> upp_bound) 
    {
        double margin;
        size_t i=0;
        Eigen::Vector3d x12=x1-x2;

        // Finding the direction of periodicity
        if (std::abs(x12[1])>std::abs(x12[0]))
            i=1;

        if (std::abs(x12[2])>std::abs(x12[i]))
            i=2;

        // using the direction to adjust x2 coodinate
        if (x12[i]<0){                       //x2 was closer to the upper-bound
            margin = upp_bound[i]-x2[i]; 
            x2[i]=low_bound[i]- margin;
        }

        else{
            margin = x2[i]-low_bound[i];
            x2[i] = upp_bound[i]+ margin;
        }

        return x2;      
    }
}