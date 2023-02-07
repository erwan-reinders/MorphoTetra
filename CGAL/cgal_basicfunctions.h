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


#define DETAILED_CELL_RATIO 3.
#define DETAILED_CELL_SIZE 5.

#define DETAILED_FACET_ANGLE 30.
#define DETAILED_FACET_SIZE 2.
#define DETAILED_FACET_APPROXIMATION 4.


#define CRUDE_CELL_RATIO 50.
#define CRUDE_CELL_SIZE 50.

#define CRUDE_FACET_ANGLE 1.
#define CRUDE_FACET_SIZE 50.
#define CRUDE_FACET_APPROXIMATION 50.

bool edgeInVector(C3t3::Edge &edge, std::vector<C3t3::Edge> &tab);

void computeCaracteristicEdges(C3t3 &c3t3, std::vector<C3t3::Edge> &caracEdge);
// ==================== GESTION DES POLYLINES ====================
void parcoursPolyZero_rec(std::vector< C3t3::Edge >& polyLine, std::vector<bool>& used, const std::vector< C3t3::Edge > V, int lineNum, int point, C3t3& c3t3, int& curveNum);

void parcoursPoly_rec(std::vector< C3t3::Edge >& polyLine, std::vector<bool>& used, const std::vector< C3t3::Edge > V, int lineNum, C3t3& c3t3, int curveNum);

void getPolyline(std::vector<std::vector<C3t3::Edge>>& polyLines, C3t3& c3t3, const std::vector<C3t3::Edge> caracEdge);

void getGroupPolyline(Tr& t,std::vector<std::vector<C3t3::Edge>>& polyLines,std::vector<std::vector<std::vector<C3t3::Edge>>>& res);

void sortTriangleAndTetraBySubdomainIndex(const std::vector<Triangle> triangles, const std::vector<Subdomain_index> triangles_subdomain_ids ,const std::vector<Tetrahedron> tetrahedra,const std::vector<Subdomain_index> tetrahedra_subdomain_ids, std::map <Subdomain_index, std::vector<int> >& sortedTriangles, std::map <Subdomain_index, std::vector<int> >& sortedTetrahedra, std::vector<Subdomain_index>& subdomain_indices);

// ==================== GESTION DES PRIMITIVES GEOMETRIQUES ====================

void getVerticesAndMap(const C3t3 c3t3,const Tr t, std::vector<qglviewer::Vec>& vertices, std::vector<Subdomain_index> verticesDimensions, std::map<Vertex_handle, int>& VMap);

void getTriangulationAndCgalEnvelop(const C3t3 c3t3,const Tr t, std::map<Vertex_handle, int> VMap, std::vector<Triangle>& triangles, std::vector<Subdomain_index>& triangles_subdomain_ids, std::vector<int>& CGAL_envelop, std::vector<Surface_index> &surface_indices);

void getTetrahedronAndMap(const C3t3 c3t3,const Tr t, std::map<Vertex_handle, int>& VMap,std::vector<Tetrahedron>& tetrahedra,std::vector<Subdomain_index>& tetrahedra_subdomain_ids, std::map<Cell_handle, int> CMap);

void getC3t3FromMeshFile(const char* filename, C3t3 & m_c3t3);

void getC3t3FromInrFile(const char* filename, C3t3 & m_c3t3,
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
                     double facetAngle = FACET_ANGLE,
                     double facetSize = FACET_SIZE,
                     double facetApproximation = FACET_APPROXIMATION,
                     CGAL::Mesh_facet_topology facetTopology = CGAL::FACET_VERTICES_ON_SURFACE,
                     double cellRatio = CELL_RATIO,
                     double cellSize = CELL_SIZE,
                     bool perturb = false,
                     bool exude = false,
                     float smoothingSigma = -1.0);

#endif // CGAL_BASICFUNCTIONS_H
