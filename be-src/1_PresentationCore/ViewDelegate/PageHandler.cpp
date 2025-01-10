#include "PageHandler.h"

#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"

void PageHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) {
  model->Clear();
  model->AddItem(MENU_ID_USER_FIRST, L"打开开发者调试工具");

  CefRefPtr<CefMenuModel> subModel =
      model->AddSubMenu(MENU_ID_USER_FIRST + 1, L"这是一个包含子菜单的测试菜单");
  subModel->AddItem(MENU_ID_USER_FIRST + 2, L"这是子菜单1");
  subModel->AddItem(MENU_ID_USER_FIRST + 3, L"这是子菜单2");

  model->AddSeparator();
  model->AddCheckItem(MENU_ID_USER_FIRST + 4, L"这是一个包含复选框的菜单");
  model->SetChecked(MENU_ID_USER_FIRST + 4, true);

  model->AddRadioItem(MENU_ID_USER_FIRST + 5, L"这是一个包含复选框的菜单", 888);
  model->AddRadioItem(MENU_ID_USER_FIRST + 6, L"这是一个包含单选框的菜单", 888);
  model->SetChecked(MENU_ID_USER_FIRST + 6, true);

  model->AddSeparator();
  model->AddItem(MENU_ID_USER_FIRST + 7, L"刷新");
}

bool PageHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                       CefRefPtr<CefContextMenuParams> params, int command_id,
                                       EventFlags event_flags) {
  switch (command_id) {
    case MENU_ID_USER_FIRST: {
      CefWindowInfo windowInfo;
      CefBrowserSettings browserSettings;
      CefPoint mousePoint(params->GetXCoord(), params->GetYCoord());
      browser->GetHost()->ShowDevTools(windowInfo, this, browserSettings, mousePoint);
      break;
    }
    case MENU_ID_USER_FIRST + 7: {
      browser->Reload();
      break;
    }
    default: {
      std::wstring msg = L"你点击的标签ID：" + std::to_wstring(command_id - MENU_ID_USER_FIRST);
      MessageBox(nullptr, (LPWSTR)msg.c_str(), L"系统提示", MB_ICONEXCLAMATION | MB_OKCANCEL);
      break;
    }
  }
  return true;
}

bool PageHandler::OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser, const CefString& message_text,
                                       bool is_reload, CefRefPtr<CefJSDialogCallback> callback) {
  // message_text.c_str() 的值是Is it OK to leave/refresh this page
  int msgboxID = MessageBox(NULL, L"您编辑的内容尚未保存.\n确定要关闭窗口吗?", L"系统提示",
                            MB_ICONEXCLAMATION | MB_OKCANCEL);
  callback->Continue(msgboxID == IDOK, CefString());
  return true;
}

bool PageHandler::OnJSDialog(CefRefPtr<CefBrowser> browser, const CefString& origin_url,
                             JSDialogType dialog_type, const CefString& message_text,
                             const CefString& default_prompt_text, CefRefPtr<CefJSDialogCallback> callback,
                             bool& suppress_message) {
  suppress_message = false;
  HWND hwnd = browser->GetHost()->GetWindowHandle();
  if (dialog_type == JSDialogType::JSDIALOGTYPE_ALERT) {  // alert弹框
    MessageBox(hwnd, message_text.ToWString().c_str(), L"系统提示alert", MB_ICONEXCLAMATION | MB_OK);
    callback->Continue(true, CefString());
  } else if (dialog_type == JSDialogType::JSDIALOGTYPE_CONFIRM) {  // confirm弹框
    int msgboxID =
        MessageBox(hwnd, message_text.ToWString().c_str(), L"系统提示confirm", MB_ICONEXCLAMATION | MB_YESNO);
    callback->Continue(msgboxID == IDYES, CefString());
  } else if (dialog_type == JSDialogType::JSDIALOGTYPE_PROMPT) {  // prompt弹框
    return false;
  }
  return true;
}

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

void PageHandler::OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                            const std::vector<CefDraggableRegion>& regions) {
  CefRefPtr<CefBrowserView> browser_view = CefBrowserView::GetForBrowser(browser);
  if (browser_view) {
    CefRefPtr<CefWindow> window = browser_view->GetWindow();
    if (window) window->SetDraggableRegions(regions);
  }
}
