#include "Topdockwidget.h"

TopDockWidget::TopDockWidget(MultiMeshViewer* _viewer, QWidget* parent) : QDockWidget(parent)
{
    QWidget* contents = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(contents);

    QCheckBox * flatShadingCheckBox = new QCheckBox("Flat shading");
    layout->addWidget(flatShadingCheckBox);

    connect(flatShadingCheckBox, SIGNAL(toggled(bool)), _viewer, SLOT(setFlatShading(bool)));

    m_viewer = _viewer;

    this->setWidget(contents);
}
