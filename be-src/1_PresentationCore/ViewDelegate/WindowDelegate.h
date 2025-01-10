#pragma once
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"

// ���ڴ����࣬�������ڵ�����
class WindowDelegate : public CefWindowDelegate {
 public:
  explicit WindowDelegate(CefRefPtr<CefBrowserView> browser_view, bool is_devtool)
      : is_devtool_(is_devtool), browser_view_(browser_view) {}
  // �ޱ߿򴰿ڣ����ε���devtool�Ĵ��ڵ�ϵͳĬ�ϵı�����
  bool IsFrameless(CefRefPtr<CefWindow> window) override;
  void OnWindowCreated(CefRefPtr<CefWindow> window) override;
  void OnWindowDestroyed(CefRefPtr<CefWindow> window) override;
  bool CanClose(CefRefPtr<CefWindow> window) override;
  CefRect GetInitialBounds(CefRefPtr<CefWindow> window) override;
  WindowDelegate(const WindowDelegate&) = delete;
  WindowDelegate& operator=(const WindowDelegate&) = delete;

 private:
  // ��ǰ�����Ƿ�Ϊ�����߹��ߴ���
  bool is_devtool_;
  CefRefPtr<CefBrowserView> browser_view_;
  IMPLEMENT_REFCOUNTING(WindowDelegate);
};
