#ifndef LOCALMONITOR
#define LOCALMONITOR

#include <QGLViewer/qglviewer.h>
#include <ParticleSystemTraffic.h>

class LocalMonitor : public QGLViewer
{
    Q_OBJECT

public :
    LocalMonitor(QWidget *parent);
    ~LocalMonitor();

    void setObservationPoint(Node *p_op_);
    void stepObservationPoint();

    Node *p_observation_point;
    bool isBad;

signals:
    void ClosureHit();

protected :
    virtual void init();
    virtual void draw();
    virtual void mousePressEvent(QMouseEvent* e);
//    virtual void mouseMoveEvent(QMouseEvent* e);
//    virtual void mouseReleaseEvent(QMouseEvent* e);
//    virtual void mouseDoubleClickEvent(QMouseEvent* e);

private:
    int bar_height;
    vector<double> realtime_density, realtime_velocity;
};

#endif
