#ifndef BLIB_GUI_ELEMENTS_SEPARATOR_HPP
#define BLIB_GUI_ELEMENTS_SEPARATOR_HPP

#include <BLIB/Interfaces/GUI/Elements/Element.hpp>

namespace bl
{
namespace gui
{
/***
 * @brief Basic element that is represented as a line between other elements. Note that the
 *        outline thickness is used as the line thickness and fill color for the line color
 *
 * @ingroup GUI
 */
class Separator : public Element {
public:
    typedef std::shared_ptr<Separator> Ptr;

    static constexpr int DefaultThickness = 1;

    virtual ~Separator() = default;

    /// Direction the separator should be in
    enum Direction { Horizontal, Vertical };

    /**
     * @brief Create a new separator in the given direction
     *
     * @param direction The direction to orient the separator
     * @param group The group the separator is in
     * @param id The id of this separator
     * @return Ptr The new separator
     */
    static Ptr create(Direction direction, const std::string& group = "",
                      const std::string& id = "");

    /**
     * @brief Returns the direction the separator is facing
     *
     */
    Direction getDirection() const;

protected:
    /**
     * @brief Create a new separator in the given direction
     *
     * @param direction The direction to orient the separator
     * @param group The group the separator is in
     * @param id The id of this separator
     */
    Separator(Direction direction, const std::string& group, const std::string& id);

    /**
     * @brief Returns the the outline thickness in both the x and y components
     *
     */
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief Renders the separator
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

private:
    const Direction dir;
};

} // namespace gui
} // namespace bl

#endif