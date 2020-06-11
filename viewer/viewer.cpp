//
// Created by janos on 08.11.19.
//

#include "viewer.hpp"
#include "io.hpp"

#include <Corrade/Utility/Resource.h>
#include <Corrade/Containers/GrowableArray.h>

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/Math/FunctionsBatch.h>
#include <Magnum/Primitives/UVSphere.h>
#include <Magnum/Primitives/Capsule.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/Transform.h>
#include <Magnum/MeshTools/FlipNormals.h>
#include <Magnum/MeshTools/Concatenate.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/GL/CubeMapTexture.h>
#include <Magnum/GL/Framebuffer.h>

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/DebugTools/ColorMap.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/ImageView.h>

using namespace Corrade;
using namespace Magnum;

using namespace Math::Literals;

using namespace Corrade::Containers;

struct VertexData {
    Vector3 position;
    Vector3 normal;
    Vector2 uv;
};

Viewer::Viewer(int argc, char** argv): Platform::Application{{argc,argv},Mg::NoCreate}{
        /* Setup window */
        {
            const Vector2 dpiScaling = this->dpiScaling({});
            Configuration conf;
            conf.setTitle("Viewer")
                    .setSize(conf.size(), dpiScaling)
                    .setWindowFlags(Configuration::WindowFlag::Resizable);
            GLConfiguration glConf;
            glConf.setSampleCount(dpiScaling.max() < 2.0f ? 8 : 2);
            if(!tryCreate(conf, glConf)) {
                create(conf, glConf.setSampleCount(0));
            }
        }

        /* Setup the arcball after the camera objects */
        const Vector3 eye = Vector3::zAxis(5.0f);
        {
            const Vector3 center{};
            const Vector3 up = Vector3::yAxis();
            camera.emplace(scene, eye, center, up, 45._degf,
                           windowSize(), framebufferSize());
        }

        /* setup shaders and color map textures */
        {
            float spacing = 2.5;
            pbrShader = PbrShader{4};
            phongShader = Shaders::Phong{};

            auto meshData = Primitives::uvSphereSolid(100, 100, Primitives::UVSphereFlag::TextureCoordinates);
            mesh = MeshTools::compile(meshData);


            const auto map = Mg::DebugTools::ColorMap::turbo();
            const Vector2i size{Int(map.size()), 1};
            colorMapTexture
                    .setMinificationFilter(SamplerFilter::Linear)
                    .setMagnificationFilter(SamplerFilter::Linear)
                    .setWrapping(SamplerWrapping::ClampToEdge) // or Repeat
                    .setStorage(1, GL::TextureFormat::RGB8, size) // or SRGB8
                    .setSubImage(0, {}, ImageView2D{PixelFormat::RGB8Srgb, size, map});

            for (int row = 0; row < 7; ++row){
                float metallness = (float)row / 7.f;
                for (int col = 0; col < 7; ++col){
                    auto object = new Object{&scene};
                    object->translate({(col - 7/2) * spacing, (row - 7/2) * spacing, 0.f});
                    auto drawable = new PbrDrawable{*object, mesh, pbrShader, &drawables};
                    drawable->roughness = Math::clamp((float)col / 7.f, 0.05f, 1.0f);
                    drawable->metallness = metallness;
                }
            }

        }

        /* Setup ImGui, load a better font */
        {
            ImGui::CreateContext();
            ImGui::StyleColorsDark();

            ImFontConfig fontConfig;
            fontConfig.FontDataOwnedByAtlas = false;
            const Vector2 size = Vector2{windowSize()}/dpiScaling();
            Utility::Resource rs{"fonts"};
            Containers::ArrayView<const char> font = rs.getRaw("SourceSansPro-Regular.ttf");
            ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
                    const_cast<char*>(font.data()), Int(font.size()),
                    20.0f*framebufferSize().x()/size.x(), &fontConfig);

            imgui = ImGuiIntegration::Context{*ImGui::GetCurrentContext(),
                                              Vector2{windowSize()}/dpiScaling(), windowSize(), framebufferSize()};

            /* Setup proper blending to be used by ImGui */
            GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
                                           GL::Renderer::BlendEquation::Add);
            GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                           GL::Renderer::BlendFunction::OneMinusSourceAlpha);

        }

        {
            auto image = loadHdrImage("/home/janos/texture_map_optimization/assets/newport_loft.hdr");
            CORRADE_INTERNAL_ASSERT(image);
            Vector2i size = image->size();

            equirectangularTexture = GL::Texture2D{};
            equirectangularTexture.setWrapping(GL::SamplerWrapping::ClampToEdge)
                                  .setMagnificationFilter(GL::SamplerFilter::Linear)
                                  .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
                                  .setStorage(Math::log2(size.min()) + 1, GL::TextureFormat::RGB32F, size)
                                  .setSubImage(0, {}, *image);

            /* flip face winding since we render from inside the cube */
            auto cubeData = MeshTools::concatenate(Primitives::cubeSolid());
            MeshTools::flipFaceWindingInPlace(cubeData.mutableIndices());
            cube = MeshTools::compile(cubeData);
            CubeMapShader cubeShader;

            GL::Renderer::enable(GL::Renderer::Feature::SeamlessCubeMapTexture);
            cubeMap = Mg::GL::CubeMapTexture{};
            cubeMap.setStorage(Math::log2(512) + 1, GL::TextureFormat::RGB32F, size);

            auto projection = Matrix4::perspectiveProjection(90._degf, 1., 0.1, 10.);

            cubeShader.setProjectionMatrix(projection)
                      .bindEquirectangularTexture(equirectangularTexture);
            using L = std::initializer_list<std::pair<GL::CubeMapCoordinate, Matrix4>>;
            for(auto&& [coordinate, tf] : L{
                                {GL::CubeMapCoordinate::PositiveX, Matrix4::lookAt({}, { 1.0f,  0.0f,  0.0f}, {0.0f, -1.0f,  0.0f}).invertedRigid()},
                                {GL::CubeMapCoordinate::NegativeX, Matrix4::lookAt({}, {-1.0f,  0.0f,  0.0f}, {0.0f, -1.0f,  0.0f}).invertedRigid()},
                                {GL::CubeMapCoordinate::PositiveY, Matrix4::lookAt({}, { 0.0f,  1.0f,  0.0f}, {0.0f,  0.0f,  1.0f}).invertedRigid()},
                                {GL::CubeMapCoordinate::NegativeY, Matrix4::lookAt({}, { 0.0f, -1.0f,  0.0f}, {0.0f,  0.0f, -1.0f}).invertedRigid()},
                                {GL::CubeMapCoordinate::PositiveZ, Matrix4::lookAt({}, { 0.0f,  0.0f,  1.0f}, {0.0f, -1.0f,  0.0f}).invertedRigid()},
                                {GL::CubeMapCoordinate::NegativeZ, Matrix4::lookAt({}, { 0.0f,  0.0f, -1.0f}, {0.0f, -1.0f,  0.0f}).invertedRigid()}
                            })
            {
                Mg::Range2Di viewport{{}, {512, 512}};
                GL::Framebuffer fb{viewport};

                fb.attachCubeMapTexture(
                        GL::Framebuffer::ColorAttachment{0},
                        cubeMap,
                        coordinate, 0)
                  .mapForDraw(GL::Framebuffer::ColorAttachment{0})
                  .clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth)
                  .bind();

                CORRADE_INTERNAL_ASSERT(fb.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete);

                cubeShader.setTransformationMatrix(tf)
                          .draw(cube);
            }
        }

        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

        /* Start the timer, loop at 60 Hz max */
        setSwapInterval(1);
        setMinimalLoopPeriod(16);
}

void Viewer::drawOptions() {
    constexpr float min = 0.f, max = 1.f;
    //ImGui::SliderScalar("Metallness", ImGuiDataType_Float, &drawable->metallness, &min, &max, "%.3f", 1);
    //ImGui::SliderScalar("Roughness", ImGuiDataType_Float, &drawable->roughness, &min, &max, "%.3f", 1);
    if(ImGui::Button("Hot-Reload Shader")){
        Utility::Resource::overrideGroup("tmo-data", "/home/janos/texture_map_optimization/shaders/resources.conf");
        pbrShader = PbrShader{};
    }
}



void Viewer::viewportEvent(ViewportEvent& event) {
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    if(camera)
        camera->reshape(event.windowSize(), event.framebufferSize());

    imgui.relayout(Vector2{event.windowSize()}/event.dpiScaling(),
                   event.windowSize(), event.framebufferSize());
}


void Viewer::keyPressEvent(KeyEvent& event) {
    if(imgui.handleKeyPressEvent(event)) {
        event.setAccepted();
        return;
    }

    if(!camera) return;

    switch(event.key()) {
        case KeyEvent::Key::L:
            if(camera->lagging() > 0.0f) {
                Debug{} << "Lagging disabled";
                camera->setLagging(0.0f);
            } else {
                Debug{} << "Lagging enabled";
                camera->setLagging(0.85f);
            }
            break;
        case KeyEvent::Key::R:
            camera->reset();
            break;
        default:
            return;
    }

    event.setAccepted();
    redraw(); /* camera or mesh has changed, redraw! */
}

void Viewer::keyReleaseEvent(KeyEvent& event) {
    if(imgui.handleKeyReleaseEvent(event)) {
        event.setAccepted();
        return;
    }
}

void Viewer::textInputEvent(TextInputEvent& event) {
    if(imgui.handleTextInputEvent(event)) {
        event.setAccepted();
        return;
    }
}

void Viewer::mousePressEvent(MouseEvent& event) {
    if(imgui.handleMousePressEvent(event)){
        event.setAccepted();
        return;
    }

    if(event.button() == MouseEvent::Button::Middle){
        trackingMouse = true;
        ///* Enable mouse capture so the mouse can drag outside of the window */
        ///** @todo replace once https://github.com/mosra/magnum/pull/419 is in */
        SDL_CaptureMouse(SDL_TRUE);

        camera->initTransformation(event.position());

        event.setAccepted();
        redraw(); /* camera has changed, redraw! */
    }
}

void Viewer::mouseReleaseEvent(MouseEvent& event) {
    if(imgui.handleMouseReleaseEvent(event)) {
        event.setAccepted();
        return;
    }

    if(event.button() == MouseEvent::Button::Middle) {
        /* Disable mouse capture again */
        /** @todo replace once https://github.com/mosra/magnum/pull/419 is in */
        if (trackingMouse) {
            SDL_CaptureMouse(SDL_FALSE);
            trackingMouse = false;
            event.setAccepted();
        }
    }
}

void Viewer::mouseMoveEvent(MouseMoveEvent& event) {
    if(imgui.handleMouseMoveEvent(event)){
        event.setAccepted();
        return;
    }

    if(trackingMouse) {
        if (event.modifiers() & MouseMoveEvent::Modifier::Shift)
            camera->translate(event.position());
        else camera->rotate(event.position());

        event.setAccepted();
        redraw(); /* camera has changed, redraw! */
    }
}

void Viewer::mouseScrollEvent(MouseScrollEvent& event) {
    if(imgui.handleMouseScrollEvent(event)) {
        /* Prevent scrolling the page */
        event.setAccepted();
        return;
    }

    if(!camera) return;

    const Float delta = event.offset().y();
    if(Math::abs(delta) < 1.0e-2f) return;

    camera->zoom(delta);

    event.setAccepted();
    redraw(); /* camera has changed, redraw! */
}

void Viewer::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);
    imgui.newFrame();

    /* Enable text input, if needed */
    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    /* draw scene */
    camera->update();
    camera->draw(drawables);

    auto tf = camera->transformationMatrix().inverted();
    auto projection = camera->projectionMatrix();
    cubeShader.setTransformationMatrix(tf)
              .setProjectionMatrix(projection)
              .bindEquirectangularTexture(equirectangularTexture)
              .draw(cube);

    //draw modifiers
    drawOptions();
    imgui.updateApplicationCursor(*this);

    /* Render ImGui window */
    {
        GL::Renderer::enable(GL::Renderer::Feature::Blending);
        GL::Renderer::disable(GL::Renderer::Feature::FaceCulling);
        GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::ScissorTest);

        imgui.drawFrame();

        GL::Renderer::disable(GL::Renderer::Feature::ScissorTest);
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);
        GL::Renderer::disable(GL::Renderer::Feature::Blending);
    }

    swapBuffers();
    redraw();
}
