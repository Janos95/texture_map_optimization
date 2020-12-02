# texture_map_optimization

# Building
To build you will need the following four dependencies : [assimp](https://github.com/assimp/assimp), [ceres](https://github.com/ceres-solver/ceres-solver),  [corrade](https://github.com/mosra/corrade), [magnum](https://github.com/mosra/magnum) and [magnum-plugins](https://github.com/mosra/magnum-plugins)
If you are on linux, there will most likely be precompiled packages for assimp and ceres provided by your
distros package manager. For the other three deps, I suggest compiling from source.

This project uses cmake to generate project files.
To build using e.g. ninja you can use the following commands
```bash
mkdir build
cmake .. -DCMAKE_BUILD_TYPE=Release -GNinja
ninja
```
# Example
The repository contains an application which can load a mesh and calibrated rgb images.
Be aware that the mesh it is *necessary* for the mesh to have texture coordinates.
If your mesh does not have texture coordinates, I suggest to generate them using e.g.
blenders smart uv unwrap tool.
Once everything is loaded you can average all rgb images into the 
texture and then optimize the individual poses of the rgb images 
to obtain a sharper texture.
The inverse texture mapping and optimization is done using compute shader.
Thus optimizing texture maps, even for large rgb images, should be fast.

Optimize texture colors on the gpu. Below you can see the result of rendering texture coordinates
and applying the inverse texture map to map image colors onto a the texture.
![alt text](https://github.com/Janos95/texture_map_optimization/blob/master/render.png)
![alt text](https://github.com/Janos95/texture_map_optimization/blob/master/texture.png)
