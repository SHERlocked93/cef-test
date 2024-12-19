#pragma once
#include "include/cef_app.h"

class PandaSchemeFactory : public CefSchemeHandlerFactory {
 public:
  PandaSchemeFactory() = default;
  CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                       const CefString& scheme_name, CefRefPtr<CefRequest> request) override;
  PandaSchemeFactory(const PandaSchemeFactory&) = delete;
  PandaSchemeFactory& operator=(const PandaSchemeFactory&) = delete;

 private:
  IMPLEMENT_REFCOUNTING(PandaSchemeFactory);
};
