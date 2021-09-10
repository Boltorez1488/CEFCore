#pragma once
#include "../header.h"

class JSHandler : public CefV8Handler {
public:
	virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) = 0;
	IMPLEMENT_REFCOUNTING(JSHandler)
};