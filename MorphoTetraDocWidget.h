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

    void setDrawMesh(int state);
    void setDrawVertices(int state);
    void setDrawPolylines(int state);
};

#endif // MORPHOTETRADOCWIDGET_H
