#include "PageHandler.h"

#include <codecvt>
#include <fstream>
#include <nlohmann/json.hpp>

#include "DialogHandler.h"
#include "include/base/cef_callback.h"
#include "include/cef_task.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"

static std::vector<std::string> split(const std::string& s, char delim) {
  std::vector<std::string> elems;
  std::istringstream iss(s);
  std::string item;
  while (std::getline(iss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

static std::string convertStr(const std::wstring& wide_string) {
  static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
  return utf8_conv.to_bytes(wide_string);
}

static std::wstring convertStr(const std::string& str) {
  static std::wstring_convert<std::codecvt_utf8<wchar_t>> utf8_conv;
  return utf8_conv.from_bytes(str);
}

void ReadFileByBlocks(const std::string& filePath, const std::string& msgName, CefRefPtr<CefFrame> frame) {
  std::ifstream fileStream(filePath, std::ifstream::binary);
  if (fileStream) {
    fileStream.seekg(0, fileStream.end);
    long length = fileStream.tellg();  // 文件总长
    fileStream.seekg(0, fileStream.beg);
    long size = 1024;
    long position = 0;
    while (position != length) {  // 循环读取，每次读size个字节
      long leftSize = length - position;
      if (leftSize < size) {
        size = leftSize;
      }
      char* buffer = new char[size];
      fileStream.read(buffer, size);
      position = fileStream.tellg();
      CefRefPtr<CefBinaryValue> data = CefBinaryValue::Create(buffer, size);
      CefRefPtr<CefProcessMessage> msgBack = CefProcessMessage::Create(msgName + "_data");
      CefRefPtr<CefListValue> msgArgs = msgBack->GetArgumentList();
      msgArgs->SetBinary(0, data);
      frame->SendProcessMessage(PID_RENDERER, msgBack);
      delete[] buffer;
    }

    fileStream.close();
    CefRefPtr<CefProcessMessage> msgBack = CefProcessMessage::Create(msgName + "_finish");
    frame->SendProcessMessage(PID_RENDERER, msgBack);
  }
}

bool PageHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                           CefProcessId source_process,
                                           CefRefPtr<CefProcessMessage> message) {
  CEF_REQUIRE_UI_THREAD()
  std::string msgName = message->GetName();
  std::vector<std::string> msgVec = split(msgName, '_');

  const auto& msgChannel = msgVec.at(0);
  const auto& msgType = msgVec.at(1);

  if (msgChannel == "window") {
    CefRefPtr<CefBrowserView> browserView = CefBrowserView::GetForBrowser(browser);
    CefRefPtr<CefWindow> window = browserView->GetWindow();

    if (msgType == "minimize") {
      window->Minimize();
    } else if (msgType == "maximize") {
      window->Maximize();
    } else if (msgType == "close") {
      window->Close();
    } else if (msgType == "restore") {
      window->Restore();
    }
  } else if (msgChannel == "system") {
    CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(msgName);
    CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();
    if (msgType == "getOSVersion") {
      std::string version = "win11.2024122901";
      msgArgs->SetString(0, version);
    }
    frame->SendProcessMessage(PID_RENDERER, msg);  // 发给渲染进程
  } else if (msgChannel == "dialog") {
    CefRefPtr<CefListValue> msgBody = message->GetArgumentList();

    json param = json::parse(msgBody->GetString(0).ToString());
    std::wstring title = convertStr(param["title"].get<std::string>());
    std::wstring defaultPath = convertStr(param["defaultPath"].get<std::string>());

    CefRefPtr<CefRunFileDialogCallback> dcb = new DialogHandler(msgName, frame);
    CefBrowserHost::FileDialogMode mode;
    std::vector<CefString> fileFilters;
    if (msgType == "openFile") {
      for (const std::string& var : param["filters"]) {
        fileFilters.emplace_back(var);
      }
      mode = param["multiSelections"].get<bool>() ? FILE_DIALOG_OPEN_MULTIPLE : FILE_DIALOG_OPEN;
      auto a = browser->GetHost();
      browser->GetHost()->RunFileDialog(mode, title, defaultPath, fileFilters, dcb);
    } else if (msgType == "openFolder") {
      mode = FILE_DIALOG_OPEN_FOLDER;
      browser->GetHost()->RunFileDialog(mode, title, defaultPath, fileFilters, dcb);
    }
  } else if (msgChannel == "file") {
    if (msgType == "readFile") {
      CefRefPtr<CefListValue> msgBody = message->GetArgumentList();
      json param = json::parse(msgBody->GetString(0).ToString());
      auto filePath = param["filePath"].get<std::string>();

      // TID_FILE_BACKGROUND 线程标记，指代文件读写后台线程，在这个线程下执行的任务不会阻塞浏览器主线程的操作
      // 浏览器进程的主线程的标记为 TID_UI，渲染进程主线程的标记为 PID_RENDERER。
      CefPostTask(TID_FILE_BACKGROUND, base::BindOnce(&ReadFileByBlocks, filePath, msgName, frame));
    }
  }

  return true;
}

void PageHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                      CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) {
  // const auto is_devtools = frame->GetURL().ToString().find("devtools://") != std::string::npos;
  // if (is_devtools) return;

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
  if (dialog_type == JSDIALOGTYPE_ALERT) {  // alert弹框
    MessageBox(hwnd, message_text.ToWString().c_str(), L"系统提示alert", MB_ICONEXCLAMATION | MB_OK);
    callback->Continue(true, CefString());
  } else if (dialog_type == JSDIALOGTYPE_CONFIRM) {  // confirm弹框
    int msgboxID =
        MessageBox(hwnd, message_text.ToWString().c_str(), L"系统提示confirm", MB_ICONEXCLAMATION | MB_YESNO);
    callback->Continue(msgboxID == IDYES, CefString());
  } else if (dialog_type == JSDIALOGTYPE_PROMPT) {  // prompt弹框
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
