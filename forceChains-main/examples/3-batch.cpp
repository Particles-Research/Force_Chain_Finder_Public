/* 
Ensure studying 1-single.cpp and 2-filter.cpp, before running this example.
This finds force chains for a batch of LIGGGHTS files.
It stores the chains in VTP or CSV files in the same path of the LIGGGHTS results.
The liggghts files for this example are not provided. However their format expected
to be same as the ones stored in liggghtsResult.tar.xz file.

It is better to run 1-single.cpp on one time-step of your results, if satisfactory,
run this for all of them.

The path variable must be the folder of your liggghts results.
*/
#include "userInterface.h"
#include "fileNameCreator.h"
using namespace std;
using namespace ForceChain;

int main()
{
    // READING INPUT PARAMETERS FROM A FILE
    ifstream inputFile("../liggghtsResult/input_parameters_batch.txt");

    if (!inputFile) {
        cerr << "Failed to open the input file." << endl;
    }
   
    
    string line, path, particleFile_name, particleFile_ext, pairWall_name, pairWall_ext, outputFile_name;  // path is where the liggghts files are placed.
    double alpha;
    int start, last, step; // the range of files number

    while (getline(inputFile, line)) {
        istringstream iss(line);

        if (line.find("path=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> path;
        } else if (line.find("particleFile_name=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> particleFile_name;
        } else if (line.find("particleFile_ext=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> particleFile_ext;
        } else if (line.find("pairWall_name=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> pairWall_name;
        } else if (line.find("pairWall_ext=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> pairWall_ext;
        } else if (line.find("alpha=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> alpha;
        } else if (line.find("start=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> start;
        } else if (line.find("last=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> last;
        } else if (line.find("step=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> step;
        } 
    }

    inputFile.close();

        //-------------------------READING DONE------------------------------------//
      
    // We recreate the name of input files like pair0.txt, pair30000.txt and so on.
    // These name structures are decided in your liggghts script, if different to
    // below ones, modify below ones accordingly.

    auto d_names = CreateNumericFileName(path + particleFile_name, particleFile_ext , start, last, step);
    auto pairNames = CreateNumericFileName(path + "pair", ".txt", start, last, step);
    auto wallNames = CreateNumericFileName(path + pairWall_name, pairWall_ext, start, last, step);

    // output files
    auto chainNames = CreateNumericFileName(path + "../"+ "forceChains_", "", start, last, step);

    for (size_t i = 0; i < d_names.size(); i++)
    {
        // Write name of file being processed.
        auto percent = i * 100. / d_names.size();
        cout <<"\r"<< d_names[i] + "  " << setiosflags(ios::fixed) << setprecision(0) << percent << "%"<<flush;
        
        UserInterface ui(d_names[i], pairNames[i], wallNames[i], alpha);
        ui.Run();

        ui.WriteChainsCsv(chainNames[i]+".csv", true, ",");
        //ui.WriteChainsVtp(chainNames[i] + ".vtp");
    }
}