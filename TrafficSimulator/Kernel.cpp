#include "Kernel.h"

// The support radius for full and half kernels.
static double h;
static double h_hk;

// The constants for kernels.
static double polyConstant = 0;
static double spikyGradientConstant = 0;
static double viscosityLaplacianConstant = 0;

// The constants for half kernels.
static double polyConstant_hk = 0;
static double spikyGradientConstant_hk = 0;
static double viscosityLaplacianConstant_hk = 0;

// The constants for 1D kernels.
static double polyConstant1D;
static double spikyGradientConstant1D;
static double spikyLaplacianConstant1D;

// The constants for 1D half kernels.
static double polyConstant1D_hk;
static double spikyGradientConstant1D_hk;
static double spikyLaplacianConstant1D_hk;

Kernel::Kernel()
{
}

void Kernel::initKernels(double h_, double h_hk_)
{
    h = h_;
    h_hk = h_hk_;
            
    polyConstant               = 4.0 / (PI * pow(h,8));
    spikyGradientConstant      = -30.0 / (PI * pow(h, 5));
    viscosityLaplacianConstant = 40.0 / (PI * pow(h, 5));

    polyConstant_hk               = 4.0 / (PI * pow(h_hk,8));
    spikyGradientConstant_hk      = -30.0 / (PI * pow(h_hk, 5));
    viscosityLaplacianConstant_hk = 40.0 / (PI * pow(h_hk, 5));

    polyConstant1D               = 35.0 / (32.0 * pow(h, 7));
    spikyGradientConstant1D      = 6.0 / (h*h*h*h);
    spikyLaplacianConstant1D     = 12.0 / (h*h*h*h);

    polyConstant1D_hk            = 35.0 / (32.0 * pow(h_hk, 7));
    spikyGradientConstant1D_hk   = 6.0 / (h_hk * h_hk * h_hk * h_hk);
    spikyLaplacianConstant1D_hk  = 12.0 / (h_hk * h_hk * h_hk * h_hk);
}

//--------------------------------------------------------------------------
// 1D Kernel
//--------------------------------------------------------------------------
double Kernel::getPoly1D(double x)
{
    double length = fabs(x);

    if (length < h)
    {
        double aux = h*h - length*length;
        return polyConstant1D * aux * aux * aux;
    }
    else
    {
        return 0.0;
    }
}
//double Kernel::getPoly1DHK(double x)
//{
//    double length = fabs(x);

//    if (length < h_hk)
//    {
//        double aux = h_hk * h_hk - length*length;
//        return polyConstant1D_hk * aux * aux * aux;
//    }
//    else
//    {
//        return 0.0;
//    }
//}

double Kernel::getSpikyGradient1D(double x)
{
    if (0.0 <= x && x < h)
    {
        double aux = h - x;
        return - spikyGradientConstant1D * aux * aux;
    }
    else if (0.0 > x && x > -h)
    {
        double aux = h + x;
        return spikyGradientConstant1D * aux * aux;
    }
    else
    {
        return 0.0;
    }
}
double Kernel::getSpikyGradient1DHK(double x)
{
    if (0.0 <= x && x < h_hk)
    {
        double aux = h_hk - x;
        return - spikyGradientConstant1D_hk * aux * aux;
    }
    else if (0.0 > x && x > -h_hk)
    {
        double aux = h_hk + x;
        return spikyGradientConstant1D_hk * aux * aux;
    }
    else
    {
        return 0.0;
    }
}

double Kernel::getSpikyLaplacian1D(double x)
{
    double length = fabs(x);

    if (length < h)
    {
        return spikyLaplacianConstant1D * (h - length);
    }
    else
    {
        return 0.0;
    }
}
double Kernel::getSpikyLaplacian1DHK(double x)
{
    double length = fabs(x);

    if (length < h_hk)
    {
        return spikyLaplacianConstant1D_hk * (h_hk - length);
    }
    else
    {
        return 0.0;
    }
}

//--------------------------------------------------------------------------
// 2D Kernel
//--------------------------------------------------------------------------
// poly 6 Kernel
double Kernel::getPoly(zVec2 r)
{
    double length2 = r.x*r.x + r.y*r.y;
    double h2 = h*h;

    if (length2 < h2)
    {
        double aux = h2 - length2;
        return polyConstant * aux * aux * aux;
    }
    else
    {
        return 0.0;
    }
}

double Kernel::getPoly(double rx, double ry)
{
    double length2 = rx*rx + ry*ry;
    double h2 = h*h;

    if (length2 < h2)
    {
        double aux = h2 - length2;
        return polyConstant * aux * aux * aux;
    }
    else
    {
        return 0.0;
    }
}

//// gaussian Kernel
//double Kernel::getGaussian(zVec2 r)
//{
//    double length = r.length();

//    if (length <= h)
//    {
//        double aux = length / h;
//        return gaussianConstant * exp( - 4.5 * aux * aux);
//    }
//    else
//    {
//        return 0.0;
//    }
//}

//double Kernel::getGaussian(double rx, double ry)
//{
//    double length = sqrt(rx*rx + ry*ry);

//    if (length <= h)
//    {
//        double aux = length / h;
//        return gaussianConstant * exp( - 4.5 * aux * aux);
//    }
//    else
//    {
//        return 0.0;
//    }
//}

// Spiky Kernel
zVec2 Kernel::getSpikyGradient(zVec2 r)
{
    zVec2 w;
    double length = r.length();

    if (length < h)
    {
        if (length != 0.0)
        {
            double aux = h - length;
            w = r * (spikyGradientConstant * aux * aux / length);
        }
    }

    return w;
}
zVec2 Kernel::getSpikyGradientHK(zVec2 r)
{
    zVec2 w;
    double length = r.length();

    if (length < h_hk)
    {
        if (length != 0.0)
        {
            double aux = h_hk - length;
            w = r * (spikyGradientConstant_hk * aux * aux / length);
        }
    }

    return w;
}

// Viscosity Kernel
double Kernel::getViscosityLaplacian(zVec2 r)
{
    double length = r.length();

    if (length < h)
    {
        return viscosityLaplacianConstant * (h - length);
    }
    else
    {
        return 0.0;
    }
}
double Kernel::getViscosityLaplacianHK(zVec2 r)
{
    double length = r.length();

    if (length < h_hk)
    {
        return viscosityLaplacianConstant_hk * (h_hk - length);
    }
    else
    {
        return 0.0;
    }
}

