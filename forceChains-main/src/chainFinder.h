/* 
 * Force Chain Finder - A software tool for the recursive detection of 
 * force-chains in granular materials via minor principal stress.
 * Copyright (C) 2023 Sina Haeri <shaeri@ed.ac.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */


#include "vector"
#include "particle.h"
#include <Eigen/Dense>
#include <cmath>
#include "vectorAngle.h"

namespace ForceChain
{
    /*
    This class is the core of ForceChain program. 
    It finds particles that form chains.

    Particle chainIds are set here. They must not 
    set anywhere else except the constructor of 
    Particle to be -1. 
    */
    class ChainFinder
    {

        std::vector<Particle> &particles;
        double alpha; // chain max angle in radian
        std::vector<std::vector<double>> box_dimension;


    public:
        // Stores the chains.
        std::vector<std::vector<size_t>> chains;

        ChainFinder(std::vector<Particle> &particles_, double alpha_, std::vector<std::vector<double>> box_dimension_)
            : particles(particles_), alpha(alpha_), box_dimension(box_dimension_) {}

    protected:
        // Studies two particles: last link of a chain and a neighbor.
        // Decides if neighbor is the next link.
        auto IsNeiNextLink(Particle &link, Particle &nei, int &neiDirSign)
        {

            if (!nei.hasMinorStress)
                return false;

            if ((nei.chainId != -1) && (nei.chainId != link.chainId))
                return false;            
            
            if (nei.chainId == link.chainId){
                for(auto id: link.linkages){
                    if (id==nei.id)
                        return false;
                }
            } 

            // is neighbor within link range
            auto xln = nei.distanceTo(link);

            //  Y periodic interaction
            if (xln.norm() > (nei.radius + link.radius)){
                auto nei_pos_adjusted= periodic_adjust(link.position, nei.position, box_dimension[0], box_dimension[1]);
                xln = nei_pos_adjusted-link.position; 
            }

            auto angle_forward = angleBetweenVectors(xln, link.minorDir);

            // forward check
            if ((angle_forward >= alpha) && (angle_forward< M_PI-alpha))
            {
                return false;
            }

            // backward check

            auto angle_back = angleBetweenVectors(-xln, nei.minorDir);
            neiDirSign = +1;
            // Change direction if angle>pi/2, and record it.
            if (angle_back > M_PI / 2)
            {
                angle_back = M_PI - angle_back;
                neiDirSign = -1;
            }

            //backward check
            if (angle_back >= alpha)
            {
                return false;
            }

            // Passed all the conditions:
            return true;
        }

        // Finds next link of chain recursively. It accounts for
        // a link branches multiple times.
        void RecursiveFindOneSideOfChain(std::vector<size_t> &chain, Particle *link)
        {
            for (auto inei : link->neighbors)
            {
                // is neighbor next link
                auto &nei = particles[inei];

                // To store which direction used for nei
                // to be next link. The opposite dir will
                // be used when nei is a link and we are
                // searching in its neighbors.
                int neiDirSign;

                if (!IsNeiNextLink(*link, nei, neiDirSign))
                    continue;

                // backward & forward check staisfied
                // nei is next link of the chain
                chain.push_back(inei);
                link->linkages.push_back(inei);

                nei.chainId = link->chainId;
                nei.linkNodes.push_back(link->id);
                nei.linkages.push_back(link->id);
                

                // reverse the used dir for next loop
                nei.minorDir = -neiDirSign * nei.minorDir;

                RecursiveFindOneSideOfChain(chain, &nei);
            }
        }

    public:
        // Finds all the chains in the system.
        auto &RecursiveFindChains()
        {
            // Each particle is center of a chain
            for (auto &center : particles)
            {
                // When checking a particle all their neighbors
                // are checked too. So, if a neighbor being studied
                // and belongs to a chain, now we ignore it.
                if (!center.hasMinorStress || center.chainId>=0)
                    continue;

                std::vector<size_t> chain;
                chain.push_back(center.id);

                // We give it next chain id, if this center forms 
                // a chain, it will hold it, otherwise it will be
                // cleared later on.
                center.chainId = chains.size();

                RecursiveFindOneSideOfChain(chain, &center);

                // Trying the other side of center
                center.minorDir = -center.minorDir;

                RecursiveFindOneSideOfChain(chain, &center);

                // Both side of the chain completed.
                // clear chain if has less than 3 particles
                if (chain.size() < 3)
                {
                    for (auto &particleId : chain)
                    {
                        particles[particleId].chainId = -1;
                    }
                    
                    // Setting the last "linkNodes" entry of the second particle of the chain to 0
                    if (chain.size()==2)
                    {
                        auto last_node_index = particles[chain[1]].linkNodes.size()-1;
                        particles[chain[1]].linkNodes[last_node_index] = -1*particles[chain[1]].linkNodes[last_node_index];
                    }
                }
                else
                { // record bigger chains

                    chains.push_back(chain);
                }
            }

            return chains;
        }
    };
}
