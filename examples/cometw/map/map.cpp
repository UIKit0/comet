
#define COMETW_THUNK_IMPL

#ifdef _UNICODE
#pragma message("Boo")
#endif

#include <tchar.h>

#define _puttc putwc

#include <comet/registry.h>

#include <windows.h>
#include <comet/tstring.h>
#include <commctrl.h>

#include "resource.h"

#pragma warning(disable : 4786)

#include <comet/server.h>
#include <cometw/window_impl.h>
#include <cometw/frame_impl.h>
#include <cometw/controls.h>


using namespace comet;
using namespace cometw;

////////////////////////////////////////////////////////////////////////////////
// Class traits EditWindowTraits
////////////////////////////////////////////////////////////////////////////////

typedef window_traits
<
	WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_TABSTOP|WS_HSCROLL|WS_VSCROLL|ES_MULTILINE, 
	WS_EX_CLIENTEDGE
> EditWindowTraits;

////////////////////////////////////////////////////////////////////////////////
// Class TestControl
////////////////////////////////////////////////////////////////////////////////

struct TestControl : 
	public dynamic_window_impl<TestControl, EditWindowTraits>
{
	static LPCTSTR window_class_name()
	{	return _T("EDIT"); }

	static LPCTSTR registry_window_class_name()
	{	return _T("TestControl"); }
};

////////////////////////////////////////////////////////////////////////////////
// Class TestWindow
////////////////////////////////////////////////////////////////////////////////

struct TestWindow : 
	public window_impl<TestWindow, EditWindowTraits>
{
	static LPCTSTR window_class_name()
	{	return _T("EDIT"); }

	bool pre_translate_message(const window_message& m) 
	{	return false;	}
};

////////////////////////////////////////////////////////////////////////////////
// Class DialogWindow
////////////////////////////////////////////////////////////////////////////////

struct DialogWindow : 
	public dialog_impl<DialogWindow, IDD_DIALOG>
{
	bool pre_translate_message(const window_message& m) 
	{	return ::IsDialogMessage(hwnd_, const_cast<LPMSG>(&m.msg)) != FALSE;	}
};

////////////////////////////////////////////////////////////////////////////////
// Class traits FrameWindowTraits
////////////////////////////////////////////////////////////////////////////////

typedef window_traits
<
	WS_VISIBLE|WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 
	WS_EX_APPWINDOW|WS_EX_WINDOWEDGE, 
	-1, 
	0U, 
	IDR_MENU1
> FrameWindowTraits;

////////////////////////////////////////////////////////////////////////////////
// Class FrameWindow
////////////////////////////////////////////////////////////////////////////////

class FrameWindow : 
	public frame_impl
	<
		FrameWindow, 
        client_impl
		<
			FrameWindow,
			statusbar_pane<>, 
			docked_pane<DialogWindow, cometw::align::top, 230>,
			docked_pane<TestWindow, cometw::align::all>
		>, 
        FrameWindowTraits, 
        cmd_id_list<IDM_APP_EXIT, IDM_HELP_ABOUT, ID_VIEW_NEXT, ID_VIEW_PREV> 
	>
{
public:
	typedef docked_pane<DialogWindow, cometw::align::top, 230> DialogWindowType;

public:
	bool on(cmd_id<IDM_APP_EXIT>) {
		::DestroyWindow(hwnd_);
	   	return true;
	}
	 
	bool on(cmd_id<IDM_HELP_ABOUT>) {
		dialog<IDD_ABOUT>().do_modal();
	   	return true;
	}

	bool on(cmd_id<ID_VIEW_NEXT>) {
		next_pane();
	   	return true;
	}

	bool on(cmd_id<ID_VIEW_PREV>) {
		next_pane(false);
	   	return true;
	}

	bool pre_translate_message(const window_message& m) 
	{	return DialogWindowType::get().pre_translate_message(m); }
};

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE, LPSTR, int)
{
	edit_control wnd1;
	RECT extent;
	cometw::impl::resize(type<cometw::align::top>(), wnd1, extent);

	module().instance(hThisInstance);
	initialize_common_controls();
	TestControl::register_window_class();

	FrameWindow wnd;
	HWND h = wnd.create(0, NULL, _T("Comet test"));
	wnd.run();
    return 0;
}
