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

#ifndef COMETW_MESSAGE_MAP_H
#define COMETW_MESSAGE_MAP_H

#include <comet/typelist.h>

#pragma warning(disable : 4244)
#pragma warning(disable : 4355)
#pragma warning(disable : 4503)

#define COMET_MSG_TEMPLATE COMET_TL_LIST( \
	typename X01=nil, typename X02=nil, typename X03=nil, typename X04=nil, typename X05=nil, \
	typename X06=nil, typename X07=nil, typename X08=nil, typename X09=nil, typename X10=nil, \
	typename X11=nil, typename X12=nil, typename X13=nil, typename X14=nil, typename X15=nil, \
	typename X16=nil, typename X17=nil, typename X18=nil, typename X19=nil, typename X20=nil, \
	typename X21=nil, typename X22=nil, typename X23=nil, typename X24=nil, typename X25=nil, \
	typename X26=nil, typename X27=nil, typename X28=nil, typename X29=nil, typename X30=nil, \
	typename X31=nil, typename X32=nil, typename X33=nil, typename X34=nil, typename X35=nil, \
	typename X36=nil, typename X37=nil, typename X38=nil, typename X39=nil)

#define COMET_UINT_TEMPLATE COMET_TL_LIST( \
	UINT X01 = 0, UINT X02 = 0, UINT X03 = 0, UINT X04 = 0, UINT X05 = 0, \
	UINT X06 = 0, UINT X07 = 0, UINT X08 = 0, UINT X09 = 0, UINT X10 = 0, \
	UINT X11 = 0, UINT X12 = 0, UINT X13 = 0, UINT X14 = 0, UINT X15 = 0, \
	UINT X16 = 0, UINT X17 = 0, UINT X18 = 0, UINT X19 = 0, UINT X20 = 0, \
	UINT X21 = 0, UINT X22 = 0, UINT X23 = 0, UINT X24 = 0, UINT X25 = 0, \
	UINT X26 = 0, UINT X27 = 0, UINT X28 = 0, UINT X29 = 0, UINT X30 = 0, \
	UINT X31 = 0, UINT X32 = 0, UINT X33 = 0, UINT X34 = 0, UINT X35 = 0, \
	UINT X36 = 0, UINT X37 = 0, UINT X38 = 0, UINT X39 = 0)

#define COMET_WRAP_ARG COMET_TL_LIST( \
	details::wrap<X01>::type, details::wrap<X02>::type, details::wrap<X03>::type, details::wrap<X04>::type, details::wrap<X05>::type, \
	details::wrap<X06>::type, details::wrap<X07>::type, details::wrap<X08>::type, details::wrap<X09>::type, details::wrap<X10>::type, \
	details::wrap<X11>::type, details::wrap<X12>::type, details::wrap<X13>::type, details::wrap<X14>::type, details::wrap<X15>::type, \
	details::wrap<X16>::type, details::wrap<X17>::type, details::wrap<X18>::type, details::wrap<X19>::type, details::wrap<X20>::type, \
	details::wrap<X21>::type, details::wrap<X22>::type, details::wrap<X23>::type, details::wrap<X24>::type, details::wrap<X25>::type, \
	details::wrap<X26>::type, details::wrap<X27>::type, details::wrap<X28>::type, details::wrap<X29>::type, details::wrap<X30>::type, \
	details::wrap<X31>::type, details::wrap<X32>::type, details::wrap<X33>::type, details::wrap<X34>::type, details::wrap<X35>::type, \
	details::wrap<X36>::type, details::wrap<X37>::type, details::wrap<X38>::type, details::wrap<X39>::type)

using namespace comet;

namespace cometw {

	////////////////////////////////////////////////////////////////////////////
	//
	// window_message
	//

	struct window_message {
		MSG msg;
		LRESULT lresult;
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class msg_all
	//

	class msg_all
	{
	public:
		msg_all(window_message& m) : wm_(m) {}

	private:
		window_message& wm_;

	public:
		const MSG& message() const 
		{	return wm_.msg; }

		UINT message_id() const 
		{	return wm_.msg.message; }

		HWND hwnd() const 
		{	return wm_.msg.hwnd; }

		WPARAM wparam() const 
		{	return wm_.msg.wParam; }

		LPARAM lparam() const 
		{	return wm_.msg.lParam; }

		void set_result(LRESULT lresult) 
		{	wm_.lresult = lresult; }
	};

	namespace details {

		////////////////////////////////////////////////////////////////////////
		//
		// class wrap
		//
		// Helper used in list/enum crackers. 
		//

		template<UINT N> struct wrap {
			typedef wrap<N> type;
			enum { id = N };
		};
		template<> struct wrap<0> {	
			typedef nil type; 
		};

		////////////////////////////////////////////////////////////////////////
		//
		// class msg_cracker
		//

		template<UINT ID>
		class msg_cracker : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		};

		////////////////////////////////////////////////////////////////////////
		//
		// class msg_cracker<WM_CREATE>
		//

		template<> class msg_cracker<WM_CREATE> : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		public:
			const CREATESTRUCT& create_struct() const { 
				COMET_ASSERT(lparam());
				return *reinterpret_cast<LPCREATESTRUCT>(lparam());
			}
		};

		////////////////////////////////////////////////////////////////////////
		//
		// class msg_cracker<WM_MOVE>
		//

		template<> class msg_cracker<WM_MOVE> : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		public:
			int x() const { return LOWORD(lparam()); }
			int y() const { return HIWORD(lparam()); }
		};

		////////////////////////////////////////////////////////////////////////
		//
		// class msg_cracker<WM_SIZE>
		//

		template<> class msg_cracker<WM_SIZE> : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		public:
			UINT flag() const { return wparam(); }
			int cx() const { return LOWORD(lparam()); }
			int cy() const { return HIWORD(lparam()); }
		};

		////////////////////////////////////////////////////////////////////////
		//
		// class msg_cracker<WM_ACTIVATE>
		//

		template<> class msg_cracker<WM_ACTIVATE> : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		public:
			WORD state() const { return LOWORD(wparam()); }
			bool minimized() const { return HIWORD(wparam()) != 0; }
			HWND other() const { return reinterpret_cast<HWND>(lparam()); }
		};

		////////////////////////////////////////////////////////////////////////
		//
		// class msg_cracker<WM_SETFOCUS>
		//

		template<> class msg_cracker<WM_SETFOCUS> : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		public:
			HWND lost_focus() const { return reinterpret_cast<HWND>(wparam()); }
		};

		////////////////////////////////////////////////////////////////////////
		//
		// class msg_cracker<WM_NOTIFY>
		//

		template<> class msg_cracker<WM_NOTIFY> : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		public:
			int id() const { return wparam(); }
			const NMHDR& nmhdr() const { 
				COMET_ASSERT(lparam());
				return *reinterpret_cast<LPNMHDR>(lparam()); 
			}
		};

		////////////////////////////////////////////////////////////////////////
		//
		// class msg_cracker<WM_INITDIALOG>
		//

		template<> class msg_cracker<WM_INITDIALOG> : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		public:
			HWND focus_control() const { return reinterpret_cast<HWND>(wparam()); }
		};
		
		////////////////////////////////////////////////////////////////////////
		// class msg_cracker<WM_COMMAND>
		////////////////////////////////////////////////////////////////////////

		template<> class msg_cracker<WM_COMMAND> : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		public:
			WORD command() const { return LOWORD(wparam()); }
			WORD code() const { return HIWORD(wparam()); }
			HWND control() const { return reinterpret_cast<HWND>(lparam()); }
		};

		////////////////////////////////////////////////////////////////////////
		// class msg_cracker<WM_ACTIVATEAPP>
		////////////////////////////////////////////////////////////////////////

		template<> class msg_cracker<WM_ACTIVATEAPP> : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		public:
			bool activation() const { return wparam() != FALSE; }
			DWORD thread_id() const { return DWORD(lparam());	}
		};

		////////////////////////////////////////////////////////////////////////
		// class msg_cracker<WM_LBUTTONDOWN>
		////////////////////////////////////////////////////////////////////////

		template<> class msg_cracker<WM_LBUTTONDOWN> : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		public:
			WORD flags() const { return static_cast<WORD>(wparam()); }
			short x() const { return LOWORD(lparam()); }
			short y() const { return HIWORD(lparam()); }
		};

		////////////////////////////////////////////////////////////////////////
		// class msg_cracker<WM_MOUSEMOVE>
		////////////////////////////////////////////////////////////////////////

		template<> class msg_cracker<WM_MOUSEMOVE> : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		public:
			WORD flags() const { return static_cast<WORD>(wparam()); }
			short x() const { return LOWORD(lparam()); }
			short y() const { return HIWORD(lparam()); }
		};

		////////////////////////////////////////////////////////////////////////
		// class msg_cracker<WM_LBUTTONUP>
		////////////////////////////////////////////////////////////////////////

		template<> class msg_cracker<WM_LBUTTONUP> : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		public:
			WORD flags() const { return static_cast<WORD>(wparam()); }
			short x() const { return LOWORD(lparam()); }
			short y() const { return HIWORD(lparam()); }
		};

		////////////////////////////////////////////////////////////////////////
		// class msg_cracker<WM_GETMINMAXINFO>
		////////////////////////////////////////////////////////////////////////

		template<> class msg_cracker<WM_GETMINMAXINFO> : public msg_all
		{
		protected:
			msg_cracker(window_message& m) : msg_all(m) {}
		public:
			MINMAXINFO& mmi() const {	return *reinterpret_cast<MINMAXINFO*>(lparam());	}
		};

	} // details

	////////////////////////////////////////////////////////////////////////////
	//
	// class msg
	//
	// Base message cracker. Its handler should look as:
	//
	// bool on(const msg<ID>& m);
	// bool on(msg<ID> m);
	//

	template<UINT ID, int ALTN = 0>
 	class msg : public details::msg_cracker<ID>
 	{
	public:
		enum { msg_id = ID };
 		msg(window_message& m) : details::msg_cracker<ID>(m)
		{}

	public:
 		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && pT->on(msg(m)); }
 	};
	
	////////////////////////////////////////////////////////////////////////////
	//
	// class msg_range
	//
	// Range message cracker. Its handler should look as:
	//
	// bool on(const msg_range<FROM, TO>& m);
	// bool on(msg_range<FROM, TO> m);
	//

	template<UINT FROM, UINT TO, int ALTN = 0> 
	class msg_range : public details::msg_cracker<0>
	{
 		msg_range(window_message& m) : details::msg_cracker<0>(m)
		{}

	public:
		COMET_STATIC_ASSERT(FROM < TO);
		enum { from = FROM, to = TO };

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message <= from) && (m.msg.message >= to) && pT->on(msg_range(m)); }
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class msg_enum
	//
	// Enumerated messages' cracker. Its handler should look as:
	//
	// bool on(const msg_enum<ID1, ID2, ...>& m);
	// bool on(msg_enum<ID1, ID2, ...> m);
	//
		
	template<COMET_UINT_TEMPLATE, int ALTN = 0>
	class msg_enum : public details::msg_cracker<0>
	{
	public:
		typedef make_list<COMET_WRAP_ARG>::result list_t;
		typedef msg_enum<COMET_LIST_ARG_0, ALTN> self;

 		msg_enum(window_message& m) : details::msg_cracker<0>(m)
		{}

#ifdef COMET_PARTIAL_SPECIALISATION

	private:
		// helper class type_iterator
		template<typename T, typename LIST> struct type_iterator {
			static bool forward(T* pT, window_message& m) {
				typedef typename LIST::head msg_t;
				if(m.msg.message == msg_t::id && pT->on(self(m)))
					return true;
				return type_iterator<T, typename LIST::tail>::forward(pT, m);
			}
		};
		template<typename T> struct type_iterator<T, nil> {
			static bool forward(const T*, window_message&) 
			{	return false; }
		};
		
	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return type_iterator<T, list_t>::forward(pT, m); }

#else // COMET_PARTIAL_SPECIALISATION

	private:
		// helper class no_part_spes
		template<class T> struct no_part_spes {
			template<class LIST> struct type_iterator {
				static bool forward(T* pT, window_message& m) {
					if(m.msg.message == LIST::head::id && pT->on(self(m)))
						return true;
					return type_iterator<LIST::tail>::forward(pT, m);
				}
			};
			template<> struct type_iterator<nil> {
				static bool forward(const T*, window_message&) 
				{	return false; }
			};
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return no_part_spes<T>::type_iterator<list_t>::forward(pT, m); }

#endif // COMET_PARTIAL_SPECIALISATION
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class alt_msg_enum
	//
	// Alternative map's cracker. Used to pass ALTN parameter in msg_enum.
	//
	
	template<int ALTN, COMET_UINT_TEMPLATE> struct alt_msg_enum
	{	typedef msg_enum<COMET_LIST_ARG_0, ALTN> result; };

	////////////////////////////////////////////////////////////////////////////
	//
	// class msg_list
	//
	// Message list cracker. It has one or more handlers that looks as
	// msg's handlers.
	//
	
	template<COMET_UINT_TEMPLATE, int ALTN = 0>
	class msg_list : public details::msg_cracker<0>
	{
 		msg_list(window_message& m) : details::msg_cracker<0>(m)
		{}
		
	public:
		typedef make_list<COMET_WRAP_ARG>::result list_t;
		
#ifdef COMET_PARTIAL_SPECIALISATION

		// helper class type_iterator
		template<typename T, typename LIST> struct type_iterator {
			static bool forward(T* pT, window_message& m) {
				typedef typename LIST::head msg_t;
				if(m.msg.message == msg_t::id && pT->on(msg<msg_t::id>(m)))
					return true;
				return type_iterator<T, typename LIST::tail>::forward(pT, m);
			}
		};
		template<typename T> struct type_iterator<T, nil> {
			static bool forward(const T*, window_message&) 
			{	return false; }
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return type_iterator<T, list_t>::forward(pT, m); }
		
#else // COMET_PARTIAL_SPECIALISATION
		
		// helper class no_part_spes
		template<class T> struct no_part_spes {
			template<class LIST> struct type_iterator {
				static bool forward(T* pT, window_message& m) {
					if(m.msg.message == LIST::head::id && pT->on(msg<LIST::head::id>(m)))
						return true;
					return type_iterator<LIST::tail>::forward(pT, m);
				}
			};
			template<> struct type_iterator<nil> {
				static bool forward(const T*, window_message&) 
				{	return false; }
			};
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return no_part_spes<T>::type_iterator<list_t>::forward(pT, m); }
		
#endif // COMET_PARTIAL_SPECIALISATION
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class alt_msg_list
	//
	// Alternative map's cracker. Used to pass ALTN parameter in msg_list.
	//
	
	template<int ALTN, COMET_UINT_TEMPLATE> struct alt_msg_list
	{	typedef msg_list<COMET_LIST_ARG_0, ALTN> result; };

	////////////////////////////////////////////////////////////////////////////
	//
	// class cmd
	//
	// A command cracker. Its handler looks like:
	//
	// bool on(const cmd<ID, CODE>& m);
	// bool on(cmd<ID, CODE> m);
	//
	
	template<WORD ID, WORD CODE, int ALTN = 0> 
	class cmd : public details::msg_cracker<WM_COMMAND>
	{
	public:
		enum { msg_id = WM_COMMAND, cmd_id = ID, cmd_code = CODE };
 		cmd(window_message& m) : details::msg_cracker<WM_COMMAND>(m)
		{}

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && (cmd_id == LOWORD(m.msg.wParam)) && (cmd_code == HIWORD(m.msg.wParam)) && pT->on(cmd(m)); }
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class cmd_id
	//
	// Command's id cracker handling WM_COMMAND with given command ID. 
	// The handler looks like:
	//
	// bool on(const cmd_id<ID>& m);
	// bool on(cmd_id<ID> m);
	//

	template<WORD ID, int ALTN = 0> 
	class cmd_id : public details::msg_cracker<WM_COMMAND>
	{
	public:
		enum { msg_id = WM_COMMAND, cmd = ID };
 		cmd_id(window_message& m) : details::msg_cracker<WM_COMMAND>(m)
		{}

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && (cmd == LOWORD(m.msg.wParam)) && pT->on(cmd_id(m)); }
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class cmd_id_range
	//
	// Command's id range cracker. Its handler looks as:
	//
	// bool on(const cmd_id_range<FROM, TO>& m);
	// bool on(cmd_id_range<FROM, TO> m);
	//

	template<WORD FROM, WORD TO, int ALTN = 0> 
	class cmd_id_range : public details::msg_cracker<WM_COMMAND>
	{
		COMET_STATIC_ASSERT(FROM < TO);
 		cmd_id_range(window_message& m) : details::msg_cracker<WM_COMMAND>(m)
		{}
	public:
		enum { msg_id = WM_COMMAND, from = FROM, to = TO };

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && (from <= LOWORD(m.msg.wParam)) && (to >= LOWORD(m.msg.wParam)) && pT->on(cmd_id_range(m)); }
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class cmd_id_enum
	//
	// Enumerated command ids' handler. Its handler looks as:
	//
	// bool on(const cmd_id_enum<ID1, ID2, ...>& m);
	// bool on(cmd_id_enum<ID1, ID2, ...> m);
	//
	
	template<COMET_UINT_TEMPLATE, int ALTN = 0>
	class cmd_id_enum : public details::msg_cracker<WM_COMMAND>
	{
	public:
		enum { msg_id = WM_COMMAND };
		typedef make_list<COMET_WRAP_ARG>::result list_t;
		typedef cmd_id_enum	<COMET_LIST_ARG_0, ALTN> self;

 		cmd_id_enum(window_message& m) : details::msg_cracker<WM_COMMAND>(m)
		{}

#ifdef COMET_PARTIAL_SPECIALISATION

		// helper class type_iterator
		template<typename T, typename LIST> struct type_iterator {
			static bool forward(T* pT, window_message& m) {
				typedef typename LIST::head msg_t;
				if(LOWORD(m.msg.wParam) == msg_t::id && pT->on(self(m)))
					return true;
				return type_iterator<T, typename LIST::tail>::forward(pT, m);
			}
		};
		template<typename T> struct type_iterator<T, nil> {
			static bool forward(const T*, window_message&) 
			{	return false; }
		};
		
	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && type_iterator<T, list_t>::forward(pT, m); }
		
#else // COMET_PARTIAL_SPECIALISATION

		// helper class no_part_spes
		template<class T> struct no_part_spes {
			template<class LIST> struct type_iterator {
				static bool forward(T* pT, window_message& m) {
					if((LIST::head::id == LOWORD(m.msg.wParam)) && pT->on(self(m)))
						return true;
					return type_iterator<LIST::tail>::forward(pT, m);
				}
			};
			template<> struct type_iterator<nil> {
				static bool forward(const T*, window_message&) 
				{	return false; }
			};
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && no_part_spes<T>::type_iterator<list_t>::forward(pT, m); }
		
#endif // COMET_PARTIAL_SPECIALISATION
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class alt_cmd_id_enum
	//
	// Alternative map's cracker. Used to pass ALTN parameter in the cmd_id_enum.
	//
	
	template<int ALTN, COMET_UINT_TEMPLATE> struct alt_cmd_id_enum
	{	typedef cmd_id_enum<COMET_LIST_ARG_0, ALTN> result; };

	////////////////////////////////////////////////////////////////////////////
	//
	// class cmd_id_list
	//
	// Command id list handler. It has one or more handlers that looks as
	// the cmd_id's handlers.
	//
	
	template<COMET_UINT_TEMPLATE, int ALTN = 0>
	class cmd_id_list : public details::msg_cracker<WM_COMMAND>
	{
 		cmd_id_list(window_message& m) : details::msg_cracker<WM_COMMAND>(m)
		{}
	public:
		enum { msg_id = WM_COMMAND };
		typedef make_list<COMET_WRAP_ARG>::result list_t;

#ifdef COMET_PARTIAL_SPECIALISATION

		// helper class type_iterator
		template<typename T, typename LIST> struct type_iterator {
			static bool forward(T* pT, window_message& m) {
				typedef typename LIST::head msg_t;
				if(LOWORD(m.msg.wParam) == msg_t::id && pT->on(cmd_id<msg_t::id>(m)))
					return true;
				return type_iterator<T, typename LIST::tail>::forward(pT, m);
			}
		};
		template<typename T> struct type_iterator<T, nil> {
			static bool forward(const T*, window_message&) 
			{	return false; }
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && type_iterator<T, list_t>::forward(pT, m); }
		
#else // COMET_PARTIAL_SPECIALISATION

		// helper class no_part_spes
		template<class T> struct no_part_spes {
			template<class LIST> struct type_iterator {
				static bool forward(T* pT, window_message& m) {
					if((LIST::head::id == LOWORD(m.msg.wParam)) && pT->on(cmd_id<LIST::head::id>(m)))
						return true;
					return type_iterator<LIST::tail>::forward(pT, m);
				}
			};
			template<> struct type_iterator<nil> {
				static bool forward(const T*, window_message&) 
				{	return false; }
			};
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && no_part_spes<T>::type_iterator<list_t>::forward(pT, m); }

#endif // COMET_PARTIAL_SPECIALISATION
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class alt_cmd_id_list
	//
	// Alternative map's cracker. Used to pass ALTN parameter in the cmd_id_list.
	//

	template<int ALTN, COMET_UINT_TEMPLATE> struct alt_cmd_id_list
	{	typedef cmd_id_list<COMET_LIST_ARG_0, ALTN> result; };


	////////////////////////////////////////////////////////////////////////////
	//
	// class cmd_code
	//
	// Command's code cracker handling WM_COMMAND message with specified notification code id.
	// Its handler looks as:
	//
	// bool on(const cmd_code<ID>& m);
	// bool on(cmd_code<ID> m);
	//
	
	template<WORD ID, int ALTN = 0> 
	class cmd_code : public details::msg_cracker<WM_COMMAND>
	{
	public:
		enum { msg_id = WM_COMMAND, code = ID };
 		cmd_code(window_message& m) : details::msg_cracker<WM_COMMAND>(m)
		{}

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && (code == HIWORD(m.msg.wParam)) && pT->on(cmd_code(m)); }
	};

	////////////////////////////////////////////////////////////////////////////
	// 
	// class cmd_code_range
	//
	// Command's code range cracker handling WM_COMMAND message with code's id 
	// in specifiend range. The handler looks as:
	//
	// bool on(const cmd_code_range<FROM, TO>& m);
	// bool on(cmd_code_range<FROM, TO> m);
	//

	template<WORD FROM, WORD TO, int ALTN = 0> 
	class cmd_code_range : public details::msg_cracker<WM_COMMAND>
	{
		COMET_STATIC_ASSERT(FROM < TO);
 		cmd_code_range(window_message& m) : details::msg_cracker<WM_COMMAND>(m)
		{}
		
	public:
		enum { msg_id = WM_COMMAND, from = FROM, to = TO };

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && (from <= HIWORD(m.msg.wParam)) && (to >= HIWORD(m.msg.wParam)) && pT->on(cmd_code_range(m)); }
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class cmd_code_enum
	//
	// Handles WM_COMMAND messages with specified code ids.
	// The handler looks as:
	//
	// bool on(const cmd_code_enum<ID1, ID2, ...>& m);
	// bool on(cmd_code_enum<ID1, ID2, ...> m);
	//

	template<COMET_UINT_TEMPLATE, int ALTN>
	class cmd_code_enum : public details::msg_cracker<WM_COMMAND>
	{
	public:
		typedef make_list<COMET_WRAP_ARG>::result list_t;
		typedef cmd_code_enum<COMET_LIST_ARG_0, ALTN> self;
		enum { msg_id = WM_COMMAND };

 		cmd_code_enum(window_message& m) : details::msg_cracker<WM_COMMAND>(m)
		{}

#ifdef COMET_PARTIAL_SPECIALISATION

		// helper class type_iterator
		template<typename T, typename LIST> struct type_iterator {
			static bool forward(T* pT, window_message& m) {
				typedef typename LIST::head msg_t;
				if(HIWORD(m.msg.wParam) == msg_t::id && pT->on(self(m)))
					return true;
				return type_iterator<T, typename LIST::tail>::forward(pT, m);
			}
		};
		template<typename T> struct type_iterator<T, nil> {
			static bool forward(const T*, window_message&) 
			{	return false; }
		};
		
	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && type_iterator<T, list_t>::forward(pT, m); }
		
#else // COMET_PARTIAL_SPECIALISATION

		// helper class no_part_spes
		template<class T> struct no_part_spes {
			template<class LIST> struct type_iterator {
				static bool forward(T* pT, window_message& m) {
					if((LIST::head::id == HIWORD(m.msg.wParam)) && pT->on(self(m)))
						return true;
					return type_iterator<LIST::tail>::forward(pT, m);
				}
			};
			template<> struct type_iterator<nil> {
				static bool forward(const T*, window_message&) 
				{	return false; }
			};
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && no_part_spes<T>::type_iterator<list_t>::forward(pT, m); }
		
#endif // COMET_PARTIAL_SPECIALISATION
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class alt_cmd_code_enum
	//
	// Alternative map's cracker. Used to pass ALTN parameter in the cmd_code_enum.
	//
	
	template<int ALTN, COMET_UINT_TEMPLATE> struct alt_cmd_code_enum
	{	typedef cmd_code_enum<COMET_LIST_ARG_0, ALTN> result; };

	////////////////////////////////////////////////////////////////////////////
	//
	// class cmd_code_list
	//
	// Handles WM_COMMAND message with notify code id in specified list
	// The handlers look as for the cmd_code cracker
	//

	template<COMET_UINT_TEMPLATE, int ALTN>
	class cmd_code_list : public details::msg_cracker<WM_COMMAND>
	{
 		cmd_code_list(window_message& m) : details::msg_cracker<WM_COMMAND>(m)
		{}

	public:
		typedef make_list<COMET_WRAP_ARG>::result list_t;
		enum { msg_id = WM_COMMAND };

#ifdef COMET_PARTIAL_SPECIALISATION

		// helper class type_iterator
		template<typename T, typename LIST> struct type_iterator {
			static bool forward(T* pT, window_message& m) {
				typedef typename LIST::head msg_t;
				if(HIWORD(m.msg.wParam) == msg_t::id && pT->on(cmd_code<msg_t::id>(m)))
					return true;
				return type_iterator<T, typename LIST::tail>::forward(pT, m);
			}
		};
		template<typename T> struct type_iterator<T, nil> {
			static bool forward(const T*, window_message&) 
			{	return false; }
		};
		
	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && type_iterator<T, list_t>::forward(pT, m); }

#else // COMET_PARTIAL_SPECIALISATION

		template<class T> struct no_part_spes {
			template<class LIST> struct type_iterator {
				static bool forward(T* pT, window_message& m) {
					if((LIST::head::id == HIWORD(m.msg.wParam)) && pT->on(cmd_code<LIST::head::id>(m)))
						return true;
					return type_iterator<LIST::tail>::forward(pT, m);
				}
			};
			template<> struct type_iterator<nil> {
				static bool forward(const T*, window_message&) 
				{	return false; }
			};
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && no_part_spes<T>::type_iterator<list_t>::forward(pT, m); }

#endif // COMET_PARTIAL_SPECIALISATION
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class alt_cmd_code_list
	//
	// Alternative map's cracker. Used to pass ALTN parameter in the cmd_code_list.
	//

	template<int ALTN, COMET_UINT_TEMPLATE> struct alt_cmd_code_list
	{	typedef cmd_code_list<COMET_LIST_ARG_0, ALTN> result; };

	////////////////////////////////////////////////////////////////////////////
	//
	// class ntf
	//
	// A notify cracker. Its handler looks like:
	//
	// bool on(const ntf<ID, CODE>& m);
	// bool on(ntf<ID, CODE> m);
	//

	template<WORD ID, DWORD CODE, int ALTN = 0> 
	class ntf : public details::msg_cracker<WM_NOTIFY>
	{
	public:
		enum { msg_id = WM_NOTIFY, ntf_id = ID, ntf_code = CODE };
 		ntf(window_message& m) : details::msg_cracker<WM_NOTIFY>(m)
		{}

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && (ntf_id == reinterpret_cast<LPNMHDR>(m.msg.lParam)->idFrom) && (ntf_code == reinterpret_cast<LPNMHDR>(m.msg.lParam)->code) && pT->on(ntf(m)); }
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class ntf_id
	//
	// Notify's id cracker handling WM_NOTIFY with given notify ID. 
	// The handler looks like:
	//
	// bool on(const ntf_id<ID>& m);
	// bool on(ntf_id<ID> m);
	//

	template<WORD ID, int ALTN = 0> 
	class ntf_id : public details::msg_cracker<WM_NOTIFY>
	{
	public:
		enum { msg_id = WM_NOTIFY, ntf = ID };
 		ntf_id(window_message& m) : details::msg_cracker<WM_NOTIFY>(m)
		{}

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && (ntf == reinterpret_cast<LPNMHDR>(m.msg.lParam)->idFrom) && pT->on(ntf_id(m)); }
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class ntf_id_range
	//
	// Notify's id range cracker. Its handler looks as:
	//
	// bool on(const ntf_id_range<FROM, TO>& m);
	// bool on(ntf_id_range<FROM, TO> m);
	//

	template<WORD FROM, WORD TO, int ALTN = 0> 
	class ntf_id_range : public details::msg_cracker<WM_NOTIFY>
	{
		COMET_STATIC_ASSERT(FROM < TO);
 		ntf_id_range(window_message& m) : details::msg_cracker<WM_NOTIFY>(m)
		{}

	public:
		enum { msg_id = WM_NOTIFY, from = FROM, to = TO };

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && (from <= reinterpret_cast<LPNMHDR>(m.msg.lParam)->idFrom) && (to >= reinterpret_cast<LPNMHDR>(m.msg.lParam)->idFrom) && pT->on(ntf_id_range(m)); }
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class ntf_id_enum
	//
	// Enumerated notify ids' handler. Its handler looks as:
	//
	// bool on(const ntf_id_enum<ID1, ID2, ...>& m);
	// bool on(ntf_id_enum<ID1, ID2, ...> m);
	//

	template<COMET_UINT_TEMPLATE, int ALTN = 0>
	class ntf_id_enum : public details::msg_cracker<WM_NOTIFY>
	{
	public:
		enum { msg_id = WM_NOTIFY };
		typedef make_list<COMET_WRAP_ARG>::result list_t;
		typedef ntf_id_enum<COMET_LIST_ARG_0, ALTN> self;

 		ntf_id_enum(window_message& m) : details::msg_cracker<WM_NOTIFY>(m)
		{}

#ifdef COMET_PARTIAL_SPECIALISATION

		// helper class type_iterator
		template<typename T, typename LIST> struct type_iterator {
			static bool forward(T* pT, window_message& m) {
				typedef typename LIST::head msg_t;
				if(reinterpret_cast<LPNMHDR>(m.msg.lParam)->idFrom == msg_t::id && pT->on(self(m)))
					return true;
				return type_iterator<T, typename LIST::tail>::forward(pT, m);
			}
		};
		template<typename T> struct type_iterator<T, nil> {
			static bool forward(const T*, window_message&) 
			{	return false; }
		};

#else // COMET_PARTIAL_SPECIALISATION

		// helper class no_part_spes
		template<class T> struct no_part_spes {
			template<class LIST> struct type_iterator {
				static bool forward(T* pT, window_message& m) {
					if((LIST::head::id == reinterpret_cast<LPNMHDR>(m.msg.lParam)->idFrom) && pT->on(self(m)))
						return true;
					return type_iterator<LIST::tail>::forward(pT, m);
				}
			};
			template<> struct type_iterator<nil> {
				static bool forward(const T*, window_message&) 
				{	return false; }
			};
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && no_part_spes<T>::type_iterator<list_t>::forward(pT, m); }

#endif // COMET_PARTIAL_SPECIALISATION
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class alt_ntf_id_enum
	//
	// Alternative map's cracker. Used to pass ALTN parameter in the ntf_id_enum.
	//

	template<int ALTN, COMET_UINT_TEMPLATE> struct alt_ntf_id_enum
	{	typedef ntf_id_enum<COMET_LIST_ARG_0, ALTN> result; };

	////////////////////////////////////////////////////////////////////////////
	//
	// class ntf_id_list
	//
	// Notify id list handler. It has one or more handlers that looks as
	// the ntf_id's handlers.
	//

	template<COMET_UINT_TEMPLATE, int ALTN>
	class ntf_id_list : public details::msg_cracker<WM_NOTIFY>
	{
 		ntf_id_list(window_message& m) : details::msg_cracker<WM_NOTIFY>(m)
		{}

	public:
		enum { msg_id = WM_NOTIFY };
		typedef make_list<COMET_WRAP_ARG>::result list_t;

#ifdef COMET_PARTIAL_SPECIALISATION

		// helper class type_iterator
		template<typename T, typename LIST> struct type_iterator {
			static bool forward(T* pT, window_message& m) {
				typedef typename LIST::head msg_t;
				if(reinterpret_cast<LPNMHDR>(m.msg.lParam)->idFrom == msg_t::id && pT->on(ntf_id<msg_t::id>(m)))
					return true;
				return type_iterator<T, typename LIST::tail>::forward(pT, m);
			}
		};
		template<typename T> struct type_iterator<T, nil> {
			static bool forward(const T*, window_message&) 
			{	return false; }
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && type_iterator<T, list_t>::forward(pT, m); }

#else // COMET_PARTIAL_SPECIALISATION

		// helper class no_part_spes
		template<class T> struct no_part_spes {
			template<class LIST> struct type_iterator {
				static bool forward(T* pT, window_message& m) {
					if((LIST::head::id == reinterpret_cast<LPNMHDR>(m.msg.lParam)->idFrom) && pT->on(ntf_id<LIST::head::id>(m)))
						return true;
					return type_iterator<LIST::tail>::forward(pT, m);
				}
			};
			template<> struct type_iterator<nil> {
				static bool forward(const T*, window_message&) 
				{	return false; }
			};
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && no_part_spes<T>::type_iterator<list_t>::forward(pT, m); }

#endif // COMET_PARTIAL_SPECIALISATION
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// alt_ntf_id_list
	//
	// Alternative map's cracker. Used to pass ALTN parameter in the ntf_id_list.
	//

	template<int ALTN, COMET_UINT_TEMPLATE> struct alt_ntf_id_list
	{	typedef ntf_id_list<COMET_LIST_ARG_0, ALTN> result; };

	////////////////////////////////////////////////////////////////////////////
	//
	// class ntf_code
	//
	// Notify code cracker handling WM_NOTIFY message with specified notification code id.
	// Its handler looks as:
	//
	// bool on(const ntf_code<ID>& m);
	// bool on(ntf_code<ID> m);
	//

	template<DWORD ID, int ALTN = 0> 
	class ntf_code : public details::msg_cracker<WM_NOTIFY>
	{
	public:
		enum { msg_id = WM_NOTIFY, code = ID };
 		ntf_code(window_message& m) : details::msg_cracker<WM_NOTIFY>(m)
		{}

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && (code == reinterpret_cast<LPNMHDR>(m.msg.lParam)->code) && pT->on(ntf_code(m)); }
	};

	////////////////////////////////////////////////////////////////////////////
	// 
	// class ntf_code_range
	//
	// Notify code range cracker handling WM_NOTIFY message with code's id 
	// in specifiend range. The handler looks as:
	//
	// bool on(const ntf_code_range<FROM, TO>& m);
	// bool on(ntf_code_range<FROM, TO> m);
	//

	template<DWORD FROM, DWORD TO, int ALTN = 0> 
	class ntf_code_range : public details::msg_cracker<WM_NOTIFY>
	{
		COMET_STATIC_ASSERT(FROM < TO);
 		ntf_code_range(window_message& m) : details::msg_cracker<WM_NOTIFY>(m)
		{}
	public:
		enum { msg_id = WM_NOTIFY, from = FROM, to = TO };

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && (from <= reinterpret_cast<LPNMHDR>(m.msg.lParam)->code) && (to >= reinterpret_cast<LPNMHDR>(m.msg.lParam)->code) && pT->on(ntf_code_range(m)); }
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class ntf_code_enum
	//
	// Handles WM_NOTIFY messages with specified code ids.
	// The handler looks as:
	//
	// bool on(const ntf_code_enum<ID1, ID2, ...>& m);
	// bool on(ntf_code_enum<ID1, ID2, ...> m);
	//

	template<COMET_UINT_TEMPLATE, int ALTN = 0>
	class ntf_code_enum : public details::msg_cracker<WM_NOTIFY>
	{
	public:
		typedef make_list<COMET_WRAP_ARG>::result list_t;
		typedef ntf_code_enum<COMET_LIST_ARG_0, ALTN> self;
		enum { msg_id = WM_NOTIFY };

 		ntf_code_enum(window_message& m) : details::msg_cracker<WM_NOTIFY>(m)
		{}

#ifdef COMET_PARTIAL_SPECIALISATION

		// helper class type_iterator
		template<typename T, typename LIST> struct type_iterator {
			static bool forward(T* pT, window_message& m) {
				typedef typename LIST::head msg_t;
				if(reinterpret_cast<LPNMHDR>(m.msg.lParam)->code == msg_t::id && pT->on(self(m)))
					return true;
				return type_iterator<T, typename LIST::tail>::forward(pT, m);
			}
		};
		template<typename T> struct type_iterator<T, nil> {
			static bool forward(const T*, window_message&) 
			{	return false; }
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && type_iterator<T, list_t>::forward(pT, m); }

#else // COMET_PARTIAL_SPECIALISATION

		// helper class no_part_spes
		template<class T> struct no_part_spes {
			template<class LIST> struct type_iterator {
				static bool forward(T* pT, window_message& m) {
					if((LIST::head::id == reinterpret_cast<LPNMHDR>(m.msg.lParam)->code) && pT->on(self(m)))
						return true;
					return type_iterator<LIST::tail>::forward(pT, m);
				}
			};
			template<> struct type_iterator<nil> {
				static bool forward(const T*, window_message&) 
				{	return false; }
			};
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && no_part_spes<T>::type_iterator<list_t>::forward(pT, m); }

#endif // COMET_PARTIAL_SPECIALISATION
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class alt_ntf_code_enum
	//
	// Alternative map's cracker. Used to pass ALTN parameter in the ntf_code_enum.
	//

	template<int ALTN, COMET_UINT_TEMPLATE> struct alt_ntf_code_enum
	{	typedef ntf_code_enum<COMET_LIST_ARG_0, ALTN> result; };

	////////////////////////////////////////////////////////////////////////////
	//
	// class ntf_code_list
	//
	// Handles WM_NOTIFY message with notify code id in specified list
	// The handlers look as for the cmd_code cracker
	//

	template<COMET_UINT_TEMPLATE, int ALTN>
	class ntf_code_list : public details::msg_cracker<WM_NOTIFY>
	{
 		ntf_code_list(window_message& m) : details::msg_cracker<WM_NOTIFY>(m)
		{}

	public:
		typedef make_list<COMET_WRAP_ARG>::result list_t;
		enum { msg_id = WM_NOTIFY };

#ifdef COMET_PARTIAL_SPECIALISATION

		// helper class type_iterator
		template<typename T, typename LIST> struct type_iterator {
			static bool forward(T* pT, window_message& m) {
				typedef typename LIST::head msg_t;
				if(reinterpret_cast<LPNMHDR>(m.msg.lParam)->code == msg_t::id && pT->on(ntf_code<msg_t::id>(m)))
					return true;
				return type_iterator<T, typename LIST::tail>::forward(pT, m);
			}
		};
		template<typename T> struct type_iterator<T, nil> {
			static bool forward(const T*, window_message&) 
			{	return false; }
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && type_iterator<T, list_t>::forward(pT, m); }

#else // COMET_PARTIAL_SPECIALISATION

		template<class T> struct no_part_spes {
			template<class LIST> struct type_iterator {
				static bool forward(T* pT, window_message& m) {
					if((LIST::head::id == reinterpret_cast<LPNMHDR>(m.msg.lParam)->code) && pT->on(ntf_code<LIST::head::id>(m)))
						return true;
					return type_iterator<LIST::tail>::forward(pT, m);
				}
			};
			template<> struct type_iterator<nil> {
				static bool forward(const T*, window_message&) 
				{	return false; }
			};
		};

	public:
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return (m.msg.message == msg_id) && no_part_spes<T>::type_iterator<list_t>::forward(pT, m); }

#endif // COMET_PARTIAL_SPECIALISATION
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class alt_ntf_code_list
	//
	// Alternative map's cracker. Used to pass ALTN parameter in the ntf_code_list.
	//

	template<int ALTN, COMET_UINT_TEMPLATE> struct alt_ntf_code_list
	{	typedef ntf_code_list<COMET_LIST_ARG_0, ALTN> result; };

	////////////////////////////////////////////////////////////////////////////
	//
	// class chain
	//
	// Chains message map of base class
	//

	template<class CHAIN>
	struct chain {
		template<class T> static bool dispatch(T* pT, window_message& m) 
		{	return process_message_map(static_cast<CHAIN*>(pT), m); }
	};

	template<> struct chain<nil> {
		template<class T> static bool dispatch(T*, window_message&) 
		{	return false; }
	};

	////////////////////////////////////////////////////////////////////////////

	template<class MAP>
	struct chain_map {
		template<class T> static bool dispatch(T* pT, window_message& m) {
#ifdef COMET_PARTIAL_SPECIALISATION
			return details::map_holder<T, typename MAP>::forward(pT, m); }
#else // COMET_PARTIAL_SPECIALISATION
			return details::map_holder<T>::map<COMET_STRICT_TYPENAME MAP>::forward(pT, m);
		}
#endif // COMET_PARTIAL_SPECIALISATION
	};

////////////////////////////////////////////////////////////////////////////////
// Class template submap
////////////////////////////////////////////////////////////////////////////////

    template<class Submap>
	struct submap
	{
		Submap submap_;

		submap() : submap_(static_cast<Submap::host_type&>(*this))
		{}

		virtual bool process_message(window_message& m)
		{	return process_message_map(&submap_, m);	}
	};

	template<class T>
	struct chain_submap
	{
		static bool dispatch(T* pT, cometw::window_message& m)
		{	return pT->process_message(m);	}
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// class chain_entry
	//
	// Chains specified cracker of base class
	//

	template<class CHAIN, class ENTRY>
	struct chain_entry {
		template<class T> static bool dispatch(T* pT, window_message& m) {
			return ENTRY::dispatch(static_cast<CHAIN*>(pT), m);
		}
	};

	////////////////////////////////////////////////////////////////////////////
	//
	// forward_cmd_mdichild
	//

	class forward_cmd_mdichild
	{
	public:
		enum { msg_id = WM_COMMAND };

	public:
		template<class T> static bool dispatch(T* pT, const window_message& m) {
			if(m.message_ != msg_id) return false;
			HWND hmdichild = reinterpret_cast<HWND>(::SendMessage(pT->mdiclient(), WM_MDIGETACTIVE, 0U, 0));
			if(hmdichild)
				::SendMessage(hmdichild, m.message_, m.wparam_, m.lparam_);
			return false;
		}
	};

} // cometw

////////////////////////////////////////////////////////////////////////////////
// Change log:
// April 13, 2002: initially released by Vladimir Voinkov and Sofus Mortensen
////////////////////////////////////////////////////////////////////////////////

#endif // COMETW_MESSAGE_MAP_H
