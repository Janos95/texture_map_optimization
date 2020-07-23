//
// Created by janos on 08.11.19.
//

#include "viewer.h"
#include "io.h"
#include "Reduction.h"
#include "UniqueFunction.hpp"

#include <ScopedTimer/ScopedTimer.h>

#include <Corrade/Utility/Resource.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/PluginManager/Manager.h>

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/Math/FunctionsBatch.h>
#include <Magnum/Primitives/Capsule.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/Axis.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/MeshTools/FlipNormals.h>
#include <Magnum/MeshTools/Duplicate.h>
#include <Magnum/GL/CubeMapTexture.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/DebugTools/ColorMap.h>
#include <Magnum/Image.h>
#include <Magnum/Trade/AbstractImageConverter.h>

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/DebugTools/ColorMap.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/ImageView.h>

#include <random>
#include <SDL_events.h>

using namespace Corrade;
using namespace Magnum;

using namespace Math::Literals;

using namespace Corrade::Containers;


Matrix4
computeProjectionMatrix(float nx, float ny, float fx, float fy, float cx, float cy) {
    constexpr float zfar = 10.0f;
    constexpr float znear = 0.01;

    Matrix4 P = {
             {fx/nx, 0, 0, 0},
             {0, fy/ny, 0, 0},
             {-cx, -cy, (znear + zfar)/(znear - zfar), -1},
            {0, 0, 2*znear*zfar / (znear - zfar), 0}
    };

    auto fov = Math::atan(nx / (2*fx));
    return Matrix4::perspectiveProjection(2*fov, nx/ny, 0.01, 10);
    return P;
}

void setupTexture(GL::Texture2D& texture, Vector2i const& size, GL::TextureFormat format){
    texture.setMagnificationFilter(GL::SamplerFilter::Linear)
           .setMinificationFilter(GL::SamplerFilter::Linear)
           .setWrapping(GL::SamplerWrapping::ClampToEdge)
           .setStorage(1, format, size);
}


Viewer::Viewer(Arguments const& args) : Platform::Application{args, Mg::NoCreate} {

    /* Setup window */
    {
        const Vector2 dpiScaling = this->dpiScaling({});
        Configuration conf;
        conf.setTitle("Viewer")
            .setSize(conf.size(), dpiScaling)
            .setWindowFlags(Configuration::WindowFlag::Resizable);
        GLConfiguration glConf;
        //glConf.setSampleCount(dpiScaling.max() < 2.0f ? 8 : 2);
        if(!tryCreate(conf, glConf)){
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
        camera->setLagging(0.85f);
    }

    /* setup texture that will be optimized */
    {
        Vector2i size = Vector2i{1024, 1024};
        shader = Magnum::Shaders::Flat3D{Magnum::Shaders::Flat3D::Flag::Textured};
        phong = Magnum::Shaders::Phong{Magnum::Shaders::Phong::Flag::DiffuseTexture};

        GL::Framebuffer fb({{}, texture.imageSize(0)});

        using L = std::initializer_list<std::pair<GL::Texture2D*, GL::TextureFormat>>;
        for(auto [tex, format] : L{
                                    {&texture, GL::TextureFormat::RGBA32F},
                                    {&texR, GL::TextureFormat::R32F},
                                    {&texG, GL::TextureFormat::R32F},
                                    {&texB, GL::TextureFormat::R32F},
                                    {&texA, GL::TextureFormat::R32F}
                                  })
        {
            *tex = GL::Texture2D{};
            setupTexture(*tex, size, format);

            fb.attachTexture(GL::Framebuffer::ColorAttachment{0}, *tex, 0)
              .clearColor(0, Color4{})
              .clear(GL::FramebufferClear::Color);
        }
    }

    /* setup scene*/
    {
        auto images = loadImages(
                "/home/janos/texture_map_optimization/assets/fountain_small/image");
        auto poses = loadPoses(
                "/home/janos/texture_map_optimization/assets/fountain_small/scene/key.log");
        Vector2i imageSize = images.front().size();
        Debug{} << "expected size " << Vector2i{640, 480} << ", got "
                << imageSize;

        Vector3ui wgCount(imageSize.x(), imageSize.y(), 1);

        Containers::arrayResize(keyFrames, images.size());
        for(std::size_t i = 0; i < images.size(); ++i){
            auto& kf = keyFrames[i];

            setupTexture(kf.image, imageSize, GL::TextureFormat::RGBA32F);

            keyFrames[i].image.setSubImage(0, {}, images[i]);
            keyFrames[i].pose = poses[i] * Matrix4::scaling({1,-1,1}) * Matrix4::reflection({0,0,1}); /*computer vision -> opengl */
        }

        meshData = *loadMeshData(
                "/home/janos/texture_map_optimization/assets/fountain_small/scene/blender.ply");
        //MeshTools::flipFaceWindingInPlace(meshData.mutableIndices());
        //meshData = MeshTools::duplicate(md);
        mesh = MeshTools::compile(meshData,
                                  MeshTools::CompileFlag::GenerateSmoothNormals);
        drawable = new FlatDrawable(scene, mesh, shader, &drawables);
        drawable->texture = &texture;
        projection = computeProjectionMatrix(640.f, 480.f, 525.f, 525.f, 319.5f,239.5f);

        cs = MeshTools::compile(Primitives::axis3D());
        vertexColored = Magnum::Shaders::VertexColor3D{};
        for(auto const& kf : keyFrames){
            auto obj = new Object(&scene);
            obj->scale({.1,.1,.1}).transform(kf.pose);
            new VertexColorDrawable(*obj, cs, vertexColored, &drawables);
        }

        m_tmo.emplace(keyFrames, mesh);
    }

    /* Setup ImGui, load a better font */
    {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        const Vector2 size = Vector2{windowSize()}/dpiScaling();
        Utility::Resource rs{"fonts"};
        Containers::ArrayView<const char> font = rs.getRaw(
                "SourceSansPro-Regular.ttf");
        ImGui::GetIO().Fonts->AddFontFromMemoryTTF(
                const_cast<char*>(font.data()), Int(font.size()),
                20.0f*framebufferSize().x()/size.x(), &fontConfig);

        imgui = ImGuiIntegration::Context{*ImGui::GetCurrentContext(),
                                          Vector2{windowSize()}/dpiScaling(),
                                          windowSize(), framebufferSize()};


        /* Setup proper blending to be used by ImGui */
        GL::Renderer::setBlendEquation(GL::Renderer::BlendEquation::Add,
                                       GL::Renderer::BlendEquation::Add);
        GL::Renderer::setBlendFunction(GL::Renderer::BlendFunction::SourceAlpha,
                                       GL::Renderer::BlendFunction::OneMinusSourceAlpha);

    }

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

}

void Viewer::drawOptions() {
    if(ImGui::Button("Run Optimization")){
        m_isOptimizing = true;
        m_tmo->setTexture(texture);
        m_tmo->run([this]{ drawEvent(); });
        m_isOptimizing = false;
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
    if(imgui.handleKeyPressEvent(event)){
        event.setAccepted();
        return;
    }

    if(!camera) return;

    switch(event.key()) {
        case KeyEvent::Key::L:
            if(camera->lagging() > 0.0f){
                Debug{} << "Lagging disabled";
                camera->setLagging(0.0f);
            } else{
                Debug{} << "Lagging enabled";
                camera->setLagging(0.85f);
            }
            break;
        case KeyEvent::Key::R:
            showTexture = !showTexture;
            camera->reset();
            break;
        default:
            return;
    }

    event.setAccepted();
    redraw(); /* camera or mesh has changed, redraw! */
}

void Viewer::keyReleaseEvent(KeyEvent& event) {
    if(imgui.handleKeyReleaseEvent(event)){
        event.setAccepted();
        return;
    }
}

void Viewer::textInputEvent(TextInputEvent& event) {
    if(imgui.handleTextInputEvent(event)){
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
    if(imgui.handleMouseReleaseEvent(event)){
        event.setAccepted();
        return;
    }

    if(event.button() == MouseEvent::Button::Middle){
        /* Disable mouse capture again */
        /** @todo replace once https://github.com/mosra/magnum/pull/419 is in */
        if(trackingMouse){
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

    if(trackingMouse){
        if(event.modifiers() & MouseMoveEvent::Modifier::Shift)
            camera->translate(event.position());
        else camera->rotate(event.position());

        event.setAccepted();
        redraw(); /* camera has changed, redraw! */
    }
}

void Viewer::mouseScrollEvent(MouseScrollEvent& event) {
    if(imgui.handleMouseScrollEvent(event)){
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
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);


    imgui.newFrame();
    /* Enable text input, if needed */
    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    {
        GL::Texture2D testTexture;
        testTexture.setMagnificationFilter(GL::SamplerFilter::Linear)
                   .setMinificationFilter(GL::SamplerFilter::Linear)
                   .setWrapping(GL::SamplerWrapping::ClampToEdge)
                   .setStorage(1, GL::TextureFormat::R32F, {4096, 4096});
        glClearTexImage(testTexture.id(), 0, GL_RED, GL_FLOAT, Mg::Color4{1}.data());
        auto sum = reduce(testTexture);
    }

    {
        /* first render the mesh using the texture */
        camera->update();
        camera->draw(drawables);

        /* render the texture into the upper right corner ontop */
        auto vp = GL::defaultFramebuffer.viewport();
        GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
        GL::defaultFramebuffer.setViewport({vp.max() * 3 / 4, vp.max()});
        triangleShader.bindTexture(texture)
                      .draw(GL::Mesh{}.setCount(3));
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::defaultFramebuffer.setViewport(vp);
    }

    if(!m_isOptimizing)
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

    if(!m_isOptimizing)
        redraw();
}
