#pragma once
#include "../header.h"

class MenuHandler : public CefContextMenuHandler {
public:
	MenuHandler();
	~MenuHandler();

	bool RunContextMenu(CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefContextMenuParams> params,
		CefRefPtr<CefMenuModel> model,
		CefRefPtr<CefRunContextMenuCallback> callback) override;

	IMPLEMENT_REFCOUNTING(MenuHandler);
};


