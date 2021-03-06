#ifndef BLIB_GUI_ELEMENTS_RADIOBUTTON_HPP
#define BLIB_GUI_ELEMENTS_RADIOBUTTON_HPP

#include <BLIB/Interfaces/GUI/Elements/ToggleButton.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief A radio button element. Only one button in the group may be active
 *
 * @ingroup GUI
 *
 */
class RadioButton : public ToggleButton {
public:
    typedef std::shared_ptr<RadioButton> Ptr;

    /**
     * @brief A group of radio buttons. Used to disable buttons
     *
     * @ingroup GUI
     *
     */
    class Group {
    public:
        /**
         * @brief Returns the currently selected radio button. May be null
         *
         */
        RadioButton* getActiveButton();

        /**
         * @brief Set the currently active button. Deactivates the rest
         *
         * @param button
         */
        void setActiveButton(RadioButton* button);

    private:
        Group(RadioButton* owner);
        void removeButton(RadioButton* button);

        RadioButton* owner;
        RadioButton* active;
        std::vector<RadioButton*> buttons;
        friend class RadioButton;
    };

    /**
     * @brief Create a new RadioButton with a Label as it's child
     *
     * @param text The text to put to the right of the check box
     * @param radioGroup The group of buttons for selection. Leave nullptr for new group
     * @param group The group of the button
     * @param id The id of this button
     * @return Ptr The new button
     */
    static Ptr create(const std::string& text, Group* radioGroup = nullptr,
                      const std::string& group = "", const std::string& id = "");

    /**
     * @brief Create a new RadioButton with any child to its right
     *
     * @param child The element to put to the right of the check box
     * @param radioGroup The group of buttons for selection. Leave nullptr for new group
     * @param group The group of the button
     * @param id The id of this button
     * @return Ptr The new button
     */
    static Ptr create(Element::Ptr child, Group* radioGroup = nullptr,
                      const std::string& group = "", const std::string& id = "");

    /**
     * @brief Removes this button from the group
     *
     */
    virtual ~RadioButton();

    /**
     * @brief Returns the group of this RadioButton
     *
     */
    Group* getRadioGroup();

protected:
    /**
     * @brief Create a new RadioButton with any child to its right
     *
     * @param child The element to put to the right of the check box
     * @param radioGroup The group of buttons for selection. Leave nullptr for new group
     * @param group The group of the button
     * @param id The id of this button
     */
    RadioButton(Element::Ptr child, Group* radioGroup, const std::string& group,
                const std::string& id);

    /**
     * @brief Renders the unchecked and checked box
     *
     * @param activeBut The canvas to render the checked box to
     * @param inactiveBut The canvas to render the unchecked box to
     * @param renderer The renderer to use to do it
     */
    virtual void renderToggles(Canvas& activeBut, Canvas& inactiveBut,
                               const Renderer& renderer) const override;

    /**
     * @brief Activates the radio button. Does not deactivate if already active
     *
     */
    virtual void onClick() override;

private:
    Group myGroup;
    Group* rgroup;

    friend class Group;
};
} // namespace gui
} // namespace bl

#endif