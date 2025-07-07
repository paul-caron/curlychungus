#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "webdriver.hpp"
#include "json.hpp"
#include <string>

const nlohmann::json caps = nlohmann::json::parse(R"({
  "capabilities": {
    "alwaysMatch": {
      "browserName": "chrome",
      "goog:chromeOptions": {
        "args": [
          "--headless",
          "--no-sandbox",
          "--disable-dev-shm-usage"
        ]
      }
    }
  }
})");

TEST_CASE("Navigate to example.com and check title") {
    WebDriverClient client("http://localhost:4444");

    CHECK_NOTHROW(client.createSession(caps));
    CHECK_NOTHROW(client.navigateTo("https://example.com"));

    std::string title = client.getTitle();
    CHECK(title == "Example Domain");

    client.deleteSession();
}

TEST_CASE("Navigate, find element, and check text") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    std::string h1id = client.findElement("css selector", "h1");
    std::string h1text = client.getElementText(h1id);

    CHECK(h1text == "Example Domain");

    client.deleteSession();
}

TEST_CASE("Fill and submit form on httpbin") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://httpbin.org/forms/post");

    std::string inputId = client.findElement("css selector", "input[name='custname']");
    client.clearElement(inputId);
    client.sendKeys(inputId, "Test User");

    std::string submitBtnId = client.findElement("css selector", "form button");
    client.clickElement(submitBtnId);

    std::string bodyId = client.findElement("css selector", "body");
    std::string bodyText = client.getElementText(bodyId);

    CHECK(bodyText.find("Test User") != std::string::npos);

    client.deleteSession();
}

TEST_CASE("Check current URL after navigation") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    std::string currentUrl = client.getCurrentUrl();
    CHECK(currentUrl.find("example.com") != std::string::npos);

    client.deleteSession();
}

TEST_CASE("JavaScript execution to get document title") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    auto result = client.executeScript("return document.title;");
    CHECK(result == "Example Domain");

    client.deleteSession();
}

TEST_CASE("Test timeouts setting and retrieval") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    nlohmann::json timeouts = {
        {"implicit", 1000},
        {"pageLoad", 5000},
        {"script", 30000}
    };

    client.setTimeouts(timeouts);
    auto returnedTimeouts = client.getTimeouts();

    CHECK(returnedTimeouts["implicit"] == 1000);
    CHECK(returnedTimeouts["pageLoad"] == 5000);
    CHECK(returnedTimeouts["script"] == 30000);

    client.deleteSession();
}

TEST_CASE("Set and retrieve cookies") {
    using nlohmann::json;
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    // Add a test cookie
    json cookie = {
        {"name", "test_cookie"},
        {"value", "cookie_value"},
        {"path", "/"},
        {"domain", "example.com"}
    };
    CHECK_NOTHROW(client.addCookie(cookie));

    // Retrieve the cookie by name
    json retrieved = client.getCookie("test_cookie");

    CHECK(retrieved["name"] == "test_cookie");
    CHECK(retrieved["value"] == "cookie_value");
    std::string domain = retrieved["domain"];
    if (domain.front() == '.')
        domain = domain.substr(1);
    CHECK(domain == "example.com");

    // Optional: check if it's included in getAllCookies
    std::vector<json> allCookies = client.getAllCookies();
    bool found = false;
    for (const auto& c : allCookies) {
        if (c["name"] == "test_cookie" && c["value"] == "cookie_value") {
            found = true;
            break;
        }
    }
    CHECK(found);

    client.deleteSession();
}

TEST_CASE("Delete cookie and verify removal") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    nlohmann::json cookie = {
        {"name", "temp_cookie"},
        {"value", "temp_value"},
        {"path", "/"},
        {"domain", "example.com"}
    };

    client.addCookie(cookie);
    client.deleteCookie("temp_cookie");

    std::vector<nlohmann::json> allCookies = client.getAllCookies();
    bool found = false;
    for (const auto& c : allCookies) {
        if (c["name"] == "temp_cookie") {
            found = true;
            break;
        }
    }
    CHECK(!found);

    client.deleteSession();
}


TEST_CASE("Element attribute and tag name retrieval") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    std::string aTag = client.findElement("css selector", "a");
    std::string href = client.getElementAttribute(aTag, "href");
    std::string tagName = client.getElementTagName(aTag);

    CHECK(href.find("iana.org") != std::string::npos);
    CHECK(tagName == "a");

    client.deleteSession();
}

TEST_CASE("Element enabled and selected checks") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://httpbin.org/forms/post");

    std::string radioId = client.findElement("css selector", "input[name='size'][value='small']");
    bool enabled = client.isElementEnabled(radioId);
    bool selected = client.isElementSelected(radioId);

    CHECK(enabled == true);
    CHECK(selected == false);

    client.clickElement(radioId);
    CHECK(client.isElementSelected(radioId) == true);

    client.deleteSession();
}

TEST_CASE("Async script execution with callback") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    std::string asyncScript = R"(
        var callback = arguments[arguments.length - 1];
        setTimeout(function() {
            callback("async result");
        }, 100);
    )";

    auto result = client.executeAsyncScript(asyncScript);
    CHECK(result == "async result");

    client.deleteSession();
}

TEST_CASE("Clear input field") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://httpbin.org/forms/post");

    std::string inputId = client.findElement("css selector", "input[name='custname']");
    client.sendKeys(inputId, "Some Text");
    client.clearElement(inputId);

    // Check that the input is empty (get property "value" should be "")
    std::string val = client.getElementProperty(inputId, "value");
    CHECK(val.empty());

    client.deleteSession();
}

TEST_CASE("Navigation back and forward") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");
    std::string url1 = client.getCurrentUrl();

    client.navigateTo("https://httpbin.org/");
    std::string url2 = client.getCurrentUrl();

    client.back();
    std::string urlBack = client.getCurrentUrl();
    CHECK(urlBack == url1);

    client.forward();
    std::string urlForward = client.getCurrentUrl();
    CHECK(urlForward == url2);

    client.deleteSession();
}

TEST_CASE("Alert accept and dismiss") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://the-internet.herokuapp.com/javascript_alerts");

    client.executeScript("alert('Test alert');");
    std::string alertText = client.getAlertText();
    CHECK(alertText == "Test alert");

    client.acceptAlert();

    client.executeScript("alert('Another alert');");
    client.dismissAlert();

    client.deleteSession();
}

TEST_CASE("Trigger prompt alert and set response text") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    // Create a prompt dialog using JavaScript
    client.executeScript(R"(
        window.promptResult = prompt('Enter your name:', 'default');
    )");

    // Set alert input text (works for prompt dialogs only)
    CHECK_NOTHROW(client.setAlertText("CurlyChungus"));

    // Accept the prompt
    CHECK_NOTHROW(client.acceptAlert());

    // Verify that the input was received correctly
    std::string result = client.executeScript("return window.promptResult;").get<std::string>();
    CHECK(result == "CurlyChungus");

    client.deleteSession();
}

TEST_CASE("Window resize and maximize") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    nlohmann::json size = {
        {"width", 800},
        {"height", 600}
    };

    client.setWindowRect(size);
    auto rect = client.getWindowRect();
    CHECK(rect["width"] == 800);
    CHECK(rect["height"] == 600);

    CHECK_NOTHROW(client.maximizeWindow());
    CHECK_NOTHROW(client.fullscreenWindow());

    client.deleteSession();
}

TEST_CASE("Switch to frame and back") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://the-internet.herokuapp.com/iframe");

    std::string iframeId = client.findElement("css selector", "iframe");

    nlohmann::json frameElement = {
        {"element-6066-11e4-a52e-4f735466cecf", iframeId}
    };

    client.switchFrame(frameElement);

    std::string bodyId = client.findElement("css selector", "body");
    CHECK(client.getElementTagName(bodyId) == "body");

    client.switchToParentFrame();

    std::string headingId = client.findElement("css selector", "h3");
    CHECK(client.getElementText(headingId).find("Editor") != std::string::npos);

    client.deleteSession();
}

TEST_CASE("Get current window handle and all window handles") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    std::string currentHandle = client.getWindowHandle();
    std::vector<std::string> allHandles = client.getWindowHandles();

    CHECK(!currentHandle.empty());
    CHECK(!allHandles.empty());
    CHECK(std::find(allHandles.begin(), allHandles.end(), currentHandle) != allHandles.end());

    client.deleteSession();
}

TEST_CASE("Open new window, switch to it, and close") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    std::string originalHandle = client.getWindowHandle();

    // Open a new window using JavaScript
    client.executeScript(R"(
        window.open("https://httpbin.org", "_blank");
    )");

    // Wait for new handle to appear
    std::vector<std::string> handles;
    for (int i = 0; i < 10; ++i) {
        handles = client.getWindowHandles();
        if (handles.size() > 1) break;
        client.waitMS(200);
    }

    CHECK(handles.size() > 1);

    std::string newHandle;
    for (const auto& h : handles) {
        if (h != originalHandle) {
            newHandle = h;
            break;
        }
    }

    CHECK(!newHandle.empty());

    // Switch to the new window and verify title
    client.switchWindow(newHandle);
    CHECK(client.getTitle().find("httpbin") != std::string::npos);

    // Close the new window and switch back
    client.closeWindow();
    client.switchWindow(originalHandle);
    CHECK(client.getTitle() == "Example Domain");

    client.deleteSession();
}


TEST_CASE("Take full-page screenshot") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    std::string base64Image = client.takeScreenshot();
    CHECK(!base64Image.empty());

    // Optionally write to file for manual verification
    detail::base64ToFile(base64Image, "example_screenshot.png");

    client.deleteSession();
}

TEST_CASE("Take screenshot of a specific element") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    std::string h1Id = client.findElement("css selector", "h1");

    std::string base64Image = client.takeElementScreenshot(h1Id);
    CHECK(!base64Image.empty());

    // Optionally write to file
    detail::base64ToFile(base64Image, "element_screenshot.png");

    client.deleteSession();
}


TEST_CASE("Print page to PDF") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://example.com");

    nlohmann::json printOptions = {
        {"landscape", false},
        {"displayHeaderFooter", false},
        {"printBackground", true},
        {"paperWidth", 8.27},   // A4 width in inches
        {"paperHeight", 11.69}, // A4 height in inches
        {"marginTop", 0.4},
        {"marginBottom", 0.4},
        {"marginLeft", 0.4},
        {"marginRight", 0.4},
        {"pageRanges", "1"}
    };

    // Likely returns just a base64-encoded string
    std::string base64Pdf = client.printPage(printOptions);
    CHECK(!base64Pdf.empty());

    // Save to file for inspection
    detail::base64ToFile(base64Pdf, "page_print.pdf");

    client.deleteSession();
}

TEST_CASE("Perform mouse click and drag using performActions") {
    WebDriverClient client("http://localhost:4444");
    client.createSession(caps);

    client.navigateTo("https://the-internet.herokuapp.com/drag_and_drop");

    // Mouse click at (100, 100)
    nlohmann::json clickAction = {{
        {"type", "pointer"},
        {"id", "mouse"},
        {"parameters", {{"pointerType", "mouse"}}},
        {"actions", {
            {{"type", "pointerMove"}, {"x", 100}, {"y", 100}, {"origin", "viewport"}},
            {{"type", "pointerDown"}, {"button", 0}},
            {{"type", "pointerUp"}, {"button", 0}}
        }}
    }};

    CHECK_NOTHROW(client.performActions({clickAction}));

    // Mouse drag from (100,100) to (200,200)
    nlohmann::json dragAction = {{
        {"type", "pointer"},
        {"id", "mouse"},
        {"parameters", {{"pointerType", "mouse"}}},
        {"actions", {
            {{"type", "pointerMove"}, {"x", 100}, {"y", 100}, {"origin", "viewport"}},
            {{"type", "pointerDown"}, {"button", 0}},
            {{"type", "pause"}, {"duration", 100}},
            {{"type", "pointerMove"}, {"x", 200}, {"y", 200}, {"origin", "viewport"}},
            {{"type", "pointerUp"}, {"button", 0}}
        }}
    }};

    CHECK_NOTHROW(client.performActions({dragAction}));

    client.deleteSession();
}
