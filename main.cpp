#include <windows.h>

#include "App.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine,
                      _In_ int nCmdShow) {
  CefMainArgs main_args(hInstance);
  int exit_code = CefExecuteProcess(main_args, nullptr, nullptr);
  if (exit_code >= 0) {
    DLOG(INFO) << "process exit code" << exit_code;
    return exit_code;
  }
  CefRefPtr<App> app(new App());
  CefSettings settings;
  CefInitialize(main_args, settings, app.get(), nullptr);
  CefRunMessageLoop();
  CefShutdown();
  return 0;
}
