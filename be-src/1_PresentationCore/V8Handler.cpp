#include "V8Handler.h"

bool V8Handler::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments,
                        CefRefPtr<CefV8Value>& retval, CefString& exception) {
  auto msgName = arguments[0]->GetStringValue();
  if (msgName == "native_registe_callback") {
    cb_ = arguments[1];
    return true;
  }

  CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(msgName);  // 创建进程间消息

  CefRefPtr<CefListValue> msgBody = msg->GetArgumentList();
  if (arguments.size() > 1            // 传递过来多于1个的参数
      && arguments[1]->IsString()) {  // 第二个参数是字符串类型
    msgBody->SetString(0, arguments[1]->GetStringValue());
  }

  CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
  context.get()->GetFrame()->SendProcessMessage(PID_BROWSER, msg);  // 由渲染进程发给浏览器进程

  return true;  // 表示已处理这个方法
};
