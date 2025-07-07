#pragma once
#include <string>
#include <vector>
#include <optional>
#include <fstream>
#include "json.hpp"
#include "curling.hpp"

namespace detail{
// Function to map a Base64 character to its 6-bit integer value.
inline int base64Value(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    if (c == '=') return -1; // Padding character
    throw std::invalid_argument("Invalid Base64 character");
}

// Function to decode a Base64 encoded string into raw bytes.
inline std::vector<unsigned char> base64Decode(const std::string &input) {
    std::vector<unsigned char> decodedData;
    int bitBuffer = 0; // Buffer for bits being assembled
    int bitCount = 0;  // Number of bits currently in buffer

    for (char c : input) {
        if (c == '=') break;

        int val = base64Value(c);
        if (val == -1) continue; // Skip padding

        bitBuffer = (bitBuffer << 6) | val;
        bitCount += 6;

        if (bitCount >= 8) {
            bitCount -= 8;
            decodedData.push_back((bitBuffer >> bitCount) & 0xFF);
        }
    }

    return decodedData;
}

// Function to write decoded Base64 data to a file
inline void base64ToFile(const std::string &b64, const std::string &filePath) {
    try {
        // Decode the Base64 string into bytes
        auto decodedData = base64Decode(b64);

        // Write the decoded byte data to the specified file in binary mode
        std::ofstream outputFile(filePath, std::ios::binary | std::ios::out);
        if (outputFile.is_open()) {
            outputFile.write(reinterpret_cast<const char*>(decodedData.data()), decodedData.size());
            outputFile.close();
            std::cout << "Image saved successfully as " << filePath << std::endl;
        } else {
            throw std::runtime_error("Unable to open the file for writing.");
        }
    } catch (const std::exception &e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
    }
}

};

class WebDriverClient {
public:
    explicit WebDriverClient(std::string remoteUrl)
      : baseUrl(std::move(remoteUrl)) {}

    // Session management
    std::string createSession(const nlohmann::json& caps = {{"capabilities", {{"alwaysMatch", {{"browserName", "firefox"}}}}}});
    void deleteSession();

    // Navigation
    nlohmann::json getStatus();
    void navigateTo(const std::string& url);
    std::string getCurrentUrl();
    void back();
    void forward();
    void refresh();
    std::string getTitle();
    void setTimeouts(const nlohmann::json& timeouts);
    nlohmann::json getTimeouts();

    // Window & Frame
    std::string getWindowHandle();
    std::vector<std::string> getWindowHandles();
    void closeWindow();
    void switchWindow(const std::string& handle);
    nlohmann::json getWindowRect();
    void setWindowRect(const nlohmann::json& rect);
    void maximizeWindow();
    void minimizeWindow();
    void fullscreenWindow();
    void switchFrame(const nlohmann::json& id = nullptr);
    void switchToParentFrame();

    // Element interaction
    std::string findElement(const std::string& using_, const std::string& value);
    std::vector<std::string> findElements(const std::string& using_, const std::string& value);
    std::string findChildElement(const std::string& eid, const std::string& using_, const std::string& value);
    std::vector<std::string> findChildElements(const std::string& eid, const std::string& using_, const std::string& value);
    std::string getElementAttribute(const std::string& eid, const std::string& name);
    std::string getElementProperty(const std::string& eid, const std::string& name);
    std::string getElementText(const std::string& eid);
    std::string getElementTagName(const std::string& eid);
    bool isElementSelected(const std::string& eid);
    bool isElementEnabled(const std::string& eid);
    nlohmann::json getElementRect(const std::string& eid);
    void clickElement(const std::string& eid);
    void clearElement(const std::string& eid);
    void sendKeys(const std::string& eid, const std::string& text);

    // Script execution
    nlohmann::json executeScript(const std::string& script, const nlohmann::json& args = nlohmann::json::array());
    nlohmann::json executeAsyncScript(const std::string& script, const nlohmann::json& args = nlohmann::json::array());

    // Cookies
    std::vector<nlohmann::json> getAllCookies();
    nlohmann::json getCookie(const std::string& name);
    void addCookie(const nlohmann::json& cookie);
    void deleteCookie(const std::string& name);
    void deleteAllCookies();

    // Alerts
    void acceptAlert();
    void dismissAlert();
    std::string getAlertText();
    void setAlertText(const std::string& text);

    // Screenshots
    std::string takeScreenshot();
    std::string takeElementScreenshot(const std::string& eid);
    nlohmann::json printPage(const nlohmann::json& printOptions = {});

    void waitMS(unsigned ms);
    void sendKeysSlowly(const std::string& eid, const std::string& text, unsigned baseDelayMs = 100);
    void performActions(const nlohmann::json& actions);
    void setFile(const std::string& elementId, const std::vector<std::string>& filePaths);

private:
    const std::string baseUrl;
    std::string sid; //session id

    nlohmann::json request(const std::string& method, const std::string& path, const std::optional<nlohmann::json>& payload = std::nullopt);
};
