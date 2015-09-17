#pragma once

#include <assert.h>

using namespace std;

namespace Primitives
{

template <typename T> class Color
{
private :
    T red, green, blue, alpha;

public :

        T  _v[4];

        Color(T r, T g, T b) { Set(r, g, b, 1.0); }
        Color(){ Set(0.0, 0.0, 0.0, 1.0 ); }
        Color(T r, T g, T b, T a) { Set(r, g, b, a); }

        void Set(T r, T g, T b, T a) {
            this->red   = r > 1.0 ? 1.0 : ( r < 0.0 ? 0.0 : r );
            this->green = g > 1.0 ? 1.0 : ( g < 0.0 ? 0.0 : g );
            this->blue  = b > 1.0 ? 1.0 : ( b < 0.0 ? 0.0 : b );
            this->alpha = a > 1.0 ? 1.0 : ( a < 0.0 ? 0.0 : a );
            this->_v[0] = this->red;    this->_v[1] = this->green;
            this->_v[2] = this->blue;   this->_v[3] = this->alpha;
        }

        static Color White()    { return Color( 1.0, 1.0, 1.0 ); }
        static Color Purple()   { return Color( 0.39, 0.18, 0.325 ); }
        static Color Black()    { return Color( 0.0, 0.0, 0.0 );              }
        static Color Red()      { return Color( 1.0, 0.0, 0.0 ); }
        static Color Green()    { return Color( 0.0, 1.0, 0.0 ); }
        static Color Blue()     { return Color( 0.0, 0.0, 1.0 ); }
        static Color Yellow()   { return Color( 1.0, 1.0, 0.0 ); }

        inline static Color Scatter(int n, int a,float Sat=.3f,float Val=.9f)
        {
          int b, k, m=n;
          int r =n;

            for (b=0, k=1; k<n; k<<=1)
                    if (a<<1>=m) {
                        if (b==0) r = k;
                        b += k;
                        a -= (m+1)>>1;
                        m >>= 1;
                    }
            else m = (m+1)>>1;
            if (r>n-b) r = n-b;

            //TRACE("Scatter range 0..%i, in %i out %i\n",n,a,b);
            Color rc;
            rc.SetHSVColor(float(b)/float(n),Sat,Val);
            return rc;
        }

        inline void SetHSVColor( float h, float s, float v)
        {
            float _r, _g, _b;
            if(s==0.0){	// gray color
                _r = _g = _b = v;
                this->red   = (unsigned char)(255*_r);
                this->green = (unsigned char)(255*_g);
                this->blue  = (unsigned char)(255*_b);
                this->alpha = 255;
                return;
            }
            if(h==1.0) h = 0.0;

            int i   = int( floor(h*6.0) );
            float f = float(h*6.0f - floor(h*6.0f));

            float p = v*(1.0f-s);
            float q = v*(1.0f-s*f);
            float t = v*(1.0f-s*(1.0f-f));

            switch(i){
                    case 0: _r = v;     _g = t;     _b = p; break;
                    case 1: _r = q;     _g = v;     _b = p; break;
                    case 2: _r = p;     _g = v;     _b = t; break;
                    case 3: _r = p;     _g = q;     _b = v; break;
                    case 4: _r = t;     _g = p;     _b = v; break;
                    case 5: _r = v;     _g = p;     _b = q; break;
                    default: _r=0;      _g=0;       _b=0; assert(0);break;
                }
            this->Set( _r, _g, _b, 1.0 );
        }

        inline T const * asArray() const { return _v; }

};

typedef Color<float>    ColorF;
typedef Color<double>   ColorD;
}
