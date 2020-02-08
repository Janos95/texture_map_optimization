//
// Created by janos on 08.11.19.
//

#include "viewer.hpp"

#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/Math/FunctionsBatch.h>

using namespace Corrade;
using namespace Magnum;

using namespace Math::Literals;


Viewer::Viewer():
        Platform::Application{{m_dummy,nullptr}, NoCreate}
{
    /* Try 8x MSAA, fall back to zero samples if not possible. Enable only 2x
       MSAA if we have enough DPI. */
    {
        const Vector2 dpiScaling = this->dpiScaling({});
        Configuration conf;
        conf.setTitle("Magnum Mouse Interaction Example")
                .setSize(conf.size(), dpiScaling);
        GLConfiguration glConf;
        glConf.setSampleCount(dpiScaling.max() < 2.0f ? 8 : 2);
        if(!tryCreate(conf, glConf))
            create(conf, glConf.setSampleCount(0));
    }

    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);


    /* Set up the camera */
    auto& camera = scene.camera();
    auto& cameraObject = scene.cameraObject();
    cameraObject.resetTransformation()
            .translate(Vector3::zAxis(5.0f))
            .rotateX(-15.0_degf)
            .rotateY(30.0_degf);
    camera.setProjectionMatrix(Matrix4::perspectiveProjection(
            45.0_degf, Vector2{windowSize()}.aspectRatio(), 0.01f, 100.0f));

    /* Initialize initial depth to the value at scene center */
    m_lastDepth = ((camera.projectionMatrix()*camera.cameraMatrix()).transformPoint({}).z() + 1.0f)*0.5f;
}


Float Viewer::depthAt(const Vector2i& windowPosition) {
    /* First scale the position from being relative to window size to being
       relative to framebuffer size as those two can be different on HiDPI
       systems */
    const Vector2i position = windowPosition*Vector2{framebufferSize()}/Vector2{windowSize()};
    const Vector2i fbPosition{position.x(), GL::defaultFramebuffer.viewport().sizeY() - position.y() - 1};

    GL::defaultFramebuffer.mapForRead(GL::DefaultFramebuffer::ReadAttachment::Front);
    Image2D data = GL::defaultFramebuffer.read(
            Range2Di::fromSize(fbPosition, Vector2i{1}).padded(Vector2i{2}),
            {GL::PixelFormat::DepthComponent, GL::PixelType::Float});

    return Math::min<Float>(Containers::arrayCast<const Float>(data.data()));
}

Vector3 Viewer::unproject(const Vector2i& windowPosition, Float depth) const {
    /* We have to take window size, not framebuffer size, since the position is
       in window coordinates and the two can be different on HiDPI systems */
    const Vector2i viewSize = windowSize();
    const Vector2i viewPosition{windowPosition.x(), viewSize.y() - windowPosition.y() - 1};
    const Vector3 in{2*Vector2{viewPosition}/Vector2{viewSize} - Vector2{1.0f}, depth*2.0f - 1.0f};

    /*
        Use the following to get global coordinates instead of camera-relative:

        (m_scene.cameraObject().absoluteTransformationMatrix()*_camera->projectionMatrix().inverted()).transformPoint(in)
    */
    return scene.camera().projectionMatrix().inverted().transformPoint(in);
}




void Viewer::keyPressEvent(KeyEvent& event) {
    /* Reset the transformation to the original view */
    if(event.key() == KeyEvent::Key::NumZero) {
        scene.cameraObject()
                .resetTransformation()
                .translate(Vector3::zAxis(5.0f))
                .rotateX(-15.0_degf)
                .rotateY(30.0_degf);
        redraw();
        return;

        /* Axis-aligned view */
    } else if(event.key() == KeyEvent::Key::NumOne ||
              event.key() == KeyEvent::Key::NumThree ||
              event.key() == KeyEvent::Key::NumSeven)
    {
        /* Start with current camera translation with the rotation inverted */
        const Vector3 viewTranslation = scene.cameraObject().transformation().rotationScaling().inverted()*scene.cameraObject().transformation().translation();

        /* Front/back */
        const Float multiplier = event.modifiers() & KeyEvent::Modifier::Ctrl ? -1.0f : 1.0f;

        Matrix4 transformation;
        if(event.key() == KeyEvent::Key::NumSeven) /* Top/bottom */
            transformation = Matrix4::rotationX(-90.0_degf*multiplier);
        else if(event.key() == KeyEvent::Key::NumOne) /* Front/back */
            transformation = Matrix4::rotationY(90.0_degf - 90.0_degf*multiplier);
        else if(event.key() == KeyEvent::Key::NumThree) /* Right/left */
            transformation = Matrix4::rotationY(90.0_degf*multiplier);
        else CORRADE_ASSERT_UNREACHABLE();

        scene.cameraObject().setTransformation(transformation*Matrix4::translation(viewTranslation));
        redraw();
    }
}

void Viewer::mousePressEvent(MouseEvent& event) {
    /* Due to compatibility reasons, scroll is also reported as a press event,
       so filter that out. Could be removed once MouseEvent::Button::Wheel is
       gone from Magnum. */
    if(event.button() != MouseEvent::Button::Left &&
       event.button() != MouseEvent::Button::Middle)
        return;

    const Float currentDepth = depthAt(event.position());
    const Float depth = currentDepth == 1.0f ? m_lastDepth : currentDepth;
    m_translationPoint = unproject(event.position(), depth);
    /* Update the rotation point only if we're not zooming against infinite
       depth or if the original rotation point is not yet initialized */
    if(currentDepth != 1.0f || m_rotationPoint.isZero()) {
        m_rotationPoint = m_translationPoint;
        m_lastDepth = depth;
    }
}

void Viewer::mouseMoveEvent(MouseMoveEvent& event) {
    if(m_lastPosition == Vector2i{-1}) m_lastPosition = event.position();
    const Vector2i delta = event.position() - m_lastPosition;
    m_lastPosition = event.position();

    if(!event.buttons()) return;

    /* Translate */
    if(event.modifiers() & MouseMoveEvent::Modifier::Shift) {
        const Vector3 p = unproject(event.position(), m_lastDepth);
        scene.cameraObject().translateLocal(m_translationPoint - p); /* is Z always 0? */
        m_translationPoint = p;

        /* Rotate around rotation point */
    } else scene.cameraObject().transformLocal(
                Matrix4::translation(m_rotationPoint)*
                Matrix4::rotationX(-0.01_radf*delta.y())*
                Matrix4::rotationY(-0.01_radf*delta.x())*
                Matrix4::translation(-m_rotationPoint));

    redraw();
}

void Viewer::mouseScrollEvent(MouseScrollEvent& event) {
    const Float currentDepth = depthAt(event.position());
    const Float depth = currentDepth == 1.0f ? m_lastDepth : currentDepth;
    const Vector3 p = unproject(event.position(), depth);
    /* Update the rotation point only if we're not zooming against infinite
       depth or if the original rotation point is not yet initialized */
    if(currentDepth != 1.0f || m_rotationPoint.isZero()) {
        m_rotationPoint = p;
        m_lastDepth = depth;
    }

    const Float direction = event.offset().y();
    if(!direction) return;

    /* Move towards/backwards the rotation point in cam coords */
    scene.cameraObject().translateLocal(m_rotationPoint*direction*0.1f);

    event.setAccepted();
    redraw();
}

void Viewer::drawEvent() {
    for(auto& cb: callbacks)
        cb(scene);

    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);

    scene.camera().draw(scene.m_drawableGroup);

    swapBuffers();
}

