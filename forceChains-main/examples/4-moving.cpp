
/* 
Ensure you have studied 1-single.cpp, 2-filter.cpp and 3-batch before running this.

This example finds force chains in a moving box. This is good to track an object
and watch the force chains around it.

The box is moved in x-axis after some delay with constant speed.

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

    // the range of files number
    auto start = 0;
    auto last = 4000;
    auto step = 500;

    // Particles and pair interaction files pattern
    auto d_names = CreateNumericFileName(path + "compress", ".liggghts", start, last, step);
    auto pairNames = CreateNumericFileName(path + "pair", ".txt", start, last, step);

    auto wallNames = CreateNumericFileName(path + "wall_pair", ".txt", start, last, step);

    // Set all elements of wallNames to empty strings if there are no stl files (or else comment this part out)
    for (auto& name : wallNames)
    {
        name.clear();
    }

    // Set names for the filter box at each time step
    auto boxNames = CreateNumericFileName(path + "../box", ".vtk", start, last, step);

    // Set names for force chains
    auto outputNames = CreateNumericFileName(path + "../forceChains_", "", start, last, step);

    auto boxWidth = 0.05;
    auto xBox0 = 0.00;
    auto boxDelay = 0;
    auto boxSpeed = 0.00275;

    for (size_t i = 0; i < d_names.size(); i++)
    {
        // Write name of file being processed.
        auto percent = i * 100. / d_names.size();
        cout <<"\r"<< d_names[i] + "  " << setiosflags(ios::fixed) << setprecision(0) << percent << "%"<<flush;
      
        UserInterface ui(d_names[i], pairNames[i], wallNames[i], M_PI_4);

        ui.Run();

        auto xBox = xBox0;
        if (i > boxDelay)
            xBox = xBox0 + (i - boxDelay) * boxSpeed;

        BoxFilter boxFilter(xBox,xBox + boxWidth, 0.03, 0.07, 0, 0.0055);
        boxFilter.WriteGeo(boxNames[i]);

        auto filter = MinorStressGreaterFilter(0.2);
        ui.stat.ApplyFilterAny(filter);
        ui.stat.ApplyFilterAll(boxFilter);

        ui.chainsIo.WriteFilteredVtp(outputNames[i]+".vtp", ui.stat.GetSampleChainIds());
    }
}