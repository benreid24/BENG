#ifndef BLIB_GUI_ELEMENTS_NOTEBOOK_HPP
#define BLIB_GUI_ELEMENTS_NOTEBOOK_HPP

#include <BLIB/Interfaces/GUI/Elements/Container.hpp>

#include <BLIB/Interfaces/GUI/Elements/Box.hpp>
#include <BLIB/Interfaces/GUI/Elements/Label.hpp>

namespace bl
{
namespace gui
{
/**
 * @brief A notebook with tabs and pages. The tabs are horizontally positioned across the top
 *        of the element. Each page is typically a Box. Allows more sophisticated UI's
 *
 * @ingroup GUI
 *
 */
class Notebook : public Container {
public:
    typedef std::shared_ptr<Notebook> Ptr;
    typedef std::function<void()> PageChangedCb;

    virtual ~Notebook();

    /**
     * @brief Create a new empty Notebook
     *
     * @param group The group the notebook is in
     * @param id The id of this notebook
     * @return Ptr The new Notebook
     */
    static Ptr create(const std::string& group = "", const std::string& id = "");

    /**
     * @brief Container struct representing a page in the notebook
     *
     */
    struct Page {
        /// The internal name of the page
        const std::string name;

        /// The label at the top of the notebook
        Label::Ptr label;

        /// Any element that is the actual page content
        Element::Ptr content;

        /// Callback to trigger when the page is opened
        PageChangedCb onOpen;

        /// Callback to trigger when the page is closed
        PageChangedCb onClose;

    private:
        Page(const std::string& name, Label::Ptr label, Element::Ptr content,
             const PageChangedCb& onOpen, const PageChangedCb& onClose);

        friend class Notebook;
    };

    /**
     * @brief Add a new page to the Notebook
     *
     * @param name The name of the page. This is not visible anywhere
     * @param title The title to put in the button
     * @param content The content to put in the notebook when the page is selected
     * @param onOpen Callback to trigger when this page is selected
     * @param onClose Callback to trigger when this page is left
     */
    void addPage(
        const std::string& name, const std::string& title, Element::Ptr content,
        const PageChangedCb& onOpen = []() {}, const PageChangedCb& onClose = []() {});

    /**
     * @brief Returns the active page itself
     *
     */
    Page* getActivePage() const;

    /**
     * @brief Returns the index of the active page
     *
     */
    unsigned int getActivePageIndex() const;

    /**
     * @brief Returns the number of pages in the Notebook
     *
     */
    unsigned int pageCount() const;

    /**
     * @brief Returns a real only reference to all the pages. Useful for Renderers
     *
     */
    const std::vector<Page*>& getPages() const;

    /**
     * @brief Returns the name of the active page
     *
     */
    const std::string& getActivePageName() const;

    /**
     * @brief Returns a pointer to the page at the given index
     *
     * @param index Index of the page to access. 0 based
     * @return Page* Pointer to the page requested. May be null if out of bounds
     */
    Page* getPageByIndex(unsigned int index);

    /**
     * @brief Returns a pointer to the page with the given name
     *
     * @param name Name of the page to fetch
     * @return Page* Pointer to the page requested. May be null if name not found
     */
    Page* getPageByName(const std::string& name);

    /**
     * @brief Deletes the page at the given index
     *
     */
    void removePageByIndex(unsigned int index);

    /**
     * @brief Removes the page with the given name
     *
     */
    void removePageByName(const std::string& name);

    /**
     * @brief Returns the acquisition of the tabs for the notebook
     *
     */
    const sf::IntRect& getTabAcquisition() const;

protected:
    /**
     * @brief Create a new empty Notebook
     *
     * @param group The group the notebook is in
     * @param id The id of this notebook
     */
    Notebook(const std::string& group, const std::string& id);

    /**
     * @brief Computes space required across all tabs
     *
     */
    virtual sf::Vector2i minimumRequisition() const override;

    /**
     * @brief Repacks the tabs and their content
     *
     */
    virtual void onAcquisition() override;

    /**
     * @brief Renders the notebook
     *
     * @param target The target to render to
     * @param states Render states to apply
     * @param renderer The renderer to use
     */
    virtual void doRender(sf::RenderTarget& target, sf::RenderStates states,
                          const Renderer& renderer) const override;

private:
    Box::Ptr tabArea;
    sf::IntRect contentArea;
    std::vector<Page*> pages;
    std::map<std::string, std::pair<unsigned int, Page*>> pageMap;
    unsigned int activePage;

    void addChildren();
    void pageClicked(Page* page);
    void makePageActive(unsigned int index);
};
} // namespace gui
} // namespace bl

#endif