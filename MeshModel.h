#ifndef MESHMODEL_H
#define MESHMODEL_H


#include "GLSL/glsl_basicfunctions.h"
#include <QGLViewer/qglviewer.h>
#include <QGLViewer/vec.h>
#include <QGLViewer/camera.h>

#include "CGAL/cgal_headers.h"
#include "CGAL/Tetrahedron.h"
#include "CGAL/Triangle.h"


#include <set>
#include <map>
#include <QColor>

#include <CGAL/Labeled_mesh_domain_3.h>

#define DEBUGAPP true

class MeshModel{
private :
    std::vector<std::vector<C3t3::Edge>>              m_polyLines;
    std::vector<std::vector<std::vector<C3t3::Edge>>> m_groupPolyLines;

    std::vector<Subdomain_index>    m_subdomain_indices;
    std::vector<Surface_index>         m_surface_indices;

    std::vector <qglviewer::Vec>    m_vertices;
    std::vector <qglviewer::Vec>    m_verticesSmoothPolylines;
    std::vector <Triangle>          m_triangles;
    std::vector <Tetrahedron>       m_tetrahedra;

    std::vector<int> m_verticesSmoothPolylinesDimensions;
    std::vector<int> m_verticesDimensions;

    std::vector<Subdomain_index> m_vertices_subdomain_ids;
    std::vector<Subdomain_index> m_triangles_subdomain_ids;
    std::vector<Subdomain_index> m_tetrahedra_subdomain_ids;

    std::map <Subdomain_index, std::vector<unsigned int> > m_sortedVertices;
    std::map <Subdomain_index, std::vector<int> > m_sortedTriangles;
    std::map <Subdomain_index, std::vector<int> > m_sortedTetrahedra;
    std::map <Subdomain_index, std::vector<unsigned int> > m_sortedPolyLines;
    std::map <Subdomain_index, std::vector<unsigned int> > m_sortedSmoothPolylines;

    std::vector<qglviewer::Vec>                             m_trianglesNormals;
    std::vector<std::map<Subdomain_index, qglviewer::Vec> > m_verticesNormals;
    std::vector<int> m_CGAL_envelop;


    GLuint m_VAO, m_verticesBuffer, m_dimensionsBuffer, m_normalsBuffer, m_indexBuffer;
    GLuint m_verticesBufferPos, m_colorsBufferPos, m_normalsBufferPos,m_dimensionsBufferPos;

    GLuint m_VAO_smooth_Catmull, m_verticesBuffer_smooth_Catmull, m_dimensionsBuffer_smooth_Catmull;

    float m_smoothPolylineSubdivisionNumber;

    bool m_glslInitialised;

protected :
    void computeFacetsNormals();
    void computeFacetNormal(int t, qglviewer::Vec& normal);
    void computeVerticesNormals(unsigned int weight);

    void initGLSL_dimensions(QOpenGLExtraFunctions*  cur_glFunctions);
    void initGLSL_vertices(QOpenGLExtraFunctions*  cur_glFunctions);
    void initGLSL_normals(QOpenGLExtraFunctions*  cur_glFunctions);

    void initGLSL_CatmullVertices(QOpenGLExtraFunctions*  cur_glFunctions);
    void initGLSL_CatmullDimensions(QOpenGLExtraFunctions*  cur_glFunctions);

    void initGLSL_Default(ShaderProgram& renderingProgram);
    void initGLSL_Catmull(ShaderProgram& renderingProgram);

    void CGALGeometry(C3t3 & m_c3t3);

    void drawMeshTriangles(ShaderProgram& renderingProgram,std::map<Subdomain_index, bool> displayMap, std::map<Subdomain_index, QColor>& colorMap);
    void drawMeshTetra    (ShaderProgram& renderingProgram,std::map<Subdomain_index, bool> displayMap, std::map<Subdomain_index, QColor>& colorMap);

public:
    qglviewer::Vec  m_center;
    float           m_radius;

    MeshModel();
    virtual ~MeshModel();

    void initFromFile(QString filename,
                      double facetAngle,
                      double facetSize,
                      double facetApproximation,
                      double cellRatio,
                      double cellSize,
                      bool perturb,
                      bool exude);
    void initWithRemeshing(QString filename);

    void initDrawingBuffers(ShaderProgram& renderingProgram);
    void initGLSL(ShaderProgram& renderingProgram);
    void initMeshData(C3t3 & m_c3t3);

    void drawMesh(ShaderProgram& renderingProgram,std::map<Subdomain_index, bool> displayMap, std::map<Subdomain_index, QColor>& colorMap, int meshDrawMode);
    void drawVerticies(ShaderProgram&  renderingProgram,std::map<Subdomain_index, bool> displayMap);
    void drawPolylines(ShaderProgram&  renderingProgram,std::map<Subdomain_index, bool> displayMap, int polylineDrawMode);

    void recomputeNormals();

    std::vector<std::vector<C3t3::Edge>> & getPolyLines(){return m_polyLines;}
    const std::vector<std::vector<C3t3::Edge>> & getPolyLines() const {return m_polyLines;}

    std::vector<std::vector<std::vector<C3t3::Edge>>> & getGroupPolyLines(){return m_groupPolyLines;}
    const std::vector<std::vector<std::vector<C3t3::Edge>>> & getGroupPolyLines() const {return m_groupPolyLines;}

    std::vector<Subdomain_index> & getSubdomainsIndex(){return m_subdomain_indices;}
    const std::vector<Subdomain_index> & getSubdomainsIndex() const {return m_subdomain_indices;}

    std::vector<Surface_index>  & getSurfaceIndex(){return m_surface_indices;}
    const std::vector<Surface_index> &  getSurfaceIndex() const {return m_surface_indices;}

    std::vector <qglviewer::Vec> & getVertices(){return m_vertices;}
    const std::vector <qglviewer::Vec> & getVertices() const {return m_vertices;}

    std::vector <Triangle> & getTriangles(){return m_triangles;}
    const std::vector <Triangle> & getTriangles() const {return m_triangles;}

    std::vector <Tetrahedron> & getTetrahedra(){return m_tetrahedra;}
    const std::vector <Tetrahedron> & getTetrahedra() const {return m_tetrahedra;}

    bool initialized();
};

#endif // MESHMODEL_H
