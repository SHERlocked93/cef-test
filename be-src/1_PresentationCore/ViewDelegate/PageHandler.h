#pragma once
#include <list>

#include "include/cef_app.h"
class PageHandler : public CefClient, public CefLifeSpanHandler {
 public:
  PageHandler() = default;
  //! 用于处理页面生命周期中的事件
  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
  //! 页面创建成功
  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  //! 页面即将关闭
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

 private:
  IMPLEMENT_REFCOUNTING(PageHandler);
  std::list<CefRefPtr<CefBrowser>> browsers;
};
