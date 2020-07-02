//
// Created by janos on 7/2/20.
//

#include "divide_by_alpha.h"

#include <Magnum/GL/Texture.h>
#include <Magnum/GL/Shader.h>
#include <Magnum/GL/Version.h>
#include <Magnum/GL/Context.h>
#include <Magnum/Math/Matrix4.h>

#include <Corrade/Containers/Reference.h>
#include <Corrade/Utility/Resource.h>

using namespace Magnum;

namespace shaders{

    DivideByAlpha::DivideByAlpha() {
        MAGNUM_ASSERT_GL_VERSION_SUPPORTED(GL::Version::GL450);

        Utility::Resource rs{"tmo-data"};

        GL::Shader comp{GL::Version::GL450, GL::Shader::Type::Compute};

        comp.addSource(rs.get("divide_by_alpha.comp"));

        CORRADE_INTERNAL_ASSERT_OUTPUT(GL::Shader::compile({comp}));

        attachShaders({comp});

        CORRADE_INTERNAL_ASSERT_OUTPUT(link());
    }


    DivideByAlpha& DivideByAlpha::bindInput(Mg::GL::Texture2D& input) {
        input.bind(m_inputUnit);
        return *this;
    }

    DivideByAlpha& DivideByAlpha::bindOutput(Mg::GL::Texture2D& output){
        output.bind(m_ouputUnit);
        return *this;
    }

}
