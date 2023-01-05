#include "MainWindow.h"
#include "MultiMeshViewer.h"

MainWindow::MainWindow(){
    if (this->objectName().isEmpty())
        this->setObjectName("window");

    m_viewer = new MultiMeshViewer(this);
    initViewerDockWidgets(m_viewer);

    m_OptionDocWidget = new MorphoTetraDocWidget(m_viewer,this);
    this->addDockWidget(Qt::RightDockWidgetArea, m_OptionDocWidget);

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

