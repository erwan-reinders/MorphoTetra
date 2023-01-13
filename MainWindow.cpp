#include "MainWindow.h"

MainWindow::MainWindow(){
    if (this->objectName().isEmpty())
        this->setObjectName("window");

    m_viewer = new MultiMeshViewer(this);
    initViewerDockWidgets(m_viewer);

    m_OptionDocWidget = new MorphoTetraDocWidget(m_viewer,this);
    this->addDockWidget(Qt::RightDockWidgetArea, m_OptionDocWidget);

    m_PlayerDocWidget = new PlayerDocWidget(m_viewer,this);
    this->addDockWidget(Qt::BottomDockWidgetArea, m_PlayerDocWidget);

    connect(m_PlayerDocWidget, SIGNAL(doubleValueChanged(double)), m_viewer, SLOT(onPlayerValueChanged(double)));
    connect(m_PlayerDocWidget, SIGNAL(paused()), m_viewer, SLOT(onPlayerPaused()));
    connect(m_PlayerDocWidget, SIGNAL(played()), m_viewer, SLOT(onPlayerPlayed()));
    connect(m_viewer, SIGNAL(updatePlayerTime(double)), m_PlayerDocWidget, SLOT(trySetDoubleValue(double)));
    connect(m_viewer, SIGNAL(pausePlayer()), m_PlayerDocWidget, SLOT(pauseAction()));

    this->setWindowTitle("Morpho Tetra");
}

void MainWindow::initViewerDockWidgets(MultiMeshViewer* m_viewer){
    QWidget * mesherWidget = new QWidget(this);

    QGridLayout * mesherLayout = new QGridLayout();

    mesherLayout->addWidget(m_viewer, 0, 1, 1, 1);

    mesherWidget->setLayout(mesherLayout);

    QGroupBox * viewerGroupBox = new QGroupBox("Viewer");

    QGridLayout * gridLayout = new QGridLayout(viewerGroupBox);
    gridLayout->setObjectName("gridLayout");

    gridLayout->addWidget(mesherWidget, 0, 1, 1, 1);

    viewerGroupBox->setLayout(gridLayout);

    this->setCentralWidget(viewerGroupBox);
}

