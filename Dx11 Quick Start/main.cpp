#include <Windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <d3dx10.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")

#define SCREEN_WIDTH  2560
#define SCREEN_HEIGHT 1440

IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer;    // the pointer to our back buffer
ID3D11InputLayout *pLayout;            // the pointer to the input layout
ID3D11VertexShader *pVS;               // the pointer to the vertex shader
ID3D11PixelShader *pPS;                // the pointer to the pixel shader
ID3D11Buffer *pVBuffer;                // the pointer to the vertex buffer

void InitD3D(HWND hWnd);															// Prepare D3D for use
void CleanD3D(void);																// Clear D3D once done
void RenderFrame(void);																// Render a single frame
void InitGraphics();																// Initialize graphics to render
void InitPipeline();																// Start pipeline to render

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); // WinProc for handling any event messages Windows sends to the program while running

struct VERTEX {
	FLOAT X, Y, Z;      // position
	D3DXCOLOR Color;    // color
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {

	HWND hWnd;                           // window handle
	WNDCLASSEX wc;                       // struct to hold window information

	ZeroMemory(&wc, sizeof(WNDCLASSEX)); // clear out the window for use

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpszClassName = L"MainWindow";
	// set background to null in order to skip flashing a window before going full screen
	// wc.hbrBackground = (HBRUSH) COLOR_WINDOW;

	RegisterClassEx(&wc);                // register window

	RECT wr = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};     // set the size, but not the position
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
		}

		RenderFrame();
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
	ID3D11Texture2D *pBackBuffer;
	D3D11_VIEWPORT viewport;

	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	scd.BufferCount = 1;									 // Number of back buffers
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;		 // use 32bit rgba
	scd.BufferDesc.Width = SCREEN_WIDTH;					 // set buffer width
	scd.BufferDesc.Height = SCREEN_HEIGHT;					 // set buffer height
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;		 // define how to use the buffer
	scd.OutputWindow = hWnd;								 // the window to render unto
	scd.SampleDesc.Count = 4;								 // multi sampling count
	scd.Windowed = FALSE;									 // windowed/not
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;      // allow fullscreen

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

	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*) &pBackBuffer);		   // find the backbuffer#0 and create backbuffer pointer for 2D Texture COM  // determine address of backbuffer
	dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);					   // create render target for Texture COM									  // create COM object to represent render target

	pBackBuffer->Release();															   // close Texture COM since render target for/with the texture is created and this is no longer needed

	devcon->OMSetRenderTargets(1, &backbuffer, NULL);								   // set Texture COM's output target to backbuffer so it can render to it 	  // set the COM object as the active render target

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = SCREEN_WIDTH;
	viewport.Height = SCREEN_HEIGHT;

	devcon->RSSetViewports(1, &viewport);

	InitPipeline();
	InitGraphics();
}

void RenderFrame(void) {
	devcon->ClearRenderTargetView(backbuffer, D3DXCOLOR(0.0f, 0.2f, 0.4f, 1.0f)); // flush the screen with deep blue color;

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;

	devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);                // select the vertex buffer to render
	devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);		  // select the render strategy
	devcon->Draw(3, 0);															  // render	to backbuffer; draw 3 vertices starting from 0

	swapchain->Present(0, 0);													  // swap front and back buffers
}

void InitPipeline() {
	ID3D10Blob *VS, *PS;                                                          // load and compile the vertex and pixel shaders
	D3D11_INPUT_ELEMENT_DESC ied[] = {                                            // create input layout object to tell the hardware how to read VERTEX struct
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, 0, 0);
	D3DX11CompileFromFile(L"shaders.shader", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, 0, 0);

	// encapsulate both shaders into shader objects
	dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
	dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

	// set the shader objects
	devcon->VSSetShader(pVS, 0, 0);
	devcon->PSSetShader(pPS, 0, 0);

	dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
	devcon->IASetInputLayout(pLayout);
}

void InitGraphics() {
	// array of vertices for a triange
	VERTEX TriangleVertices[] = {
		{0.0f, 0.5f, 0.0f, D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f)},
		{0.45f, -0.5, 0.0f, D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f)},
		{-0.45f, -0.5f, 0.0f, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f)}
	};
	D3D11_BUFFER_DESC bd;
	D3D11_MAPPED_SUBRESOURCE ms;

	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DYNAMIC;                // write only for CPU and read only for GPU
	bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write into buffer

	dev->CreateBuffer(&bd, NULL, &pVBuffer);                           // create the buffer
	devcon->Map(pVBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);   // map the buffer
	memcpy(ms.pData, TriangleVertices, sizeof(TriangleVertices));      // copy the data	to vertex buffer
	devcon->Unmap(pVBuffer, NULL);                                     // unmap the buffer
}

void CleanD3D(void) {
	swapchain->SetFullscreenState(FALSE, NULL);                                   // switch to windowed mode before closing

	pLayout->Release();
	pVS->Release();
	pPS->Release();
	pVBuffer->Release();
	swapchain->Release();
	backbuffer->Release();
	dev->Release();
	devcon->Release();
}
