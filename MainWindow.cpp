#include "MainWindow.h"

MainWindow::MainWindow(){
    if (this->objectName().isEmpty())
        this->setObjectName("window");

    m_viewer = new MultiMeshViewer(this);
    initViewerDockWidgets(m_viewer);

    m_OptionDocWidget = new MorphoTetraDocWidget(m_viewer,this);
    this->addDockWidget(Qt::RightDockWidgetArea, m_OptionDocWidget);

    m_PlayerDocWidget = new PlayerDocWidget(m_viewer,this);
    this->addDockWidget(Qt::RightDockWidgetArea, m_PlayerDocWidget);

    //Connect sliders for the video
    connect(m_PlayerDocWidget, SIGNAL(doubleValueChanged(double)), m_viewer, SLOT(onPlayerValueChanged(double)));
    connect(m_PlayerDocWidget, SIGNAL(paused()), m_viewer, SLOT(onPlayerPaused()));
    connect(m_PlayerDocWidget, SIGNAL(played()), m_viewer, SLOT(onPlayerPlayed()));
    connect(m_viewer, SIGNAL(updatePlayerTime(double)), m_PlayerDocWidget, SLOT(trySetDoubleValue(double)));
    connect(m_viewer, SIGNAL(pausePlayer()), m_PlayerDocWidget, SLOT(pauseAction()));

    //For the menu
    QMenuBar * menubar = new QMenuBar(this);
    this->setMenuBar(menubar);

    QMenu * menuFile = new QMenu("File", this);
    menubar->addAction(menuFile->menuAction());

    QAction * actionLoadTetraFolder = new QAction("Load Folder datas", this);
    menuFile->addAction(actionLoadTetraFolder);
    connect(actionLoadTetraFolder, SIGNAL(triggered()), this, SLOT(openTetraFolder()));

    QAction * actionLoadInr = new QAction("Load inr", this);
    menuFile->addAction(actionLoadInr);
    connect(actionLoadInr, SIGNAL(triggered()), this, SLOT(openInr()));


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

//    QGroupBox * viewerGroupBox = new QGroupBox("Viewer");
//    QGridLayout * gridLayout = new QGridLayout(viewerGroupBox);
//    gridLayout->setObjectName("gridLayout");
//    gridLayout->addWidget(mesherWidget, 0, 1, 1, 1);
//    viewerGroupBox->setLayout(gridLayout);

    this->setCentralWidget(mesherWidget);
}

void MainWindow::openRemeshingDockWidget(QString filePath)
{
    m_RemeshingDockWidget = new RemeshingDockWidget(m_viewer, filePath, statusBar(), this);
    this->addDockWidget(Qt::TopDockWidgetArea, m_RemeshingDockWidget);
}


void MainWindow::openTetraFolder(){
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    //dialog.setOption(QFileDialog::ShowDirsOnly);
    dialog.setViewMode(QFileDialog::Detail);

    int res = dialog.exec();

    if(res){
        QStringList directoryNames = dialog.selectedFiles();
        QStringList fileNames;
        foreach (QString directoryName, directoryNames) {
            QDir directorySelected = directoryName;
            QStringList fileList = directorySelected.entryList(QDir::Files);
            foreach (QString fileName, fileList) {
                fileNames.append(directorySelected.absoluteFilePath(fileName));
            }
        }

        m_viewer->loadMeshes(statusBar(), fileNames);
    }
}

void MainWindow::openInr() {
    QFileDialog dialog(this);
    dialog.setNameFilter("3D Image (*.inr)");
    dialog.setViewMode(QFileDialog::Detail);
    dialog.setFileMode(QFileDialog::ExistingFile);
    //dialog.selectMimeTypeFilter(".inr");

    if(dialog.exec()){
        QStringList fileNames = dialog.selectedFiles();

        m_viewer->loadMeshes(statusBar(), fileNames);

        openRemeshingDockWidget(fileNames[0]);
    }
}
