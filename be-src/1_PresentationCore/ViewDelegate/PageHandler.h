#pragma once
#include <list>

#include "include/cef_app.h"
class PageHandler : public CefClient,
                    public CefLifeSpanHandler,     // 页面生命周期
                    public CefJSDialogHandler,     // JS的弹框
                    public CefContextMenuHandler,  // 右键菜单
                    public CefDragHandler          // 拖拽事件
{
 public:
  PageHandler() = default;

  CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }

  //! 浏览器进程接收渲染进程发来的消息
  bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                CefProcessId source_process, CefRefPtr<CefProcessMessage> message) override;

  //! 右键展开菜单时
  //! \param browser 浏览器对象
  //! \param frame 页面上的iframe
  //! \param params 获得用户当前鼠标所在的位置、鼠标所在位置的页面元素的类型、当前页面的路径等信息
  //! \param model 默认的菜单项
  void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                           CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model) override;

  //! 点击了右键菜单某项时的事件
  //! \param browser
  //! \param frame
  //! \param params
  //! \param command_id 获取用户点击了具体的哪个菜单项
  //! \param event_flags 获取按下鼠标右键时，是否同时按下了 Ctrl 键或者 Alt 键等信息
  //! \return
  bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                            CefRefPtr<CefContextMenuParams> params, int command_id,
                            EventFlags event_flags) override;

  CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override { return this; }

  //! 阻止页面关闭的对话框弹出之前触发的事件
  //! \param browser 浏览器对象
  //! \param message_text 提示文本字符串
  //! \param is_reload 当前关闭页面的行为是否刷新行为
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

  CefRefPtr<CefDragHandler> GetDragHandler() override { return this; }

  void OnDraggableRegionsChanged(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                 const std::vector<CefDraggableRegion>& regions) override;

 private:
  IMPLEMENT_REFCOUNTING(PageHandler);
  std::list<CefRefPtr<CefBrowser>> browsers;
};
