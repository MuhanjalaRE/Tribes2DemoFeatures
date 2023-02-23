#include <Windows.h>
#include "detours/detours.h"
#include <t2/hooks/global_hooks.h>
#include <t2/abstraction/Player.h>
#include <t2/abstraction/GameConnection.h>
#include <t2/abstraction/NetConnection.h>
#include <t2/abstraction/SceneObject.h>
#include <t2/abstraction/Camera.h>
#include <t2/abstraction/ShapeBase.h>
#include <keys/keys.h>
#include <t2/settings/settings.h>
#include <t2/game data/demo.h>

#ifdef _DEBUG
#include <iostream>
#endif

WNDPROC original_windowproc_callback = NULL;
LRESULT WINAPI CustomWindowProcCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef LRESULT(__stdcall* SetWindowLongPtr_)(HWND, int, long);
SetWindowLongPtr_ OriginalSetWindowLongPtr;

LRESULT __stdcall SetWindowLongPtrHook(HWND hWnd, int arg1, long arg2) {
	LRESULT res;
	//res = OriginalSetWindowLongPtr(hWnd, arg1, arg2);

	if (arg1 != GWL_WNDPROC) {
		return OriginalSetWindowLongPtr(hWnd, arg1, arg2);
	}
	else {
		original_windowproc_callback = (WNDPROC)OriginalSetWindowLongPtr(hWnd, arg1, arg2);
		OriginalSetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)CustomWindowProcCallback);
		return (LRESULT)original_windowproc_callback;


		/*
		//OriginalSetWindowLongPtr(hWnd, arg1, arg2);
		original_windowproc_callback = (WNDPROC)OriginalSetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)CustomWindowProcCallback);
		return (LRESULT)original_windowproc_callback;
		*/
	}
	//original_windowproc_callback = (WNDPROC)SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)CustomWindowProcCallback);
}

void OnDLLProcessAttach(void) {

#ifdef _DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::verbose, &consoleAppender);
    PLOG_DEBUG << "DLL injected successfully. Hooking game functions.";
#endif

    HWND hWnd = FindWindow(NULL, L"Tribes 2");
    original_windowproc_callback = (WNDPROC)SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)CustomWindowProcCallback);

	unsigned int setwindowlongptr_address = 0;
	HMODULE hModule = GetModuleHandle(L"User32.dll");
	if (hModule) {
		setwindowlongptr_address = (unsigned int)GetProcAddress(hModule, "SetWindowLongW");
		OriginalSetWindowLongPtr = (SetWindowLongPtr_)setwindowlongptr_address;
	}

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

#ifdef _DEBUG
    DetourAttach(&(PVOID&)t2::hooks::con::OriginalPrintf, t2::hooks::con::PrintfHook);
#endif
	DetourAttach(&(PVOID&)t2::hooks::fps::OriginalFpsUpdate, t2::hooks::fps::FpsUpdateHook);
    DetourAttach(&(PVOID&)t2::hooks::ShapeBase::OriginalGetEyeTransform, t2::hooks::ShapeBase::GetEyeTransformHook);
    DetourAttach(&(PVOID&)t2::abstraction::hooks::GameConnection::OriginalSetControlObject, t2::abstraction::hooks::GameConnection::SetControlObjectHook);
	// This will probably get called in ReadPacket when a camera is the control object. Hook this and disable it so we don't have any damage flashes?
    DetourAttach(&(PVOID&)t2::abstraction::hooks::Camera::OriginalProcessTick, t2::abstraction::hooks::Camera::ProcessTickHook);
    DetourAttach(&(PVOID&)t2::abstraction::hooks::GameConnection::OriginalReadPacket, t2::abstraction::hooks::GameConnection::ReadPacketHook);
	//DetourAttach(&(PVOID&)t2::hooks::game::OriginalSetCameraFOV, t2::hooks::game::SetCameraFOVHook);

	
	DetourAttach(&(PVOID&)t2::abstraction::hooks::GameConnection::OriginalDemoPlayBackComplete, t2::abstraction::hooks::GameConnection::DemoPlayBackCompleteHook);
	/*
	DetourAttach(&(PVOID&)t2::abstraction::hooks::NetConnection::OriginalStartDemoRecord, t2::abstraction::hooks::NetConnection::StartDemoRecordHook);
	DetourAttach(&(PVOID&)t2::abstraction::hooks::NetConnection::OriginalStopDemoRecord, t2::abstraction::hooks::NetConnection::StopDemoRecordHook);
	*/

	if (setwindowlongptr_address)
		DetourAttach(&(PVOID&)OriginalSetWindowLongPtr, SetWindowLongPtrHook);


    DetourTransactionCommit();

	t2::settings::LoadSettings();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)OnDLLProcessAttach, NULL, NULL, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

LRESULT WINAPI CustomWindowProcCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//PLOG_DEBUG << "CustomWindowProcCallback";
	if (msg == WM_KEYDOWN) {
		//PLOG_DEBUG << "Key down";
		keys::key_states[wParam] = true;
		if (wParam == VK_INSERT) {
			t2::settings::LoadSettings();
		}
		if (wParam == 0x43){
			//t2::settings::set_camera = !t2::settings::set_camera;
			t2::game_data::demo::ToggleViewTarget();
		}
		if (wParam == VK_F3) {
			t2::game_data::demo::ToggleRecording();
		}
	}
	else if (msg == WM_KEYUP) {
		keys::key_states[wParam] = false;
	}
	else if (msg == WM_LBUTTONDOWN) {
		keys::mouse_states[0] = true;
	}
	else if (msg == WM_LBUTTONUP) {
		keys::mouse_states[0] = false;
	}
	else if (msg == WM_RBUTTONDOWN) {
		keys::mouse_states[1] = true;
	}
	else if (msg == WM_RBUTTONUP) {
		keys::mouse_states[1] = false;
	}
	else if (msg == WM_MOUSEMOVE) {
		static int camera_mouse_x = 0;
		static int camera_mouse_y = 0;
		static int mouse_x_previous = 0;
		static int mouse_y_previous = 0;
		static int counter = 0;

		int mouse_x = LOWORD(lParam);
		int mouse_y = HIWORD(lParam);

		if (mouse_x == mouse_x_previous && mouse_y == mouse_y_previous) {
			counter++;
			if (counter >= 50) {
				camera_mouse_x = mouse_x;
				camera_mouse_y = mouse_y;
			}
		}
		else {
			counter = 0;
		}

		mouse_x_previous = mouse_x;
		mouse_y_previous = mouse_y;

		keys::mouse_states[2] = 0;
		keys::mouse_states[3] = 0;

		if (mouse_x > camera_mouse_x) {
			keys::mouse_states[2] = 1 * abs(camera_mouse_x - mouse_x);
		}
		else if (mouse_x < camera_mouse_x) {
			keys::mouse_states[2] = -1 * abs(camera_mouse_x - mouse_x);
		}

		if (mouse_y > camera_mouse_y) {
			keys::mouse_states[3] = 1 * abs(camera_mouse_y - mouse_y);
		}
		else if (mouse_y < camera_mouse_y) {
			keys::mouse_states[3] = -1 * abs(camera_mouse_y - mouse_y);
		}
	}

	return CallWindowProc(original_windowproc_callback, hWnd, msg, wParam, lParam);
}