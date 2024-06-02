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
#include <sstream>
#include <fstream>
#include <string>
#include <cmath>
#include <exception>

#include "particle.h"
#include "periodicCorrection.h"

namespace ForceChain
{
    // Reads Particles individual properties: x,y,z,radius
    // from a liggghts CSV file and store them.
    // Returns a vector of this particles.
    // Note: Particles[0] is empty as liggghts index start
    // from 1.
    auto ReadParticles(std::string fileName)
    {

        std::vector<Particle> particles;

        std::ifstream fileStream(fileName);

        if(fileStream.fail()){
            throw std::runtime_error("\n Error: The below file does not exist:\n"+fileName);
        }

        size_t particlesCount = 0;
        size_t iLine = 0;
        std::string line;
        while (getline(fileStream, line))
        {
            iLine++;
            if (iLine == 4)
            {
                std::stringstream str(line);
                str >> particlesCount;
                particles.resize(particlesCount + 1);
            }
            // skip file headers
            if (iLine < 10)
                continue;

            std::stringstream str(line);

            size_t id;
            str >> id;
            if (id > particles.size() - 1)
                std::cout << "The id of a particle cannot be bigger than number of particles!";

            particles[id].id = id;

            str >> particles[id].type;
            str >> particles[id].position(0) >> particles[id].position(1) >> particles[id].position(2);

            // skip unused particle properties
            double tmp;
            for (size_t i = 0; i < 12; i++)
                str >> tmp;

            str >> particles[id].radius;

        }

        fileStream.close();
        return particles;
    }

    // Reads Pair interactions from a liggghts CSV file.
    // Fills particles' neighbors and stress tensor.
    auto ReadPair(std::vector<Particle> &particles, std::string fileName)
    {

        // Read from the text file
        std::ifstream fileStream(fileName);

        if(fileStream.fail()){
            throw std::runtime_error("\n Error: The below file does not exist:\n"+fileName);
        }

        // The stresses and forces are initial at zero as they are summed up
        // in the interaction loop
        for (auto &&particle : particles)
        {
            particle.stress = Eigen::Matrix3d::Zero();
            particle.force = Eigen::Vector3d::Zero();
        }

        int iLine = 0;
        std::string line;
        std::vector <double> low_bound(3);
        std::vector <double> upp_bound(3);

        while (getline(fileStream, line))
        {
            iLine++;
            std::stringstream stream(line);

            // file header part
            if (iLine < 10){
                if (iLine==6){
                    stream >> low_bound[0] >> upp_bound[0];
                }
                else  if (iLine==7)
                    stream >> low_bound[1] >> upp_bound[1];
                else if (iLine==8)
                    stream >> low_bound[2] >> upp_bound[2];
                continue;
            }

            double overlap;
            int id1, id2;
            bool isPeriodicPair;
            Eigen::Vector3d x1, x2, f12;

            
            stream >> x1[0] >> x1[1] >> x1[2] >> x2[0] >> x2[1] >> x2[2] >> id1 >> id2 >> isPeriodicPair >> f12[0] >> f12[1] >> f12[2] >> overlap;


            if (id1 > particles.size() - 1 || id2 > particles.size() - 1)
                std::cout << "The id of a particle cannot be bigger than number of particles!";

            // fill neighbors
            particles[id1].neighbors.push_back(id2);
            particles[id2].neighbors.push_back(id1);

            // calculate stress
           
            // adjusts the positions of particle x2 in periodic interaction with x1 
            //for appropriately calculating the x12 vector

            Eigen::Vector3d x12 = x1 - x2;
            if (x12.norm()>(particles[id1].radius+particles[id2].radius)){
                x2=periodic_adjust(x1, x2, low_bound, upp_bound);
                x12 = x1 - x2;
            }     
            x12 = x12 / x12.norm();

            // fill forces
            for (size_t i = 0; i < 3; i++)
            {
                particles[id1].force(i) +=   f12[i];
                particles[id2].force(i) +=  - f12[i];
            }

            double vol1 = particles[id1].getVolume();
            double vol2 = particles[id2].getVolume();

            // Contract: Compressive stress is negative.
            // f12 is the force acting on particle 1.
            // x12 is toward center of particle 1, i.e. 
            // it is in the direction of compressive force
            // acting on particle 1.
            // -f12.x12 = is in direction of negative compression
            // which is used here.
            for (size_t i = 0; i < 3; i++)
            {
                for (size_t j = 0; j < 3; j++)
                {
                    particles[id1].stress(i, j) +=
                        f12[i] * (-x12[j]) * (particles[id1].radius - overlap / 2.) / vol1;

                    particles[id2].stress(i, j) +=
                        -f12[i] * (+x12[j]) * (particles[id2].radius - overlap / 2.) / vol2;
                }
            }
        }

        // Close the file
        fileStream.close();
        std::vector<std::vector<double>> sim_box;
        sim_box.push_back(low_bound);
        sim_box.push_back(upp_bound);
        return sim_box;
    }

    // Reads particle wall interactions from a liggghts CSV file.
    //  Because stress and force are calculated in sumation way,
    // first call ReadPair then this.
    void ReadWall(std::vector<Particle> &particles, std::string fileName)
    {

        // Read from the text file
        std::ifstream fileStream(fileName);
        if(fileStream.fail()){
            throw std::runtime_error("\n Error: The below file does not exist:\n"+fileName);
        }

        
        int iLine = 0;
        std::string line;
        while (getline(fileStream, line))
        {
            iLine++;

            // skip file headers
            if (iLine < 10)
                continue;

            double overlap;
            int pid, mid, tid; // particle id, mesh id, stl triangle id
            Eigen::Vector3d x1, x2, f12; // x1 is for mesh, x2 particle

            std::stringstream stream(line);
            stream >> x1[0] >> x1[1] >> x1[2] >> x2[0] >> x2[1] >> x2[2] >> mid >> tid >> pid >> f12[0] >> f12[1] >> f12[2] >> overlap;


            if (pid > particles.size() - 1)
                std::cout << "The id of a particle cannot be bigger than number of particles!";


            // calculate stress
            Eigen::Vector3d x12 = x1 - x2;
            x12 = x12 / x12.norm();

            // fill forces, note force
            // have value calculated from ReadPair(),
            // here we add to that one.
            for (size_t i = 0; i < 3; i++)
            {
                particles[pid].force(i) +=   f12[i];
            }

            double vol = particles[pid].getVolume();

            for (size_t i = 0; i < 3; i++)
            {
                for (size_t j = 0; j < 3; j++)
                {

                    particles[pid].stress(i, j) +=
                        f12[i] * (x12[j]) * (particles[pid].radius - overlap / 2.) / vol;
                }
            }
        }

        // Close the file
        fileStream.close();
    }
}

