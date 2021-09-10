#include "RenderProcessHandler.h"

RenderProcessHandler::RenderProcessHandler(std::vector<std::shared_ptr<Browser>>* browsers) {
	browsers_ = browsers;
}

RenderProcessHandler::~RenderProcessHandler() {
}

void RenderProcessHandler::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
	for (auto it = browsers_->begin(); it != browsers_->end(); ++it) {
		auto b = *it;
		if(b->get()->GetIdentifier() == browser->GetIdentifier()) {
			b->OnContextCreated(browser, frame, context);
			break;
		}
	}
}
