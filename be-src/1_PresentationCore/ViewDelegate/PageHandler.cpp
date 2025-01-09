#include "PageHandler.h"

#include "include/wrapper/cef_helpers.h"

void PageHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD()
  browsers.push_back(browser);
}

void PageHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  CEF_REQUIRE_UI_THREAD()
  for (auto it = browsers.begin(); it != browsers.end(); ++it) {
    if ((*it)->IsSame(browser)) {
      browsers.erase(it);
      break;
    }
  }

  if (browsers.empty()) CefQuitMessageLoop();
}
