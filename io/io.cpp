//
// Created by janos on 2/8/20.
//

#include "io.h"

#include <Corrade/Containers/Pointer.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Utility/Directory.h>


#include <MagnumPlugins/AnySceneImporter/AnySceneImporter.h>
#include <MagnumPlugins/StbImageImporter/StbImageImporter.h>

#include <fstream>

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

        auto data = importer->image2D(0);
        Containers::arrayAppend(images, std::move(*data));
        importer->image2D(0);
    }
    Debug{} << "Imported " << images.size() << " images";
    return images;
}

Containers::Array<Matrix4> loadPoses(std::string const& path)
{
    std::ifstream file(path);
    Containers::Array<Matrix4> tfs;
    int dummy;
    while(file >> dummy >> dummy >> dummy)  {
        Matrix4 tf;
        file >> tf[0][0] >> tf[1][0] >> tf[2][0] >> tf[3][0] >>
             tf[0][1] >> tf[1][1] >> tf[2][1] >> tf[3][1] >>
             tf[0][2] >> tf[1][2] >> tf[2][2] >> tf[3][2] >>
             tf[0][3] >> tf[1][3] >> tf[2][3] >> tf[3][3];
        Containers::arrayAppend(tfs, tf);
    }

    return tfs;
}
