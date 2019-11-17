//
// Created by janos on 11/15/19.
//

#pragma once

#include <Magnum/Math/Matrix4.h>
#include <Corrade/Utility/Debug.h>
#include <scn/scn.h>

#include <vector>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void handle_error(char const * error){}

auto loadPosesSimple(char const *fname)
{
    auto fd = std::fopen(fname,"r");
    auto file = scn::file(fd);

    Magnum::Matrix4d tf;
    std::vector<Magnum::Matrix4d> tfs;
    int a1, a2, a3;
    while(scn::scan(file, "{} {} {}\n {} {} {} {}\n {} {} {} {}\n {} {} {} {}\n {} {} {} {}\n",
                    a1, a2, a3,
                    tf[0][0], tf[1][0], tf[2][0], tf[3][0],
                    tf[0][1], tf[1][1], tf[2][1], tf[3][1],
                    tf[0][2], tf[1][2], tf[2][2], tf[3][2],
                    tf[0][3], tf[1][3], tf[2][3], tf[3][3]))
    {
        tfs.push_back(tf);
        Utility::Debug{} << tf;
    }

    std::fclose(fd);
    return tfs;
}

auto loadPoses(char const *fname)
{
    auto fd = open(fname, O_RDONLY);

    struct stat status;
    fstat(fd, &status);

    auto size = status.st_size;
    auto data = std::make_unique<char[]>(size);

    posix_fadvise(fd, 0, 0, 1); //sequential advice

    auto readStatus = read(fd, data.get(), size);
    close(fd);
    uintmax_t lines = 0;

    auto view = std::string_view(data.get(), size);

    Magnum::Matrix4d tf;
    std::vector<Magnum::Matrix4d> tfs;
    int a1, a2, a3;
    while(scn::scan(view, scn::default_tag,
            a1, a2, a3,
            tf[0][0], tf[1][0], tf[2][0], tf[3][0],
            tf[0][1], tf[1][1], tf[2][1], tf[3][1],
            tf[0][2], tf[1][2], tf[2][2], tf[3][2],
            tf[0][3], tf[1][3], tf[2][3], tf[3][3]))
    {
        tfs.push_back(tf);
    }

    return tfs;
}