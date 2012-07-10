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

#ifndef COMETW_ROOT_THUNK_H
#	define COMETW_ROOT_THUNK_H

#	pragma warning(disable : 4311)

namespace cometw {

	namespace details {

#ifdef _M_IX86
#	pragma pack(push, 1)

		struct thunk_aux
		{
			DWORD m_mov;		// mov dword ptr [esp+0x4], pThis (esp+0x4 is hWnd)
			DWORD m_this;		//
			BYTE m_jmp;			// jmp WndProc
			DWORD m_relproc;	// relative jmp
		};

#	pragma pack(pop)
#elif defined _M_ALPHA

	// For ALPHA we will stick the this pointer into a0, which is where
	// the HWND is.  However, we don't actually need the HWND so this is OK.
#	pragma pack(push,4)
		
		struct thunk_aux	//this should come out to 20 bytes
		{
			DWORD ldah_at;	//  ldah    at, HIWORD(func)
			DWORD ldah_a0;	//  ldah    a0, HIWORD(this)
			DWORD lda_at;	//  lda     at, LOWORD(func)(at)
			DWORD lda_a0;	//  lda     a0, LOWORD(this)(a0)
			DWORD jmp;		//  jmp     zero,(at),0
		};

#	pragma pack(pop)
#else	// _M_IX86
#	error Only Alpha and X86 supported
#endif	// _M_IX86

		// window_thunk

		class window_thunk
		{
		public:
			union
			{
				void* pointer_;
				thunk_aux thunk_;
			};

			void init(WNDPROC proc, void* pointer)
			{
#ifdef _M_IX86
				thunk_.m_mov = 0x042444C7;  //C7 44 24 0C
				thunk_.m_this = reinterpret_cast<DWORD>(pointer);
				thunk_.m_jmp = 0xe9;
				thunk_.m_relproc = reinterpret_cast<int>(proc) - (reinterpret_cast<int>(this) + sizeof(thunk_aux));
#elif defined _M_ALPHA
				thunk.ldah_at = (0x279f0000 | HIWORD(proc)) + (LOWORD(proc)>>15);
				thunk.ldah_a0 = (0x261f0000 | HIWORD(pointer)) + (LOWORD(pointer)>>15);
				thunk.lda_at = 0x239c0000 | LOWORD(proc);
				thunk.lda_a0 = 0x22100000 | LOWORD(pointer);
				thunk.jmp = 0x6bfc0000;
#endif
				// write block from data cache and flush from instruction cache
				::FlushInstructionCache(GetCurrentProcess(), &thunk_, sizeof(thunk_));
			}
		};

		// window_pointer_thunk

		struct window_pointer_thunk
		{
		public:
			template<typename T> 
			static WNDPROC initialize(T* pT)
			{
				pT->thunk_.init(T::window_proc, pT);
				return (WNDPROC)&(pT->thunk_.thunk_);
			}

			static void* hwnd2pointer(HWND hwnd)
			{	
				return hwnd;
			}

			template<typename T> 
			static WNDPROC procedure(T* pT)
			{
				return (WNDPROC)&(pT->thunk_.thunk_);
			}
			window_thunk thunk_;
		};
		
		typedef window_pointer_thunk window_pointer;

	} // details

} // cometw

////////////////////////////////////////////////////////////////////////////////
// Change log:
// April 13, 2002: initially released by Vladimir Voinkov.
////////////////////////////////////////////////////////////////////////////////

#endif // COMETW_ROOT_THUNK_H
