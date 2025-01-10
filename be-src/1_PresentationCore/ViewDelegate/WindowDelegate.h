#pragma once
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"

// 窗口代理类，操作窗口的能力
class WindowDelegate : public CefWindowDelegate {
 public:
  explicit WindowDelegate(CefRefPtr<CefBrowserView> browser_view, bool is_devtool)
      : is_devtool_(is_devtool), browser_view_(browser_view) {}
  // 无边框窗口，屏蔽掉非devtool的窗口的系统默认的标题栏
  bool IsFrameless(CefRefPtr<CefWindow> window) override;
  void OnWindowCreated(CefRefPtr<CefWindow> window) override;
  void OnWindowDestroyed(CefRefPtr<CefWindow> window) override;
  bool CanClose(CefRefPtr<CefWindow> window) override;
  CefRect GetInitialBounds(CefRefPtr<CefWindow> window) override;
  WindowDelegate(const WindowDelegate&) = delete;
  WindowDelegate& operator=(const WindowDelegate&) = delete;

 private:
  // 当前窗口是否为开发者工具窗口
  bool is_devtool_;
  CefRefPtr<CefBrowserView> browser_view_;
  IMPLEMENT_REFCOUNTING(WindowDelegate);
};
