#ifndef BLIB_MENU_RENDERERS_BASICRENDERER_HPP
#define BLIB_MENU_RENDERERS_BASICRENDERER_HPP

#include <BLIB/Interfaces/Menu/Renderer.hpp>

namespace bl
{
namespace menu
{
/**
 * @brief Basic renderer that renders items with padding and alignment
 *
 * @ingroup Menu
 *
 */
class BasicRenderer : public Renderer {
public:
    enum Alignment { Left, Right, Center, Top = Left, Bottom = Right };

    /**
     * @brief Construct a new Basic Renderer object
     *
     * @param horizontalAlignment How to align items horizontally
     * @param verticalAlignment How to align items vertically
     * @param horizontalPadding Padding to place on either side of items
     * @param verticalPadding Padding to place on top and bottom of items
     */
    BasicRenderer(Alignment horizontalAlignment = Left, Alignment verticalAlignment = Top,
                  float horizontalPadding = 2.f, float verticalPadding = 2.f);

    virtual ~BasicRenderer() = default;

    /**
     * @brief Set the vertical padding
     *
     * @param padding Padding in pixels
     */
    void setVerticalPadding(float padding);

    /**
     * @brief Set the horizontal padding
     *
     * @param padding Padding in pixels
     */
    void setHorizontalPadding(float padding);

    /**
     * @brief Set the horizontal alignment of items
     *
     */
    void setHorizontalAlignment(Alignment align);

    /**
     * @brief Set the vertical alignment of items
     *
     */
    void setVerticalAlignment(Alignment align);

    /**
     * @brief Set a uniform size for all items. Items that are larger will be allowed to take the
     *        extra space required. Items that are smaller are expanded to fill the size
     *
     * @param size The minimum size for items
     */
    void setUniformSize(const sf::Vector2f& size);

    /**
     * @see Renderer::renderItem
     */
    virtual sf::Vector2f renderItem(sf::RenderTarget& target, sf::RenderStates renderStates,
                                    const Item& item, const sf::Vector2f& position,
                                    float columnWidth, float rowHeight, int x,
                                    int y) const override;

    /**
     * @see Renderer::estimateItemSize
     */
    virtual sf::Vector2f estimateItemSize(const Item& item) const override;

private:
    Alignment horizontalAlignment;
    Alignment verticalAlignment;
    sf::Vector2f padding;
    sf::Vector2f minSize;
};

} // namespace menu
} // namespace bl

#endif