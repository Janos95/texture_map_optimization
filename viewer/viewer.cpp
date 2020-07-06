//
// Created by janos on 08.11.19.
//

#include "viewer.h"
#include "io.h"
#include "Remap.h"
#include "ScreenTriangle.h"

#include <scoped_timer/scoped_timer.hpp>

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


Viewer::Viewer(int argc, char** argv) : Platform::Application{{argc, argv},
                                                              Mg::NoCreate} {
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
        Vector2i imageSize = images.front().size();
        Debug{} << "expected size " << Vector2i{640, 480} << ", got "
                << imageSize;

        Containers::arrayResize(imageTextures, images.size());
        for(std::size_t i = 0; i < images.size(); ++i){
            setupTexture(imageTextures[i], imageSize, GL::TextureFormat::RGBA32F);
            imageTextures[i].setSubImage(0, {}, images[i]);
            //imageTextures[i].setMagnificationFilter(GL::SamplerFilter::Linear)
            //                .setMinificationFilter(GL::SamplerFilter::Linear,
            //                                       GL::SamplerMipmap::Linear)
            //                .setWrapping(GL::SamplerWrapping::ClampToEdge)
            //                .setStorage(1, GL::TextureFormat::RGBA32F,
            //                            imageSize)
            //                .setSubImage(0, {}, images[i]);
        }

        meshData = *loadMeshData(
                "/home/janos/texture_map_optimization/assets/fountain_small/scene/blender.ply");
        //MeshTools::flipFaceWindingInPlace(meshData.mutableIndices());
        //meshData = MeshTools::duplicate(md);
        mesh = MeshTools::compile(meshData,
                                  MeshTools::CompileFlag::GenerateSmoothNormals);
        drawable = new FlatDrawable(scene, mesh, shader, &drawables);
        drawable->texture = &texture;
        poses = loadPoses(
                "/home/janos/texture_map_optimization/assets/fountain_small/scene/key.log");
        projection = computeProjectionMatrix(640.f, 480.f, 525.f, 525.f, 319.5f,239.5f);

        texCoordsShader = ::Shaders::TextureCoordinates{};
        remap = ::Shaders::Remap{};
        combine = ::Shaders::Combine{};

        cs = MeshTools::compile(Primitives::axis3D());
        vertexColored = Magnum::Shaders::VertexColor3D{};
        for(auto& tf : poses){
            tf = tf * Matrix4::scaling({1,-1,1}) * Matrix4::reflection({0,0,1});
            auto obj = new Object(&scene);
            obj->scale({.1,.1,.1}).transform(tf);
            new VertexColorDrawable(*obj, cs, vertexColored, &drawables);
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

    /* Start the timer, loop at 60 Hz max */
    setSwapInterval(1);
    setMinimalLoopPeriod(16);
}

void Viewer::drawOptions() {
    ImGui::Checkbox("Show Texture", &showTexture);
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
        ScopedTimer t{"Averaging all colors", true};

        Range2Di viewport{{}, imageSize};
        GL::Framebuffer fb{viewport};
        GL::Renderbuffer depthBuffer;

        depthBuffer.setStorage(GL::RenderbufferFormat::DepthComponent24,
                               viewport.size());
        fb.attachRenderbuffer(GL::Framebuffer::BufferAttachment::Depth,
                              depthBuffer);

        GL::Texture2D texCoordsTexture;
        setupTexture(texCoordsTexture, imageSize, GL::TextureFormat::RGBA32F);

        fb.attachTexture(GL::Framebuffer::ColorAttachment{0}, texCoordsTexture, 0)
          .mapForDraw(GL::Framebuffer::ColorAttachment{0})
          .bind();

        CORRADE_INTERNAL_ASSERT(
                fb.checkStatus(GL::FramebufferTarget::Draw) ==
                GL::Framebuffer::Status::Complete);


        Vector3ui wgCount1(imageSize.x(), imageSize.y(), 1);
        Vector3ui wgCount2(textureSize.x(), textureSize.y(), 1);

        const auto map = DebugTools::ColorMap::turbo();
        const Vector2i mapSize{Int(map.size()), 1};

        GL::Texture2D colorMapTexture;
        colorMapTexture
                .setMinificationFilter(SamplerFilter::Linear)
                .setMagnificationFilter(SamplerFilter::Linear)
                .setWrapping(SamplerWrapping::ClampToEdge) // or Repeat
                .setStorage(1, GL::TextureFormat::RGB8, mapSize) // or SRGB8
                .setSubImage(0, {}, ImageView2D{PixelFormat::RGB8Srgb, mapSize, map});

        fb.clearColor(0, Color4{});

        for(int i = 0; i < poses.size(); ++i){

            fb.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth);
            texCoordsShader.setTransformationProjectionMatrix(projection * poses[i].inverted())
                           .bindColorMap(colorMapTexture)
                           .setNumPrimitives(meshData.indexCount() / 3)
                           .draw(mesh);

            remap.bindTextureR(texR)
                 .bindTextureG(texG)
                 .bindTextureB(texB)
                 .bindTextureA(texA)
                 .bindImage(imageTextures[i])
                 .bindTextureCoordinates(texCoordsTexture)
                 .dispatchCompute(wgCount1);
        }

        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        combine.bindTextureR(texR)
               .bindTextureG(texG)
               .bindTextureB(texB)
               .bindTextureA(texA)
               .bindRgbaImage(texture)
               .dispatchCompute(wgCount2);

        //GL::Renderer::setMemoryBarrier(Mg::GL::Renderer::MemoryBarrier::TextureFetch);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        fb.clear(GL::FramebufferClear::Color);
        //auto coordsImage = texCoordsTexture.image(0,PixelFormat::RGBA32F);
        //for(auto row : coordsImage.pixels<Color4>()){
        //    for(auto& p : row){
        //        Debug{} << p;
        //    }
        //}
        //std::abort();

        GL::defaultFramebuffer.bind();
        if(!showTexture){
            camera->update();
            camera->draw(drawables);
        } else {
            GL::defaultFramebuffer.bind();
            ::Shaders::ScreenTriangle triangleShader;
            triangleShader.bindTexture(texture)
                          .draw(GL::Mesh{}.setCount(3));
        }

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
    redraw();
}
