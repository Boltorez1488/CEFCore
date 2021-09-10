#pragma once
#include "../header.h"

class BrowserClient : public CefClient, public CefLifeSpanHandler, public CefLoadHandler {
	int browser_id = 0;
	bool closing = false;
	bool loaded = false;
	CefRefPtr<CefRenderHandler> handler;
	CefRefPtr<CefContextMenuHandler> mh;
public:
	BrowserClient(CefRefPtr<CefRenderHandler> ptr, CefRefPtr<CefContextMenuHandler> mh);

	CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override;
	CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
	CefRefPtr<CefLoadHandler> GetLoadHandler() override;
	CefRefPtr<CefRenderHandler> GetRenderHandler() override;

	void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
	bool DoClose(CefRefPtr<CefBrowser> browser) override;
	void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;
	void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) override;
	void OnLoadError(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString & failedUrl, CefString & errorText);
	void OnLoadingStateChange(CefRefPtr<CefBrowser> browser, bool isLoading, bool canGoBack, bool canGoForward) override;
	void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame);

	bool closeAllowed() const;
	bool isLoaded() const;

	IMPLEMENT_REFCOUNTING(BrowserClient);
};

