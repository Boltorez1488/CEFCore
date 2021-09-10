#pragma once
#include "Input.h"
#include "BrowserClient.h"
#include "JSHandler.h"
#include "../Render/DX9Render.h"
#include "../Handlers/MenuHandler.h"
#include <wrl/client.h>

class Browser : public CefRenderHandler,
public CefClient, public CefV8Handler,
public CefLifeSpanHandler, public CefRequestHandler {
	CefRefPtr<CefBrowser> browser_;
	CefRefPtr<CefContextMenuHandler> mh = new MenuHandler();
	Input* input_;

	std::vector<CefRefPtr<JSHandler>> js_handlers_;

	DX9Render* pRender = nullptr;
	Microsoft::WRL::ComPtr<IDirect3DTexture9> pTexture = nullptr, pPTexture = nullptr;

	int x, y, width, height;
	CefRect popup = { 0, 0, 0, 0 };

	bool is_fs_ = false;
	bool is_hidden_ = false;
	bool is_mouse_lock_ = false;
	bool is_minimized_ = false;
	bool is_render = false;

	void recreate_texture();
public:
	Browser(DX9Render* pRender, char* url);
	Browser(DX9Render* pRender, char* url, CefRect rect);
	~Browser();

	CefRefPtr<CefBrowser> get() const;
	CefRect get_rect() const;
	void set_rect(CefRect rect);
	bool check_pixel(int x, int y) const;
	bool wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	//########################[ Client ]########################//
	CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override;
	CefRefPtr<CefRenderHandler> GetRenderHandler() override;
	CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override;
	CefRefPtr<CefRequestHandler> GetRequestHandler() override;

	bool OnOpenURLFromTab(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, CefRequestHandler::WindowOpenDisposition target_disposition, bool user_gesture) override;
	bool OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url, const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture, const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client, CefBrowserSettings& settings, bool* no_javascript_access) override;
	
	//########################[ JavaScript ]########################//
	void add_js_handler(CefRefPtr<JSHandler> js);
	void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context);
	bool Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception) override;
	void ExecuteJavaScript(const CefString& code, const CefString& script_url, int start_line) const;
	void ExecuteJavaScript(const CefString& code) const;

	//########################[ RenderHandler ]########################//
	bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) override;
	void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) override;
	void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) override;
	void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects, const void* buffer, int width, int height) override;
	void pre_reset(int w, int h);
	void post_reset();
	void render() const;

	//########################[ Browser Functions ]########################//
	void show_dev_tools();
	void close_dev_tools() const;
	void resize(int w, int h);
	void set_fullscreen();
	bool is_fullscreen() const;
	void hide();
	void show();
	bool is_hidden() const;

	IMPLEMENT_REFCOUNTING(Browser);
};

