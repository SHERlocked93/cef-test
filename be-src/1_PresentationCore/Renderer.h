#pragma once
#include "include/cef_app.h"
class Renderer:public CefApp, public CefRenderProcessHandler
{
public:
	Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override { return this; }
	void OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) override;
private:
	IMPLEMENT_REFCOUNTING(Renderer);
};

