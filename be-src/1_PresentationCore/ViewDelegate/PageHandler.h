#pragma once
#include <list>

#include "include/cef_app.h"
class PageHandler : public CefClient, public CefLifeSpanHandler, public CefJSDialogHandler {
 public:
  PageHandler() = default;

  CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override { return this; }

  //! 阻止页面关闭的对话框弹出之前触发的事件
  //! \param browser 浏览器对象
  //! \param message_text 提示文本字符串
  //! \param is_reload 当前关闭页面的行为是不是刷新行为
  //! \param callback
  //! \return
  bool OnBeforeUnloadDialog(CefRefPtr<CefBrowser> browser, const CefString& message_text, bool is_reload,
                            CefRefPtr<CefJSDialogCallback> callback) override;

  //! 弹出对话框事件
  //! \param browser 浏览器对象
  //! \param origin_url 弹出对话框的页面的url
  //! \param dialog_type 弹出对话框的类型 alert、confirm、prompt
  //! \param message_text JS提供的弹出对话框所承载的文本信息
  //! \param default_prompt_text 显示在prompt对话框中输入框的默认值
  //! \param callback 把用户做出的选择通知回调
  //! \param suppress_message 要不要屏蔽默认对话框
  //! \return
  bool OnJSDialog(CefRefPtr<CefBrowser> browser, const CefString& origin_url, JSDialogType dialog_type,
                  const CefString& message_text, const CefString& default_prompt_text,
                  CefRefPtr<CefJSDialogCallback> callback, bool& suppress_message) override;

  //! 用于处理页面生命周期中的事件
  CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
  //! 页面创建成功
  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  //! 页面即将关闭
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;

 private:
  IMPLEMENT_REFCOUNTING(PageHandler);
  std::list<CefRefPtr<CefBrowser>> browsers;
};
