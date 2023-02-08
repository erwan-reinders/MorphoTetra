#include "RemeshingDockWidget.h"

RemeshingDockWidget::RemeshingDockWidget(MultiMeshViewer* _viewer, QString filePath, QStatusBar *statusbar, QWidget* parent) : QDockWidget(parent)
{
    m_viewer = _viewer;
    m_statusbar = statusbar;
    m_filePath = filePath;

    QWidget* contents = new QWidget();
    this->setWidget(contents);

    QHBoxLayout* layout = new QHBoxLayout(contents);

    QGroupBox * cellCriteriaGroup = new QGroupBox("Cell criteria");
    layout->addWidget(cellCriteriaGroup);
    QFormLayout * cellFormLayout = new QFormLayout(cellCriteriaGroup);
    cellCriteriaGroup->setLayout(cellFormLayout);
    QLabel * cellRatioLabel = new QLabel("Cell ratio");
    m_cellRatioNumber       = new QDoubleSpinBox(cellCriteriaGroup);
    QLabel * cellSizeLabel  = new QLabel("Cell size");
    m_cellSizeNumber        = new QDoubleSpinBox(cellCriteriaGroup);
    m_cellRatioNumber->setValue(DETAILED_CELL_RATIO);
    m_cellSizeNumber->setValue(DETAILED_CELL_SIZE);
    cellFormLayout->setWidget(0, QFormLayout::LabelRole, cellRatioLabel);
    cellFormLayout->setWidget(0, QFormLayout::FieldRole, m_cellRatioNumber);
    cellFormLayout->setWidget(1, QFormLayout::LabelRole, cellSizeLabel);
    cellFormLayout->setWidget(1, QFormLayout::FieldRole, m_cellSizeNumber);

    QGroupBox * facetCriteriaGroup = new QGroupBox("Facet criteria");
    layout->addWidget(facetCriteriaGroup);
    QFormLayout * facetFormLayout = new QFormLayout(facetCriteriaGroup);
    facetCriteriaGroup->setLayout(facetFormLayout);
    QLabel * facetAngleLabel         = new QLabel("Facet angle");
    m_facetAngleNumber               = new QDoubleSpinBox(facetCriteriaGroup);
    QLabel * facetSizeLabel          = new QLabel("Facet size");
    m_facetSizeNumber                = new QDoubleSpinBox(facetCriteriaGroup);
    QLabel * facetApproximationLabel = new QLabel("Facet approximation");
    m_facetApproximationNumber       = new QDoubleSpinBox(facetCriteriaGroup);
    QLabel * facetTopologyLabel      = new QLabel("Facet topology");
    m_facetTopologyBox               = new QComboBox(facetCriteriaGroup);
    m_facetAngleNumber->setValue(DETAILED_FACET_ANGLE);
    m_facetAngleNumber->setMaximum(30);
    m_facetSizeNumber->setValue(DETAILED_FACET_SIZE);
    m_facetApproximationNumber->setValue(DETAILED_FACET_APPROXIMATION);
    m_facetTopologyBox->addItem("Facet on surface", CGAL::FACET_VERTICES_ON_SURFACE);
    m_facetTopologyBox->addItem("Facet on same surface", CGAL::FACET_VERTICES_ON_SAME_SURFACE_PATCH);
    m_facetTopologyBox->addItem("Facet on same surface with adjacency", CGAL::FACET_VERTICES_ON_SAME_SURFACE_PATCH_WITH_ADJACENCY_CHECK);
    //m_facetTopologyBox->setCurrentIndex(0);
    facetFormLayout->setWidget(0, QFormLayout::LabelRole, facetAngleLabel);
    facetFormLayout->setWidget(0, QFormLayout::FieldRole, m_facetAngleNumber);
    facetFormLayout->setWidget(1, QFormLayout::LabelRole, facetSizeLabel);
    facetFormLayout->setWidget(1, QFormLayout::FieldRole, m_facetSizeNumber);
    facetFormLayout->setWidget(2, QFormLayout::LabelRole, facetApproximationLabel);
    facetFormLayout->setWidget(2, QFormLayout::FieldRole, m_facetApproximationNumber);
    facetFormLayout->setWidget(3, QFormLayout::LabelRole, facetTopologyLabel);
    facetFormLayout->setWidget(3, QFormLayout::FieldRole, m_facetTopologyBox);

    QGroupBox * edgeCriteriaGroup = new QGroupBox("Edge criteria");
    layout->addWidget(edgeCriteriaGroup);
    QFormLayout * edgeFormLayout = new QFormLayout(edgeCriteriaGroup);
    edgeCriteriaGroup->setLayout(edgeFormLayout);
    QLabel * edgeSizeLabel  = new QLabel("Edge size");
    m_edgeSizeNumber        = new QDoubleSpinBox(edgeCriteriaGroup);
    m_edgeSizeNumber->setValue(DETAILED_EDGE_SIZE);
    edgeFormLayout->setWidget(0, QFormLayout::LabelRole, edgeSizeLabel);
    edgeFormLayout->setWidget(0, QFormLayout::FieldRole, m_edgeSizeNumber);

    QGroupBox * optimizationGroup = new QGroupBox("Optimizations");
    layout->addWidget(optimizationGroup);
    QFormLayout * optimizationFormLayout = new QFormLayout(optimizationGroup);
    optimizationGroup->setLayout(optimizationFormLayout);
    m_perturbOption = new QCheckBox("Perturb");
    m_exudeOption   = new QCheckBox("Exude");
    m_perturbOption->setChecked(false);
    m_exudeOption->setChecked(false);
    optimizationFormLayout->setWidget(0, QFormLayout::FieldRole, m_perturbOption);
    optimizationFormLayout->setWidget(1, QFormLayout::FieldRole, m_exudeOption);

    QPushButton * remeshPushButton = new QPushButton("Remesh");
    layout->addWidget(remeshPushButton);

    m_filePathLabel = new QLabel(filePath.split("/").last());
    layout->addWidget(m_filePathLabel);

    layout->addStretch(999);

    connect(remeshPushButton, SIGNAL(clicked()), this, SLOT(remesh()));
}

void RemeshingDockWidget::remesh() {
    m_viewer->loadMeshes(m_statusbar, QStringList(m_filePath),
                         m_edgeSizeNumber->value(),
                         m_facetAngleNumber->value(),
                         m_facetSizeNumber->value(),
                         m_facetApproximationNumber->value(),
                         m_facetTopologyBox->currentData().value<CGAL::Mesh_facet_topology>(),
                         m_cellRatioNumber->value(),
                         m_cellSizeNumber->value(),
                         m_perturbOption->isChecked(),
                         m_exudeOption->isChecked()
                         );
}
