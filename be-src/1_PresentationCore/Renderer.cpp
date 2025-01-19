#include "Renderer.h"

const std::string CEF_JS_BRIDGE_NAME = "globalBridge";

void Renderer::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {
  registrar->AddCustomScheme("panda", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_CORS_ENABLED);
}

void Renderer::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) {
  CefRefPtr<CefV8Value> globalObject = context->GetGlobal();  // JS全局对象window
  v8Handler = new V8Handler();
  CefRefPtr<CefV8Value> nativeCall = CefV8Value::CreateFunction(CEF_JS_BRIDGE_NAME, v8Handler);
  globalObject->SetValue(CEF_JS_BRIDGE_NAME, nativeCall, V8_PROPERTY_ATTRIBUTE_READONLY);
}
