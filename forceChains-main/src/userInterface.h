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