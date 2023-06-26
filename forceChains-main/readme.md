# Force Chain Finder

In a granular material, only a minority of particles carry the most load. These groups of particles create chains that bear strong forces. They are called force chains. It is found [1] that force-chain formation depends on the direction and magnitude of most compressive principal stress (minor principal stress). A particle must have minor principle stress that its magnitude is bigger than the average and its direction is aligned with the line of contact with neighbouring particles. Force chains are found in the code by these steps:

A LIGGGHTS file containing particle pair interactions is read.
The stress tensor on each particle is calculated based on its interactions with other particles.
Using Eigen Library, magnitude and direction most compressive principal stress is found.
Force chains are found recursively by traversing a particle, its neighbours, neighbours of those neighbours and so on. 
At least three particles form a chain.
The direction of the minor principle must create an angle smaller than alpha with the line of contact with neighbouring particles. The smaller the alpha, the fewer chains will be found. 
The algorithm can find 3D chains with many branches. 

The code offers below Post-processing features:

Exporting the force-chain results in CSV format.
Analysing a specific region.
Analysing a moving region.
Finding the normal vector of a force chain
Chains statistics: average, median, minimum and maximum length, number of chains  


## Examples

To use the code see the examples folder.

## User Interface

The `UserInterface` class is the first place to visit for creating custom code. 

## Get Pair interactions from LIGGGHTS

Add the below line to a LIGGGHTS script:

```
compute         3 all pair/gran/local id pos delta force
```

Then below dump:

```
dump		dmp2 all local ${dmp_stp} ./build/post/pair*.txt c_3[1] c_3[2] c_3[3] c_3[4] c_3[5] c_3[6] c_3[7] c_3[8] c_3[9] c_3[10] c_3[11] c_3[12] c_3[13]
```
The order of output file will be different to `compute` command, it is

```
p1x p1y p1z p2x p2y p2z id1 id2 isPeriodicInteraction f12x f12y f12z overlap
```

where p1 and p2 mean particle 1 and 2 and f12 is the interaction force between them. f12 is the force acting on particle 1.

A script example is in `./examples/liggghtsResult.tar.xz` file.

## Get Wall interactions from LIGGGHTS

Only meshed wall, imported STL files can export interactions.
Therefore the blade particle interactions can be easily extracted.
However, the bed needs to be created with STL mesh. To do so follow these steps:

1- In FreeCad create a 3D box at the size of the bed, the height of it is not important.
2- Import it to Meshlab
3- remove all the triangles, except the ones that create one plane at the size of the bed.
4- Go to filter -> remeshing and ... -> isotropic meshing.
5- Mesh the surface to have 40-100 triangles. 
6- Import in LIGGGHTS run, if it gives errors, remesh with more triangles.


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

## Reference

Code is created based on ideas in the paper below:

2005 Characterization of force chains in granular material by
J. F. Peters,  M. Muthuswamy, J. Wibowo and A. Tordesillas

