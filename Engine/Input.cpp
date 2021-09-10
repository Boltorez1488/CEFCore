#include "Input.h"

Input::Input(CefRefPtr<CefBrowser> browser) {
	browser_ = browser;
}

Input::~Input() {
}

MousePoint Input::get_last_point(int winX, int winY) const {
	return MousePoint(last_x - winX, last_y - winY);
}

void Input::set_offset(int x, int y) {
	off_x = -x;
	off_y = -y;
}

bool is_key_down(WPARAM wparam) {
	return (GetKeyState(wparam) & 0x8000) != 0;
}

int get_mouse_modifiers(WPARAM wparam) {
	int modifiers = 0;
	if (wparam & MK_CONTROL)
		modifiers |= EVENTFLAG_CONTROL_DOWN;
	if (wparam & MK_SHIFT)
		modifiers |= EVENTFLAG_SHIFT_DOWN;
	if (is_key_down(VK_MENU))
		modifiers |= EVENTFLAG_ALT_DOWN;
	if (wparam & MK_LBUTTON)
		modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
	if (wparam & MK_MBUTTON)
		modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
	if (wparam & MK_RBUTTON)
		modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

	// Low bit set from GetKeyState indicates "toggled".
	if (::GetKeyState(VK_NUMLOCK) & 1)
		modifiers |= EVENTFLAG_NUM_LOCK_ON;
	if (::GetKeyState(VK_CAPITAL) & 1)
		modifiers |= EVENTFLAG_CAPS_LOCK_ON;
	return modifiers;
}

int get_keyboard_modifiers(WPARAM wparam, LPARAM lparam) {
	int modifiers = 0;
	if (is_key_down(VK_SHIFT))
		modifiers |= EVENTFLAG_SHIFT_DOWN;
	if (is_key_down(VK_CONTROL))
		modifiers |= EVENTFLAG_CONTROL_DOWN;
	if (is_key_down(VK_MENU))
		modifiers |= EVENTFLAG_ALT_DOWN;

	// Low bit set from GetKeyState indicates "toggled".
	if (::GetKeyState(VK_NUMLOCK) & 1)
		modifiers |= EVENTFLAG_NUM_LOCK_ON;
	if (::GetKeyState(VK_CAPITAL) & 1)
		modifiers |= EVENTFLAG_CAPS_LOCK_ON;

	switch (wparam) {
		case VK_RETURN:
			if ((lparam >> 16) & KF_EXTENDED)
				modifiers |= EVENTFLAG_IS_KEY_PAD;
			break;
		case VK_INSERT:
		case VK_DELETE:
		case VK_HOME:
		case VK_END:
		case VK_PRIOR:
		case VK_NEXT:
		case VK_UP:
		case VK_DOWN:
		case VK_LEFT:
		case VK_RIGHT:
			if (!((lparam >> 16) & KF_EXTENDED))
				modifiers |= EVENTFLAG_IS_KEY_PAD;
			break;
		case VK_NUMLOCK:
		case VK_NUMPAD0:
		case VK_NUMPAD1:
		case VK_NUMPAD2:
		case VK_NUMPAD3:
		case VK_NUMPAD4:
		case VK_NUMPAD5:
		case VK_NUMPAD6:
		case VK_NUMPAD7:
		case VK_NUMPAD8:
		case VK_NUMPAD9:
		case VK_DIVIDE:
		case VK_MULTIPLY:
		case VK_SUBTRACT:
		case VK_ADD:
		case VK_DECIMAL:
		case VK_CLEAR:
			modifiers |= EVENTFLAG_IS_KEY_PAD;
			break;
		case VK_SHIFT:
			if (is_key_down(VK_LSHIFT))
				modifiers |= EVENTFLAG_IS_LEFT;
			else if (is_key_down(VK_RSHIFT))
				modifiers |= EVENTFLAG_IS_RIGHT;
			break;
		case VK_CONTROL:
			if (is_key_down(VK_LCONTROL))
				modifiers |= EVENTFLAG_IS_LEFT;
			else if (is_key_down(VK_RCONTROL))
				modifiers |= EVENTFLAG_IS_RIGHT;
			break;
		case VK_MENU:
			if (is_key_down(VK_LMENU))
				modifiers |= EVENTFLAG_IS_LEFT;
			else if (is_key_down(VK_RMENU))
				modifiers |= EVENTFLAG_IS_RIGHT;
			break;
		case VK_LWIN:
			modifiers |= EVENTFLAG_IS_LEFT;
			break;
		case VK_RWIN:
			modifiers |= EVENTFLAG_IS_RIGHT;
			break;
	}
	return modifiers;
}

void Input::wnd_proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	cef_mouse_button_type_t type;
	switch (uMsg) {
		case WM_SYSCHAR:
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		case WM_CHAR:
		{
			if (!browser_)
				break;

			const auto is_system_key = uMsg == WM_SYSCHAR ||
				uMsg == WM_SYSKEYDOWN ||
				uMsg == WM_SYSKEYUP;
		
			CefKeyEvent ev; 
			ev.is_system_key = is_system_key;
			ev.windows_key_code = wParam;
			ev.native_key_code = lParam;
			ev.modifiers = get_keyboard_modifiers(wParam, lParam);
			
			if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) {
				ev.type = KEYEVENT_RAWKEYDOWN;
			} else if (uMsg == WM_KEYUP || uMsg == WM_SYSKEYUP) {
				ev.type = KEYEVENT_KEYUP;
			} else {
				ev.type = KEYEVENT_CHAR;
			}

			browser_->GetHost()->SendKeyEvent(ev);
			break;
		}

		case WM_LBUTTONDOWN:
			type = MBT_LEFT;
			goto down;
		case WM_RBUTTONDOWN:
			type = MBT_RIGHT;
			goto down;
		case WM_MBUTTONDOWN:
		{
			type = MBT_MIDDLE;

		down:
			const int x = last_x = LOWORD(lParam) + off_x;
			const int y = last_y = HIWORD(lParam) + off_y;

			if (browser_) {
				CefMouseEvent ev;
				ev.x = x;
				ev.y = y;
				ev.modifiers = get_mouse_modifiers(wParam);
				browser_->GetHost()->SendMouseClickEvent(ev, type, false, 1);
			}
			break;
		}

		case WM_LBUTTONUP:
			type = MBT_LEFT;
			goto up;
		case WM_RBUTTONUP:
			type = MBT_RIGHT;
			goto up;
		case WM_MBUTTONUP:
		{
			type = MBT_MIDDLE;

		up:
			const int x = last_x = LOWORD(lParam) + off_x;
			const int y = last_y = HIWORD(lParam) + off_y;

			if (browser_) {
				CefMouseEvent ev;
				ev.x = x;
				ev.y = y;
				ev.modifiers = get_mouse_modifiers(wParam);
				browser_->GetHost()->SendMouseClickEvent(ev, type, true, 1);
			}
			break;
		}

		case WM_MOUSEMOVE:
		{
			const int x = last_x = LOWORD(lParam) + off_x;
			const int y = last_y = HIWORD(lParam) + off_y;

			if (!mouse_tracking_) {
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.hwndTrack = hWnd;
				TrackMouseEvent(&tme);
			}

			if (browser_) {
				CefMouseEvent ev;
				ev.x = x;
				ev.y = y;
				ev.modifiers = get_mouse_modifiers(wParam);
				browser_->GetHost()->SendMouseMoveEvent(ev, false);
			}
			break;
		}
		case WM_MOUSELEAVE:
		{
			if (mouse_tracking_) {
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE & TME_CANCEL;
				tme.hwndTrack = hWnd;
				TrackMouseEvent(&tme);
				mouse_tracking_ = false;
			}

			if (browser_) {
				POINT p;
				::GetCursorPos(&p);
				::ScreenToClient(hWnd, &p);

				CefMouseEvent ev;
				ev.x = p.x;
				ev.y = p.y;
				ev.modifiers = get_mouse_modifiers(wParam);

				browser_->GetHost()->SendMouseMoveEvent(ev, true);
			}
			break;
		}
		case WM_MOUSEWHEEL:
		{
			POINT screen_point = { LOWORD(lParam), HIWORD(lParam) };
			const HWND scrolled_wnd = ::WindowFromPoint(screen_point);
			if (scrolled_wnd != hWnd)
				break;

			ScreenToClient(hWnd, &screen_point);
			const int delta = GET_WHEEL_DELTA_WPARAM(wParam);

			if (browser_) {
				CefMouseEvent ev;
				ev.x = screen_point.x;
				ev.y = screen_point.y;
				ev.modifiers = get_mouse_modifiers(wParam);
				browser_->GetHost()->SendMouseWheelEvent(ev, is_key_down(VK_SHIFT) ? delta : 0, !is_key_down(VK_SHIFT) ? delta : 0);
			}
			break;
		}
		case WM_LBUTTONDBLCLK:
			type = MBT_LEFT;
			goto dbl;
		case WM_RBUTTONDBLCLK:
			type = MBT_RIGHT;
			goto dbl;
		case WM_MBUTTONDBLCLK:
		{
			type = MBT_MIDDLE;

		dbl:
			const int x = last_x = LOWORD(lParam) + off_x;
			const int y = last_y = HIWORD(lParam) + off_y;

			if (browser_) {
				CefMouseEvent ev;
				ev.x = x;
				ev.y = y;
				ev.modifiers = get_mouse_modifiers(wParam);
				browser_->GetHost()->SendMouseClickEvent(ev, type, false, 2);
			}
			break;
		}
	}
}