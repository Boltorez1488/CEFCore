#pragma once
#include "../header.h"

struct MousePoint {
	int x;
	int y;
	MousePoint(int x, int y) {
		this->x = x;
		this->y = y;
	}
};

class Input {
	CefRefPtr<CefBrowser> browser_;
	bool mouse_tracking_ = false;

	int last_x = 0;
	int last_y = 0;

	int off_x = 0;
	int off_y = 0;

public:
	Input(CefRefPtr<CefBrowser> browser);
	~Input();

	MousePoint get_last_point(int startX, int startY) const;
	void set_offset(int x, int y);

	void wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

