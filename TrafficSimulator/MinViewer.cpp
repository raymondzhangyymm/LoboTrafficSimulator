#include "MinViewer.h"

MinViewer::MinViewer(QWidget *parent) : MainViewer(parent)
{

}

//=============================================================================
void MinViewer::draw()
{
    if (!traffic_system_p)
    {
        return;
    }

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    if (viewer_mode == 1)
    {
        setGridIsDrawn(true);
        drawAxis(sceneRadius(), "Density");
        drawRoads(color_road_light);
        drawFieldMeshDensity();
        drawColormapDensity();

        char str[1024];
        sprintf(str, "Density:  %06.2f (ave)  %06.2f (peak)",
                traffic_system_p->density_average, traffic_system_p->density_peak);
        drawFreeTypeBoldOnScreen(2, height()-16, 16, zVec4(0,0,0,1), str);
    }

    else if (viewer_mode == 2)
    {
        setGridIsDrawn(true);
        drawAxis(sceneRadius(), "Velocity");
        drawRoads(color_road_light);
        drawFieldMeshVelocity();
        drawColormapVelocity();

        char str[1024];
        sprintf(str, "Velocity:  %06.2f (ave)  %06.2f (peak)",
                traffic_system_p->velocity_average, traffic_system_p->velocity_peak);
        drawFreeTypeBoldOnScreen(2, height()-16, 16, zVec4(0,0,0,1), str);
    }
}

void MinViewer::mouseDoubleClickEvent(QMouseEvent* e)
{
    restoreCamera();
}

