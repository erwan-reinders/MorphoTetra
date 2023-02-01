#ifndef MULTIMESHVIEWER_H
#define MULTIMESHVIEWER_H

#pragma once
#include "QGLViewer/qglviewer.h"
#include <vector>
#include "MeshModel.h"
#include <QStatusBar>
#include <QDir>

class MultiMeshViewer: public QGLViewer{
    Q_OBJECT

private :
    bool loadedData;
    std::vector<MeshModel> m_meshes;
    int m_curModel;

    std::map<Subdomain_index, QColor>   m_colorMap;
    std::map<Subdomain_index, bool>     m_displayMap;
    std::map<Subdomain_index, QColor>   m_fileColors;

    //Rendering programs
    //Mesh OpenGL prog
    ShaderProgram mesh_glProgram;
    //Points and Polylines OpenGL prog
    ShaderProgram pointNlines_glProgram;

    bool m_drawWireFrame;
    bool m_drawMesh;
    int  m_meshDrawMode;
    bool m_drawPoints;
    bool m_drawPolylines;
    int  m_polylineDrawMode;

    const int m_wantedDeltaTime = 33;

    double m_playerTime;
    double m_playerTimeScale;
    bool m_playerPaused;
public:
    MultiMeshViewer(QWidget *parent);
    virtual ~MultiMeshViewer();

    void selectAll();
    void discardAll();
    void setVisibility(unsigned int i, bool visibility);
    void setDrawMesh(bool state);
    void setMeshDrawMode(int mode);
    void setDrawVertices(int state);
    void setDrawPolylines(int state);
    void setPolylineDrawMode(int mode);

    void loadMeshes(QStatusBar *statusbar,QStringList filenames, QDir& directorySelected);

    std::map<Subdomain_index, QColor> getColorMap(){return m_colorMap;}
    const std::map<Subdomain_index, QColor> getColorMap()const {return m_colorMap;}

    bool m_cutPlaneActiveX;
    bool m_cutPlaneActiveY;
    bool m_cutPlaneActiveZ;
    qglviewer::Vec m_clippingPoint;
    qglviewer::Vec m_clippingNormal;
    qglviewer::Vec m_cut;
    qglviewer::Vec m_cutDirection;

protected :
    virtual void init();
    virtual void draw();

    void clear();
    void initMatrix(ShaderProgram & program);

    void initLigthAndMaterial();

    void initCurrentDisplayedMesh();
    void initAllMesh();

    void computeRandomColors(const std::vector<int> & subdomain_indices, std::map<Subdomain_index, QColor> & cMap);
    void compileRenderingPrograms();

public slots :
    void mainLoop();
    void onPlayerPaused();
    void onPlayerPlayed();
    void onPlayerValueChanged(double doubleValue);

    void setXCut(int val);
    void setYCut(int val);
    void setZCut(int val);

    void invertXCut();
    void invertYCut();
    void invertZCut();

    void setXCutDisplay(bool state);
    void setYCutDisplay(bool state);
    void setZCutDisplay(bool state);
signals:
   void setMaxCutPlanes(int _xMax, int _yMax, int _zMax);
   void setMeshSubdomains();
   void updatePlayerTime(double _playerTime);
   void pausePlayer();
};

#endif // MULTIMESHVIEWER_H
