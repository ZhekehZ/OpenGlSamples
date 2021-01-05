# OpenGL samples [![Build Status](https://travis-ci.com/ZhekehZ/OpenGlSamples.svg?branch=master)](https://travis-ci.com/ZhekehZ/OpenGlSamples)

## 1. [Fractal](https://github.com/ZhekehZ/OpenGlSamples/tree/master/fractal)
[Julia set](https://en.wikipedia.org/wiki/Julia_set) visualization. 

Features: 
  - zoom and navigation (mouse scroll, mouse drag)
  - sliders for changing parameters (radius, number of iterations)
  - palette for color selection
  
Example:
<p align="center"> <img src="https://raw.githubusercontent.com/ZhekehZ/OpenGlSamples/master/fractal/screenshots/example_upd4.png", width=800> </p>
  
  
## 2. [Cubemap](https://github.com/ZhekehZ/OpenGlSamples/tree/master/cubemap)
Visualization of a 3d model from a .obj file.

Features: 
  - zoom and navigation (rotation)
  - simple visualization of color (diffuse light only) and texture
  - cubmap with a menu for background selection
  - reflection + refraction (mixed by Fresnel ([Schlick's approximation](https://en.wikipedia.org/wiki/Schlick%27s_approximation)))
  
Example:
<p align="center"> <img src="https://raw.githubusercontent.com/ZhekehZ/OpenGlSamples/master/cubemap/screenshots/example3.png", width=800> </p>
  

## 3. [Toric landscape](https://github.com/ZhekehZ/OpenGlSamples/tree/master/toric_landscape)
A simple game-like appication.

Features:
  - landscape
    - toric shape
    - generated from a height map
    - textured with a set of tiles
  - car model
    - loaded from .obj file
    - textured and colored (diffused light + emitted light)
    - placed on the surface
  - lights
    - directional (sun) + cascade shadows (two levels)
    - attached to object (headlights)
  - car control (`W` `A` `S` `D`) + zoom
  - movable camera
  - background cubemap
  
  
Example:
<p align="center"> <img src="https://raw.githubusercontent.com/ZhekehZ/OpenGlSamples/master/toric_landscape/screenshots/example4.png", width=800> </p>
 
 
## 4. [Metaballs](https://github.com/ZhekehZ/OpenGlSamples/tree/master/metaballs) 
[Metaballs](https://en.wikipedia.org/wiki/Metaballs) visualization using marching cubes.

Features:
  - visualization using the [marching cube algorithm](http://paulbourke.net/geometry/polygonise/)
  - reflection + refraction (mixed by Fresnel ([Schlick's approximation](https://en.wikipedia.org/wiki/Schlick%27s_approximation)))
  - background cubemap
  

Example:
<p align="center"> <img src="https://github.com/ZhekehZ/OpenGlSamples/raw/metaballs/metaballs/examples/metaballs.gif?raw=true", width=800> </p>
