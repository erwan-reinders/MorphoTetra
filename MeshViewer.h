/*Based on this original code : https://github.com/Isnao/TER_GenMaillage/blob/master/viewer/src/MeshViewer.h*/

/****************************************************************************

 Copyright (C) 2002-2014 Gilles Debunne. All rights reserved.

 This file is part of the QGLViewer library version 2.7.2.

 http://www.libqglviewer.com - contact@libqglviewer.com

 This file may be used under the terms of the GNU General Public License
 versions 2.0 or 3.0 as published by the Free Software Foundation and
 appearing in the LICENSE file included in the packaging of this file.
 In addition, as a special exception, Gilles Debunne gives you certain
 additional rights, described in the file GPL_EXCEPTION in this package.

 libQGLViewer uses dual licensing. Commercial/proprietary software must
 purchase a libQGLViewer Commercial License.

 This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*****************************************************************************/

#ifndef MESH_VIEWER_H
#define MESH_VIEWER_H

#include "cgal_headers.h"
#include <QGLViewer/qglviewer.h>
#include <QString>
#include <set>
#include <map>
#include <QColor>
#include <stdio.h>

#define DEBUG false

class Viewer : public QGLViewer {
protected:
    virtual void draw();
    virtual void init();
    virtual QString helpString() const;

    void glFacet( const Facet & facet );

    void drawSD();
    void drawBoundaries();
    void drawVertices();
    void drawEdges();
    void drawPolyline();
    void drawSDP();

    C3t3 m_c3t3;

    std::vector<std::vector<C3t3::Edge>> polyLines;

    std::vector<std::vector<std::vector<C3t3::Edge>>> groupPolyLines;

    int m_indices[4][3];

    std::set<Subdomain_index> m_subdomain_indices;
    std::set<Surface_index> m_surface_indices;
    int m_nb_curve_segments;

    std::map<Subdomain_index, QColor> m_subdomain_colors;

    std::vector<int> nb_curve_segments_list;

    qglviewer::Vec m_center;

    bool E, V, F, P, SD, SDP;
    int indexP, indexSP, indexSD, indexSDP;

public slots:
    void onlyEdges();
    void onlyVertices();
    void onlyFaces();
    void activePolyline(bool a, int i, int j);
    void updateIndexPoly(int i, int j);
    void updateC3t3(C3t3 &c, std::vector<std::vector<C3t3::Edge>> p, std::set<Subdomain_index> sdi, std::set<Surface_index> sfi, std::map<Subdomain_index, QColor> sdc,std::vector<std::vector<std::vector<C3t3::Edge>>> g);
    int selectSubdomain(int i);
    void activeSubdomain(bool a, int i);
    int selectSubdomainP(int i);
    void activeSubdomainP(bool a, int i);
    void activeProjPoly(bool a, int i);
    void projection(C3t3 &c1, std::vector<C3t3::Edge> poly);

};
#endif 
