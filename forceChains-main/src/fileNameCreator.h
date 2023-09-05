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