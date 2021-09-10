#pragma once
#include "App.h"
#include "Browser.h"
#include "../Handlers/RenderProcessHandler.h"

class Chromium {
	CefRefPtr<App> app_;
	CefRefPtr<RenderProcessHandler> process_handler_;
	std::vector<std::shared_ptr<Browser>> browsers_;
	DX9Render* pRender = nullptr;
	bool is_mouse_capture_ = false;
public:
	Chromium(LPDIRECT3DDEVICE9 pDevice, char* res_dir, CefRefPtr<CefApp> scheme_app);
	~Chromium();

	Browser* create_browser(char* url);
	Browser* create_browser(char* url, CefRect rect);

	void render() const;
	void pre_reset(int w, int h) const;
	void post_reset() const;

	void resize(int w, int h) const;

	bool wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

