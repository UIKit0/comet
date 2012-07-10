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

#ifndef COMETW_FRAME_IMPL_H
#define COMETW_FRAME_IMPL_H

#include <commctrl.h>
#include <comet/server.h>
#include <comet/scope_guard.h>
#include <cometw/config.h>
#include <cometw/window_impl.h>
#include <cometw/client_impl.h>

namespace cometw {

	////////////////////////////////////////////////////////////////////////////
	// Class mdiframe_procedure
	////////////////////////////////////////////////////////////////////////////

	class ATL_NO_VTABLE mdiframe_procedure : public window_procedure_base
	{
	public:
		enum { proc_type = GWL_WNDPROC };
		
	public:
		mdiframe_procedure() : hmdiclient_(NULL)
		{}

		HWND mdiclient() const
		{	return hmdiclient_; }

		void mdiclient(HWND h)
		{	hmdiclient_ = h; }

	public:
		void default_window_proc(window_message& m) 
		{	m.lresult = ::DefFrameProc(m.msg.hwnd, hmdiclient_, m.msg.message, m.msg.wParam, m.msg.lParam); }
	
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
		// We don't have wrappers yet
		HWND hmdiclient_;
	};

	////////////////////////////////////////////////////////////////////////////
	// Class mdichild_procedure
	////////////////////////////////////////////////////////////////////////////

	class ATL_NO_VTABLE mdichild_procedure : public window_procedure_base
	{
	public:
		enum { proc_type = GWL_WNDPROC };
		
	public:
		void default_window_proc(window_message& m) 
		{	m.lresult = ::DefMDIChildProc(m.msg.hwnd, m.msg.message, m.msg.wParam, m.msg.lParam); }
	
		template<typename T>	
		void window_proc(T* pT, window_message& m)
		{
			// pass to the message map to process
			if(!pT->process_message(m))
				pT->default_window_proc(m);

			if(m.message_ == WM_NCDESTROY) {
				HWND h = pT->hwnd_;
				pT->hwnd_ = NULL;
				pT->on_destroyed(h);
			}
		}
	};

	////////////////////////////////////////////////////////////////////////////
	// Class default_client
	// Doing nothing default client used with frame implementation
	////////////////////////////////////////////////////////////////////////////

	struct default_client
	{
		typedef comet::make_list<>::result msg_map;
	};

	////////////////////////////////////////////////////////////////////////////
	// Class default_mdiclient
	////////////////////////////////////////////////////////////////////////////

	namespace details 
	{
		enum { first_child_id = 1024 };

		inline HWND create_mdiclient(HWND hparent, HMENU hmenu) 
		{
			CLIENTCREATESTRUCT ccs = { hmenu, first_child_id };
			return cometw::create(_T("MDICLIENT"), hparent, NULL, NULL, 
				WS_VISIBLE|WS_CHILD|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|MDIS_ALLCHILDSTYLES, 
				WS_EX_CLIENTEDGE, 0U, &ccs);
		}
	}

	template<typename T>
	class default_mdiclient
	{
	public:
		typedef comet::make_list< msg_list<WM_CREATE, WM_SIZE, WM_SETFOCUS> >::result msg_map;

		bool on(msg<WM_CREATE> m) 
		{
			HWND hwnd = details::create_mdiclient(m.hwnd(), ::GetSubMenu(::GetMenu(m.hwnd()), 1));
			if(hwnd == NULL) 
			{
				m.set_result(-1);
				return true;
			}

			static_cast<T&>(*this).mdiclient(hwnd);
			return false;
		}

		bool on(msg<WM_SIZE>& m) 
		{
			::MoveWindow(static_cast<const T&>(*this).mdiclient(), 0, 0, m.cx(), m.cy(), true);
			return false;
		}

		bool on(msg<WM_SETFOCUS>) 
		{
			::SetFocus(static_cast<const T&>(*this).mdiclient());
			return false;
		}
	};

	////////////////////////////////////////////////////////////////////////////
	// Class frame_impl
	// Implements SDI framework. Takes as second parameter client implementation.
	//  Default parameter default_client does nothing; it is even not a window.
	//  Has overrideable member function pre_translate_message to support 
	//  accelerators. Being destroyed it calls PostQuitMessage.
	// Client has first chance to handle the message. Assumed that client's handler
	//  always returns false that means message is not handled and requires further
	//  processing
	////////////////////////////////////////////////////////////////////////////

	template
	<
		typename T, 
		typename CLIENT = default_client, 
		typename TRAITS = frame_traits, 
		COMET_MSG_TEMPLATE
	>
	class ATL_NO_VTABLE frame_impl : public window_impl_base<window_procedure>, public window_class<T, TRAITS>, public CLIENT
	{
	public:
		typedef frame_impl self;
		typedef window_impl_base<window_procedure> procedure_type;
		typedef window_class<T, TRAITS> class_type;
		typedef TRAITS traits_type;
		typedef CLIENT client_type;

		typedef comet::typelist::append< comet::make_list< chain<client_type> >::result,
			comet::make_list<COMET_LIST_ARG_0>::result > msg_map;

		virtual bool process_message(window_message& m)
		{	return process_message_map(static_cast<T*>(this), m); }

		virtual void on_destroyed(HWND) 
		{	::PostQuitMessage(0); }

	public:
 		HWND create(HWND hparent, const auto_rect& rc = NULL, LPCTSTR name = NULL, DWORD style = 0, DWORD exstyle = 0, const auto_menu_id& menu = 0U, LPVOID param = NULL)
		{
			COMET_ASSERT(hwnd_ == NULL);
			ATOM atom = class_type::initialize(sub_proc_);

			store_pointer(static_cast<procedure_type*>(this));
			scope_guard guard = make_guard(clear_pointer, static_cast<procedure_type*>(this));
			return cometw::create(atom, hparent, rc, name, traits_type::window_style(style), traits_type::window_exstyle(exstyle), menu, param);
		}

		// Message loop support
		// @@@ It isn't good place in doing so
		int run() 
		{
			window_message msg;
			while(::GetMessage(&msg.msg, NULL, 0, 0)) {
				if(!static_cast<T*>(this)->pre_translate_message(msg)) {
					::TranslateMessage(&msg.msg);
					::DispatchMessage(&msg.msg);
				}
			}
			return msg.msg.wParam;
		}

		bool pre_translate_message(const window_message& m) 
		{	return false; }
	};

	///////////////////////////////////////////////////////////////////////////////
	// Class mdiframe_impl
	// Implements MDI framework.
	///////////////////////////////////////////////////////////////////////////////

	template
	<
		typename T, 
		typename CLIENT = default_mdiclient, 
		typename TRAITS = frame_traits, 
		COMET_MSG_TEMPLATE
	>
	class ATL_NO_VTABLE mdiframe_impl : public window_impl_base<mdiframe_procedure>, public window_class<T, TRAITS>, public CLIENT
	{
	public:
		typedef mdiframe_impl self;
		typedef window_impl_base<mdiframe_procedure> procedure_type;
		typedef window_class<T, TRAITS> class_type;
		typedef TRAITS traits_type;
		typedef CLIENT client_type;

		typedef comet::typelist::append< comet::make_list< chain<client_type> >::result,
			comet::make_list<COMET_LIST_ARG_0>::result > msg_map;

		virtual bool process_message(window_message& m)
		{	return process_message_map(static_cast<T*>(this), m); }

		virtual void on_destroyed(HWND) 
		{	::PostQuitMessage(0); }

	public:
 		HWND create(HWND hparent, const auto_rect& rc = NULL, LPCTSTR name = NULL, DWORD style = 0, DWORD exstyle = 0, const auto_menu_id& menu = 0U, LPVOID param = NULL) {
			COMET_ASSERT(hwnd_ == NULL);
			ATOM atom = class_type::initialize();

			store_pointer(static_cast<procedure_type*>(this));
			scope_guard guard = make_guard(clear_pointer, static_cast<procedure_type*>(this));
			return cometw::create(atom, hparent, rc, name, traits_type::window_style(style), traits_type::window_exstyle(exstyle), menu, param);
		}

		// Message loop support
		// @@@ It isn't good place in doing so
		int run() 
		{
			window_message msg;
			while(::GetMessage(&msg.msg, NULL, 0, 0)) {
				if(!static_cast<T*>(this)->pre_translate_message(msg)) {
					::TranslateMessage(&msg.msg);
					::DispatchMessage(&msg.msg);
				}
			}
			return msg.msg.wParam;
		}

		bool pre_translate_message(const window_message& m) 
		{	return (::TranslateMDISysAccel(hmdiclient_, const_cast<PMSG>(&m.msg)) != FALSE); }
	};

	///////////////////////////////////////////////////////////////////////////////
	// Class mdichild_impl
	// Implements MDI child framework
	///////////////////////////////////////////////////////////////////////////////

	template
	<
		typename T, 
		typename CLIENT = default_client, 
		typename TRAITS = mdichild_traits, 
		COMET_MSG_TEMPLATE
	>
	class ATL_NO_VTABLE mdichild_impl : public window_impl_base<mdichild_procedure>, public window_class<T, TRAITS>, public CLIENT
	{
	public:
		typedef mdichild_impl self;
		typedef window_impl_base<mdichild_procedure> procedure_type;
		typedef window_class<T, TRAITS> class_type;
		typedef TRAITS traits_type;
		typedef CLIENT client_type;

		typedef comet::typelist::append< comet::make_list< chain<client_type> >::result,
			comet::make_list<COMET_LIST_ARG_0>::result > msg_map;

		virtual bool process_message(window_message& m)
		{	return process_message_map(static_cast<T*>(this), m); }

	public:
		mdichild_impl() : mdiclient_(NULL)
		{}

 		HWND create(HWND hparent, const auto_rect& rc = NULL, LPCTSTR name = NULL, DWORD style = 0, DWORD exstyle = 0, const auto_menu_id& menu = 0U, LPVOID param = NULL)
		{
			COMET_ASSERT(hwnd_ == NULL);
			COMET_ASSERT(traits_type::window_exstyle(exstyle) & WS_EX_MDICHILD);

			ATOM atom = class_type::initialize();

			BOOL active_ismaximized;
			::SendMessage(hparent, WM_MDIGETACTIVE, 0U, reinterpret_cast<LPARAM>(&active_ismaximized));
		
			style = traits_type::window_style(style);
			bool isvisible = (style&WS_VISIBLE) != 0;
			bool ismaximized = (style&WS_MAXIMIZE || active_ismaximized) != 0;
			style &= ~(WS_MAXIMIZE|WS_VISIBLE);

			store_pointer(static_cast<procedure_type*>(this));
			scope_guard guard = make_guard(clear_pointer, static_cast<procedure_type*>(this));
			if(!cometw::create(atom, hparent, rc, name, style, traits_type::window_exstyle(exstyle), menu, param))
				return NULL;

			if(ismaximized && isvisible)
				::SendMessage(hparent, WM_MDIMAXIMIZE, reinterpret_cast<WPARAM>(hwnd_), 0);
			else if(isvisible) {
				::SendMessage(hparent, WM_MDIACTIVATE, reinterpret_cast<WPARAM>(hwnd_), 0);
				::ShowWindow(hwnd_, SW_SHOW);
			}

			::SendMessage(hparent, WM_MDIREFRESHMENU, 0U, 0);
			mdiclient_ = hparent;
			return hwnd_;
		}

	protected:
		HWND mdiclient_;
	};

}

////////////////////////////////////////////////////////////////////////////////
// Change log:
// April 13, 2002: initially released by Vladimir Voinkov.
////////////////////////////////////////////////////////////////////////////////

#endif // COMETW_FRAME_IMPL_H
