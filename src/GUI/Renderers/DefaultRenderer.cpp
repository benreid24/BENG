#include <BLIB/GUI/Renderers/DefaultRenderer.hpp>

#include <BLIB/GUI.hpp>
#include <GUI/Data/Font.hpp>
#include <cmath>
#include <iostream>

namespace bl
{
namespace gui
{
namespace
{
RenderSettings getWindowDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color(75, 75, 75);
    settings.outlineColor     = sf::Color(20, 20, 20);
    settings.outlineThickness = 1;
    return settings;
}

RenderSettings getTitlebarDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color(95, 95, 95);
    settings.outlineColor     = sf::Color(20, 20, 20);
    settings.outlineThickness = 1;
    return settings;
}

RenderSettings getButtonDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color(70, 70, 70);
    settings.outlineColor     = sf::Color::Black;
    settings.outlineThickness = Button::DefaultOutlineThickness;
    return settings;
}

RenderSettings getSliderDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color(120, 120, 120);
    settings.outlineColor     = sf::Color(85, 85, 85);
    settings.outlineThickness = 1;
    return settings;
}

RenderSettings getContainerDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color::Transparent;
    settings.outlineColor     = sf::Color::Transparent;
    settings.outlineThickness = 0;
    return settings;
}

RenderSettings getImageDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color::Transparent;
    settings.outlineColor     = sf::Color::Transparent;
    settings.outlineThickness = 0;
    return settings;
}

RenderSettings getLabelDefaults() {
    RenderSettings settings;
    settings.characterSize       = Label::DefaultFontSize;
    settings.fillColor           = sf::Color::Black;
    settings.outlineColor        = sf::Color(0, 0, 0, 90);
    settings.outlineThickness    = 0;
    settings.style               = sf::Text::Regular;
    settings.horizontalAlignment = RenderSettings::Center;
    settings.verticalAlignment   = RenderSettings::Center;
    return settings;
}

RenderSettings getProgressBarDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color(120, 120, 120);
    settings.outlineColor     = sf::Color(20, 20, 20);
    settings.outlineThickness = 1;
    return settings;
}

RenderSettings getProgressBarSecondaryDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color(114, 219, 72);
    settings.outlineColor     = sf::Color::Transparent;
    settings.outlineThickness = 0;
    return settings;
}

RenderSettings getTextEntryBoxDefaults() {
    RenderSettings settings;
    settings.fillColor        = sf::Color::White;
    settings.outlineColor     = sf::Color::Black;
    settings.outlineThickness = 1;
    return settings;
}

RenderSettings getTextEntryTextDefaults() {
    RenderSettings settings;
    settings.fillColor           = sf::Color::Black;
    settings.outlineColor        = sf::Color::Transparent;
    settings.outlineThickness    = 0;
    settings.style               = sf::Text::Regular;
    settings.horizontalAlignment = RenderSettings::Left;
    settings.verticalAlignment   = RenderSettings::Top;
    settings.font                = Font::get();
    return settings;
}

} // namespace

DefaultRenderer::Ptr DefaultRenderer::create() { return Ptr(new DefaultRenderer()); }

void DefaultRenderer::renderCustom(sf::RenderTarget& target, sf::RenderStates states,
                                   const Element& element) const {
    std::cerr << "Error: renderCustom() called on default renderer. Use a custom renderer\n";
}

void DefaultRenderer::renderBox(sf::RenderTarget& target, sf::RenderStates states,
                                const Container& container) const {
    const RenderSettings settings        = getSettings(&container);
    static const RenderSettings defaults = getContainerDefaults();
    RendererUtil::renderRectangle(
        target, states, container.getAcquisition(), settings, defaults);
}

void DefaultRenderer::renderButton(sf::RenderTarget& target, sf::RenderStates states,
                                   const Button& button) const {
    const RenderSettings settings        = getSettings(&button);
    static const RenderSettings defaults = getButtonDefaults();
    RendererUtil::renderRectangle(target, states, button.getAcquisition(), settings, defaults);
}

void DefaultRenderer::renderMouseoverOverlay(sf::RenderTarget& target, sf::RenderStates states,
                                             const Element* element) const {
    sf::RectangleShape rect({static_cast<float>(element->getAcquisition().width),
                             static_cast<float>(element->getAcquisition().height)});
    rect.setPosition(element->getAcquisition().left, element->getAcquisition().top);
    if (element->mouseOver() || element->leftPressed()) {
        rect.setOutlineThickness(0);
        if (element->leftPressed())
            rect.setFillColor(sf::Color(30, 30, 30, 100));
        else
            rect.setFillColor(sf::Color(255, 255, 255, 100));
        target.draw(rect, states);
    }
}

void DefaultRenderer::renderNotebook(sf::RenderTarget& target, sf::RenderStates states,
                                     const Notebook& nb) const {
    // TODO - render the notebook
}

void DefaultRenderer::renderProgressBar(sf::RenderTarget& target, sf::RenderStates states,
                                        const ProgressBar& bar) const {
    RenderSettings settings               = getSettings(&bar);
    static const RenderSettings defaults  = getProgressBarDefaults();
    static const RenderSettings sdefaults = getProgressBarSecondaryDefaults();

    sf::IntRect rect = bar.getAcquisition();
    RendererUtil::renderRectangle(target, states, rect, settings, defaults);
    const float xs = static_cast<float>(rect.width) * bar.getProgress();
    const float ys = static_cast<float>(rect.height) * bar.getProgress();
    if (bar.getFillDirection() == ProgressBar::LeftToRight)
        rect.width = xs;
    else if (bar.getFillDirection() == ProgressBar::TopToBottom)
        rect.height = ys;
    else if (bar.getFillDirection() == ProgressBar::LeftToRight) {
        rect.left += rect.width - xs;
        rect.width = xs;
    }
    else {
        rect.top += rect.height - ys;
        rect.height = ys;
    }
    const int spacing =
        settings.secondaryOutlineThickness.value_or(1) + settings.outlineThickness.value_or(1);
    rect.left += spacing;
    rect.top += spacing;
    rect.width -= spacing * 2;
    rect.height -= spacing * 2;
    settings.promoteSecondaries();
    RendererUtil::renderRectangle(target, states, rect, settings, sdefaults);
}

void DefaultRenderer::renderSeparator(sf::RenderTarget& target, sf::RenderStates states,
                                      const Separator& sep) const {
    const RenderSettings settings = getSettings(&sep);
    const float thickness = settings.outlineThickness.value_or(Separator::DefaultThickness);
    sf::Vector2f lineSize = {static_cast<float>(sep.getAcquisition().width), thickness};
    if (sep.getDirection() == Separator::Vertical)
        lineSize = {thickness, static_cast<float>(sep.getAcquisition().height)};
    const sf::Vector2f pos = RendererUtil::calculatePosition(
        settings.horizontalAlignment.value_or(RenderSettings::Center),
        settings.verticalAlignment.value_or(RenderSettings::Center),
        sep.getAcquisition(),
        lineSize);

    sf::RectangleShape rect(lineSize);
    rect.setPosition(pos);
    rect.setFillColor(settings.fillColor.value_or(sf::Color::Black));
    target.draw(rect, states);
}

void DefaultRenderer::renderSlider(sf::RenderTarget& target, sf::RenderStates states,
                                   const Slider& slider) const {
    const RenderSettings settings        = getSettings(&slider);
    static const RenderSettings defaults = getSliderDefaults();
    RendererUtil::renderRectangle(target, states, slider.getAcquisition(), settings, defaults);
}

void DefaultRenderer::renderSliderButton(sf::RenderTexture& texture, bool hor,
                                         bool increasing) const {
    texture.clear(sf::Color::Transparent);

    const float cosRatio      = cos(45.f / 180.f * 3.1415f);
    const sf::Vector2f center = static_cast<sf::Vector2f>(texture.getSize()) / 2.f;
    const float halfWidth     = static_cast<float>(texture.getSize().x) * 0.20f;
    const float rectWidth     = halfWidth / cosRatio;
    const float rectHeight    = static_cast<float>(texture.getSize().y) * 0.08f;

    sf::RenderTexture subtexture;
    subtexture.create(texture.getSize().x, texture.getSize().y);

    sf::RectangleShape rect({rectWidth, rectHeight});
    rect.setOrigin(rectWidth / 2, rectHeight / 2);
    rect.setFillColor(sf::Color::Black);
    rect.setRotation(45);
    rect.setPosition(center.x + halfWidth / 2 * cosRatio, center.y * 1.1);
    subtexture.draw(rect);
    rect.setRotation(-45);
    rect.setPosition(center.x - halfWidth / 2 * cosRatio, center.y * 1.1);
    subtexture.draw(rect);

    sf::Sprite sprite(subtexture.getTexture());
    sprite.setOrigin(center);
    sprite.setPosition(center);
    if (hor) {
        if (!increasing)
            sprite.setRotation(90); // right
        else
            sprite.setRotation(270); // left
    }
    else if (!increasing)
        sprite.setRotation(180); // down

    texture.draw(sprite);
    texture.display();
}

void DefaultRenderer::renderLabel(sf::RenderTarget& target, sf::RenderStates states,
                                  const Label& label) const {
    static const RenderSettings defaults = getLabelDefaults();
    target.draw(RendererUtil::buildRenderText(
                    label.getText(), label.getAcquisition(), getSettings(&label), defaults),
                states);
}

void DefaultRenderer::renderTextEntry(sf::RenderTarget& target, sf::RenderStates states,
                                      const TextEntry& entry) const {
    RenderSettings settings                  = getSettings(&entry);
    static const RenderSettings boxDefaults  = getTextEntryBoxDefaults();
    static const RenderSettings textDefaults = getTextEntryTextDefaults();

    RendererUtil::renderRectangle(
        target, states, entry.getAcquisition(), settings, boxDefaults);

    sf::IntRect textArea = entry.getAcquisition();
    const int thickness  = settings.outlineThickness.value_or(2);
    textArea.left += thickness;
    textArea.top += thickness;
    textArea.width -= thickness * 2;
    textArea.height -= thickness * 2;
    settings.promoteSecondaries();
    sf::Text text =
        RendererUtil::buildRenderText(entry.getInput(), textArea, settings, textDefaults);
    text.setPosition(text.getPosition().x, text.getPosition().y - text.getLocalBounds().top);
    target.draw(text, states);

    if (entry.cursorVisible()) {
        const sf::Vector2f pos = text.findCharacterPos(entry.getCursorPosition());
        sf::RectangleShape carat(
            {1.5, text.getFont()->getLineSpacing(text.getCharacterSize())});
        carat.setPosition(pos);
        carat.setFillColor(sf::Color::Black);
        target.draw(carat, states);
    }
}

void DefaultRenderer::renderWindow(sf::RenderTarget& target, sf::RenderStates states,
                                   const Container* titlebar, const Window& window) const {
    const RenderSettings settings        = getSettings(&window);
    static const RenderSettings defaults = getWindowDefaults();

    const sf::FloatRect area = static_cast<sf::FloatRect>(window.getAcquisition());

    RendererUtil::renderRectangle(target, states, window.getAcquisition(), settings, defaults);
    if (titlebar) {
        const RenderSettings settings        = getSettings(titlebar);
        static const RenderSettings defaults = getTitlebarDefaults();
        RendererUtil::renderRectangle(target,
                                      states,
                                      {window.getAcquisition().left,
                                       window.getAcquisition().top,
                                       titlebar->getAcquisition().width,
                                       titlebar->getAcquisition().height},
                                      settings,
                                      defaults);
    }
}

void DefaultRenderer::renderImage(sf::RenderTarget& target, sf::RenderStates states,
                                  const Element* e, const sf::Sprite& image) const {
    const RenderSettings settings        = getSettings(e);
    static const RenderSettings defaults = getImageDefaults();

    const sf::Vector2f size(image.getGlobalBounds().width, image.getGlobalBounds().height);
    const sf::Vector2f pos = RendererUtil::calculatePosition(
        settings.horizontalAlignment.value_or(RenderSettings::Center),
        settings.verticalAlignment.value_or(RenderSettings::Center),
        e->getAcquisition(),
        size);
    const sf::IntRect region(static_cast<sf::Vector2i>(pos), static_cast<sf::Vector2i>(size));

    RendererUtil::renderRectangle(target, states, region, settings, defaults);
    states.transform.translate(pos);
    target.draw(image, states);
}

} // namespace gui
} // namespace bl