#include "CGAL/cgal_basicfunctions.h"

bool edgeInVector(C3t3::Edge &edge, std::vector<C3t3::Edge> &tab){
    for (unsigned int i = 0; i < tab.size(); i++)
    {
        if ((edge.first->vertex(edge.second) == tab[i].first->vertex(tab[i].second) && edge.first->vertex(edge.third) == tab[i].first->vertex(tab[i].third)) || (edge.first->vertex(edge.second) == tab[i].first->vertex(tab[i].third) && edge.first->vertex(edge.third) == tab[i].first->vertex(tab[i].second)))
            return true;
    }
    return false;
}

void computeCaracteristicEdges(C3t3 &c3t3, std::vector<C3t3::Edge> &caracEdge){
    caracEdge.clear();

    Tr &t = c3t3.triangulation();
    int c3t3CornerIndex = 0;
    for (Tr::All_vertices_iterator it = t.all_vertices_begin(); it != t.all_vertices_end(); ++it){
        int nb = 0;
        std::vector<Tr::Cell_handle> cells;
        t.incident_cells(it, std::back_inserter(cells));
        for (unsigned int i = 0; i < cells.size(); i++)
        {
            bool flag = true;
            for (unsigned int j = 0; j < i; j++)
            {
                if ((int)(cells[i]->subdomain_index()) == (int)(cells[j]->subdomain_index()))
                {
                    flag = false;
                    break;
                }
            }
            if (flag)
            {
                nb++;
            }
        }
        if (nb == 1)
        {
            c3t3.set_dimension(it, 3);
        }
        if (nb == 2)
        {
            c3t3.set_dimension(it, 2);
        }
        if (nb > 2)
        {
            c3t3.set_dimension(it, 1);
        }
    }

    for (Tr::All_vertices_iterator it = t.all_vertices_begin(); it != t.all_vertices_end(); ++it)
    {
        if (c3t3.in_dimension(it) == 1 || c3t3.in_dimension(it) == 0)
        {
            int nb_edges = 0;
            std::vector<C3t3::Edge> edges;
            t.incident_edges(it, std::back_inserter(edges));
            for (unsigned int i = 0; i < edges.size(); i++)
            {
                if ((c3t3.in_dimension(edges[i].first->vertex(edges[i].second)) == 1 || c3t3.in_dimension(edges[i].first->vertex(edges[i].second)) == 0) && (c3t3.in_dimension(edges[i].first->vertex(edges[i].third)) == 0 || c3t3.in_dimension(edges[i].first->vertex(edges[i].third)) == 1))
                {
                    if (edgeInVector(edges[i], caracEdge))
                    {
                        nb_edges++;
                    }
                    else
                    {
                        Tr::Cell_circulator c = t.incident_cells(edges[i]);
                        Tr::Cell_circulator done = c;
                        std::vector<int> domains;
                        do
                        {
                            C3t3::Cell_handle cell = c;
                            int n = (int)(cell->subdomain_index());
                            if (std::find(domains.begin(), domains.end(), n) == domains.end())
                            {
                                domains.push_back(n);
                            }
                            c++;
                        } while (c != done);
                        if (domains.size() > 2)
                        {
                            caracEdge.push_back(edges[i]);
                            c3t3.add_to_complex(edges[i], 1);
                            nb_edges++;
                        }
                    }
                }
            }
            if (nb_edges > 2)
            {
                c3t3.set_dimension(it, 0);
                c3t3.add_to_complex(it, c3t3CornerIndex++);
            }
        }
    }
}

// ==================== GESTION DES POLYLINES ====================
void parcoursPolyZero_rec(std::vector< C3t3::Edge >& polyLine, std::vector<bool>& used, const std::vector< C3t3::Edge > V, int lineNum, int point, C3t3& c3t3, int& curveNum) {
    used[lineNum] = true;
    polyLine.push_back(V[lineNum]);
    c3t3.remove_from_complex(V[lineNum]);
    c3t3.add_to_complex(V[lineNum], curveNum);
    C3t3::Vertex_handle vertex;
    if (point == 2) {
        if (c3t3.in_dimension(V[lineNum].first->vertex(V[lineNum].third)) == 0) {
            curveNum++;
            return;
        }
        vertex = V[lineNum].first->vertex(V[lineNum].third);
    }
    else {
        if (point == 3) {
            if (c3t3.in_dimension(V[lineNum].first->vertex(V[lineNum].second)) == 0) {
                curveNum++;
                return;
            }
            vertex = V[lineNum].first->vertex(V[lineNum].second);
        }
    }
    for (int i = 0; i < V.size(); i++) {
        if ((V[i].first->vertex(V[i].second)) == vertex && used[i] == false) {
            parcoursPolyZero_rec(polyLine, used, V, i, 2, c3t3, curveNum);
            return;
        }
        else {
            if ((V[i].first->vertex(V[i].third)) == vertex && used[i] == false) {
                parcoursPolyZero_rec(polyLine, used, V, i, 3, c3t3, curveNum);
                return;
            }
        }
    }
    curveNum++;
}

void parcoursPoly_rec(std::vector< C3t3::Edge >& polyLine, std::vector<bool>& used, const std::vector< C3t3::Edge > V, int lineNum, C3t3& c3t3, int curveNum) {
    used[lineNum] = true;
    polyLine.push_back(V[lineNum]);
    c3t3.remove_from_complex(V[lineNum]);
    c3t3.add_to_complex(V[lineNum], curveNum);
    for (unsigned int i = 0; i < V.size(); i++) {
        if (used[i] == false) {
            if (c3t3.in_dimension(V[lineNum].first->vertex(V[lineNum].second)) == 1) {
                if ((V[i].first->vertex(V[i].second)) == V[lineNum].first->vertex(V[lineNum].second) || (V[i].first->vertex(V[i].third)) == V[lineNum].first->vertex(V[lineNum].second)) {
                    parcoursPoly_rec(polyLine, used, V, i, c3t3, curveNum);
                }
            }
            if (c3t3.in_dimension(V[lineNum].first->vertex(V[lineNum].third)) == 1) {
                if ((V[i].first->vertex(V[i].second)) == V[lineNum].first->vertex(V[lineNum].third) || (V[i].first->vertex(V[i].third)) == V[lineNum].first->vertex(V[lineNum].third)) {
                    parcoursPoly_rec(polyLine, used, V, i, c3t3, curveNum);
                }
            }
        }
    }
}

void getPolyline(std::vector<std::vector<C3t3::Edge>>& polyLines, C3t3& c3t3, const std::vector<C3t3::Edge> caracEdge){
    polyLines.clear();

    int curveNum_c3t3=1;
    std::vector<bool> used;
    for (unsigned int i = 0; i < caracEdge.size(); i++){
        used.push_back(false);
    }

    for (unsigned int i = 0; i < caracEdge.size(); i++) {
        if ((int)(c3t3.in_dimension(caracEdge[i].first->vertex(caracEdge[i].second))) == 0 && used[i] == false) {
            std::vector<C3t3::Edge> tempPolyLine;
            parcoursPolyZero_rec(tempPolyLine, used, caracEdge, i, 2, c3t3, curveNum_c3t3);
            polyLines.push_back(tempPolyLine);
        }
        else {
            if ((int)(c3t3.in_dimension(caracEdge[i].first->vertex(caracEdge[i].third))) == 0 && used[i] == false) {
                std::vector<C3t3::Edge> tempPolyLine;
                parcoursPolyZero_rec(tempPolyLine, used, caracEdge, i, 3, c3t3, curveNum_c3t3);
                polyLines.push_back(tempPolyLine);
            }
        }
    }

    for (unsigned int i = 0; i < caracEdge.size(); i++){
        if(used[i]==false){
            std::vector<C3t3::Edge> tempPolyLine;
            parcoursPoly_rec(tempPolyLine, used, caracEdge, i, c3t3,curveNum_c3t3);
            polyLines.push_back(tempPolyLine);
            curveNum_c3t3++;
        }
    }
}

void getGroupPolyline(Tr& t,std::vector<std::vector<C3t3::Edge>>& polyLines,std::vector<std::vector<std::vector<C3t3::Edge>>>& res){
  res.clear();

  std::vector<bool> used;
  for(unsigned int i=0;i<polyLines.size();i++){
    used.push_back(false);
  }

  for(unsigned int i=0;i<polyLines.size();i++){

    if(!used[i]){
      std::vector<std::vector<C3t3::Edge>> temp;
      std::vector<int> domains;
      Tr::Cell_circulator c=t.incident_cells(polyLines[i][0]);
      Tr::Cell_circulator done=c;

      do{
        C3t3::Cell_handle cell=c;
        int n=(int)(cell->subdomain_index());
        if(std::find(domains.begin(), domains.end(), n) == domains.end()){
          domains.push_back(n);
        }
        c++;
      }while(c != done);

      for(unsigned int j=i;j<polyLines.size();j++){
        if(!used[j]){
          bool isSame=true;
          for(unsigned int k=0;k<domains.size();k++){
            c=t.incident_cells(polyLines[j][0]);
            bool b=true;
            done=c;
            do{
              C3t3::Cell_handle cell=c;
              if((int)(cell->subdomain_index())==domains[k]){
                b=false;
                break;
              }
              c++;
            }while(c != done);
            if(b){
              isSame=false;
              break;
            }
          }

          if(isSame){
            temp.push_back(polyLines[i]);
            used[j]=true;
          }
        }
      }

      res.push_back(temp);
    }
  }
}

inline bool isVoxelEdge(const CGAL::Image_3 & image, unsigned int x, unsigned int y, unsigned int z) {
    float tmp, v0, v1;
    int nb = 0;
    for (unsigned int cx = 0; cx < 2 && nb < 3; cx++) {
        for (unsigned int cy = 0; cy < 2 && nb < 3; cy++) {
            for (unsigned int cz = 0; cz < 2 && nb < 3; cz++) {
                tmp = image.value(x+cx, y+cy, z+cz);
                if (nb==0) {
                    v0 = tmp;
                    nb++;
                }
                else if (nb==1 && tmp!=v0) {
                    v1 = tmp;
                    nb++;
                }
                else if (tmp != v1 && tmp != v0) {
                    nb++;
                }
            }
        }
    }
    return nb == 3;
}

//std::vector<std::vector<std::pair<Point_3, Point_3>>> & polyLines
void getPolylineFrom3DImage(const CGAL::Image_3 & image, std::vector<Point_3> & polyLines) {
    polyLines.clear();

    unsigned int xcheck = image.xdim()-2;
    unsigned int ycheck = image.ydim()-2;
    unsigned int zcheck = image.zdim()-2;
    for (unsigned int x = 0; x < xcheck; x++) {
        for (unsigned int y = 0; y < ycheck; y++) {
            for (unsigned int z = 0; z < zcheck; z++) {
                if (isVoxelEdge(image, x, y, z)) {
                    polyLines.push_back(Point_3(x, y, z));
                }
            }
        }
    }
}

void testGetPolylineFrom3DImage(const char* filename, std::vector<Point_3> & polyLines) {
    CGAL::Image_3 image;
    image.read(filename);
    getPolylineFrom3DImage(image, polyLines);
}


void sortTriangleAndTetraBySubdomainIndex(const std::vector<Triangle> triangles, const std::vector<Subdomain_index> triangles_subdomain_ids ,const std::vector<Tetrahedron> tetrahedra,const std::vector<Subdomain_index> tetrahedra_subdomain_ids, std::map <Subdomain_index, std::vector<int> >& sortedTriangles, std::map <Subdomain_index, std::vector<int> >& sortedTetrahedra, std::vector<Subdomain_index>& subdomain_indices){
    sortedTriangles.clear();
    sortedTetrahedra.clear();

    for(unsigned int t = 0 ; t < triangles.size() ; t++){
        sortedTriangles[ triangles_subdomain_ids[t] ].push_back(t);
    }

    for(unsigned int t = 0 ; t < tetrahedra.size() ; t++){
        sortedTetrahedra[ tetrahedra_subdomain_ids[t] ].push_back(t);
    }

    bool noZero = true;
    subdomain_indices.clear();
    for(std::map<int, std::vector<int> >::iterator it = sortedTriangles.begin(); it != sortedTriangles.end() ; it ++  ){
        if( it->first == 0 ) noZero = false;
        subdomain_indices.push_back(it->first);
    }
    //if(noZero) subdomain_indices.push_back(0);
    std::sort(subdomain_indices.begin() , subdomain_indices.end());
}

// ==================== GESTION DES PRIMITIVES GEOMETRIQUES ====================

int tet_index[4][3] = {
   {3,1,2},
   {3,2,0},
   {3,0,1},
   {2,1,0}
};

void getVerticesAndMap(const C3t3 c3t3,const Tr t, std::vector<qglviewer::Vec>& vertices, std::vector<Subdomain_index> verticesDimensions, std::map<Vertex_handle, int>& VMap){
    VMap.clear();

    vertices.clear();
    verticesDimensions.clear();
    vertices.resize(t.number_of_vertices());
    verticesDimensions.resize(t.number_of_vertices());

    int VCount = 0;
    for(Finite_vertices_iterator vit = t.finite_vertices_begin() ; vit != t.finite_vertices_end() ; vit++){
        vertices[VCount]      = qglviewer::Vec(vit->point().x(),vit->point().y(),vit->point().z());
        verticesDimensions[VCount] = c3t3.in_dimension(vit);

        VMap[vit] = VCount++;
    }
}

void getTriangulationAndCgalEnvelop(const C3t3 c3t3,const Tr t, std::map<Vertex_handle, int> VMap, std::vector<Triangle>& triangles, std::vector<Subdomain_index>& triangles_subdomain_ids, std::vector<int>& CGAL_envelop, std::vector<Surface_index> &surface_indices){
    triangles.clear();
    triangles_subdomain_ids.clear();
    CGAL_envelop.clear();
    surface_indices.clear();

    for(Tr::Finite_facets_iterator fit = t.finite_facets_begin() ; fit != t.finite_facets_end() ; ++fit ){
        Facet mirror = t.mirror_facet(*fit);

        if(c3t3.is_in_complex(fit->first, fit->second)){
            if(c3t3.is_in_complex(fit->first) && !t.is_infinite(fit->first)){
                Cell_handle ch = fit->first;
                Subdomain_index si = c3t3.subdomain_index(ch);

                surface_indices.push_back(c3t3.surface_index(*fit));

                if(t.is_infinite(mirror.first) || !c3t3.is_in_complex(mirror.first))
                    CGAL_envelop.push_back(triangles.size());

                int t_0 = tet_index[fit->second][0];
                int t_1 = tet_index[fit->second][1];
                int t_2 = tet_index[fit->second][2];

                Vertex_handle vh0 = ch->vertex(t_0);
                Vertex_handle vh1 = ch->vertex(t_1);
                Vertex_handle vh2 = ch->vertex(t_2);

                int v0 = VMap[vh0];
                int v1 = VMap[vh1];
                int v2 = VMap[vh2];

                Triangle cur_t(v0,v1,v2);

                triangles.push_back(cur_t);
                triangles_subdomain_ids.push_back(si);
            }


            if(c3t3.is_in_complex(mirror.first) && !t.is_infinite(mirror.first)){
                Cell_handle ch = mirror.first;
                Subdomain_index si = c3t3.subdomain_index(ch);

                if(t.is_infinite(fit->first) || !c3t3.is_in_complex(fit->first))
                    CGAL_envelop.push_back(triangles.size());
                Triangle cur_t(VMap[ch->vertex(tet_index[mirror.second][0])],
                        VMap[ch->vertex(tet_index[mirror.second][1])] ,
                        VMap[ch->vertex(tet_index[mirror.second][2])]);
                triangles.push_back(cur_t);
                triangles_subdomain_ids.push_back(si);
            }
        }

    }
}

void getTetrahedronAndMap(const C3t3 c3t3,const Tr t, std::map<Vertex_handle, int>& VMap,std::vector<Tetrahedron>& tetrahedra,std::vector<Subdomain_index>& tetrahedra_subdomain_ids, std::map<Cell_handle, int> CMap){
    CMap.clear();
    tetrahedra.clear();
    tetrahedra_subdomain_ids.clear();


    int count = 0;
    for(Tr::Finite_cells_iterator cit = t.finite_cells_begin() ; cit != t.finite_cells_end() ; ++cit ){
        CMap[cit] = count++;
    }

    for(Tr::Finite_cells_iterator cit = t.finite_cells_begin() ; cit != t.finite_cells_end() ; ++cit ){
        Subdomain_index si = c3t3.subdomain_index(cit);
        Tetrahedron tet = Tetrahedron(VMap[cit->vertex(0)], VMap[cit->vertex(1)], VMap[cit->vertex(2)], VMap[cit->vertex(3)], si);
        for( int i = 0 ; i < 4 ; i ++ ){
            if(!t.is_infinite(cit->neighbor(i))){
                tet.setNeighbor( i, CMap[cit->neighbor(i)] );
            }
        }
        tetrahedra.push_back(tet);
        tetrahedra_subdomain_ids.push_back(si);
    }
}

void getC3t3FromMeshFile(const char* filename, C3t3 & m_c3t3) {
    std::ifstream c3t3_load(filename);
    c3t3_load >> m_c3t3;
    c3t3_load.close();
}

void getC3t3FromInrFile(const char* filename, C3t3 & m_c3t3,
                        double edgeSize,
                        double facetAngle,
                        double facetSize,
                        double facetApproximation,
                        CGAL::Mesh_facet_topology facetTopology,
                        double cellRatio,
                        double cellSize,
                        bool perturb,
                        bool exude,
                        float smoothingSigma) {
    if (DEBUGAPP) {
        std::cout << "MESHING : " << filename << std::endl;
        std::cout << "edgeSize = " << edgeSize << std::endl;
        std::cout << "facetAngle = " << facetAngle << std::endl;
        std::cout << "facetSize = " << facetSize << std::endl;
        std::cout << "facetApproximation = " << facetApproximation << std::endl;
        std::cout << "facetTopology = " << facetTopology << std::endl;
        std::cout << "cellRatio = " << cellRatio << std::endl;
        std::cout << "cellSize = " << cellSize << std::endl;
        std::cout << "perturb = " << perturb << std::endl;
        std::cout << "exude = " << exude << std::endl;
        std::cout << "smoothingSigma = " << smoothingSigma << std::endl;
    }
    namespace p = CGAL::parameters;

    CGAL::Image_3 image;
    image.read(filename);

//    std::cout << "Image dimension (number of columns, rows, planes) : " << image.xdim() << " " << image.ydim() << " " << image.zdim() << std::endl;
//    std::cout << "Image voxel size : " << image.vx() << " " << image.vy() << " " << image.vz() << std::endl;
//    std::cout << "Image offset : " << image.tx() << " " << image.ty() << " " << image.tz() << std::endl;

    // Domain
    //CGAL::Labeled_mesh_domain_3<K> domain = CGAL::Labeled_mesh_domain_3<K>::create_labeled_image_mesh_domain(image);
    Mesh_domain domain = Mesh_domain::create_labeled_image_mesh_domain(image);
    std::vector<std::tuple<Mesh_domain::Curve_index,std::pair<Mesh_domain::Point_3,Mesh_domain::Index>,std::pair<Mesh_domain::Point_3,Mesh_domain::Index> >> curves;

    // Mesh criteria
    Mesh_criteria::Edge_criteria edge_criteria(edgeSize);
    Mesh_criteria::Facet_criteria facet_criteria(facetAngle, facetSize, facetApproximation, facetTopology);
    Mesh_criteria::Cell_criteria cell_criteria(cellRatio, cellSize);
    Mesh_criteria criteria(edge_criteria, facet_criteria, cell_criteria);

    // Meshing
    p::internal::Exude_options exudeOption = exude ? p::exude() : p::no_exude();
    p::internal::Perturb_options perturbOption = perturb ? p::perturb() : p::no_perturb();
    m_c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria, exudeOption, perturbOption, p::features(domain));
//    Mesh_domain::Subdomain sd = domain.is_in_domain_object()(Mesh_domain::Point_3(50, 50, 50));
//    std::cout << "sd = " << sd.value_or(0) << std::endl;
//    Mesh_domain::Index id = domain.index_from_subdomain_index(sd.value_or(0));
//    std::cout << "id.sd = " << domain.subdomain_index(id) << std::endl;
//    sd = domain.is_in_domain_object()(Mesh_domain::Point_3(0, 0, 0));
//    std::cout << "sd = " << sd.value_or(0) << std::endl;
//    id = domain.index_from_subdomain_index(sd.value_or(0));
//    std::cout << "id.sd = " << domain.subdomain_index(id) << std::endl;
//    sd = domain.is_in_domain_object()(Mesh_domain::Point_3(-50, -50, -50));
//    std::cout << "sd = " << sd.value_or(0) << std::endl;
//    id = domain.index_from_subdomain_index(sd.value_or(0));
//    std::cout << "id.sd = " << domain.subdomain_index(id) << std::endl;
}

CGAL::Labeled_mesh_domain_3<K> getDomain(CGAL::Image_3 image, float smoothingSigma) {
    if (smoothingSigma > 0) {
        std::cerr << "NOT YET IMPLEMENTED" << std::endl;
        return CGAL::Labeled_mesh_domain_3<K>::create_labeled_image_mesh_domain(image);
    }
    else {
        return CGAL::Labeled_mesh_domain_3<K>::create_labeled_image_mesh_domain(image);
    }
}

void getC3t3FromFile(QString filename, C3t3 & m_c3t3,
                     double edgeSize,
                     double facetAngle,
                     double facetSize,
                     double facetApproximation,
                     CGAL::Mesh_facet_topology facetTopology,
                     double cellRatio,
                     double cellSize,
                     bool perturb,
                     bool exude,
                     float smoothingSigma)
{
    if(filename.endsWith(".mesh")){
        getC3t3FromMeshFile(filename.toStdString().c_str(), m_c3t3);
    }else if (filename.endsWith(".inr")) {
        getC3t3FromInrFile(filename.toStdString().c_str(), m_c3t3,
            edgeSize, facetAngle, facetSize, facetApproximation, facetTopology, cellRatio, cellSize, perturb, exude, smoothingSigma);
    }
}

void getC3t3FromInrFileWithFeatures(const char* filename, C3t3 & m_c3t3,
                                    double edgeSize,
                                    double facetAngle,
                                    double facetSize,
                                    double facetApproximation,
                                    double cellRatio,
                                    double cellSize,
                                    bool perturb,
                                    bool exude)
{
    if (DEBUGAPP) {
        std::cout << "MESHING WITH FEATURES : " << filename << std::endl;
        std::cout << "facetAngle = " << facetAngle << std::endl;
        std::cout << "facetSize = " << facetSize << std::endl;
        std::cout << "facetApproximation = " << facetApproximation << std::endl;
        std::cout << "cellRatio = " << cellRatio << std::endl;
        std::cout << "cellSize = " << cellSize << std::endl;
        std::cout << "perturb = " << perturb << std::endl;
        std::cout << "exude = " << exude << std::endl;
    }

    CGAL::Image_3 image;
    image.read(filename);

    // Domain
    //CGAL::Labeled_mesh_domain_3<K> domain = CGAL::Labeled_mesh_domain_3<K>::create_labeled_image_mesh_domain(image);
    Mesh_domain domain = Mesh_domain::create_labeled_image_mesh_domain(image);

    const std::string lines_fname = CGAL::data_file_path("images/420.polylines.txt");

    if(!add_1D_features(image, domain, lines_fname)) {
        std::cerr << "Error adding polylines features" << std::endl;
        return;
    }

    // Mesh criteria
    Mesh_criteria::Edge_criteria edge_criteria(edgeSize);
    Mesh_criteria::Facet_criteria facet_criteria(facetAngle, facetSize, facetApproximation);
    Mesh_criteria::Cell_criteria cell_criteria(cellRatio, cellSize);
    Mesh_criteria criteria(edge_criteria, facet_criteria, cell_criteria);

    // Meshing
    CGAL::parameters::internal::Exude_options exudeOption = exude ? CGAL::parameters::exude() : CGAL::parameters::no_exude();
    CGAL::parameters::internal::Perturb_options perturbOption = perturb ? CGAL::parameters::perturb() : CGAL::parameters::no_perturb();
    m_c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria, exudeOption, perturbOption);
}

template <typename Point_3>
bool read_polylines(const std::string fname,
                    std::vector<std::vector<Point_3>> & polylines)
{
    std::ifstream ifs(fname);
    if(ifs.bad()) return false;
    std::size_t n;
    while(ifs >> n) {
        polylines.resize(polylines.size()+1);
        std::vector<Point_3>& polyline = polylines.back();
        while(n-- != 0) {
            Point_3 p;
            ifs >> p;
            if(ifs.fail()) return false;
            polyline.push_back(p);
        }
    }
    if(ifs.bad()) return false;
    else return ifs.eof();
}


// Protect the intersection of the object with the box of the image,
// by declaring 1D-features. Note that `CGAL::polylines_to_protect` is
// not documented.
bool add_1D_features(const CGAL::Image_3& image,
                     Mesh_domain& domain,
                     const std::string lines_fname)
{
    typedef K::Point_3 Point_3;
    typedef unsigned char Word_type;

    std::vector<std::vector<Point_3> > features_inside;
    if(!read_polylines(lines_fname, features_inside)) // see file "read_polylines.h"
    {
        std::cerr << "Error: Cannot read file " <<  lines_fname << std::endl;
        return false;
    }

    std::vector<std::vector<Point_3> > polylines_on_bbox;
    CGAL::polylines_to_protect<Point_3, Word_type>(image, polylines_on_bbox,
                                                   features_inside.begin(),
                                                   features_inside.end());

    domain.add_features(polylines_on_bbox.begin(), polylines_on_bbox.end());

    // It is very important that the polylines from the file `lines_fname`
    // contain only polylines in the inside of the box of the image.
    domain.add_features(features_inside.begin(), features_inside.end());
    return true;
}

//void getC3t3FromInrFileWithRemeshing(const char* filename, C3t3 & m_c3t3) {
//    CGAL::Image_3 image;
//    image.read(filename);

//    CGAL::Labeled_mesh_domain_3<K> domain = CGAL::Labeled_mesh_domain_3<K>::create_labeled_image_mesh_domain(image);        // Domain

//    // Mesh criteria
//    //Mesh_criteria::Edge_criteria edge_criteria(6);
//    Mesh_criteria::Facet_criteria facet_criteria(FACET_ANGLE, FACET_SIZE, FACET_APPROXIMATION); // angle, size, approximation
//    Mesh_criteria::Cell_criteria cell_criteria(CELL_RATIO, CELL_SIZE);        // radius-edge ratio, size
//    Mesh_criteria criteria(facet_criteria, cell_criteria);

//    // Meshing
//    m_c3t3 = CGAL::make_mesh_3<C3t3>(domain, criteria, CGAL::parameters::no_exude(), CGAL::parameters::no_perturb());

////    std::cout << "ODT" << std::endl;
//    CGAL::odt_optimize_mesh_3(m_c3t3,
//                        domain,
//                        CGAL::parameters::max_iteration_number = 100,
//                        CGAL::parameters::convergence = 0,
//                        CGAL::parameters::time_limit=30);

////    std::cout << "lloyd" << std::endl;
////    CGAL::lloyd_optimize_mesh_3(m_c3t3,
////                        domain,
////                        CGAL::parameters::convergence=0.01,
////                        CGAL::parameters::time_limit=10);

////    std::cout << "perturb" << std::endl;
////    CGAL::perturb_mesh_3(m_c3t3,
////                        domain,
////                        CGAL::parameters::sliver_bound = 10,
////                        CGAL::parameters::time_limit=10);

////    std::cout << "exude" << std::endl;
////    CGAL::exude_mesh_3(m_c3t3,
////                        CGAL::parameters::time_limit=10);


////    CGAL::Labeled_mesh_domain_3<K> domainNew = CGAL::Labeled_mesh_domain_3<K>::create_labeled_image_mesh_domain(image);
////    Mesh_criteria::Facet_criteria facet_criteria2(30, 10, 1); // angle, size, approximation
////    Mesh_criteria::Cell_criteria cell_criteria2(0, 0);        // radius-edge ratio, size
////    Mesh_criteria criteria2(facet_criteria2, cell_criteria2);


////    CGAL::refine_mesh_3(m_c3t3, domain, criteria,
////            CGAL::parameters::lloyd(),
////            CGAL::parameters::odt(),
////            CGAL::parameters::perturb(),
////            CGAL::parameters::exude(),
////            CGAL::parameters::manifold()
////        );

//    //CGAL::refine_mesh_3(m_c3t3, domain, criteria);
////    CGAL::refine_mesh_3(m_c3t3, domain, criteria,
////                CGAL::parameters::exude(),
////                CGAL::parameters::perturb(),
////                CGAL::parameters::odt(),
////                CGAL::parameters::lloyd(),
////                CGAL::parameters::manifold()
////            );

////    Mesh_criteria new_criteria(CGAL::parameters::cell_radius_edge_ratio=3, CGAL::parameters::cell_size=0.03);
////    using namespace CGAL::parameters;
////    CGAL::refine_mesh_3(m_c3t3, domain, new_criteria, manifold());
//}
