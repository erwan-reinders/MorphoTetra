/*Based on this original code : https://github.com/Isnao/TER_GenMaillage/blob/master/viewer/src/DisplayDockWidget.h*/

#ifndef DISPLAYDOCKWIDGET_H
#define DISPLAYDOCKWIDGET_H

#include "MeshViewer.h"
#include <QDockWidget>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <iostream>

class DisplayDockWidget : public QDockWidget
{
       Q_OBJECT
public:
    DisplayDockWidget(Viewer * viewer1, Viewer * viewer2, QWidget * parent );
    Viewer * m_viewer1;
    Viewer * m_viewer2;

    QPushButton *Edges;
    QPushButton *Faces;
    QPushButton *Vertices;
    QSpinBox *Curves;
    QCheckBox *CurvesActive;
    QSpinBox *EdgesPolyligne;
    bool isCurveActive = false;
    QPushButton *addMaillage;
    QSpinBox *maillageNumber1;
    QSpinBox *maillageNumber2;
    QCheckBox *activeSubdomain;
    QSpinBox *subdomain;
    QCheckBox *projPoly;
    QSpinBox *projPolySB;

    std::vector<std::vector<std::vector<std::vector<C3t3::Edge>>>> groupsPolyLines;

    QCheckBox *activeSubdomainP;
    QSpinBox *subdomainP;

protected:

    std::vector<C3t3> c3t3_list;

    std::vector<std::vector<std::vector<C3t3::Edge>>> polyLines_list;
    std::vector<std::set<Subdomain_index>> subdomain_indices_list;
    std::vector<std::set<Surface_index>> surface_indices_list;
    std::vector<std::map<Subdomain_index, QColor>> subdomain_colors_list;

    void openMesh(const QString &filename);
    void updateMaillage(int i, Viewer * v);
};

#endif // DISPLAYDOCKWIDGET_H
