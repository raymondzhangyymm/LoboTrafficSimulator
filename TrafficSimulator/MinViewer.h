#ifndef MINVIEWER_H
#define MINVIEWER_H

#include "MainViewer.h"

class MinViewer : public MainViewer
{
public:
    MinViewer(QWidget *parent);

    virtual void draw();
    virtual void mouseDoubleClickEvent(QMouseEvent* e);
};

#endif // MINVIEWER_H
