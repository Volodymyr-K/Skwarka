// raytracer_app.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "TestTracer.h"
#include "raytracer_app.h"
#include "Console.h"
#include <Raytracer/Core/Renderer.h>
#include <Raytracer/Core/Film.h>
#include <Raytracer/Core/SpectrumRoutines.h>
#include <Raytracer/Core/Color.h>

static HINSTANCE	g_hInstance;
static HWND			g_hWnd;
static HDC			g_memDC;
static HBITMAP		g_bitmap;

static const TCHAR* szTitle = _T("Ray tracing test");
static const TCHAR* szWindowClass = _T("RayTracerWindowClass");

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

#define IMAGE_WIDTH 800
#define IMAGE_HEIGHT 600

TestTracer *tracer;
Console* g_console;

bool tracing_complete = false;

class MyDisplayUpdateCallback: public DisplayUpdateCallback
  {
  public:
    MyDisplayUpdateCallback(Byte* i_image): m_image(i_image) {}

    void Update(intrusive_ptr<const Film> ip_film)
      {
      const Film *p_film = ip_film.get();
      size_t height = p_film->GetYResolution(), width = p_film->GetXResolution();

      for(int y=0;y<(int)height;++y)
        for(int x=0;x<(int)width;++x)
          {
          Spectrum_d sp;
          p_film->GetPixel(Point2D_i(x,y),sp);
          RGBColor_d color = global_sRGB_D65_ColorSystem.XYZ_To_RGB(SpectrumRoutines::SpectrumToXYZ(sp),true);
          color.Clamp(0.0,256.0);
          color[0]/=256.0;color[1]/=256.0;color[2]/=256.0;
          color = global_sRGB_D65_ColorSystem.GammaEncode(color);
          int r = std::min(255,(int)(color[0]*256.0));
          int g = std::min(255,(int)(color[1]*256.0));
          int b = std::min(255,(int)(color[2]*256.0));

          unsigned int pixel_index = (unsigned int) ((y*width+x)*4);
          Byte* pixel = m_image;
          pixel[pixel_index+2] = Byte(r);
          pixel[pixel_index+1] = Byte(g);
          pixel[pixel_index+0] = Byte(b);
          }

      BitBlt(GetDC(g_hWnd), 0, 0, tracer->GetImageWidth(), tracer->GetImageHeight(), g_memDC, 0, 0, SRCCOPY);
      }

  private:
    Byte* m_image;
  };

MyDisplayUpdateCallback *p_update_callback;

void CreateRenderer() {
  tracer = new TestTracer(IMAGE_WIDTH, IMAGE_HEIGHT);
  tracer->LoadMesh();
  }

//---------------------------------------------------------------------
static bool CreateMainWindow(int nCmdShow) {
	// Register window class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInstance;
	wcex.hIcon			= LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_RAYTRACER_APP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_RAYTRACER_APP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	if (!RegisterClassEx(&wcex)) {
		return false;
	}
	// Create window
	DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
	RECT r;
	SetRect(&r, 0, 0, tracer->GetImageWidth()-1, tracer->GetImageHeight()-1);
	BOOL res = AdjustWindowRect(&r, style, TRUE);
	assert(res);
	const int w = r.right - r.left + 1;
	const int h = r.bottom - r.top + 1;
	g_hWnd = CreateWindow(szWindowClass, szTitle, style, CW_USEDEFAULT, 0, w, h, NULL, NULL, g_hInstance, NULL);
	if (!g_hWnd) {
		return false;
	}
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);
	return true;
}

//---------------------------------------------------------------------
static bool CreateMainImage() {
	g_memDC = CreateCompatibleDC(GetDC(g_hWnd));
	if (!g_memDC) {
		return false;
	}
	BITMAPINFO info;
	info.bmiHeader.biSize = sizeof(info.bmiHeader);
	info.bmiHeader.biWidth			= tracer->GetImageWidth();
	info.bmiHeader.biHeight			= -tracer->GetImageHeight();
	info.bmiHeader.biPlanes			= 1;
	info.bmiHeader.biBitCount		= 32;
	info.bmiHeader.biCompression	= BI_RGB;
	info.bmiHeader.biSizeImage		= 0;
	info.bmiHeader.biXPelsPerMeter	= 0;
	info.bmiHeader.biYPelsPerMeter	= 0;
	info.bmiHeader.biClrUsed		= 0;
	info.bmiHeader.biClrImportant	= 0;

	Byte* image;
	g_bitmap = CreateDIBSection(GetDC(g_hWnd), &info, DIB_RGB_COLORS, (void**)&image, NULL, 0);
	if (!g_bitmap) {
		return false;
	}
	HGDIOBJ res = SelectObject(g_memDC, g_bitmap);
	if (!res || res == HGDI_ERROR) {
		return false;
	}
	BYTE* ptr = (BYTE*)image;
	for (int i = 3; i < tracer->GetImageWidth()*tracer->GetImageHeight()*4; i += 4) {
		ptr[i] = 255;
	}

  p_update_callback = new MyDisplayUpdateCallback(image);
  tracer->SetDisplayUpdateCallback(p_update_callback);

	return true;
}

//---------------------------------------------------------------------
static double g_prevTime;
static double g_lastFPSUpdate;
static std::vector<double> g_fpsValues;

double GetTime() {
	__int64 count, freq;
	QueryPerformanceCounter((LARGE_INTEGER*)&count);
	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	return double(count) / freq;
}

//---------------------------------------------------------------------
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int nCmdShow)
{
  g_console = new Console();

	g_hInstance = hInstance;
  CreateRenderer();
	if (!CreateMainWindow(nCmdShow)) {
		return FALSE;
	}
	if (!CreateMainImage()) {
		return FALSE;
	}

  tracer->RenderImage();
  tracing_complete = true;

	MSG msg;
	while (true) {
		// Process messages
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) {
			if (GetMessage(&msg, NULL, 0, 0)) {
				DispatchMessage(&msg);
			}
			else {
				return 0;
			}
		}
	}
	DeleteDC(g_memDC);
	DeleteObject(g_bitmap);
	return (int) msg.wParam;
}

//---------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) {
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId) {
		case IDM_ABOUT:
			DialogBox(g_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...

    if (tracing_complete)
      BitBlt(GetDC(g_hWnd), 0, 0, tracer->GetImageWidth(), tracer->GetImageHeight(), g_memDC, 0, 0, SRCCOPY);

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

//---------------------------------------------------------------------
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
