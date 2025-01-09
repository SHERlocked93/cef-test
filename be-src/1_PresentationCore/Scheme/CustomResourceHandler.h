#pragma once
#include "include/cef_app.h"
#include "include/cef_resource_handler.h"
#include "include/wrapper/cef_helpers.h"

class CustomResourceHandler : public CefResourceHandler {
 public:
  CustomResourceHandler() {}
  CustomResourceHandler(const CustomResourceHandler&) = delete;
  CustomResourceHandler& operator=(const CustomResourceHandler&) = delete;

  // 请求发生时触发的事件
  bool Open(CefRefPtr<CefRequest> request, bool& handle_request, CefRefPtr<CefCallback> callback) override;
  // 发送响应头之前触发的事件
  void GetResponseHeaders(CefRefPtr<CefResponse> response, long long& response_length, CefString& redirectUrl) override;
  // 请求被取消时触发的事件
  void Cancel() override { CEF_REQUIRE_IO_THREAD() }
  //! 响应数据时触发的事件，如响应的数据比较多，这个方法可能会被多次调用，每次调用响应一部分数据内容
  //! \param data_out 输出的内容指针
  //! \param bytes_to_read [in] 本次输出的数据的长度
  //! \param bytes_read [out] 本次真实输出的数据的长度
  //! \param callback
  //! \return
  bool Read(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefResourceReadCallback> callback) override;

 private:
  std::string data_;
  size_t offset_ = 0;  // 记录响应请求时响应数据块的位置
  IMPLEMENT_REFCOUNTING(CustomResourceHandler);
};
