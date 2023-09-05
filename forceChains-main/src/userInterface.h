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

#include "liggghtsReader.h"
#include "chainFinder.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "chainsIo.h"
#include "stat.h"

namespace ForceChain
{
    // A public interface for this library for users.
    class UserInterface
    {
        std::string particlesFile;
        std::string pairFile;
        std::string wallFile;
        // It usually is pi/4, the smaller angle, the particles must
        // be more aligned to form a chain.
        double chainMaxAngle;

    public:
        std::vector<Particle> particles;
        std::vector<std::vector<double>> simulation_box;
        std::vector<std::vector<size_t>> chains;

        ChainsIo chainsIo;
        Stat stat;

        UserInterface(std::string particlesFile_, std::string pairFile_,
                      std::string wallFile_, double chainMaxAngle_)
            : particlesFile(particlesFile_), pairFile(pairFile_), wallFile(wallFile_),
              chainMaxAngle(chainMaxAngle_), chainsIo(chains, particles), stat(chains, particles)
        {
        }

        auto GetStat()
        {
            return stat;
        }

        auto Run()
        {
            particles = ReadParticles(particlesFile);
            simulation_box = ReadPair(particles, pairFile);
    
            if (wallFile.find(".txt") != std::string::npos) {
                ReadWall(particles, wallFile);
            }

            for (auto &particle : particles)
            {
                particle.setPrincipleStressAndDir();
            }

            ChainFinder chainFinder{particles, chainMaxAngle, simulation_box};
            chains = chainFinder.RecursiveFindChains();
            stat.ResetSamples();
        }

        auto WriteBigStressChains(std::string fileName,
                                  double minMinorStress,
                                  bool withHeaders,
                                  std::string delimiter)
        {
            auto filter = MinorStressGreaterFilter(minMinorStress);
            auto chainIds = stat.ApplyFilterAny(filter);
            chainsIo.WriteFilteredCsv(fileName, withHeaders, delimiter, chainIds);
        }

        auto WriteChainsCsv(std::string fileName,
                            bool withHeaders,
                            std::string delimiter)
        {
            chainsIo.WriteCsv(fileName, withHeaders, delimiter);
        }

        auto WriteChainsVtp(std::string fileName)
        {
            chainsIo.WriteVtp(fileName);
        }

        auto WriteChainsOnTerminal()
        {
            chainsIo.WriteChainsOnTerminal();
        }
    };
}