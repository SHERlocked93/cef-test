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
    app = new App();
  } else if (command_line->GetSwitchValue("type").ToString() == "renderer") {
    app = new Renderer();
  } else {
    app = new Other();
  }

  int exit_code = CefExecuteProcess(main_args, app, nullptr);
  if (exit_code >= 0) {
    DLOG(INFO) << "process exit code" << exit_code;
    return exit_code;
  }
  CefSettings settings;
  settings.no_sandbox = 1;

  CefInitialize(main_args, settings, app.get(), nullptr);
  CefRunMessageLoop();
  CefShutdown();
  return 0;
}
