#include "App.h"

#include "./Scheme/PandaSchemeFactory.h"
#include "./ViewDelegate/PageHandler.h"
#include "./ViewDelegate/ViewDelegate.h"
#include "./ViewDelegate/WindowDelegate.h"
#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_scheme.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

const std::string SCHEMA_NAME = "panda";

void App::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD()

  CefRegisterSchemeHandlerFactory(SCHEMA_NAME, "main-view", new PandaSchemeFactory());

  CefBrowserSettings settings;

  CefRefPtr<PageHandler> pageHandler(new PageHandler());
  CefRefPtr<ViewDelegate> viewDelegate(new ViewDelegate());
  CefRefPtr<CefBrowserView> browserView = CefBrowserView::CreateBrowserView(
      pageHandler, "panda://main-view/index.html", settings, nullptr, nullptr, viewDelegate);

  CefWindow::CreateTopLevelWindow(new WindowDelegate(browserView, false));
}

void App::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {
  registrar->AddCustomScheme(
      SCHEMA_NAME,
      CEF_SCHEME_OPTION_STANDARD |  // 标准协议 [scheme]://[username]:[password]@[host]:[port]/[url-path]
          CEF_SCHEME_OPTION_CORS_ENABLED  // 放开同源策略
  );
}
