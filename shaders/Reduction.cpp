//
// Created by janos on 7/21/20.
//

#include "Reduction.h"

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>
#include <Magnum/GL/ImageFormat.h>

#include <Corrade/Containers/Array.h>
#include <Corrade/Utility/FormatStl.h>
#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>

namespace shaders {

using namespace Magnum;
using namespace Corrade;

Reduction::Reduction(UnsignedInt stepSize) {
    MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL460);

    Utility::Resource rs{"tmo-data"};

    GL::Shader comp{GL::Version::GL460, GL::Shader::Type::Compute};

    comp.addSource(Utility::formatString("#define BLOCK_SIZE {}\n", 1u << stepSize))
        .addSource(rs.get("Reduction.comp"));

    CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({comp}));

    attachShaders({comp});

    CORRADE_INTERNAL_ASSERT_OUTPUT(link());
}


Reduction& Reduction::bindInput(GL::Texture2D& texture, int level) {
    texture.bindImage(m_inputUnit, level, GL::ImageAccess::ReadOnly,
                      GL::ImageFormat::R32F);
    return *this;
}

Reduction&
Reduction::bindMipMapTexture(Magnum::GL::Texture2D& texture, int level) {
    texture.bindImage(m_outputUnit, level, GL::ImageAccess::WriteOnly,
                      GL::ImageFormat::R32F);
    return *this;
}

}
