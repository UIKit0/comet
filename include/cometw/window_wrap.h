//
// window_wrap.h
//
////////////////////////////////////////////////////////////////////////////////

#ifndef COMETW_WINDOW_WRAP_H
# define COMETW_WINDOW_WRAP_H

#pragma warning(disable : 4312)
#pragma warning(disable : 4267)

namespace cometw {

////////////////////////////////////////////////////////////////////////////////
// Helpers
////////////////////////////////////////////////////////////////////////////////

	struct auto_rect {
		auto_rect(LPCRECT prc) : prc_(prc) {}
		auto_rect(const RECT& rc) : prc_(&rc) {}
		operator LPCRECT() const { return prc_; }
	private:
		LPCRECT prc_;
	};
	struct auto_menu_id {
		auto_menu_id(HMENU h) : h_(h) {}
		auto_menu_id(UINT id) : h_(reinterpret_cast<HMENU>(id)) {}
		operator HMENU() const { return h_; }
		operator UINT() const { return reinterpret_cast<UINT>(h_); }
	private:
		HMENU h_;
	};
	struct auto_string_id {
		auto_string_id(LPCTSTR s) : s_(s) {}
		auto_string_id(UINT id) : s_(MAKEINTRESOURCE(id)) {}
		operator LPCTSTR() const { return s_; }
	private:
		LPCTSTR s_;
	};
	struct auto_string_atom {
		auto_string_atom(LPCTSTR s) : s_(s) {}
		auto_string_atom(ATOM a) : s_(reinterpret_cast<LPCTSTR>(MAKELONG(a, 0))) {}
		operator LPCTSTR() const { return s_; }
	private:
		LPCTSTR s_;
	};

	inline LPCRECT default_rect()
	{
		static RECT rc = { CW_USEDEFAULT, CW_USEDEFAULT, 0, 0 };
		return &rc;
	}

	inline HWND create(const auto_string_atom& wndclass, HWND hparent, const auto_rect& rect = NULL, LPCTSTR name = NULL, 
		DWORD style = 0, DWORD exstyle = 0, const auto_menu_id& menu = 0U, void* param = NULL)
	{
		const RECT& rc = *(rect ? static_cast<LPCRECT>(rect) : default_rect());
		return ::CreateWindowEx(exstyle, wndclass, name, style, 
			rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hparent, menu,
			comet::module().instance(), param);
	}

	namespace impl {

////////////////////////////////////////////////////////////////////////////////
//
// class cometw::impl::window_base
//
// Base class for all wrapper and window implementations
//
////////////////////////////////////////////////////////////////////////////////

		class window_base
		{
		protected:
			window_base() throw() : hwnd_(NULL)
			{}

			window_base(HWND hwnd) throw() : hwnd_(hwnd)
			{}

		public:
			bool create(const auto_string_atom& wndclass, HWND hparent, const auto_rect& rect = NULL, 
				LPCTSTR name = NULL, DWORD style = 0, DWORD exstyle = 0, const auto_menu_id& menu = 0U, 
				void* param = NULL)
			{
				const RECT& rc = *(rect ? static_cast<LPCRECT>(rect) : default_rect());
				hwnd_ = ::CreateWindowEx(exstyle, wndclass, name, style, 
					rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hparent, menu,
					comet::module().instance(), param);
				return (hwnd_ != NULL);
			}

		public:
			HWND wnd() const throw()
			{	return hwnd_;	}

			bool text(LPCTSTR str) throw()
			{	return (::SetWindowText(hwnd_, str) != FALSE);	}

			LPTSTR text(LPTSTR buffer, size_t size) const throw()
			{
				::GetWindowText(hwnd_, buffer, size);
				return buffer;
			}

			size_t text_length() const throw()
			{	return static_cast<size_t>(::GetWindowTextLength(hwnd_));	}

# ifdef COMET_TSTRING_H
			bool text(const comet::tstring& str) throw()
			{	return (::SetWindowText(hwnd_, str.c_str()) != FALSE);	}

			tstring text() const throw(std::bad_alloc)
			{
				tstring str(text_length() + 1, 0);
				::GetWindowText(hwnd_, const_cast<LPTSTR>(str.c_str()), str.length());
				return str;
			}

			void text(tstring& str) const throw(std::bad_alloc)
			{
				size_t size = text_length();
				str.resize(++size);
				::GetWindowText(hwnd_, const_cast<LPTSTR>(str.c_str()), size);
				str.resize(--size);
			}
# endif	// COMET_TSTRING_H

			bool destroy() throw()
			{	return (::DestroyWindow(hwnd_) != FALSE);	}

			bool is_child(HWND hwnd)
			{	return (IsChild(hwnd_, hwnd) != FALSE);	}

			HWND hwnd_;
		};

	}	// impl

////////////////////////////////////////////////////////////////////////////////
//
// class template cometw::window_wrapper
//
////////////////////////////////////////////////////////////////////////////////

	template<class T>
	struct window_wrapper : public T
	{
		window_wrapper() throw()
		{}

		window_wrapper(HWND hwnd) throw()
		{	hwnd_ = hwnd;	}

		window_wrapper(HWND hparent, int id) throw()
		{	
			hwnd_ = ::GetDlgItem(hparent, id);
			_ASSERT(hwnd_);
		}

		window_wrapper& operator=(HWND hwnd) throw()
		{	
			hwnd_ = hwnd;	
			return *this; 
		}

		bool create(HWND hparent, const auto_rect& rect = NULL, LPCTSTR name = NULL, 
			DWORD style = T::default_style, DWORD exstyle = T::default_exstyle, 
			const auto_menu_id& menu = 0U, void* param = NULL
			)
		{
			T* pT = static_cast<T*>(this);
			const RECT& rc = *(rect ? static_cast<LPCRECT>(rect) : default_rect());
			pT->hwnd_ = ::CreateWindowEx(exstyle, T::window_class_name(), name, style, 
				rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hparent, menu,
				comet::module().instance(), param);
			return (pT->hwnd_ != NULL);
		}

		operator HWND() const throw()
		{	return hwnd_;	}
	};

////////////////////////////////////////////////////////////////////////////////
//
// class cometw::default_interface
//
// Implements default window interface
//
////////////////////////////////////////////////////////////////////////////////

	struct default_interface : public impl::window_base
	{
		enum { default_style = WS_CHILD|WS_VISIBLE, default_exstyle = 0 };

		static LPCTSTR window_class_name() throw()
		{	return NULL;	}
	};

	typedef window_wrapper<default_interface> window;
}

////////////////////////////////////////////////////////////////////////////////
// Change log:
// April 13, 2002: initially released by Vladimir Voinkov.
////////////////////////////////////////////////////////////////////////////////

#endif // COMETW_WINDOW_WRAP_H
