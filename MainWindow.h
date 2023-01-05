#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MultiMeshViewer.h"
#include "MorphoTetraDocWidget.h"
#include <QGroupBox>
#include <QGridLayout>

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow();
private :

    MultiMeshViewer *           m_viewer;
    MorphoTetraDocWidget *      m_OptionDocWidget;

    QDockWidget *               mesherDockWidget;

    void initViewerDockWidgets(MultiMeshViewer* m_viewer);
};

#endif // MAINWINDOW_H
