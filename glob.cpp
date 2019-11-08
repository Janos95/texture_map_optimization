//
// Created by jmeny on 08.11.19.
//

#include "glob.h"

#include <regex>

std::vector<fs::path> glob(const fs::path& path, const std::string& regex)
{
    std::vector<fs::path> paths;
    std::copy(fs::directory_iterator(path), fs::directory_iterator(), std::back_inserter(paths));
    std::vector<fs::path> files;

    const std::regex my_filter(regex);

    for(const auto& path: paths)
    {
        if(!fs::is_regular_file(path)) continue;

        if(!std::regex_match(path.filename().string(), my_filter)) continue;

        files.push_back(path);
    }

    return files;
}