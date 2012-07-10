//
// controls.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef COMETW_CONTROLS_H
# define COMETW_CONTROLS_H

# pragma comment(lib, "comctl32")

# include <cometw/config.h>
# include <cometw/window_wrap.h>
# include <commctrl.h>

namespace cometw {

	////////////////////////////////////////////////////////////////////////////
	//
	// initialize_common_controls
	//

	inline bool initialize_common_controls(DWORD flags = ICC_BAR_CLASSES)
	{
# if(_WIN32_IE >= 0x0300)
		INITCOMMONCONTROLSEX icc;
		icc.dwSize = sizeof(icc);
		icc.dwICC = flags;
		return (::InitCommonControlsEx(&icc) != FALSE);
# else
		::InitCommonControls();
		return true;
# endif
	}

////////////////////////////////////////////////////////////////////////////////
// Class cometw::edit_interface
// Implements an edit box control interface
////////////////////////////////////////////////////////////////////////////////

	struct edit_interface : public impl::window_base
	{
		enum { default_style = WS_CHILD|WS_VISIBLE|WS_BORDER|ES_LEFT|ES_AUTOHSCROLL, 
			default_exstyle = 0 };

		static LPCTSTR window_class_name()
		{	return _T("EDIT");	}

		bool modify() const
		{
			COMET_ASSERT(::IsWindow(hwnd_));
			return (::SendMessage(hwnd_, EM_GETMODIFY, 0, 0L) != FALSE);
		}
	};

	typedef window_wrapper<edit_interface> edit_control;

////////////////////////////////////////////////////////////////////////////////
// Class cometw::statusbar_interface
// Implements an status bar control interface
////////////////////////////////////////////////////////////////////////////////

	struct statusbar_interface : public impl::window_base
	{
		enum { default_style = WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 
			default_exstyle = 0 };

		static LPCTSTR window_class_name() throw()
		{	return STATUSCLASSNAME;	}

		int parts(int count, int* pparts) const
		{
			COMET_ASSERT(::IsWindow(hwnd_));
			return (int)::SendMessage(hwnd_, SB_GETPARTS, count, (LPARAM)pparts);
		}

		bool parts(int count, int* pwidths)
		{
			COMET_ASSERT(::IsWindow(hwnd_));
			return (::SendMessage(hwnd_, SB_SETPARTS, count, (LPARAM)pwidths) != FALSE);
		}
	};

	typedef window_wrapper<statusbar_interface> statusbar_control;

////////////////////////////////////////////////////////////////////////////////
// Class cometw::toolbar_interface
// Implements an tool bar control interface
////////////////////////////////////////////////////////////////////////////////

	struct toolbar_interface : public impl::window_base
	{
		enum { default_style = WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 
			default_exstyle = 0 };

		static LPCTSTR window_class_name() throw()
		{	return TOOLBARCLASSNAME;	}

		bool button_enabled(int id) const
		{
			COMET_ASSERT(::IsWindow(hwnd_));
			return (::SendMessage(hwnd_, TB_ISBUTTONENABLED, id, 0L) != false);
		}
		bool button_checked(int id) const
		{
			COMET_ASSERT(::IsWindow(hwnd_));
			return (::SendMessage(hwnd_, TB_ISBUTTONCHECKED, id, 0L) != false);
		}
		bool button_pressed(int id) const
		{
			return (::SendMessage(hwnd_, TB_ISBUTTONPRESSED, id, 0L) != false);
		}
		bool button_hidden(int id) const
		{
			return (::SendMessage(hwnd_, TB_ISBUTTONHIDDEN, id, 0L) != false);
		}
		bool button_indeterminate(int id) const
		{
			return (::SendMessage(hwnd_, TB_ISBUTTONINDETERMINATE, id, 0L) != false);
		}
		int state(int id) const
		{
			return (int)::SendMessage(hwnd_, TB_GETSTATE, id, 0L);
		}
		bool state(int id, UINT newState)
		{
			return (::SendMessage(hwnd_, TB_SETSTATE, id, MAKELPARAM(newState, 0)) != false);
		}
		bool button(int index, LPTBBUTTON pButton) const
		{
			return (::SendMessage(hwnd_, TB_GETBUTTON, index, (LPARAM)pButton) != false);
		}
		int button_count() const
		{
			return (int)::SendMessage(hwnd_, TB_BUTTONCOUNT, 0, 0L);
		}
		bool item_rect(int index, RECT& rc) const
		{
			return (::SendMessage(hwnd_, TB_GETITEMRECT, index, (LPARAM)&rc) != false);
		}
		void button_struct_size(int size = sizeof(TBBUTTON))
		{
			::SendMessage(hwnd_, TB_BUTTONSTRUCTSIZE, size, 0L);
		}
		bool button_size(SIZE size)
		{
			return (::SendMessage(hwnd_, TB_SETBUTTONSIZE, 0, MAKELPARAM(size.cx, size.cy)) != false);
		}
		bool button_size(int cx, int cy)
		{
			return (::SendMessage(hwnd_, TB_SETBUTTONSIZE, 0, MAKELPARAM(cx, cy)) != false);
		}
		bool bitmap_size(SIZE size)
		{
			return (::SendMessage(hwnd_, TB_SETBITMAPSIZE, 0, MAKELPARAM(size.cx, size.cy)) != false);
		}
		bool bitmap_size(int cx, int cy)
		{
			return (::SendMessage(hwnd_, TB_SETBITMAPSIZE, 0, MAKELPARAM(cx, cy)) != false);
		}
		HWND tool_tips() const
		{
			return (HWND)::SendMessage(hwnd_, TB_GETTOOLTIPS, 0, 0L);
		}
		void tool_tips(HWND hwnd)
		{
			::SendMessage(hwnd_, TB_SETTOOLTIPS, (WPARAM)hwnd, 0L);
		}
		void SetNotifyWnd(HWND hwnd)
		{
			::SendMessage(hwnd_, TB_SETPARENT, (WPARAM)hwnd, 0L);
		}
		int rows() const
		{
			return (int)::SendMessage(hwnd_, TB_GETROWS, 0, 0L);
		}
		void rows(int newRows, bool larger, RECT& rc)
		{
			::SendMessage(hwnd_, TB_SETROWS, MAKELPARAM(newRows, larger), (LPARAM)&rc);
		}
		bool cmd_id(int index, UINT id)
		{
			return (::SendMessage(hwnd_, TB_SETCMDID, index, id) != false);
		}
		DWORD bitmap_flags() const
		{
			return (DWORD)::SendMessage(hwnd_, TB_GETBITMAPFLAGS, 0, 0L);
		}
		int bitmap(int id) const
		{
			return (int)::SendMessage(hwnd_, TB_GETBITMAP, id, 0L);
		}
		int button_text(int id, LPTSTR text) const
		{
			return (int)::SendMessage(hwnd_, TB_GETBUTTONTEXT, id, (LPARAM)text);
		}
		HIMAGELIST image_list() const
		{
			return (HIMAGELIST)::SendMessage(hwnd_, TB_GETIMAGELIST, 0, 0L);
		}
		HIMAGELIST image_list(HIMAGELIST hlist)
		{
			return (HIMAGELIST)::SendMessage(hwnd_, TB_SETIMAGELIST, 0, (LPARAM)hlist);
		}
		HIMAGELIST disabled_image_list() const
		{
			return (HIMAGELIST)::SendMessage(hwnd_, TB_GETDISABLEDIMAGELIST, 0, 0L);
		}
		HIMAGELIST disabled_image_list(HIMAGELIST hlist)
		{
			return (HIMAGELIST)::SendMessage(hwnd_, TB_SETDISABLEDIMAGELIST, 0, (LPARAM)hlist);
		}
		HIMAGELIST hot_image_list() const
		{
			return (HIMAGELIST)::SendMessage(hwnd_, TB_GETHOTIMAGELIST, 0, 0L);
		}
		HIMAGELIST hot_image_list(HIMAGELIST hlist)
		{
			return (HIMAGELIST)::SendMessage(hwnd_, TB_SETHOTIMAGELIST, 0, (LPARAM)hlist);
		}
		DWORD tb_style() const
		{
			return (DWORD)::SendMessage(hwnd_, TB_GETSTYLE, 0, 0L);
		}
		void tb_Style(DWORD style)
		{
			::SendMessage(hwnd_, TB_SETSTYLE, 0, style);
		}
		DWORD button_size() const
		{
			return (DWORD)::SendMessage(hwnd_, TB_GETBUTTONSIZE, 0, 0L);
		}
		void button_size(SIZE& size) const
		{
			DWORD dw = (DWORD)::SendMessage(hwnd_, TB_GETBUTTONSIZE, 0, 0L);
			size.cx = LOWORD(dw);
			size.cy = HIWORD(dw);
		}
		bool rect(int id, RECT& rc) const
		{
			return (::SendMessage(hwnd_, TB_GETRECT, id, (LPARAM)&rc) != false);
		}
		int text_rows() const
		{
			return (int)::SendMessage(hwnd_, TB_GETTEXTROWS, 0, 0L);
		}
		bool SetButtonWidth(int cxMin, int cxMax)
		{
			return (::SendMessage(hwnd_, TB_SETBUTTONWIDTH, 0, MAKELPARAM(cxMin, cxMax)) != false);
		}
		bool indent(int newIndent)
		{
			return (::SendMessage(hwnd_, TB_SETINDENT, newIndent, 0L) != false);
		}
		bool max_text_rows(int newMax)
		{
			return (::SendMessage(hwnd_, TB_SETMAXTEXTROWS, newMax, 0L) != false);
		}
#if (_WIN32_IE >= 0x0400)
		bool anchor_highlight() const
		{
			return (::SendMessage(hwnd_, TB_GETANCHORHIGHLIGHT, 0, 0L) != false);
		}
		bool anchor_highlight(bool enable = true)
		{
			return (::SendMessage(hwnd_, TB_SETANCHORHIGHLIGHT, enable, 0L) != false);
		}
		int button_info(int id, TBBUTTONINFO& bi) const
		{
			return (int)::SendMessage(hwnd_, TB_GETBUTTONINFO, id, (LPARAM)&bi);
		}
		bool button_info(int id, TBBUTTONINFO& bi)
		{
			return (::SendMessage(hwnd_, TB_SETBUTTONINFO, id, (LPARAM)&bi) != false);
		}
		int hot_item() const
		{
			return (int)::SendMessage(hwnd_, TB_GETHOTITEM, 0, 0L);
		}
		int hot_item(int item)
		{
			return (int)::SendMessage(hwnd_, TB_SETHOTITEM, item, 0L);
		}
		bool button_highlighted(int id) const
		{
			return (::SendMessage(hwnd_, TB_ISBUTTONHIGHLIGHTED, id, 0L) != false);
		}
		DWORD draw_text_flags(DWORD mask, DWORD flags)
		{
			return (DWORD)::SendMessage(hwnd_, TB_SETDRAWTEXTFLAGS, mask, flags);
		}
		bool color_scheme(COLORSCHEME& cs) const
		{
			return (::SendMessage(hwnd_, TB_GETCOLORSCHEME, 0, (LPARAM)&cs) != false);
		}
		void color_scheme(const COLORSCHEME& cs)
		{
			::SendMessage(hwnd_, TB_SETCOLORSCHEME, 0, (LPARAM)&cs);
		}
		DWORD tb_extended_style() const
		{
			return (DWORD)::SendMessage(hwnd_, TB_GETEXTENDEDSTYLE, 0, 0L);
		}
		DWORD tb_extendedStyle(DWORD style)
		{
			return (DWORD)::SendMessage(hwnd_, TB_SETEXTENDEDSTYLE, 0, style);
		}
		void insert_mark(TBINSERTMARK& im) const
		{
			::SendMessage(hwnd_, TB_GETINSERTMARK, 0, (LPARAM)&im);
		}
		void insert_mark(const TBINSERTMARK& im)
		{
			::SendMessage(hwnd_, TB_SETINSERTMARK, 0, (LPARAM)&im);
		}
		COLORREF insert_mark_color() const
		{
			return (COLORREF)::SendMessage(hwnd_, TB_GETINSERTMARKCOLOR, 0, 0L);
		}
		COLORREF insert_mark_color(COLORREF clr)
		{
			return (COLORREF)::SendMessage(hwnd_, TB_SETINSERTMARKCOLOR, 0, (LPARAM)clr);
		}
		bool max_size(SIZE& size) const
		{
			return (::SendMessage(hwnd_, TB_GETMAXSIZE, 0, (LPARAM)&size) != false);
		}
		void padding(SIZE& padding) const
		{
			DWORD dw = (DWORD)::SendMessage(hwnd_, TB_GETPADDING, 0, 0L);
			padding.cx = LOWORD(dw);
			padding.cy = HIWORD(dw);
		}
		void padding(int cx, int cy, LPSIZE prev)
		{
			DWORD dw = (DWORD)::SendMessage(hwnd_, TB_SETPADDING, 0, MAKELPARAM(cx, cy));
			if(prev)
			{
				prev->cx = LOWORD(dw);
				prev->cy = HIWORD(dw);
			}
		}
		bool unicode_format() const
		{
			return (::SendMessage(hwnd_, TB_GETUNICODEFORMAT, 0, 0L) != false);
		}
		bool unicode_format(bool unicode)
		{
			return (::SendMessage(hwnd_, TB_SETUNICODEFORMAT, unicode, 0L) != false);
		}
#endif // (_WIN32_IE >= 0x0400)

#if (_WIN32_IE >= 0x0500)
		int string(int index, LPTSTR str, int len) const
		{
			return (int)::SendMessage(hwnd_, TB_GETSTRING, MAKEWPARAM(len, index), (LPARAM)str);
		}
#endif //(_WIN32_IE >= 0x0500)

#if (_WIN32_WINNT >= 0x0501)
		void metrics(TBMETRICS& m) const
		{
			::SendMessage(hwnd_, TB_GETMETRICS, 0, (LPARAM)&m);
		}
		void metrics(const TBMETRICS& m)
		{
			::SendMessage(hwnd_, TB_SETMETRICS, 0, (LPARAM)&m);
		}
		void window_theme(LPCWSTR theme)
		{
			::SendMessage(hwnd_, TB_SETWINDOWTHEME, 0, (LPARAM)theme);
		}
#endif //(_WIN32_WINNT >= 0x0501)

		bool enable_button(int id, bool enable = true)
		{
			return (::SendMessage(hwnd_, TB_ENABLEBUTTON, id, MAKELPARAM(enable, 0)) != false);
		}
		bool check_button(int id, bool check = true)
		{
			return (::SendMessage(hwnd_, TB_CHECKBUTTON, id, MAKELPARAM(check, 0)) != false);
		}
		bool press_button(int id, bool press = true)
		{
			return (::SendMessage(hwnd_, TB_PRESSBUTTON, id, MAKELPARAM(press, 0)) != false);
		}
		bool hide_button(int id, bool hide = true)
		{
			return (::SendMessage(hwnd_, TB_HIDEBUTTON, id, MAKELPARAM(hide, 0)) != false);
		}
		bool indeterminate(int id, bool flag = true)
		{
			return (::SendMessage(hwnd_, TB_INDETERMINATE, id, MAKELPARAM(flag, 0)) != false);
		}
		int add_bitmap(int numButtons, UINT bitmapID)
		{
			TBADDBITMAP tbab;
			tbab.hInst = module().instance();
			tbab.nID = bitmapID;
			return (int)::SendMessage(hwnd_, TB_ADDBITMAP, (WPARAM)numButtons, (LPARAM)&tbab);
		}
		int add_bitmap(int numButtons, HBITMAP bitmap)
		{
			TBADDBITMAP tbab;
			tbab.hInst = NULL;
			tbab.nID = (UINT_PTR)bitmap;
			return (int)::SendMessage(hwnd_, TB_ADDBITMAP, (WPARAM)numButtons, (LPARAM)&tbab);
		}
		bool add_buttons(int numButtons, const LPTBBUTTON buttons)
		{
			return (::SendMessage(hwnd_, TB_ADDBUTTONS, numButtons, (LPARAM)buttons) != false);
		}
		bool insert_button(int index, const TBBUTTON& button)
		{
			return (::SendMessage(hwnd_, TB_INSERTBUTTON, index, (LPARAM)&button) != false);
		}
		bool delete_button(int index)
		{
			return (::SendMessage(hwnd_, TB_DELETEBUTTON, index, 0L) != false);
		}
		UINT command_to_index(UINT id) const
		{
			return (UINT)::SendMessage(hwnd_, TB_COMMANDTOINDEX, id, 0L);
		}
		void save_state(HKEY root, LPCTSTR subKey, LPCTSTR valueName)
		{
			TBSAVEPARAMS tbs;
			tbs.hkr = root;
			tbs.pszSubKey = subKey;
			tbs.pszValueName = valueName;
			::SendMessage(hwnd_, TB_SAVERESTORE, (WPARAM)true, (LPARAM)&tbs);
		}
		void restore_state(HKEY root, LPCTSTR subKey, LPCTSTR valueName)
		{
			TBSAVEPARAMS tbs;
			tbs.hkr = root;
			tbs.pszSubKey = subKey;
			tbs.pszValueName = valueName;
			::SendMessage(hwnd_, TB_SAVERESTORE, (WPARAM)false, (LPARAM)&tbs);
		}
		void customize()
		{
			::SendMessage(hwnd_, TB_CUSTOMIZE, 0, 0L);
		}
		int add_string(UINT stringID)
		{
			return (int)::SendMessage(hwnd_, TB_ADDSTRING, (WPARAM)module().instance(), (LPARAM)stringID);
		}
		int add_strings(LPCTSTR strings)
		{
			return (int)::SendMessage(hwnd_, TB_ADDSTRING, 0, (LPARAM)strings);
		}
		void auto_size()
		{
			::SendMessage(hwnd_, TB_AUTOSIZE, 0, 0L);
		}

		bool change_bitmap(int id, int bitmap)
		{
			return (::SendMessage(hwnd_, TB_CHANGEBITMAP, id, MAKELPARAM(bitmap, 0)) != false);
		}
		int load_images(int bitmapID)
		{
			return (int)::SendMessage(hwnd_, TB_LOADIMAGES, bitmapID, (LPARAM)module().instance());
		}
		int load_std_images(int bitmapID)
		{
			return (int)::SendMessage(hwnd_, TB_LOADIMAGES, bitmapID, (LPARAM)HINST_COMMCTRL);
		}
		bool replace_bitmap(TBREPLACEBITMAP& rb)
		{
			return (::SendMessage(hwnd_, TB_REPLACEBITMAP, 0, (LPARAM)&rb) != false);
		}

#if (_WIN32_IE >= 0x0400)
		int hit_test(const POINT& pt) const
		{
			return (int)::SendMessage(hwnd_, TB_HITTEST, 0, (LPARAM)&pt);
		}
		bool insert_mark_hit_test(const POINT& pt, TBINSERTMARK& im)
		{
			return (::SendMessage(hwnd_, TB_INSERTMARKHITTEST, (WPARAM)&pt, (LPARAM)&im) != false);
		}
		bool insert_mark_hit_test(int x, int y, TBINSERTMARK& im)
		{
			POINT pt = { x, y };
			return (::SendMessage(hwnd_, TB_INSERTMARKHITTEST, (WPARAM)&pt, (LPARAM)&im) != false);
		}
		bool map_accelerator(TCHAR accel, int& id) const
		{
			return (::SendMessage(hwnd_, TB_MAPACCELERATOR, (WPARAM)accel, (LPARAM)&id) != false);
		}
		bool mark_button(int id, bool highlight = true)
		{
			return (::SendMessage(hwnd_, TB_MARKBUTTON, id, MAKELPARAM(highlight, 0)) != false);
		}
		bool move_button(int oldPos, int newPos)
		{
			return (::SendMessage(hwnd_, TB_MOVEBUTTON, oldPos, newPos) != false);
		}
		HRESULT object(REFIID iid, LPVOID* ppObject)
		{
			return (HRESULT)::SendMessage(hwnd_, TB_GETOBJECT, (WPARAM)&iid, (LPARAM)ppObject);
		}
#endif //(_WIN32_IE >= 0x0400)
	};

} // cometw

////////////////////////////////////////////////////////////////////////////////
// Change log:
// April 13, 2002: initially released by Vladimir Voinkov.
////////////////////////////////////////////////////////////////////////////////

#endif // COMETW_CONTROLS_H
