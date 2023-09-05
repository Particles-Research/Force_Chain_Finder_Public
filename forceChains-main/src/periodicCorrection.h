/* 
# Force Chain Finder - A software tool for the recursive detection of
# force-chains in granular materials via minor principal stress.
# Copyright (C) 2023 Omid Ejtehadi 'oejtehad@ed.ac.uk'
#		             Aashish K Gupta 'A.K.Gupta-2@sms.ed.ac.uk'
#		             Sorush Khajepor 'sorush.kh@gmail.com'
#		             Sina Haeri 'shaeri@ed.ac.uk'
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

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