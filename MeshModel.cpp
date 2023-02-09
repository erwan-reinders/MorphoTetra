#include "MeshModel.h"


MeshModel::MeshModel(){
    if(DEBUGAPP) std::cout << "[Model] construction" << std::endl;

    m_smoothPolylineSubdivisionNumber = 8.0f;
    m_verticesBufferPos     = 0;
    m_normalsBufferPos      = 1;
    m_dimensionsBufferPos   = 2;

    m_glslInitialised = false;
}

MeshModel::~MeshModel(){}

bool MeshModel::initialized() {
    return m_glslInitialised;
}

// Initialise le modèle à partir d'un fichier (.mesh ou .inr)
void MeshModel::initFromFile(QStatusBar * statusbar,
                             QString filename,
                             double edgeSize,
                             double facetAngle,
                             double facetSize,
                             double facetApproximation,
                             CGAL::Mesh_facet_topology facetTopology,
                             double cellRatio,
                             double cellSize,
                             bool perturb,
                             bool exude) {
    C3t3 m_c3t3;
    //testGetPolylineFrom3DImage(filename.toStdString().c_str(), m_imagePolyLines);
    getC3t3FromFile(filename, m_c3t3,
                    edgeSize, facetAngle, facetSize, facetApproximation, facetTopology, cellRatio, cellSize, perturb, exude);

    statusbar->showMessage("Initializing data...");
    QCoreApplication::processEvents();

    initMeshData(m_c3t3);
}

void MeshModel::initWithRemeshing(QString filename) {
    std::cerr << "initWithRemeshing : NOT IMPLEMENTED" << std::endl;
    C3t3 m_c3t3;
    getC3t3FromFile(filename, m_c3t3);

//    Triangulation_3 tr = CGAL::convert_to_triangulation_3(std::move(m_c3t3));
//    const double target_edge_length = 1.0;
//    CGAL::tetrahedral_isotropic_remeshing(tr, target_edge_length, CGAL::parameters::number_of_iterations(3));

    initMeshData(m_c3t3);
}


void MeshModel::initDrawingBuffers(ShaderProgram& renderingProgram){
    renderingProgram.glFunctions->glGenVertexArrays(1, &m_VAO);
    renderingProgram.glFunctions->glGenBuffers(1, &m_verticesBuffer);
    renderingProgram.glFunctions->glGenBuffers(1, &m_normalsBuffer);
    renderingProgram.glFunctions->glGenBuffers(1, &m_dimensionsBuffer);
    renderingProgram.glFunctions->glGenBuffers(1, &m_indexBuffer);

    renderingProgram.glFunctions->glGenVertexArrays(1,&m_VAO_smooth_Catmull);
    renderingProgram.glFunctions->glGenBuffers(1, &m_verticesBuffer_smooth_Catmull);
    renderingProgram.glFunctions->glGenBuffers(1, &m_dimensionsBuffer_smooth_Catmull);

    renderingProgram.glFunctions->glGenVertexArrays(1,&m_VAO_imagePolyLines);
    renderingProgram.glFunctions->glGenBuffers(1, &m_verticesBuffer_imagePolyLines);
    renderingProgram.glFunctions->glGenBuffers(1, &m_dimensionsBuffer_imagePolyLines);
}

void MeshModel::initGLSL_vertices(QOpenGLExtraFunctions*  cur_glFunctions){
    if(DEBUGAPP) std::cout << "[Model] init GLSL vertices" << std::endl;

    std::vector<float>  drawVertices;
    drawVertices.reserve(m_vertices.size() * 3);

    for(unsigned int i = 0; i<m_vertices.size(); i++){
        drawVertices.push_back(m_vertices[i][0]);
        drawVertices.push_back(m_vertices[i][1]);
        drawVertices.push_back(m_vertices[i][2]);
    }

    cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_verticesBuffer);
    cur_glFunctions->glBufferData(GL_ARRAY_BUFFER, drawVertices.size() * sizeof(float), &drawVertices[0], GL_STATIC_DRAW);
    cur_glFunctions->glVertexAttribPointer(m_verticesBufferPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    cur_glFunctions->glEnableVertexAttribArray(m_verticesBufferPos);
    checkOpenGLError();

    drawVertices.clear();
}

void MeshModel::initGLSL_normals(QOpenGLExtraFunctions*  cur_glFunctions){
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

void MeshModel::initGLSL_dimensions(QOpenGLExtraFunctions*  cur_glFunctions){
    if(DEBUGAPP) std::cout << "[Model] init GLSL dimensions : " << m_verticesDimensions.size() << std::endl;

    cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_dimensionsBuffer);
    cur_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_verticesDimensions.size() * sizeof(int), &m_verticesDimensions[0], GL_STATIC_DRAW);
    cur_glFunctions->glVertexAttribIPointer(m_dimensionsBufferPos, 1, GL_INT, 1.0 * sizeof(int), (void*)0);
    cur_glFunctions->glEnableVertexAttribArray(m_dimensionsBufferPos);
    checkOpenGLError();
}

void MeshModel::initGLSL_CatmullVertices(QOpenGLExtraFunctions*  cur_glFunctions){
    if(DEBUGAPP) std::cout << "[Model] init GLSL vertices CATMULL" << std::endl;

    std::vector<float>  drawVertices;
    for(unsigned int i = 0; i<m_verticesSmoothPolylines.size(); i++){
        drawVertices.push_back(m_verticesSmoothPolylines[i][0]);
        drawVertices.push_back(m_verticesSmoothPolylines[i][1]);
        drawVertices.push_back(m_verticesSmoothPolylines[i][2]);
    }
    cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_verticesBuffer_smooth_Catmull);
    cur_glFunctions->glBufferData(GL_ARRAY_BUFFER, drawVertices.size() * sizeof(float), &drawVertices[0], GL_STATIC_DRAW);
    cur_glFunctions->glVertexAttribPointer(m_verticesBufferPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    cur_glFunctions->glEnableVertexAttribArray(m_verticesBufferPos);
    checkOpenGLError();
    drawVertices.clear();
}

void MeshModel::initGLSL_imagePolyLinesVertices(QOpenGLExtraFunctions*  cur_glFunctions){
    if(DEBUGAPP) std::cout << "[Model] init GLSL vertices imagePolyLines" << std::endl;

    std::vector<float>  drawVertices;
    drawVertices.reserve(m_imagePolyLines.size()*3);
    for(unsigned int i = 0; i<m_imagePolyLines.size(); i++){
        drawVertices.push_back(m_imagePolyLines[i][0]);
        drawVertices.push_back(m_imagePolyLines[i][1]);
        drawVertices.push_back(m_imagePolyLines[i][2]);
    }
    cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_verticesBuffer_imagePolyLines);
    cur_glFunctions->glBufferData(GL_ARRAY_BUFFER, drawVertices.size() * sizeof(float), &drawVertices[0], GL_STATIC_DRAW);
    cur_glFunctions->glVertexAttribPointer(m_verticesBufferPos, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    cur_glFunctions->glEnableVertexAttribArray(m_verticesBufferPos);
    checkOpenGLError();
    drawVertices.clear();
}

void MeshModel::initGLSL_CatmullDimensions(QOpenGLExtraFunctions*  cur_glFunctions){
    if(DEBUGAPP) std::cout << "[Model] init GLSL dimensions CATMULL : " << m_verticesSmoothPolylinesDimensions.size() << std::endl;

    cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_dimensionsBuffer_smooth_Catmull);
    cur_glFunctions->glBufferData(GL_ARRAY_BUFFER, m_verticesSmoothPolylinesDimensions.size() * sizeof(int), &m_verticesSmoothPolylinesDimensions[0], GL_STATIC_DRAW);
    cur_glFunctions->glVertexAttribIPointer(m_dimensionsBufferPos, 1, GL_INT, 1.0 * sizeof(int), (void*)0);
    cur_glFunctions->glEnableVertexAttribArray(m_dimensionsBufferPos);
    checkOpenGLError();
}

void MeshModel::initGLSL_imagePolyLinesDimensions(QOpenGLExtraFunctions*  cur_glFunctions){
    if(DEBUGAPP) std::cout << "[Model] init GLSL dimensions imagePolyLines : " << m_imagePolyLines.size() << std::endl;

    std::vector<float>  dim;
    dim.reserve(m_imagePolyLines.size());
    for(unsigned int i = 0; i<m_imagePolyLines.size(); i++){
        dim.push_back(2);
    }
    cur_glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_dimensionsBuffer_imagePolyLines);
    cur_glFunctions->glBufferData(GL_ARRAY_BUFFER, dim.size() * sizeof(int), &dim[0], GL_STATIC_DRAW);
    cur_glFunctions->glVertexAttribIPointer(m_dimensionsBufferPos, 1, GL_INT, 1.0 * sizeof(int), (void*)0);
    cur_glFunctions->glEnableVertexAttribArray(m_dimensionsBufferPos);
    checkOpenGLError();
    dim.clear();
}

void MeshModel::initGLSL_Default(ShaderProgram& renderingProgram){
    if(DEBUGAPP){
        std::cout << "[Model] init GLSL Default" << std::endl;
    }

    renderingProgram.glFunctions->glBindVertexArray(m_VAO);

    initGLSL_vertices(renderingProgram.glFunctions);
    renderingProgram.glFunctions->glEnableVertexAttribArray(m_normalsBufferPos);
    initGLSL_dimensions(renderingProgram.glFunctions);

    renderingProgram.glFunctions->glBindVertexArray(0);
    renderingProgram.glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkOpenGLError();
}

void MeshModel::initGLSL_Catmull(ShaderProgram& renderingProgram){
    if(DEBUGAPP){
        std::cout << "[Model] init GLSL Catmull" << std::endl;
    }

    renderingProgram.glFunctions->glBindVertexArray(m_VAO_smooth_Catmull);
    initGLSL_CatmullVertices(renderingProgram.glFunctions);
    initGLSL_CatmullDimensions(renderingProgram.glFunctions);

    renderingProgram.glFunctions->glBindVertexArray(0);
    renderingProgram.glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkOpenGLError();
}

void MeshModel::initGLSL_imagePolyLines(ShaderProgram& renderingProgram){
    if(DEBUGAPP){
        std::cout << "[Model] init GLSL imagePolyLines" << std::endl;
    }

    renderingProgram.glFunctions->glBindVertexArray(m_VAO_imagePolyLines);
    initGLSL_imagePolyLinesVertices(renderingProgram.glFunctions);
    initGLSL_imagePolyLinesDimensions(renderingProgram.glFunctions);

    renderingProgram.glFunctions->glBindVertexArray(0);
    renderingProgram.glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
    checkOpenGLError();
}

void MeshModel::initGLSL(ShaderProgram& renderingProgram){
    if(DEBUGAPP){
        std::cout << "[Model] init GLSL" << std::endl;

        std::cout << "vertices l : " << m_vertices.size() << std::endl;
        std::cout << "triangles l : " << m_triangles.size() << std::endl;
        std::cout << "sortedTriangles l : " << m_sortedTriangles.size() << std::endl;

        std::cout << "catmull vert : " << m_verticesSmoothPolylines.size() << std::endl;
    }

    initGLSL_Default(renderingProgram);
    initGLSL_Catmull(renderingProgram);
    initGLSL_imagePolyLines(renderingProgram);

    m_glslInitialised = true;

    if(DEBUGAPP){
        std::cout << "[Model] init GLSL END" << std::endl;

        std::cout << "vertices buf : " << m_verticesBuffer << std::endl;
        std::cout << "index buf : " << m_indexBuffer << std::endl;
        std::cout << "normal buf : " << m_normalsBuffer << std::endl;
        std::cout << "dim buf : " << m_dimensionsBuffer << std::endl;
        std::cout << "vertices buf Catmull : " << m_verticesBuffer_smooth_Catmull << std::endl;
    }
}


// Initialise les tableaux et valeurs des attributs de la classe.
void MeshModel::CGALGeometry(C3t3 & m_c3t3){
    typedef CGAL::Mesh_triangulation_3<Mesh_domain>::type Triangulation;
    //typedef CGAL::Mesh_triangulation_3<Mesh_domain,CGAL::Default,Concurrency_tag>::type Triangulation;
    typedef Triangulation::Finite_facets_iterator Finite_facets_iterator;
    typedef Triangulation::Finite_vertices_iterator Finite_vertices_iterator;
    typedef Triangulation::Finite_cells_iterator Finite_cells_iterator;


    int indices[4][3] = {
        {1,2,3},
        {3,2,0},
        {3,0,1},
        {2,1,0}
    };

    const Triangulation & triangulation = m_c3t3.triangulation();

    std::map<Vertex_handle, int> VMap;
    int VCount = 0;

    // 1. Remplis la lite de sommet m_vertices et leurs dimensions m_verticesDimensions

    m_vertices.clear();
    m_verticesDimensions.clear();
    m_vertices.resize(triangulation.number_of_vertices());
    m_verticesDimensions.resize(triangulation.number_of_vertices());

    int surf_v_count = 0;
    for(Finite_vertices_iterator vit = triangulation.finite_vertices_begin() ; vit != triangulation.finite_vertices_end() ; vit++){
        m_vertices[VCount] = qglviewer::Vec(vit->point().x(), vit->point().y(), vit->point().z());
        int VDim = m_c3t3.in_dimension(vit);
        m_verticesDimensions[VCount] = VDim;
        VMap[vit] = VCount++;
        if(VDim==2)
            surf_v_count++;
    }

    // 2. Remplis la liste de triangles et leurs domaines

    m_triangles.clear();
    //m_triangles_subdomain_ids.clear();
    m_CGAL_envelop.clear();
    m_triangles.reserve(triangulation.number_of_finite_facets());
    //m_triangles_subdomain_ids.reserve(triangulation.number_of_finite_facets());

    for(Triangulation::Finite_facets_iterator fit = triangulation.finite_facets_begin() ; fit != triangulation.finite_facets_end() ; ++fit ){

        Facet mirror = triangulation.mirror_facet(*fit);

        if(m_c3t3.is_in_complex(fit->first, fit->second)){
            Subdomain_index si = 0;
            Subdomain_index mirror_si = 0;
            Cell_handle ch, mirror_ch;
            int tetVet = fit->second;

            if(m_c3t3.is_in_complex(fit->first) && !triangulation.is_infinite(fit->first)){
                ch = fit->first;
                si = m_c3t3.subdomain_index(ch);
            }

            if(m_c3t3.is_in_complex(mirror.first) && !triangulation.is_infinite(mirror.first)){
                mirror_ch = mirror.first;
                mirror_si = m_c3t3.subdomain_index(mirror_ch);
            }

            if (si == 0) {
                si = mirror_si;
                ch = mirror_ch;
                tetVet = mirror.second;
                mirror_si = 0;
            }
            if (si != 0) {
                if(triangulation.is_infinite(mirror.first) || !m_c3t3.is_in_complex(mirror.first) || triangulation.is_infinite(fit->first) || !m_c3t3.is_in_complex(fit->first))
                    m_CGAL_envelop.push_back(m_triangles.size());

                m_triangles.push_back(Surface_Triangle(VMap[ch->vertex(indices[tetVet][0])],
                                                       VMap[ch->vertex(indices[tetVet][1])],
                                                       VMap[ch->vertex(indices[tetVet][2])],
                                                       si, mirror_si));

                //m_triangles_subdomain_ids.push_back(si);
            }
        }
    }

    // 3. Remplis la liste des tétraèdre et leurs domaines. On profite de l'itération pour trier les sommets.

    std::map<Cell_handle, int> CMap;
    int count = 0;
    for(Triangulation::Finite_cells_iterator cit = triangulation.finite_cells_begin() ; cit != triangulation.finite_cells_end() ; ++cit ){
        CMap[cit] = count++;
    }

    m_sortedVertices.clear();
    m_tetrahedra.clear();
    m_tetrahedra_subdomain_ids.clear();
    m_tetrahedra.reserve(triangulation.number_of_finite_cells());
    m_tetrahedra_subdomain_ids.reserve(triangulation.number_of_finite_cells());

    std::map<Subdomain_index, std::set<unsigned int>> unique_sortedVertices;

    for(Triangulation::Finite_cells_iterator cit = triangulation.finite_cells_begin() ; cit != triangulation.finite_cells_end() ; ++cit ){
        Subdomain_index si = m_c3t3.subdomain_index(cit);
        Tetrahedron tet = Tetrahedron(VMap[cit->vertex(0)], VMap[cit->vertex(1)], VMap[cit->vertex(2)], VMap[cit->vertex(3)], si);
        for( int i = 0 ; i < 4 ; i ++ ) {
            if(!triangulation.is_infinite(cit->neighbor(i))) {
                tet.setNeighbor( i, CMap[cit->neighbor(i)] );
            }
            unique_sortedVertices[si].insert(VMap[cit->vertex(i)]);
        }
        m_tetrahedra.push_back(tet);
        m_tetrahedra_subdomain_ids.push_back(si);
    }

    for (std::map<Subdomain_index, std::set<unsigned int>>::iterator it = unique_sortedVertices.begin(); it != unique_sortedVertices.end(); ++it) {
        std::set<unsigned int> & vSet = it->second;
        for (std::set<unsigned int>::iterator sit = vSet.begin(); sit != vSet.end(); ++sit) {
            m_sortedVertices[it->first].push_back(*sit);
        }
    }

    // 4. Trie les triangles et les tétraèdres dans les sous domaines

    m_sortedTriangles.clear();
    m_sortedTetrahedra.clear();

    for(unsigned int t = 0 ; t < m_triangles.size() ; t++){
        Surface_Triangle tri = m_triangles[t];
        m_sortedTriangles[ tri.getLabel() ].push_back(t);
        Subdomain_index label2 = tri.getLabel2();
        if (label2!=0) {
            m_sortedTriangles[ label2 ].push_back(t);
        }
    }

    for(unsigned int t = 0 ; t < m_tetrahedra.size() ; t++){
        m_sortedTetrahedra[ m_tetrahedra_subdomain_ids[t] ].push_back(t);
    }

    //bool noZero = true;
    m_subdomain_indices.clear();
    for(std::map<int, std::vector<int> >::iterator it = m_sortedTriangles.begin(); it != m_sortedTriangles.end() ; it ++  ){
        //if( it->first == 0 ) noZero = false;
        m_subdomain_indices.push_back(it->first);
    }
    //if(noZero) subdomain_indices.push_back(0);
    std::sort(m_subdomain_indices.begin() , m_subdomain_indices.end());


    // 5. On trie les polylines dans leurs domaines

    //Compute polylines index for drawing
    m_sortedPolyLines.clear();

    for(unsigned int i = 0; i<m_polyLines.size(); i++){
        std::vector<C3t3::Edge> cur_egrp = m_polyLines[i];

        for(unsigned int j = 0; j< cur_egrp.size(); j++){
            std::vector<int> domains;
            Tr::Cell_circulator c = triangulation.incident_cells(cur_egrp[j]);
            Tr::Cell_circulator done=c;
            do{
                C3t3::Cell_handle cell=c;
                int n=(int)(cell->subdomain_index());
                if(std::find(domains.begin(), domains.end(), n) == domains.end()){
                    domains.push_back(n);
                }
                c++;
            }while(c != done);

            for(unsigned int k = 0; k<domains.size(); k++){
                m_sortedPolyLines[domains[k]].push_back(VMap[cur_egrp[j].first->vertex(cur_egrp[j].second)]);
                m_sortedPolyLines[domains[k]].push_back(VMap[cur_egrp[j].first->vertex(cur_egrp[j].third)]);
            }
        }
    }

    // 6. On crée une polyline interpollée grâce à Catmull-Rom

    // Creation de nouvelle polyline smooth avec plus de points
    m_verticesSmoothPolylines.clear();
    m_sortedSmoothPolylines.clear();
    m_verticesSmoothPolylinesDimensions.clear();

    std::vector<std::vector<bool>> reversed;
    for (unsigned int i = 0; i < m_polyLines.size(); ++i) {
        std::vector<bool> r_p;
        std::vector<C3t3::Edge> cur_egrp = m_polyLines[i];

        if(cur_egrp.size() > 1){
            C3t3::Edge e0 = cur_egrp[0];
            C3t3::Edge e1 = cur_egrp[1];

            if(e0.first->vertex(e0.second) == e1.first->vertex(e1.second) || e0.first->vertex(e0.second) == e1.first->vertex(e1.third)){
                r_p.push_back(true);
            }else{
                r_p.push_back(false);
            }
            for (int j = 1; j < cur_egrp.size(); ++j) {
                e0 = cur_egrp[j-1];
                e1 = cur_egrp[j];
                if (e1.first->vertex(e1.second) == e0.first->vertex(e0.second) || e1.first->vertex(e1.second) == e0.first->vertex(e0.third)) {
                    r_p.push_back(false);
                }
                else {
                    r_p.push_back(true);
                }
            }
        }else{
            r_p.push_back(false);
        }
        reversed.push_back(r_p);
    }

    qglviewer::Vec p0, p1, p2, p3;
    std::vector<qglviewer::Vec> tmpPoints;

    std::vector<int> domains;
    bool samePolyline;
    for(unsigned int i = 0; i<m_polyLines.size(); i++) {
        std::vector<C3t3::Edge> cur_egrp = m_polyLines[i];
        if (cur_egrp.size() == 0) continue;
        samePolyline = false;

        if (reversed[i][0]) {
            p1 = pointToVec(cur_egrp[0].first->vertex(cur_egrp[0].third)->point());
            p2 = pointToVec(cur_egrp[0].first->vertex(cur_egrp[0].second)->point());
        }
        else {
            p1 = pointToVec(cur_egrp[0].first->vertex(cur_egrp[0].second)->point());
            p2 = pointToVec(cur_egrp[0].first->vertex(cur_egrp[0].third)->point());
        }

        p0 = p1 + (p1-p2);
        for(unsigned int j = 0; j< cur_egrp.size(); j++) {
            C3t3::Edge cur_edge = cur_egrp[j];
            if (reversed[i][j]) {
                p1 = pointToVec(cur_edge.first->vertex(cur_edge.third)->point());
                p2 = pointToVec(cur_edge.first->vertex(cur_edge.second)->point());
            }
            else {
                p1 = pointToVec(cur_edge.first->vertex(cur_edge.second)->point());
                p2 = pointToVec(cur_edge.first->vertex(cur_edge.third)->point());
            }

            // 1. Catmull-Rom
            if (j+1 < cur_egrp.size()) {
                if (reversed[i][j+1]) {
                    p3 = pointToVec(cur_egrp[j+1].first->vertex(cur_egrp[j+1].second)->point());
                }
                else {
                    p3 = pointToVec(cur_egrp[j+1].first->vertex(cur_egrp[j+1].third)->point());
                }
            }
            else {
                p3 = p2 + (p2-p1);
            }

            tmpPoints.clear();
            Catmull_Rom(tmpPoints, p0, p1, p2, p3, m_smoothPolylineSubdivisionNumber);

            // 2. get domains
            domains.clear();
            Tr::Cell_circulator c = triangulation.incident_cells(cur_egrp[j]);
            Tr::Cell_circulator done=c;
            do{
              C3t3::Cell_handle cell=c;
              int n=(int)(cell->subdomain_index());
              if(std::find(domains.begin(), domains.end(), n) == domains.end()){
                domains.push_back(n);
              }
              c++;
            }while(c != done);

            // 3. Add points and domains
            int min_s = tmpPoints.size()/2;
            for (unsigned int k = 0; k < tmpPoints.size(); k++) {

                if(min_s > k){
                    if(reversed[i][j]){
                        m_verticesSmoothPolylinesDimensions.push_back(m_c3t3.in_dimension(cur_edge.first->vertex(cur_edge.third)));
                    }else{
                        m_verticesSmoothPolylinesDimensions.push_back(m_c3t3.in_dimension(cur_edge.first->vertex(cur_edge.second)));
                    }
                }else{
                    if(reversed[i][j]){
                        m_verticesSmoothPolylinesDimensions.push_back(m_c3t3.in_dimension(cur_edge.first->vertex(cur_edge.second)));
                    }else{
                        m_verticesSmoothPolylinesDimensions.push_back(m_c3t3.in_dimension(cur_edge.first->vertex(cur_edge.third)));
                    }
                }

                m_verticesSmoothPolylines.push_back(tmpPoints[k]);

                if (m_verticesSmoothPolylines.size() > 1 && samePolyline) {
                    for(unsigned int l = 0; l<domains.size(); l++){
                        m_sortedSmoothPolylines[domains[l]].push_back(m_verticesSmoothPolylines.size()-2);
                        m_sortedSmoothPolylines[domains[l]].push_back(m_verticesSmoothPolylines.size()-1);
                    }
                }
                if (!samePolyline) {
                    samePolyline = true;
                }
            }

            // 4. initialise next loop

            p0 = p1;

            // 5. Edge case
            if (j == cur_egrp.size()-1) {

                if(reversed[i][j]){
                    m_verticesSmoothPolylinesDimensions.push_back(m_c3t3.in_dimension(cur_edge.first->vertex(cur_edge.third)));
                }else{
                    m_verticesSmoothPolylinesDimensions.push_back(m_c3t3.in_dimension(cur_edge.first->vertex(cur_edge.second)));
                }

                m_verticesSmoothPolylines.push_back(p2);

                if (m_verticesSmoothPolylines.size() > 1 && samePolyline) {
                    for(unsigned int l = 0; l<domains.size(); l++){
                        m_sortedSmoothPolylines[domains[l]].push_back(m_verticesSmoothPolylines.size()-2);
                        m_sortedSmoothPolylines[domains[l]].push_back(m_verticesSmoothPolylines.size()-1);
                    }
                }
            }
        }
    }
}

// Calcule les polylines et initialise les tableaux et valeurs des attributs de la classe
void MeshModel::initMeshData(C3t3 & m_c3t3) {
    CGAL::Bbox_3 bbox = m_c3t3.bbox();
    m_center = qglviewer::Vec ((bbox.xmax() - bbox.xmin())/2., (bbox.ymax() - bbox.ymin())/2., (bbox.zmax() - bbox.zmin())/2.);
    m_radius = std::max(std::max(bbox.xmax() - bbox.xmin(), bbox.ymax() - bbox.ymin()), bbox.zmax() - bbox.zmin())*2.0;

    Tr & m_t = m_c3t3.triangulation();

    std::vector<C3t3::Edge> caracEdge;
    computeCaracteristicEdges(m_c3t3, caracEdge);

    getPolyline(m_polyLines, m_c3t3, caracEdge);
    getGroupPolyline(m_t, m_polyLines, m_groupPolyLines);

    CGALGeometry(m_c3t3);

    recomputeNormals();

    if(DEBUGAPP){
        std::cout << "[Model] init MESH DATA END" << std::endl;
        std::cout << "vertices size : " << m_vertices.size() << std::endl;
        std::cout << "triangles size : " << m_triangles.size() << std::endl;
        std::cout << "tetrahedra size : " << m_tetrahedra.size() << std::endl;
        std::cout << "polylines size : " << m_polyLines.size() << std::endl;
        std::cout << "group polylines size : " << m_groupPolyLines.size() << std::endl;
    }
}

void MeshModel::drawMeshTriangles(ShaderProgram& renderingProgram,std::map<Subdomain_index, bool> displayMap, std::map<Subdomain_index, QColor>& colorMap) {
    renderingProgram.glFunctions->glBindVertexArray(m_VAO);

    std::map<Subdomain_index, QColor>::const_iterator itCol;
    QColor color;

    std::vector<unsigned int> drawIndex;
    std::vector<float> drawNormals;

    for(std::map<Subdomain_index, std::vector<int> >::iterator it = m_sortedTriangles.begin(); it != m_sortedTriangles.end() ; it ++  ){
        Subdomain_index si = it->first;

        if(displayMap[si]){
            drawNormals.resize(m_vertices.size()*3, 0.0);
            itCol = colorMap.find(si);
            if( itCol == colorMap.end() )
                color.setHsvF(0.5, 1.,1.);
            else
                color = itCol->second;

            for(unsigned int tr = 0; tr < it->second.size(); tr++){
                Surface_Triangle t = m_triangles[it->second[tr]];
                qglviewer::Vec normal = m_trianglesNormals[it->second[tr]];
                if (si == t.getLabel2()) {
                    normal = -normal;
                }

                for(int i = 0; i<3;i++){
                    drawIndex.push_back(t[i]);

                    drawNormals[t[i]*3]   += normal[0];
                    drawNormals[t[i]*3+1] += normal[1];
                    drawNormals[t[i]*3+2] += normal[2];
                }

            }

            renderingProgram.glFunctions->glUniform3f(
                        renderingProgram.glFunctions->glGetUniformLocation(renderingProgram.programID, "u_color")
                        ,color.redF(),color.greenF(),color.blueF());

            renderingProgram.glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_normalsBuffer);

            renderingProgram.glFunctions->glBufferData(GL_ARRAY_BUFFER, drawNormals.size() * sizeof(float), &drawNormals[0], GL_STATIC_DRAW);

            renderingProgram.glFunctions->glVertexAttribPointer(m_normalsBufferPos, 3, GL_FLOAT, GL_TRUE,  3 * sizeof(float), (void*)0);

            renderingProgram.glFunctions->glDrawElements(GL_TRIANGLES, it->second.size() * 3, GL_UNSIGNED_INT, &drawIndex[0]);

            drawIndex.clear();
            drawNormals.clear();

            renderingProgram.glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
            checkOpenGLError();
        }
    }

    renderingProgram.glFunctions->glBindVertexArray(0);
    //renderingProgram.glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    renderingProgram.glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshModel::drawMeshTetra(ShaderProgram& renderingProgram,std::map<Subdomain_index, bool> displayMap, std::map<Subdomain_index, QColor>& colorMap) {
    renderingProgram.glFunctions->glBindVertexArray(m_VAO);

    std::map<Subdomain_index, QColor>::const_iterator itCol;
    QColor color;

    std::vector<unsigned int> drawIndex;
    std::vector<float> drawNormals;

    int indices[4][3] = {
        {1,2,3},
        {3,2,0},
        {3,0,1},
        {2,1,0}
    };

    for(std::map<Subdomain_index, std::vector<int> >::iterator it = m_sortedTetrahedra.begin(); it != m_sortedTetrahedra.end() ; it ++  ){
        Subdomain_index si = it->first;

        if(displayMap[si]){
            drawNormals.resize(m_vertices.size()*3, 0.0);
            itCol = colorMap.find(si);
            if( itCol == colorMap.end() )
                color.setHsvF(0.5, 1.,1.);
            else
                color = itCol->second;

            for (unsigned int tetr = 0; tetr < it->second.size(); tetr++) {
                int te = it->second[tetr];
                Tetrahedron t = m_tetrahedra[te];

                for(int i = 0; i<4;i++) {
                    qglviewer::Vec e01 = m_vertices[t[indices[i][1]]] - m_vertices[t[indices[i][0]]];
                    qglviewer::Vec e02 = m_vertices[t[indices[i][2]]] - m_vertices[t[indices[i][0]]];
                    qglviewer::Vec normal = e02 ^ e01;
                    normal.normalize();

                    for (int j = 0; j < 3; j++) {
                        drawIndex.push_back(t[indices[i][j]]);

                        drawNormals[t[i]*3]   += normal[0];
                        drawNormals[t[i]*3+1] += normal[1];
                        drawNormals[t[i]*3+2] += normal[2];
                    }
                }
            }


            renderingProgram.glFunctions->glUniform3f(
                        renderingProgram.glFunctions->glGetUniformLocation(renderingProgram.programID, "u_color")
                        ,color.redF(),color.greenF(),color.blueF());

            renderingProgram.glFunctions->glBindBuffer(GL_ARRAY_BUFFER, m_normalsBuffer);

            renderingProgram.glFunctions->glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*3 * sizeof(float), &drawNormals[0], GL_STATIC_DRAW);

            renderingProgram.glFunctions->glVertexAttribPointer(m_normalsBufferPos, 3, GL_FLOAT, GL_TRUE,  3 * sizeof(float), (void*)0);

            renderingProgram.glFunctions->glDrawElements(GL_TRIANGLES, drawIndex.size(), GL_UNSIGNED_INT, &drawIndex[0]);

            drawIndex.clear();
            drawNormals.clear();

            renderingProgram.glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
            checkOpenGLError();
        }
    }

    renderingProgram.glFunctions->glBindVertexArray(0);
    //renderingProgram.glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    renderingProgram.glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshModel::drawMesh(ShaderProgram&  renderingProgram,std::map<Subdomain_index, bool> displayMap, std::map<Subdomain_index, QColor>& colorMap, int meshDrawMode) {
    if (meshDrawMode == 0) {
        drawMeshTriangles(renderingProgram, displayMap, colorMap);
    }
    else if (meshDrawMode == 1) {
        drawMeshTetra(renderingProgram, displayMap, colorMap);
    }
}

void MeshModel::drawVerticies(ShaderProgram&  renderingProgram,std::map<Subdomain_index, bool> displayMap) {
    renderingProgram.glFunctions->glBindVertexArray(m_VAO);

    glPointSize(2.0f);

    for(std::map<Subdomain_index, std::vector<unsigned int>>::iterator it = m_sortedVertices.begin(); it != m_sortedVertices.end(); ++it) {
        Subdomain_index si = it->first;

        if(displayMap[si]){
            //renderingProgram.glFunctions->glUniform1i(renderingProgram.glFunctions->glGetUniformLocation(renderingProgram.programID, "u_dimension"), 1);
            renderingProgram.glFunctions->glDrawElements(GL_POINTS,it->second.size(),GL_UNSIGNED_INT, &it->second[0]);
        }
    }

    renderingProgram.glFunctions->glBindVertexArray(0);
    //renderingProgram.glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    renderingProgram.glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void MeshModel::drawPolylines(ShaderProgram&  renderingProgram,std::map<Subdomain_index, bool> displayMap, int polylineDrawMode) {
    if(polylineDrawMode == 0){
        renderingProgram.glFunctions->glBindVertexArray(m_VAO);
        glLineWidth(3.0f);

        for(std::map<Subdomain_index, std::vector<unsigned int> >::iterator it = m_sortedPolyLines.begin(); it != m_sortedPolyLines.end() ; it ++  ){
            Subdomain_index si = it->first;

            if(displayMap[si]){
                renderingProgram.glFunctions->glDrawElements(GL_LINES, it->second.size(),GL_UNSIGNED_INT, &(it->second[0]));
            }
        }
    }else{
        if(polylineDrawMode == 1){
            renderingProgram.glFunctions->glBindVertexArray(m_VAO_smooth_Catmull);
            glLineWidth(3.0f);

            for(std::map<Subdomain_index, std::vector<unsigned int> >::iterator it = m_sortedSmoothPolylines.begin(); it != m_sortedSmoothPolylines.end() ; it ++  ){
                Subdomain_index si = it->first;
                if(displayMap[si]){
                    renderingProgram.glFunctions->glDrawElements(GL_LINES, it->second.size(),GL_UNSIGNED_INT, &(it->second[0]));
                }
            }
        }
    }

    renderingProgram.glFunctions->glBindVertexArray(0);
    //renderingProgram.glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    renderingProgram.glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);

    renderingProgram.glFunctions->glBindVertexArray(m_VAO_imagePolyLines);
    glPointSize(2.0f);
    renderingProgram.glFunctions->glDrawArrays(GL_POINTS, 0, m_imagePolyLines.size());

    renderingProgram.glFunctions->glBindVertexArray(0);
    //renderingProgram.glFunctions->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    renderingProgram.glFunctions->glBindBuffer(GL_ARRAY_BUFFER, 0);
}




void MeshModel::recomputeNormals () {
    if(DEBUGAPP) std::cout << "[Model] recompute normals" << std::endl;

    computeFacetsNormals();
    computeVerticesNormals(true);
}

void MeshModel::computeFacetsNormals(){
    if(DEBUGAPP) std::cout << "[Model] recompute facet triangles" << std::endl;

    m_trianglesNormals.clear();
    m_trianglesNormals.reserve(m_triangles.size());
    for(unsigned int i = 0 ; i < m_triangles.size() ; i++){
        qglviewer::Vec normal;
        computeFacetNormal(i, normal);
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
    if(DEBUGAPP) std::cout << "[Model] compute vertices normal end" << std::endl;
}
