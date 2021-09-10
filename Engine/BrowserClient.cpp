#include "BrowserClient.h"

BrowserClient::BrowserClient(CefRefPtr<CefRenderHandler> ptr, CefRefPtr<CefContextMenuHandler> mh) {
	handler = ptr;
	this->mh = mh;
}

CefRefPtr<CefContextMenuHandler> BrowserClient::GetContextMenuHandler() {
	return mh;
}

CefRefPtr<CefLifeSpanHandler> BrowserClient::GetLifeSpanHandler() {
	return this;
}

CefRefPtr<CefLoadHandler> BrowserClient::GetLoadHandler() {
	return this;
}

CefRefPtr<CefRenderHandler> BrowserClient::GetRenderHandler() {
	return handler;
}

void BrowserClient::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();
	browser_id = browser->GetIdentifier();
}

bool BrowserClient::DoClose(CefRefPtr<CefBrowser> browser) {
	CEF_REQUIRE_UI_THREAD();
	if (browser->GetIdentifier() == browser_id) {
		closing = true;
	}
	return false;
}

void BrowserClient::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
}

void BrowserClient::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) {
	loaded = true;
}

void BrowserClient::OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString & failedUrl, CefString & errorText) {
	loaded = true;
}

void BrowserClient::OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) {
}

void BrowserClient::OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame) {
}

bool BrowserClient::closeAllowed() const {
	return closing;
}

bool BrowserClient::isLoaded() const {
	return loaded;
}