#pragma once
#include "../header.h"
#include "../Engine/Browser.h"

class RenderProcessHandler : public CefRenderProcessHandler {
	std::vector<std::shared_ptr<Browser>>* browsers_;
public:
	RenderProcessHandler(std::vector<std::shared_ptr<Browser>>* browsers);
	~RenderProcessHandler();

	void OnContextCreated(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefV8Context> context) override;

	IMPLEMENT_REFCOUNTING(RenderProcessHandler)
};

