// Engine_3D.cpp : 定义控制台应用程序的入口点。
//

#include "./flatform/Drawable.h"

int main(int argc, char* argv[])
{
	EP_Init(200, 200);
	//EP_Init(-1, -1);

	Initialize();

	INT count = 0;
	char str[100];
	EP_RenderStart();
	do
	{
		onPaint(getHWnd());

		while (mousemsg())
		{
			mouse_msg msg = getmouse();
			if (msg.is_wheel()) {
				onScroll(msg.wheel);
			}
			if (msg.is_right())
			{
				if (msg.is_down())
				{
					onMenu(msg.x, msg.y, 1);
				}
				else if (msg.is_up())
				{
					onMenu(msg.x, msg.y, 0);
				}
			}
			else
			{
				if (msg.is_down())
				{
					onDrag(msg.x, msg.y, 1);
				}
				else if (msg.is_up())
				{
					onDrag(msg.x, msg.y, 0);
				}
			}
			if (msg.is_move())
			{
				onMenu(msg.x, msg.y, 2);
				onDrag(msg.x, msg.y, 2);
			}
		}
		if (kbmsg()) {
			key_msg msg = getkey();
			if (msg.msg == key_msg_up) {
				onKeyUp(msg.key);
			}
			else if (msg.msg == key_msg_down) {
				onKeyDown(msg.key);
				if (msg.key == VK_ESCAPE) {
					break;
				}
			}
		}
		flushkey();
		flushmouse();

		if (count++ > 2) {
			count = 0;
			onTimer();
		}
		//delay_ms(100);
		EP_RenderFlush();
		sprintf_s(str, "%f", getfps());
		::SetWindowText(getHWnd(), str);
	} while (1);
	onClose();
	closegraph();
}

