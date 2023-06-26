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