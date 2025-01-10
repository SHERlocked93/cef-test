#include "ViewDelegate.h"

#include "WindowDelegate.h"

bool ViewDelegate::OnPopupBrowserViewCreated(CefRefPtr<CefBrowserView> browser_view,
                                             CefRefPtr<CefBrowserView> popup_browser_view, bool is_devtools) {
  CefWindow::CreateTopLevelWindow(new WindowDelegate(popup_browser_view, is_devtools));
  return true;
}
