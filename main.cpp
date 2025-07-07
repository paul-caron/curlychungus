#include "webdriver.hpp"
#include <iostream>

using namespace std;

int main() {
    WebDriverClient wdc("http://localhost:4444");
    string sid = wdc.createSession();
    cout << "session: " << sid << endl;
    auto status = wdc.getStatus();
    cout << "status json: " << status.dump() << endl;
    auto rect = wdc.getWindowRect();
    cout << "window rect json: " << rect.dump() << endl;
    wdc.navigateTo("https://example.org/");
    cout << "title: " << wdc.getTitle() << endl;
    cout << "url: " << wdc.getCurrentUrl() << endl;
    wdc.waitMS(2000);
    wdc.navigateTo("https://httpbin.org/forms/post");
    wdc.waitMS(2000);
    wdc.executeScript("alert('testing alert message 1')");
    wdc.waitMS(2000);
    wdc.acceptAlert();
    wdc.waitMS(2000);
    wdc.executeScript("alert('testing alert message 2')");
//    wdc.setAlertText("testing alert message 2");
    wdc.waitMS(2000);
    wdc.dismissAlert();
    wdc.waitMS(2000);
    wdc.back();
    wdc.waitMS(2000);
    wdc.forward();
    wdc.waitMS(2000);
    wdc.refresh();
    wdc.waitMS(2000);
    wdc.minimizeWindow();
    wdc.waitMS(2000);
    wdc.maximizeWindow();
    wdc.waitMS(2000);
    wdc.fullscreenWindow();
    wdc.waitMS(2000);
    auto eid = wdc.findElement("css selector","input[name=\"custname\"]");
    wdc.sendKeysSlowly(eid, "Curly Chungus");
    wdc.clearElement(eid);
    wdc.sendKeysSlowly(eid, "Curly Chungus");
    wdc.waitMS(1000);
    eid = wdc.findElement("css selector","input[name=\"custtel\"]");
    wdc.sendKeysSlowly(eid, "1 234 567 8901");
    wdc.waitMS(1000);
    eid = wdc.findElement("css selector","input[name=\"custemail\"]");
    wdc.sendKeysSlowly(eid, "chungusaurus.rx@gmail.com");
    wdc.waitMS(1000);
    wdc.executeScript("document.querySelector('input[type=\"radio\"][name=\"size\"][value=\"large\"]').click();");
    wdc.waitMS(1000);
    wdc.executeScript("document.querySelector('input[type=\"checkbox\"][name=\"topping\"][value=\"mushroom\"]').click();");
    wdc.waitMS(1000);
    wdc.executeScript("document.querySelector('input[type=\"time\"][name=\"delivery\"]').value='11:15';");
    wdc.waitMS(1000);
    eid = wdc.findElement("css selector","textarea[name=\"comments\"]");
    wdc.sendKeysSlowly(eid, "NO ANCHOVIES!");
    wdc.waitMS(1000);
    eid = wdc.findElement("css selector", "form button");
    wdc.clickElement(eid);
    wdc.waitMS(2000);
    auto b64 = wdc.takeScreenshot();
    detail::base64ToFile(b64, "./screenshot.png");
    wdc.waitMS(2000);
    wdc.deleteSession();
    return 0;
}
