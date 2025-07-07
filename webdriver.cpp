// WebDriverClient.cpp
#include "webdriver.hpp"
#include <stdexcept>
#include <random>

using json = nlohmann::json;

void WebDriverClient::sendKeysSlowly(const std::string& eid, const std::string& text, unsigned baseDelayMs) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(-20, 20); // +/- 20ms randomness

    for (char c : text) {
        std::string s(1, c);
        sendKeys(eid, s);
        int delay = static_cast<int>(baseDelayMs) + dist(gen);
        if (delay < 0) delay = 0;
        waitMS(static_cast<unsigned>(delay));
    }
}

json WebDriverClient::request(const std::string& method, const std::string& path, const std::optional<json>& payload) {
    curling::Request req;
    req.setMethod([&] {
        if (method == "GET") return curling::Request::Method::GET;
        if (method == "POST") return curling::Request::Method::POST;
        if (method == "DELETE") return curling::Request::Method::DEL;
        throw std::logic_error("Unsupported HTTP method");
    }())
    .setURL(baseUrl + path)
    .addHeader("Content-Type: application/json");

    if (payload) {
        req.setBody(payload->dump());
    }

    auto res = req.send();
    if (res.httpCode < 200 || res.httpCode >= 300) {
        throw std::runtime_error(
            "HTTP " + std::to_string(res.httpCode) + " error on " + method + " " + path + ": " + res.body
        );
    }

    json resp = json::parse(res.body);
    if (resp.contains("value")) return resp["value"];
    return resp;
}

// Session Management
std::string WebDriverClient::createSession(const json& caps) {
    auto v = request("POST", "/session", caps);
    sid = v.value("sessionId", v.value("session_id", ""));
    return sid;
}

void WebDriverClient::deleteSession() {
    if (sid.empty()) throw std::runtime_error("No active session to delete");
    request("DELETE", "/session/" + sid);
    sid.clear();
}

// Navigation
json WebDriverClient::getStatus() {
    return request("GET", "/status");
}

void WebDriverClient::navigateTo(const std::string& url) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/url", json{{"url", url}});
}

std::string WebDriverClient::getCurrentUrl() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/url").get<std::string>();
}

void WebDriverClient::back() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/back", json::object());
}

void WebDriverClient::forward() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/forward", json::object());
}

void WebDriverClient::refresh() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/refresh", json::object());
}

std::string WebDriverClient::getTitle() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/title", json::object()).get<std::string>();
}

void WebDriverClient::setTimeouts(const json& timeouts) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/timeouts", timeouts);
}

json WebDriverClient::getTimeouts() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/timeouts");
}

// Window & Frame
std::string WebDriverClient::getWindowHandle() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/window").get<std::string>();
}

std::vector<std::string> WebDriverClient::getWindowHandles() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/window/handles").get<std::vector<std::string>>();
}

void WebDriverClient::closeWindow() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("DELETE", "/session/" + sid + "/window", json::object());
}

void WebDriverClient::switchWindow(const std::string& handle) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/window", json{{"handle", handle}});
}

json WebDriverClient::getWindowRect() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/window/rect");
}

void WebDriverClient::setWindowRect(const json& rect) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/window/rect", rect);
}

void WebDriverClient::maximizeWindow() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/window/maximize", json::object());
}

void WebDriverClient::minimizeWindow() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/window/minimize", json::object());
}

void WebDriverClient::fullscreenWindow() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/window/fullscreen", json::object());
}

void WebDriverClient::switchFrame(const json& id) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/frame", id.is_null() ? json{} : json{{"id", id}});
}

void WebDriverClient::switchToParentFrame() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/frame/parent", json::object());
}

// Element interaction
std::string WebDriverClient::findElement(const std::string& using_, const std::string& value) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    auto v = request("POST", "/session/" + sid + "/element", json{{"using", using_}, {"value", value}});
    return v.at("element-6066-11e4-a52e-4f735466cecf").get<std::string>();
}

std::vector<std::string> WebDriverClient::findElements(const std::string& using_, const std::string& value) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    auto arr = request("POST", "/session/" + sid + "/elements", json{{"using", using_}, {"value", value}});
    std::vector<std::string> out;
    for (auto& e : arr) out.push_back(e.at("element-6066-11e4-a52e-4f735466cecf").get<std::string>());
    return out;
}

std::string WebDriverClient::findChildElement(const std::string& eid, const std::string& using_, const std::string& value) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    auto v = request("POST", "/session/" + sid + "/element/" + eid + "/element", json{{"using", using_}, {"value", value}});
    return v.at("element-6066-11e4-a52e-4f735466cecf").get<std::string>();
}

std::vector<std::string> WebDriverClient::findChildElements(const std::string& eid, const std::string& using_, const std::string& value) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    auto arr = request("POST", "/session/" + sid + "/element/" + eid + "/elements", json{{"using", using_}, {"value", value}});
    std::vector<std::string> out;
    for (auto& e : arr) out.push_back(e.at("element-6066-11e4-a52e-4f735466cecf").get<std::string>());
    return out;
}

std::string WebDriverClient::getElementAttribute(const std::string& eid, const std::string& name) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/element/" + eid + "/attribute/" + name).get<std::string>();
}

std::string WebDriverClient::getElementProperty(const std::string& eid, const std::string& name) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/element/" + eid + "/property/" + name).get<std::string>();
}

std::string WebDriverClient::getElementText(const std::string& eid) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/element/" + eid + "/text").get<std::string>();
}

std::string WebDriverClient::getElementTagName(const std::string& eid) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/element/" + eid + "/name").get<std::string>();
}

bool WebDriverClient::isElementSelected(const std::string& eid) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/element/" + eid + "/selected").get<bool>();
}

bool WebDriverClient::isElementEnabled(const std::string& eid) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/element/" + eid + "/enabled").get<bool>();
}

json WebDriverClient::getElementRect(const std::string& eid) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/element/" + eid + "/rect");
}

void WebDriverClient::clickElement(const std::string& eid) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/element/" + eid + "/click", nlohmann::json::object());
}

void WebDriverClient::clearElement(const std::string& eid) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/element/" + eid + "/clear", json::object());
}

void WebDriverClient::sendKeys(const std::string& eid, const std::string& text) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/element/" + eid + "/value", json{{"text", text}, {"value", std::vector<char>(text.begin(), text.end())}});
}

// Script execution
json WebDriverClient::executeScript(const std::string& script, const json& args) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("POST", "/session/" + sid + "/execute/sync", json{{"script", script}, {"args", args}});
}

json WebDriverClient::executeAsyncScript(const std::string& script, const json& args) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("POST", "/session/" + sid + "/execute/async", json{{"script", script}, {"args", args}});
}

// Cookies
std::vector<json> WebDriverClient::getAllCookies() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/cookie").get<std::vector<json>>();
}

json WebDriverClient::getCookie(const std::string& name) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/cookie/" + name);
}

void WebDriverClient::addCookie(const json& cookie) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/cookie", json{{"cookie", cookie}});
}

void WebDriverClient::deleteCookie(const std::string& name) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("DELETE", "/session/" + sid + "/cookie/" + name);
}

void WebDriverClient::deleteAllCookies() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("DELETE", "/session/" + sid + "/cookie");
}

// Alerts
void WebDriverClient::acceptAlert() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/alert/accept", json::object());
}

void WebDriverClient::dismissAlert() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/alert/dismiss", json::object());
}

std::string WebDriverClient::getAlertText() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/alert/text").get<std::string>();
}

void WebDriverClient::setAlertText(const std::string& text) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/alert/text", json{{"text", text}});
}

// Screenshots & Print
std::string WebDriverClient::takeScreenshot() {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/screenshot").get<std::string>();
}

std::string WebDriverClient::takeElementScreenshot(const std::string& eid) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("GET", "/session/" + sid + "/element/" + eid + "/screenshot").get<std::string>();
}

json WebDriverClient::printPage(const json& printOptions) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    return request("POST", "/session/" + sid + "/print", json{{"printOptions", printOptions}});
}

//wait, thread sleep for milliseconds
void WebDriverClient::waitMS(unsigned ms) {
    curling::waitMs(ms);
}

void WebDriverClient::performActions(const json& actions) {
    if (sid.empty()) throw std::runtime_error("Session not created");
    request("POST", "/session/" + sid + "/actions", json{{"actions", actions}});
}

void WebDriverClient::setFile(const std::string& elementId, const std::vector<std::string>& filePaths) {
    nlohmann::json payload;
    payload["files"] = filePaths;
    std::string path = "/session/" + sid + "/element/" + elementId + "/file";
    request("POST", path, payload);
}
