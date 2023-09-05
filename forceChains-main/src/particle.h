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

#ifndef PARTICLE_H
#define PARTICLE_H

#include <array>
#include <vector>
#include <memory>
#include <cmath>
#include <iostream>
#include <Eigen/Dense>
#include "precision.h"
#include "vectorAngle.h"

namespace ForceChain
{
    class Particle
    {
    public:
        size_t id;
        size_t type;
        double radius;       
        mint chainId; // Negative value means it has no chain
        Eigen::Vector3d position;
        Eigen::Vector3d force;
        Eigen::Matrix3d stress;
        Eigen::Matrix3cd principleDirs;
        Eigen::Vector3cd principleStresses;
        bool hasMinorStress;
        double minorStress;
        Eigen::Vector3d minorDir;

        std::vector<size_t> neighbors;
        std::vector<size_t> linkages; //vector to store the particle id's which are connected to a given particle
        std::vector<int> linkNodes; //vector to store the particle id's from which an inward link is identified while travesing the chain 

        Particle()
        {
            id = 0;
            type = 0;
            chainId = -1;
            radius = 0;

            position = Eigen::Vector3d::Zero();
            force = Eigen::Vector3d::Zero();
            stress = Eigen::Matrix3d::Zero();
            principleDirs = Eigen::Matrix3cd::Zero();
            principleStresses = Eigen::Vector3cd::Zero();

            hasMinorStress = false;
            minorStress = 0.;
            minorDir = Eigen::Vector3d::Zero();
        }
        auto getVolume()
        {
            return 4. / 3. * M_PI * radius * radius * radius;
        }
        Eigen::Vector3d distanceTo(const Particle &other)
        {
            return position - other.position;
        }

        // The imaginary dir are replaced with zero
        auto GetRealPrincipleDirs() const
        {
            Eigen::Matrix3d realDirs = principleDirs.real();
            for (size_t i = 0; i < 3; i++)
            {
                if (std::abs(principleStresses[i].imag()) > epsilon)
                    realDirs.col(i) = Eigen::Vector3d::Zero();
            }
            return realDirs;
        }

        auto AnyImagDir()
        {
            return (principleDirs.imag().array().abs() > epsilon).any();
        }

        auto IsImagDir(int dir)
        {
            return (principleDirs.imag().col(dir).array().abs() > epsilon).any();
        }

        // The imaginary stresses are replaced with zero
        auto GetRealPrincipleStresses() const
        {
            Eigen::Vector3d realStresses = principleStresses.real();
            for (size_t i = 0; i < 3; i++)
            {
                if (std::abs(principleStresses[i].imag()) > epsilon)
                    realStresses(i) = 0;
            }
            return realStresses;
        }

        auto AreAllPrincipleStressesReal()
        {
            return (principleStresses.imag().array().abs() < epsilon).all();
        }

        auto HasRealNegativePrincipleStress()
        {
            for (size_t i = 0; i < 3; i++)
            {
                if (principleStresses[i].real() < 0 &&
                    std::abs(principleStresses[i].imag()) < epsilon)
                    return true;
            }
            return false;
        }

        auto GetRealPrincipleStressesCount()
        {
            return (principleStresses.imag().array().abs() < epsilon).count();
        }

        // returns det(stress - σ I)
        auto GetDeterminantCharacMatrix(double principleStress)
        {
            return (stress - principleStress * Eigen::Matrix3d::Identity()).determinant();
        }

        // returns det(stress - σ I) for
        // real σ, principle stresses.
        auto GetDeterminantsCharacMatrix()
        {
            Eigen::Vector3d dets;
            for (size_t i = 0; i < 3; i++)
                dets[i] = (stress - GetRealPrincipleStresses()[i] * Eigen::Matrix3d::Identity()).determinant();
            return dets;
        }

        // returns the angle between principle directions.
        // For a symmetric matrix the directions are orthogonal.
        // Angles are reported less than PI/2.
        auto GetPrincipleDirAngles()
        {
            Eigen::Vector3d angles(0, 0, 0);
            if (!AreAllPrincipleStressesReal())
                return angles;
            auto realDirs = GetRealPrincipleDirs();
            angles[0] = angleBetweenVectors(realDirs.col(0), realDirs.col(1));
            angles[1] = angleBetweenVectors(realDirs.col(1), realDirs.col(2));
            angles[2] = angleBetweenVectors(realDirs.col(0), realDirs.col(2));

            for (size_t i = 0; i < 3; i++)
                angles[i] = angles[i] <= M_PI_2 ? angles[i] : M_PI - angles[i];

            return angles;
        }

        // Returns the radian angle of minor principle
        // stress with x, y and z axis.
        auto GetMinorDirAngles()
        {
            Eigen::Vector3d angles(0, 0, 0);
            if (!hasMinorStress)
                return angles;
            auto realDirs = GetRealPrincipleDirs();
            angles[0] = angleBetweenVectors(minorDir, Eigen::Vector3d(1, 0, 0));
            angles[1] = angleBetweenVectors(minorDir, Eigen::Vector3d(0, 1, 0));
            angles[2] = angleBetweenVectors(minorDir, Eigen::Vector3d(0, 0, 1));

            for (size_t i = 0; i < 3; i++)
                angles[i] = angles[i] <= M_PI_2 ? angles[i] : M_PI - angles[i];

            return angles;
        }

        auto setPrincipleStressAndDir()
        {
            Eigen::EigenSolver<Eigen::Matrix3d> es(stress);
            principleStresses = es.eigenvalues();
            principleDirs = es.eigenvectors();

            minorStress = std::numeric_limits<double>::max();
            size_t minorId = -1;
            hasMinorStress = false;

            for (size_t i = 0; i < 3; i++)
            {
                auto real = principleStresses(i).real();
                auto imag = principleStresses(i).imag();

                if (std::abs(imag) > epsilon)
                    continue;

                if (real < minorStress)
                {
                    minorStress = real;
                    minorId = i;
                };
            }

            // only compressive stress accounted
            if (minorStress < -1e-14)
            {
                hasMinorStress = true;
                minorDir(0) = principleDirs(0, minorId).real();
                minorDir(1) = principleDirs(1, minorId).real();
                minorDir(2) = principleDirs(2, minorId).real();
            }
        }

        auto GetStressInfo()
        {
            std::stringstream out;
             out << "Force  = \n" << force << "\n";
            out << "Stress  = \n" << stress << "\n";
            out << "Stress Determinant = " << stress.determinant() << "\n";
            out << "principle stresses:\n"
                << principleStresses << "\n";
            out << "principle Directions:\n"
                << principleDirs << "\n";
            out << "any imag dir? " << AnyImagDir() << "\n";
            out << "is 0 imag dir? " << IsImagDir(0) << "\n";
            out << "is 1 imag dir? " << IsImagDir(1) << "\n";
            out << "is 2 imag dir? " << IsImagDir(2) << "\n";

            
            out << "real principle stresses:\n"
                << GetRealPrincipleStresses() << "\n";
            out << "are all real? " << AreAllPrincipleStressesReal() << "\n";
            out << "real count: " << GetRealPrincipleStressesCount() << "\n";
            out << "Has real negative? " << HasRealNegativePrincipleStress() << "\n";
            out << "Minor Stress = " << minorStress << "\n";
            
            out << "Determinants of characteristic matrix  =\n "
                << GetDeterminantsCharacMatrix() << "\n";
            out << "Angles between principle dirs (degree):\n"
                << GetPrincipleDirAngles() * 180 / M_PI << "\n";
            out << "Minor principle stress angle with x,y,z Axis:\n"
                << GetMinorDirAngles() * 180 / M_PI << "\n";
            return out.str();
        }

        auto PrintStressInfo()
        {
            std::cout << GetStressInfo();
        }
    };
}
#endif // PARTICLE_H