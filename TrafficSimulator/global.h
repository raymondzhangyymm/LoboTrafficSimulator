#ifndef GLOBAL
#define GLOBAL

#include <QtOpenGL>
#include "GL/GL.h"
#include "GL/GLU.h"
#include <map>
#include <vector>
#include <list>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <time.h>
#include <assert.h>
#include <math.h>
#include <random>


#define PI 3.14159265
#define HASH_FUNCTION_PRIME_1 73856093
#define HASH_FUNCTION_PRIME_2 32452843 //19349663
#define HASH_FUNCTION_PRIME_3 83492791

#define DEFAULT_LANE_CELL_LENGTH 9
#define DEFAULT_LANE_WIDTH 3.0
#define DEFAULT_VEHICLE_LENGTH 4.5

#define CONSTRAIN_SPEED 0.0

using namespace std;

//My vector class definition
class zVec2
{
public:

    // constructors
    zVec2()
    {
        x = 0; y = 0;
    }
    zVec2(double xx, double yy)
    {
        x = xx; y = yy;
    }

    // components
    double x, y;

    // member functions
    // output values
    inline friend std::ostream &operator<<(std::ostream &os, const zVec2 &v)
    {
        os << '(' << v.x << ',' << v.y << ')';
        return os;
    }

    // set to zeros
    inline void clear(void)
    {
        x = 0; y = 0;
    }

    // length
    inline double length(void)
    {
        return sqrt(x*x + y*y);
    }

    // inner product
    inline double dotproduct(const zVec2 &v)
    {
        return x*v.x + y*v.y;
    }

    // normalize
    inline void normalize()
    {
        double len = sqrt(x*x + y*y);
        x /= len;
        y /= len;
    }

    // sign of the cross product (v0 X v1)
    static int signofCrossProduct(const zVec2 &v0, const zVec2 &v1)
    {
        double ss = v0.x*v1.y - v0.y*v1.x;
        if (ss > 0)
        {
            return 1;
        }
        else if (ss < 0)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }

    //-------------------------------------------------------------
    // subscript
    //-------------------------------------------------------------
    inline double& operator[](const int &idx)
    {
        if (idx == 0)
            return x;
        else if (idx == 1)
            return y;
        else
            assert("zVec2[] Access error!");
        return x;
    }

    //-------------------------------------------------------------
    // assignment
    //-------------------------------------------------------------
    inline zVec2& operator=(const zVec2 &v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }
    inline zVec2& operator+=(const zVec2 &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }
    inline zVec2& operator-=(const zVec2 &v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }
    inline zVec2& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }
    inline zVec2& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    //-------------------------------------------------------------
    // unary
    //-------------------------------------------------------------
    inline zVec2& operator+()
    {
        return *this;
    }
    inline zVec2 operator-()
    {
        return zVec2(-x, -y);
    }

    //-------------------------------------------------------------
    // arithmetic
    //-------------------------------------------------------------
    inline zVec2 operator+(const zVec2 &v)
    {
        return zVec2(x+v.x, y+v.y);
    }
    inline zVec2 operator-(const zVec2 &v)
    {
        return zVec2(x-v.x, y-v.y);
    }
    inline zVec2 operator*(float scalar)
    {
        return zVec2(x*scalar, y*scalar);
    }
    inline zVec2 operator/(float scalar)
    {
        return zVec2(x/scalar, y/scalar);
    }
};

class zVec3
{
public:

    // constructors
    zVec3()
    {
        x = 0; y = 0; z = 0;
    }
    zVec3(double xx, double yy, double zz)
    {
        x = xx; y = yy; z = zz;
    }

    // components
    double x, y, z;

    // member functions
    // output values
    inline friend std::ostream &operator<<(std::ostream &os, const zVec3 &v)
    {
        os << '(' << v.x << ',' << v.y << ',' << v.z << ')';
        return os;
    }

    // set to zeros
    inline void clear(void)
    {
        x = 0; y = 0; z = 0;
    }

    // length
    inline double length(void)
    {
        return sqrt(x*x + y*y + z*z);
    }

    // inner product
    static double dotproduct(const zVec3 &v1, const zVec3 &v2)
    {
        return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
    }

    // normalize
    static zVec3 normalize(const zVec3 &v)
    {
        double len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
        return zVec3(v.x/len, v.y/len, v.z/len);
    }

    // compute the normal of triangle
    static zVec3 normalOfTriangle(const zVec3 &v1, const zVec3 &v2, const zVec3 &v3)
    {
        zVec3 u, v;

        u.x = v2.x - v1.x;
        u.y = v2.y - v1.y;
        u.z = v2.z - v1.z;

        v.x = v3.x - v1.x;
        v.y = v3.y - v1.y;
        v.z = v3.z - v1.z;

        zVec3 nl;
        nl.x = u.y*v.z - u.z*v.y;
        nl.y = u.z*v.x - u.x*v.z;
        nl.z = u.x*v.y - u.y*v.x;

        return nl/nl.length();
    }

    //-------------------------------------------------------------
    // subscript
    //-------------------------------------------------------------
    inline double& operator[](const int &idx)
    {
        if (idx == 0)
            return x;
        else if (idx == 1)
            return y;
        else if (idx == 2)
            return z;
        else
            assert("zVec3[] Access error!");
    }

    //-------------------------------------------------------------
    // assignment
    //-------------------------------------------------------------
    inline zVec3& operator=(const zVec3 &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        return *this;
    }
    inline zVec3& operator+=(const zVec3 &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }
    inline zVec3& operator-=(const zVec3 &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }
    inline zVec3& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }
    inline zVec3& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    //-------------------------------------------------------------
    // unary
    //-------------------------------------------------------------
    inline zVec3& operator+()
    {
        return *this;
    }
    inline zVec3 operator-()
    {
        return zVec3(-x, -y, -z);
    }

    //-------------------------------------------------------------
    // arithmetic
    //-------------------------------------------------------------
    inline zVec3 operator+(const zVec3 &v)
    {
        return zVec3(x+v.x, y+v.y, z+v.z);
    }
    inline zVec3 operator-(const zVec3 &v)
    {
        return zVec3(x-v.x, y-v.y, z-v.z);
    }
    inline zVec3 operator*(float scalar)
    {
        return zVec3(x*scalar, y*scalar, z*scalar);
    }
    inline zVec3 operator/(float scalar)
    {
        return zVec3(x/scalar, y/scalar, z/scalar);
    }
};

class zVec4
{
public:

    // constructors
    zVec4()
    {
        x = 0; y = 0; z = 0; a = 0;
    }
    zVec4(double xx, double yy, double zz, double aa)
    {
        x = xx; y = yy; z = zz; a = aa;
    }

    // components
    double x, y, z, a;

    // member functions
    // output values
    inline friend std::ostream &operator<<(std::ostream &os, const zVec4 &v)
    {
        os << '(' << v.x << ',' << v.y << ',' << v.z << ',' << v.a << ')';
        return os;
    }

    // set to zeros
    inline void clear(void)
    {
        x = 0; y = 0; z = 0; a = 0;
    }

    // length
    inline double length(void)
    {
        return sqrt(x*x + y*y + z*z + a*a);
    }

    // inner product
    static double dotproduct(const zVec4 &v1, const zVec4 &v2)
    {
        return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.a*v2.a;
    }

    // normalize
    static zVec4 normalize(const zVec4 &v)
    {
        double len = sqrt(v.x*v.x + v.y*v.y + v.z*v.z + v.a*v.a);
        return zVec4(v.x/len, v.y/len, v.z/len, v.a/len);
    }

    //-------------------------------------------------------------
    // subscript
    //-------------------------------------------------------------
    inline double& operator[](const int &idx)
    {
        if (idx == 0)
            return x;
        else if (idx == 1)
            return y;
        else if (idx == 2)
            return z;
        else if (idx == 3)
            return a;
        else
            assert("zVec4[] Access error!");
    }

    //-------------------------------------------------------------
    // assignment
    //-------------------------------------------------------------
    inline zVec4& operator=(const zVec4 &v)
    {
        x = v.x;
        y = v.y;
        z = v.z;
        a = v.a;
        return *this;
    }
    inline zVec4& operator+=(const zVec4 &v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
        a += v.a;
        return *this;
    }
    inline zVec4& operator-=(const zVec4 &v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        a -= v.a;
        return *this;
    }
    inline zVec4& operator*=(float scalar)
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        a *= scalar;
        return *this;
    }
    inline zVec4& operator/=(float scalar)
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        a /= scalar;
        return *this;
    }

    //-------------------------------------------------------------
    // unary
    //-------------------------------------------------------------
    inline zVec4& operator+()
    {
        return *this;
    }
    inline zVec4 operator-()
    {
        return zVec4(-x, -y, -z, -a);
    }

    //-------------------------------------------------------------
    // arithmetic
    //-------------------------------------------------------------
    inline zVec4 operator+(const zVec4 &v)
    {
        return zVec4(x+v.x, y+v.y, z+v.z, a+v.a);
    }
    inline zVec4 operator-(const zVec4 &v)
    {
        return zVec4(x-v.x, y-v.y, z-v.z, a-v.a);
    }
    inline zVec4 operator*(float scalar)
    {
        return zVec4(x*scalar, y*scalar, z*scalar, a*scalar);
    }
    inline zVec4 operator/(float scalar)
    {
        return zVec4(x/scalar, y/scalar, z/scalar, a/scalar);
    }
};

enum SelectionGroup {nothing, particle, observation, spline, block, road};

enum MapElementType {highway, building, railway,
                     beziers, otherlinks};  // beziers means makeup roads.

enum HighwayType {trunk, trunk_link, motorway, motorway_link, primary, primary_link,
                  secondary, tertiary, residential, service, footway, unclassified,
                  path, otherhighways};
// Others like steps, cycleway, track, pedestrian, construction are not considered.

class TextImage
{
public:
    GLuint texture_id;
    char ch;
//    zVec2 bitmapsize;
    zVec2 size;
    zVec2 bearing;
    GLuint advance;
};

typedef vector<zVec2> PointArray;

class TaxiNode
{
public:
    TaxiNode(){}

    // the raw data
    string company_name;
    long long id;
    string plate_number;
    string timestamp;
    double longitude;
    double latitude;
    double speed;
    bool state_1;
    bool state_2;
    double angle_to_north;
    string vehicle_type;

    // the data converted
    double timemark;
    zVec2 position;
};

#endif // GLOBAL

