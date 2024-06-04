/* 
Ensure studying 1-single.cpp first, then use this. 
This example is similar to previous case 1-single.cpp but also has filters.
Filters are added to remove the chains that don't have certain conditions.
Here we only accept chains that are within a box and have at least one 
particle with MinorStress>stress_threshold.

Stat (statistics) class applies the filters and calculates the statistics. 
*/
#include "userInterface.h"
#include "fileNameCreator.h"
#include "sectionFilter.h"

using namespace std;
using namespace ForceChain;

int main()
{
    // Where the liggghts files placed.
    std::string path = "../liggghtsResult/batch_compression_2d/post/";

    // input files
    std::string particleFile = path + "compress4000.liggghts";
    std::string pairFile = path + "pair4000.txt";
    std::string pairWall = "";
    
    // output file
    std::string outputFile = path + "../forceChainsFiltered";

    UserInterface ui(particleFile, pairFile, pairWall, M_PI_4);
    ui.Run();

    // Define a box filter with its boundaries
    BoxFilter boxFilter(0.03, 0.07, 0.03, 0.07, 0, 0.0055);   
    
    // Write the box file in vtk format for Paraview
    boxFilter.WriteGeo(path+"../box.vtk");

    // Define a filter that minorStress> stress_threshold
    auto filter = MinorStressGreaterFilter(0.0);
    
    // Accept chains that has at least one particle
    // with minorStress> stress_threshold
    ui.stat.ApplyFilterAny(filter);
    
    // Accept chains that all their particles are
    // within the box.
    ui.stat.ApplyFilterAll(boxFilter);

    ui.chainsIo.WriteFilteredCsv(outputFile+".csv", true, ",", ui.stat.GetSampleChainIds());
    ui.chainsIo.WriteFilteredVtp(outputFile+".vtp", ui.stat.GetSampleChainIds());

    cout<<"Sample info:\n"
    << "num of chains: " << ui.stat.GetChainsCount() << "\n"
    <<"Average Length: " << ui.stat.GetAverageChainLength() <<"\n"
    <<"Max Length: " << ui.stat.GetMaxChainLength() <<"\n"
    <<"Min Length: " << ui.stat.GetMinChainLength() <<"\n"
    <<"Median Length: " << ui.stat.GetMedianChainLength() <<"\n";
}