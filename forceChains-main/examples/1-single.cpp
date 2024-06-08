/*
This example finds force chains for one time step of LIGGGHTS files.
Unzip the files in liggghtsResult.tar.xz in build folder and run
the program.

Always make sure the format and order of columns in your results are
the same as the files in the tar archive. The tar file also includes
an input script for liggghts, so the user can create similar results
digestable by this program.

Here, it is assumed executables are in build folder.
*/

#include "userInterface.h"
#include "fileNameCreator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;
using namespace ForceChain;

int main()
{

    // READING INPUT PARAMETERS FROM A FILE
    ifstream inputFile("../liggghtsResult/input_parameters.txt");

    if (!inputFile) {
        cerr << "Failed to open the input file." << endl;
    }
   
    // path is where the liggghts files are placed.
    string line, inputPath, particleFile_name, pairFile_name, pairWall_name, outputPath, outputFile_name;
    double alpha;

    while (getline(inputFile, line)) {
        istringstream iss(line);

        if (line.find("inputPath=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> inputPath;
        } else if (line.find("particleFile_name=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> particleFile_name;
        } else if (line.find("pairFile_name=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> pairFile_name;
        } else if (line.find("pairWall_name=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> pairWall_name;
        } else if (line.find("outputPath=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> outputPath;
        } else if (line.find("outputFile_name=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> outputFile_name;
        } else if (line.find("alpha=") != string::npos) {
            iss.ignore(line.length(), '=');
            iss >> alpha;
        } 
    }

    inputFile.close();
    
    //-------------------------READING DONE------------------------------------//

    string particleFile = inputPath + particleFile_name;

    // Particle-Particle interactions file
    string pairFile = inputPath + pairFile_name;

    // Particle-wall(STL) interactions
    string pairWall = inputPath + pairWall_name;

    // Choose a name for output
    string outputFile = outputPath + outputFile_name;

    // Inject the file names and the maximum angle for force chains
    UserInterface ui(particleFile, pairFile, pairWall, alpha);

    // Calculate force chains
    ui.Run();

    // You export chains in CSV or VTP (Paraview) format.
    ui.WriteChainsCsv(outputFile + ".csv", true, ",");
    ui.WriteChainsVtp(outputFile + ".vtp");

    cout << outputFile << "\n done.";

    cout << "chains are: \n";
    ui.WriteChainsOnTerminal();
}
