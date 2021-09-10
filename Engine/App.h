#pragma once
#include "../header.h"

class App : public CefApp {
	CefRefPtr<CefRenderProcessHandler> rh;
	CefRefPtr<CefApp> scheme_app;
public:
	App(CefRefPtr<CefRenderProcessHandler> handler, CefRefPtr<CefApp> scheme_app);
	~App();

	CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;

	void OnBeforeCommandLineProcessing(
		const CefString& process_type,
		CefRefPtr<CefCommandLine> command_line) override;

	void OnRegisterCustomSchemes(
		CefRawPtr<CefSchemeRegistrar> registrar) override;

	IMPLEMENT_REFCOUNTING(App);
};

