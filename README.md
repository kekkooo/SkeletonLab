# SkeletonLab
Interactive Tool to Create, Edit, and Repair Curve-Skeletons

Source code from the paper "Skeleton Lab: an Interactive Tool to Create, Edit, and Repair Curve-Skeletons" 
that  has been awardeed as Best Paper at the conference STAG 2015 held in Verona (October 2015)

Full paper, slides and bibtex entry available at [link](http://francescousai.info/skel_lab.html).

Compiled binaries will be available soon.


## Checkout and compile
First checkout the source code

```
git clone https://github.com/kekkooo/SkeletonLab.git
```

Then setup the .pro file in order to set the necessary paths to include the required libraries:
libQGLViewer. (tested with version 2.5.3 ), CGAL and  Boost

```
INCLUDEPATH *= <path to libQGLViewer includes>/libQGLViewer-2.5.3
LIBS    *= -F<path to QGLViewer framework> -framework QGLViewer

LIBS    += -L<path to CGAL include> -lCGAL
LIBS    += -L<path to Boost include> -lboost_system
```

CGAL and Boost are used to load and interact with meshes, you can however avoid installing these libraries turning off this capabilities commenting the lines
```
DEFINES += use_boost
DEFINES += use_cgal
```

It should compile and run using QT4.8.5 + GCC 4.2 and QT5 and Clang on OsX. It also works on Windows with QT4.8.5 and Visual Studio.

## Gallery
![](http://francescousai.info/assets/img/portfolio/skel_lab.png)
