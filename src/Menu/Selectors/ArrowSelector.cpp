#include <BENG/Menu/Selectors/ArrowSelector.hpp>

namespace bg
{
namespace menu
{
namespace
{
const float HeightRatio = 0.5;

bg::Triangle makeTriangle(float w) {
    return bg::Triangle({0, 0}, {w, w * HeightRatio}, {0, 2.f * w * HeightRatio});
}
} // namespace

ArrowSelector::Ptr ArrowSelector::create(float w) { return Ptr(new ArrowSelector(w)); }

ArrowSelector::ArrowSelector(float w)
: width(w)
, triangle(makeTriangle(w)) {}

void ArrowSelector::render(sf::RenderTarget& target, sf::RenderStates states,
                           sf::FloatRect itemArea) const {
    triangle.setPosition(
        {itemArea.left - width - 2.f, itemArea.top + itemArea.height - width * HeightRatio});
    target.draw(triangle, states);
}

} // namespace menu
} // namespace bg