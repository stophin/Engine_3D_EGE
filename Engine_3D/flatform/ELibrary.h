//ELibrary.h
//now we're using ege library tool
//you may change this
//once changed, associated files should be changed
//Author: Stophin
//2014.01.08
//Ver: 0.01
//
#ifndef _ELIBRARY_H_
#define _ELIBRARY_H_

#include <graphics.h>


struct EPointF {
	FLOAT X;
	FLOAT Y;
};

struct ERectF {
	FLOAT X;
	FLOAT Y;
	FLOAT Width;
	FLOAT Height;
};

#define EP_GetWnd getHWnd
#define EP_MouseHit mousemsg
#define EP_MouseMsg mouse_msg
#define EP_GetMouseMsg getmouse

#define EP_IsWheel(msg) msg.is_wheel()
#define EP_IsRight(msg) msg.is_right()
#define EP_IsLeft(msg) msg.is_left()
#define EP_IsDown(msg) msg.is_down()
#define EP_IsUp(msg) msg.is_up()
#define EP_IsMove(msg) msg.is_move()
#define EP_Wheel(msg) msg.wheel()
#define EP_X(msg) (msg.x)
#define EP_Y(msg) (msg.y)

#define EP_KBMsg kbmsg
#define EP_MSG key_msg
#define EP_GetKBMsg getkey
#define EP_KBIsUp(msg) (msg.msg == key_msg_up)
#define EP_KBIsDown(msg) (msg.msg == key_msg_down)
#define EP_GetKey(msg) (msg.key)
#define EP_Equal(msg, val) (msg.key == val)

#define EP_FlushKey flushkey
#define EP_FlushMouse flushmouse

#define EP_Delay(ms) delay_ms(ms)
#define EP_ClearDevice cleardevice

#endif	//end of _ELIBRARY_H_
//end of file
