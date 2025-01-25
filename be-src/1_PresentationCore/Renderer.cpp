#include "Renderer.h"

const std::string CEF_JS_BRIDGE_NAME = "globalBridge";

void Renderer::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {
  registrar->AddCustomScheme("panda", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_CORS_ENABLED);
}

bool Renderer::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                        CefProcessId source_process, CefRefPtr<CefProcessMessage> message) {
  CefString messageName = message->GetName();
  CefRefPtr<CefListValue> args = message->GetArgumentList();
  CefString result = args->GetString(0);
  CefRefPtr<CefV8Value> messageNameV8 = CefV8Value::CreateString(messageName);
  CefRefPtr<CefV8Value> resultV8 = CefV8Value::CreateString(result);
  CefV8ValueList argsForJs;

  argsForJs.push_back(messageNameV8);
  argsForJs.push_back(resultV8);
  CefRefPtr<CefV8Context> context = frame->GetV8Context();
  context->Enter();
  if (v8Handler->cb_) v8Handler->cb_->ExecuteFunction(nullptr, argsForJs);
  context->Exit();
  return true;
}

void Renderer::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) {
  CefRefPtr<CefV8Value> globalObject = context->GetGlobal();  // JS全局对象window
  v8Handler = new V8Handler();
  CefRefPtr<CefV8Value> nativeCall = CefV8Value::CreateFunction(CEF_JS_BRIDGE_NAME, v8Handler);
  globalObject->SetValue(CEF_JS_BRIDGE_NAME, nativeCall, V8_PROPERTY_ATTRIBUTE_READONLY);
}
