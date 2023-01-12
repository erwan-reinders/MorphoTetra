#ifndef MULTIMESHVIEWER_H
#define MULTIMESHVIEWER_H

#pragma once
#include "QGLViewer/qglviewer.h"
#include <vector>
#include "MeshModel.h"

class MultiMeshViewer: public QGLViewer{
    Q_OBJECT

private :
    std::vector<MeshModel> m_meshes;
    int m_curModel;

    std::map<Subdomain_index, QColor>   m_colorMap;
    std::map<Subdomain_index, bool>     m_displayMap;
    std::map<Subdomain_index, QColor>   m_fileColors;

    //Rendering programs
    QOpenGLContext*         cur_glContext;
    QOpenGLExtraFunctions*  cur_glFunctions;
    GLuint                  cur_programID;

    //Mesh OpenGL prog
    QOpenGLContext*         mesh_glContext;
    QOpenGLExtraFunctions*  mesh_glFunctions;
    GLuint                  mesh_programID;

    //Points and Polylines OpenGL prog
    QOpenGLContext*         pointNlines_glContext;
    QOpenGLExtraFunctions*  pointNlines_glFunctions;
    GLuint                  pointNlines_programID;

    bool m_drawWireFrame;
    bool m_drawMesh;
    bool m_drawPoints;
    bool m_drawPolylines;

public:
    MultiMeshViewer(QWidget *parent);
    virtual ~MultiMeshViewer();

    void selectAll();
    void discardAll();
    void setVisibility(unsigned int i, bool visibility);
    void setDrawMesh(int state);
    void setDrawVertices(int state);
    void setDrawPolylines(int state);

    std::map<Subdomain_index, QColor> getColorMap(){return m_colorMap;}
    const std::map<Subdomain_index, QColor> getColorMap()const {return m_colorMap;}

protected :
    virtual void init();
    virtual void draw();

    void clear();
    void initMatrix(QOpenGLExtraFunctions*  cur_glFunctions);

    void initLigthAndMaterial();

    void initCurrentDisplayedMesh();
    void initAllMesh();

    void computeRandomColors(const std::vector<int> & subdomain_indices, std::map<Subdomain_index, QColor> & cMap);
    void compileRenderingPrograms();

public slots :
    void test(){};
signals:
   void setMaxCutPlanes(int _xMax, int _yMax, int _zMax);
   void setMeshSubdomains();
};

#endif // MULTIMESHVIEWER_H
