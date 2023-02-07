#include "cleaver_basicfunctions.h"

void cleaver_test() {
    bool verbose = true;
    std::vector<std::string> material_fields;
    material_fields.push_back("C:/Users/aclement/Desktop/dev/Cleaver2/src/test/test_data/input/spheres1.nrrd");
    material_fields.push_back("C:/Users/aclement/Desktop/dev/Cleaver2/src/test/test_data/input/spheres2.nrrd");
    material_fields.push_back("C:/Users/aclement/Desktop/dev/Cleaver2/src/test/test_data/input/spheres3.nrrd");
    material_fields.push_back("C:/Users/aclement/Desktop/dev/Cleaver2/src/test/test_data/input/spheres4.nrrd");
    std::vector<cleaver::AbstractScalarField*> fields;
    fields = NRRDTools::segmentationToIndicatorFunctions(material_fields[0], 1.0);

    cleaver::Volume *volume = new cleaver::Volume(fields);
    cleaver::CleaverMesher mesher(volume);
    cleaver::AbstractScalarField *sizingField =
    cleaver::SizingFieldCreator::createSizingFieldFromVolume(
         volume,
         (float)(1.0/0.2),  //lipschitz
         (float)1.0,        //sampling_rate
         (float)1.0,        //feature_scaling
         (int)1,            //padding
         verbose);          //verbose
    volume->setSizingField(sizingField);

    cleaver::TetMesh *bgMesh = nullptr;
    bgMesh = mesher.createBackgroundMesh(verbose);
    mesher.buildAdjacency(verbose);
    mesher.sampleVolume(verbose);
    mesher.computeAlphas(verbose);
    mesher.computeInterfaces(verbose);
    mesher.generalizeTets(verbose);
    mesher.snapsAndWarp(verbose);
    mesher.stencilTets(verbose);
    cleaver::TetMesh *mesh = mesher.getTetMesh();
    std::cout << "TEST CLEAVER : " << std::endl;
    std::cout << "mesh : " << std::endl;
    std::cout << "    vertices size : " << mesh->verts.size() << std::endl;
    std::cout << "    faces size : " << mesh->faces.size() << std::endl;
    std::cout << "    tets size : " << mesh->tets.size() << std::endl;
    std::cout << "bgMesh : " << std::endl;
    std::cout << "    vertices size : " << bgMesh->verts.size() << std::endl;
    std::cout << "    faces size : " << bgMesh->faces.size() << std::endl;
    std::cout << "    tets size : " << bgMesh->tets.size() << std::endl;
}
