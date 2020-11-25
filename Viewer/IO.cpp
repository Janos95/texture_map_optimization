//
// Created by janos on 2/8/20.
//

#include "IO.h"
#include "Types.h"
#include "RemoveIf.h"

#include <Corrade/Containers/Pointer.h>
#include <Corrade/Containers/Optional.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/StringStl.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Utility/Directory.h>

#include <MagnumPlugins/AnySceneImporter/AnySceneImporter.h>
#include <MagnumPlugins/AnyImageImporter/AnyImageImporter.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Trade/ImageData.h>
#include <Magnum/Trade/MeshData.h>

#include <fstream>
//#include <atomic>

namespace Directory = Corrade::Utility::Directory;

namespace TextureMapOptimization {

Optional<Mg::Trade::MeshData> loadMeshData(const StringView& path) {
    Mg::PluginManager::Manager<Mg::Trade::AbstractImporter> manager;
    auto importer = manager.loadAndInstantiate("AssimpImporter");
    if(!importer) std::exit(1);

    Debug{} << "Opening file" << path;

    if(!importer->openFile(path))
        std::exit(4);

    Debug{} << "Imported " << importer->meshCount() << " meshes";

    return importer->mesh(0);
}

#define PBSTR "||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||"
#define PBWIDTH 60

void printProgress(double percentage) {
    int val = (int) (percentage * 100);
    int lpad = (int) (percentage * PBWIDTH);
    int rpad = PBWIDTH - lpad;
    printf("\r%3d%% [%.*s%*s]", val, lpad, PBSTR, rpad, "");
    fflush(stdout);
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
Array<Mg::Trade::ImageData2D> loadImages(const StringView& dir) {
    auto files = Directory::list(dir, Directory::Flag::SkipDirectories |
                                      Directory::Flag::SkipSpecial |
                                      Directory::Flag::SortAscending);

    size_t counter = 0;
    Array<Optional<Mg::Trade::ImageData2D>> optionalImages{files.size()};

    Mg::PluginManager::Manager<Mg::Trade::AbstractImporter> manager;

//#pragma omp parallel for
    for(size_t i = 0; i < optionalImages.size(); ++i) {
        auto path = Directory::join(dir, files[i]);
        auto imageImporter = manager.loadAndInstantiate("AnyImageImporter");
        if(!imageImporter->openFile(path) || !imageImporter->image2DCount() ||
           !imageImporter->image2D(0))
            continue;

        optionalImages[i] = imageImporter->image2D(0);
//#pragma omp critical
        double progress = double(counter++)/files.size();
        printProgress(progress);
    }

    Array<Mg::Trade::ImageData2D> images;
    for(auto& optionalImage : optionalImages) {
        if(optionalImage)
            arrayAppend(images, std::move(*optionalImage));
    }

    Debug{} << "\nImported " << images.size() << " images";
    return images;
}
#pragma clang diagnostic pop

Array<Matrix4> loadPoses(const StringView& path) {
    std::ifstream file(path);
    Array<Matrix4> tfs;
    int dummy;
    while(file >> dummy >> dummy >> dummy){
        Matrix4 tf;
        file >> tf[0][0] >> tf[1][0] >> tf[2][0] >> tf[3][0] >>
             tf[0][1] >> tf[1][1] >> tf[2][1] >> tf[3][1] >>
             tf[0][2] >> tf[1][2] >> tf[2][2] >> tf[3][2] >>
             tf[0][3] >> tf[1][3] >> tf[2][3] >> tf[3][3];
        arrayAppend(tfs, tf);
    }

    return tfs;
}

}