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

#ifndef COMETW_ROOT_H
#define COMETW_ROOT_H

#include <comet/threading.h>
#include <comet/typelist.h>

#include <cometw/config.h>
#include <cometw/message_crackers.h>

#ifdef COMETW_THUNK_IMPL
#	include <cometw/root_thunk.h>
#else // COMETW_THUNK_IMPL

namespace cometw {

	namespace details {
	
		////////////////////////////////////////////////////////////////////////
		// class window_root_userdata_impl
		// 
		
		struct window_pointer_userdata
		{
			template<typename T> 
			static WNDPROC initialize(T* pT)
			{
				::SetWindowLong(pT->hwnd_, GWL_USERDATA, reinterpret_cast<long>(pT));
				return T::window_proc;
			}
			
			static void* hwnd2pointer(HWND hwnd)
			{	return reinterpret_cast<void*>(::GetWindowLong(hwnd, GWL_USERDATA)); }
			
			template<typename T> 
			static WNDPROC procedure(T* pT)
			{	return T::window_proc; }
		};
		
		typedef window_pointer_userdata window_pointer;
		
	} // details
	
} // cometw

#endif // COMETW_THUNK_IMPL


////////////////////////////////////////////////////////////////////////////////


namespace cometw {

	namespace details {

		////////////////////////////////////////////////////////////////////////
		//
		// window_impl_core
		//
		// Implements passing "this" pointer to start_window_proc
		//

		class window_impl_core
		{
		public:
			static comet::critical_section& cs()
			{
				static comet::critical_section csection;
				return csection;
			}
			
			static void store_pointer(void* pv)
			{
				COMET_ASSERT(pv);
				window_impl_core::cs().enter();
#ifdef _DEBUG
				// Make sure that store_pointer() isn't called twice
				COMET_ASSERT(debug_thread_id() == NULL);
				debug_thread_id() = GetCurrentThreadId();
#endif // _DEBUG
				window_impl_core::pointer() = pv;
			}
			
			static void* restore_pointer()
			{
				void* pv = window_impl_core::pointer();
				COMET_ASSERT(pv);
				window_impl_core::pointer() = NULL;
#ifdef _DEBUG
				// Make sure that we're taking our pointer
				COMET_ASSERT(debug_thread_id() == GetCurrentThreadId());
				debug_thread_id() = NULL;
#endif // _DEBUG
				window_impl_core::cs().leave();
				return pv;
			}
			
			static void clear_pointer(void* pv)
			{
				COMET_ASSERT(pv);
				if(window_impl_core::pointer() == pv) {
#ifdef _DEBUG
					// Make sure that we're clearing our pointer
					COMET_ASSERT(debug_thread_id() == GetCurrentThreadId());
					debug_thread_id() = NULL;
#endif // _DEBUG
					window_impl_core::pointer() = NULL;
					window_impl_core::cs().leave();
				}
			}

		private:
			static void*& pointer()
			{
				static volatile void* data = NULL;
				return const_cast<void*&>(data);
			}
#ifdef _DEBUG
			static DWORD& debug_thread_id() {
				static DWORD thread_id = NULL;
				return thread_id;
			}
#endif // _DEBUG
		};

		////////////////////////////////////////////////////////////////////////
		//
		// class map_holder
		//
		// Processes the message through given message map
		//
		
#ifdef COMET_PARTIAL_SPECIALISATION

		template<typename T, typename LIST> struct map_holder 
		{
			static bool forward(T* pT, window_message& m) {
                typedef typename LIST::head msg_t;
				if(msg_t::dispatch(pT, m))
					return true;
				return map_holder<T, typename LIST::tail>::forward(pT, m);
			}
		};
		template<typename T> struct map_holder<T, comet::nil> 
		{
			static bool forward(T*, window_message&)
			{	return false; }
		};
		
#else // COMET_PARTIAL_SPECIALISATION

		template<class T> struct map_holder 
		{
			template<class LIST> struct map 
			{
				static bool forward(T* pT, window_message& m) {
					if(LIST::head::dispatch(pT, m))
						return true;
					return map<LIST::tail>::forward(pT, m);
				}
			};
			template<> struct map<comet::nil> 
			{
				static bool forward(T*, window_message&)
				{	return false; }
			};
		};
		
#endif // COMET_PARTIAL_SPECIALISATION
			
	} // details

	////////////////////////////////////////////////////////////////////////////
	//
	// Compiler independent utility for message processing
	//

	template<typename T>
	inline bool process_message_map(T* pT, window_message& m) 
	{
#ifdef COMET_PARTIAL_SPECIALISATION

		return details::map_holder<T, typename T::msg_map>::forward(pT, m);
		
#else // COMET_PARTIAL_SPECIALISATION

		return details::map_holder<T>::map<COMET_STRICT_TYPENAME T::msg_map>::forward(pT, m);
		
#endif // COMET_PARTIAL_SPECIALISATION
	}

	template<typename T> struct type2type
	{	typedef T param_type;	};

	template<typename T, typename Map>
	inline bool process_message_map(T* pT, window_message& m, Map) 
	{
#ifdef COMET_PARTIAL_SPECIALISATION

		return details::map_holder<T, typename Map::param_type>::forward(pT, m);
		
#else // COMET_PARTIAL_SPECIALISATION

		return details::map_holder<T>::map<COMET_STRICT_TYPENAME Map::param_type>::forward(pT, m);
		
#endif // COMET_PARTIAL_SPECIALISATION
	}

	////////////////////////////////////////////////////////////////////////////
	//
	// class window_procedure_base
	//
	// Virtual interface for message processing
	//

	class ATL_NO_VTABLE window_procedure_base
	{
	public:
		virtual bool process_message(window_message& m) = 0;
		virtual void default_window_proc(window_message& m) = 0; 
		virtual void on_destroyed(HWND) 
		{}
	};

} // cometw

////////////////////////////////////////////////////////////////////////////////
// Change log:
// April 13, 2002: initially released by Vladimir Voinkov.
////////////////////////////////////////////////////////////////////////////////
#endif // COMETW_ROOT_H
