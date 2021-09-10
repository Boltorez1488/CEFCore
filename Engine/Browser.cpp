#include "Browser.h"

void Browser::recreate_texture() {
	pTexture.Reset();
	pPTexture.Reset();
	D3DXCreateTexture(pRender->get_device(), width, height, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pTexture);
	D3DXCreateTexture(pRender->get_device(), width, height, D3DX_DEFAULT, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pPTexture);
}

Browser::Browser(DX9Render* pRender, char* url) {
	this->pRender = pRender;
	const auto rect = pRender->get_rect();

	x = rect.left;
	y = rect.top;
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	is_fs_ = true;

	CefWindowInfo window_info;
	window_info.SetAsWindowless(pRender->get_hwnd());

	CefBrowserSettings browser_settings;
	browser_settings.windowless_frame_rate = 300;

	browser_ = CefBrowserHost::CreateBrowserSync(window_info, this, url, browser_settings, nullptr);
	input_ = new Input(browser_);
	input_->set_offset(x, y);

	recreate_texture();
}

Browser::Browser(DX9Render* pRender, char* url, CefRect rect) {
	this->pRender = pRender;

	x = rect.x;
	y = rect.y;
	width = rect.width;
	height = rect.height;
	
	CefWindowInfo window_info;
	window_info.SetAsWindowless(pRender->get_hwnd());
	
	CefBrowserSettings browser_settings;
	browser_settings.windowless_frame_rate = 300;
	//CefString(&browser_settings.default_encoding) = "utf-8";

	browser_ = CefBrowserHost::CreateBrowserSync(window_info, this, url, browser_settings, nullptr);
	input_ = new Input(browser_);
	input_->set_offset(x, y);

	recreate_texture();
	if (x == 0 && y == 0) {
		const auto rc = pRender->get_rect();
		const int w = rc.right - rc.left;
		const int h = rc.bottom - rc.top;
		if (width == w && height == h) {
			is_fs_ = true;
			return;
		}
	}
	is_fs_ = false;
}

Browser::~Browser() {
	delete input_;
	pTexture.Reset();
	pPTexture.Reset();
}

CefRefPtr<CefBrowser> Browser::get() const {
	return browser_;
}

CefRect Browser::get_rect() const {
	return CefRect(x, y, width, height);
}

void Browser::set_rect(CefRect rect) {
	x = rect.x;
	y = rect.y;
	width = rect.width;
	height = rect.height;
	recreate_texture();
	browser_->GetHost()->WasResized();
	input_->set_offset(x, y);
	if(x == 0 && y == 0) {
		const auto rc = pRender->get_rect();
		const int w = rc.right - rc.left;
		const int h = rc.bottom - rc.top;
		if(width == w && height == h) {
			is_fs_ = true;
			return;
		}
	}
	is_fs_ = false;
}

bool Browser::check_pixel(int x, int y) const {
	if (x > width || y > width)
		return false;

	auto texture = pTexture.Get();
	if (x > popup.x && x < popup.x + popup.width && y > popup.y && y < popup.y + popup.height) {
		texture = pPTexture.Get();
	}
	if (texture == nullptr)
		return false;

	bool result = false;
	D3DLOCKED_RECT rect;
	texture->LockRect(0, &rect, NULL, 0);

	DWORD* imageData = (DWORD*)rect.pBits;
	const int index = y * rect.Pitch / 4 + x;
	BYTE* pixel = (BYTE*)&imageData[index];
	if (pixel[3] != 0) {
		result = true;
	}

	texture->UnlockRect(0);
	return result;
}

bool Browser::wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (is_hidden_)
		return false;
	POINT mouse;
	mouse.x = 0;
	mouse.y = 0;
	switch(uMsg) {
		default:
			input_->wnd_proc(hWnd, uMsg, wParam, lParam);
			return false;
		case WM_SYSCHAR:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONUP:
		case WM_MBUTTONUP:
			goto mouse;
		case WM_MOUSEMOVE:
			input_->wnd_proc(hWnd, uMsg, wParam, lParam);
		case WM_MOUSELEAVE:
		case WM_MOUSEWHEEL:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDBLCLK:
		mouse:
			GetCursorPos(&mouse);
			ScreenToClient(hWnd, &mouse);
			//mouse.x -= x; mouse.y -= y;
			if (mouse.x > x && mouse.x < x + width &&
				mouse.y > y && mouse.y < y + height) {
				input_->wnd_proc(hWnd, uMsg, wParam, lParam);
				if (check_pixel(mouse.x, mouse.y))
					return true;
				if (!is_fs_ || is_mouse_lock_) {
					return true;
				}
			}
		case WM_SYSCOMMAND:
			if(wParam == SC_MINIMIZE) {
				is_minimized_ = true;
			} else {
				is_minimized_ = false;
			}
			return false;
	}
	return false;
}

CefRefPtr<CefContextMenuHandler> Browser::GetContextMenuHandler() {
	return mh;
}

CefRefPtr<CefRenderHandler> Browser::GetRenderHandler() {
	return this;
}

CefRefPtr<CefLifeSpanHandler> Browser::GetLifeSpanHandler() {
	return this;
}

CefRefPtr<CefRequestHandler> Browser::GetRequestHandler() {
	return this;
}

bool Browser::OnOpenURLFromTab(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url,
	CefRequestHandler::WindowOpenDisposition target_disposition, bool user_gesture) {
	std::string url = "openUrl('"; url += target_url; url += "');";
	browser_->GetMainFrame()->ExecuteJavaScript(url, "", 0);
	return true;
}

bool Browser::OnBeforePopup(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, const CefString& target_url,
	const CefString& target_frame_name, CefLifeSpanHandler::WindowOpenDisposition target_disposition, bool user_gesture,
	const CefPopupFeatures& popupFeatures, CefWindowInfo& windowInfo, CefRefPtr<CefClient>& client,
	CefBrowserSettings& settings, bool* no_javascript_access) {
	std::string url = "openUrl('"; url += target_url; url += "');";
	browser_->GetMainFrame()->ExecuteJavaScript(url, "", 0);
	return true;
}

void Browser::add_js_handler(CefRefPtr<JSHandler> js) {
	js_handlers_.push_back(js);
}

void Browser::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) {
	auto object = context->GetGlobal();
	object->SetValue("mouselock", CefV8Value::CreateFunction("mouselock", this), V8_PROPERTY_ATTRIBUTE_NONE);
	object->SetValue("mouseunlock", CefV8Value::CreateFunction("mouseunlock", this), V8_PROPERTY_ATTRIBUTE_NONE);
	for each(auto obj in js_handlers_) {
		obj->OnContextCreated(browser, frame, context);
	}
}

bool Browser::Execute(const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments,
	CefRefPtr<CefV8Value>& retval, CefString& exception) {
	if (name == "mouselock") {
		is_mouse_lock_ = true;
		return true;
	}
	if (name == "mouseunlock") {
		is_mouse_lock_ = false;
		return true;
	}
	for (auto obj : js_handlers_) {
		if(obj->Execute(name, object, arguments, retval, exception)) {
			return true;
		}
	}
	return false;
}

void Browser::ExecuteJavaScript(const CefString& code, const CefString& script_url, int start_line) const {
	browser_->GetMainFrame()->ExecuteJavaScript(code, script_url, start_line);
}

void Browser::ExecuteJavaScript(const CefString& code) const {
	auto frame = browser_->GetMainFrame();
	frame->ExecuteJavaScript(code, frame->GetURL(), 0);
}

bool Browser::GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) {
	rect = CefRect(x, y, width, height);
	return true;
}

void Browser::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) {
	if(!show) {
		popup = { 0, 0, 0, 0 };
		if (pPTexture.Get()) {
			const unsigned row_size = width * 4;
			D3DLOCKED_RECT rect;
			if (pPTexture->LockRect(0, &rect, 0, 0) == S_OK) {
				BYTE *p_dst = static_cast<BYTE*>(rect.pBits);
				for (int row = 0; row < height; ++row)
					memset(p_dst + row * rect.Pitch, 0x0, row_size);
				pPTexture->UnlockRect(0);
			}
		}
	}
}

void Browser::OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect & rect) {
	popup = rect;
}

void Browser::OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType type, const RectList& dirtyRects,
	const void* buffer, int w, int h) {
	if (is_minimized_)
		return;
	if (type == PET_VIEW && pTexture.Get()) {
		const unsigned row_size = w * 4;
		const auto src = (DWORD*)buffer;

		D3DLOCKED_RECT rect;
		RECT rc;
		rc.left = 0;
		rc.top = 0;
		rc.right = w;
		rc.bottom = h;

		if(pTexture->LockRect(0, &rect, &rc, 0) == S_OK) {
			DWORD *p_dst = static_cast<DWORD*>(rect.pBits);
			bool flag = false;
			for (int i = 0; i < h; i++) {
				for (int j = 0; j < w; j++) {
					const int index = i * rect.Pitch / 4 + j;
					p_dst[index] = src[i * row_size / 4 + j];
					if (p_dst[index] != 0x0)
						flag = true;
				}
			}
			is_render = flag;
			/*for (int row = 0; row < h; ++row) {
				memcpy(p_dst + row * rect.Pitch, src + row * row_size, row_size);
			}*/
			pTexture->UnlockRect(0);
		}
	} 
	if (type == PET_POPUP && pPTexture.Get()) {
		const unsigned row_size = w * 4;
		const auto src = (char*)buffer;

		D3DLOCKED_RECT rect;
		RECT rc;
		rc.left = popup.x;
		rc.top = popup.y;
		rc.right = popup.x + popup.width;
		rc.bottom = popup.y + popup.height;

		if (pPTexture->LockRect(0, &rect, &rc, 0) == S_OK) {
			BYTE *p_dst = static_cast<BYTE*>(rect.pBits);
			for (int row = 0; row < h; ++row)
				memcpy(p_dst + row * rect.Pitch, src + row * row_size, row_size);
			pPTexture->UnlockRect(0);
		}
	}
}

void Browser::pre_reset(int w, int h) {
	pTexture.Reset();
	pPTexture.Reset();
	if (is_fullscreen() || width > w || height > h) {
		x = 0;
		y = 0;
		width = w;
		height = h;
	}
}

void Browser::post_reset() {
	recreate_texture();
	browser_->GetHost()->WasResized();
}

void Browser::render() const {
	if(!is_render)
		return;
	RECT rc = { x, y, x + width, y + height };
	pRender->begin();
	pRender->draw_texture(pTexture.Get(), &rc);
	if(popup.width != 0)
		pRender->draw_texture(pPTexture.Get(), &rc);
	pRender->end();
}

void Browser::show_dev_tools() {
	if (browser_->GetHost()->HasDevTools())
		return;
	CefWindowInfo info;
	info.SetAsPopup(nullptr, "DevTools");
	const CefBrowserSettings settings;
	browser_->GetHost()->ShowDevTools(info, this, settings, CefPoint());
}

void Browser::close_dev_tools() const {
	if(browser_->GetHost()->HasDevTools())
		browser_->GetHost()->CloseDevTools();
}

void Browser::resize(int w, int h) {
	if (is_fullscreen()) {
		set_rect(CefRect(0, 0, w, h));
	}
}

void Browser::set_fullscreen() {
	const auto rect = pRender->get_rect();
	x = rect.left;
	y = rect.top;
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
	recreate_texture();
	browser_->GetHost()->WasResized();
	input_->set_offset(x, y);
	is_fs_ = true;
}

bool Browser::is_fullscreen() const {
	return is_fs_;
}

void Browser::hide() {
	browser_->GetHost()->WasHidden(true);
	is_hidden_ = true;
}

void Browser::show() {
	browser_->GetHost()->WasHidden(false);
	is_hidden_ = false;
}

bool Browser::is_hidden() const {
	return is_hidden_;
}
