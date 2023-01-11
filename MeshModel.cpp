#include "MeshModel.h"
#include "QGLViewer/manipulatedFrame.h"

#include "CGAL/cgal_basicfunctions.h"

QOpenGLContext* MeshModel::cur_glContext = 0;
QOpenGLExtraFunctions* MeshModel::cur_glFunctions = 0;
GLuint MeshModel::cur_programID = 0;

void MeshModel::computeShaderPrograms(){
    if(DEBUGAPP) std::cout << "[Model] compute shader program" << std::endl;

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

MeshModel::MeshModel(const char* filename){
    if(DEBUGAPP) std::cout << "[Model] construction" << std::endl;
    initCGAL(filename);

    m_verticesBufferPos = 0;
    m_colorsBufferPos   = 2;
    m_normalsBufferPos  = 1;
}

MeshModel::~MeshModel(){}

void MeshModel::initGLSL_vertices(){
    if(DEBUGAPP) std::cout << "[Model] init GLSL vertices" << std::endl;

    std::vector<float>  drawVertices;
    //drawVertices.resize(m_vertices.size() * 3 * 2);
    //int deb_second_color = m_vertices.size() * 3;

    for(unsigned int i = 0; i<m_vertices.size(); i++){
        //drawVertices[i] = (m_vertices[i][0]);
        //drawVertices[i] = (m_vertices[i][1]);
        //drawVertices[i] = (m_vertices[i][2]);
        drawVertices.push_back(m_vertices[i][0]);
        drawVertices.push_back(m_vertices[i][1]);
        drawVertices.push_back(m_vertices[i][2]);

        //drawVertices[i+deb_second_color] = (m_vertices[i][0]);
        //drawVertices[i+deb_second_color] = (m_vertices[i][1]);
        //drawVertices[i+deb_second_color] = (m_vertices[i][2]);
    }

    cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_verticesBuffer);
    cur_glFunctions->glBufferData(GL_ARRAY_BUFFER, drawVertices.size() * sizeof(float), &drawVertices[0], GL_STATIC_DRAW);
    cur_glFunctions->glVertexAttribPointer(m_verticesBufferPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    cur_glFunctions->glEnableVertexAttribArray(m_verticesBufferPos);
    checkOpenGLError();

    drawVertices.clear();
}

void MeshModel::initGLSL_normals(){
    if(DEBUGAPP) std::cout << "[Model] init GLSL normals" << std::endl;

    std::vector<float>  drawNormals;
    for(unsigned int i = 0; i<m_vertices.size(); i++){
        qglviewer::Vec cur_normal(0.0f,0.0f,0.0f);

        for(std::map<Subdomain_index, qglviewer::Vec>::iterator it = m_verticesNormals[i].begin() ; it != m_verticesNormals[i].end() ; it ++ ){
            cur_normal += it->second;
        }

        float norm = cur_normal.norm();
        if( norm > 1.0e10){
            cur_normal/=norm;
        }

        drawNormals.push_back(cur_normal[0]);
        drawNormals.push_back(cur_normal[1]);
        drawNormals.push_back(cur_normal[2]);
    }

    cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_normalsBuffer);
    cur_glFunctions->glBufferData(GL_ARRAY_BUFFER, drawNormals.size() * sizeof(float), &drawNormals[0], GL_STATIC_DRAW);
    cur_glFunctions->glVertexAttribPointer(m_normalsBufferPos, 3, GL_FLOAT, GL_FALSE,  3 * sizeof(float), (void*)0);
    cur_glFunctions->glEnableVertexAttribArray(m_normalsBufferPos);
    checkOpenGLError();

    drawNormals.clear();
}

void MeshModel::initGLSL_colors(std::map<Subdomain_index, QColor>& colorMap){
    if(DEBUGAPP) std::cout << "[Model] init GLSL colors : " << colorMap.size() << std::endl;

    std::vector<float>  drawColors;
    //drawColors.resize(m_vertices.size() * 3 * 2);
    //drawColors.resize(m_subdomain_indices.size());


    //int deb_second_color = m_vertices.size() * 3;
    //std::vector<bool>  drawColorsBool;
    //drawColorsBool.resize(m_vertices.size());

    //std::vector<Subdomain_index>  drawColorsInd;
    //drawColorsInd.resize(m_vertices.size());

    std::map<Subdomain_index, QColor>::const_iterator itCol;
    QColor color;
    for(std::map<Subdomain_index, std::vector<int> >::iterator it = m_sortedTriangles.begin(); it != m_sortedTriangles.end() ; it ++){
        Subdomain_index si = it->first;
        itCol = colorMap.find(si);

        if( itCol == colorMap.end() )
            color.setHsvF(0.5, 1.,1.);
        else
            color =itCol->second;

        drawColors.push_back(color.redF());
        drawColors.push_back(color.greenF());
        drawColors.push_back(color.blueF());

        //for(unsigned int i = 0; i<it->second.size(); i++){
        //    Triangle t = m_triangles[it->second[i]];
        //    for(int el = 0; el < 3; el++){


                //if(drawColorsBool[t[el]]){
                //    if(si > drawColorsInd[t[el]]){
                //        drawColors[deb_second_color + t[el]*3]      = color.redF();
                //        drawColors[deb_second_color + t[el]*3+1]    = color.greenF();
                //        drawColors[deb_second_color + t[el]*3+2]    = color.blueF();
                //    }else{
                //       //On écrase de telle sorte à avoir le plus petit sous-domaine qui a sa couleur en premier
                //       float v_r =  drawColors[t[el]*3];
                //       float v_g =  drawColors[t[el]*3+1];
                //       float v_b =  drawColors[t[el]*3+2];

                //       drawColors[t[el]*3]     = color.redF();
                //       drawColors[t[el]*3+1]   = color.greenF();
                //       drawColors[t[el]*3+2]   = color.blueF();

                //       drawColors[deb_second_color + t[el]*3]      = v_r;
                //       drawColors[deb_second_color + t[el]*3+1]    = v_g;
                //       drawColors[deb_second_color + t[el]*3+2]    = v_b;
                //    }
                //}else{


                    //drawColors[t[el]*3]     = color.redF();
                    //drawColors[t[el]*3+1]   = color.greenF();
                    //drawColors[t[el]*3+2]   = color.blueF();


                //    drawColorsBool[t[el]] = true;
                //    drawColorsInd[t[el]]  = si;
                //}


            //}
        //}
    }

    cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_colorsBuffer);
    cur_glFunctions->glBufferData(GL_ARRAY_BUFFER, drawColors.size() * sizeof(float), &drawColors[0], GL_STATIC_DRAW);
    cur_glFunctions->glVertexAttribPointer(m_colorsBufferPos, 3, GL_FLOAT, GL_FALSE,  3 * sizeof(float), (void*)0);
    cur_glFunctions->glEnableVertexAttribArray(m_colorsBufferPos);
    checkOpenGLError();

    drawColors.clear();
    //drawColorsBool.clear();
    //drawColorsInd.clear();
}

void MeshModel::initGLSL_subdomains(){
    //TODO : mettre ne place un système de coloration et normalisation par triangles triés par sous-domaine
    if(DEBUGAPP) std::cout << "[Model] init GLSL subdomains " << std::endl;

    std::vector<float>  drawColors;
    drawColors.resize(m_vertices.size() * 3 * 2);
    for(std::map<Subdomain_index, std::vector<int> >::iterator it = m_sortedTriangles.begin(); it != m_sortedTriangles.end() ; it ++){
        Subdomain_index si = it->first;

    }

    cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_colorsBuffer);
    cur_glFunctions->glBufferData(GL_ARRAY_BUFFER, drawColors.size() * sizeof(float), &drawColors[0], GL_STATIC_DRAW);
    cur_glFunctions->glVertexAttribPointer(m_colorsBufferPos, 3, GL_FLOAT, GL_FALSE,  3 * sizeof(float), (void*)0);
    cur_glFunctions->glEnableVertexAttribArray(m_colorsBufferPos);
    checkOpenGLError();

    drawColors.clear();
    //drawColorsBool.clear();
    //drawColorsInd.clear();
}

void MeshModel::initGLSL(std::map<Subdomain_index, QColor>& colorMap){
    if(DEBUGAPP){
        std::cout << "[Model] init GLSL" << std::endl;

        std::cout << "vertices l : " << m_vertices.size() << std::endl;
        std::cout << "triangles l : " << m_triangles.size() << std::endl;
        std::cout << "sortedTriangles l : " << m_sortedTriangles.size() << std::endl;
    }

    recomputeNormals();

    cur_glFunctions->glUseProgram(cur_programID);


    cur_glFunctions->glGenVertexArrays(1,&m_VAO);

    cur_glFunctions->glGenBuffers(1, &m_verticesBuffer);
    cur_glFunctions->glGenBuffers(1, &m_normalsBuffer);
    cur_glFunctions->glGenBuffers(1, &m_colorsBuffer);
    cur_glFunctions->glGenBuffers(1, &m_indexBuffer);

    cur_glFunctions->glBindVertexArray(m_VAO);

    initGLSL_vertices();
    //cur_glFunctions->glVertexAttribDivisor(m_verticesBufferPos,3);
    //initGLSL_normals();

    //Mais pour les couleurs
    cur_glFunctions->glEnableVertexAttribArray(m_normalsBufferPos);
    //cur_glFunctions->glEnableVertexAttribArray(m_normalsBufferPos);
    //initGLSL_colors(colorMap);


    cur_glFunctions->glBindVertexArray(0);
    cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkOpenGLError();

    if(DEBUGAPP){
        std::cout << "[Model] init GLSL END" << std::endl;

        std::cout << "vertices buf : " << m_verticesBuffer << std::endl;
        std::cout << "index buf : " << m_indexBuffer << std::endl;
        std::cout << "normal buf : " << m_normalsBuffer << std::endl;
        std::cout << "color buf : " << m_colorsBuffer << std::endl;
    }

}


void MeshModel::CGALGeometry(C3t3 & m_c3t3){
    typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Triangulation;
    //typedef CGAL::Mesh_triangulation_3<Mesh_domain,CGAL::Default,Concurrency_tag>::type Triangulation;
    typedef Triangulation::Finite_facets_iterator Finite_facets_iterator;
    typedef Triangulation::Finite_vertices_iterator Finite_vertices_iterator;
    typedef Triangulation::Finite_cells_iterator Finite_cells_iterator;


    int indices[4][3] = {
        {3,1,2},
        {3,2,0},
        {3,0,1},
        {2,1,0}
    };

    const Triangulation & triangulation = m_c3t3.triangulation();

    std::map<Vertex_handle, int> VMap;
    int VCount = 0;

    m_vertices.clear();
    //    verticesDimensions.clear();
    m_vertices.resize(triangulation.number_of_vertices());
    //    verticesDimensions.resize(triangulation.number_of_vertices());

    int surf_v_count = 0;
    for(Finite_vertices_iterator vit = triangulation.finite_vertices_begin() ; vit != triangulation.finite_vertices_end() ; vit++){
        m_vertices[VCount] = qglviewer::Vec(vit->point().x(), vit->point().y(), vit->point().z());
        VMap[vit] = VCount++;
        if(m_c3t3.in_dimension(vit)==2)
            surf_v_count++;
    }

    std::cout << 100.*float(surf_v_count) / m_c3t3.triangulation().number_of_vertices() << " % of vertices on boundaries" << std::endl;

    for(Triangulation::Finite_facets_iterator fit = triangulation.finite_facets_begin() ; fit != triangulation.finite_facets_end() ; ++fit ){

        Facet mirror = triangulation.mirror_facet(*fit);

        if(m_c3t3.is_in_complex(fit->first, fit->second)){

            if(m_c3t3.is_in_complex(fit->first) && !triangulation.is_infinite(fit->first)){
                Cell_handle ch = fit->first;
                Subdomain_index si = m_c3t3.subdomain_index(ch);

                if(triangulation.is_infinite(mirror.first) || !m_c3t3.is_in_complex(mirror.first))
                    m_CGAL_envelop.push_back(m_triangles.size());

                Triangle t(VMap[ch->vertex(indices[fit->second][0])],
                        VMap[ch->vertex(indices[fit->second][1])] ,
                        VMap[ch->vertex(indices[fit->second][2])] , si);
                m_triangles.push_back(t);

                m_triangles_subdomain_ids.push_back(si);

            }


            if(m_c3t3.is_in_complex(mirror.first) && !triangulation.is_infinite(mirror.first)){
                Cell_handle ch = mirror.first;
                Subdomain_index si = m_c3t3.subdomain_index(ch);

                if(triangulation.is_infinite(fit->first) || !m_c3t3.is_in_complex(fit->first))
                    m_CGAL_envelop.push_back(m_triangles.size());

                m_triangles.push_back(Triangle(VMap[ch->vertex(indices[mirror.second][0])],
                                             VMap[ch->vertex(indices[mirror.second][1])] ,
                                             VMap[ch->vertex(indices[mirror.second][2])]));

                m_triangles_subdomain_ids.push_back(si);
            }
        }

    }

    std::map<Cell_handle, int> CMap;
    int count = 0;
    for(Triangulation::Finite_cells_iterator cit = triangulation.finite_cells_begin() ; cit != triangulation.finite_cells_end() ; ++cit ){
        CMap[cit] = count++;
    }


    for(Triangulation::Finite_cells_iterator cit = triangulation.finite_cells_begin() ; cit != triangulation.finite_cells_end() ; ++cit ){
        Subdomain_index si = m_c3t3.subdomain_index(cit) ;
        Tetrahedron tet = Tetrahedron(VMap[cit->vertex(0)], VMap[cit->vertex(1)], VMap[cit->vertex(2)], VMap[cit->vertex(3)], si);
        for( int i = 0 ; i < 4 ; i ++ ){
            if(!triangulation.is_infinite(cit->neighbor(i))){
                tet.setNeighbor( i, CMap[cit->neighbor(i)] );
            }
        }
        m_tetrahedra.push_back(tet);
        m_tetrahedra_subdomain_ids.push_back(si);
    }

    m_sortedTriangles.clear();
    m_sortedTetrahedra.clear();

    for(unsigned int t = 0 ; t < m_triangles.size() ; t++){
        m_sortedTriangles[ m_triangles_subdomain_ids[t] ].push_back(t);
    }

    for(unsigned int t = 0 ; t < m_tetrahedra.size() ; t++){
        m_sortedTetrahedra[ m_tetrahedra_subdomain_ids[t] ].push_back(t);
    }

    bool noZero = true;
    m_subdomain_indices.clear();
    for(std::map<int, std::vector<int> >::iterator it = m_sortedTriangles.begin(); it != m_sortedTriangles.end() ; it ++  ){
        if( it->first == 0 ) noZero = false;
        m_subdomain_indices.push_back(it->first);
    }
    //if(noZero) subdomain_indices.push_back(0);
    std::sort(m_subdomain_indices.begin() , m_subdomain_indices.end());
}

void MeshModel::initCGAL(const char* filename){
    if(DEBUGAPP) std::cout << "[Model] init CGAL" << std::endl;

    C3t3 m_c3t3;
    std::ifstream c3t3_load(filename);
    c3t3_load >> m_c3t3;

    CGAL::Bbox_3 bbox = m_c3t3.bbox();
    m_center = qglviewer::Vec ((bbox.xmax() - bbox.xmin())/2., (bbox.ymax() - bbox.ymin())/2., (bbox.zmax() - bbox.zmin())/2.);
    m_radius = std::max(std::max(bbox.xmax() - bbox.xmin(), bbox.ymax() - bbox.ymin()), bbox.zmax() - bbox.zmin())*2.;

    //testCGAL(m_c3t3);
    Tr & m_t = m_c3t3.triangulation();


    std::vector<C3t3::Edge> caracEdge;
    computeCaracteristicEdges(m_c3t3, caracEdge);

    getPolyline(m_polyLines, m_c3t3, caracEdge);
    getGroupPolyline(m_t, m_polyLines, m_groupPolyLines);

    //std::map<Vertex_handle, int> VMap;
    //getVerticesAndMap(m_c3t3 ,m_t, m_vertices, m_verticesDimensions, VMap);
    //std::map<Cell_handle, int> CMap;
    //getTriangulationAndCgalEnvelop(m_c3t3,m_t,VMap, m_triangles, m_triangles_subdomain_ids, m_CGAL_envelop, m_surface_indices);
    //getTetrahedronAndMap(m_c3t3,m_t, VMap,m_tetrahedra,m_tetrahedra_subdomain_ids, CMap);
    //sortTriangleAndTetraBySubdomainIndex(m_triangles,m_triangles_subdomain_ids ,m_tetrahedra,m_tetrahedra_subdomain_ids, m_sortedTriangles, m_sortedTetrahedra, m_subdomain_indices);


    CGALGeometry(m_c3t3);

    if(DEBUGAPP){
        std::cout << "[Model] init CGAL END" << std::endl;
        std::cout << "vertices size : " << m_vertices.size() << std::endl;
        std::cout << "triangles size : " << m_triangles.size() << std::endl;
        std::cout << "tetrahedra size : " << m_tetrahedra.size() << std::endl;
        std::cout << "polylines size : " << m_polyLines.size() << std::endl;
        std::cout << "group polylines size : " << m_groupPolyLines.size() << std::endl;
    }
}

void MeshModel::draw(qglviewer::Camera *camera, std::map<Subdomain_index, bool> displayMap, std::map<Subdomain_index, QColor>& colorMap){
    cur_glFunctions->glUseProgram(cur_programID);

    float pMatrix[16];
    float mvMatrix[16];
    camera->getProjectionMatrix(pMatrix);
    camera->getModelViewMatrix(mvMatrix);

    cur_glFunctions->glUniformMatrix4fv(
                cur_glFunctions->glGetUniformLocation(cur_programID, "proj_matrix")
                ,1, GL_FALSE, pMatrix);
    cur_glFunctions->glUniformMatrix4fv(
                cur_glFunctions->glGetUniformLocation(cur_programID, "mv_matrix")
                ,1, GL_FALSE, mvMatrix);

    cur_glFunctions->glBindVertexArray(m_VAO);

    std::map<Subdomain_index, QColor>::const_iterator itCol;
    QColor color;

    std::vector<unsigned int> drawIndex;
    float* drawNormals;

    //GLTools::initLightsDefault(cur_programID,cur_glFunctions);
    //MLoadStandard(cur_programID,cur_glFunctions, GLTools::MY_MATERIAL_01);

    for(std::map<Subdomain_index, std::vector<int> >::iterator it = m_sortedTriangles.begin(); it != m_sortedTriangles.end() ; it ++  ){
        Subdomain_index si = it->first;

        if(displayMap[si]){
            drawNormals = new float[m_vertices.size()*3]{};
            itCol = colorMap.find(si);
            if( itCol == colorMap.end() )
                color.setHsvF(0.5, 1.,1.);
            else
                color =itCol->second;

            for(unsigned int tr = 0; tr < it->second.size(); tr++){
                Triangle t = m_triangles[it->second[tr]];

                for(int i = 0; i<3;i++){
                    drawIndex.push_back(t[i]);

                    drawNormals[t[i]*3]     += (m_trianglesNormals[it->second[tr]][0]);
                    drawNormals[t[i]*3+1]   += (m_trianglesNormals[it->second[tr]][1]);
                    drawNormals[t[i]*3+2]   += (m_trianglesNormals[it->second[tr]][2]);
                }

            }

            cur_glFunctions->glUniform3f(
                        cur_glFunctions->glGetUniformLocation(cur_programID, "u_color")
                        ,color.redF(),color.greenF(),color.blueF());

            cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_normalsBuffer);
            cur_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*3 * sizeof(float), drawNormals, GL_STATIC_DRAW);
            cur_glFunctions->glVertexAttribPointer(m_normalsBufferPos, 3, GL_FLOAT, GL_TRUE,  3 * sizeof(float), (void*)0);

            cur_glFunctions->glDrawElements(GL_TRIANGLES,it->second.size() * 3,GL_UNSIGNED_INT, &drawIndex[0]);


            drawIndex.clear();
            delete[] drawNormals;

            cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
            checkOpenGLError();
        }
    }


    //if(drawIndex.size()>=3){
        //cur_glFunctions->glBindVertexArray(m_VAO);

        //cur_glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
        //cur_glFunctions->glBufferData(GL_ELEMENT_ARRAY_BUFFER, drawIndex.size() * sizeof(unsigned int), &drawIndex[0], GL_STATIC_DRAW);
        //checkOpenGLError();
        //cur_glFunctions->glDrawElements(GL_TRIANGLES,drawIndex.size(),GL_UNSIGNED_INT, 0);
        //checkOpenGLError();
    //}
    cur_glFunctions->glBindVertexArray(0);
    cur_glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshModel::recomputeNormals () {
    if(DEBUGAPP) std::cout << "[Model] recompute normals" << std::endl;

    computeFacetsNormals();
    computeVerticesNormals(true);
}

void MeshModel::computeFacetsNormals(){
    if(DEBUGAPP) std::cout << "[Model] recompute facet triangles" << std::endl;

    m_trianglesNormals.clear();
    for(unsigned int i = 0 ; i < m_triangles.size() ; i++){
        qglviewer::Vec normal;
        computeFacetNormal(i, normal);
        //if(DEBUGAPP) std::cout << normal[0] << "," << normal[1] << "," << normal[2] << std::endl;
        m_trianglesNormals.push_back(normal);
    }

}

void MeshModel::computeFacetNormal( int id , qglviewer::Vec& normal){
    const Triangle & t = m_triangles[id];
    //std::cout << "triangle : " << t.getVertex(0) << ","  << t.getVertex(1) << "," << t.getVertex(2) << std::endl;
    normal = cross(m_vertices[t.getVertex(1)] - m_vertices[t.getVertex(0)], m_vertices[t.getVertex(2)]- m_vertices[t.getVertex(0)]);
    //if(DEBUGAPP) std::cout << "normal : " << normal[0] << "," << normal[1] << "," << normal[2] << std::endl;
    normal.normalize();
}

void MeshModel::computeVerticesNormals(unsigned int normWeight){
    if(DEBUGAPP) std::cout << "[Model] compute vertices normal" << std::endl;

    m_verticesNormals.resize(m_vertices.size());

    for( std::map<Subdomain_index, std::vector<int> >::iterator it = m_sortedTriangles.begin() ; it != m_sortedTriangles.end() ; it++ ){

        const std::vector<int> & T = it->second;
        Subdomain_index si = it->first;
        for (unsigned int i = 0 ; i < T.size(); i++){

            const Triangle & t = m_triangles[T[i]];
            for (unsigned int  j = 0; j < 3; j++) {
                unsigned int vj = t.getVertex (j);
                float w = 1.0; // uniform weights
                qglviewer::Vec e0 = m_vertices[t.getVertex ((j+1)%3)] - m_vertices[vj];
                qglviewer::Vec e1 = m_vertices[t.getVertex ((j+2)%3)] - m_vertices[vj];

                if (normWeight == 1) { // area weight
                    w = cross (e0, e1).norm() / 2.0;
                } else if (normWeight == 2) { // angle weight
                    e0.normalize ();
                    e1.normalize ();
                    w = (2.0 - ((e0 * e1) + 1.0)) / 2.0;
                }
                if (w <= 0.0)
                    continue;

                std::map<Subdomain_index, qglviewer::Vec>::iterator it = m_verticesNormals[vj].find(si);

                const qglviewer::Vec & normal = m_trianglesNormals[T[i]];
                if( it == m_verticesNormals[vj].end() ){
                    m_verticesNormals[vj][si] = normal * w;
                } else {
                    it->second = it->second + normal * w;
                }

            }
        }

    }

    for(unsigned int i = 0 ; i < m_verticesNormals.size() ; i++){
        for(std::map<Subdomain_index, qglviewer::Vec>::iterator it = m_verticesNormals[i].begin(); it != m_verticesNormals[i].end() ; it++){
            it->second.normalize();
        }
    }
}
