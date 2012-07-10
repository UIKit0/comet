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

#ifndef COMETW_WINDOW_CLASS_H
#define COMETW_WINDOW_CLASS_H

#include <comet/threading.h>

/////////////////////////////////////////////////////////////////////////////

namespace cometw {

	namespace details {

		//
		// class atom_t
		//
		// Helper for registering window class
		//

		class atom_t
		{
		public:
			atom_t() : atom_(0), subclass_proc_(0)
			{}

			operator ATOM() const 
			{	return atom_; }

			const WNDPROC& subclass_proc() const 
			{	return subclass_proc_; }

			bool registry_window_class(WNDCLASSEX& wc_reg, LPCTSTR subclass_name) {
				COMET_ASSERT(atom_ == 0);

				if(subclass_name) {
					// look up window class subclass_name_ at global and local context
					WNDCLASSEX wc = { sizeof(wc) }; 
					if(!::GetClassInfoEx(NULL, subclass_name, &wc) && !::GetClassInfoEx(comet::module().instance(), subclass_name, &wc))
						// subclass_name isn't registered
						return 0;

					// assign got WNDCLASSEX to wc_reg
					LPCTSTR temp_classname = wc_reg.lpszClassName;
					WNDPROC temp_proc = wc_reg.lpfnWndProc;
					::memcpy(&wc_reg, &wc, sizeof(wc));
					wc_reg.lpszClassName = temp_classname;
					wc_reg.lpfnWndProc = temp_proc;

					// store returned window procedure
					subclass_proc_ = wc.lpfnWndProc;
				}
				else
					subclass_proc_ = ::DefWindowProc;

				atom_ = ::RegisterClassEx(&wc_reg);
				return (atom_ != 0);
			}

		private:
			ATOM atom_;
			WNDPROC subclass_proc_;

		private:
			atom_t(const atom_t&);
			const atom_t& operator=(const atom_t&);
		};

		//
		// class unique_name
		//

		class unique_name
		{
			TCHAR buff_[16];
			unique_name(const unique_name&);
			const unique_name& operator=(const unique_name&);

		public:
			unique_name() 
			{	::wsprintf(buff_, _T("cometw:%8.8X"), reinterpret_cast<DWORD>(buff_)); }

			operator LPCTSTR() const 
			{	return buff_; }
		};
	}

/////////////////////////////////////////////////////////////////////////////

	//
	// class window_traits
	//
	// Traits' class to set up window class registration
	//

	template
	<
		DWORD STYLE = 0, 
		DWORD EXSTYLE = 0, 
		int BRUSH = COLOR_WINDOW, 
		UINT CLASS_STYLE = 0U, 
		UINT MENU = 0U,
		UINT CURSOR = reinterpret_cast<UINT>(IDC_ARROW),
		UINT ICON = reinterpret_cast<UINT>(IDI_APPLICATION)
	>
	struct window_traits
	{
		static DWORD window_style(DWORD param = 0)
		{	return param ? param : STYLE; }

		static DWORD window_exstyle(DWORD param = 0)
		{	return param ? param : EXSTYLE; }

		static HBRUSH class_brush()
		{	return reinterpret_cast<HBRUSH>(BRUSH + 1); }

		static int class_style()
		{	return CLASS_STYLE; }

		static HCURSOR class_cursor() {
			HCURSOR h = ::LoadCursor(NULL, MAKEINTRESOURCE(CURSOR));
			if(h) return h;
			return ::LoadCursor(comet::module().instance(), MAKEINTRESOURCE(CURSOR));
		}

		static HICON class_icon() {
			HICON h = ::LoadIcon(NULL, MAKEINTRESOURCE(ICON));
			if(h) return h;
			return ::LoadIcon(comet::module().instance(), MAKEINTRESOURCE(ICON));
		}

		static LPCTSTR class_menu()
		{	return MAKEINTRESOURCE(MENU); }
	};

	typedef window_traits<WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, 0> control_traits;
	typedef window_traits<WS_CHILD|WS_VISIBLE, 0, -1> transparent_control_traits;
	typedef window_traits<WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|WS_VISIBLE, 0/*WS_EX_APPWINDOW|WS_EX_WINDOWEDGE*/> frame_traits;
	typedef window_traits<WS_OVERLAPPEDWINDOW|WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS, WS_EX_MDICHILD> mdichild_traits;
	typedef window_traits<0, 0> null_style;

/////////////////////////////////////////////////////////////////////////////

	//
	// class window_class
	//
	// This is the policy class to control window class
	//

	template<typename T, typename TRAITS = control_traits>
	class window_class
	{
	private:
		static volatile details::atom_t& atom() {
			static volatile details::atom_t window_class_atom;
			return window_class_atom;
		}

	public:
		static ATOM initialize(WNDPROC& subclass_proc) {
			locking_ptr<details::atom_t> ptr(atom(), details::window_impl_core::cs());

			if(*ptr == 0) 
			{
				WNDCLASSEX wc = 
				{ 
					sizeof(wc),
					TRAITS::class_style(),
					T::start_window_proc,
					0, 0, 
					comet::module().instance(),
					TRAITS::class_icon(),
					TRAITS::class_cursor(),
					TRAITS::class_brush(),
					TRAITS::class_menu(),
					T::registry_window_class_name(),
					NULL
				};

				// registers window class
				if(!ptr->registry_window_class(wc, T::window_class_name()))
					return 0;
			}
			subclass_proc = ptr->subclass_proc();
			return *ptr;
		}
			
		static ATOM initialize() {
			locking_ptr<details::atom_t> ptr(atom(), details::window_impl_core::cs());

			if(*ptr == 0) {
				WNDCLASSEX wc = { 
					sizeof(wc),
					TRAITS::class_style(),
					T::start_window_proc,
					0, 0, 
					comet::module().instance(),
					TRAITS::class_icon(),
					TRAITS::class_cursor(),
					TRAITS::class_brush(),
					TRAITS::class_menu(),
					T::registry_window_class_name(),
					NULL
				};

				// registers window class
				if(!ptr->registry_window_class(wc, T::window_class_name()))
					return 0;
			}
			return *ptr;
		}

	public:
		// Returns the registered window class name. The function can be replaced to specify another 
		// window class name, otherwise the framework generate unique name
		static LPCTSTR registry_window_class_name()
		{
			static details::unique_name name;
			return name; 
		}

		// Returning NULL means that we aren't going to subclass any
		static LPCTSTR window_class_name()
		{	return NULL; }
	};
}

////////////////////////////////////////////////////////////////////////////////
// Change log:
// April 13, 2002: initially released by Vladimir Voinkov and Sofus Mortensen
// August 6, 2002: fixed bug in window subclassing (Vladimir Voinkov)
// August 12, 2002: removed T* parameter from initialize() routine (Vladimir Voinkov)
////////////////////////////////////////////////////////////////////////////////

#endif // COMETW_WINDOW_H
