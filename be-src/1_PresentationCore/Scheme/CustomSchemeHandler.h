#pragma once
#include "include/cef_app.h"
#include "include/cef_resource_handler.h"
#include "include/wrapper/cef_helpers.h"

class CustomSchemeHandler : public CefResourceHandler {
 public:
  CustomSchemeHandler() {}
  CustomSchemeHandler(const CustomSchemeHandler&) = delete;
  CustomSchemeHandler& operator=(const CustomSchemeHandler&) = delete;
  bool Open(CefRefPtr<CefRequest> request, bool& handle_request, CefRefPtr<CefCallback> callback) override;
  void GetResponseHeaders(CefRefPtr<CefResponse> response, long long& response_length, CefString& redirectUrl) override;
  void Cancel() override { CEF_REQUIRE_IO_THREAD(); }
  bool Read(void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefResourceReadCallback> callback) override;

 private:
  std::string data_;
  size_t offset_ = 0;
  IMPLEMENT_REFCOUNTING(CustomSchemeHandler);
};
