#include "PandaSchemeFactory.h"

#include <filesystem>
#include <fstream>

#include "./CustomSchemeHandler.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_stream_resource_handler.h"

CefRefPtr<CefResourceHandler> PandaSchemeFactory::Create(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame,
                                                         const CefString& scheme_name, CefRefPtr<CefRequest> request) {
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

  return new CustomSchemeHandler();
};
