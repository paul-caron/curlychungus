#pragma once
#include <string>
#include <vector>
#include <optional>
#include "json.hpp"
#include "curling.hpp"

/**
 * @class WebDriverClient
 * @brief A C++ client for interacting with a WebDriver server (e.g., Selenium, GeckoDriver).
 */
class WebDriverClient {
public:
    /**
     * @brief Constructs a WebDriver client with a given remote URL and optional user agent.
     * @param remoteUrl The base URL of the WebDriver server.
     * @param agent The user agent to use for requests (default: Firefox).
     */
    explicit WebDriverClient(std::string remoteUrl, curling::UserAgent agent = curling::UserAgent::Firefox);

    // --- Session management ---

    /**
     * @brief Creates a new WebDriver session.
     * @param caps Desired capabilities for the session.
     * @return The session ID string.
     */
    std::string createSession(const nlohmann::json& caps = {{"capabilities", {{"alwaysMatch", {{"browserName", "firefox"}}}}}});

    /**
     * @brief Deletes the current WebDriver session.
     */
    void deleteSession();

    // --- Navigation ---

    /**
     * @brief Gets the status of the WebDriver server.
     * @return A JSON object containing status information.
     */
    nlohmann::json getStatus();

    /**
     * @brief Navigates the browser to the given URL.
     * @param url The URL to navigate to.
     */
    void navigateTo(const std::string& url);

    /**
     * @brief Gets the current URL of the browser.
     * @return The current URL as a string.
     */
    std::string getCurrentUrl();

    /**
     * @brief Navigates one step backward in the browser history.
     */
    void back();

    /**
     * @brief Navigates one step forward in the browser history.
     */
    void forward();

    /**
     * @brief Reloads the current page.
     */
    void refresh();

    /**
     * @brief Retrieves the page title.
     * @return The page title as a string.
     */
    std::string getTitle();

    /**
     * @brief Sets timeout durations.
     * @param timeouts A JSON object with timeout values (e.g., "implicit", "pageLoad").
     */
    void setTimeouts(const nlohmann::json& timeouts);

    /**
     * @brief Retrieves the currently configured timeouts.
     * @return A JSON object with timeout values.
     */
    nlohmann::json getTimeouts();

    // --- Window & Frame ---

    /**
     * @brief Gets the current window handle.
     * @return The window handle string.
     */
    std::string getWindowHandle();

    /**
     * @brief Retrieves all open window handles.
     * @return A vector of window handle strings.
     */
    std::vector<std::string> getWindowHandles();

    /**
     * @brief Closes the current browser window.
     */
    void closeWindow();

    /**
     * @brief Switches focus to a different window.
     * @param handle The handle of the window to switch to.
     */
    void switchWindow(const std::string& handle);

    /**
     * @brief Retrieves the dimensions and position of the current window.
     * @return A JSON object describing the window rectangle.
     */
    nlohmann::json getWindowRect();

    /**
     * @brief Sets the dimensions and position of the window.
     * @param rect A JSON object with "x", "y", "width", and "height" keys.
     */
    void setWindowRect(const nlohmann::json& rect);

    /**
     * @brief Maximizes the current window.
     */
    void maximizeWindow();

    /**
     * @brief Minimizes the current window.
     */
    void minimizeWindow();

    /**
     * @brief Puts the window into fullscreen mode.
     */
    void fullscreenWindow();

    /**
     * @brief Switches focus to a specific frame.
     * @param id The frame ID or element (nullable).
     */
    void switchFrame(const nlohmann::json& id = nullptr);

    /**
     * @brief Switches focus back to the parent frame.
     */
    void switchToParentFrame();

    // --- Element interaction ---

    /**
     * @brief Finds a single element on the page.
     * @param using_ The strategy to use (e.g., "css selector").
     * @param value The value to search for.
     * @return The element ID.
     */
    std::string findElement(const std::string& using_, const std::string& value);

    /**
     * @brief Finds multiple elements on the page.
     * @param using_ The strategy to use.
     * @param value The value to search for.
     * @return A vector of element IDs.
     */
    std::vector<std::string> findElements(const std::string& using_, const std::string& value);

    /**
     * @brief Finds a single child element within a parent element.
     * @param eid The ID of the parent element.
     * @param using_ The strategy to use.
     * @param value The value to search for.
     * @return The child element ID.
     */
    std::string findChildElement(const std::string& eid, const std::string& using_, const std::string& value);

    /**
     * @brief Finds multiple child elements within a parent element.
     * @param eid The ID of the parent element.
     * @param using_ The strategy to use.
     * @param value The value to search for.
     * @return A vector of child element IDs.
     */
    std::vector<std::string> findChildElements(const std::string& eid, const std::string& using_, const std::string& value);

    /**
     * @brief Gets the value of an element attribute.
     * @param eid The element ID.
     * @param name The attribute name.
     * @return The attribute value.
     */
    std::string getElementAttribute(const std::string& eid, const std::string& name);

    /**
     * @brief Gets the value of an element property.
     * @param eid The element ID.
     * @param name The property name.
     * @return The property value.
     */
    std::string getElementProperty(const std::string& eid, const std::string& name);

    /**
     * @brief Retrieves the visible text content of an element.
     * @param eid The element ID.
     * @return The text content.
     */
    std::string getElementText(const std::string& eid);

    /**
     * @brief Gets the tag name of an element.
     * @param eid The element ID.
     * @return The tag name.
     */
    std::string getElementTagName(const std::string& eid);

    /**
     * @brief Checks if an element is selected.
     * @param eid The element ID.
     * @return True if selected, false otherwise.
     */
    bool isElementSelected(const std::string& eid);

    /**
     * @brief Checks if an element is enabled.
     * @param eid The element ID.
     * @return True if enabled, false otherwise.
     */
    bool isElementEnabled(const std::string& eid);

    /**
     * @brief Gets the bounding rectangle of an element.
     * @param eid The element ID.
     * @return A JSON object with "x", "y", "width", and "height".
     */
    nlohmann::json getElementRect(const std::string& eid);

    /**
     * @brief Simulates a click on an element.
     * @param eid The element ID.
     */
    void clickElement(const std::string& eid);

    /**
     * @brief Clears the content of an input element.
     * @param eid The element ID.
     */
    void clearElement(const std::string& eid);

    /**
     * @brief Sends text input to an element.
     * @param eid The element ID.
     * @param text The text to input.
     */
    void sendKeys(const std::string& eid, const std::string& text);

    /**
     * @brief Sends text input one character at a time, simulating slow typing.
     * @param eid The element ID.
     * @param text The text to input.
     * @param baseDelayMs Delay in milliseconds between each character (default: 100ms).
     */
    void sendKeysSlowly(const std::string& eid, const std::string& text, unsigned baseDelayMs = 100);

    // --- Script execution ---

    /**
     * @brief Executes a JavaScript script in the context of the page.
     * @param script The JavaScript code.
     * @param args Arguments passed to the script (as JSON array).
     * @return Result of the script execution.
     */
    nlohmann::json executeScript(const std::string& script, const nlohmann::json& args = nlohmann::json::array());

    /**
     * @brief Executes an asynchronous JavaScript script.
     * @param script The JavaScript code.
     * @param args Arguments passed to the script (as JSON array).
     * @return Result of the script execution.
     */
    nlohmann::json executeAsyncScript(const std::string& script, const nlohmann::json& args = {});

    // --- Cookies ---

    /**
     * @brief Retrieves all cookies visible to the current page.
     * @return A vector of cookie JSON objects.
     */
    std::vector<nlohmann::json> getAllCookies();

    /**
     * @brief Retrieves a single cookie by name.
     * @param name The name of the cookie.
     * @return The cookie as a JSON object.
     */
    nlohmann::json getCookie(const std::string& name);

    /**
     * @brief Adds a cookie to the current page.
     * @param cookie A JSON object representing the cookie.
     */
    void addCookie(const nlohmann::json& cookie);

    /**
     * @brief Deletes a specific cookie by name.
     * @param name The name of the cookie to delete.
     */
    void deleteCookie(const std::string& name);

    /**
     * @brief Deletes all cookies visible to the current page.
     */
    void deleteAllCookies();

    // --- Alerts ---

    /**
     * @brief Accepts the currently displayed alert.
     */
    void acceptAlert();

    /**
     * @brief Dismisses the currently displayed alert.
     */
    void dismissAlert();

    /**
     * @brief Retrieves the text from the currently displayed alert.
     * @return The alert text.
     */
    std::string getAlertText();

    /**
     * @brief Sets the text of a prompt-type alert.
     * @param text The text to set in the alert prompt.
     */
    void setAlertText(const std::string& text);

    // --- Screenshots ---

    /**
     * @brief Takes a screenshot of the current page.
     * @return A base64-encoded PNG image string.
     */
    std::string takeScreenshot();

    /**
     * @brief Takes a screenshot of a specific element.
     * @param eid The element ID.
     * @return A base64-encoded PNG image string.
     */
    std::string takeElementScreenshot(const std::string& eid);

    /**
     * @brief Prints the current page to a PDF or as configured.
     * @param printOptions A JSON object with print settings.
     * @return A JSON object with print output data (e.g., base64 PDF).
     */
    nlohmann::json printPage(const nlohmann::json& printOptions = {});

    /**
     * @brief Pauses execution for a given number of milliseconds.
     * @param ms Milliseconds to wait.
     */
    void waitMS(unsigned ms);

private:
    const std::string baseUrl;      ///< Base URL of the WebDriver server.
    const curling::UserAgent ua;    ///< User agent used for HTTP requests.
    std::string sid;                ///< Session ID of the current WebDriver session.

    /**
     * @brief Sends a raw HTTP request to the WebDriver server.
     * @param method HTTP method (e.g., "GET", "POST").
     * @param path URL path relative to session or server.
     * @param payload Optional JSON payload.
     * @return JSON response from the server.
     */
    nlohmann::json request(const std::string& method, const std::string& path, const std::optional<nlohmann::json>& payload = std::nullopt);
};



