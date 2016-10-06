# Shade

Shade is a simple ShaderToy-esque GLSL renderer.  It's currently a work in progress.

TODO:
- [x] Rendering with UV coordinates
- [x] Loading fragment shader files
- [x] Live reloading of shaders
- [x] Show framerate
- [x] Built-in variables (time, resolution, mouse)
- [ ] dear imgui control of shader parameters
- [ ] Loading textures 
- [ ] Export video
- [ ] Full ShaderToy compatibility


## Compiling

To compile Shade, use CMake:

``` sh
mkdir build && cd build
cmake ..
make
```

This puts the Shade binary at `<root>/shade/shade`