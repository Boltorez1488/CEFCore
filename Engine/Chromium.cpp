#include "Chromium.h"

Chromium::Chromium(LPDIRECT3DDEVICE9 pDevice, char* res_dir, CefRefPtr<CefApp> scheme_app) {
	CefEnableHighDPISupport();

	DeleteFile("debug.log");

	process_handler_ = new RenderProcessHandler(&browsers_);
	app_ = new App(process_handler_, scheme_app);

	const CefMainArgs args(GetModuleHandle(nullptr));
	CefExecuteProcess(args, nullptr, nullptr);

	CefSettings settings;
	settings.single_process = true;
	settings.no_sandbox = true;
	settings.command_line_args_disabled = STATE_ENABLED;
	//settings.persist_session_cookies = STATE_ENABLED;

	std::string dir;
	dir.resize(MAX_PATH);
	GetCurrentDirectoryA(MAX_PATH, const_cast<LPSTR>(dir.data()));
	CefString(&settings.resources_dir_path) = dir.c_str() + std::string("\\") + res_dir;
	CefString(&settings.locales_dir_path) = dir.c_str() + std::string("\\") + res_dir + std::string("\\locales");
	//CefString(&settings.cache_path) = dir.c_str() + std::string("\\CEF");

	CefInitialize(args, settings, app_, nullptr);

	pRender = new DX9Render(pDevice);
}

Chromium::~Chromium() {
	for each(auto obj in browsers_) {
		obj.reset();
	}
	browsers_.clear();
	delete pRender;
}

Browser* Chromium::create_browser(char* url) {
	std::shared_ptr<Browser> browser(new Browser(pRender, url));
	browsers_.push_back(browser);
	return browser.get();
}

Browser* Chromium::create_browser(char* url, CefRect rect) {
	std::shared_ptr<Browser> browser(new Browser(pRender, url, rect));
	browsers_.push_back(browser);
	return browser.get();
}

void Chromium::render() const {
	browsers_[0]->render();
	/*const auto size = browsers_.size();
	for (auto i = size; i > 0; i--) {
		const auto b = browsers_[i-1].get();
		b->render();
	}*/
}

void Chromium::pre_reset(int w, int h) const {
	for each(auto obj in browsers_) {
		obj->pre_reset(w, h);
	}
}

void Chromium::post_reset() const {
	for each(auto obj in browsers_) {
		obj->post_reset();
	}
}

void Chromium::resize(int w, int h) const {
	for each(auto obj in browsers_) {
		obj->resize(w, h);
	}
}

bool Chromium::wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	const auto size = browsers_.size();
	for(size_t i = 0; i < size; i++) {
		const auto browser = browsers_[i];
		if (browser->wnd_proc(hWnd, uMsg, wParam, lParam)) {
			switch(uMsg) {
				default:
					break;
				case WM_SYSCHAR:
				case WM_SYSKEYDOWN:
				case WM_KEYDOWN:
				case WM_CHAR:
				case WM_LBUTTONDOWN:
				case WM_RBUTTONDOWN:
				case WM_MBUTTONDOWN:
				case WM_LBUTTONDBLCLK:
				case WM_RBUTTONDBLCLK:
				case WM_MBUTTONDBLCLK:
					if (size > 1 && i != 0) {
						for (int j = i; j > 0; j--) {
							browsers_[j] = browsers_[j - 1];
						}
						browsers_[0] = browser;
					}
			}
			return true;
		}
	}
	return is_mouse_capture_;
}
