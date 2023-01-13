#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MorphoTetraDocWidget.h"
#include "PlayerDocWidget.h"
#include <QGroupBox>
#include <QGridLayout>

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    MainWindow();
private :

    MultiMeshViewer *           m_viewer;
    MorphoTetraDocWidget *      m_OptionDocWidget;
    PlayerDocWidget *           m_PlayerDocWidget;

    QDockWidget *               mesherDockWidget;

    void initViewerDockWidgets(MultiMeshViewer* m_viewer);
};

#endif // MAINWINDOW_H
