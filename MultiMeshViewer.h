#ifndef MULTIMESHVIEWER_H
#define MULTIMESHVIEWER_H

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

public:
    MultiMeshViewer(QWidget *parent);
    virtual ~MultiMeshViewer();

    void selectAll();
    void discardAll();
    void setVisibility(unsigned int i, bool visibility);

    std::map<Subdomain_index, QColor> getColorMap(){return m_colorMap;}
    const std::map<Subdomain_index, QColor> getColorMap()const {return m_colorMap;}

protected :
    virtual void init();
    virtual void draw();

    void clear();

    void initLigthAndMaterial();

    void initCurrentDisplayedMesh();
    void initAllMesh();

    void computeRandomColors(const std::vector<int> & subdomain_indices, std::map<Subdomain_index, QColor> & cMap);


public slots :
    void test(){};
signals:
   void setMaxCutPlanes(int _xMax, int _yMax, int _zMax);
   void setMeshSubdomains();
};

#endif // MULTIMESHVIEWER_H
