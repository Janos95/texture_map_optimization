//
// Created by janos on 08.11.19.
//

#include "viewer.h"

#include <Magnum/GL/DefaultFramebuffer.h>

using namespace Corrade;
using namespace Magnum;

Viewer::Viewer(const Arguments& arguments):
        Platform::Application{arguments, Configuration{}
                .setTitle("Viewer")
                .setWindowFlags(Configuration::WindowFlag::Resizable)}
{
    /* Every scene needs a camera */
    m_cameraObject
            .setParent(&m_scene)
            .translate(Vector3::zAxis(5.0f));
    (*(m_camera = new SceneGraph::Camera3D{m_cameraObject}))
            .setAspectRatioPolicy(SceneGraph::AspectRatioPolicy::Extend)
            .setProjectionMatrix(Matrix4::perspectiveProjection(35.0_degf, 1.0f, 0.01f, 1000.0f))
            .setViewport(GL::defaultFramebuffer.viewport().size());

    /* Base object, parent of all (for easy manipulation) */
    m_manipulator.setParent(&m_scene);

    /* Setup renderer and shader defaults */
    GL::Renderer::enable(GL::Renderer::Feature::DepthTest);
    GL::Renderer::enable(GL::Renderer::Feature::FaceCulling);

    _coloredShader
            .setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0xffffff_rgbf)
            .setShininess(80.0f);
    _texturedShader
            .setAmbientColor(0x111111_rgbf)
            .setSpecularColor(0x111111_rgbf)
            .setShininess(80.0f);
}



void Viewer::drawEvent() {
    GL::defaultFramebuffer.clear(GL::FramebufferClear::Color|GL::FramebufferClear::Depth);
    m_camera->draw(m_drawables);
    swapBuffers();
}


void Viewer::viewportEvent(ViewportEvent& event) {
    GL::defaultFramebuffer.setViewport({{}, event.framebufferSize()});
    m_camera->setViewport(event.windowSize());
}

void Viewer::mousePressEvent(MouseEvent& event) {
    if(event.button() == MouseEvent::Button::Left)
        m_previousPosition = positionOnSphere(event.position());
}

void Viewer::mouseReleaseEvent(MouseEvent& event) {
    if(event.button() == MouseEvent::Button::Left)
        m_previousPosition = Vector3();
}

void Viewer::mouseScrollEvent(MouseScrollEvent& event) {
    if(!event.offset().y()) return;

    /* Distance to origin */
    const Float distance = m_cameraObject.transformation().translation().z();

    /* Move 15% of the distance back or forward */
    m_cameraObject.translate(Vector3::zAxis(
            distance*(1.0f - (event.offset().y() > 0 ? 1/0.85f : 0.85f))));

    redraw();
}

Vector3 Viewer::positionOnSphere(const Vector2i& position) const {
    const Vector2 positionNormalized = Vector2{position}/Vector2{m_camera->viewport()} - Vector2{0.5f};
    const Float length = positionNormalized.length();
    const Vector3 result(length > 1.0f ? Vector3(positionNormalized, 0.0f) : Vector3(positionNormalized, 1.0f - length));
    return (result*Vector3::yScale(-1.0f)).normalized();
}

void Viewer::mouseMoveEvent(MouseMoveEvent& event) {
    if(!(event.buttons() & MouseMoveEvent::Button::Left)) return;

    const Vector3 currentPosition = positionOnSphere(event.position());
    const Vector3 axis = Math::cross(m_previousPosition, currentPosition);

    if(m_previousPosition.length() < 0.001f || axis.length() < 0.001f) return;

    m_manipulator.rotate(Math::angle(m_previousPosition, currentPosition), axis.normalized());
    m_previousPosition = currentPosition;

    redraw();
}