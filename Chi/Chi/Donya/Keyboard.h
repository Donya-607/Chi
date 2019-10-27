#ifndef INCLUDED_DONYA_KEYBOARD_H_
#define INCLUDED_DONYA_KEYBOARD_H_

#define NOMINMAX
#include <Windows.h>	// For virtual key codes.

namespace Donya
{
	/// <summary>
	/// Using GetKeyboardState().
	/// </summary>
	namespace Keyboard
	{
		/// <summary>
		/// Please call every frame.
		/// </summary>
		void Update();

		enum Mode
		{
			PRESS = 0,
			RELEASE,
			REPEAT,
			TRIGGER
		};

		// int State( int virtualKeycode, Mode inputMode = TRIGGER );

		/// <summary>
		/// Returns pressing frame.
		/// </summary>
		int Press( int virtualKeycode );
		/// <summary>
		/// Returns TRUE(1) if just frame of release the key. else FALSE(0).
		/// </summary>
		int Release( int virtualKeycode );
		/// <summary>
		/// Returns TRUE(1) per "repeatIntervalFrame" and current pressing frame greater than "lowestNeedFrame". else FALSE(0).<para></para>
		/// If current pressing frame is equal to "lowestNeedFrame", return TRUE(1).
		/// </summary>
		int Repeat( int virtualKeyCode, int repeatIntervalFrame, int lowestNeedFrame = 0 );
		/// <summary>
		/// Returns TRUE(1) if current pressing frame is 1. else FALSE(0).
		/// </summary>
		int Trigger( int virtualKeycode );

		/// <summary>
		/// State() by focus of VK_LSHIFT, VK_RSHIFT.
		/// </summary>
		int Shifts( Mode inputMode = PRESS );
	}

}

#endif // INCLUDED_DONYA_KEYBOARD_H_
