/*
 * Copyright © 2002 Vladimir Voinkov
 *
 * This material is provided "as is", with absolutely no warranty 
 * expressed or implied. Any use is at your own risk. Permission to 
 * use or copy this software for any purpose is hereby granted without 
 * fee, provided the above notices are retained on all copies. 
 * Permission to modify the code and to distribute modified code is 
 * granted, provided the above notices are retained, and a notice that 
 * the code was modified is included with the above copyright notice. 
 *
 * This header is part of comet.
 * http://www.lambdasoft.dk/comet
 */

#ifndef COMETW_WINDOW_IMPL_H
#define COMETW_WINDOW_IMPL_H

#include <comet/server.h>
#include <comet/scope_guard.h>

#include <cometw/config.h>
#include <cometw/root.h>
#include <cometw/message_crackers.h>
#include <cometw/window_class.h>
#include <cometw/window_wrap.h>

namespace cometw {

////////////////////////////////////////////////////////////////////////////////
// Class window_procedure
////////////////////////////////////////////////////////////////////////////////

	class ATL_NO_VTABLE window_procedure : public window_procedure_base
	{
	public:
		enum { proc_type = GWL_WNDPROC };
		
	public:
		window_procedure() : sub_proc_(::DefWindowProc) 
		{}
		
	public:
		void default_window_proc(window_message& m) 
		{
#	ifdef STRICT
			m.lresult = ::CallWindowProc(sub_proc_, m.msg.hwnd, m.msg.message, m.msg.wParam, m.msg.lParam);
#	else 
			m.lresult = ::CallWindowProc(reinterpret_cast<FARPROC>(sub_proc_), m.msg.hwnd, m.msg.message, m.msg.wParam, m.msg.lParam);
#	endif 
		}
	
		template<typename T>	
		void window_proc(T* pT, window_message& m)
		{
			// pass to the message map to process
			if(!pT->process_message(m))
				pT->default_window_proc(m);

			if(m.msg.message == WM_NCDESTROY) {
				HWND h = pT->hwnd_;
				pT->hwnd_ = NULL;
				pT->on_destroyed(h);
			}
		}

	protected:
		WNDPROC sub_proc_;
	};

////////////////////////////////////////////////////////////////////////////////
// Class dialog_procedure
////////////////////////////////////////////////////////////////////////////////

	class ATL_NO_VTABLE dialog_procedure : public window_procedure_base
	{
	public:
		enum { proc_type = DWL_DLGPROC };
		
	//
	public:
		void default_window_proc(window_message&) 
		{}
	
		template<typename T>	
		void window_proc(T* pT, window_message& m)
		{
			if(pT->process_message(m)) {
				// set result if message was handled
				switch(m.msg.message) {
				case WM_CHARTOITEM:
				case WM_COMPAREITEM:
				case WM_CTLCOLORBTN:
				case WM_CTLCOLORDLG:
				case WM_CTLCOLOREDIT:
				case WM_CTLCOLORLISTBOX:
				case WM_CTLCOLORSCROLLBAR:
				case WM_CTLCOLORSTATIC:
				case WM_INITDIALOG:
				case WM_QUERYDRAGICON:
				case WM_VKEYTOITEM:
				case WM_CTLCOLORMSGBOX:
					return;

				default:
					::SetWindowLong(pT->hwnd_, DWL_MSGRESULT, m.lresult);
					m.lresult = TRUE;
				}
			}
			else
				pT->default_window_proc(m);

			if(m.msg.message == WM_NCDESTROY) {
				HWND h = pT->hwnd_;
				pT->hwnd_ = NULL;
				pT->on_destroyed(h);
			}
		}
	};

////////////////////////////////////////////////////////////////////////////////
// Class window_impl_base
////////////////////////////////////////////////////////////////////////////////

	template<typename PROC, typename IMPL = details::window_pointer>
	class window_impl_base : public details::window_impl_core, public PROC, public IMPL, public impl::window_base
	{
	public:
		typedef window_impl_base<PROC, IMPL> self;
		typedef details::window_impl_core base;
		
	public:
		static LRESULT CALLBACK start_window_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
		{
			self* pT = static_cast<self*>(base::restore_pointer());
			pT->hwnd_ = hwnd;

			WNDPROC proc = IMPL::initialize(pT);
			::SetWindowLong(hwnd, PROC::proc_type, reinterpret_cast<long>(proc));
			return proc(hwnd, message, wparam, lparam);
		}

		static LRESULT CALLBACK window_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
		{
			self* pT = static_cast<self*>(hwnd2pointer(hwnd));
			window_message m = { pT->hwnd_, message, wparam, lparam };
			pT->PROC::window_proc(pT, m);
			return m.lresult;
		}
	protected:
		void store_this_pointer() {
			store_pointer(this);
		}
	};

////////////////////////////////////////////////////////////////////////////////
// Class window_impl
////////////////////////////////////////////////////////////////////////////////
		
	template<typename T, typename TRAITS = control_traits, COMET_MSG_TEMPLATE>
	class window_impl : public window_impl_base<window_procedure>, public window_class<T, TRAITS>
	{
	public:
		typedef comet::make_list<COMET_LIST_ARG_0>::result msg_map;
		typedef window_impl_base<window_procedure> base;

		virtual bool process_message(window_message& m)
		{	return process_message_map(static_cast<T*>(this), m); }

 		HWND create(HWND hparent, const auto_rect& rc = NULL, LPCTSTR name = NULL, DWORD style = 0, DWORD exstyle = 0, const auto_menu_id& menu = 0U, LPVOID param = NULL)
		{
			COMET_ASSERT(hwnd_ == NULL);
			UINT id = (menu == 0 && style&WS_CHILD) ? reinterpret_cast<UINT>(this) : static_cast<UINT>(menu);

			ATOM atom = window_class<T, TRAITS>::initialize(sub_proc_);

			store_pointer(static_cast<base*>(this));
			scope_guard guard = make_guard(clear_pointer, static_cast<base*>(this));
			return cometw::create(atom, hparent, rc, name, TRAITS::window_style(style), TRAITS::window_exstyle(exstyle), id, param);
		}
	};

////////////////////////////////////////////////////////////////////////////////
// Class dynamic_window_impl
////////////////////////////////////////////////////////////////////////////////

	template<typename T, typename TRAITS = control_traits, COMET_MSG_TEMPLATE>
	class dynamic_window_impl : public window_impl_base<window_procedure>, public window_class<T, TRAITS>
	{
	public:
		typedef comet::make_list<COMET_LIST_ARG_0>::result msg_map;
		typedef window_impl_base<window_procedure> base;

		virtual bool process_message(window_message& m)
		{	return process_message_map(static_cast<T*>(this), m); }

		static bool register_window_class()
		{	return (window_class<T, TRAITS>::initialize() != NULL);	}

		virtual ~dynamic_window_impl()
		{}

		virtual void on_destroyed(HWND) 
		{	delete this;	}

	private:
		friend window_class<T, TRAITS>;
		static LRESULT CALLBACK start_window_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
		{
			try
			{
				T* pWindow = new T;
				window_class<T, TRAITS>::initialize(pWindow->sub_proc_);
				pWindow->store_this_pointer();
				::SetWindowLong(hwnd, T::proc_type, reinterpret_cast<long>(base::start_window_proc));
			}
			catch(...)
			{
				::SetWindowLong(hwnd, T::proc_type, reinterpret_cast<long>(failed_window_proc));
				return -1;
			}
			return base::start_window_proc(hwnd, message, wparam, lparam);
		}
		static LRESULT CALLBACK failed_window_proc(HWND, UINT, WPARAM, LPARAM)
		{	return -1;	}
	};

	namespace details {

////////////////////////////////////////////////////////////////////////////////
// Dialog initialization support
// apply_dlginit
// Parses DLGINIT resource and applies it to dialog. Adapted from MFC
////////////////////////////////////////////////////////////////////////////////

		inline bool load_dlginit(HWND hwnd, void* presource)
		{
			COMET_ASSERT(presource);

			bool ok = true;
			WORD* ptr = (WORD*)presource;

			while(ok && *ptr != 0)
			{
				// Next control id
				WORD idControl = *ptr++;
				WORD idMessage = *ptr++;
				DWORD length = *((DWORD*&)ptr)++;

				// In Win32 the WM_ messages have changed. They have to be translated 
				//  from the 32-bit values to 16-bit values here.

				const WORD WIN16_LB_ADDSTRING = 0x0401;
				const WORD WIN16_CB_ADDSTRING = 0x0403;
				const WORD AFX_CB_ADDSTRING = 0x1234;

				// unfortunately, WIN16_CB_ADDSTRING == CBEM_INSERTITEM
				if(idMessage == AFX_CB_ADDSTRING)
					idMessage = CBEM_INSERTITEM;
				else if(idMessage == WIN16_LB_ADDSTRING)
					idMessage = LB_ADDSTRING;
				else if (idMessage == WIN16_CB_ADDSTRING)
					idMessage = CB_ADDSTRING;

				// check for invalid/unknown message types
				COMET_ASSERT(idMessage == LB_ADDSTRING || idMessage == CB_ADDSTRING || idMessage == CBEM_INSERTITEM);
				
				// For AddStrings, the count must exactly delimit the string, including 
				//  the NULL termination.  This check will not catch all mal-formed 
				//  ADDSTRINGs, but will catch some.
				COMET_ASSERT(*((LPBYTE)ptr + (UINT)length - 1) == 0);

				if(idMessage == CBEM_INSERTITEM)
				{
					COMBOBOXEXITEM item;
					item.mask = CBEIF_TEXT;
					item.iItem = -1;
					item.pszText = LPTSTR(ptr);

					if(::SendDlgItemMessage(hwnd, idControl, idMessage, 0, LPARAM(&item)) == -1)
						ok = false;
				}
				else if(idMessage == LB_ADDSTRING || idMessage == CB_ADDSTRING)
				{
					// List/Combobox returns -1 for error
					if(::SendDlgItemMessageA(hwnd, idControl, idMessage, 0, LPARAM(ptr)) == -1)
						ok = false;
				}

				// skip past data
				ptr = PWORD(LPBYTE(ptr) + UINT(length));
			}

			return ok;
		}

////////////////////////////////////////////////////////////////////////////////
// load_dialoginit
// Looks for and loads specified dialog init resource
////////////////////////////////////////////////////////////////////////////////

		const LPTSTR RT_DLGINIT = MAKEINTRESOURCE(240);

        inline bool load_dlginit(HWND hwnd, UINT id)
        {
            COMET_ASSERT(id);

            HRSRC hdlginit = ::FindResource(module().instance(), MAKEINTRESOURCE(id), RT_DLGINIT);
            HGLOBAL hresource = ::LoadResource(module().instance(), hdlginit);
            if(!hresource)
                return false;

            bool rslt = false;
            void* presource = ::LockResource(hresource);
            if(presource)
                rslt = load_dlginit(hwnd, presource);

            ::FreeResource(hresource);
            return rslt;
		}
	}	// details

////////////////////////////////////////////////////////////////////////////////
// Class dialog_impl
////////////////////////////////////////////////////////////////////////////////

	template<typename T, UINT IDD, COMET_MSG_TEMPLATE>
	class ATL_NO_VTABLE dialog_impl : public window_impl_base<dialog_procedure>
	{
	public:
		enum { idd = IDD };
		typedef comet::make_list< msg<WM_INITDIALOG> >::result msg_submap;
		typedef comet::make_list<COMET_LIST_ARG_0>::result msg_map;
		typedef window_impl_base<dialog_procedure> base;
		typedef dialog_impl self;

		virtual bool process_message(window_message& m)
		{	
			return process_message_map(static_cast<dialog_impl*>(this), m, type2type<msg_submap>())
				|| process_message_map(static_cast<T*>(this), m);
		}

	public:
		int do_modal(HWND hparent = ::GetActiveWindow(), LPARAM param = 0)
		{
			COMET_ASSERT(hwnd_ == NULL);
			store_pointer(static_cast<base*>(this));
			scope_guard guard = make_guard(clear_pointer, static_cast<base*>(this));
			return ::DialogBoxParam(module().instance(), MAKEINTRESOURCE(idd), hparent, reinterpret_cast<DLGPROC>(T::start_window_proc), param);
		}

		HWND create(HWND hparent, LPARAM param = NULL)
		{
			COMET_ASSERT(hwnd_ == NULL);
			store_pointer(static_cast<base*>(this));
			scope_guard guard = make_guard(clear_pointer, static_cast<base*>(this));
			return ::CreateDialogParam(module().instance(), MAKEINTRESOURCE(idd), hparent, reinterpret_cast<DLGPROC>(T::start_window_proc), param);
		}

	protected:
		friend msg<WM_INITDIALOG>;
		bool on(msg<WM_INITDIALOG> m)
		{
			details::load_dlginit(hwnd_, idd);
			m.set_result(1);
			return false;
		}
	};

////////////////////////////////////////////////////////////////////////////////
// Class dialog
////////////////////////////////////////////////////////////////////////////////

	template<UINT IDD>
	class dialog : public window_impl_base<dialog_procedure>
	{
	public:
		typedef make_list< msg<WM_INITDIALOG>, cmd_id_range<IDOK, IDCLOSE> >::result msg_map;
		typedef window_impl_base<dialog_procedure> base;

		int do_modal(HWND hparent = ::GetActiveWindow())
		{
			COMET_ASSERT(hwnd_ == NULL);
			store_pointer(static_cast<base*>(this));
			scope_guard guard = make_guard(clear_pointer, static_cast<base*>(this));
			return ::DialogBox(module().instance(), MAKEINTRESOURCE(IDD), hparent, reinterpret_cast<DLGPROC>(start_window_proc));
		}

		virtual bool process_message(window_message& m)
		{	return process_message_map(static_cast<dialog*>(this), m); }

	private:
		friend msg<WM_INITDIALOG>;
		bool on(msg<WM_INITDIALOG> m)
		{
			details::load_dlginit(hwnd_, IDD);
			m.set_result(1);
			return true;
		}

		friend cmd_id_range<IDOK, IDCLOSE>;
		bool on(cmd_id_range<IDOK, IDCLOSE> m) 
		{
			::EndDialog(hwnd_, m.command());
			return true;
		}
	};
}

////////////////////////////////////////////////////////////////////////////////
// Change log:
// April 13, 2002: initially released by Vladimir Voinkov.
// August 12, 2002: added dynamic_window_impl (Vladimir Voinkov)
////////////////////////////////////////////////////////////////////////////////

#endif // COMETW_WINDOW_IMPL_H

