/*Based on this original code : https://github.com/Isnao/TER_GenMaillage/blob/master/viewer/src/MeshViewer.cpp*/

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

#include "MeshViewer.h"
#include "QGLViewer/manipulatedFrame.h"
#include <map>
#include <iostream>
#include <cfloat>


using namespace std;
using namespace qglviewer;

using namespace CGAL::parameters;

void parcoursPolyBis1(std::vector< C3t3::Edge >& polyLine, std::vector<bool>& used, std::vector< C3t3::Edge >& V, int lineNum, int point, C3t3& c3t3, int& curveNum) {
    used[lineNum] = true;
    polyLine.push_back(V[lineNum]);
    c3t3.remove_from_complex(V[lineNum]);
    c3t3.add_to_complex(V[lineNum], curveNum);
    C3t3::Vertex_handle vertex;
    if (point == 2) {
        if (c3t3.in_dimension(V[lineNum].first->vertex(V[lineNum].third)) == 0) {
            curveNum++;
            return;
        }
        vertex = V[lineNum].first->vertex(V[lineNum].third);
    }
    else {
        if (point == 3) {
            if (c3t3.in_dimension(V[lineNum].first->vertex(V[lineNum].second)) == 0) {
                curveNum++;
                return;
            }
            vertex = V[lineNum].first->vertex(V[lineNum].second);
        }
    }
    for (int i = 0; i < V.size(); i++) {
        if ((V[i].first->vertex(V[i].second)) == vertex && used[i] == false) {
            parcoursPolyBis1(polyLine, used, V, i, 2, c3t3, curveNum);
            return;
        }
        else {
            if ((V[i].first->vertex(V[i].third)) == vertex && used[i] == false) {
                parcoursPolyBis1(polyLine, used, V, i, 3, c3t3, curveNum);
                return;
            }
        }
    }
    curveNum++;
}

// edgeInVector :
bool edgeInVector1(C3t3::Edge &edge, std::vector<C3t3::Edge> &tab)
{
    for (unsigned int i = 0; i < tab.size(); i++)
    {
        if ((edge.first->vertex(edge.second) == tab[i].first->vertex(tab[i].second) && edge.first->vertex(edge.third) == tab[i].first->vertex(tab[i].third)) || (edge.first->vertex(edge.second) == tab[i].first->vertex(tab[i].third) && edge.first->vertex(edge.third) == tab[i].first->vertex(tab[i].second)))
            return true;
    }
    return false;
}
// Fin edgeInVector

// c3t3Param :
void c3t3Param1(C3t3 &c3t3, std::vector<C3t3::Edge> &CaracEdge){

    Tr &t = c3t3.triangulation();
    int c3t3CornerIndex = 0;
    for (Tr::All_vertices_iterator it = t.all_vertices_begin(); it != t.all_vertices_end(); ++it)
    {
        int nb = 0;
        std::vector<Tr::Cell_handle> cells;
        t.incident_cells(it, std::back_inserter(cells));
        for (unsigned int i = 0; i < cells.size(); i++)
        {
            bool flag = true;
            for (unsigned int j = 0; j < i; j++)
            {
                if ((int)(cells[i]->subdomain_index()) == (int)(cells[j]->subdomain_index()))
                {
                    flag = false;
                    break;
                }
            }
            if (flag)
            {
                nb++;
            }
        }
        if (nb == 1)
        {
            c3t3.set_dimension(it, 3);
        }
        if (nb == 2)
        {
            c3t3.set_dimension(it, 2);
        }
        if (nb > 2)
        {
            c3t3.set_dimension(it, 1);
        }
    }
    for (Tr::All_vertices_iterator it = t.all_vertices_begin(); it != t.all_vertices_end(); ++it)
    {
        if (c3t3.in_dimension(it) == 1 || c3t3.in_dimension(it) == 0)
        {
            int nb_edges = 0;
            std::vector<C3t3::Edge> edges;
            t.incident_edges(it, std::back_inserter(edges));
            for (unsigned int i = 0; i < edges.size(); i++)
            {
                if ((c3t3.in_dimension(edges[i].first->vertex(edges[i].second)) == 1 || c3t3.in_dimension(edges[i].first->vertex(edges[i].second)) == 0) && (c3t3.in_dimension(edges[i].first->vertex(edges[i].third)) == 0 || c3t3.in_dimension(edges[i].first->vertex(edges[i].third)) == 1))
                {
                    if (edgeInVector1(edges[i], CaracEdge))
                    {
                        nb_edges++;
                    }
                    else
                    {
                        Tr::Cell_circulator c = t.incident_cells(edges[i]);
                        Tr::Cell_circulator done = c;
                        std::vector<int> domains;
                        do
                        {
                            C3t3::Cell_handle cell = c;
                            int n = (int)(cell->subdomain_index());
                            if (std::find(domains.begin(), domains.end(), n) == domains.end())
                            {
                                domains.push_back(n);
                            }
                            c++;
                        } while (c != done);
                        if (domains.size() > 2)
                        {
                            CaracEdge.push_back(edges[i]);
                            c3t3.add_to_complex(edges[i], 1);
                            nb_edges++;
                        }
                    }
                }
            }
            if (nb_edges > 2)
            {
                c3t3.set_dimension(it, 0);
                c3t3.add_to_complex(it, c3t3CornerIndex++);
            }
        }
    }

    if(DEBUG) std::cout << "carac size of c3t3 : " << CaracEdge.size() << std::endl;
}
// Fin c3t3Param

// parcoursPoly :
void parcoursPoly1(std::vector< C3t3::Edge >& polyLine, std::vector<bool>& used, std::vector< C3t3::Edge >& V, int lineNum, C3t3& c3t3, int curveNum) {
    used[lineNum] = true;
    polyLine.push_back(V[lineNum]);
    c3t3.remove_from_complex(V[lineNum]);
    c3t3.add_to_complex(V[lineNum], curveNum);
    for (unsigned int i = 0; i < V.size(); i++) {
        if (used[i] == false) {
            if (c3t3.in_dimension(V[lineNum].first->vertex(V[lineNum].second)) == 1) {
                if ((V[i].first->vertex(V[i].second)) == V[lineNum].first->vertex(V[lineNum].second) || (V[i].first->vertex(V[i].third)) == V[lineNum].first->vertex(V[lineNum].second)) {
                    parcoursPoly1(polyLine, used, V, i, c3t3, curveNum);
                }
            }
            if (c3t3.in_dimension(V[lineNum].first->vertex(V[lineNum].third)) == 1) {
                if ((V[i].first->vertex(V[i].second)) == V[lineNum].first->vertex(V[lineNum].third) || (V[i].first->vertex(V[i].third)) == V[lineNum].first->vertex(V[lineNum].third)) {
                    parcoursPoly1(polyLine, used, V, i, c3t3, curveNum);
                }
            }
        }
    }
}
// Fin parcoursPoly

// groupPolylines
void getGroupPolyline1(C3t3& c3t3,Tr& t,std::vector<std::vector<C3t3::Edge>>& polyLines,std::vector<std::vector<std::vector<C3t3::Edge>>>& res){
  std::vector<bool> used;
  for(unsigned int i=0;i<polyLines.size();i++){
    used.push_back(false);
  }

  for(unsigned int i=0;i<polyLines.size();i++){
    if(!used[i]){
      std::vector<std::vector<C3t3::Edge>> temp;
      std::vector<int> domains;
      Tr::Cell_circulator c=t.incident_cells(polyLines[i][0]);
      Tr::Cell_circulator done=c;
      do{
        C3t3::Cell_handle cell=c;
        int n=(int)(cell->subdomain_index());
        if(std::find(domains.begin(), domains.end(), n) == domains.end()){
          domains.push_back(n);
        }
        c++;
      }while(c != done);

      for(unsigned int j=i;j<polyLines.size();j++){
        if(!used[j]){
          bool isSame=true;
          for(unsigned int k=0;k<domains.size();k++){
            c=t.incident_cells(polyLines[j][0]);
            bool b=true;
            done=c;
            do{
              C3t3::Cell_handle cell=c;
              if((int)(cell->subdomain_index())==domains[k]){
                b=false;
                break;
              }
              c++;
            }while(c != done);
            if(b){
              isSame=false;
              break;
            }
          }
          if(isSame){
            temp.push_back(polyLines[i]);
            used[j]=true;
          }
        }
      }

      res.push_back(temp);
    }
  }
}
// Fin groupPolyLines

// getSamePolylines
void getSamePolylines1(C3t3 &c3t31, Tr &t, C3t3 &c3t32, Tr &t2, C3t3::Edge e, std::vector<std::vector<C3t3::Edge>> &polyLinesC3t32, std::vector<std::vector<C3t3::Edge>> &polyLines)
{ //c3t3,tr1,c3t32,tr2,e : one edge of the polyline in c3t3,polylignes of c3t32, the list of found polylines in c3t32 with same domains as v
    std::vector<int> domains;
    Tr::Cell_circulator c = t.incident_cells(e);
    Tr::Cell_circulator done = c;
    do
    {
        C3t3::Cell_handle cell = c;
        int n = (int)(cell->subdomain_index());
        if (std::find(domains.begin(), domains.end(), n) == domains.end())
        {
            domains.push_back(n);
        }
        c++;
    } while (c != done);

    for (unsigned int i = 0; i < polyLinesC3t32.size(); i++)
    {
        bool isSame = true;
        for (unsigned int k = 0; k < domains.size(); k++)
        {
            c = t2.incident_cells(polyLinesC3t32[i][0]);
            bool b = true;
            done = c;
            do
            {
                C3t3::Cell_handle cell = c;
                if ((int)(cell->subdomain_index()) == domains[k])
                {
                    b = false;
                    break;
                }
                c++;
            } while (c != done);
            if (b)
            {
                isSame = false;
                break;
            }
        }
        if (isSame)
        {
            polyLines.push_back(polyLinesC3t32[i]);
        }
    }
}
// Fin getSamePolylines

void printPolyligne(std::vector<C3t3::Edge> &polyLine)
{
    std::cout << "Polyline :" << std::endl;
    for (unsigned int j = 0; j < polyLine.size(); j++)
    {
        C3t3::Vertex_handle p1 = polyLine[j].first->vertex(polyLine[j].second);
        C3t3::Vertex_handle p2 = polyLine[j].first->vertex(polyLine[j].third);
        std::cout << "\tvertex 1 : " << p1->point()
                  << " , \tVertex 2 : " << p2->point() << std::endl;
    }
}

bool pointInGoodSideOfPlane(const K::Vector_3 & p, const K::Vector_3 & planeCenter, const K::Vector_3 planeNormal) {
    return CGAL::scalar_product(planeCenter - p, planeNormal) > 0;
}

void Viewer::glFacet( const Facet & facet ){
    const Point_3 & pa = facet.first->vertex(m_indices[facet.second][0])->point();
    const Point_3 & pb = facet.first->vertex(m_indices[facet.second][1])->point();
    const Point_3 & pc = facet.first->vertex(m_indices[facet.second][2])->point();
    K::Vector_3 pac (pc.x() - pa.x(), pc.y() - pa.y(), pc.z() - pa.z());
    K::Vector_3 pab (pb.x() - pa.x(), pb.y() - pa.y(), pb.z() - pa.z());

    bool sholdDrawFacet = true;
    if (cutPlane) {
        sholdDrawFacet = pointInGoodSideOfPlane(K::Vector_3(pa.x(), pa.y(), pa.z()), planeCenter, planeNormal)
                || pointInGoodSideOfPlane(K::Vector_3(pb.x(), pb.y(), pb.z()), planeCenter, planeNormal)
                || pointInGoodSideOfPlane(K::Vector_3(pc.x(), pc.y(), pc.z()), planeCenter, planeNormal);
    }

    if (!sholdDrawFacet) {
        return;
    }

    K::Vector_3 n = CGAL::cross_product( pac, pab );
    n = n / CGAL::sqrt(n.squared_length());

    glNormal3d(n.x(),n.y(),n.z());

    glVertex3d(pa.x(),pa.y(),pa.z());
    glVertex3d(pb.x(),pb.y(),pb.z());
    glVertex3d(pc.x(),pc.y(),pc.z());

}

void Viewer::drawBoundaries(){

    glEnable(GL_LIGHTING);
    glBegin(GL_TRIANGLES);

    for (auto it = m_surface_indices.begin(); it != m_surface_indices.end(); ++it){
        Surface_index surface_index = *it;
        QColor color = m_subdomain_colors[it->second];
        glColor4f(color.redF(), color.greenF(), color.blueF(), 1.);

        for (C3t3::Facets_in_complex_iterator fit = m_c3t3.facets_in_complex_begin (surface_index) ; fit != m_c3t3.facets_in_complex_end (surface_index); ++fit ) {

            Facet facet = *fit;

            if( m_c3t3.subdomain_index(facet.first) != it->second )
                facet = m_c3t3.triangulation().mirror_facet(facet);

            glFacet(facet);

        }
    }
    glEnd();
}

void Viewer::drawSD(){

    glEnable(GL_LIGHTING);


    glBegin(GL_TRIANGLES);

    QColor color = m_subdomain_colors[indexSD];
    glColor4f(color.redF(), color.greenF(), color.blueF(), 1.);
        for (C3t3::Facets_in_complex_iterator fit = m_c3t3.facets_in_complex_begin() ; fit != m_c3t3.facets_in_complex_end (); ++fit ) {

            Facet facet = *fit;
            int k = m_c3t3.subdomain_index(facet.first);
            if (k == indexSD or m_c3t3.subdomain_index(m_c3t3.triangulation().mirror_facet(facet).first) == indexSD) {
                if(k  == 0 )
                    facet = m_c3t3.triangulation().mirror_facet(facet);

                glFacet(facet);
            }

        }
   
    glEnd();

}

void Viewer::drawSDP(){

    glDisable(GL_LIGHTING);
    glLineWidth (2.0f);

   glColor4f( 1., 0.8, 0., 1.);

    glBegin(GL_LINES);

	Tr& t = m_c3t3.triangulation();
    for (C3t3::Edges_in_complex_iterator eit = m_c3t3.edges_in_complex_begin () ; eit != m_c3t3.edges_in_complex_end (); ++eit ){
    	bool ok=false;

        Tr::Cell_circulator c = t.incident_cells(*eit);
        Tr::Cell_circulator done = c;
        do {
           	if ((int)(c->subdomain_index()) == indexSDP){
           	    ok=true;
                break;
            }
            c++;
        }while (c != done);

        if (ok){
                
            Point_3 from = eit->first->vertex( eit->second )->point();
        	Point_3 to = eit->first->vertex( eit->third )->point();

        	glVertex3f( from.x(), from.y(), from.z() );
        	glVertex3f( to.x(), to.y(), to.z() );
        }

    }

    glEnd();
}

void Viewer::drawEdges(){
    //if(DEBUG) std::cout << "Viewer draw edges !" << std::endl;
    glDisable(GL_LIGHTING);
    glLineWidth (2.0f);

    glColor4f( 1.,0.8,0., 1.);

    //TODO cylindres, ...
    glBegin(GL_LINES);

    for (C3t3::Edges_in_complex_iterator eit = m_c3t3.edges_in_complex_begin () ; eit != m_c3t3.edges_in_complex_end (); ++eit ) {
        //std::cout << "Im in for !" << std::endl;

        //std::cout << eit->first->vertex( eit->second )->point() << std::endl;
        //std::cout << eit->second << std::endl;
        //std::cout << eit->third << std::endl;


        //std::cout << eit->third << std::endl;
        //QColor color;
        //color.setHsvF(0.05 + 0.9*double(m_c3t3.curve_segment_index(*eit))/double(m_nb_curve_segments), 0.6 ,1. );

        //glColor4f(color.redF(), color.greenF(), color.blueF(), 1.);
        Point_3 from = eit->first->vertex( eit->second )->point();
        Point_3 to = eit->first->vertex( eit->third )->point();

        glVertex3f( from.x(), from.y(), from.z() );
        glVertex3f( to.x(), to.y(), to.z() );
    }

    glEnd();

}

void Viewer::drawPolyline(){

    glDisable(GL_LIGHTING);
    glLineWidth (2.0f);

    glBegin(GL_LINES);
    glColor4f( 1., 0.8, 0., 0.5);

    for (C3t3::Edges_in_complex_iterator eit = m_c3t3.edges_in_complex_begin () ; eit != m_c3t3.edges_in_complex_end (); ++eit ){

        if ((int)(m_c3t3.curve_segment_index(*eit)) == indexP){

            Point_3 from = eit->first->vertex( eit->second )->point();
        	Point_3 to = eit->first->vertex( eit->third )->point();

        	glVertex3f( from.x(), from.y(), from.z() );
        	glVertex3f( to.x(), to.y(), to.z() );
        }
    }


    /*
	if(indexP >= (int)groupPolyLines.size()){
		std::cout<<"Indice polylignes hors range !!!"<<std::endl;
	}

    for(unsigned int i=0;i<groupPolyLines[indexP].size();i++){
    	printPolyligne(groupPolyLines[indexP][i]);
	}

	for(unsigned int i=0;i<groupPolyLines[indexP].size();i++){
		for(unsigned int j=0;j<groupPolyLines[indexP][i].size();j++){
			std::cout<<groupPolyLines[indexP][i][j].first->vertex(groupPolyLines[indexP][i][j].second)->point()<<std::endl;
			std::cout<<"oui"<<std::endl;

			glColor4f( 1., 0.8, 0., 1.);

        	Point_3 from = groupPolyLines[indexP][i][j].first->vertex(groupPolyLines[indexP][i][j].second)->point();
        	std::cout<<"oui2"<<std::endl;
        	Point_3 to = groupPolyLines[indexP][i][j].first->vertex( groupPolyLines[indexP][i][j].third)->point();

        	std::cout<<"oui3"<<std::endl;

       		glVertex3f( from.x(), from.y(), from.z() );
       		std::cout<<"oui4"<<std::endl;
        	glVertex3f( to.x(), to.y(), to.z() );
        	std::cout<<"oui5"<<std::endl;
		}
    }
    */

    glEnd();

}

void Viewer::drawVertices(){

    glDisable(GL_LIGHTING);

    glPointSize( 5.0f);

    glColor4f( 1.,0.8,0., 1.);

    glBegin(GL_POINTS);

    for( Tr::Finite_vertices_iterator vit = m_c3t3.triangulation().finite_vertices_begin(); vit != m_c3t3.triangulation().finite_vertices_end() ; ++vit  ){
        if(m_c3t3.in_dimension(vit) < 3){
            glColor4f( 0.f,0.f,1.f, 1.);


            if(m_c3t3.in_dimension(vit) == 0){
                glColor4f( 1.f,1.f,0.f, 1.);

            } else if(m_c3t3.in_dimension(vit) == 1){
                glColor4f( 1.f,0.f,0.f, 1.);
            }
           	else if (m_c3t3.in_dimension(vit) == 2 ) {
                glColor4f( 0.f,1.f,0.f, 1.);
            }


            Point_3 position = vit->point();
            glVertex3f( position.x(), position.y(), position.z() );
        }
    }
    glEnd();
}

// Draws a spiral
void Viewer::draw() {
    //if(DEBUG) {
    //    std::cout << "VIEWER : draw" << std::endl;
    //    std::cout << "V : " << V << std::endl;
    //    std::cout << "E : " << E << std::endl;
    //    std::cout << "F : " << F << std::endl;
    //    std::cout << "P : " << P << std::endl;
    //    std::cout << "SD : " << SD << std::endl;
    //    std::cout << "SDP : " << SDP << std::endl;
    //}

    // TODO regarder ces méthodes (extraire les données avant le dessin)
    if (V)
        drawVertices();
    if (E)
        drawEdges();
    if (F)
        drawBoundaries();
    if (P)
        drawPolyline();
    if (SD)
        drawSD();
    if (SDP)
        drawSDP();


    /*
    glPushMatrix();
    glMultMatrixd(manipulatedFrame()->matrix());
    // Since the Clipping Plane equation is multiplied by the current modelView,
    // we can define a constant equation (plane normal along Z and passing by the
    // origin) since we are here in the manipulatedFrame coordinates system (we
    // glMultMatrixd() with the manipulatedFrame matrix()).

    static const GLdouble equation[] = {m_center[0], m_center[1], m_center[2], 0.0};
    glClipPlane(GL_CLIP_PLANE0, equation);

    // Draw a plane representation: Its normal...
    glColor3f(0.8f, 0.8f, 0.8f);
    drawArrow(0.4f, 0.015f);
    // ...and a quad (with a slightly shifted z so that it is not clipped).
    glBegin(GL_QUADS);
    glVertex3f(-1.0, -1.0, 0.001f);
    glVertex3f(-1.0, 1.0, 0.001f);
    glVertex3f(1.0, 1.0, 0.001f);
    glVertex3f(1.0, -1.0, 0.001f);
    glEnd();
    glPopMatrix();*/
}

void Viewer::init() {
    if(DEBUG) std::cout << "VIEWER : init" << std::endl;
    E = true;
    V = false;
    F = false;
    P = false;
    SD = false;
    SDP= false;
    cutPlane = true; // TODO false
    indexP = 0;
    indexSP = 0;
    indexSD = 0;
    indexSDP = 0;
    planeCenter = K::Vector_3(0, 0, 0);
    planeNormal = K::Vector_3(0, 1, 0);

    m_indices[0][0] = 3; m_indices[0][2] = 1; m_indices[0][1] = 2;
    m_indices[1][0] = 3; m_indices[1][2] = 2; m_indices[1][1] = 0;
    m_indices[2][0] = 3; m_indices[2][2] = 0; m_indices[2][1] = 1;
    m_indices[3][0] = 2; m_indices[3][2] = 1; m_indices[3][1] = 0;

    if(DEBUG) std::cout << "VIEWER : restore state from file" << std::endl;

    restoreStateFromFile();

    // Opens help window
    if(DEBUG) std::cout << "VIEWER : help menu" << std::endl;
    //help();


    if(DEBUG) std::cout << "VIEWER : c3t3 load" << std::endl;
    std::ifstream c3t3_load("data/out.mesh");    
    if(DEBUG) std::cout << "VIEWER : DATA LOADED" << std::endl;
    c3t3_load >> m_c3t3;

    int nbV = 0;
    for( Tr::Finite_vertices_iterator vit = m_c3t3.triangulation().finite_vertices_begin(); vit != m_c3t3.triangulation().finite_vertices_end() ; ++vit  ){
        if(m_c3t3.in_dimension(vit) < 3){
            Point_3 position = vit->point();
            planeCenter += K::Vector_3(position.x(), position.y(), position.z());
            nbV++;
        }
    }
    planeCenter /= nbV;

    if(DEBUG) std::cout << "VIEWER : remplissage des dimensions c3t3 + remplissages des egdes caracteristiques" << std::endl;
      std::vector<C3t3::Edge> CaracEdge;
      c3t3Param1(m_c3t3,CaracEdge);

      //////////////////////////////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////////////////////////////

      //std::cout << std::endl;

      if(DEBUG) std::cout << "VIEWER : test comparaisons" << std::endl;
      typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr>::Vertices_in_complex_iterator Complex_Vertex_Iterator;

      // vertex ***************************

      if(DEBUG) std::cout << "VIEWER : C3T3 Number of cells : " << m_c3t3.number_of_cells() << std::endl;
      int i = 0;
      std::map<C3t3::Vertex_handle, int> V;
      for (Complex_Vertex_Iterator it = m_c3t3.vertices_in_complex_begin(); it != m_c3t3.vertices_in_complex_end(); ++it)
      {
        if (m_c3t3.in_dimension(it) == 0)
        {
          V[it] = i;
          //std::cout << "Vertex #" << i << " : " << it->point() << " Dimension : " << c3t3.in_dimension(it) << std::endl;
        }
        i++;
      }
      if(DEBUG) std::cout << " VIEWER : Taille V : " << V.size() << " (nombre de vertex de dimension 0 dans c3t3)" << std::endl;

      ///////////////////////////////////////////////
      // edges ****************************************

      i = 0;
      std::map<C3t3::Edge, int> V_e;
      for (C3t3::Edges_in_complex_iterator it = m_c3t3.edges_in_complex_begin(); it != m_c3t3.edges_in_complex_end(); ++it)
      {
        // add the current Point_3 to the map with its current index
        C3t3::Edge edge = *it;
        V_e[edge] = i;
        ++i;
      }

      if(DEBUG) std::cout << " VIEWER : Taille V_e : " << V_e.size() << "(nombre d'edge caractéristiques dans c3t3)" << std::endl;

      //////////////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////////////

      //polylignes

       if(DEBUG) std::cout << " VIEWER : deb poly" << std::endl;
        //c3t3
        int curveNum_c3t3=1;
      std::vector<bool> used;
      for (unsigned int i = 0; i < CaracEdge.size(); i++){
        used.push_back(false);
      }

      std::vector<std::vector<C3t3::Edge>> p;

      
    	for (int i = 0; i < CaracEdge.size(); i++) {
      		if ((int)(m_c3t3.in_dimension(CaracEdge[i].first->vertex(CaracEdge[i].second))) == 0 && used[i] == false) {
          		std::vector<C3t3::Edge> tempPolyLine;
          		parcoursPolyBis1(tempPolyLine, used, CaracEdge, i, 2, m_c3t3, curveNum_c3t3);
          		p.push_back(tempPolyLine);
      		}
      		else {
          		if ((int)(m_c3t3.in_dimension(CaracEdge[i].first->vertex(CaracEdge[i].third))) == 0 && used[i] == false) {
              		std::vector<C3t3::Edge> tempPolyLine;
              		parcoursPolyBis1(tempPolyLine, used, CaracEdge, i, 3, m_c3t3, curveNum_c3t3);
              		p.push_back(tempPolyLine);
         	 	}
      		}
  		}

      for (unsigned int i = 0; i < CaracEdge.size(); i++){
        if(used[i]==false){
          std::vector<C3t3::Edge> tempPolyLine;
          parcoursPoly1(tempPolyLine, used, CaracEdge, i, m_c3t3,curveNum_c3t3);
          polyLines.push_back(tempPolyLine);
          curveNum_c3t3++;
       }
      }

      if(DEBUG) std::cout << " VIEWER : fin poly" << std::endl;

      if(DEBUG) std::cout << " VIEWER : create tetrahedra's informations" << std::endl;


      for (C3t3::Cells_in_complex_iterator cit = m_c3t3.cells_in_complex_begin () ; cit != m_c3t3.cells_in_complex_end (); ++cit ) {
          m_subdomain_indices.insert(m_c3t3.subdomain_index(cit));
      }

      for (C3t3::Facets_in_complex_iterator fit = m_c3t3.facets_in_complex_begin () ; fit != m_c3t3.facets_in_complex_end (); ++fit ) {
          m_surface_indices.insert(m_c3t3.surface_index(*fit));
      }

      int nb = 0;
      for (auto it = m_subdomain_indices.begin(); it != m_subdomain_indices.end(); ++it){
          QColor color;
          color.setHsvF(0.05 + 0.9*double(nb++)/double(m_subdomain_indices.size()), 0.6 ,1. );
          m_subdomain_colors[*it] = color;
          //std::cout << *it<< std::endl;
      }


    if(DEBUG) std::cout << " VIEWER : clipping plane positionning" << std::endl;
    // The ManipulatedFrame will be used to position the clipping plane
    setManipulatedFrame(new ManipulatedFrame());

    CGAL::Bbox_3 bbox = m_c3t3.bbox();

    m_center = qglviewer::Vec ((bbox.xmax() - bbox.xmin())/2., (bbox.ymax() - bbox.ymin())/2., (bbox.zmax() - bbox.zmin())/2.);

    camera()->setSceneCenter(qglviewer::Vec((bbox.xmax() - bbox.xmin())/2., (bbox.ymax() - bbox.ymin())/2., (bbox.zmax() - bbox.zmin())/2.));
    camera()->setSceneRadius(std::max(std::max(bbox.xmax() - bbox.xmin(), bbox.ymax() - bbox.ymin()), bbox.zmax() - bbox.zmin())*2.);

    camera()->showEntireScene();

    // Enable plane clipping
    //glEnable(GL_CLIP_PLANE0);

    if(DEBUG) std::cout << "VIEWER : fin init" << std::endl;
}

QString Viewer::helpString() const {
    QString text("<h2>C l i p p i n g P l a n e</h2>");
    text += "The standard OpenGL <i>GL_CLIP_PLANE</i> feature is used to add an "
            "additionnal clipping ";
    text += "plane in the scene, which position and orientation are set by a "
            "<b>ManipulatedFrame</b>.<br><br>";
    text += "Hold the <b>Control</b> key pressed down while using the mouse to "
            "modify the plane orientation (left button) ";
    text += "and position (right button) and to interactively see the clipped "
            "result.<br><br>";
    text += "Since the plane equation is defined with respect to the current "
            "modelView matrix, a constant equation (normal";
    text += "along the Z axis) can be used since we transformed the coordinates "
            "system using the <b>matrix()</b> method.";

    return text;
}

void Viewer::onlyEdges() {
    E = true;
    F = false;
    V = false;
    std::cerr << "onlyEdges()" << std::endl;
    update();
}

void Viewer::onlyVertices() {
    V = true;
    F = false;
    E = false;
    std::cerr << "onlyVertices()" << std::endl;
    update();
}

void Viewer::onlyFaces() {
    F = true;
    V = false;
    E = false;
std::cerr << "onlyFaces()" << std::endl;
    update();
}

void Viewer::activePolyline(bool a, int i, int j) {
    P = a;
    indexP = i;
    indexSP = j;
    if (P) {
        F = false;
        V = false;
        E = false;
    } else {
        F = false;
        V = false;
        E = true;
    }
    std::cerr << "activePolyline(bool)" << std::endl;
    update();
}

void Viewer::updateIndexPoly(int i, int j) {
    indexP = i;
    indexSP = j;
    if (P)
        update();
}

void Viewer::updateC3t3(C3t3 &c, std::vector<std::vector<C3t3::Edge>> p, std::set<Subdomain_index> sdi, std::set<Surface_index> sfi, std::map<Subdomain_index, QColor> sdc,std::vector<std::vector<std::vector<C3t3::Edge>>> g) {
    if(DEBUG) {
        std::cout << "Viewer :  update C3T3" << std::endl;
        //std::cout << c << std::endl;
    }

    m_c3t3 = c;
    polyLines = p;
    m_subdomain_indices = sdi;
    m_surface_indices = sfi;
    m_subdomain_colors = sdc;
    CGAL::Bbox_3 bbox = m_c3t3.bbox();
    groupPolyLines = g;

    m_center = qglviewer::Vec ((bbox.xmax() - bbox.xmin())/2., (bbox.ymax() - bbox.ymin())/2., (bbox.zmax() - bbox.zmin())/2.);
    camera()->setSceneCenter(qglviewer::Vec((bbox.xmax() - bbox.xmin())/2., (bbox.ymax() - bbox.ymin())/2., (bbox.zmax() - bbox.zmin())/2.));
    camera()->setSceneRadius(std::max(std::max(bbox.xmax() - bbox.xmin(), bbox.ymax() - bbox.ymin()), bbox.zmax() - bbox.zmin())*2.);

    update();

}

int Viewer::selectSubdomain(int i) {

    indexSD = i;
    update();
    return m_subdomain_indices.size();

}

void Viewer::activeSubdomain(bool a, int i) {

    SD = a;
    indexSD = i;
    if (SD) {
        V = false;
        E = false;
        F = false;
        P = false;
        SDP=false;
    } else {
        V = false;
        E = false;
        P = false;
        F = true;
        SDP=false;
    }
    std::cerr << "activeSubdomain(bool)" << std::endl;
    update();
}

int Viewer::selectSubdomainP(int i) {

    indexSDP = i;
    update();
    return m_subdomain_indices.size();

}

void Viewer::activeSubdomainP(bool a, int i) {

    SDP = a;
    indexSDP = i;
    if (SDP) {
        V = false;
        E = false;
        F = false;
        P = false;
        SD = false;
        
    } else {
        V = false;
        E = true;
        P = false;
        F = false;
        SD = false;
    }
    std::cerr << "activeSubdomainP(bool)" << std::endl;
    update();

}


void Viewer::activeProjPoly(bool a, int i){}
void Viewer::projection(C3t3 &c1, std::vector<C3t3::Edge> poly){}
