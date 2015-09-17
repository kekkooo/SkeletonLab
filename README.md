# SkeletonLab
Interactive Tool to Create, Edit, and Repair Curve-Skeletons

Source code from the paper "Skeleton Lab: an Interactive Tool to Create, Edit, and Repair Curve-Skeletons".

Full paper, slides and bibtex entry will be available for download after its definitive acceptance to STAG 2015.

Compiled binaries will be available soon.


## Checkout and compile
First checkout the source code

```
git clone https://github.com/kekkooo/SkeletonLab.git
```

Then setup the necessary paths to include the needed libraries:
libQGLViewer. (tested with version 2.5.3 ), CGAL and  Boost

```
INCLUDEPATH *= <path to libQGLViewer includes>/libQGLViewer-2.5.3
LIBS    *= -F<path to QGLViewer framework> -framework QGLViewer

LIBS    += -L<path to CGAL include> -lCGAL
LIBS    += -L<path to Boost include> -lboost_system
```

If you do not want to install CGAL and Boost you can turn off mesh loading capabilities removing:
```
DEFINES += use_boost
DEFINES += use_cgal
```

It should compile and run using QT4.8.5 + GCC 4.2 and QT5 and Clang on OXX. It sould work also on Windows with QT4.8.5 and Visual Studio.

## Gallery
![](http://francescousai.info/assets/img/portfolio/skel_lab.png)
