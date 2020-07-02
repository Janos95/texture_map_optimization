//
// Created by janos on 2/8/20.
//

#include "io.hpp"

#include <Corrade/Containers/Pointer.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Utility/Directory.h>


#include <MagnumPlugins/AnySceneImporter/AnySceneImporter.h>
#include <MagnumPlugins/StbImageImporter/StbImageImporter.h>

using namespace Magnum;
using namespace Corrade;
namespace Directory=Corrade::Utility::Directory;

Containers::Optional<Trade::MeshData> loadMeshData(const std::string& path){
    PluginManager::Manager<Trade::AbstractImporter> manager;
    auto importer = manager.loadAndInstantiate("AssimpImporter");
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

Containers::Array<Magnum::Trade::ImageData2D> loadImages(const std::string& dir){
    PluginManager::Manager<Trade::AbstractImporter> manager;
    auto importer = manager.loadAndInstantiate("JpegImporter");
    if(!importer){
        Debug{} << "Could not instantiate importer";
        std::exit(1);
    }
    auto files = Directory::list(dir, Directory::Flag::SkipDirectories|Directory::Flag::SkipSpecial|Directory::Flag::SortAscending);
    Containers::Array<Trade::ImageData2D> images;
    for(auto const& file : files){
        auto path = Directory::join(dir, file);
        if(!importer->openFile(path) || !importer->image2DCount() || !importer->image2D(0))
            continue;
        Containers::arrayAppend(images, *(importer->image2D(0)));
        importer->image2D(0);
    }
    Debug{} << "Imported " << images.size() << " images";
}
