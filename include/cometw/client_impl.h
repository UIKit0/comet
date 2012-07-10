//
// client_impl.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef COMETW_CLIENT_IMPL_H
#define COMETW_CLIENT_IMPL_H

#include <algorithm>
#include <cometw/config.h>
#include <cometw/controls.h>
#include <commctrl.h>

using namespace comet;

namespace cometw {

////////////////////////////////////////////////////////////////////////////////
// Class simple_client
// Implements client for frame_impl and mdichild_impl. Behavior is not fancy;
//  it just adjusts given as first parameter window to fullest client extent.
//  WINDOW might be window implementation or window wrapper. Takes a care
//  of handling WM_CREATE, WM_SIZE and WM_SETFOCUS
////////////////////////////////////////////////////////////////////////////////

	template
	<
		typename WINDOW, 
		DWORD STYLE = 0, 
		DWORD EXSTYLE = 0
	>
	class simple_client
	{
	public:
		typedef make_list< msg_list<WM_CREATE, WM_SIZE, WM_SETFOCUS> >::result msg_map;

		bool on(msg<WM_CREATE> m)
		{
			if(!wnd_.create(m.hwnd(), NULL, STYLE, EXSTYLE))
			{
				m.set_result(-1);
				return true;
			}
			return false;
		}

		bool on(msg<WM_SIZE> m)
		{
			::MoveWindow(wnd_.hwnd_, 0, 0, m.cx(), m.cy(), true);
			return false;
		}

		bool on(msg<WM_SETFOCUS>)
		{
			::SetFocus(wnd_.hwnd_);
			return false;
		}

	private:
		WINDOW wnd_;
	};

////////////////////////////////////////////////////////////////////////////////
// Class client_impl
// A general client implementation. 
// COMET_LIST_TEMPLATE is typelist of pane policies
////////////////////////////////////////////////////////////////////////////////

	template
	<
		typename T, 
		COMET_LIST_TEMPLATE
	>
	class client_impl : 
		public typelist::inherit_all<make_list<COMET_LIST_ARG_1>::result>
	{
	public:
		typedef make_list<COMET_LIST_ARG_1>::result pane_list;
		typedef make_list< msg_list<WM_SETFOCUS, WM_CREATE, WM_SIZE, WM_ACTIVATEAPP,
			WM_ACTIVATE> >::result msg_map;

		// At list one pane should exist
		enum { pane_count = typelist::length<pane_list>::value };
		COMET_STATIC_ASSERT(pane_count > 0);

	private:
		window focus_;
		window activeChild_[pane_count];

	public:
		bool on(msg<WM_SETFOCUS>) 
		{
			if(!focus_)
			{
				int pane = 0;
				focus_ = index_iterator<0>::query_pane(static_cast<T&>(*this), pane);
			}
			if(focus_)
                ::SetFocus(focus_);
			return false;
		}
		bool on(msg<WM_CREATE> m) 
		{
			if(!index_iterator<0>::create_pane(static_cast<T&>(*this), m.hwnd())) 
			{
				m.set_result(-1);
				return true;
			}
			return false;
		}
		bool on(msg<WM_SIZE> m) 
		{
			T& t = static_cast<T&>(*this);
			RECT rc;
			::GetClientRect(t.hwnd_, &rc);
			index_iterator<0>::update_pane_layout(t, rc);
			return true;
		}
		bool on(msg<WM_ACTIVATE> m)
		{
			if(m.state() == WA_INACTIVE)
				focus_ = ::GetFocus();
			else
			{
				if(!focus_)
				{
					int pane = 0;
					focus_ = index_iterator<0>::query_pane(static_cast<T&>(*this), pane);
				}
				if(focus_)
					::SetFocus(focus_);
			}
			return false;
		}
		bool on(msg<WM_ACTIVATEAPP> m)
		{
			if(!m.activation())
				focus_ = ::GetFocus();
			else
			{
				if(!focus_)
				{
					int pane = 0;
					focus_ = index_iterator<0>::query_pane(static_cast<T&>(*this), pane);
				}
				if(focus_)
					::SetFocus(focus_);
			}
			return false;
		}

	public:
        int active_pane()
		{
			HWND activeChild = ::GetFocus();

			T* pT = static_cast<T*>(this);
			if(!::IsChild(pT->hwnd_, activeChild))
			{
				if(::IsChild(pT->hwnd_, focus_))
                    activeChild = focus_;
				else
                    activeChild = NULL;
			}
			if(activeChild == NULL)
				return -1;
			return index_iterator<0>::query_child(static_cast<T&>(*this), activeChild);
		}
        bool next_pane(bool next = true)
		{
			int pane = active_pane();
			if(pane == -1)
				pane = 0;
			else
			{
				COMET_ASSERT(pane < pane_count);
				activeChild_[pane] = ::GetFocus();
			}

			HWND hwnd;
			if(next)
			{
				pane = ++pane % pane_count;
				hwnd = index_iterator<0>::query_pane(static_cast<T&>(*this), pane);
			}
			else
			{
				pane = (pane_count + pane - 1) % pane_count;
				hwnd = index_iterator<pane_count - 1>::query_pane_backward(static_cast<T&>(*this), pane);
			}

			if(hwnd)
			{
				COMET_ASSERT(pane < pane_count);
				HWND focus = activeChild_[pane];
				focus = ::IsWindow(focus) ? focus : hwnd;
                ::SetFocus(focus);
				focus_ = focus;
				return true;
			}

			return false;
		}
		void update_layout()
		{
			RECT rc;
			::GetClientRect(static_cast<T&>(*this).hwnd_, &rc);
			index_iterator<0>::update_pane_layout(static_cast<T&>(*this), rc);
		}

	public:
		////////////////////////////////////////////////////////////////////////
		// Pane iterating helper
		template<int Idx> struct index_iterator
		{
			enum { pane_index = Idx };
			static HWND query_pane(T& x, int& start, bool passedEnd = false)
			{
				if(!passedEnd && pane_index < start)
					return index_iterator<pane_index + 1>::query_pane(x, start, passedEnd);
				if(passedEnd && pane_index >= start)
					return NULL;
				if(static_cast<typelist::type_at<pane_list, pane_index>::result&>(x).client_query_default_focus())
				{
					start = pane_index;
					return static_cast<typelist::type_at<pane_list, pane_index>::result&>(x).get().wnd();
				}
				return index_iterator<pane_index + 1>::query_pane(x, start, passedEnd);
			}
			static HWND query_pane_backward(T& x, int& start, bool passedBegin = false)
			{
				if(!passedBegin && pane_index > start)
					return index_iterator<pane_index - 1>::query_pane_backward(x, start, passedBegin);
				if(passedBegin && pane_index <= start)
					return NULL;
				if(static_cast<typelist::type_at<pane_list, pane_index>::result&>(x).client_query_default_focus())
				{
					start = pane_index;
					return static_cast<typelist::type_at<pane_list, pane_index>::result&>(x).get().wnd();
				}
				return index_iterator<pane_index - 1>::query_pane_backward(x, start, passedBegin);
			}
			static bool create_pane(T& x, HWND hwnd) 
			{
				if(!static_cast<typelist::type_at<pane_list, pane_index>::result&>(x).client_create(hwnd))
					return false;
				return index_iterator<pane_index + 1>::create_pane(x, hwnd);
			}
			static void update_pane_layout(T& x, RECT& rc)
			{
				static_cast<typelist::type_at<pane_list, pane_index>::result&>(x).client_update_layout(rc);
				index_iterator<pane_index + 1>::update_pane_layout(x, rc);
			}
			static int query_child(T& x, HWND hwnd)
			{
				HWND hself = static_cast<typelist::type_at<pane_list, pane_index>::result&>(x).get().wnd();
				if(hself == hwnd || ::IsChild(hself, hwnd))
					return pane_index;
				return index_iterator<pane_index + 1>::query_child(x, hwnd);
			}
		};
		template<> struct index_iterator<pane_count>
		{
			static HWND query_pane(T& x, int& start, bool)
			{	return index_iterator<0>::query_pane(x, start, true);	}
			static bool create_pane(T&, HWND) 
			{	return true;	}
			static void update_pane_layout(T&, RECT&)
			{}
			static int query_child(T&, HWND) 
			{	return -1;	}
		};
		template<> struct index_iterator<-1>
		{
			static HWND query_pane_backward(T& x, int& start, bool)
			{	return index_iterator<pane_count - 1>::query_pane_backward(x, start, true);	}
		};
	};

////////////////////////////////////////////////////////////////////////////////

	enum align { left, top, right, bottom, all };

	template<INT N> struct type {};

	namespace impl {

////////////////////////////////////////////////////////////////////////////////
// Resize helpers
////////////////////////////////////////////////////////////////////////////////

		inline void resize(type<cometw::align::top>, HWND hwnd, RECT& extent)
		{
			// Keep the height been assigned when creating
			RECT rc;
			::GetWindowRect(hwnd, &rc);
			long cy = rc.bottom - rc.top;

			rc = extent;
			rc.bottom = rc.top + cy;
			::MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, cy, TRUE);
			::SubtractRect(&extent, &extent, &rc);
		}

		template<class T>
		inline void resize(type<cometw::align::top>, T& wnd, RECT& extent)
		{}

		inline void resize(type<cometw::align::top>, edit_control& wnd, RECT& extent)
		{}
		
		inline void resize(type<cometw::align::bottom>, HWND hwnd, RECT& extent)
		{
			RECT rc;
			::GetWindowRect(hwnd, &rc);
			long cy = rc.bottom - rc.top;

			rc = extent;
			rc.top =  rc.bottom - cy;
			::MoveWindow(hwnd, rc.left, rc.top, rc.right - rc.left, cy, TRUE);
			::SubtractRect(&extent, &extent, &rc);
		}

		inline void resize(type<cometw::align::left>, HWND hwnd, RECT& extent)
		{
			// Keep the width been assigned when creating
			RECT rc;
			::GetWindowRect(hwnd, &rc);
			long cx = rc.right - rc.left;

			rc = extent;
			rc.right = rc.left + cx;
			::MoveWindow(hwnd, rc.left, rc.top, cx, rc.bottom - rc.top, TRUE);
			::SubtractRect(&extent, &extent, &rc);
		}

		inline void resize(type<cometw::align::right>, HWND hwnd, RECT& extent)
		{
			// Keep the width been assigned when creating
			RECT rc;
			::GetWindowRect(hwnd, &rc);
			long cx = rc.right - rc.left;

			rc = extent;
			rc.left = rc.right - cx;
			::MoveWindow(hwnd, rc.left, rc.top, cx, rc.bottom - rc.top, TRUE);
			::SubtractRect(&extent, &extent, &rc);
		}

		inline void resize(type<cometw::align::all>, HWND hwnd, RECT& extent)
		{
			::MoveWindow(hwnd, extent.left, extent.top, extent.right - extent.left, 
				extent.bottom - extent.top, TRUE);
			::SetRectEmpty(&extent);
		}
	}

////////////////////////////////////////////////////////////////////////////////
// Class template docked_pane
////////////////////////////////////////////////////////////////////////////////

	template
	<
		typename WINDOW,
		align Align = cometw::align::bottom,
		long Height = 20
	>
	class docked_pane
	{
		WINDOW pane_;

	// pane interface
	public:
		bool client_create(HWND hparent) 
		{
			if(!pane_.create(hparent))
				return false;
			::MoveWindow(pane_.hwnd_, 0, 0, Height, Height, false);
			return true;
		}

		void client_update_layout(RECT& extent)
		{	impl::resize(type<Align>(), pane_.hwnd_, extent);	}

		bool client_query_default_focus() 
		{
			::SetFocus(pane_.hwnd_);
			return true;
		}

	public:
		WINDOW& get()
		{	return pane_;	}

		const WINDOW& get() const
		{	return pane_;	}

		long height() const
		{	return height_;	}

		void height(long y) const
		{	height_ = y;	}
	};

////////////////////////////////////////////////////////////////////////////////
// Class template statusbar_pane
////////////////////////////////////////////////////////////////////////////////

	template<align Align = cometw::align::bottom, bool Grip = true>
	class statusbar_pane
	{
		enum { 
			style = (Grip ? SBARS_SIZEGRIP : 0)  
				| (Align == cometw::align::left ? CCS_LEFT : 0) | (Align == cometw::align::top ? CCS_TOP : 0) 
				| (Align == cometw::align::right ? CCS_RIGHT : 0) | (Align == cometw::align::bottom ? CCS_BOTTOM : 0)
		};

		statusbar_control pane_;

	// pane interface
	public:
		bool client_query_default_focus() 
		{	return false;	}

		bool client_create(HWND hparent) 
		{
			if(!pane_.create(hparent, NULL, _T("Ready"), WS_VISIBLE|WS_CHILD|style, NULL, 0U, NULL))
				return false;

			// Stop self-willed resizing
			SetWindowLong(pane_, GWL_STYLE, GetWindowLong(pane_, GWL_STYLE)|CCS_NORESIZE);
			return true;
		}

		void client_update_layout(RECT& extent)
		{	impl::resize(type<Align>(), pane_.hwnd_, extent);	}

	public:
		statusbar_control& get()
		{	return pane_;	}

		const statusbar_control& get() const
		{	return pane_;	}
	};

////////////////////////////////////////////////////////////////////////////////
// Class worker_window
////////////////////////////////////////////////////////////////////////////////

typedef window_traits<WS_CHILD|WS_VISIBLE|WS_CLIPCHILDREN|WS_CLIPSIBLINGS|CCS_TOP,
	0, CTLCOLOR_DLG> worker_traits;
/*
class worker_window :
	public window_impl
	<
		worker_window, worker_traits, 
		msg_list<WM_CREATE, WM_LBUTTONDOWN, WM_MOUSEMOVE, WM_LBUTTONUP>,
		ntf_code<RBN_HEIGHTCHANGE>
	>
{
	long height_;
	long min_height_;

public:
	worker_window() : height_(0)
	{}

public:
	static LPCTSTR registry_window_class_name()
	{	return _T("WorkerWindow");	}

	void update_layout(RECT& rc)
	{
		if(height_ > rc.bottom - rc.top)
			height_ = rc.bottom - rc.top;
		::MoveWindow(hwnd_, rc.left, rc.top, rc.right, rc.top + height_, TRUE);
		rc.top += height_;
	}
	void process(long height)
	{
		height_ = std::max<long>(height, min_height_);
		::SendMessage(::GetParent(hwnd_), WM_SIZE, 0, 0);
	}

public:
	bool on(msg<WM_CREATE>)
	{
		RECT rc;
		::GetClientRect(hwnd_, &rc);
		height_ = rc.bottom - rc.top;
		min_height_ = height_;
		return true;
	}

	bool on(msg<WM_LBUTTONDOWN> m)
	{
		if(::GetCapture())
			return true;
		::SetCapture(hwnd_);
		process(m.y());
		return true;
	}
	bool on(msg<WM_MOUSEMOVE> m)
	{
		if(::GetCapture() != hwnd_)
			return true;
		process(m.y());
		return true;
	}
	bool on(msg<WM_LBUTTONUP> m)
	{
		if(::GetCapture() != hwnd_)
			return true;
		::ReleaseCapture();
		process(m.y());
		return true;
	}
	bool on(ntf_code<RBN_HEIGHTCHANGE>)
	{	return true;	}
};
*/
////////////////////////////////////////////////////////////////////////////////
// Class template worker
////////////////////////////////////////////////////////////////////////////////
/*
template
<
	typename WINDOW,
	align Align = cometw::align::top,
	long Height = 20
>
struct worker
{
	worker_window worker_;
	WINDOW pane_;

// pane interface
public:
	bool client_create(HWND hparent) 
	{
		RECT rc = { 0, 0, Height, Height };
		if(!worker_.create(hparent, rc) || !pane_.create(worker_.wnd()))
			return false;
		return true;
	}

	void client_update_layout(RECT& extent)
	{	
		//impl::resize(type<Align>(), worker_.wnd(), extent);	
		worker_.update_layout(extent);
	}

	bool client_query_default_focus() 
	{
		::SetFocus(worker_.wnd());
		return true;
	}

public:
	WINDOW& get()
	{	return pane_;	}

	const WINDOW& get() const
	{	return pane_;	}
};
*/
////////////////////////////////////////////////////////////////////////////////
// Class mdiclient_pane
////////////////////////////////////////////////////////////////////////////////

	template<typename T>
	class mdiclient_pane
	{
	public:
		bool client_create(HWND hparent) 
		{
			HWND hwnd = details::create_mdiclient(hparent, ::GetSubMenu(::GetMenu(hparent), 1));
			if(hwnd == NULL)
				return false;

			static_cast<T&>(*this).mdiclient(hwnd);
			return true;
		}

		void client_update_layout(RECT& extent) 
		{	impl::resize(type<cometw::align::all>, static_cast<const T&>(*this).mdiclient(), extent);	}

		bool client_query_default_focus() 
		{
			::SetFocus(static_cast<const T&>(*this).mdiclient());
			return true;
		}
	};
}

////////////////////////////////////////////////////////////////////////////////
// Change log:
// July 24, 2002: initially released by Vladimir Voinkov.
////////////////////////////////////////////////////////////////////////////////

#endif // COMETW_CLIENT_IMPL_H
