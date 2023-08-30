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
#include "particle.h"

#include <vtkCellArray.h>
#include <vtkNew.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkDoubleArray.h>

namespace ForceChain
{

    // Write chains in CSV and VTP format.
    class ChainsIo
    {
        const std::vector<Particle> &particles;
        const std::vector<std::vector<size_t>> &chains;

    public:
        ChainsIo(const std::vector<std::vector<size_t>> &chains_,const std::vector<Particle> &particles_)
            : chains(chains_), particles(particles_) {}

        // Write paraview readable format for chains that
        // their id is given.
        auto WriteFilteredVtp(std::string fileName,
                              const std::vector<size_t> &chainIds)
        {
            vtkNew<vtkPoints> points;

            auto ids = vtkSmartPointer<vtkIntArray>::New();
            ids->SetNumberOfComponents(1);
            ids->SetName("LiggghtsId");

            auto chainIds_out = vtkSmartPointer<vtkIntArray>::New();
            chainIds_out->SetNumberOfComponents(1);
            chainIds_out->SetName("chainId");

            auto radii = vtkSmartPointer<vtkDoubleArray>::New();
            radii->SetNumberOfComponents(1);
            radii->SetName("Radius");

            auto minorStresses = vtkSmartPointer<vtkDoubleArray>::New();
            minorStresses->SetNumberOfComponents(1);
            minorStresses->SetName("MinorStress");

            int id[1] = {0};
            int chainId_out[1] = {0};

            for (auto &&chainId : chainIds)
            {

                for (auto &&particleId : chains[chainId])
                {

                    auto &particle = particles[particleId];

                    auto pos = particle.position;
                    points->InsertNextPoint(pos[0], pos[1], pos[2]);

                    id[0] = particle.id;
                    chainId_out[0] = particle.chainId;

                    ids->InsertNextTypedTuple(id);
                    chainIds_out->InsertNextTypedTuple(chainId_out);
                    radii->InsertNextTypedTuple(&particle.radius);
                    minorStresses->InsertNextTypedTuple(&particle.minorStress);
                }
            }

            vtkNew<vtkPolyData> polydata;
            polydata->SetPoints(points);

            polydata->GetPointData()->AddArray(ids);
            polydata->GetPointData()->AddArray(chainIds_out);
            polydata->GetPointData()->AddArray(minorStresses);
            polydata->GetPointData()->AddArray(radii);

            vtkNew<vtkXMLPolyDataWriter> writer;
            writer->SetFileName(fileName.c_str());
            writer->SetInputData(polydata);

            // Optional - set the mode. The default is binary.
            // writer->SetDataModeToBinary();
            writer->SetDataModeToAscii();

            writer->Write();
        }

        // Write all chains in paraview VTP format
        auto WriteVtp(std::string fileName)
        {
            std::vector<size_t> allChainIds;
            for (size_t i = 0; i < chains.size(); i++)
            {
                allChainIds.push_back(i);
            }

            WriteFilteredVtp(fileName, allChainIds);
        }

        // Writes a CSV file of chains whose id is given.
        auto WriteFilteredCsv(std::string fileName,
                              bool withHeaders,
                              std::string delimiter,
                              const std::vector<size_t> &chainIds)
        {
            std::ofstream fileStream(fileName);

            if (withHeaders)
            {
                fileStream << "LiggghtsId" << delimiter
                           << "X" << delimiter
                           << "Y" << delimiter
                           << "Z" << delimiter
                           << "radius" << delimiter
                           << "chainId" << delimiter
                           << "minorStress" << delimiter
                           << "minorDirX" << delimiter
                           << "minorDirY" << delimiter
                           << "minorDirZ" << delimiter
                           << "PrincipleStress0" << delimiter
                           << "PrincipleStress1" << delimiter
                           << "PrincipleStress2" << delimiter
                           // first dir
                           << "StressDir00" << delimiter
                           << "StressDir10" << delimiter
                           << "StressDir20" << delimiter
                           // 2nd dir
                           << "StressDir01" << delimiter
                           << "StressDir11" << delimiter
                           << "StressDir21" << delimiter
                           // 3rd dir
                           << "StressDir02" << delimiter
                           << "StressDir12" << delimiter
                           << "StressDir22" << delimiter
                           << "linkNodes"    << delimiter

                           << "\n";
            }

            // Write to the file
            for (auto &&chainId : chainIds)
            {
                auto &chain = chains[chainId];

                for (auto &&particleId : chain)
                {

                    const Particle &particle = particles[particleId];
                    const auto realPrincipleStresses = particle.GetRealPrincipleStresses();
                    const auto realPrincipleDirs = particle.GetRealPrincipleDirs();

                    fileStream << particle.id << delimiter
                               << particle.position(0) << delimiter
                               << particle.position(1) << delimiter
                               << particle.position(2) << delimiter
                               << particle.radius << delimiter
                               << particle.chainId << delimiter
                               << particle.minorStress << delimiter
                               << particle.minorDir[0] << delimiter
                               << particle.minorDir[1] << delimiter
                               << particle.minorDir[2] << delimiter
                               << realPrincipleStresses[0] << delimiter
                               << realPrincipleStresses[1] << delimiter
                               << realPrincipleStresses[2] << delimiter;
                    for (size_t k = 0; k < 3; k++)
                    {
                        for (size_t j = 0; j < 3; j++)
                        {
                            fileStream << realPrincipleDirs(j, k) << delimiter;
                        }
                    }
                    // fileStream << particle.linkNodes << delimiter;
                    fileStream << "\n";
                }
            }
            fileStream.close();
        }

        auto WriteCsv_1(std::string fileName,
                      bool withHeaders,
                      std::string delimiter)
        {
            std::vector<size_t> allChainIds;
            for (size_t i = 0; i < chains.size(); i++)
            {
                allChainIds.push_back(i);
            }

            WriteFilteredCsv(fileName, withHeaders, delimiter, allChainIds);
        }

        auto WriteChainsOnTerminal()
        {
            size_t id = -1;
            auto sum = 0.;
            for (auto &&chain : chains)
            {
                std::cout << "(" << ++id << ") ";
                for (auto &&item : chain)
                {
                    std::cout << item << " ";
                    sum += particles[item].id;
                }
                std::cout << "\n";
            }
            std::cout << "For debug: the sum of id all chain particles=\n"
                      << sum << "\n";
        }
    
        auto WriteCsv(std::string fileName,
                      bool withHeaders,
                      std::string delimiter)
        {
            std::ofstream fileStream(fileName);

            if (withHeaders)
            {
                fileStream << "LiggghtsId" << delimiter
                           << "X" << delimiter
                           << "Y" << delimiter
                           << "Z" << delimiter
                           << "radius" << delimiter
                           << "chainId" << delimiter
                           << "minorStress" << delimiter
                           << "minorDirX" << delimiter
                           << "minorDirY" << delimiter
                           << "minorDirZ" << delimiter
                           << "PrincipleStress0" << delimiter
                           << "PrincipleStress1" << delimiter
                           << "PrincipleStress2" << delimiter
                           // first dir
                           << "StressDir00" << delimiter
                           << "StressDir10" << delimiter
                           << "StressDir20" << delimiter
                           // 2nd dir
                           << "StressDir01" << delimiter
                           << "StressDir11" << delimiter
                           << "StressDir21" << delimiter
                           // 3rd dir
                           << "StressDir02" << delimiter
                           << "StressDir12" << delimiter
                           << "StressDir22" << delimiter
                           << "linkNodes"    << delimiter

                           << "\n";
            }

            // Write to the file
            for (auto &particle : particles)
            {
                if (particle.chainId==-1){
                    continue;
                }
             
                const auto realPrincipleStresses = particle.GetRealPrincipleStresses();
                const auto realPrincipleDirs = particle.GetRealPrincipleDirs();

                fileStream << particle.id << delimiter
                            << particle.position(0) << delimiter
                            << particle.position(1) << delimiter
                            << particle.position(2) << delimiter
                            << particle.radius << delimiter
                            << particle.chainId << delimiter
                            << particle.minorStress << delimiter
                            << particle.minorDir[0] << delimiter
                            << particle.minorDir[1] << delimiter
                            << particle.minorDir[2] << delimiter
                            << realPrincipleStresses[0] << delimiter
                            << realPrincipleStresses[1] << delimiter
                            << realPrincipleStresses[2] << delimiter;

                for (size_t k = 0; k < 3; k++)
                {
                    for (size_t j = 0; j < 3; j++)
                    {
                        fileStream << realPrincipleDirs(j, k) << delimiter;
                    }
                }

                if (particle.linkNodes.size()==0)
                    fileStream << "starting" << delimiter;

                else {
                    fileStream << "\"";
                    for (size_t i = 0; i < particle.linkNodes.size(); i++) {
                        fileStream << particle.linkNodes[i] ;
                        if (i != particle.linkNodes.size() - 1)
                            fileStream << ",";
                    }   
                    fileStream << "\"";
                    fileStream << delimiter;
                }

                fileStream << "\n";
            }

            fileStream.close();
        }

    };
}
