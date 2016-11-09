#include <Windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

IDXGISwapChain *swapchain;
ID3D11Device *dev;
ID3D11DeviceContext *devcon;

void InitD3D(HWND hWnd);															// Prepare D3D for use
void CleanD3D(void);																// Clear D3D once done
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); // WinProc for handling any event messages Windows sends to the program while running


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	HWND hWnd;                           // window handle
	WNDCLASSEX wc;                       // struct to hold window information

	ZeroMemory(&wc, sizeof(WNDCLASSEX)); // clear out the window for use

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH) COLOR_WINDOW;
	wc.lpszClassName = L"MainWindow";

	RegisterClassEx(&wc);                // register window

	RECT wr = {0, 0, 800, 600};          // set the size, but not the position
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE); // adjust the size

	hWnd = CreateWindowEx(               // Create window handle from the registered window
		NULL,
		L"MainWindow",                   // name of the window class
		L"Our First Windowed Program",   // title of the window
		WS_OVERLAPPEDWINDOW,             // window style
		300,                             // x-position of the window
		300,                             // y-position of the window
		wr.right - wr.left,              // width of the window
		wr.bottom - wr.left,             // height of the window
		NULL,                            // we have no parent window, NULL
		NULL,                            // we aren't using menus, NULL
		hInstance,                       // application handle
		NULL							 // used with multiple windows, NULL
	);

	ShowWindow(hWnd, nShowCmd);

	InitD3D(hWnd);

	MSG msg;

	while (TRUE) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) { // PM_REMOVE will pop message from quere while PM_REMOVE don't
			TranslateMessage(&msg);      // translate keystroke messages into the right format
			DispatchMessage(&msg);       // send the message to the WindowProc function

			if (msg.message == WM_QUIT)
				break;
		} else {
			// run dx code
		}
	}

	CleanD3D();

	return msg.wParam;                   // return this part of the WM_QUIT message to Windows
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_DESTROY:                 // this message is read when the window is closed
			PostQuitMessage(0);          // close the application entirely
			return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam); // Handle any messages the switch statement didn't
}

void InitD3D(HWND hWnd) {
	DXGI_SWAP_CHAIN_DESC scd;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	scd.BufferCount = 1;									 // Number of back buffers
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		 // use 32bit rgba
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		 // define how to use the buffer
	scd.OutputWindow = hWnd;								 // the window to render unto
	scd.SampleDesc.Count = 4;								 // multi sampling count
	scd.Windowed = true;									 // windowed/not

	D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scd,
		&swapchain,
		&dev,
		NULL,
		&devcon
	);
}

void CleanD3D(void) {
	swapchain->Release();
	dev->Release();
	devcon->Release();
}
