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

#ifndef STAT_H
#define STAT_H

#include <vector>
#include "particle.h"
#include "filter.h"
#include <memory>
#include "regression.h"
#include <tuple>

namespace ForceChain
{
    // Calculates statistics of sample chains
    class Stat
    {
        std::vector<size_t> sampleChainIds;
        const std::vector<Particle> &particles;
        const std::vector<std::vector<size_t>> &chains;

    public:
        const auto &GetSampleChainIds()
        {
            return sampleChainIds;
        }

        // Accepts chains that all their particles satisfy filter
        auto ApplyFilterAll(Filter &filter)
        {
            std::vector<size_t> newSampleChainIds;

            for (auto &&chainId : sampleChainIds)
            {
                auto chain = chains[chainId];
                bool allInside = true;
                for (auto &&particleId : chain)
                {
                    if (!filter.IsInside(particles[particleId]))
                    {
                        allInside = false;
                        break;
                    }
                }
                if (allInside)
                    newSampleChainIds.push_back(chainId);
            }
            sampleChainIds = newSampleChainIds;

            return sampleChainIds;
        }

        // Accepts chains that have at lease one
        // particle which satisfy filter
        auto ApplyFilterAny(Filter &filter)
        {
            std::vector<size_t> newSampleChainIds;

            for (auto &&chainId : sampleChainIds)
            {
                auto &chain = chains[chainId];
                bool anyInside = false;
                for (auto &&particleId : chain)
                {
                    if (filter.IsInside(particles[particleId]))
                    {
                        anyInside = true;
                        break;
                    }
                }
                if (anyInside)
                    newSampleChainIds.push_back(chainId);
            }
            sampleChainIds = newSampleChainIds;
            return sampleChainIds;
        }

        // Reset sampleChainIds to point to all chains.
        // Applying each filter strips down samples.
        // Run this to start fresh or when chains are changed.
        auto ResetSamples()
        {
            sampleChainIds.clear();

            for (size_t id = 0; id < chains.size(); id++)
            {
                sampleChainIds.push_back(id);
            }
        }

        Stat(const std::vector<std::vector<size_t>> &chains_, const std::vector<Particle> &particles_)
            : chains(chains_), particles(particles_)
        {
            ResetSamples();
        }

        auto GetAverageChainLength()
        {
            auto lengthSum = 0.0;
            for (auto &&chainId : sampleChainIds)
            {
                auto &chain = chains[chainId];
                lengthSum += chain.size();
            }
            return lengthSum / sampleChainIds.size();
        }

        auto GetMaxChainLength()
        {

            size_t maxLength = 0;
            for (auto &&chainId : sampleChainIds)
            {
                auto &chain = chains[chainId];
                if (maxLength < chain.size())
                    maxLength = chain.size();
            }
            return maxLength;
        }

        auto GetMinChainLength()
        {

            auto minLength = std::numeric_limits<size_t>::max();
            for (auto &&chainId : sampleChainIds)
            {
                auto &chain = chains[chainId];
                if (minLength > chain.size())
                    minLength = chain.size();
            }
            return minLength;
        }

        // Could use pass by copy to avoid changing vector
        double GetMedianChainLength()
        {
            std::vector<size_t> lengths;
            for (auto &&chainId : sampleChainIds)
            {
                auto &chain = chains[chainId];
                lengths.push_back(chain.size());
            }
            size_t n = lengths.size() / 2;
            std::nth_element(lengths.begin(), lengths.begin() + n, lengths.end());
            int vn = lengths[n];
            if (lengths.size() % 2 == 1)
            {
                return vn;
            }
            else
            {
                std::nth_element(lengths.begin(), lengths.begin() + n - 1, lengths.end());
                return 0.5 * (vn + lengths[n - 1]);
            }
        }

        auto GetChainsCount()
        {
            return sampleChainIds.size();
        }

        // Finds a regression plane passing the particles
        // of a chain and returns the normal of the plane.
        auto GetChainNormalVector(const std::vector<size_t> &chain)
        {

            auto particlesCount = chain.size();
            Eigen::MatrixXd xy(particlesCount, 2);
            Eigen::VectorXd z(particlesCount);

            for (size_t i = -1; auto &&particleId : chain)
            {
                i++;
                auto &particle = particles[particleId];
                xy(i, 0) = particle.position(0);
                xy(i, 1) = particle.position(1);
                z(i) = particle.position(2);
            }
            Regression reg(xy, z);
            auto b = reg.Compute();

            Eigen::Vector3d normal{-b(1), -b(2), 1.};
            return normal;
        }

        // Gives angle of chain with the bed
        // when viewed from x-z view.
        // The angle is reported in degree [0,180]
        // with reference to x-axis. So 0 and 180 degrees
        // means chain is horizontal. Chain direction is
        // always in +z direction.
        auto GetChainAngleWithBed(const std::vector<size_t> &chain)
        {
            auto n = GetChainNormalVector(chain);
            auto normalAngle = atan(n(2) / n(0)) * 180 / M_PI;
            return normalAngle + 90;
        }

        auto GetChainsAnglesWithBed()
        {
            std::vector<std::tuple<size_t, double>> chainIdsAngles;
            for (size_t i = 0; i < sampleChainIds.size(); i++)
            {
                auto chainId = sampleChainIds[i];
                auto &chain = chains[chainId];

                auto angle = GetChainAngleWithBed(chain);
                chainIdsAngles.push_back(std::make_tuple(chainId, angle));
            }
            return chainIdsAngles;
        }

        auto WriteChainsAnglesWithBed(std::string fileName){
            std::ofstream fileStream(fileName);

            auto chainIdsAngles = GetChainsAnglesWithBed();

            for (size_t i = 0; i < chainIdsAngles.size(); i++)
            {
                auto& [chainId, angle] = chainIdsAngles[i];

                fileStream << chainId << ","<<angle<<"\n";
            }
        }
    };
}

#endif