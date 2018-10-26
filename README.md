# Volume visualization

This is a demonstration of (3D) volume rendering of volumetric data, e.g. CT or MRI datasets. 

It is written in C#/.NET, and loosely based on a course I took during my Master of Computer Science degree (Visualization group at the University of Bergen, Norway). Currently purely CPU-based rendering. I might experiment with pixel shaders in Direct3D if the logic is stable and there is nothing that inspires me more.

The front-end is written in Windows Forms, until I come to my senses. (Likely never - WinForms is excellent for prototyping lightweight Windows GUI applications, sorry to say). 

## To do:

* Transfer function (colors)  
* Transfer function editor (colors & UI improvements)  
* Histogram in TF editor  
* Color/opacity band in TF editor that shows the current TF parameters  
* Dragging the *lines* and not just the nodes in TF editor  
* Temporarily changing TF on mouse-over of histogram, to hilight the points in the volume that have the intensity in question  
* Phong shading  

* Gradient-based transfer function  
* Exponential refinement after view has been selected & not interacted with for a bit  

* Refactoring rendering algorithms to separate files  

* Direct3D acceleration via pixel shaders  
* Fix slightly wonky rotation if convenient  