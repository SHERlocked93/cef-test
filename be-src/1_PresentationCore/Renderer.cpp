#include "Renderer.h"
#include "ReleaseCallback.h"

const std::string CEF_JS_BRIDGE_NAME = "globalBridge";

void Renderer::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {
  registrar->AddCustomScheme("panda", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_CORS_ENABLED);
}

bool Renderer::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                        CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
  CefString messageName = message->GetName();
  CefRefPtr<CefListValue> args = message->GetArgumentList();
  CefRefPtr<CefV8Context> context = frame->GetV8Context();
  context->Enter();
  CefRefPtr<CefV8Value> messageNameV8 = CefV8Value::CreateString(messageName);
  CefV8ValueList argsForJs;
  argsForJs.push_back(messageNameV8);

  if (args->GetType(0) == CefValueType::VTYPE_STRING) {
    CefString result = args->GetString(0);
    CefRefPtr<CefV8Value> resultV8 = CefV8Value::CreateString(result);
    argsForJs.push_back(resultV8);
  } else if (args->GetType(0) == CefValueType::VTYPE_BINARY) {
    CefRefPtr<CefBinaryValue> data = args->GetBinary(0);
    size_t size = data->GetSize();
    unsigned char* result = new unsigned char[size];
    data->GetData(result, size, 0);
    CefRefPtr<CefV8ArrayBufferReleaseCallback> cb = new ReleaseCallback();
    CefRefPtr<CefV8Value> resultV8 = CefV8Value::CreateArrayBuffer(result, size, cb);
    argsForJs.push_back(resultV8);
  }

  v8Handler->cb_->ExecuteFunction(nullptr, argsForJs);
  context->Exit();
  return true;
}

void Renderer::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) {
  CefRefPtr<CefV8Value> globalObject = context->GetGlobal();  // JS全局对象window
  v8Handler = new V8Handler();
  CefRefPtr<CefV8Value> jsBridge = CefV8Value::CreateFunction(CEF_JS_BRIDGE_NAME, v8Handler);
  globalObject->SetValue(CEF_JS_BRIDGE_NAME, jsBridge, V8_PROPERTY_ATTRIBUTE_READONLY);
}
