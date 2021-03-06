//
// Created by janos on 08.11.19.
//

#include "Viewer.h"
#include "IO.h"
#include "Reduction.h"
#include "UniqueFunction.h"
#include "Optimization.h"
#include "ScopedTimer.h"

#include <Corrade/Utility/Resource.h>
#include <Corrade/Containers/GrowableArray.h>
#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Containers/StringView.h>
#include <Corrade/Containers/StringStl.h>

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/GL/RenderbufferFormat.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/ImageView.h>
#include <Magnum/Math/FunctionsBatch.h>
#include <Magnum/Primitives/Capsule.h>
#include <Magnum/Primitives/Axis.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/GL/CubeMapTexture.h>
#include <Magnum/DebugTools/ColorMap.h>
#include <Magnum/Image.h>
#include <Magnum/Trade/AbstractImageConverter.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Algorithms/Svd.h>

#include <Magnum/ImGuiIntegration/Context.hpp>
#include <Magnum/GL/TextureFormat.h>
#include <Magnum/PixelFormat.h>
#include <Corrade/Utility/Directory.h>

#include "imgui_internal.h"

namespace TextureMapOptimization {

template <typename T, size_t Size>
char (*countof_helper(T (&_Array)[Size]))[Size];

#define COUNTOF(array) (sizeof(*countof_helper(array)) + 0)

using namespace Math::Literals;
using namespace Corrade::Containers::Literals;

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

/**
 * If matrix is not rigid, project rotation part onto SO3.
 * By default, we assume that the poses transform camera
 * coordinates into world coordinates. For optimization
 * we need the inverse of that. Furthermore we assume that
 * the the transformation are in "computer vision" coordinates
 * which is different from the coordinate conventions opengl
 * uses, so we fix that as well.
 * @param transformations
 * @param cv2gl whether to do computer vision to opengl coordinate conversion
 * @param invert if true, invert the transformations
 */
void preprocessTransformations(Array<Matrix4>& transformations, bool cv2gl = true, bool invert = true) {
    for(auto& tf : transformations) {
        if(!tf.isRigidTransformation()) {
            Matrix3 rot{{tf[0].xyz(),
                                tf[1].xyz(),
                                tf[2].xyz()}};
            auto [U, _, V] = Math::Algorithms::svd(rot);
            tf = Matrix4::from(U*V.transposed(), tf.translation());
            CORRADE_ASSERT(tf.isRigidTransformation(), "Couldn't salvage not rigid transformation",);
        }
        if(cv2gl)
            tf = tf*Matrix4::scaling({1, -1, 1})*Matrix4::reflection({0, 0, 1});
        if(invert)
            tf = tf.invertedRigid();
    }
}

void setupTexture(GL::Texture2D& texture, Vector2i const& size, GL::TextureFormat format) {
    texture = GL::Texture2D{};
    texture.setMagnificationFilter(GL::SamplerFilter::Linear)
           .setMinificationFilter(GL::SamplerFilter::Linear)
           .setWrapping(GL::SamplerWrapping::ClampToEdge)
           .setStorage(1, format, size);
}

Viewer::Viewer(Arguments const& args) : Mg::Platform::Application{args, Mg::NoCreate} {

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
        vertexColored = Magnum::Shaders::VertexColor3D{};
        triangleShader = Shaders::FullScreenTriangle{};

        Vector2i size = Vector2i{512, 512};
        setupTexture(texture, size, GL::TextureFormat::RGBA32F);

        Array<char> data{NoInit, size.product()*sizeof(Color4)};
        for(Color4& c : arrayCast<Color4>(data)) {
            c = Color4::red();
        }
        Mg::Image2D redImage{Mg::PixelFormat::RGBA32F, size, std::move(data)};
        texture.setSubImage(0, {}, redImage);
    }

    /* setup scene*/
    if(Cr::Utility::Directory::exists(path)) {
        loadScene(path);
    }

    /* Setup ImGui, load a better font */
    {
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGuiIO& io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        ImFontConfig fontConfig;
        fontConfig.FontDataOwnedByAtlas = false;
        const Vector2 size = Vector2{windowSize()}/dpiScaling();
        Cr::Utility::Resource rs{"fonts"};
        ArrayView<const char> font = rs.getRaw(
                "SourceSansPro-Regular.ttf");
        io.Fonts->AddFontFromMemoryTTF(
                const_cast<char*>(font.data()), Int(font.size()),
                20.0f*framebufferSize().x()/size.x(), &fontConfig);

        imgui = Mg::ImGuiIntegration::Context{*ImGui::GetCurrentContext(),
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
    if(ImGui::Begin("Options")) {
        if(ImGui::Button("Run Optimization")) {
            isOptimizing = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Map Texture")) {
            renderPass->setTexture(texture);
            renderPass->averagingPass();
        }

        ImGui::Checkbox("Draw Poses", &drawPoses);

        if(ImGui::InputInt("Keyframe Idx", &currentKf)) {
            currentKf = Math::clamp<int>(currentKf, 0, keyFrames.size() - 1);
            onNewKeyFrame();
        }

        static const char* options[] = {
                "Texture",
                "Groundtruth Image",
                "Rendered Image",
                "Cost",
                "Rotation Gradient",
                "Translation Gradient",
        };

        if(ImGui::BeginCombo("Overlay Options", options[currentOption])) {
            for(size_t i = 0; i < COUNTOF(options); ++i) {
                bool isSelected = i == currentOption;
                if(ImGui::Selectable(options[i], isSelected)) {
                    currentOption = i;
                    RenderPass::VisualizatonFlag visFlag{0};
                    switch(i) {
                        case 0:
                            overlay = &texture;
                            onNewKeyFrame = []{}; /* remove old callback */
                            break;
                        case 1:
                            onNewKeyFrame = [this]{ overlay = &keyFrames[currentKf].image; };
                            onNewKeyFrame();
                            break;
                        case 2:
                            visFlag = RenderPass::VisualizatonFlag::RenderedImage;
                            break;
                        case 3:
                            visFlag = RenderPass::VisualizatonFlag::Cost;
                            break;
                        case 4:
                            visFlag = RenderPass::VisualizatonFlag::RotationGradient;
                            break;
                        case 5:
                            visFlag = RenderPass::VisualizatonFlag::TranslationGradient;
                            break;

                        default: CORRADE_ASSERT(false, "Unknown Overlay Option", );
                    }
                    if(bool(visFlag)) {
                        onNewKeyFrame = [this, visFlag]{
                            renderPass->renderIntoTexture(renderedImage, currentKf, visFlag);
                            GL::DefaultFramebuffer().bind();
                        };
                        onNewKeyFrame();
                        overlay = &renderedImage;
                    }
                }
                if(isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if(ImGui::Button("Camera To Keyframe")) {
            const Matrix4 pose = keyFrames[currentKf].tf.invertedRigid();
            arcball->setViewParameters(pose.translation(), pose.translation()-pose[2].xyz(), pose[1].xyz());
        }

        if(ImGui::Button("Reload Shaders")) {
            renderPass->reloadShader();
        }

        ImGui::Text("Import/Export to");
        ImGui::InputText("##Export to", path, sizeof(path));

        if(ImGui::Button("Load Scene")) {
            loadScene(path);
        }

        ImGui::End();
    }
}

void Viewer::loadScene(const char* path) {

    std::string imagesPath = Cr::Utility::Directory::join({path,"image"});
    std::string scenePath = Cr::Utility::Directory::join({path ,"scene"});
    std::string transformationsPath = Cr::Utility::Directory::join(scenePath ,"key.log");
    std::string meshPath = Cr::Utility::Directory::join(scenePath ,"mesh.ply");

    auto images = loadImages(imagesPath);
    auto transformations = loadPoses(transformationsPath);

    preprocessTransformations(transformations);

    imageSize = images.front().size();
    Debug{} << "Image size of imported images" << imageSize;
    setupTexture(renderedImage, imageSize, GL::TextureFormat::RGBA32F);

    arrayResize(keyFrames, images.size());
    for(std::size_t i = 0; i < images.size(); ++i) {
        //if(i % 10 != 0) continue;

        auto& kf = keyFrames[i];

        setupTexture(kf.image, imageSize, GL::TextureFormat::RGBA32F);

        kf.image.setSubImage(0, {}, images[i]);
        /*computer vision -> opengl */
        kf.tf = transformations[i];
        kf.compressPose();
    }

    meshData = *loadMeshData(meshPath);

    Debug{} << meshData.hasAttribute(Mg::Trade::MeshAttribute::Normal);
    Debug{} << meshData.hasAttribute(Mg::Trade::MeshAttribute::TextureCoordinates);
    //MeshTools::flipFaceWindingInPlace(meshData.mutableIndices());
    //meshData = MeshTools::duplicate(md);
    mesh = Mg::MeshTools::compile(meshData);
    axis = Mg::MeshTools::compile(Mg::Primitives::axis3D());

    renderPass.emplace(mesh, keyFrames);
    renderPass->setTexture(texture);
    renderPass->setCameraParameters(640.f, 480.f, 525.f, 525.f, 319.5f, 239.5f);

    /* do one averaging pass so we have some color to begin with.
     * Also set the camera to the first key frame pose */
    renderPass->averagingPass();
    auto pose = keyFrames[currentKf].tf.invertedRigid();
    arcball->setViewParameters(pose.translation(), pose.translation()-pose[2].xyz(), pose[1].xyz());

    loaded = true;
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

        if(trackingMouse) {
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
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color | GL::FramebufferClear::Depth)
                          .bind();

    imgui.newFrame();
    /* Enable text input, if needed */
    if(ImGui::GetIO().WantTextInput && !isTextInputActive())
        startTextInput();
    else if(!ImGui::GetIO().WantTextInput && isTextInputActive())
        stopTextInput();

    if(loaded) {

        arcball->updateTransformation();
        Matrix4 viewTf = arcball->viewMatrix();

        if(drawPoses) {
            for(auto const& kf : keyFrames) {
                Matrix4 tf = viewTf*kf.tf.invertedRigid()*Matrix4::scaling({0.1, 0.1, 0.1});
                vertexColored.setTransformationProjectionMatrix(projection*tf)
                             .draw(axis);
            }
            //GL::Texture2D testTexture;
            //testTexture.setMagnificationFilter(GL::SamplerFilter::Linear)
            //           .setMinificationFilter(GL::SamplerFilter::Linear)
            //           .setWrapping(GL::SamplerWrapping::ClampToEdge)
            //           .setStorage(1, GL::TextureFormat::R32F, {4096, 4096});
            //glClearTexImage(testTexture.id(), 0, GL_RED, GL_FLOAT, Mg::Color4{1}.data());
            //auto sum = reduce(testTexture);
        }

        /* first render the mesh using the texture */
        phong
                .bindDiffuseTexture(texture)
                .setTransformationMatrix(viewTf)
                .setNormalMatrix(viewTf.normalMatrix())
                .setProjectionMatrix(projection)
                .setLightPositions({{-3.0f, 10.0f, 10.0f, 0}})
                        //.setLightPosition({-3, 10, 10})
                        //.setDiffuseColor(0x2f83cc_rgbf)
                .setSpecularColor(Color4{0.3})
                .setShininess(20)
                .draw(mesh);

        /* render the texture into the upper right corner ontop */
        auto vp = GL::defaultFramebuffer.viewport();
        GL::Renderer::disable(GL::Renderer::Feature::DepthTest);

        GL::defaultFramebuffer.setViewport({vp.max()*2./3., vp.max()});

        triangleShader.bindTexture(*overlay)
                      .draw(GL::Mesh{}.setCount(3));
        GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
        GL::defaultFramebuffer.setViewport(vp);
    }

    //ImGui::Begin("Viewer");
    //auto dockspaceId = ImGui::GetID("Viewer");
    //if (ImGui::DockBuilderGetNode(dockspaceId) == nullptr)
    //    presetLayout();
    drawOptions();
    //ImGui::End();

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

void Viewer::startOptimization() {
    [[maybe_unused]] bool userFailure = runOptimization(keyFrames,
                                   texture,
                                   *renderPass,
                                   [this]{ return mainLoopIteration() && isOptimizing; }
    );
    isOptimizing = false;
}

void Viewer::presetLayout() {
    auto dockspaceId = ImGui::GetID("Viewer");
    ImGui::DockBuilderRemoveNode(dockspaceId); // Clear out existing layout
    ImGui::DockBuilderAddNode(dockspaceId); // Add empty node
    ImGui::DockBuilderSetNodeSize(dockspaceId, ImGui::GetIO().DisplaySize);

    ImGuiID dock_main_id = dockspaceId; // This variable will track the document node, however we are not using it here as we aren't docking anything into it.
    ImGuiID dock_id_prop = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, NULL, &dock_main_id);

    ImGui::DockBuilderDockWindow("Options", dock_id_prop);
    ImGui::DockBuilderFinish(dockspaceId);
}


}