#ifndef SEQM_HELPER_H
#define SEQM_HELPER_H

namespace SEQM_helper{
class SEQM_Point {
public:
    inline SEQM_Point (float x = 0.f, float y = 0.f, float z = 0.f) {
        p[0] = x;
        p[1] = y;
        p[2] = z;
    }
    inline float & operator[] (unsigned int i) { return p[i]; }
    inline const float & operator[] (unsigned int i) const { return p[i]; }
    inline SEQM_Point& operator= (const SEQM_Point & P) {
      p[0] = P[0];
      p[1] = P[1];
      p[2] = P[2];
      return (*this);
    };

    inline SEQM_Point& operator+= (const SEQM_Point & P) {
      p[0] += P[0];
      p[1] += P[1];
      p[2] += P[2];
      return (*this);
    };

    inline SEQM_Point& operator-= (const SEQM_Point & P) {
      p[0] -= P[0];
      p[1] -= P[1];
      p[2] -= P[2];
      return (*this);
    };

    inline SEQM_Point& operator*= (const SEQM_Point & P) {
      p[0] *= P[0];
      p[1] *= P[1];
      p[2] *= P[2];
      return (*this);
    };

    inline SEQM_Point& operator*= (float s) {
      p[0] *= s;
      p[1] *= s;
      p[2] *= s;
      return (*this);
    };

    inline SEQM_Point& operator/= (const SEQM_Point & P) {
      p[0] /= P[0];
      p[1] /= P[1];
      p[2] /= P[2];
      return (*this);
    };

    inline SEQM_Point& operator/= (float s) {
      p[0] /= s;
      p[1] /= s;
      p[2] /= s;
      return (*this);
    };

    inline SEQM_Point operator+ (const SEQM_Point & P) const {
      SEQM_Point res;
      res[0] = p[0] + P[0];
      res[1] = p[1] + P[1];
      res[2] = p[2] + P[2];
      return (res);
    };

    inline SEQM_Point operator- (const SEQM_Point & P) const {
      SEQM_Point res;
      res[0] = p[0] - P[0];
      res[1] = p[1] - P[1];
      res[2] = p[2] - P[2];
      return (res);
    };

    inline SEQM_Point operator- () const {
      SEQM_Point res;
      res[0] = -p[0];
      res[1] = -p[1];
      res[2] = -p[2];
      return (res);
    };

    inline SEQM_Point operator* (const SEQM_Point & P) const {
      SEQM_Point res;
      res[0] = p[0] * P[0];
      res[1] = p[1] * P[1];
      res[2] = p[2] * P[2];
      return (res);
    };

    inline SEQM_Point operator* (float s) const {
      SEQM_Point res;
      res[0] = p[0] * s;
      res[1] = p[1] * s;
      res[2] = p[2] * s;
      return (res);
    };

    inline SEQM_Point operator/ (const SEQM_Point & P) const {
      SEQM_Point res;
      res[0] = p[0] / P[0];
      res[1] = p[1] / P[1];
      res[2] = p[2] / P[2];
      return (res);
    };

    inline SEQM_Point operator/ (float s) const {
      SEQM_Point res;
      res[0] = p[0] / s;
      res[1] = p[1] / s;
      res[2] = p[2] / s;
      return (res);
    };

    inline bool operator == (const SEQM_Point & a) const {
      return(p[0] == a[0] && p[1] == a[1] && p[2] == a[2]);
    };

    inline bool operator != (const SEQM_Point & a) const {
      return(p[0] != a[0] || p[1] != a[1] || p[2] != a[2]);
    };

    inline bool operator < (const SEQM_Point & a) const {
      return(p[0] < a[0] && p[1] < a[1] && p[2] < a[2]);
    };

    inline bool operator >= (const SEQM_Point &a) const {
      return(p[0] >= a[0] && p[1] >= a[1] && p[2] >= a[2]);
    };
private:
    float p[3];
};

inline SEQM_Point operator * (float s, const SEQM_Point & p) {
    return (p * s);
}

inline float dot (const SEQM_Point & a, const SEQM_Point & b) {
    return (a[0] * b[0] + a[1] * b[1] + a[2] * b[2]);
}

inline float length (const SEQM_Point & x) {
    return sqrt (dot (x, x));
}

inline SEQM_Point normalize (const SEQM_Point & x) {
    SEQM_Point n (x);
    n /= length (n);
    return n;
}
}

#endif // SEQM_HELPER_H
