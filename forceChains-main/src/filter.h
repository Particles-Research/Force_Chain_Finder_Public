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

#ifndef FILTER_H
#define FILTER_H

#include "particle.h"

namespace ForceChain
{
    class Filter
    {
    public:
        virtual bool IsInside(const Particle &particle) = 0;
    };

    class MinorStressGreaterFilter : public Filter
    {
        double minMinorStress;

    public:
        MinorStressGreaterFilter(double minMinorStress_) : minMinorStress(minMinorStress_) {}
        bool IsInside(const Particle &particle) override
        {
            return std::abs(particle.minorStress) >= std::abs(minMinorStress);
        }
    };
}

#endif //FILTER_H