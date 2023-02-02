#ifndef TOPDOCKWIDGET_H
#define TOPDOCKWIDGET_H

#include <QDockWidget>

#include <QWidget>
#include <QCheckBox>
#include <QVBoxLayout>

#include "MultiMeshViewer.h"


class TopDockWidget : public QDockWidget
{
    Q_OBJECT
private:
    MultiMeshViewer*    m_viewer;
public:
    TopDockWidget(MultiMeshViewer* _viewer, QWidget* parent);
};

#endif // TOPDOCKWIDGET_H
