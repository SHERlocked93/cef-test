#include <windows.h>

#include "App.h"
#include "Other.h"
#include "Renderer.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine,
                      _In_ int nCmdShow) {
  CefMainArgs main_args(hInstance);
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
  command_line->InitFromString(::GetCommandLineW());

  CefRefPtr<CefApp> app;
  if (!command_line->HasSwitch("type")) {
    app = new App();  // 浏览器进程
  } else if (command_line->GetSwitchValue("type").ToString() == "renderer") {
    app = new Renderer();  // 渲染进程
  } else {
    app = new Other();  // GPU进程、辅助进程等
  }

  int exit_code = CefExecuteProcess(main_args, app, nullptr);  // 启动进程
  if (exit_code >= 0) {
    DLOG(INFO) << "process exit code" << exit_code;
    return exit_code;
  }
  CefSettings settings;
  // settings.no_sandbox = 1;
  settings.remote_debugging_port = 9222;        // 指定远程调试端口
  settings.log_severity = LOGSEVERITY_VERBOSE;  // 启用详细日志

  CefInitialize(main_args, settings, app.get(), nullptr);
  CefRunMessageLoop();
  CefShutdown();
  return 0;
}
