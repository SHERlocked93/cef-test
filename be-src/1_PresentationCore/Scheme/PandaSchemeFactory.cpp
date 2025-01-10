#include "PandaSchemeFactory.h"

#include <filesystem>
#include <fstream>

// #include "./CustomResourceHandler.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_stream_resource_handler.h"

CefRefPtr<CefResourceHandler> PandaSchemeFactory::Create(CefRefPtr<CefBrowser> browser,
                                                         CefRefPtr<CefFrame> frame,
                                                         const CefString& scheme_name,
                                                         CefRefPtr<CefRequest> request) {
  // 确保此方法运行在IO线程上，这类线程只负责处理IPC消息和网络消息（IPC消息是CEF框架用于进程间通信的消息），避免在此类线程上执行阻塞性工作
  CEF_REQUIRE_IO_THREAD()
  std::string url = request->GetURL().ToString();
  std::string urlPrefix = "panda://";

  if (url.starts_with(urlPrefix)) url.erase(0, urlPrefix.size());

  std::ranges::replace(url, '/', '\\');

  size_t paramIndex = url.find_first_of('?');
  if (paramIndex != std::string::npos) url.erase(paramIndex);

  TCHAR buffer[MAX_PATH] = {0};
  GetModuleFileName(NULL, buffer, MAX_PATH);
  std::filesystem::path targetPath(buffer);
  targetPath = targetPath / ".." / ".." / ".." / "fe-dist" / url;
  if (!std::filesystem::exists(targetPath))
    DLOG(INFO) << "try load: " << targetPath.generic_wstring() << ", but cannot find!";

  std::string ext = targetPath.extension().generic_string();
  std::string mime_type_ = ext == ".html" ? "text/html" : "*";
  auto stream = CefStreamReader::CreateForFile(targetPath.generic_wstring());

  return new CefStreamResourceHandler(mime_type_, stream);
  // return new CustomResourceHandler();
};
