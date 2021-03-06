// include the basic windows header files and the Direct3D header files
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>

#include <iostream>
#include <fstream>
#include "resource.h"
using namespace std;

// include the Direct3D Library file
#pragma comment (lib, "d3d11.lib")

// define the screen resolution
#define SCREEN_WIDTH  1440
#define SCREEN_HEIGHT 1080

// global declarations
static bool g_running = TRUE;
char *shader_resource = 0;
size_t shader_res_sz  = 0;
IDXGISwapChain *swapchain;             // the pointer to the swap chain interface
ID3D11Device *dev;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext *devcon;           // the pointer to our Direct3D device context
ID3D11RenderTargetView *backbuffer;    // the pointer to our back buffer
ID3D11InputLayout *pLayout;            // the pointer to the input layout
ID3D11VertexShader *pVS;               // the pointer to the vertex shader
ID3D11PixelShader *pPS;                // the pointer to the pixel shader
ID3D11Buffer *pVBuffer;                // the pointer to the vertex buffer

// a struct to define a single vertex
struct VERTEX{FLOAT X, Y, Z; float Color[4];};

// function prototypes
void InitD3D(HWND hWnd);    // sets up and initializes Direct3D
void RenderFrame(void);     // renders a single frame
void CleanD3D(void);        // closes Direct3D and releases memory
void InitGraphics(void);    // creates the shape to render
void InitPipeline(void);    // loads and prepares the shaders
void LoadShaderResource();
// the WindowProc function prototype
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
  HWND hWnd;
  WNDCLASSEX wc;

  ZeroMemory(&wc, sizeof(WNDCLASSEX));
  wc.cbSize        = sizeof(WNDCLASSEX);
  wc.style         = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = WindowProc;
  wc.hInstance     = hInstance;
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wc.lpszClassName = "WindowClass";

  RegisterClassEx(&wc);

  RECT wr = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

  hWnd = CreateWindowEx(0,
                        "WindowClass",
                        "DX11 Test Window",
                        WS_OVERLAPPEDWINDOW,
                        300,
                        300,
                        wr.right - wr.left,
                        wr.bottom - wr.top,
                        NULL,
                        NULL,
                        hInstance,
                        NULL);

  ShowWindow(hWnd, nCmdShow);

  // set up and initialize Direct3D
  InitD3D(hWnd);


  // Load the Overlay in the same fashion the Injector would
  LoadLibraryA("overlay_build/overlay.dll");

  MSG msg;

  while(g_running)
  {
    if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);

      if(msg.message == WM_QUIT)
        break;
    }

    RenderFrame();
  }

  // clean up DirectX and COM
  CleanD3D();
  delete[] shader_resource;

  return msg.wParam;
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message)
  {
    case WM_CLOSE:
      g_running = FALSE;
      DestroyWindow(hWnd);
      return 0;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    case WM_SIZING:
      if (swapchain)
      {
        RECT *rec = (RECT*)lParam;
        devcon->OMSetRenderTargets(0, 0, 0);

        // Release all outstanding references to the swap chain's buffers.
        backbuffer->Release();


        // Preserve the existing buffer count and format.
        // Automatically choose the width and height to match the client rect for HWNDs.
        swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

        // Perform error handling here!

        // Get buffer and create a render-target-view.
        ID3D11Texture2D *pBuffer;
        swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **) &pBuffer);
        // Perform error handling here!

        dev->CreateRenderTargetView(pBuffer, NULL, &backbuffer);
        // Perform error handling here!
        pBuffer->Release();

        devcon->OMSetRenderTargets(1, &backbuffer, NULL);

        // Set up the viewport.
        D3D11_VIEWPORT vp;
        vp.Width    = (float)(rec->right - rec->left);
        vp.Height   = (float)(rec->bottom - rec->top);
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        devcon->RSSetViewports(1, &vp);
      }
      break;
    default:
    break;
  }

  return DefWindowProc (hWnd, message, wParam, lParam);
}


// this function initializes and prepares Direct3D for use
void InitD3D(HWND hWnd)
{
  // create a struct to hold information about the swap chain
  DXGI_SWAP_CHAIN_DESC scd;

  // clear out the struct for use
  ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

  // fill the swap chain description struct
  scd.BufferCount       = 1;                                // one back buffer
  scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;       // use 32-bit color
  scd.BufferDesc.Width  = SCREEN_WIDTH;                     // set the back buffer width
  scd.BufferDesc.Height = SCREEN_HEIGHT;                    // set the back buffer height
  scd.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;  // how swap chain is to be used
  scd.OutputWindow      = hWnd;                             // the window to be used
  scd.SampleDesc.Count  = 4;                                // how many multisamples
  scd.Windowed          = TRUE;                             // windowed/full-screen mode
  scd.Flags             = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;    // allow full-screen switching

  // create a device, device context and swap chain using the information in the scd struct
  D3D11CreateDeviceAndSwapChain(NULL,
                                D3D_DRIVER_TYPE_HARDWARE,
                                NULL,
                                0,
                                NULL,
                                0,
                                D3D11_SDK_VERSION,
                                &scd,
                                &swapchain,
                                &dev,
                                NULL,
                                &devcon);


  // get the address of the back buffer
  ID3D11Texture2D *pBackBuffer;
  swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

  // use the back buffer address to create the render target
  dev->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
  pBackBuffer->Release();

  // set the render target as the back buffer
  devcon->OMSetRenderTargets(1, &backbuffer, NULL);


  // Set the viewport
  D3D11_VIEWPORT viewport;
  ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

  viewport.TopLeftX = 0;
  viewport.TopLeftY = 0;
  viewport.Width    = SCREEN_WIDTH;
  viewport.Height   = SCREEN_HEIGHT;

  devcon->RSSetViewports(1, &viewport);

  InitPipeline();
  InitGraphics();
}


// this is the function used to render a single frame
void RenderFrame(void)
{
  float color[4] = {0.0f, 0.2f, 0.4f, 1.0f};
  // clear the back buffer to a deep blue
  devcon->ClearRenderTargetView(backbuffer, color);

  // select which vertex buffer to display
  UINT stride = sizeof(VERTEX);
  UINT offset = 0;
  devcon->IASetVertexBuffers(0, 1, &pVBuffer, &stride, &offset);

  // select which primtive type we are using
  devcon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

  // draw the vertex buffer to the back buffer
  devcon->Draw(3, 0);

  // switch the back buffer and the front buffer
  swapchain->Present(0, 0);
}


// this is the function that cleans up Direct3D and COM
void CleanD3D(void)
{
  swapchain->SetFullscreenState(FALSE, NULL);    // switch to windowed mode

  // close and release all existing COM objects
  pLayout->Release();
  pVS->Release();
  pPS->Release();
  pVBuffer->Release();
  swapchain->Release();
  backbuffer->Release();
  dev->Release();
  devcon->Release();
}


// this is the function that creates the shape to render
void InitGraphics()
{
  // create a triangle using the VERTEX struct
  VERTEX OurVertices[] =
             {
                 {0.0f, 0.5f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f}},
                 {0.45f, -0.5, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f}},
                 {-0.45f, -0.5f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f}}
             };


  // create the vertex buffer
  D3D11_BUFFER_DESC bd;
  ZeroMemory(&bd, sizeof(bd));

  bd.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
  bd.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

  dev->CreateBuffer(&bd, NULL, &pVBuffer);       // create the buffer


  // copy the vertices into the buffer
  D3D11_MAPPED_SUBRESOURCE ms;
  devcon->Map(pVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);    // map the buffer
  memcpy(ms.pData, OurVertices, sizeof(OurVertices));                 // copy the data
  devcon->Unmap(pVBuffer, 0);                                      // unmap the buffer
}

void WINAPI D3DCompileFromFile(
    const D3D_SHADER_MACRO *defines,
    ID3DInclude *include,
    const char *entrypoint,
    const char *target,
    UINT sflags,
    UINT eflags,
    ID3DBlob **shader,
    ID3DBlob **error_messages
    )
    {

  auto hr = D3DCompile(
      shader_resource,
      shader_res_sz,
      "shaders.hlsl",
      defines,
      include,
      entrypoint,
      target,
      sflags,
      eflags,
      shader,
      error_messages
      );

  if(FAILED(hr))
  {
    throw "could not compile shader";
  }

}
//// this function loads and prepares the shaders
void InitPipeline()
{
  // load and compile the two shaders
  ID3D10Blob *VS, *PS;

  LoadShaderResource();
  D3DCompileFromFile(0, 0, "VShader", "vs_4_0", 0, 0, &VS, 0);
  D3DCompileFromFile(0, 0, "PShader", "ps_4_0", 0, 0, &PS, 0);

  // encapsulate both shaders into shader objects
  dev->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &pVS);
  dev->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &pPS);

  // set the shader objects
  devcon->VSSetShader(pVS, 0, 0);
  devcon->PSSetShader(pPS, 0, 0);

  // create the input layout object
  D3D11_INPUT_ELEMENT_DESC ied[] =
                               {
                                   {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
                                   {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
                               };

  dev->CreateInputLayout(ied, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &pLayout);
  devcon->IASetInputLayout(pLayout);
}

void LoadFileInResource(int name, int type, OUT size_t &size, OUT const char*& data) // *& is passing the pointer by reference and not by val.
{
  HMODULE handle = ::GetModuleHandleW(NULL);
  HRSRC rc		   = ::FindResource(handle, MAKEINTRESOURCE(name), MAKEINTRESOURCE(type));
  HGLOBAL rcData = ::LoadResource(handle, rc);
  size			   = ::SizeofResource(handle, rc);
  data			   = static_cast<const char*>(::LockResource(rcData));
  //LockResource does not actually lock memory; it is just used to obtain a pointer to the memory containing the resource data.
}

void LoadShaderResource()
{
  const char* data = NULL;
  LoadFileInResource(IDR_SHADER, HLSL, shader_res_sz, data);
  shader_resource = new char[shader_res_sz + 1];
  memcpy(shader_resource, data, shader_res_sz);
  shader_resource[shader_res_sz] = 0; // NULL terminator
}