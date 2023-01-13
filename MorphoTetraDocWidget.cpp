#include "MorphoTetraDocWidget.h"

MorphoTetraDocWidget::MorphoTetraDocWidget(MultiMeshViewer * _viewer, QWidget * parent)  : QDockWidget(parent){
    m_viewer = _viewer;
    QWidget * contents = new QWidget();
    QVBoxLayout * contentLayout = new QVBoxLayout(contents);
    setSubdomainsElement(contentLayout);

    this->setWidget(contents);
}

MorphoTetraDocWidget::~MorphoTetraDocWidget(){}

void MorphoTetraDocWidget::setSubdomainsElement(QVBoxLayout * contentLayout){

    QGroupBox * groupBox = new QGroupBox("Cutting plane", this);
    groupBox->setMaximumSize(QSize(16777215, 200));

    contentLayout->addWidget ( groupBox) ;

    QGridLayout * cuttingPlaneGridLayout = new QGridLayout(groupBox);
    xHSlider = new QSlider(groupBox);
    xHSlider->setOrientation(Qt::Horizontal);

    cuttingPlaneGridLayout->addWidget(xHSlider, 1, 0, 1, 1);

    yHSlider = new QSlider(groupBox);
    yHSlider->setOrientation(Qt::Horizontal);

    cuttingPlaneGridLayout->addWidget(yHSlider, 3, 0, 1, 1);

    zHSlider = new QSlider(groupBox);
    zHSlider->setOrientation(Qt::Horizontal);

    cuttingPlaneGridLayout->addWidget(zHSlider, 5, 0, 1, 1);

    QPushButton * invertXPushButton = new QPushButton("invert", groupBox);
    cuttingPlaneGridLayout->addWidget(invertXPushButton, 1, 1, 1, 1);

    QPushButton * invertYPushButton = new QPushButton("invert", groupBox);
    cuttingPlaneGridLayout->addWidget(invertYPushButton, 3, 1, 1, 1);

    QPushButton * invertZPushButton = new QPushButton("invert", groupBox);
    cuttingPlaneGridLayout->addWidget(invertZPushButton, 5, 1, 1, 1);

    QLabel * labelCutX = new QLabel("x cut position", groupBox);
    cuttingPlaneGridLayout->addWidget(labelCutX, 0, 0, 1, 1);

    QLabel * labelCutY = new QLabel("y cut position", groupBox);
    cuttingPlaneGridLayout->addWidget(labelCutY, 2, 0, 1, 1);

    QLabel * labelCutZ = new QLabel("z cut position", groupBox);
    cuttingPlaneGridLayout->addWidget(labelCutZ, 4, 0, 1, 1);

    QCheckBox * displayXCut = new QCheckBox("display", groupBox);
    cuttingPlaneGridLayout->addWidget(displayXCut, 0, 1, 1, 1);

    QCheckBox * displayYCut = new QCheckBox("display", groupBox);
    cuttingPlaneGridLayout->addWidget(displayYCut, 2, 1, 1, 1);

    QCheckBox * displayZCut = new QCheckBox("display", groupBox);
    cuttingPlaneGridLayout->addWidget(displayZCut, 4, 1, 1, 1);




    m_displaySubdomainsBox = new QGroupBox("Label display", this);

    QVBoxLayout * segIVLayout = new QVBoxLayout(m_displaySubdomainsBox);

    QFrame * segIFrame = new QFrame();
    m_segIGridLayout = new QGridLayout(segIFrame);

    QScrollArea * displayIScrollArea = new QScrollArea(m_displaySubdomainsBox);
    displayIScrollArea->setWidget(segIFrame);

    displayIScrollArea->setWidgetResizable(true);
    displayIScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    segIVLayout->addWidget(displayIScrollArea);
    m_signalIMapper = new QSignalMapper(m_displaySubdomainsBox);


    QGridLayout * segIGridKayout = new QGridLayout();

    QPushButton *selectIPushButton = new QPushButton("Select All", m_displaySubdomainsBox);
    segIGridKayout->addWidget(selectIPushButton, 0, 0, 1, 1);

    QPushButton *discardIPushButton = new QPushButton("Discard All", m_displaySubdomainsBox);
    segIGridKayout->addWidget(discardIPushButton, 0, 1, 1, 1);

    // ========= CHECKBOX
    QCheckBox * setDrawMeshBox = new QCheckBox("Draw Mesh", m_displaySubdomainsBox);
    setDrawMeshBox->setChecked(true);
    segIGridKayout->addWidget(setDrawMeshBox, 1, 0, 1, 2);

    QCheckBox * setDrawVerticesBox = new QCheckBox("Draw Vertices", m_displaySubdomainsBox);
    setDrawVerticesBox->setChecked(false);
    segIGridKayout->addWidget(setDrawVerticesBox, 2, 0, 1, 2);



    //QCheckBox * setDrawPolylinesBox = new QCheckBox("Draw polylines", m_displaySubdomainsBox);
    //setDrawPolylinesBox->setChecked(false);
    //segIGridKayout->addWidget(setDrawPolylinesBox, 3, 0, 1, 2);

    QGroupBox * polylineDrawModeGroup = new QGroupBox("Draw polylines", m_displaySubdomainsBox);
    polylineDrawModeGroup->setCheckable(true);
    polylineDrawModeGroup->setChecked(false);
    segIGridKayout->addWidget(polylineDrawModeGroup, 4, 0, 2, 2);

    QVBoxLayout * polylineDrawModeGroupLayout = new QVBoxLayout(polylineDrawModeGroup);
    QRadioButton * linearModeRadio = new QRadioButton("Linear");
    QRadioButton * CRModeRadio = new QRadioButton("Cattmull-Rom");
    linearModeRadio->setChecked(true);
    polylineDrawModeGroupLayout->addWidget(linearModeRadio);
    polylineDrawModeGroupLayout->addWidget(CRModeRadio);




    segIVLayout->addLayout(segIGridKayout);

    segIVLayout->addStretch(0);

    connect(discardIPushButton, SIGNAL(clicked()), this, SLOT(discardAll()));
    connect(selectIPushButton, SIGNAL(clicked()), this, SLOT(selectAll()));
    connect(setDrawMeshBox, SIGNAL(stateChanged(int)), this, SLOT(setDrawMesh(int)));
    connect(setDrawVerticesBox, SIGNAL(stateChanged(int)), this, SLOT(setDrawVertices(int)));
    connect(polylineDrawModeGroup, SIGNAL(clicked(bool)), this, SLOT(setDrawPolylines(bool)));
    connect(linearModeRadio, SIGNAL(clicked(bool)), this, SLOT(setPolylineModeLinear(bool)));
    connect(CRModeRadio, SIGNAL(clicked(bool)), this, SLOT(setPolylineModeSmooth(bool)));


    connect(m_viewer,SIGNAL(setMaxCutPlanes(int,int,int)), this, SLOT(setMaxCutPlanes(int,int,int)));
    connect(xHSlider,SIGNAL(valueChanged(int)), m_viewer, SLOT(setXCut(int)));
    connect(yHSlider,SIGNAL(valueChanged(int)), m_viewer, SLOT(setYCut(int)));
    connect(zHSlider,SIGNAL(valueChanged(int)), m_viewer, SLOT(setZCut(int)));

    connect(invertXPushButton,SIGNAL(clicked()), m_viewer, SLOT(invertXCut()));
    connect(invertYPushButton,SIGNAL(clicked()), m_viewer, SLOT(invertYCut()));
    connect(invertZPushButton,SIGNAL(clicked()), m_viewer, SLOT(invertZCut()));

    connect(displayXCut,SIGNAL(toggled(bool)), m_viewer, SLOT(setXCutDisplay(bool)));
    connect(displayYCut,SIGNAL(toggled(bool)), m_viewer, SLOT(setYCutDisplay(bool)));
    connect(displayZCut,SIGNAL(toggled(bool)), m_viewer, SLOT(setZCutDisplay(bool)));


    connect(m_viewer, SIGNAL(setMeshSubdomains()), this, SLOT(setMeshSubDomains()));
    contentLayout->addWidget(m_displaySubdomainsBox);

    contentLayout->addStretch(0);
}


// ==== Gestion des sous-domaines ====
void MorphoTetraDocWidget::setMeshSubDomains(){
    const std::map<Subdomain_index, QColor> colorMap = m_viewer->getColorMap();
    addMeshSubDomains(colorMap);
    selectAll();
}

void MorphoTetraDocWidget::addMeshSubDomains(const std::map<Subdomain_index, QColor> & colors){
    m_indexIToLabel.clear();

    int i = 0;
    for (std::map<Subdomain_index, QColor>::const_iterator it = colors.begin(); it != colors.end() ; ++it , i++){
        m_indexIToLabel.push_back(it->first);
        QColor color = it->second;

        if(m_labelICheckButtons.size() > i ){
            m_imageLabels[i]->setPalette(color);
            m_imageLabels[i]->setVisible(true);
            m_labelICheckButtons[i]->setVisible(true);
        } else {

            QLabel * labelText  = new QLabel(QString::number(it->first));
            m_segIGridLayout->addWidget(labelText, i, 0, 1, 1);

            QCheckBox * labelICheckBox = new QCheckBox(m_displaySubdomainsBox);
            m_segIGridLayout->addWidget(labelICheckBox, i, 2, 1, 1);

            QLabel * labelColor = new QLabel(m_displaySubdomainsBox);
            labelColor->setPalette(color);
            labelColor->setAutoFillBackground(true);

            m_segIGridLayout->addWidget(labelColor, i, 1, 1, 1);

            m_imageLabels.push_back(labelColor);
            m_labelICheckButtons.push_back(labelICheckBox);

            connect(labelICheckBox, SIGNAL(clicked()), m_signalIMapper, SLOT (map()));
            m_signalIMapper->setMapping(labelICheckBox, i);
        }
    }

    if( (unsigned int)m_labelICheckButtons.size() > colors.size() ){
        for( int b = colors.size() ; b < m_labelICheckButtons.size() ; b++ ){
            m_labelICheckButtons[b]->setVisible(false);
            m_imageLabels[b]->setVisible(false);
        }
    }

    connect(m_signalIMapper,SIGNAL(mappedInt(int)), this, SLOT(setVisibility(int)));
    m_displaySubdomainsBox->adjustSize();

}

void MorphoTetraDocWidget::setVisibility(int i){
    if(i < (int)m_indexIToLabel.size())
        m_viewer->setVisibility(m_indexIToLabel.at(i), m_labelICheckButtons[i]->isChecked());
}

void MorphoTetraDocWidget::selectAll(){
    for (int i = 0;i<m_labelICheckButtons.size();i++){
        if(!m_labelICheckButtons[i]->isChecked()){
            m_labelICheckButtons[i]->setChecked(true);
        }
    }
    m_viewer->selectAll();
}

void MorphoTetraDocWidget::discardAll(){
    for (int i = 0;i<m_labelICheckButtons.size();i++){
        if(m_labelICheckButtons[i]->isChecked()){
            m_labelICheckButtons[i]->setChecked(false);
        }
    }
    m_viewer->discardAll();
}

void MorphoTetraDocWidget::setDrawMesh(int state) {
    m_viewer->setDrawMesh(state);
}

void MorphoTetraDocWidget::setDrawVertices(int state) {
    m_viewer->setDrawVertices(state);
}

void MorphoTetraDocWidget::setDrawPolylines(bool state){
    m_viewer->setDrawPolylines(state);
}

void MorphoTetraDocWidget::setPolylineModeLinear(bool state) {
    if (state) {
        m_viewer->setPolylineDrawMode(0);
    }
}

void MorphoTetraDocWidget::setPolylineModeSmooth(bool state) {
    if (state) {
        m_viewer->setPolylineDrawMode(1);
    }
}

void MorphoTetraDocWidget::setMaxCutPlanes( int x, int y , int z ){
    xHSlider->setRange(0,x);
    yHSlider->setRange(0,y);
    zHSlider->setRange(0,z);
}
