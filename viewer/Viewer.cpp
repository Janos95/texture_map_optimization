//
// Created by janos on 08.11.19.
//

#include "Viewer.h"
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
#include <Magnum/Math/Color.h>

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/DebugTools/ColorMap.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/ImageView.h>

#include <random>
#include <SDL_events.h>

namespace TextureMapOptimization {

using namespace Corrade;
using namespace Magnum;

using namespace Math::Literals;

using namespace Corrade::Containers;


Matrix4
computeProjectionMatrix(float nx, float ny, float fx, float fy, float cx, float cy) {
    constexpr float zfar = 10.0f;
    constexpr float znear = 0.01;

    Matrix4 P = {
            {fx/nx, 0,     0,                             0},
            {0,     fy/ny, 0,                             0},
            {-cx,   -cy,   (znear + zfar)/(znear - zfar), -1},
            {0,     0,     2*znear*zfar/(znear - zfar),   0}
    };

    auto fov = Math::atan(nx/(2*fx));
    return Matrix4::perspectiveProjection(2*fov, nx/ny, 0.01, 10);
    return P;
}

void setupTexture(GL::Texture2D& texture, Vector2i const& size, GL::TextureFormat format) {
    texture = GL::Texture2D{};
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
        if(!tryCreate(conf, glConf)) {
            create(conf, glConf.setSampleCount(0));
        }
    }

    /* Setup camera */
    {
        const Vector3 eye = Vector3::zAxis(5.0f);
        const Vector3 viewCenter;
        const Vector3 up = Vector3::yAxis();
        const Deg fov = 45.0_degf;
        arcball.emplace(eye, viewCenter, up, fov, windowSize());
        arcball->setLagging(0.85f);

        projection = computeProjectionMatrix(640.f, 480.f, 525.f, 525.f, 319.5f, 239.5f);
    }

    /* setup texture that will be optimized */
    {
        flat = Magnum::Shaders::Flat3D{Magnum::Shaders::Flat3D::Flag::Textured};
        phong = Magnum::Shaders::Phong{Magnum::Shaders::Phong::Flag::DiffuseTexture};
        triangleShader = Shaders::FullScreenTriangle{};

        Vector2i size = Vector2i{512, 512};
        setupTexture(texture, size, GL::TextureFormat::RGBA32F);

        Containers::Array<char> data{NoInit, size.product()*sizeof(Color4)};
        for(Color4& c : Containers::arrayCast<Color4>(data)) {
            c = Color4::red();
        }
        Image2D redImage{PixelFormat::RGBA32F, size, std::move(data)};
        texture.setSubImage(0, {}, redImage);
    }

    /* setup scene*/
    {
        auto images = loadImages(
                "/home/janos/texture_map_optimization/assets/fountain_small/image");
        auto poses = loadPoses(
                "/home/janos/texture_map_optimization/assets/fountain_small/scene/key.log");
        Debug{} << "expected size " << Vector2i{640, 480} << ", got "
                << imageSize;

        Containers::arrayResize(keyFrames, images.size());
        for(std::size_t i = 0; i < images.size(); ++i) {
            auto& kf = keyFrames[i];

            setupTexture(kf.image, imageSize, GL::TextureFormat::RGBA32F);

            kf.image.setSubImage(0, {}, images[i]);
            /*computer vision -> opengl */
            kf.pose = poses[i]*Matrix4::scaling({1, -1, 1})*Matrix4::reflection({0, 0, 1});
            kf.projection = projection;
            kf.compressPose();
        }

        meshData = *loadMeshData("/home/janos/texture_map_optimization/assets/fountain_small/scene/fountain.ply");

        Debug{} << meshData.hasAttribute(Trade::MeshAttribute::Normal);
        Debug{} << meshData.hasAttribute(Trade::MeshAttribute::TextureCoordinates);
        //MeshTools::flipFaceWindingInPlace(meshData.mutableIndices());
        //meshData = MeshTools::duplicate(md);
        mesh = MeshTools::compile(meshData);

        m_tmo.emplace(keyFrames, meshData);
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
    if(ImGui::Button("Run Optimization")) {
        isOptimizing = true;
    }
    ImGui::SameLine();
    if(ImGui::Button("Map Texture")) {
        m_tmo->setTexture(texture);
        m_tmo->mapTexture();
    }
}

void Viewer::viewportEvent(ViewportEvent& event) {
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});

    arcball->reshape(event.windowSize());
    imgui.relayout(Vector2{event.windowSize()}/event.dpiScaling(),
                   event.windowSize(), event.framebufferSize());
}

void Viewer::keyPressEvent(KeyEvent& event) {
    if(imgui.handleKeyPressEvent(event)) {
        event.setAccepted();
        return;
    }

    switch(event.key()) {
        case KeyEvent::Key::L:
            if(arcball->lagging() > 0.0f) {
                Debug{} << "Lagging disabled";
                arcball->setLagging(0.0f);
            } else {
                Debug{} << "Lagging enabled";
                arcball->setLagging(0.85f);
            }
            break;
        case KeyEvent::Key::R:
            showTexture = !showTexture;
            arcball->reset();
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
    if(imgui.handleMousePressEvent(event)) {
        event.setAccepted();
        return;
    }

    if(event.button() == MouseEvent::Button::Middle) {
        trackingMouse = true;
        ///* Enable mouse capture so the mouse can drag outside of the window */
        ///** @todo replace once https://github.com/mosra/magnum/pull/419 is in */
        SDL_CaptureMouse(SDL_TRUE);

        arcball->initTransformation(event.position());

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
        if(trackingMouse) {
            SDL_CaptureMouse(SDL_FALSE);
            trackingMouse = false;
            event.setAccepted();
        }
    }
}

void Viewer::mouseMoveEvent(MouseMoveEvent& event) {
    if(imgui.handleMouseMoveEvent(event)) {
        event.setAccepted();
        return;
    }

    if(trackingMouse) {
        if(event.modifiers() & MouseMoveEvent::Modifier::Shift)
            arcball->translate(event.position());
        else arcball->rotate(event.position());

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

    const Float delta = event.offset().y();
    if(Math::abs(delta) < 1.0e-2f) return;

    arcball->zoom(delta);

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
        //GL::Texture2D testTexture;
        //testTexture.setMagnificationFilter(GL::SamplerFilter::Linear)
        //           .setMinificationFilter(GL::SamplerFilter::Linear)
        //           .setWrapping(GL::SamplerWrapping::ClampToEdge)
        //           .setStorage(1, GL::TextureFormat::R32F, {4096, 4096});
        //glClearTexImage(testTexture.id(), 0, GL_RED, GL_FLOAT, Mg::Color4{1}.data());
        //auto sum = reduce(testTexture);
    }

    {
        /* first render the mesh using the texture */
        arcball->updateTransformation();

        Matrix4 viewTf = arcball->viewMatrix();
        phong
             .bindDiffuseTexture(texture)
             .setTransformationMatrix(viewTf)
             .setNormalMatrix(viewTf.normalMatrix())
             .setProjectionMatrix(projection)
             .setLightPositions({{-3.0f, 10.0f, 10.0f, 0}})
             //.setLightPosition({-3, 10, 10})
             //.setDiffuseColor(0x2f83cc_rgbf)
             .draw(mesh);

        /* render the texture into the upper right corner ontop */
        auto vp = GL::defaultFramebuffer.viewport();
        GL::Renderer::disable(GL::Renderer::Feature::DepthTest);
        GL::defaultFramebuffer.setViewport({vp.max()*2./3., vp.max()});
        triangleShader.bindTexture(texture)
                      .draw(GL::Mesh{}.setCount(3));
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::defaultFramebuffer.setViewport(vp);
    }

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

    if(!isOptimizing)
        redraw();
}

ceres::TerminationType Viewer::runOptimization(UniqueFunction<bool()> cb) {
    m_tmo->setTexture(texture);
    return m_tmo->run(std::move(cb));
}

}