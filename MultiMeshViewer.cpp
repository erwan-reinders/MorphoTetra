#include "MultiMeshViewer.h"
#include <QGLViewer/manipulatedFrame.h>
#include "GLSL/glsl_basicfunctions.h"
#include <utility>

MultiMeshViewer::MultiMeshViewer(QWidget *parent) : QGLViewer(parent){
    //m_meshes.push_back(MeshModel("data/out.mesh"));

    //m_meshes.push_back(MeshModel());
    //m_meshes[m_meshes.size()-1].initFromInrFile("data/patric/140317-Patrick-St8_fuse_seg_post_reg_t001.inr");
    //m_meshes.clear();
    /*

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
    }*/

    loadedData = false;

    m_drawWireFrame= false;
    m_drawMesh= true;
    m_drawPoints= false;
    m_drawPolylines = false;

    m_cutPlaneActiveX =  false;
    m_cutPlaneActiveY =  false;
    m_cutPlaneActiveZ =  false;
    m_clippingPoint =   qglviewer::Vec(0.0f, 0.0f, 0.0f);
    m_clippingNormal =  qglviewer::Vec(0.0f, 1.0f, 0.0f);
    m_cut =             qglviewer::Vec(0.0f, 0.0f, 0.0f);
    m_cutDirection =    qglviewer::Vec(1.0f, 1.0f, 1.0f);

    m_playerTime = 0.0;
    m_playerTimeScale = 1.0;
    m_playerPaused = true;

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(mainLoop()));
    timer->start(m_wantedDeltaTime);
    //help();

}

MultiMeshViewer::~MultiMeshViewer(){
    clear();
}

void MultiMeshViewer::initMatrix(ShaderProgram & program){
    float pMatrix[16];
    float mvMatrix[16];
    camera()->getProjectionMatrix(pMatrix);
    camera()->getModelViewMatrix(mvMatrix);

    program.glFunctions->glUniformMatrix4fv(
                program.glFunctions->glGetUniformLocation(program.programID, "proj_matrix")
                ,1, GL_FALSE, pMatrix);
    program.glFunctions->glUniformMatrix4fv(
                program.glFunctions->glGetUniformLocation(program.programID, "mv_matrix")
                ,1, GL_FALSE, mvMatrix);

    program.glFunctions->glUniform1i(
                program.glFunctions->glGetUniformLocation(program.programID, "visibility_checkX")
                ,m_cutPlaneActiveX);

    program.glFunctions->glUniform1i(
                program.glFunctions->glGetUniformLocation(program.programID, "visibility_checkY")
                ,m_cutPlaneActiveY);

    program.glFunctions->glUniform1i(
                program.glFunctions->glGetUniformLocation(program.programID, "visibility_checkZ")
                ,m_cutPlaneActiveZ);

    program.glFunctions->glUniform3f(
                program.glFunctions->glGetUniformLocation(program.programID, "clippingPoint")
                ,m_clippingPoint[0], m_clippingPoint[1], m_clippingPoint[2]);

    program.glFunctions->glUniform3f(
                program.glFunctions->glGetUniformLocation(program.programID, "clippingNormal")
                ,m_clippingNormal[0], m_clippingNormal[1], m_clippingNormal[2]);

    program.glFunctions->glUniform3f(
                program.glFunctions->glGetUniformLocation(program.programID, "cut")
                ,m_cut[0], m_cut[1], m_cut[2]);

    program.glFunctions->glUniform3f(
                program.glFunctions->glGetUniformLocation(program.programID, "cutDirection")
                ,m_cutDirection[0], m_cutDirection[1], m_cutDirection[2]);

}

void MultiMeshViewer::compileRenderingPrograms(){
    QOpenGLExtraFunctions* glFunctions = QOpenGLContext::currentContext()->extraFunctions();
    mesh_glProgram.compileShaderProgram_VF(glFunctions,"./GLSL/shaders/morpho.vert", "./GLSL/shaders/morpho.frag");
    //glFunctions->glUseProgram(0);
    pointNlines_glProgram.compileShaderProgram_VF(glFunctions,"./GLSL/shaders/morpho_simple.vert", "./GLSL/shaders/morpho_simple.frag");

    glFunctions->glUseProgram(mesh_glProgram.programID);
    GLTools::initLightsDefault(mesh_glProgram.programID,mesh_glProgram.glFunctions);
    MLoadStandard(mesh_glProgram.programID,mesh_glProgram.glFunctions, GLTools::MY_MATERIAL_01);
    glFunctions->glUseProgram(0);

    if(DEBUGAPP){
        std::cout << "meshglfunctions : " << mesh_glProgram.glFunctions << std::endl;
        std::cout << "meshprog : " << mesh_glProgram.programID << std::endl;

        std::cout << "pnlglfunctions : " << pointNlines_glProgram.glFunctions << std::endl;
        std::cout << "pnlprog : " << pointNlines_glProgram.programID << std::endl;
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
    m_drawPoints = state;
    update();
}

void MultiMeshViewer::setDrawPolylines(int state) {
    m_drawPolylines = state;
    update();
}

void MultiMeshViewer::setPolylineDrawMode(int mode) {
    m_polylineDrawMode = mode;
    update();
}

void MultiMeshViewer::init(){
    restoreStateFromFile();
    setManipulatedFrame(new qglviewer::ManipulatedFrame());
    compileRenderingPrograms();

    initLigthAndMaterial();

    glDisable( GL_DEBUG_OUTPUT );
}

void MultiMeshViewer::draw(){
    if (m_curModel >= m_meshes.size()) {
        return;
    }
    if (!m_meshes[m_curModel].initialized()) {
        m_meshes[m_curModel].initDrawingBuffers(mesh_glProgram);

        mesh_glProgram.glFunctions->glUseProgram(mesh_glProgram.programID);
        m_meshes[m_curModel].initGLSL(mesh_glProgram);

        pointNlines_glProgram.glFunctions->glUseProgram(pointNlines_glProgram.programID);
        m_meshes[m_curModel].initGLSL(pointNlines_glProgram);
        pointNlines_glProgram.glFunctions->glUseProgram(0);
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glPolygonMode (GL_FRONT_AND_BACK, GL_FILL);

    if(m_drawMesh){
        mesh_glProgram.glFunctions->glUseProgram(mesh_glProgram.programID);
        initMatrix(mesh_glProgram);
        //glEnable(GL_DEPTH_TEST);
        m_meshes[m_curModel].drawMesh(mesh_glProgram,m_displayMap,m_colorMap);
    }

    if(m_drawPolylines){
        pointNlines_glProgram.glFunctions->glUseProgram(pointNlines_glProgram.programID);
        initMatrix(pointNlines_glProgram);
        glDisable(GL_DEPTH_TEST);
        m_meshes[m_curModel].drawPolylines(pointNlines_glProgram,m_displayMap, m_polylineDrawMode);
    }

    if(m_drawPoints){
        pointNlines_glProgram.glFunctions->glUseProgram(pointNlines_glProgram.programID);
        initMatrix(pointNlines_glProgram);
        glDisable(GL_DEPTH_TEST);
        m_meshes[m_curModel].drawVerticies(pointNlines_glProgram,m_displayMap);
    }

    mesh_glProgram.glFunctions->glUseProgram(0);

}

void MultiMeshViewer::clear(){
    m_meshes.clear();
}

void MultiMeshViewer::mainLoop() {
    if (!m_playerPaused) {
        //Todo compute real deltaTime
        m_playerTime += ((double) m_wantedDeltaTime / 1000.0) * m_playerTimeScale;

        if (m_playerTime > 1.0) {
            m_playerTime = 1.0;
            m_playerPaused = true;
            emit pausePlayer();
        }

        emit updatePlayerTime(m_playerTime);
    }

    int meshId = (int) (m_playerTime * m_meshes.size());
    if (meshId >= m_meshes.size()) {
        meshId = m_meshes.size()-1;
    }
    if (m_curModel != meshId) {
        m_curModel = meshId;
        update();
    }
}


void MultiMeshViewer::onPlayerPaused() {
    m_playerPaused = true;
}

void MultiMeshViewer::onPlayerPlayed() {
    m_playerPaused = false;
    if (m_playerTime >= 1.0) {
        m_playerTime = 0.0;
    }
}

void MultiMeshViewer::onPlayerValueChanged(double doubleValue) {
    m_playerTime = doubleValue;
}

void MultiMeshViewer::initLigthAndMaterial(){}

void MultiMeshViewer::initCurrentDisplayedMesh(){
    camera()->setSceneCenter(m_meshes[m_curModel].m_center);
    camera()->setSceneRadius(m_meshes[m_curModel].m_radius);
    camera()->showEntireScene();
}
void MultiMeshViewer::initAllMesh(){
    for(unsigned int i  = 0; i< m_meshes.size(); i++){
        m_meshes[i].initDrawingBuffers(mesh_glProgram);

        mesh_glProgram.glFunctions->glUseProgram(mesh_glProgram.programID);
        m_meshes[i].initGLSL(mesh_glProgram);

        pointNlines_glProgram.glFunctions->glUseProgram(pointNlines_glProgram.programID);
        m_meshes[i].initGLSL(pointNlines_glProgram);
        pointNlines_glProgram.glFunctions->glUseProgram(0);
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


void MultiMeshViewer::loadMeshes(QStatusBar *statusbar, QStringList filenames,  QDir& directorySelected){
    m_meshes.clear();
    QString filename;
    int i = 0;
    foreach (filename, filenames) {
        QString file = directorySelected.absoluteFilePath(filename);

        statusbar->showMessage("Opening tetra mesh : " + file);
        update();

        m_meshes.push_back(MeshModel());
        if (i%2 == 0) {
            m_meshes[m_meshes.size()-1].initFromFile(file);
        }
        else {
            m_meshes[m_meshes.size()-1].initWithRemeshing(file);
        }
        i++;

        statusbar->showMessage("Tetra mesh opened !");
        update();
    }


    m_curModel = 0;
    std::vector<Subdomain_index> subdomain_indices;
    for (int m = 0; m < m_meshes.size(); m++) {
        std::vector<Subdomain_index> & curent_subdomain_indices = m_meshes[m].getSubdomainsIndex();
        for (int si = 0; si < curent_subdomain_indices.size(); si++) {
            subdomain_indices.push_back(curent_subdomain_indices[si]);
        }
    }

    m_displayMap.clear();
    m_colorMap.clear();
    for(unsigned int i = 0 ; i < subdomain_indices.size() ; i++)
        m_displayMap[subdomain_indices[i]] = true;

    computeRandomColors(subdomain_indices, m_colorMap);

    //initAllMesh();
    initCurrentDisplayedMesh();
    emit setMeshSubdomains();

    emit setMaxCutPlanes(1000, 1000, 1000);
}

void MultiMeshViewer::setXCut(int val) {
    m_cut[0] = float(val);
    update();
}

void MultiMeshViewer::setYCut(int val) {
    m_cut[1] = float(val);
    update();
}

void MultiMeshViewer::setZCut(int val) {
    m_cut[2] = float(val);
    update();
}

void MultiMeshViewer::invertXCut() {
    m_cutDirection[0] = -m_cutDirection[0];
    update();
}

void MultiMeshViewer::invertYCut() {
    m_cutDirection[1] = -m_cutDirection[1];
    update();
}

void MultiMeshViewer::invertZCut() {
    m_cutDirection[2] = -m_cutDirection[2];
    update();
}

void MultiMeshViewer::setXCutDisplay(bool state) {
    m_cutPlaneActiveX = state;
    update();
}

void MultiMeshViewer::setYCutDisplay(bool state) {
    m_cutPlaneActiveY = state;
    update();
}

void MultiMeshViewer::setZCutDisplay(bool state) {
    m_cutPlaneActiveZ = state;
    update();
}
