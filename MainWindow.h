#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MorphoTetraDocWidget.h"
#include "PlayerDocWidget.h"
#include <QGroupBox>
#include <QGridLayout>
#include <QMenu>
#include <QFileDialog>
#include <QStatusBar>
#include <QMenuBar>

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

 public slots:
    void openTetraFolder();
};

#endif // MAINWINDOW_H
