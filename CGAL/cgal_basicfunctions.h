#ifndef CGAL_BASICFUNCTIONS_H
#define CGAL_BASICFUNCTIONS_H

#include "cgal_headers.h"
#include <vector>

#include "Triangle.h"
#include "Tetrahedron.h"
#include "QGLViewer/vec.h"

#define CELL_RATIO 10.
#define CELL_SIZE 20.

#define FACET_ANGLE 30.
#define FACET_SIZE 10.
#define FACET_APPROXIMATION 5.

#define EDGE_SIZE 0.


#define DETAILED_CELL_RATIO 3.
#define DETAILED_CELL_SIZE 5.

#define DETAILED_FACET_ANGLE 30.
#define DETAILED_FACET_SIZE 2.
#define DETAILED_FACET_APPROXIMATION 4.

#define DETAILED_EDGE_SIZE 0.


#define CRUDE_CELL_RATIO 50.
#define CRUDE_CELL_SIZE 50.

#define CRUDE_FACET_ANGLE 1.
#define CRUDE_FACET_SIZE 50.
#define CRUDE_FACET_APPROXIMATION 50.

#define CRUDE_EDGE_SIZE 0.

bool edgeInVector(C3t3::Edge &edge, std::vector<C3t3::Edge> &tab);

void sortTriangleAndTetraBySubdomainIndex(const std::vector<Triangle> triangles, const std::vector<Subdomain_index> triangles_subdomain_ids ,const std::vector<Tetrahedron> tetrahedra,const std::vector<Subdomain_index> tetrahedra_subdomain_ids, std::map <Subdomain_index, std::vector<int> >& sortedTriangles, std::map <Subdomain_index, std::vector<int> >& sortedTetrahedra, std::vector<Subdomain_index>& subdomain_indices);

// ==================== GESTION DES POLYLINES ====================

// Permet de récupérer les arêtes caractéristiques de la triangulation.
// Pour cela, elle calcule les dimensions de chaque vertex
//  et parcours les arêtes afin de trouver les arêtes ayant deux points de dimension 1 ou moins.
void computeCaracteristicEdges(C3t3 &c3t3, std::vector<C3t3::Edge> &caracEdge);

// Permet de récupérer les polyLines.
// Cette méthode va donc regrouper les arêtes caractéristiques entre chaque embranchement.
void getPolyline(std::vector<std::vector<C3t3::Edge>>& polyLines, C3t3& c3t3, const std::vector<C3t3::Edge> caracEdge);
void parcoursPolyZero_rec(std::vector< C3t3::Edge >& polyLine, std::vector<bool>& used, const std::vector< C3t3::Edge > V, int lineNum, int point, C3t3& c3t3, int& curveNum);
void parcoursPoly_rec(std::vector< C3t3::Edge >& polyLine, std::vector<bool>& used, const std::vector< C3t3::Edge > V, int lineNum, C3t3& c3t3, int curveNum);

// Permet de regrouper les polylines en fonction des domaines incidents.
// Utile pour afficher ou cacher les polylines en fonction des domaines.
void getGroupPolyline(Tr& t,std::vector<std::vector<C3t3::Edge>>& polyLines,std::vector<std::vector<std::vector<C3t3::Edge>>>& res);


// Permet d'exrtaire une polyline directement depuis l'image 3D segmentée.
void getPolylineFrom3DImage(const CGAL::Image_3 & image, std::vector<Point_3> & polyLines);
void testGetPolylineFrom3DImage(const char* filename, std::vector<Point_3> & polyLines);

// ==================== GESTION DES PRIMITIVES GEOMETRIQUES ====================

void getVerticesAndMap(const C3t3 c3t3,const Tr t, std::vector<qglviewer::Vec>& vertices, std::vector<Subdomain_index> verticesDimensions, std::map<Vertex_handle, int>& VMap);

void getTriangulationAndCgalEnvelop(const C3t3 c3t3,const Tr t, std::map<Vertex_handle, int> VMap, std::vector<Triangle>& triangles, std::vector<Subdomain_index>& triangles_subdomain_ids, std::vector<int>& CGAL_envelop, std::vector<Surface_index> &surface_indices);

void getTetrahedronAndMap(const C3t3 c3t3,const Tr t, std::map<Vertex_handle, int>& VMap,std::vector<Tetrahedron>& tetrahedra,std::vector<Subdomain_index>& tetrahedra_subdomain_ids, std::map<Cell_handle, int> CMap);

// ==================== CREATION DE C3T3 ====================

void getC3t3FromMeshFile(const char* filename, C3t3 & m_c3t3);

void getC3t3FromInrFile(const char* filename, C3t3 & m_c3t3,
                        double edgeSize,
                        double facetAngle,
                        double facetSize,
                        double facetApproximation,
                        CGAL::Mesh_facet_topology facetTopology,
                        double cellRatio,
                        double cellSize,
                        bool perturb,
                        bool exude,
                        float smoothingSigma);

CGAL::Labeled_mesh_domain_3<K> getDomain(CGAL::Image_3 image, float smoothingSigma);

void getC3t3FromFile(QString filename, C3t3 & m_c3t3,
                     double edgeSize = EDGE_SIZE,
                     double facetAngle = FACET_ANGLE,
                     double facetSize = FACET_SIZE,
                     double facetApproximation = FACET_APPROXIMATION,
                     CGAL::Mesh_facet_topology facetTopology = CGAL::FACET_VERTICES_ON_SURFACE,
                     double cellRatio = CELL_RATIO,
                     double cellSize = CELL_SIZE,
                     bool perturb = false,
                     bool exude = false,
                     float smoothingSigma = -1.0);

void getC3t3FromInrFileWithFeatures(const char* filename, C3t3 & m_c3t3,
                                    double edgeSize = EDGE_SIZE,
                                    double facetAngle = FACET_ANGLE,
                                    double facetSize = FACET_SIZE,
                                    double facetApproximation = FACET_APPROXIMATION,
                                    double cellRatio = CELL_RATIO,
                                    double cellSize = CELL_SIZE,
                                    bool perturb = false,
                                    bool exude = false);

// Undocumented code from CGAL
//  from example "4.6.3 Domains from Segmented 3D Images, with 1D Features"
//  in file read_polylines.h
template <typename Point_3>
bool read_polylines(const std::string fname,
                    std::vector<std::vector<Point_3>> & polylines);


//  from example "4.6.3 Domains from Segmented 3D Images, with 1D Features"
//  in files mesh_3D_image_with_features.cpp
// Protect the intersection of the object with the box of the image,
// by declaring 1D-features. Note that `CGAL::polylines_to_protect` is
// not documented.
bool add_1D_features(const CGAL::Image_3& image,
                     Mesh_domain& domain,
                     const std::string lines_fname);

#endif // CGAL_BASICFUNCTIONS_H
