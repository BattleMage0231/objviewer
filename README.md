# ObjViewer

ObjViewer is a cross-platform 3D model viewer built using C++ and OpenGL. Currently, it supports the following features:

- Loading arbitrary `.obj` geometry files with associated `.mtl` material files
- An interactive orbit camera system with yaw/pitch rotation and zooming
- Showing/hiding individual groups defined in the `.obj` file
- Rendering transparent materials using back-to-front sorting
- Clicking on parts of the model to select the corresponding group
- Phong shading with adjustable ambient, diffuse, specular, and emissive components

![](https://i.imgur.com/jqAs5mv.png)

# Building & Dependencies

This project uses the following dependencies in addition to OpenGL: GLFW, GLM, tinyobjloader, Glad, Dear ImGui.

To build the project, run the following commands:

```
git submodule update --init --recursive
mkdir build
cd build
cmake ..
make
```

The executable will then be built at `./objviewer`.

# Usage

```
./objviewer ../examples/bmw.obj
```

Note: example `.obj` and `.mtl` files are taken from [here](https://casual-effects.com/data/index.html).
