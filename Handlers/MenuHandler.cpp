#include "MenuHandler.h"

MenuHandler::MenuHandler() {
}


MenuHandler::~MenuHandler() {
}

bool MenuHandler::RunContextMenu(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefContextMenuParams> params, CefRefPtr<CefMenuModel> model, CefRefPtr<CefRunContextMenuCallback> callback) {
	return true;
}