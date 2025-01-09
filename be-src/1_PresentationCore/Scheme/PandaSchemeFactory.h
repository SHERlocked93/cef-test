#pragma once
#include "include/cef_app.h"

// 自定义协议，接管并处理请求
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
