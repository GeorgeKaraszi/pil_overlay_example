#include "pch.hpp"

DWORD WINAPI MainThread(HMODULE hModule)
{
  Overlay::D3DHook hook(hModule);
  hook.Start();
  hook.Run();
  hook.Shutdown();
  return TRUE;
}

BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID _reserved)
{
  switch(nReason)
  {
    case DLL_PROCESS_ATTACH:
      CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hDllHandle, 0, nullptr);
      break;
    case DLL_PROCESS_DETACH:
    default:
      break;
  }
  return TRUE;
}