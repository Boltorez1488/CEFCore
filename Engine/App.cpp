#include "App.h"

App::App(CefRefPtr<CefRenderProcessHandler> handler, CefRefPtr<CefApp> scheme_app) {
	rh = handler;
	this->scheme_app = scheme_app;
}

App::~App() {
	rh->Release();
}

CefRefPtr<CefRenderProcessHandler> App::GetRenderProcessHandler() {
	return rh;
}

void App::OnBeforeCommandLineProcessing(const CefString & process_type, CefRefPtr<CefCommandLine> command_line) {
	command_line->AppendSwitch("disable-gpu");
	command_line->AppendSwitch("disable-web-security");
}

void App::OnRegisterCustomSchemes(CefRawPtr<CefSchemeRegistrar> registrar) {
	if(scheme_app.get() != nullptr) {
		scheme_app->OnRegisterCustomSchemes(registrar);
	}
}
