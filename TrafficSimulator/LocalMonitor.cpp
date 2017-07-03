#include "LocalMonitor.h"

// Constructor must call the base class constructor.
LocalMonitor::LocalMonitor(QWidget *parent) : QGLViewer(parent)
{
    p_observation_point = NULL;    
    realtime_density.assign(100, 0.0);
    realtime_velocity.assign(100, 0.0);
    bar_height = 20;
    isBad = false;

    init();
}
LocalMonitor::~LocalMonitor()
{
}

void LocalMonitor::init()
{
    glDisable(GL_LIGHTING);
    glClearColor(1.0,1.0,1.0,0);

    updateGL();
}

void LocalMonitor::setObservationPoint(Node *p_op_)
{
    p_observation_point = p_op_;
}

void LocalMonitor::draw()
{
    if (!p_observation_point)
    {
        return;
    }

    // determine the rectangle for displaying text
    int x_width = width();
    int y_height = height() - bar_height;

    // the coordinates of the rect
    int x0 = width() - x_width;
    int y0 = 0;
    int x1 = width();
    int y1 = y_height;

    int half_x_width = x_width / 2;

    startScreenCoordinatesSystem(true);

    // draw the rectangle
    glLineWidth(2.0);
    glColor4f(0.8,0.8,0.8,1);
    glNormal3f(0.0, 0.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex2i(x0, y0);
    glVertex2i(x1, y0);
    glVertex2i(x1, y1);
    glVertex2i(x0, y1);
    glEnd();

    glBegin(GL_LINES);
    glVertex2i(x0+half_x_width, y0);
    glVertex2i(x0+half_x_width, y1);
    glEnd();

    // draw the closure sign
    int closure_len = bar_height - 4;
    int closure_pos_x = width() - closure_len - 3;
    int closure_pos_y = height() - bar_height + 3;
    glLineWidth(2.0);
    glColor4f(0,0,0,1);
    glBegin(GL_LINE_LOOP);
    glVertex2i(closure_pos_x, closure_pos_y);
    glVertex2i(closure_pos_x+closure_len, closure_pos_y);
    glVertex2i(closure_pos_x+closure_len, closure_pos_y+closure_len);
    glVertex2i(closure_pos_x, closure_pos_y+closure_len);
    glEnd();
    glBegin(GL_LINES);
    glVertex2i(closure_pos_x, closure_pos_y);
    glVertex2i(closure_pos_x+closure_len, closure_pos_y+closure_len);
    glVertex2i(closure_pos_x+closure_len, closure_pos_y);
    glVertex2i(closure_pos_x, closure_pos_y+closure_len);
    glEnd();

    double step = double(realtime_density.size()) / double(half_x_width);
    zVec4 color = p_observation_point->color;
    glColor4f(color.x, color.y, color.z, color.a);
    glBegin(GL_LINES);
    for (int i=0; i<half_x_width; ++i)
    {
        int mag = realtime_density[int(step*i)]*y_height;
        glVertex2i(x0+i, y0);
        glVertex2i(x0+i, y0+mag);

        mag = realtime_velocity[int(step*i)]*y_height;
        glVertex2i(x0+half_x_width+i, y0);
        glVertex2i(x0+half_x_width+i, y0+mag);
    }
    glEnd();

    stopScreenCoordinatesSystem();

    // draw text
    QString str;
    QFont qf;

    str.sprintf("Den.:%04.1f", p_observation_point->density);
    qf.setBold(false);
    qf.setPixelSize(16);
    drawText(x0, height()-y_height-2, str, qf);

    str.sprintf("Vel.:%04.1f", p_observation_point->velocity);
    qf.setBold(false);
    qf.setPixelSize(16);
    drawText(x0+half_x_width, height()-y_height-2, str, qf);

    str.sprintf("%i", p_observation_point->id);
    qf.setBold(true);
    qf.setPixelSize(16);
    drawText(closure_pos_x - bar_height*2, height() - closure_pos_y, str, qf);

    // Depth clear is not absolutely needed. An other option would have been to draw the
    // QUAD with a 0.999 z value (z ranges in [0, 1[ with startScreenCoordinatesSystem()).
    //glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_LIGHTING);
}

void LocalMonitor::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        int closure_len = bar_height - 4;
        int closure_pos_x = width() - closure_len - 3;
        int closure_pos_y = height() - bar_height + 3;

        int xx = e->pos().x();
        int yy = height() - e->pos().y();
        if (closure_pos_x <= xx && xx <= closure_pos_x+closure_len &&
            closure_pos_y <= yy && yy <= closure_pos_y+closure_len)
        {
            // the closure sign clicked
            isBad = true;
            emit ClosureHit();
        }
    }
}

void LocalMonitor::stepObservationPoint()
{
    int size = realtime_density.size() - 1;
    for (int i=0; i<size; ++i)
    {
        realtime_density[i] = realtime_density[i+1];
    }
    if (p_observation_point->normalized_density > 1.0)
    {
        realtime_density[size] = 1.0;
    }
    else
    {
        realtime_density[size] = p_observation_point->normalized_density;
    }

    size = realtime_velocity.size() - 1;
    for (int i=0; i<size; ++i)
    {
        realtime_velocity[i] = realtime_velocity[i+1];
    }
    if (p_observation_point->normalized_velocity > 1.0)
    {
        realtime_velocity[size] = 1.0;
    }
    else
    {
        realtime_velocity[size] = p_observation_point->normalized_velocity;
    }
}



