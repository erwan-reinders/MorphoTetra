#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "MorphoTetraDocWidget.h"
#include "RemeshingDockWidget.h"
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
    RemeshingDockWidget*        m_RemeshingDockWidget;
    PlayerDocWidget *           m_PlayerDocWidget;

    QDockWidget *               mesherDockWidget;

    void initViewerDockWidgets(MultiMeshViewer* m_viewer);

    void openRemeshingDockWidget(QString filePath);

 public slots:
    void openTetraFolder();
    void openInr();
};

#endif // MAINWINDOW_H
