#include <string>
#include <sstream>
#include <vector>
#include <iomanip>

namespace ForceChain
{
    // Creates a vector of file names with a specific prefix, postfix 
    // and increasing numbers: d_0.liggghts, d_3000.liggghts, and etc.
    // start: the number of first file, 
    // last: the number of last file
    // step: the step number between two subsequent files.

    auto CreateNumericFileName(std::string prefix,
                               std::string postfix,
                               size_t start, size_t last, size_t step)
    {
        std::vector<std::string> fileNames;

        for (size_t i = start; i <= last; i += step)
        {
            std::stringstream ss;
            std::string name = prefix + std::to_string(i) + postfix;
            fileNames.push_back(name);
        }
        return fileNames;
    }
}