#include <BLIB/Interfaces/GUI/Renderers/RendererUtil.hpp>

#include <BLIB/Interfaces/GUI.hpp>
#include <BLIB/Logging.hpp>
#include <Interfaces/GUI/Data/Font.hpp>

namespace bl
{
namespace gui
{
sf::Text RendererUtil::buildRenderText(const std::string& text, const sf::IntRect& acquisition,
                                       const RenderSettings& s, const RenderSettings& defaults) {
    RenderSettings settings = defaults;
    settings.merge(s);

    resource::Resource<sf::Font>::Ref font = settings.font.value_or(Font::get());
    if (!font) {
        BL_LOG_ERROR << "Attempting to render text with no sf::Font";
        return {};
    }

    sf::Text sfText;
    sfText.setFont(*font);
    sfText.setString(text);
    sfText.setCharacterSize(settings.characterSize.value_or(Label::DefaultFontSize));
    sfText.setFillColor(settings.fillColor.value_or(sf::Color::Black));
    sfText.setOutlineColor(settings.outlineColor.value_or(sf::Color(0, 0, 0, 90)));
    sfText.setOutlineThickness(settings.outlineThickness.value_or(0));
    sfText.setStyle(settings.style.value_or(sf::Text::Regular));

    const sf::Vector2f size(sfText.getGlobalBounds().width + sfText.getGlobalBounds().left * 2,
                            sfText.getGlobalBounds().height + sfText.getGlobalBounds().top * 2);
    sfText.setPosition(
        calculatePosition(settings.horizontalAlignment.value_or(RenderSettings::Center),
                          settings.verticalAlignment.value_or(RenderSettings::Center),
                          acquisition,
                          size));
    return sfText;
}

void RendererUtil::renderRectangle(sf::RenderTarget& target, sf::RenderStates states,
                                   const sf::IntRect& area, const RenderSettings& s,
                                   const RenderSettings& defaults) {
    RenderSettings settings = defaults;
    settings.merge(s);

    sf::RectangleShape rect({static_cast<float>(area.width), static_cast<float>(area.height)});
    rect.setPosition(area.left, area.top);
    rect.setFillColor(settings.fillColor.value_or(sf::Color(75, 75, 75)));
    rect.setOutlineThickness(-settings.outlineThickness.value_or(1));
    rect.setOutlineColor(settings.outlineColor.value_or(sf::Color(20, 20, 20)));
    target.draw(rect, states);
}

sf::Vector2f RendererUtil::calculatePosition(RenderSettings::Alignment horizontalAlignment,
                                             RenderSettings::Alignment verticalAlignment,
                                             const sf::IntRect& region, const sf::Vector2f& size) {
    sf::Vector2f position;
    switch (horizontalAlignment) {
    case RenderSettings::Left:
        position.x = region.left;
        break;
    case RenderSettings::Right:
        position.x = region.left + region.width - size.x;
        break;
    case RenderSettings::Center:
    default:
        position.x = region.left + region.width / 2 - size.x / 2;
        break;
    }

    switch (verticalAlignment) {
    case RenderSettings::Top:
        position.y = region.top;
        break;
    case RenderSettings::Bottom:
        position.y = region.top + region.height - size.y;
        break;
    case RenderSettings::Center:
    default:
        position.y = region.top + region.height / 2 - size.y / 2;
        break;
    }

    return position;
}

} // namespace gui
} // namespace bl