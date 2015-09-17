#pragma once

#include <mesh/base.h>
#include <utilities/myGLutils.h>

namespace QuadLayoutUI
{
    typedef struct
    {
        vcg::Point3f start;
        vcg::Point3f end;

    }myCylinder;

    typedef struct
    {
        vcg::Point3f center;
        int          valence;
    }Singularity;

    class QuadLayout
    {

    public :

        QuadLayout()
        {
            val3        = vcg::Color4b::Red;
            val5        = vcg::Color4b::Blue;
            //val5_plus   = vcg::Color4b::Green;
            val5_plus.SetRGB(17, 133, 18);
            cyl_color.SetRGB(230, 217,39);

        }

        inline void addPiece( vcg::Point3f start, vcg::Point3f end )
        {
            myCylinder cyl;
            cyl.start   = start;
            cyl.end     = end;
            pieces.push_back(cyl);
        }

        inline void addPpoint( vcg::Point3f point, int valence )
        {
            Singularity v;
            v.center     = point;
            v.valence    = valence;
            points.push_back(v);
        }

        inline float getCylinderRadius()        { return cylRadius; }
        inline void  setCylinderRadius(float r) { cylRadius = r; }

        inline float getPointRadius()        { return pointRadius; }
        inline void  setPointRadius(float r) { pointRadius = r; }

        inline int   n_cylinders()          { return pieces.size(); }
        inline int   n_points()             { return points.size(); }

        inline myCylinder getCylinder( int i ){
            return pieces[i];
        }
        inline Singularity getPoint( int i ){
            return points[i];
        }

        void reset(){ pieces.clear(); points.clear(); cylRadius = 0.0; pointRadius = 0.0; }

        void draw( float metric, bool draw_points = true)
        {
            if(draw_points)
            {
                for(int i = 0; i < n_points(); ++i )
                {
                    vcg::Color4b c;
                    if ( points[i].valence == 5 ) c = val5;
                    if ( points[i].valence == 3 ) c = val3;
                    if ( points[i].valence > 5 )  c = val5_plus;

//                    MyGLutils::sphere( points[i].center,metric * 0.7, (points[i].valence == 5) ? val5 : val3 );
                      MyGLutils::sphere( points[i].center,metric*2.0, c );
                }
            }

            for(int i = 0; i < n_cylinders(); ++i )
            {
                //vcg::Point3f dir = pieces[i].end - pieces[i].start;
                //float extralength = dir.Norm();
                MyGLutils::cylinder( pieces[i].start, pieces[i].end, metric/2.0, metric/2.0, cyl_color );
            }

        }

    private :
        std::vector<myCylinder>     pieces;
        std::vector<Singularity>    points;
        vcg::Color4b                val3 ,
                                    val5,
                                    val5_plus;
        vcg::Color4b                cyl_color;
        float                       cylRadius, pointRadius;
    };
}
