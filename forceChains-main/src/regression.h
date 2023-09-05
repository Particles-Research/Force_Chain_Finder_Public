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

#ifndef REGRESSION_H
#define REGRESSION_H

#include <Eigen/Dense>
#include <iostream>
class Regression
{
    // matrix of X of points
    Eigen::MatrixXd x;
    // vector of y of points
    Eigen::VectorXd y;

    // A matrix with first column are 1,
    // and the rest are X like:
    //  1 x00 x01
    //  1 x10 x11
    Eigen::MatrixXd xc;

    // The solution
    Eigen::VectorXd b;

public:
    Regression(Eigen::MatrixXd x_,
               Eigen::VectorXd y_) : x(x_), y(y_)
    {

        // setup xc
        xc.resize(x.rows(), x.cols() + 1);
        for (size_t i = 0; i < x.rows(); i++)
            xc(i, 0) = 1;
        xc.block(0, 1, x.rows(), x.cols()) = x;
    };

    auto Compute()
    {
        b = xc.colPivHouseholderQr().solve(y);

        return b;
    }

    auto Print()
    {
        std::cout << "Intercept = " << b[0] << "\n";
        std::cout << "Slope =\n";

        for (size_t i = 1; i < b.size(); i++)
        {
            std::cout << b[i] << " ";
        }
        std::cout << std::endl;
    }
};

#endif // !REGRESSION_H
