
# FCF-Public-V1 Documentation
## Background
Granular materials are widespread in many engineering and geophysical applications. The behavior of granular materials is complex, with the stress state and deformation patterns depending on the nature of the material, the boundary conditions, and the loading history. Understanding the mechanical behavior of granular materials is crucial for many practical applications, such as landslide prediction, the design of bulk material handling systems, and the production of pharmaceuticals.

One of the key features of granular materials is the formation of force chains, which are chains of particles that bear strong forces. Force chains play a critical role in determining the mechanical behavior of granular materials, and their characterization is important for understanding the deformation patterns and failure mechanisms of granular materials. In this context, using a computational approach, the **Force Chain Finder** software has been developed to identify force chains in granular materials.

## Theory
**Force Chain Finder** is a software tool that can identify force chains in a granular material. Force chains are the groups of particles that carry the most load in a granular material. The software uses the direction and magnitude of the most compressive principal stress to identify force chains. The force chains are found by recursively traversing a particle, its neighbors, neighbors of those neighbors, and so on. The code can find 3D chains with many branches. The software is derived from the research outlined in [1], with algorithmic enhancements incorporated. Further insights into the technical implementation can be found in [1], where advancements to the original work are detailed.

The **Force Chain Finder** software is based on the idea that force-chain formation depends on the direction and magnitude of the most compressive principal stress (minor principal stress) [2]. According to the theory, a particle must have minor principal stress with a magnitude larger than the average and a direction aligned with the line of contact with neighboring particles to participate in force-chain formation. The software identifies force chains by traversing a particle, its neighbors, neighbors of neighbors, and so on, recursively. The direction of the minor principal stress must create an angle smaller than alpha with the line of contact with neighboring particles, where alpha is a user-defined parameter. The algorithm can find 3D chains with many branches.

## Features
The **Force Chain Finder** software offers the following features:
- Calculating the force-chain based on minor principal stress
- Analyzing a specific region
- Analyzing a moving region
- Enabling visualization of force-chains via Paraview
- Chains statistics: average, median, minimum and maximum length, number of chains

## Software Architecture
The **Force Chain Finder** software is implemented in C++ and uses the Eigen library for matrix and vector operations. The software consists of four main components:
1. Input Reader: This component reads the input LIGGGHTS file containing particle pair interactions. The file format should include the positions and forces of each particle.
2. Stress Tensor Calculator: This component calculates the stress tensor on each particle based on its interactions with other particles. The stress tensor is calculated using the Virial formula.
3. Force Chain Finder: This component identifies force chains in the material by traversing the particles recursively and applying the conditions for force-chain formation.
4. Output Writer: This component exports the force-chain results in CSV format and provides post-processing features such as analyzing specific regions, analyzing moving regions, finding the normal vector of a force chain, and providing chain statistics such as the average, median, minimum, and maximum length and number of chains.

## Dependencies
The **Force Chain Finder** software depends on the following libraries:
- Eigen Library
- pybind Library

The packages Eigen and Pybind should be placed in the src directory. Pybind is an open-source C++ library that offers a seamless bridge between C++ and Python. It enables users to expose C++ code to Python and vice versa, allowing them to call C++ functions from Python and Python functions from C++. Both Eigen and Pybind are header-only libraries, meaning that they do not require a separate installation or compilation. They can be used by simply including the appropriate header files in the C++ code and then compiling the code as part of the project build process.

## Build Instruction
Step 1: Set Up Your Project Directory
Create a directory for your C++ project and navigate to it in your terminal.
```bash
mkdir MyCppProject
cd MyCppProject
```
Step 2: Create a CMakeLists.txt File
Create a CMakeLists.txt file in your project directory. A sample CMakeLists.txt file is provided. 

Step 3: Configure Your Build
In your terminal, navigate to your project directory and create a build directory. This is where you'll build your program.
```bash
mkdir build
cd build
```
Run CMake to configure the build:

```bash
cmake ..
```

This command generates build files based on your CMakeLists.txt file.
Step 4: Build the Program
Once CMake has configured your build, you can build your C++ program using a build tool like make on Unix-based systems or Visual Studio on Windows. Use the --build option to specify the target (your project name).
On Unix-based systems:
```bash
make MyCppProject
```
Step 6: Run the Program
After the build is successful, you can run the program:
On Unix-based systems:
```bash
./MyCppProject
```
## Usage
To use the software, follow the steps below:
1. Clone the repository from GitHub.
2. Copy the pair file obtained from LIGGGHTS in the example directory
3. Run the example code
6. Run the Python script to generate vtp files.
7. Open the vtp file in the ParaView for visualization

### User Interface
The `UserInterface` class is the first place to visit for creating custom code. The class provides a simple and intuitive interface for configuring the software parameters and executing the analysis. The user can set the input and output file paths, the value of alpha, and the region of interest. The user can also select the post-processing features to be applied.

### Examples
The **Force Chain Finder** software comes with several examples to demonstrate its capabilities. The examples folder contains input files, scripts, and output files for different types of granular materials. The user can run the examples to see how the software works and to understand the output format. The examples folder also contains a script to generate the LIGGGHTS input file.

#### Get Pair interactions from LIGGGHTS
To get pair interactions from LIGGGHTS, add the following line to a LIGGGHTS script:
```bash
compute 3 all pair/gran/local id pos delta force
```
Then, below the dump command, add:
```bash
dump dmp2 all local ${dmp_stp} ./build/post/pair*.txt c_3[1] c_3[2] c_3[3] c_3[4] c_3[5] c_3[6] c_3[7] c_3[8] c_3[9] c_3[10] c_3[11] c_3[12] c_3[13]
```
The order of the output file will be different from the compute command. It is:
```bash
p1x p1y p1z p2x p2y p2z id1 id2 is PeriodicInteraction f12x f12y f12z overlap
```
where p1 and p2 mean particle 1 and 2, and f12 is the interaction force between them. f12 is the force acting on particle 1 due to 2.

#### Get Wall interactions from LIGGGHTS
To get the interaction, add this line to a LIGGGHTS script:

```
compute         4 all wall/gran/local id pos delta force
```

and have dump line 

```
dump		dmp3 all local ${dmp_stp} ./build/post/wall_pair*.txt c_4[1] c_4[2] c_4[3] c_4[4] c_4[5] c_4[6] c_4[7] c_4[8] c_4[9] c_4[10] c_4[11] c_4[12] c_4[13]
```

The output is 

```
position wall (3), position particle (3), Mesh Id, triangle id, particle id, force(3), overlap
```
Force is the one acting on the particle.

## References
[1] O Ejtehadi, AK Gupta, S Khajepor, S Haeri, Force-chain finder: A software tool for the recursive detection of force-chains in granular materials via minor principal stress, Computer Physics Communications 297 (2024) 109070.

[2] J. Peters, M. Muthuswamy, J. Wibowo, A. Tordesillas, Characterization of force chains in granular material, Physical Review E, 72  041307 (2005)
