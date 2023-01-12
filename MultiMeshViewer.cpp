#include "MultiMeshViewer.h"
#include <QGLViewer/manipulatedFrame.h>
#include "GLSL/glsl_basicfunctions.h"
#include <utility>

MultiMeshViewer::MultiMeshViewer(QWidget *parent) : QGLViewer(parent){
    m_meshes.push_back(MeshModel("data/out.mesh"));
    m_curModel = 0;

    const std::vector<Subdomain_index> & subdomain_indices = m_meshes[m_curModel].getSubdomainsIndex();
    m_displayMap.clear();
    m_colorMap.clear();
    for(unsigned int i = 0 ; i < subdomain_indices.size() ; i++)
        m_displayMap[subdomain_indices[i]] = true;

    computeRandomColors(subdomain_indices, m_colorMap);

    if(DEBUGAPP){
        for(std::map<Subdomain_index, QColor>::iterator it = m_colorMap.begin() ; it != m_colorMap.end(); ++it ){
            std::cout << "COLOR FOR  : " << it->first << " is = ( " << it->second.redF() << "," << it->second.greenF() << "," << it->second.blueF() << ")" << std::endl;
        }
    }

    //help();
}

MultiMeshViewer::~MultiMeshViewer(){
    clear();
}


void MultiMeshViewer::initMatrix(QOpenGLExtraFunctions*  cur_glFunctions){
    float pMatrix[16];
    float mvMatrix[16];
    camera()->getProjectionMatrix(pMatrix);
    camera()->getModelViewMatrix(mvMatrix);

    cur_glFunctions->glUniformMatrix4fv(
                cur_glFunctions->glGetUniformLocation(cur_programID, "proj_matrix")
                ,1, GL_FALSE, pMatrix);
    cur_glFunctions->glUniformMatrix4fv(
                cur_glFunctions->glGetUniformLocation(cur_programID, "mv_matrix")
                ,1, GL_FALSE, mvMatrix);
}

void MultiMeshViewer::compileRenderingPrograms(){
    compileShaderProgram_VF(&mesh_glContext, &cur_glFunctions, cur_programID, "./GLSL/shaders/morpho.vert", "./GLSL/shaders/morpho.frag");
    compileShaderProgram_VF(&cur_glContext, &cur_glFunctions, cur_programID, "./GLSL/shaders/morpho.vert", "./GLSL/shaders/morpho.frag");

    cur_glFunctions->glUseProgram(cur_programID);
    GLTools::initLightsDefault(cur_programID,cur_glFunctions);
    MLoadStandard(cur_programID,cur_glFunctions, GLTools::MY_MATERIAL_01);
    cur_glFunctions->glUseProgram(0);

    if(DEBUGAPP){
        std::cout << "curglcontext : " << cur_glContext << std::endl;
        std::cout << "curglfunctions : " << cur_glFunctions << std::endl;
        std::cout << "curprog : " << cur_programID << std::endl;
    }
}

void MultiMeshViewer::selectAll(){
    for(std::map<Subdomain_index, bool>::iterator it = m_displayMap.begin() ; it != m_displayMap.end(); ++it ){
            m_displayMap[it->first] = true;
    }
    update();
}

void MultiMeshViewer::discardAll(){
    for(std::map<Subdomain_index, bool>::iterator it = m_displayMap.begin() ; it != m_displayMap.end(); ++it ){
        m_displayMap[it->first] = false;
    }
    update();
}

void MultiMeshViewer::setVisibility(unsigned int i, bool visibility){
    if(m_displayMap.find(i) != m_displayMap.end()){
        m_displayMap[i] = visibility;
    }
    update();
}

void MultiMeshViewer::setDrawMesh(int state) {
    m_drawMesh = state;
    update();
}

void MultiMeshViewer::setDrawVertices(int state) {
    m_meshes[i].m_drawPoints = state;
    update();
}

void MultiMeshViewer::setDrawPolylines(int state) {
    m_meshes[i].m_drawPolylines = state;
    update();
}

void MultiMeshViewer::init(){
    restoreStateFromFile();
    setManipulatedFrame(new qglviewer::ManipulatedFrame());
    MeshModel::computeShaderPrograms();
    initAllMesh();

    initLigthAndMaterial();
    initCurrentDisplayedMesh();

    glDisable( GL_DEBUG_OUTPUT );

    emit setMeshSubdomains();
}

void MultiMeshViewer::draw(){
    //glClearColor(1.0,.5,.6,1.0);
    //glClear(GL_COLOR_BUFFER_BIT);
    //qglviewer::Vec cam = camera()->worldCoordinatesOf( qglviewer::Vec(0.,0.,0.) );
    //QColor defaultTMColor (0.7*255, 255, 0.32*255);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    compileRenderingPrograms();
    if(m_drawMesh){
        cur_glFunctions->glUseProgram(mesh_programID);
        glEnable(GL_DEPTH_TEST);
        m_meshes[m_curModel].drawMesh(mesh_glFunctions,m_displayMap,m_colorMap);
    }

    if(m_drawPolylines){
        cur_glFunctions->glUseProgram(mesh_programID);
        glDisable(GL_DEPTH_TEST);
        m_meshes[m_curModel].drawPolylines(pointNlines_glFunctions,m_displayMap);
    }

    if(m_drawPoints){
        cur_glFunctions->glUseProgram(mesh_programID);
        glDisable(GL_DEPTH_TEST);
        m_meshes[m_curModel].drawPolylines(pointNlines_glFunctions,m_displayMap);
    }
}

void MultiMeshViewer::clear(){
    m_meshes.clear();
}

void MultiMeshViewer::initLigthAndMaterial(){

}

void MultiMeshViewer::initCurrentDisplayedMesh(){
    std::cout << "[MULTI MESH VIEWER] init current displayed mesh" << std::endl;

    std::cout << "center : " << m_meshes[m_curModel].m_center[0] << "," <<  m_meshes[m_curModel].m_center[1] << "," << m_meshes[m_curModel].m_center[2] << std::endl;
    std::cout << "radius : " << m_meshes[m_curModel].m_radius << std::endl;
    camera()->setSceneCenter(m_meshes[m_curModel].m_center);
    camera()->setSceneRadius(m_meshes[m_curModel].m_radius);
    camera()->showEntireScene();
}
void MultiMeshViewer::initAllMesh(){
    for(unsigned int i  = 0; i< m_meshes.size(); i++){
        cur_glFunctions->glUseProgram(mesh_programID);
        m_meshes[i].initGLSL(mesh_glFunctions);
        cur_glFunctions->glUseProgram(pointNlines_programID);
        m_meshes[i].initGLSL(pointNlines_glFunctions);
    }
}

void MultiMeshViewer::computeRandomColors(const std::vector<int> & subdomain_indices, std::map<Subdomain_index, QColor> & cMap){
    cMap.clear();

    if( subdomain_indices.size() == 1 ){
        cMap[subdomain_indices[0]] = QColor(255, 255, 255);
    }

    else {
        std::vector< std::pair<int, int> > sorted_ids;
        int off = 0;
        for( unsigned int i = 0 ; i < subdomain_indices.size() ; i ++ ){
            if(subdomain_indices[i] == 0) off = 1;
            sorted_ids.push_back(std::make_pair(subdomain_indices[i], i));
        }

        std::sort( sorted_ids.begin(), sorted_ids.end() );


        for( unsigned int i = 0 ; i < sorted_ids.size() ; i ++ ){
            int si = sorted_ids[i].first;
            QColor col(0,0,0);
            if( si > 0 ){
                //                if(useRandomColor){
                //                    float r, g , b;
                //                    RGBColor::get_random_RGB_from_HSV(r,g,b);

                //                    col.setRgbF(r, g, b);
                //                } else {
                //                    //double h = (280 - int(360*double(i-off)/(sorted_ids.size()-off)));
                //                    //if( h <= 0. ) h = 360.;


                col.setHsvF(0.98 * double(i-off)/(sorted_ids.size()-off), 0.6,1.);
                if( m_fileColors.size() > 0 )
                    col = m_fileColors[si];
                //                }
            }
            cMap[si] = col;
        }
    }
}

