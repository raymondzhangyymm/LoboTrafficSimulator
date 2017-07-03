#ifndef Kernel_H
#define Kernel_H

#include "global.h"

class Kernel
{
public:
    Kernel();

    static void initKernels(double h_, double h_hk_);

    //--------------------------------------------------------------------------------------
    // 1D kernel
    //--------------------------------------------------------------------------------------
    // Poly Kernel
    static double getPoly1D(double x);
    static double getPoly1DHK(double x);

    // Spiky Kernel Gradient
    static double getSpikyGradient1D(double x);
    static double getSpikyGradient1DHK(double x);

    // Spiky Kernel Laplacian
    static double getSpikyLaplacian1D(double x);
    static double getSpikyLaplacian1DHK(double x);

    //--------------------------------------------------------------------------------------
    // 2D kernel
    //--------------------------------------------------------------------------------------
    // Poly Kernel
    static double getPoly(zVec2 r);
    static double getPoly(double rx, double ry);

//    // Gaussian Kernel
//    static double getGaussian(zVec2 r);
//    static double getGaussian(double rx, double ry);

    // Spiky Kernel
    static zVec2 getSpikyGradient(zVec2 r);
    static zVec2 getSpikyGradientHK(zVec2 r);

    // Viscosity Kernel
    static double getViscosityLaplacian(zVec2 r);
    static double getViscosityLaplacianHK(zVec2 r);
};

#endif

