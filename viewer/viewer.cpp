//
// Created by janos on 08.11.19.
//

#include "viewer.h"
#include "io.h"
#include "divide_by_alpha.h"

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
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/GL/CubeMapTexture.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/Image.h>
#include <Magnum/Trade/AbstractImageConverter.h>

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/DebugTools/ColorMap.h>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/PixelFormat.h>
#include <Magnum/ImageView.h>
#include <Magnum/Image.h>

using namespace Corrade;
using namespace Magnum;

using namespace Math::Literals;

using namespace Corrade::Containers;

Matrix4 computeProjectionMatrix(float width, float height, float fx, float fy, float cx, float cy)
{
    // Source: https://blog.noctua-software.com/opencv-opengl-projection-matrix.html
    constexpr float zfar = 10.0f;
    constexpr float znear = 0.1;

    Matrix4 P(Mg::Math::ZeroInit);
    P[0][0] = 2.0 * fx / width;
    P[1][1] = -2.0 * fy / height;
    P[2][0] = 1.0 - 2.0 * cx / width;
    P[2][1] = 2.0 * cy / height - 1.0;
    P[2][2] = (zfar + znear) / (znear - zfar);
    P[2][3] = -1.0;
    P[3][2] = 2.0 * zfar * znear / (znear - zfar);

    return P;
}


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

    /* setup texture that will be optimized */
    Vector2i size{1024,1024};
    {
        Containers::Array<char> data(Containers::NoInit, size.product() * 4 * sizeof(float));
        for(auto& x : Containers::arrayCast<float>(data))
            x = 0.1f;
        Image2D image{PixelFormat::RGBA32F, size, std::move(data)};
        shader = Shaders::Flat3D{Shaders::Flat3D::Flag::Textured};
        texture = GL::Texture2D{};
        texture.setMagnificationFilter(GL::SamplerFilter::Linear)
               .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
               .setWrapping(GL::SamplerWrapping::ClampToEdge)
               .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
               .setStorage(1, GL::TextureFormat::RGBA32F, size)
               .setSubImage(0, {}, image);
    }

    /* setup scene*/
    {
        auto images = loadImages("/home/janos/texture_map_optimization/assets/fountain_small/image");
        Vector2i imageSize = images.front().size();
        Debug{} << "expected size " << Vector2i{640,480} << ", got " << imageSize;

        Containers::arrayResize(imageTextures, images.size());
        for (std::size_t i = 0; i < images.size(); ++i) {
            imageTextures[i].setMagnificationFilter(GL::SamplerFilter::Linear)
                            .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
                            .setWrapping(GL::SamplerWrapping::ClampToEdge)
                            .setMaxAnisotropy(GL::Sampler::maxMaxAnisotropy())
                            .setStorage(1, GL::TextureFormat::RGBA32F, imageSize)
                            .setSubImage(0, {}, images[i]);
        }

        meshData = *loadMeshData("/home/janos/texture_map_optimization/assets/fountain_small/scene/integrated.ply");
        mesh = MeshTools::compile(meshData, MeshTools::CompileFlag::GenerateSmoothNormals);
        drawable = new FlatDrawable(scene, mesh, shader, &drawables);
        drawable->texture = &texture;
        poses = loadPoses("/home/janos/texture_map_optimization/assets/fountain_small/scene/key.log");
        projection = computeProjectionMatrix(640.f, 480.f, 525.f, 525.f, 319.5f, 239.5f);

        Range2Di viewport{{}, imageSize};
        GL::Texture2D colors[2];
        GL::Framebuffer fb{viewport};
        GL::Renderbuffer depthBuffer;
        averageColor = shaders::AverageColor{};

        depthBuffer.setStorage(GL::RenderbufferFormat::DepthComponent24, viewport.size());
        fb.attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth, depthBuffer);

        for (auto& color : colors) {
            color.setStorage(1, GL::TextureFormat::RGBA32F, size)
                 .setWrapping(GL::SamplerWrapping::ClampToEdge)
                 .setMinificationFilter(GL::SamplerFilter::Linear, GL::SamplerMipmap::Linear)
                 .setMagnificationFilter(GL::SamplerFilter::Linear);
        }

        fb.attachTexture(GL::Framebuffer::ColorAttachment{0}, colors[0], 0)
          .clearColor(0, Color4{})
          .clear(GL::FramebufferClear::Color);

        for (int i = 0; i < poses.size(); ++i) {
            int last = i % 2;
            int current = (i+1) % 2;

            fb.attachTexture(GL::Framebuffer::ColorAttachment{0}, colors[current], 0)
              .mapForDraw(GL::Framebuffer::ColorAttachment{0})
              .clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth)
              .bind();

            CORRADE_INTERNAL_ASSERT(fb.checkStatus(GL::FramebufferTarget::Draw) == GL::Framebuffer::Status::Complete);

            averageColor.setTransformationMatrix(poses[i])
                        .setProjectionMatrix(projection)
                        .bindColors(colors[last])
                        .bindImage(imageTextures[i])
                        .draw(mesh);
        }

        shaders::DivideByAlpha divide;
        Vector2ui s{size};
        Debug{} << "texture size " << s;
        /* @todo do I need a framebuffer ? */
        divide.bindInput(colors[poses.size()%2])
              .bindOutput(texture)
              .dispatchCompute({s.x(), s.y(), 1});
        GL::Renderer::setMemoryBarrier(Mg::GL::Renderer::MemoryBarrier::TextureFetch);
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

    GL::Renderer::setDepthFunction(GL::Renderer::DepthFunction::LessOrEqual);
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    /* Start the timer, loop at 60 Hz max */
    setSwapInterval(1);
    setMinimalLoopPeriod(16);
}

void Viewer::drawOptions() {
    if(ImGui::Button("Hot-Reload Shader")){

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
