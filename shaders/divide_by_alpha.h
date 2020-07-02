//
// Created by janos on 7/2/20.
//

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Generic.h>

namespace Mg = Magnum;

namespace shaders{

class DivideByAlpha : public Mg::GL::AbstractShaderProgram {
public:

    explicit DivideByAlpha();
    explicit DivideByAlpha(Mg::NoCreateT) : Mg::GL::AbstractShaderProgram{Mg::NoCreate} {};

    DivideByAlpha& bindInput(Mg::GL::Texture2D&);
    DivideByAlpha& bindOutput(Mg::GL::Texture2D&);

private:

    Mg::Int m_inputUnit = 0;
    Mg::Int m_ouputUnit = 1;
};

}
