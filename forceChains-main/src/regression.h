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
