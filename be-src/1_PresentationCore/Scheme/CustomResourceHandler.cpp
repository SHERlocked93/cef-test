#include "CustomResourceHandler.h"

#include <filesystem>
#include <fstream>

#include "include/wrapper/cef_helpers.h"
#undef min

bool CustomResourceHandler::Open(CefRefPtr<CefRequest> request, bool& handle_request,
                                 CefRefPtr<CefCallback> callback) {
  DCHECK(!CefCurrentlyOn(TID_UI) && !CefCurrentlyOn(TID_IO));
  handle_request = true;
  bool handled = true;
  this->data_ = "<html><head><title>hello</title></head><body><h1>world!!</h1></body></html>";

  return handled;
}

void CustomResourceHandler::GetResponseHeaders(CefRefPtr<CefResponse> response, long long& response_length,
                                               CefString& redirectUrl) {
  CEF_REQUIRE_IO_THREAD()
  DCHECK(!data_.empty());
  response->SetMimeType("text/html");
  response->SetStatus(200);
  response_length = data_.length();
}

bool CustomResourceHandler::Read(void* data_out, int bytes_to_read, int& bytes_read,
                                 CefRefPtr<CefResourceReadCallback> callback) {
  DCHECK(!CefCurrentlyOn(TID_UI) && !CefCurrentlyOn(TID_IO));
  bool has_data = false;
  bytes_read = 0;

  if (offset_ < data_.length()) {
    int transfer_size = std::min(bytes_to_read, static_cast<int>(data_.length() - offset_));
    memcpy(data_out, data_.c_str() + offset_, transfer_size);
    offset_ += transfer_size;
    bytes_read = transfer_size;
    has_data = true;
  }
  return has_data;
}
