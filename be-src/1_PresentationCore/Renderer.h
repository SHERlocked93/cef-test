#pragma once
#include "V8Handler.h"
#include "include/cef_app.h"

class Renderer : public CefApp, public CefRenderProcessHandler {
 public:
  Renderer() = default;
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }
  void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;

  //! 渲染进程接收浏览器进程发来的消息
  bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

  //! 页面的JS执行上下文创建完成
  //! \param context JS执行上下文
  void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefV8Context> context) override;
  // 这个成员变量是private类型的
  CefRefPtr<V8Handler> v8Handler;

 private:
  IMPLEMENT_REFCOUNTING(Renderer);
};
