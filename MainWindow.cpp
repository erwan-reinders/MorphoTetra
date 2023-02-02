#include "MainWindow.h"

MainWindow::MainWindow(){
    if (this->objectName().isEmpty())
        this->setObjectName("window");

    m_viewer = new MultiMeshViewer(this);
    initViewerDockWidgets(m_viewer);

    m_OptionDocWidget = new MorphoTetraDocWidget(m_viewer,this);
    this->addDockWidget(Qt::RightDockWidgetArea, m_OptionDocWidget);

//    m_TopDockWidget = new TopDockWidget(m_viewer, this);
//    this->addDockWidget(Qt::RightDockWidgetArea, m_TopDockWidget);

    m_PlayerDocWidget = new PlayerDocWidget(m_viewer,this);
    this->addDockWidget(Qt::RightDockWidgetArea, m_PlayerDocWidget);

    //Connect sliders for the video
    connect(m_PlayerDocWidget, SIGNAL(doubleValueChanged(double)), m_viewer, SLOT(onPlayerValueChanged(double)));
    connect(m_PlayerDocWidget, SIGNAL(paused()), m_viewer, SLOT(onPlayerPaused()));
    connect(m_PlayerDocWidget, SIGNAL(played()), m_viewer, SLOT(onPlayerPlayed()));
    connect(m_viewer, SIGNAL(updatePlayerTime(double)), m_PlayerDocWidget, SLOT(trySetDoubleValue(double)));
    connect(m_viewer, SIGNAL(pausePlayer()), m_PlayerDocWidget, SLOT(pauseAction()));

    //For the menu
    QAction * actionLoadTetraFolder = new QAction("Load Folder datas", this);
    QMenu * menuFile = new QMenu("File", this);
    menuFile->addAction(actionLoadTetraFolder);
    connect(actionLoadTetraFolder, SIGNAL(triggered()), this, SLOT(openTetraFolder()));

    QMenuBar * menubar = new QMenuBar(this);
    menubar->addAction(menuFile->menuAction());
    this->setMenuBar(menubar);
    QStatusBar *statusbar = new QStatusBar(this);
    this->setStatusBar(statusbar);


    m_PlayerDocWidget->setVisible(true);
    m_OptionDocWidget->setVisible(true);

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


void MainWindow::openTetraFolder(){
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    //dialog.setOption(QFileDialog::ShowDirsOnly);
    dialog.setViewMode(QFileDialog::Detail);

    int res = dialog.exec();

    QDir directorySelected;
    if(res){
        directorySelected = dialog.selectedFiles()[0];
        QStringList fileList = directorySelected.entryList(QDir::Files);

        m_viewer->loadMeshes(statusBar(), fileList, directorySelected);

    }
}
