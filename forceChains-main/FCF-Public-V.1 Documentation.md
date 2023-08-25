
# FCF-Public-V1 Documentation
## Background
Granular materials are widespread in many engineering and geophysical applications. The behavior of granular materials is complex, with the stress state and deformation patterns depending on the nature of the material, the boundary conditions, and the loading history. Understanding the mechanical behavior of granular materials is crucial for many practical applications, such as landslide prediction, the design of bulk material handling systems, and the production of pharmaceuticals.

One of the key features of granular materials is the formation of force chains, which are chains of particles that bear strong forces. Force chains play a critical role in determining the mechanical behavior of granular materials, and their characterization is important for understanding the deformation patterns and failure mechanisms of granular materials. In this context, using a computational approach, the **Force Chain Finder** software has been developed to identify force chains in granular materials.

## Theory
The **Force Chain Finder** software is based on the idea that force-chain formation depends on the direction and magnitude of the most compressive principal stress (minor principal stress). According to the theory, a particle must have minor principal stress with a magnitude larger than the average and a direction aligned with the line of contact with neighboring particles to participate in force-chain formation. The software identifies force chains by traversing a particle, its neighbors, neighbors of neighbors, and so on, recursively. The direction of the minor principal stress must create an angle smaller than alpha with the line of contact with neighboring particles, where alpha is a user-defined parameter. The algorithm can find 3D chains with many branches.

**Force Chain Finder** is a software tool that can identify force chains in a granular material. Force chains are the groups of particles that carry the most load in a granular material. The software uses the direction and magnitude of the most compressive principal stress to identify force chains. The force chains are found by recursively traversing a particle, its neighbors, neighbors of those neighbors, and so on. The code can find 3D chains with many branches.

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
where p1 and p2 mean particle 1 and 2, and f12 is the interaction force between them. f12 is the force acting on particle 1. An example script is in the `./examples/liggghtsResult.tar.xz` file.

#### Get Wall interactions from LIGGGHTS
To get wall interactions from LIGGGHTS, follow the steps below:
1. Create a 3D box in FreeCad at the size of the bed. The height of the bed is not important.
2. Import it to Meshlab.
3. Remove all the triangles except the ones that create one plane at the size of the bed.
4. Go to `filter -> remeshing and ... -> isotropic meshing`.
5. Mesh the surface to have 40-100 triangles.
6. Import in LIGGGHTS run.

Please note that formatting may vary slightly depending on the specific rendering engine you use to view the Markdown document.
