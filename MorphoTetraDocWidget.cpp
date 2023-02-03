#include "MorphoTetraDocWidget.h"

MorphoTetraDocWidget::MorphoTetraDocWidget(MultiMeshViewer * _viewer, QWidget * parent)  : QDockWidget(parent){
    m_viewer = _viewer;
    QWidget * contents = new QWidget();
    QHBoxLayout * contentLayout = new QHBoxLayout(contents);
    setSubdomainsElement(contentLayout);

    this->setWidget(contents);
}

MorphoTetraDocWidget::~MorphoTetraDocWidget(){}

void MorphoTetraDocWidget::setSubdomainsElement(QBoxLayout * contentLayout){

    // === Cutting plane ===

    QGroupBox * planeGroupBox = new QGroupBox("Cutting plane", this);

    contentLayout->addWidget(planeGroupBox);

    QGridLayout * cuttingPlaneGridLayout = new QGridLayout(planeGroupBox);
    xHSlider = new QSlider(planeGroupBox);
    xHSlider->setOrientation(Qt::Horizontal);

    cuttingPlaneGridLayout->addWidget(xHSlider, 1, 0, 1, 1);

    yHSlider = new QSlider(planeGroupBox);
    yHSlider->setOrientation(Qt::Horizontal);

    cuttingPlaneGridLayout->addWidget(yHSlider, 3, 0, 1, 1);

    zHSlider = new QSlider(planeGroupBox);
    zHSlider->setOrientation(Qt::Horizontal);

    cuttingPlaneGridLayout->addWidget(zHSlider, 5, 0, 1, 1);

    QPushButton * invertXPushButton = new QPushButton("invert", planeGroupBox);
    cuttingPlaneGridLayout->addWidget(invertXPushButton, 1, 1, 1, 1);

    QPushButton * invertYPushButton = new QPushButton("invert", planeGroupBox);
    cuttingPlaneGridLayout->addWidget(invertYPushButton, 3, 1, 1, 1);

    QPushButton * invertZPushButton = new QPushButton("invert", planeGroupBox);
    cuttingPlaneGridLayout->addWidget(invertZPushButton, 5, 1, 1, 1);

    QLabel * labelCutX = new QLabel("x cut position", planeGroupBox);
    cuttingPlaneGridLayout->addWidget(labelCutX, 0, 0, 1, 1);

    QLabel * labelCutY = new QLabel("y cut position", planeGroupBox);
    cuttingPlaneGridLayout->addWidget(labelCutY, 2, 0, 1, 1);

    QLabel * labelCutZ = new QLabel("z cut position", planeGroupBox);
    cuttingPlaneGridLayout->addWidget(labelCutZ, 4, 0, 1, 1);

    QCheckBox * displayXCut = new QCheckBox("display", planeGroupBox);
    cuttingPlaneGridLayout->addWidget(displayXCut, 0, 1, 1, 1);

    QCheckBox * displayYCut = new QCheckBox("display", planeGroupBox);
    cuttingPlaneGridLayout->addWidget(displayYCut, 2, 1, 1, 1);

    QCheckBox * displayZCut = new QCheckBox("display", planeGroupBox);
    cuttingPlaneGridLayout->addWidget(displayZCut, 4, 1, 1, 1);

    contentLayout->addStretch(0);


    // === Labels Diplay ===


    m_displaySubdomainsBox = new QGroupBox("Label display", this);
    contentLayout->addWidget(m_displaySubdomainsBox);

    QVBoxLayout * segIVLayout = new QVBoxLayout(m_displaySubdomainsBox);

    QScrollArea * displayIScrollArea = new QScrollArea(m_displaySubdomainsBox);
    segIVLayout->addWidget(displayIScrollArea);
    displayIScrollArea->setWidgetResizable(true);
    displayIScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QFrame * segIFrame = new QFrame();
    displayIScrollArea->setWidget(segIFrame);

    m_segIGridLayout = new QGridLayout(segIFrame);

    m_signalIMapper = new QSignalMapper(m_displaySubdomainsBox);

    QGridLayout * segIGridKayout = new QGridLayout();
    segIVLayout->addLayout(segIGridKayout);

    QPushButton *selectIPushButton = new QPushButton("Select All", m_displaySubdomainsBox);
    segIGridKayout->addWidget(selectIPushButton, 0, 0, 1, 1);

    QPushButton *discardIPushButton = new QPushButton("Discard All", m_displaySubdomainsBox);
    segIGridKayout->addWidget(discardIPushButton, 0, 1, 1, 1);

    contentLayout->addStretch(0);


    // === Draw checkboxes ===


    QGroupBox * drawGroupBox = new QGroupBox("Draw options", this);
    contentLayout->addWidget(drawGroupBox);

    QVBoxLayout * drawGroupLayout = new QVBoxLayout(drawGroupBox);

    QCheckBox * flatShadingCheckBox = new QCheckBox("Flat shading");
    drawGroupLayout->addWidget(flatShadingCheckBox);


    QGroupBox * meshDrawModeGroup = new QGroupBox("Draw Mesh");
    meshDrawModeGroup->setCheckable(true);
    meshDrawModeGroup->setChecked(true);
    drawGroupLayout->addWidget(meshDrawModeGroup);

    QVBoxLayout * meshDrawModeGroupLayout = new QVBoxLayout(meshDrawModeGroup);
    QRadioButton * triangleModeRadio = new QRadioButton("Triangles");
    QRadioButton * tetrahedralModeRadio = new QRadioButton("Tetrahedral");
    triangleModeRadio->setChecked(true);
    meshDrawModeGroupLayout->addWidget(triangleModeRadio);
    meshDrawModeGroupLayout->addWidget(tetrahedralModeRadio);


    QGroupBox * wireDrawModeGroup = new QGroupBox("Draw Wireframe");
    wireDrawModeGroup->setCheckable(true);
    wireDrawModeGroup->setChecked(false);
    drawGroupLayout->addWidget(wireDrawModeGroup);

    QVBoxLayout * wireDrawModeGroupLayout = new QVBoxLayout(wireDrawModeGroup);
    QRadioButton * wireTriangleModeRadio = new QRadioButton("Triangles");
    QRadioButton * wireTetrahedralModeRadio = new QRadioButton("Tetrahedral");
    wireTriangleModeRadio->setChecked(true);
    wireDrawModeGroupLayout->addWidget(wireTriangleModeRadio);
    wireDrawModeGroupLayout->addWidget(wireTetrahedralModeRadio);


    QCheckBox * setDrawVerticesBox = new QCheckBox("Draw Vertices");
    setDrawVerticesBox->setChecked(false);
    drawGroupLayout->addWidget(setDrawVerticesBox);


    QGroupBox * polylineDrawModeGroup = new QGroupBox("Draw polylines");
    polylineDrawModeGroup->setCheckable(true);
    polylineDrawModeGroup->setChecked(false);
    drawGroupLayout->addWidget(polylineDrawModeGroup);

    QVBoxLayout * polylineDrawModeGroupLayout = new QVBoxLayout(polylineDrawModeGroup);
    QRadioButton * linearModeRadio = new QRadioButton("Linear");
    QRadioButton * CRModeRadio = new QRadioButton("Cattmull-Rom");
    linearModeRadio->setChecked(true);
    polylineDrawModeGroupLayout->addWidget(linearModeRadio);
    polylineDrawModeGroupLayout->addWidget(CRModeRadio);


    // === Connections ===


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

    connect(discardIPushButton,    SIGNAL(clicked()),         this,     SLOT(discardAll()));
    connect(selectIPushButton,     SIGNAL(clicked()),         this,     SLOT(selectAll()));
    connect(m_viewer, SIGNAL(setMeshSubdomains()), this, SLOT(setMeshSubDomains()));

    connect(flatShadingCheckBox,       SIGNAL(toggled(bool)),     m_viewer, SLOT(setFlatShading(bool)));
    connect(wireDrawModeGroup,         SIGNAL(clicked(bool)),     this,     SLOT(setDrawWire(bool)));
    connect(wireTriangleModeRadio,     SIGNAL(clicked(bool)),     this,     SLOT(setWireModeTriangles(bool)));
    connect(wireTetrahedralModeRadio,  SIGNAL(clicked(bool)),     this,     SLOT(setWireModeTetrahedral(bool)));
    connect(meshDrawModeGroup,         SIGNAL(clicked(bool)),     this,     SLOT(setDrawMesh(bool)));
    connect(triangleModeRadio,         SIGNAL(clicked(bool)),     this,     SLOT(setMeshModeTriangles(bool)));
    connect(tetrahedralModeRadio,      SIGNAL(clicked(bool)),     this,     SLOT(setMeshModeTetrahedral(bool)));
    connect(setDrawVerticesBox,        SIGNAL(stateChanged(int)), this,     SLOT(setDrawVertices(int)));
    connect(polylineDrawModeGroup,     SIGNAL(clicked(bool)),     this,     SLOT(setDrawPolylines(bool)));
    connect(linearModeRadio,           SIGNAL(clicked(bool)),     this,     SLOT(setPolylineModeLinear(bool)));
    connect(CRModeRadio,               SIGNAL(clicked(bool)),     this,     SLOT(setPolylineModeSmooth(bool)));
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

void MorphoTetraDocWidget::setDrawMesh(bool state) {
    m_viewer->setDrawMesh(state);
}

void MorphoTetraDocWidget::setMeshModeTriangles(bool state) {
    if (state) {
        m_viewer->setMeshDrawMode(0);
    }
}

void MorphoTetraDocWidget::setMeshModeTetrahedral(bool state) {
    if (state) {
        m_viewer->setMeshDrawMode(1);
    }
}

void MorphoTetraDocWidget::setDrawWire(bool state) {
    m_viewer->setDrawWire(state);
}

void MorphoTetraDocWidget::setWireModeTriangles(bool state) {
    if (state) {
        m_viewer->setWireDrawMode(0);
    }
}

void MorphoTetraDocWidget::setWireModeTetrahedral(bool state) {
    if (state) {
        m_viewer->setWireDrawMode(1);
    }
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
