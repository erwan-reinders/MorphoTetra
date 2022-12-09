/*Based on this original code : https://github.com/Isnao/TER_GenMaillage/blob/master/viewer/src/DisplayDockWidget.cpp*/

#include "DisplayDockWidget.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QFileDialog>
#include <algorithm>
#include <stdexcept>

using namespace CGAL::parameters;

void parcoursPolyBis(std::vector< C3t3::Edge >& polyLine, std::vector<bool>& used, std::vector< C3t3::Edge >& V, int lineNum, int point, C3t3& c3t3, int& curveNum) {
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
            parcoursPolyBis(polyLine, used, V, i, 2, c3t3, curveNum);
            return;
        }
        else {
            if ((V[i].first->vertex(V[i].third)) == vertex && used[i] == false) {
                parcoursPolyBis(polyLine, used, V, i, 3, c3t3, curveNum);
                return;
            }
        }
    }
    curveNum++;
}


// edgeInVector :
bool edgeInVector(C3t3::Edge &edge, std::vector<C3t3::Edge> &tab)
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
void c3t3Param(C3t3 &c3t3, std::vector<C3t3::Edge> &CaracEdge)
{
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
                    if (edgeInVector(edges[i], CaracEdge))
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
void parcoursPoly(std::vector< C3t3::Edge >& polyLine, std::vector<bool>& used, std::vector< C3t3::Edge >& V, int lineNum, C3t3& c3t3, int curveNum) {
    used[lineNum] = true;
    polyLine.push_back(V[lineNum]);
    c3t3.remove_from_complex(V[lineNum]);
    c3t3.add_to_complex(V[lineNum], curveNum);
    for (unsigned int i = 0; i < V.size(); i++) {
        if (used[i] == false) {
            if (c3t3.in_dimension(V[lineNum].first->vertex(V[lineNum].second)) == 1) {
                if ((V[i].first->vertex(V[i].second)) == V[lineNum].first->vertex(V[lineNum].second) || (V[i].first->vertex(V[i].third)) == V[lineNum].first->vertex(V[lineNum].second)) {
                    parcoursPoly(polyLine, used, V, i, c3t3, curveNum);
                }
            }
            if (c3t3.in_dimension(V[lineNum].first->vertex(V[lineNum].third)) == 1) {
                if ((V[i].first->vertex(V[i].second)) == V[lineNum].first->vertex(V[lineNum].third) || (V[i].first->vertex(V[i].third)) == V[lineNum].first->vertex(V[lineNum].third)) {
                    parcoursPoly(polyLine, used, V, i, c3t3, curveNum);
                }
            }
        }
    }
}
// Fin parcoursPoly

// groupPolylines
void getGroupPolyline(C3t3& c3t3,Tr& t,std::vector<std::vector<C3t3::Edge>>& polyLines,std::vector<std::vector<std::vector<C3t3::Edge>>>& res){

  if(DEBUG) std::cout << "GET GROUP POLILYNE" << std::endl;

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

  if(DEBUG) std::cout << "END GET GROUP POLILYNE" << std::endl;

}
// Fin groupPolyLines

// getSamePolylines
void getSamePolylines(C3t3 &c3t31, Tr &t, C3t3 &c3t32, Tr &t2, C3t3::Edge e, std::vector<std::vector<C3t3::Edge>> &polyLinesC3t32, std::vector<std::vector<C3t3::Edge>> &polyLines)
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

DisplayDockWidget::DisplayDockWidget(Viewer * viewer1, Viewer * viewer2, QWidget * parent ):QDockWidget(parent)
{

    m_viewer1 = viewer1;
    m_viewer2 = viewer2;

    if(DEBUG) std::cout << "DISPLAYDOCWIDGET : Construct UI" << std::endl;


    QWidget * contents = new QWidget();

    QVBoxLayout * contentLayout = new QVBoxLayout(contents);

    QGroupBox * groupBox = new QGroupBox(parent);
    groupBox->setMaximumSize(QSize(8388607, 100));
    contentLayout->addWidget(groupBox);
    QGridLayout * test = new QGridLayout(groupBox);

    QLabel * l = new QLabel(groupBox);
    l->setText("C3t3 numéro VG :");
    test->addWidget(l, 0, 0, 1, 1);
    maillageNumber1 = new QSpinBox(groupBox);
    maillageNumber1->setRange(0,0);
    test->addWidget(maillageNumber1, 0, 1, 1, 1);
    QLabel * lb = new QLabel(groupBox);
    lb->setText("C3t3 numéro VD :");
    test->addWidget(lb, 1, 0, 1, 1);
    maillageNumber2 = new QSpinBox(groupBox);
    maillageNumber2->setRange(0,0);
    test->addWidget(maillageNumber2, 1, 1, 1, 1);

    QGroupBox * groupBox2 = new QGroupBox("View",parent);
    groupBox2->setMaximumSize(QSize(8388607, 200));
    contentLayout->addWidget ( groupBox2) ;

    QGroupBox * groupBox3 = new QGroupBox("Comparaison",parent);
    groupBox3->setMaximumSize(QSize(8388607, 200));
    contentLayout->addWidget( groupBox3);

    addMaillage = new QPushButton("ADD NEW C3T3", parent);
    contentLayout->addWidget(addMaillage);

    QGridLayout * viewGridLayout = new QGridLayout(groupBox2);
    QPushButton * Edges = new QPushButton("Only Edges", groupBox2);
    viewGridLayout->addWidget(Edges, 0, 0, 1, 1);
    QPushButton * Faces = new QPushButton("Only Faces", groupBox2);
    viewGridLayout->addWidget(Faces, 1, 0, 1, 1);
    QPushButton * Vertices = new QPushButton("Only Vertices", groupBox2);
    viewGridLayout->addWidget(Vertices, 2, 0, 1, 1);

    QGridLayout * compGridLayout = new QGridLayout(groupBox3);

    QCheckBox * CurvesActive = new QCheckBox("Only Polyline ?", groupBox3);
    compGridLayout->addWidget(CurvesActive, 1, 0, 1, 2);
    QLabel * l1 = new QLabel(groupBox3);
    l1->setText("N° Curve(poly) :");
    compGridLayout->addWidget(l1, 2, 0, 1, 1);
    QSpinBox * Curves = new QSpinBox( groupBox3);
    compGridLayout->addWidget(Curves, 2, 1, 1, 1);

    QLabel * l2 = new QLabel(groupBox3);
    l2->setText("N° Edge(poly) :");
    compGridLayout->addWidget(l2, 3, 0, 1, 1);
    EdgesPolyligne = new QSpinBox( groupBox3);
    compGridLayout->addWidget(EdgesPolyligne, 3, 1, 1, 1);

    QCheckBox * activeSubdomain = new QCheckBox("Only one Subdomain ?", groupBox3);
    compGridLayout->addWidget(activeSubdomain, 4, 0, 1, 2);

    QLabel * l3 = new QLabel(groupBox3);
    l3->setText("N° SubDomain :");
    compGridLayout->addWidget(l3, 5, 0, 1, 1);
    subdomain = new QSpinBox( groupBox3);
    subdomain->setRange(0, 2);
    compGridLayout->addWidget(subdomain, 5, 1, 1, 1);

    activeSubdomainP = new QCheckBox("Only polyline on one Subdomain ?", groupBox3);
    compGridLayout->addWidget(activeSubdomainP, 6, 0, 1, 2);
    QLabel * l4 = new QLabel(groupBox3);
    l4->setText("N° SubdomainP :");
    compGridLayout->addWidget(l4, 7, 0, 1, 1);
    subdomainP = new QSpinBox(groupBox3);
    subdomainP->setRange(0,2);
    compGridLayout->addWidget(subdomainP, 7, 1, 1, 1);

    projPoly = new QCheckBox("Projection polyligne sur viewer droite", groupBox3);
    compGridLayout->addWidget(projPoly, 8, 0, 1, 2);
    QLabel * l5 = new QLabel(groupBox3);
    l5->setText("N° polyligne :");
    compGridLayout->addWidget(l5, 9, 0, 1, 1);
    projPolySB = new QSpinBox(groupBox3);
    projPolySB->setRange(0,2);
    compGridLayout->addWidget(projPolySB, 9, 1, 1, 1);

    if(DEBUG) std::cout << "DISPLAYDOCWIDGET : Connect elements" << std::endl;

    connect(maillageNumber1, QOverload<int>::of(&QSpinBox::valueChanged), m_viewer1, [=](int i) {
        updateMaillage(i,m_viewer1);
    });
    connect(maillageNumber2, QOverload<int>::of(&QSpinBox::valueChanged), m_viewer2, [=](int i) {
        updateMaillage(i,m_viewer2);
    });
    connect(addMaillage, &QPushButton::clicked, this, [=]() {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),"/path/to/file/",tr("C3t3 Files (*.mesh)"));
        openMesh(fileName);
        maillageNumber1->setRange(0,c3t3_list.size()-1);
        maillageNumber2->setRange(0,c3t3_list.size()-1);
    });

    connect(CurvesActive, &QCheckBox::clicked, this, [=](bool b) {
        m_viewer1->activePolyline(b,Curves->value(),EdgesPolyligne->value());
        m_viewer2->activePolyline(b,Curves->value(),EdgesPolyligne->value());
    });
    connect(Curves, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int i) {
        m_viewer1->updateIndexPoly(i, EdgesPolyligne->value());
        m_viewer2->updateIndexPoly(i, EdgesPolyligne->value());
    });
    connect(EdgesPolyligne, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int i) {
        m_viewer1->updateIndexPoly(Curves->value(),i);
        m_viewer2->updateIndexPoly(Curves->value(),i);
    });
    connect(Edges, &QPushButton::clicked, this, [=]() {
        m_viewer1->onlyEdges();
        m_viewer2->onlyEdges();
    });
    connect(Faces, &QPushButton::clicked, this, [=]() {
        m_viewer1->onlyFaces();
        m_viewer2->onlyFaces();
    });
    connect(Vertices, &QPushButton::clicked, this, [=]() {
        m_viewer1->onlyVertices();
        m_viewer2->onlyVertices();
    });

    connect(activeSubdomain, &QCheckBox::clicked, this, [=](bool b) {
        m_viewer1->activeSubdomain(b, subdomain->value());
        m_viewer2->activeSubdomain(b, subdomain->value());
    });
    connect(subdomain, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int i) {
        subdomain->setRange(0,std::max(m_viewer1->selectSubdomain(i),m_viewer2->selectSubdomain(i)));
    });

    connect(activeSubdomainP, &QCheckBox::clicked, this, [=](bool b) {
       m_viewer1->activeSubdomainP(b, subdomain->value());
       m_viewer2->activeSubdomainP(b, subdomain->value());
    });
    connect(subdomainP, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int i) {
       subdomainP->setRange(0,std::max(m_viewer1->selectSubdomainP(i), m_viewer2->selectSubdomainP(i)));
    });

    connect(projPoly, &QCheckBox::clicked, this, [=](bool b) {
        m_viewer1->activePolyline(b,projPolySB->value(),0);
        m_viewer2->activeProjPoly(b,projPolySB->value());
    });
    connect(projPolySB, QOverload<int>::of(&QSpinBox::valueChanged), this, [=](int i) {
        m_viewer1->updateIndexPoly(i, 0);
        int t = maillageNumber1->value();
        m_viewer2->projection(c3t3_list.at(t), polyLines_list.at(t).at(i));
    });


    this->openMesh("data/out.mesh");
    this->setWidget(contents);
}

void DisplayDockWidget::openMesh(const QString &filename){

    if(DEBUG) {
        std::cout << "DISPLAYDOCWIDGET : OpenMesh" << std::endl;
        std::cout << filename.toStdString() << std::endl;
    }

    std::ifstream c3t3_load(filename.toStdString());
    C3t3 t;
    c3t3_load >> t;

    if(DEBUG) std::cout << "DISPLAYDOCWIDGET : remplissage des dimensions c3t3 + remplissages des egdes caracteristiques" << std::endl;
    // remplissage des dimensions c3t3 + remplissages des egdes caracteristiques
      std::vector<C3t3::Edge> CaracEdge;
      c3t3Param(t,CaracEdge);

      //////////////////////////////////////////////////////////////////////////////////////////////////////
      ///////////////////////////////////////////////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////////////////////////////////////////////

      if(DEBUG) std::cout << "DISPLAYDOCWIDGET : test comparaisons" << std::endl;
      //test comparaisons
      typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr>::Vertices_in_complex_iterator Complex_Vertex_Iterator;

      // vertex ***************************

      if(DEBUG) std::cout << "DISPLAYDOCWIDGET : C3T3 Number of cells : " << t.number_of_cells() << std::endl;
      int i = 0;
      std::map<C3t3::Vertex_handle, int> V;
      for (Complex_Vertex_Iterator it = t.vertices_in_complex_begin(); it != t.vertices_in_complex_end(); ++it)
      {
        if (t.in_dimension(it) == 0)
        {
          V[it] = i;
          //std::cout << "Vertex #" << i << " : " << it->point() << " Dimension : " << c3t3.in_dimension(it) << std::endl;
        }
        i++;
      }

      if(DEBUG) std::cout << "DISPLAYDOCWIDGET : Taille V : " << V.size() << " (nombre de vertex de dimension 0 dans c3t3)" << std::endl;

      ///////////////////////////////////////////////
      // edges ****************************************

      i = 0;
      std::map<C3t3::Edge, int> V_e;
      for (C3t3::Edges_in_complex_iterator it = t.edges_in_complex_begin(); it != t.edges_in_complex_end(); ++it)
      {
        // add the current Point_3 to the map with its current index
        C3t3::Edge edge = *it;
        V_e[edge] = i;
        ++i;
      }

      if(DEBUG) std::cout << "DISPLAYDOCWIDGET : Taille V_e : " << V_e.size() << " (nombre d'edge caracteristiques dans c3t3)" << std::endl;

      //////////////////////////////////////////////////////////////////////////////////////
      ////////////////////////////////////////////////////////////////////////////////////

      //polylignes

      if(DEBUG) std::cout << "DISPLAYDOCWIDGET : deb poly" << std::endl;
        //c3t3
        int curveNum_c3t3=1;
      std::vector<bool> used;
      for (unsigned int i = 0; i < CaracEdge.size(); i++){
        used.push_back(false);
      }

      std::vector<std::vector<C3t3::Edge>> p;

        for (int i = 0; i < CaracEdge.size(); i++) {
          if ((int)(t.in_dimension(CaracEdge[i].first->vertex(CaracEdge[i].second))) == 0 && used[i] == false) {
              std::vector<C3t3::Edge> tempPolyLine;
              parcoursPolyBis(tempPolyLine, used, CaracEdge, i, 2, t, curveNum_c3t3);
              p.push_back(tempPolyLine);
          }
          else {
              if ((int)(t.in_dimension(CaracEdge[i].first->vertex(CaracEdge[i].third))) == 0 && used[i] == false) {
                  std::vector<C3t3::Edge> tempPolyLine;
                  parcoursPolyBis(tempPolyLine, used, CaracEdge, i, 3, t, curveNum_c3t3);
                  p.push_back(tempPolyLine);
            }
          }
      }

      for (unsigned int i = 0; i < CaracEdge.size(); i++){
        if(used[i]==false){
          std::vector<C3t3::Edge> tempPolyLine;
          parcoursPoly(tempPolyLine, used, CaracEdge, i, t,curveNum_c3t3);
          p.push_back(tempPolyLine);
          curveNum_c3t3++;
        }
      }

      if(DEBUG) std::cout << "DISPLAYDOCWIDGET : fin poly" << std::endl;

      std::set<Subdomain_index> sdi;
      std::set<Surface_index> sfi;
      std::map<Subdomain_index, QColor> sdc;

      for (C3t3::Cells_in_complex_iterator cit = t.cells_in_complex_begin () ; cit != t.cells_in_complex_end (); ++cit ) {
          sdi.insert(t.subdomain_index(cit));
      }

      for (C3t3::Facets_in_complex_iterator fit = t.facets_in_complex_begin () ; fit != t.facets_in_complex_end (); ++fit ) {
          sfi.insert(t.surface_index(*fit));
      }

      if(DEBUG) std::cout << "DISPLAYDOCWIDGET : debut color subdomain mapping" << std::endl;
      int nb = 0;
      for (auto it = sdi.begin(); it != sdi.end(); ++it){
          QColor color;
          color.setHsvF(0.05 + 0.9*double(nb++)/double(sdi.size()), 0.6 ,1. );
          sdc[*it] = color;
          //std::cout << *it<< std::endl;
      }

      if(DEBUG) std::cout << "DISPLAYDOCWIDGET : fin color subdomain mapping" << std::endl;
//      if(DEBUG) std::cout << t << std::endl;
      //if(DEBUG) std::cout << "DISPLAYDOCWIDGET : 0" << std::endl;
      if(DEBUG) std::cout << "DISPLAYDOCWIDGET : len : " <<  c3t3_list.size() << std::endl;


      //try {
          c3t3_list.push_back(t);
      //} catch( int n ) {
      //        std::cerr << n << std::endl;
      //}

      //if(DEBUG) std::cout << "DISPLAYDOCWIDGET : 1" << std::endl;

      polyLines_list.push_back(p);
      //if(DEBUG) std::cout << "DISPLAYDOCWIDGET : 2" << std::endl;

      subdomain_indices_list.push_back(sdi);
      //if(DEBUG) std::cout << "DISPLAYDOCWIDGET : 3" << std::endl;

      surface_indices_list.push_back(sfi);
      //if(DEBUG) std::cout << "DISPLAYDOCWIDGET : 4" << std::endl;

      subdomain_colors_list.push_back(sdc);

      if(DEBUG) std::cout << "DISPLAYDOCWIDGET : end list interactions" << std::endl;
      std::vector<std::vector<std::vector<C3t3::Edge>>> groupPolyLines;

      getGroupPolyline(t,t.triangulation(),p,groupPolyLines);
      groupsPolyLines.push_back(groupPolyLines);

      if(DEBUG) std::cout << "DISPLAYDOCWIDGET : fin construction" << std::endl;
}

void DisplayDockWidget::updateMaillage(int i, Viewer * v) {

    if(DEBUG) std::cout << "DISPLAYDOCWIDGET : update maillage" << std::endl;

    if (i < (int)c3t3_list.size()) {
        v->updateC3t3(c3t3_list.at(i), polyLines_list.at(i), subdomain_indices_list.at(i), surface_indices_list.at(i), subdomain_colors_list.at(i), groupsPolyLines.at(i));
    } else
        std::cerr << "Error : not in range" << std::endl;

}
