#pragma once
#include "gl.h"
#include "assert.h"
#include <vcg/complex/trimesh/base.h>

struct color{ float r; float b; float g; };

class rotationGizmo
{
public:
    rotationGizmo();
    void    setPosition(   float x, float y, float z       );
    void    setRadius(     float r                         );
    int     getRotation(   int frame                       );
    void    draw(          bool select  = false            );

    inline void hide()                  { visibility = false; }
    inline void show()                  { visibility = true;  }
    inline void selectAxis( int axis )  { assert(axis >0 && axis < 4); selected_axis = axis; }
    inline int  getSelectedAxis()       { return selected_axis; }
    inline void setCubeId(  int cid  )  { cube_id = cid; }
    inline int  getCubeId( )            { return cube_id; }


    void    pickAxis(       float x, float y,
                            float width, float height      );
    void    setAxis(        int axis_id, float x, float y, float z);

    void rotateAlongSelected( int angle );


private:
    color                   colors  [3];
    vcg::Point3f            frame   [3];
    vcg::Point3f            XYZ     [3];
    vcg::Point3f            origin;
    vcg::Matrix44f          _rot;

    bool                    visibility;

    float                   radius;
    int                     selected_axis;
    int                     rotation;
    int                     cube_id;

    void    processHits(    GLint hits, GLuint buffer[]    );
    void    DrawCircle(     float cx, float cy, float cz,
                            float r, int num_segments,
                            int frame, bool select  = false );
    color   buildColor(     int frame                        );

    //void SelectAxis()
};


