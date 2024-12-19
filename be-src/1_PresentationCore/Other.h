#pragma once
#include "include/cef_app.h"

class Other : public CefApp {
 public:
  Other() = default;
  Other(const Other&) = delete;
  Other& operator=(const Other&) = delete;
  void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;

 private:
  IMPLEMENT_REFCOUNTING(Other);
};
