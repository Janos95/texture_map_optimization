//
// Created by janos on 2/8/20.
//

#include "load_mesh_data.hpp"

#include <Corrade/Containers/Pointer.h>
#include <Corrade/Containers/Optional.h>

#include <MagnumPlugins/AssimpImporter/AssimpImporter.h>

using namespace Magnum;
using namespace Corrade;

Containers::Optional<Trade::MeshData3D> loadMeshData(const std::string& path){
    PluginManager::Manager<Trade::AbstractImporter> manager;
    Containers::Pointer<Trade::AbstractImporter> importer = manager.loadAndInstantiate("AssimpImporter");
    if(!importer) std::exit(1);

    Debug{} << "Opening file" << path.c_str();

    if(!importer->openFile(path))
        std::exit(4);

    Debug{} << "Imported " << importer->mesh3DCount() << " meshes";

    return importer->mesh3D(0);
}

