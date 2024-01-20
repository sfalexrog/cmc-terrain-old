# cmc-terrain-old

Old version (circa 2017) of the CMC-Terrain demo/task project.

This repository is basically a historical artifact. Still, I'd like to clean up the code a bit, and add stuff like Android/WASM ports that were missing from the original task.

The task description [is available in the `docs/` folder](docs/Task.md).

## Building and running

This project was tested on Ubuntu 16.04 and 22.04, and should hopefully run on anything in between.

Building requires a C++11-compatible compiler, CMake 3.0+ and SDL2.

Running requires an OpenGL 3.3+-compatible GPU. The project uses OpenGL 3.3 Core.

### Building

Create a build directory, run CMake with your preferred generator, then build the project:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

There should now be a `terrain` executable in the current directory.

### Running

The `terrain` executable should be run from the `bin` directory of the repository. In order to do that, run the following from the project root after building:

```bash
cd bin
../build/terrain
```

### Controls

Use `w`, `a`, `s`, `d` to move the camera around. Press and hold the left mouse button while moving the mouse to pan/tilt the camera.

Press `1`, `2`, `3` to switch between different views:

* `1` - display textured view (default);
* `2` - display normals with color;
* `3` - display wireframe + normals as arrows.

Press `z` to show/hide depth map from the light source.

## Scoring

Since the project was initially a CMC task, it implemented the basic task and some additional features:

* base task (10 points):
  * [x] a height map generation algorithm, as well as a landscape coloring (texturing) algorithm;
  * [x] a renderer for the generated landscape that uses OpenGL 3+;
  * [x] textured landscape visualization;
  * [x] normals visualization;
  * [x] lighting calculation;
* additional tasks:
  * sky visualization:
    * [x] using cube or sphere maps - 1 point;
    * [ ] time of day simulation and lightning recalculation using spherical harmonics - up to 6 points depending on how realistic this looks.
  * shadows (with depth map visualization from the light source - otherwise no points will be awarded):
    * [x] landscape and other object shadowing (simple shadow map) - 3 points;
    * [ ] shadow map + PCF - 4 points;
    * [ ] more sophisticated and realistic methods - up to 6 points.
  * water surface visualization:
    * [x] simple translucent water - 1 point;
    * [x] sky reflections - 1 point;
    * [x] objects reflection - 4 points;
    * waves:
      * [x] simple texture-based imitation - 1 point;
      * [ ] realistic water simulation - 4 points;
    * [x] refraction - 2 points.
  * additional object visualization:
    * [ ] trees - 2-4 points depending on how realistic they look;
    * [ ] foilage - 2-4 points depending on how realistic it looks;
    * [ ] stones - 1 point;
    * [ ] houses, cars - 1-4 points depending on how realistic they look;
    * [ ] people and animals - 1-4 points depending on how realistic they look.
  * microrelief imitation:
    * [ ] simple normal mapping - 1 point;
    * [ ] simple normal mapping + mip filtering - 2 points;
    * [ ] parallax occlusion mapping - 4 points;
    * [ ] GPU tessellation - 2-6 points.
  * large-scale visualisation:
    * [x] endless or cyclic landscape - up to 4 points;
    * [x] fog - 1 point;
    * [ ] LODs for parts of the landscape and wireframe visualization - 2 points.
  * [not a goal] using professional packages for terrain generation and importing heightmaps from them - 1-2 points (note thet this still requires implementing a generator);
  * [x] normal visualization with arrows - 2 points
