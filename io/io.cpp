//
// Created by janos on 2/8/20.
//

#include "io.hpp"

#include <Corrade/Containers/Pointer.h>
#include <Corrade/Containers/Optional.h>


#include <MagnumPlugins/AnySceneImporter/AnySceneImporter.h>
#include <MagnumPlugins/StbImageImporter/StbImageImporter.h>

using namespace Magnum;
using namespace Corrade;

Containers::Optional<Trade::MeshData> loadMeshData(const std::string& path){
    PluginManager::Manager<Trade::AbstractImporter> manager;
    auto importer = manager.loadAndInstantiate("AnySceneImporter");
    if(!importer) std::exit(1);

    Debug{} << "Opening file" << path.c_str();

    if(!importer->openFile(path))
        std::exit(4);

    Debug{} << "Imported " << importer->meshCount() << " meshes";

    return importer->mesh(0);
}


Containers::Optional<Magnum::Trade::ImageData2D> loadHdrImage(const std::string& path){
    PluginManager::Manager<Trade::AbstractImporter> manager;
    auto importer = manager.loadAndInstantiate("HdrImporter");
    if(!importer){
        Debug{} << "Could not instantiate importer";
        std::exit(1);
    }
    if(!importer->openFile(path))
        std::exit(4);

    Debug{} << "Imported " << importer->image2DCount() << " images";

    return importer->image2D(0);
}

