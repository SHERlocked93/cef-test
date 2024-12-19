#pragma once
#include "include/cef_app.h"

class App : public CefApp, public CefBrowserProcessHandler {
 public:
  App() = default;
  App(const App&) = delete;
  App& operator=(const App&) = delete;
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }
  void OnContextInitialized() override;
  void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;

 private:
  IMPLEMENT_REFCOUNTING(App);
};
