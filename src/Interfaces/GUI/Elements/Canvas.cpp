#include <BLIB/Interfaces/GUI/Elements/Canvas.hpp>

namespace bl
{
namespace gui
{
Canvas::Ptr Canvas::create(unsigned int w, unsigned int h, const std::string& group,
                           const std::string& id) {
    return Ptr(new Canvas(w, h, group, id));
}

Canvas::Canvas(unsigned int w, unsigned int h, const std::string& group, const std::string& id)
: Element(group, id)
, fillAcq(false)
, maintainAR(true) {
    texture.create(w, h);
    texture.clear(sf::Color::Transparent);
    sprite.setTexture(texture.getTexture());
    getSignal(Action::AcquisitionChanged).willAlwaysCall(std::bind(&Canvas::setScale, this));
}

void Canvas::resize(unsigned int w, unsigned int h, bool resetScale) {
    texture.create(w, h);
    texture.clear(sf::Color::Transparent);
    sprite.setTexture(texture.getTexture());
    if (resetScale) size.reset();
    setScale();
    makeDirty();
}

void Canvas::scaleToSize(const sf::Vector2f& s, bool md) {
    size = s;
    setScale();
    if (md) makeDirty();
}

void Canvas::setFillAcquisition(bool fill, bool mar) {
    fillAcq    = fill;
    maintainAR = mar;
    setScale();
    makeDirty();
}

sf::RenderTexture& Canvas::getTexture() { return texture; }

sf::Vector2i Canvas::minimumRequisition() const {
    return static_cast<sf::Vector2i>(
        size.value_or(sf::Vector2f(texture.getSize().x, texture.getSize().y)));
}

void Canvas::doRender(sf::RenderTarget& target, sf::RenderStates states,
                      const Renderer& renderer) const {
    renderer.renderImage(target, states, this, sprite);
}

void Canvas::setScale() {
    const sf::Vector2f origSize(texture.getSize().x, texture.getSize().y);
    sf::Vector2f area = size.value_or(origSize);
    if (fillAcq) {
        area.x = getAcquisition().width;
        area.y = getAcquisition().height;
    }
    float sx = area.x / origSize.x;
    float sy = area.y / origSize.y;
    if (fillAcq && maintainAR) {
        sx = std::min(sx, sy);
        sy = sx;
    }
    sprite.setScale(sx, sy);
}

} // namespace gui
} // namespace bl