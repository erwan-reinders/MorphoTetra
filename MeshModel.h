#ifndef MESHMODEL_H
#define MESHMODEL_H

#include "CGAL/cgal_headers.h"
#include "GLSL/glsl_basicfunctions.h"
#include "GLSL/GLUtilityMethods.h"
#include "CGAL/Tetrahedron.h"
#include "CGAL/Triangle.h"


#include <QGLViewer/qglviewer.h>
#include <QGLViewer/vec.h>>
#include <QGLViewer/camera.h>

#include <set>
#include <map>
#include <QColor>

#define DEBUGAPP true

class MeshModel{
private :
    std::vector<std::vector<C3t3::Edge>>              m_polyLines;
    std::vector<std::vector<std::vector<C3t3::Edge>>> m_groupPolyLines;

    std::vector<Subdomain_index>    m_subdomain_indices;
    std::vector<Surface_index>         m_surface_indices;

    std::vector <qglviewer::Vec>    m_vertices;
    std::vector <Triangle>          m_triangles;
    std::vector <Tetrahedron>       m_tetrahedra;

    std::vector<int> m_verticesDimensions;

    std::vector<Subdomain_index> m_vertices_subdomain_ids;
    std::vector<Subdomain_index> m_triangles_subdomain_ids;
    std::vector<Subdomain_index> m_tetrahedra_subdomain_ids;
    std::map <Subdomain_index, std::vector<int> > m_sortedVertices;
    std::map <Subdomain_index, std::vector<int> > m_sortedTriangles;
    std::map <Subdomain_index, std::vector<int> > m_sortedTetrahedra;

    std::vector<qglviewer::Vec>                             m_trianglesNormals;
    std::vector<std::map<Subdomain_index, qglviewer::Vec> > m_verticesNormals;
    std::vector<int> m_CGAL_envelop;


    GLuint m_VAO, m_verticesBuffer, m_colorsBuffer, m_colorsBuffer2, m_normalsBuffer, m_normalsBuffer2, m_indexBuffer;
    GLuint m_verticesBufferPos, m_colorsBufferPos, m_normalsBufferPos;

    static QOpenGLContext*         cur_glContext;
    static QOpenGLExtraFunctions*  cur_glFunctions;
    static GLuint                  cur_programID;

protected :
    void computeFacetsNormals();
    void computeFacetNormal(int t, qglviewer::Vec& normal);
    void computeVerticesNormals(unsigned int weight);

    void initGLSL_colors(std::map<Subdomain_index, QColor>& colorMap);
    void initGLSL_vertices();
    void initGLSL_normals();
    void initGLSL_subdomains();

    void CGALGeometry(C3t3 & m_c3t3);

    void drawMesh(std::map<Subdomain_index, bool> displayMap, std::map<Subdomain_index, QColor>& colorMap);
    void drawVerticies(std::map<Subdomain_index, bool> displayMap, std::map<Subdomain_index, QColor>& colorMap);
    void drawPolylines(std::map<Subdomain_index, bool> displayMap, std::map<Subdomain_index, QColor>& colorMap);
public:
    qglviewer::Vec  m_center;
    float           m_radius;

    bool m_drawWireFrame;
    bool m_drawMesh;
    bool m_drawPoints;
    bool m_drawPolylines;

    MeshModel(const char* filename);
    virtual ~MeshModel();

    void draw(qglviewer::Camera *camera,std::map<Subdomain_index, bool> displayMap, std::map<Subdomain_index, QColor>& colorMap);
    void initGLSL(std::map<Subdomain_index, QColor>& colorMap);
    void initCGAL(const char* filename);

    void recomputeNormals();

    static void computeShaderPrograms();

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
};

#endif // MESHMODEL_H
