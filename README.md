# Volume visualization

This is a demonstration of (3D) volume rendering of volumetric data, e.g. CT or MRI datasets. 

It is written in C#/.NET, and loosely based on a course I took during my Master of Computer Science degree (Visualization group at the University of Bergen, Norway).

The front-end is written in Windows Forms, until I come to my senses. (Likely never - WinForms is excellent for prototyping lightweight Windows GUI applications, sorry to say). 

Direct3D code based on the SharpDX samples of https://github.com/sharpdx/SharpDX-Samples.

## To do:

### UI:

* Histogram in TF editor  
* Color/opacity band in TF editor that shows the current TF parameters  
* Dragging the *lines* and not just the nodes in TF editor  
* Temporarily changing TF on mouse-over of histogram, to hilight the points in the volume that have the intensity in question  
* UI to change contribution of gradients to transfer function
* Threshold selector for first-hit render
* Dynamically update transfer function when selected color is modified
* Select transfer function memoization resolution
* Dataset selector. Additional datasets
* Make the UI less weird at some point. Preferably after renderers are a bit more complete.

### GPU renderer

* Add the remaining rendering algorithms
* Add transfer function support
* Add gradient support
* Integrate TF editor and rendering parameters to renderer
* Phong shading
* Integrate step size in a way that doesn't break the debugger

### Refactoring/performance

* Refactoring rendering algorithms to separate files  
* Refactoring parts of long files into separate classes
* Refactor different rendering algorithms to different shader files
* Make transfer function editor more performant. (Do a profiler run?)

### Software renderer

* Exponential refinement after view has been selected & not interacted with for a bit  
* Performance improvements. Memoization table for voxel lookup? Deferred gradient calculation?
* HSV interpolation rather than RGB interpolation

### Misc

* Fix slightly wonky rotation if convenient  