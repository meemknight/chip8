#include <Windows.h>
#include "executeEmulation.h"
#include "cpu.h"

void win32redrawScreen(const char *screen, HWND wind);

static const uint8_t sprites[] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0x70, // 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
	0x90, 0x90, 0xF0, 0x10, 0x10, // 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
	0xF0, 0x10, 0x20, 0x40, 0x40, // 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
	0xF0, 0x90, 0xF0, 0x90, 0x90, // A
	0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
	0xF0, 0x80, 0x80, 0x80, 0xF0, // C
	0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
	0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};


static int running = 1;
static const int pixelSize = 12;

LRESULT CALLBACK winProc(HWND wind, UINT msg, WPARAM wp, LPARAM lp)
{
	LRESULT rez = 0;

	switch (msg)
	{
	case WM_CLOSE:
		running = 0;
		CloseWindow(wind);
		break;

	default:
		rez = DefWindowProc(wind, msg, wp, lp);
		break;
	}

	return rez;
}

void win32execute(const char * p, long size)
{

	BITMAPINFO bitmap;
	bitmap.bmiHeader.biSize = sizeof(bitmap.bmiHeader);
	bitmap.bmiHeader.biWidth = 64;
	bitmap.bmiHeader.biHeight = -32;
	bitmap.bmiHeader.biPlanes = 1;
	bitmap.bmiHeader.biBitCount = 32;
	bitmap.bmiHeader.biCompression = BI_RGB;
	bitmap.bmiHeader.biSizeImage = 0;
	bitmap.bmiHeader.biXPelsPerMeter = 0;
	bitmap.bmiHeader.biYPelsPerMeter = 0;
	bitmap.bmiHeader.biClrUsed = 0;
	bitmap.bmiHeader.biClrImportant = 0;

	unsigned char screenBuf[32 * 64 * 4];


	HINSTANCE h = (HINSTANCE)GetModuleHandle(0);

	WNDCLASS wc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = 0;
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hIcon = 0;
	wc.hInstance = h;
	wc.lpfnWndProc = winProc;
	wc.lpszClassName = "windowClass";
	wc.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	wc.lpszMenuName = 0;

	RegisterClass(&wc);

	HWND wind = CreateWindow
	(
		wc.lpszClassName,
		"CHIP-8",
		(WS_OVERLAPPEDWINDOW | WS_VISIBLE) ^ WS_SIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		pixelSize * 64,
		pixelSize * 32 + 38,
		0,
		0,
		h,
		0
	);

	srand(time(0));
	//resetting the state

	//reserved from 0 to 0x1ff
	uint8_t memory[4096];
	memset(memory, 0, sizeof(memory));

	//adding characters sprites
	memcpy(memory, sprites, sizeof(sprites));

	//copying the roam
	memcpy(&memory[512], p, size);

	regs_t regs;
	memset(&regs, 0, sizeof(regs));
	regs.pc = 512;

	uint16_t stack[STACK_SIZE];
	memset(stack, 0, sizeof(stack));

	char screen[32 * 64];
	for (int i = 0; i < 32 * 64; i++)
	{
		screen[i] = ' ';
	}

	///milliseconds
	int deltaTime = 0;
	int time1 = clock(0);
	int time2 = clock(0);
	int ctime = 16;

	const int frameDuration = (1.f / PROCESSOR_CLOCK_SPEED) * 1000;

	while (running)
	{
		MSG msg;
		if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		time2 = clock(0);
		deltaTime += time2 - time1;
		time1 = clock(0);

		if (deltaTime > frameDuration)
		{

			//cpu logic

			executeInstruction(memory, &regs, stack, screen);

			if (regs.shouldRedraw)
			{
				win32redrawScreen(screen, wind, bitmap, screenBuf);
				regs.shouldRedraw = 0;
			}

			//todo implement sound, properly implement duration
			ctime -= deltaTime;

			if (ctime <= 0)
			{
				ctime = 16 + ctime;
				if (regs.dt != 0)
				{
					regs.dt--;
					if (regs.dt == 0)
					{
						Beep(640, 350);
					}
				}

				if (regs.st != 0)
				{
					regs.st--;
				}
			}


			deltaTime = 0;
		}

	}

	CloseWindow(wind);

}

void win32redrawScreen(char *screen, HWND wind, BITMAPINFO info, unsigned char* bitMapMemory)
{
	HDC hdc = GetDC(wind);

	//for(int x=0; x<64 * pixelSize;x++)
	//{
	//	for(int y=0; y<32 * pixelSize; y++)
	//	{
	//	
	//		SetPixel(hdc, x, y, screen[x / pixelSize + (y / pixelSize) * 64] == ' ' ? RGB(20,20,20) : RGB(20,100,40));
	//	
	//	}
	//}

	for(int x=0; x<64;x++)
	{
		for(int y=0; y<32; y++)
		{
		
			if(screen[x + y * 64] == ' ')
			{
				bitMapMemory[(x + y * 64) * 4] = 20;
				bitMapMemory[(x + y * 64) * 4 + 1] = 20;
				bitMapMemory[(x + y * 64) * 4 + 2] = 20;
				bitMapMemory[(x + y * 64) * 4 + 3] = 0;
			}else
			{
				bitMapMemory[(x + y * 64) * 4] = 40;
				bitMapMemory[(x + y * 64) * 4 + 1] = 100;
				bitMapMemory[(x + y * 64) * 4 + 2] = 10;
				bitMapMemory[(x + y * 64) * 4 + 3] = 0;
			}
		
		}
	}

	StretchDIBits
	(
		hdc,
		0, 0, 64 * pixelSize - pixelSize, 32 * pixelSize,
		0, 0, 64, 32,
		bitMapMemory,
		&info,
		DIB_RGB_COLORS,
		SRCCOPY
	);

	ReleaseDC(hdc, wind);
}