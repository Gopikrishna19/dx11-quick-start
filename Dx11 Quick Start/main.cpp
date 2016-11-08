#include <Windows.h>
#include <windowsx.h>

//WinProc for handling any event messages Windows sends to the program while running
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


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