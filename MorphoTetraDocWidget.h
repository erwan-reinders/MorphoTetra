#ifndef MORPHOTETRADOCWIDGET_H
#define MORPHOTETRADOCWIDGET_H

#include <QGroupBox>
#include <QGridLayout>
#include <QSignalMapper>
#include <QDockWidget>
#include <QCheckBox>
#include <QVector>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QBoxLayout>
#include <QRadioButton>
#include <QSlider>

#include "MultiMeshViewer.h"

class MorphoTetraDocWidget: public QDockWidget {
    Q_OBJECT

private :
    MultiMeshViewer*            m_viewer;
    //Gestion des sous-domaines
    QGroupBox *                 m_displaySubdomainsBox;
    QGridLayout *               m_segIGridLayout;
    QSignalMapper *             m_signalIMapper; 

    QVector<QCheckBox *>        m_labelICheckButtons;
    std::vector<unsigned int>   m_indexIToLabel;
    QVector<QLabel *>           m_imageLabels;

    QSlider * xHSlider;
    QSlider * yHSlider;
    QSlider * zHSlider;

protected :
    void setSubdomainsElement(QVBoxLayout * contentLayout);

public:
    MorphoTetraDocWidget(MultiMeshViewer * _viewer, QWidget * parent );
    ~MorphoTetraDocWidget();

public slots :
    void setVisibility(int i);
    void selectAll();
    void discardAll();
    void setMeshSubDomains();
    void addMeshSubDomains(const std::map<Subdomain_index, QColor> & colors);

    void setDrawMesh(bool state);
    void setMeshModeTriangles(bool state);
    void setMeshModeTetrahedral(bool state);
    void setDrawVertices(int state);
    void setDrawPolylines(bool state);
    void setPolylineModeLinear(bool state);
    void setPolylineModeSmooth(bool state);

    void setMaxCutPlanes(int _xMax, int _yMax, int _zMax);
};

#endif // MORPHOTETRADOCWIDGET_H
