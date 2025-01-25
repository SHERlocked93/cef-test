#pragma once
#include <nlohmann/json.hpp>

#include "include/cef_browser.h"
#include "include/wrapper/cef_message_router.h"

using nlohmann::json;

class DialogHandler : public CefRunFileDialogCallback {
 public:
  DialogHandler(std::string& msgName_, CefRefPtr<CefFrame> frame_) : msgName(msgName_), frame(frame_) {}

  void OnFileDialogDismissed(const std::vector<CefString>& file_paths) override {
    json result;
    result["success"] = true;
    result["data"] = {};
    for (const auto& file_path : file_paths) {
      result["data"].push_back(file_path.ToString());
    }
    CefRefPtr<CefProcessMessage> msgBack = CefProcessMessage::Create(msgName);
    CefRefPtr<CefListValue> msgArgs = msgBack->GetArgumentList();
    std::string dataStr = result.dump();
    msgArgs->SetString(0, dataStr);
    frame->SendProcessMessage(PID_RENDERER, msgBack);
  }
  DialogHandler(const DialogHandler&) = delete;
  DialogHandler& operator=(const DialogHandler&) = delete;

 private:
  std::string msgName;
  CefRefPtr<CefFrame> frame;
  IMPLEMENT_REFCOUNTING(DialogHandler);
};
