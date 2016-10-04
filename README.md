# Shade

Shade is a simple ShaderToy-esque GLSL renderer.  It's currently a work in progress.

TODO:
- [x] Rendering with UV coordinates
- [x] Loading fragment shader files
- [ ] Built-in variables (time, resolution, mouse, etc.)
- [ ] dear imgui control of shader parameters
- [ ] Loading textures 
- [ ] Export video


## Compiling

To compile Shade, use CMake:

``` sh
mkdir build && cd build
cmake ..
make
```

This puts the Shade binary at `<root>/shade/shade`