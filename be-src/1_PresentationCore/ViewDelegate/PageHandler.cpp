#include "PageHandler.h"

#include "include/wrapper/cef_helpers.h"

bool PageHandler::OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser, const CefString& message_text,
                                       bool is_reload, CefRefPtr<CefJSDialogCallback> callback) {
  // message_text.c_str() 的值是Is it OK to leave/refresh this page
  int msgboxID = MessageBox(NULL, L"您编辑的内容尚未保存.\n确定要关闭窗口吗?", L"系统提示",
                            MB_ICONEXCLAMATION | MB_OKCANCEL);
  if (msgboxID == IDOK) {
    callback->Continue(true, CefString());
  } else {
    callback->Continue(false, CefString());
  }
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
