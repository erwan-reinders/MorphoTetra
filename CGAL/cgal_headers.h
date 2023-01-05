/*Based on this original code : https://github.com/Isnao/TER_GenMaillage/blob/master/viewer/src/cgal_headers.h*/

#ifndef CGAL_HEADERS_H
#define CGAL_HEADERS_H

#define BOOST_PARAMETER_MAX_ARITY 12

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Mesh_triangulation_3.h>
#include <CGAL/Mesh_complex_3_in_triangulation_3.h>
#include <CGAL/Mesh_domain_with_polyline_features_3.h>
#include <CGAL/Mesh_criteria_3.h>

#include <CGAL/Labeled_mesh_domain_3.h>
#include <CGAL/make_mesh_3.h>
#include <CGAL/Image_3.h>

// Domain
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Labeled_mesh_domain_3<K> Image_domain;
typedef CGAL::Mesh_domain_with_polyline_features_3<Image_domain> Mesh_domain;
// Triangulation

#ifdef CGAL_CONCURRENT_MESH_3
typedef CGAL::Parallel_tag Concurrency_tag;
#else
typedef CGAL::Sequential_tag Concurrency_tag;
#endif

// Triangulation
typedef CGAL::Mesh_triangulation_3<Mesh_domain,CGAL::Default,Concurrency_tag>::type Tr;

typedef CGAL::Mesh_complex_3_in_triangulation_3<Tr> C3t3;

// Criteria
typedef CGAL::Mesh_criteria_3<Tr> Mesh_criteria;


typedef C3t3::Cell_handle Cell_handle;
typedef C3t3::Vertex_handle Vertex_handle;
typedef C3t3::Facet Facet;

typedef C3t3::Subdomain_index Subdomain_index;
typedef C3t3::Surface_index Surface_index;
typedef C3t3::Surface_patch_index Surface_patch_index;
typedef C3t3::Corner_index Corner_index;
typedef C3t3::Curve_segment_index Curve_segment_index;
typedef C3t3::Point Point_3;

#endif // CGAL_HEADERS_H
