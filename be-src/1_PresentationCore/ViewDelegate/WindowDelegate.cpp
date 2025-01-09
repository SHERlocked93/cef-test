#include "WindowDelegate.h"

#include "include/cef_app.h"
#include "include/views/cef_display.h"

void WindowDelegate::OnWindowCreated(CefRefPtr<CefWindow> window) {
  window->AddChildView(browser_view_);
  window->Show();
  browser_view_->RequestFocus();
  window->SetTitle("这是我的窗口标题");
  // window->CenterWindow(CefSize(800, 600));
}

void WindowDelegate::OnWindowDestroyed(CefRefPtr<CefWindow> window) { browser_view_ = nullptr; }

bool WindowDelegate::CanClose(CefRefPtr<CefWindow> window) {
  bool result = true;
  CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
  if (browser) {
    result = browser->GetHost()->TryCloseBrowser();
  }
  return result;
}

CefRect WindowDelegate::GetInitialBounds(CefRefPtr<CefWindow> window) {
  CefRefPtr<CefDisplay> display = CefDisplay::GetPrimaryDisplay();
  CefRect rect = display->GetBounds();
  rect.x = (rect.width - 800) / 2;
  rect.y = (rect.height - 600) / 2;
  rect.width = 800;
  rect.height = 600;
  return rect;
}
