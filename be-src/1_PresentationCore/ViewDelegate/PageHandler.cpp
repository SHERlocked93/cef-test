#include "PageHandler.h"

#include <sqlite3.h>

#include <chrono>
#include <codecvt>
#include <fstream>
#include <nlohmann/json.hpp>
#include <thread>

#include "DialogHandler.h"
#include "include/base/cef_callback.h"
#include "include/cef_task.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "vlc/vlc.h"
using namespace std::chrono_literals;

static sqlite3* db = nullptr;

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

//! 执行sqlite3的sql语句
static void ExecuteSql(const std::string& sqlStr, const std::string& msgName, CefRefPtr<CefFrame> frame) {
  json result;
  result["data"] = {};
  const char* zTail = sqlStr.c_str();
  sqlite3_exec(db, "BEGIN TRANSACTION;", NULL, NULL, NULL);
  while (strlen(zTail) != 0) {
    sqlite3_stmt* stmt = NULL;
    // 判断 prepareResult == SQLITE_OK 并处理异常
    int prepareResult = sqlite3_prepare_v2(db, zTail, -1, &stmt, &zTail);
    int stepResult = sqlite3_step(stmt);
    while (stepResult == SQLITE_ROW) {
      json row;
      int columnCount = sqlite3_column_count(stmt);
      for (size_t i = 0; i < columnCount; i++) {
        std::string columnName = sqlite3_column_name(stmt, i);
        int type = sqlite3_column_type(stmt, i);
        if (type == SQLITE_INTEGER) {
          row[columnName] = sqlite3_column_int(stmt, i);
        } else if (type == SQLITE3_TEXT) {
          const unsigned char* val = sqlite3_column_text(stmt, i);
          row[columnName] = reinterpret_cast<const char*>(val);
        }
        // 这里只处理了两种数据类型是不足以满足生产条件的
      }
      result["data"].push_back(row);
      stepResult = sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
  }
  sqlite3_exec(db, "END TRANSACTION;", NULL, NULL, NULL);

  CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(msgName);
  CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();
  result["success"] = true;
  msgArgs->SetString(0, result.dump());
  frame->SendProcessMessage(PID_RENDERER, msg);
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
  } else if (msgChannel == "db") {
    CefRefPtr<CefListValue> msgBody = message->GetArgumentList();
    if (msgType == "open") {
      json param = json::parse(msgBody->GetString(0).ToString());
      std::string dbPath = param["dbPath"].get<std::string>();
      int rc = sqlite3_open(dbPath.c_str(), &db);
      CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(msgName);
      CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();
      json result;
      result["success"] = rc == 0;
      msgArgs->SetString(0, result.dump());
      frame->SendProcessMessage(PID_RENDERER, msg);
    } else if (msgType == "close") {
      int rc = sqlite3_close(db);
      CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(msgName);
      CefRefPtr<CefListValue> msgArgs = msg->GetArgumentList();
      json result;
      const auto isSucc = rc == SQLITE_OK;
      if (isSucc) db = nullptr;

      result["success"] = isSucc;
      msgArgs->SetString(0, result.dump());
      frame->SendProcessMessage(PID_RENDERER, msg);
    } else if (msgType == "execute") {
      json param = json::parse(msgBody->GetString(0).ToString());
      std::string sqlStr = param["sql"].get<std::string>();
      CefPostTask(TID_FILE_BACKGROUND, base::BindOnce(&ExecuteSql, sqlStr, msgName, frame));
    }
  } else if (msgChannel == "video") {
    static libvlc_media_player_t* player = nullptr;
    static libvlc_instance_t* vlc_ins = nullptr;
    static libvlc_media_t* media = nullptr;
    if (msgType == "open") {
      CefRefPtr<CefListValue> msgBody = message->GetArgumentList();
      json param = json::parse(msgBody->GetString(0).ToString());
      std::string videoPath = param["videoPath"].get<std::string>();

      if (player) libvlc_media_player_release(player);
      if (media) libvlc_media_release(media);
      if (vlc_ins) libvlc_release(vlc_ins);

      vlc_ins = libvlc_new(0, NULL);  // 创建vlc实例

      if (!vlc_ins) {
        fprintf(stderr, "Unable to get VLC instance.\n\t%s\n", libvlc_errmsg());
        return EXIT_FAILURE;
      }

      media = libvlc_media_new_path(vlc_ins, videoPath.c_str());  // 注意路径为utf8
      player = libvlc_media_player_new_from_media(media);         // 创建播放对象
      if (!player) {
        fprintf(stderr, "Unable to get VLC media player.\n\t%s\n", libvlc_errmsg());
        if (media) libvlc_media_release(media);  // 释放媒体实例
        if (vlc_ins) libvlc_release(vlc_ins);    // 释放VLC实例
        return EXIT_FAILURE;
      }

      std::thread videoPlaying([] {
        int ret = libvlc_media_player_play(player);  // 进行播放
        if (ret == -1 || !player) return EXIT_FAILURE;

        while (libvlc_media_player_is_playing(player)) {
          std::this_thread::sleep_for(2ms);
        }
        return EXIT_SUCCESS;
      });
      Sleep(1000);

      libvlc_time_t tm = libvlc_media_player_get_length(player);  // 获得视频长度
      std::string fmtTime = std::format("{:02}:{:02}:{:02}",
                                        tm / 3600000,       // 小时
                                        (tm / 60000) % 60,  // 分钟
                                        (tm / 1000) % 60);  // 秒
      int width = libvlc_video_get_width(player);           // 获取视频宽度
      int hight = libvlc_video_get_height(player);          // 获取视频高度

      std::string fmtSize = std::format("w-{}:h-{}", width, hight);
      videoPlaying.detach();

    } else if (msgType == "pause") {
      if (player) libvlc_media_player_pause(player);
    } else if (msgType == "play") {
      if (player) libvlc_media_player_play(player);
    } else if (msgType == "stop") {
      if (player) libvlc_media_player_stop(player);
    } else if (msgType == "close") {
      if (player) libvlc_media_player_release(player);  // 释放播放实例
      if (media) libvlc_media_release(media);           // 释放媒体实例
      if (vlc_ins) libvlc_release(vlc_ins);             // 释放VLC实例
    } else if (msgType == "fullscreen") {
      CefRefPtr<CefListValue> msgBody = message->GetArgumentList();
      json param = json::parse(msgBody->GetString(0).ToString());
      bool isFullscreen = param["fullscreen"].get<bool>();
      if (player) libvlc_set_fullscreen(player, isFullscreen);  // 全屏
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
