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

#ifdef _DEBUG
#include <iostream>
#endif

WNDPROC original_windowproc_callback = NULL;
LRESULT WINAPI CustomWindowProcCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void OnDLLProcessAttach(void) {

#ifdef _DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    static plog::ConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::verbose, &consoleAppender);
    PLOG_DEBUG << "DLL injected successfully. Hooking game functions.";
#endif

    HWND hwnd = FindWindowA(NULL, "Tribes 2");
    original_windowproc_callback = (WNDPROC)SetWindowLongPtr(hwnd, GWL_WNDPROC, (LONG_PTR)CustomWindowProcCallback);

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

#ifdef _DEBUG
    DetourAttach(&(PVOID&)t2::hooks::con::OriginalPrintf, t2::hooks::con::PrintfHook);
#endif
	DetourAttach(&(PVOID&)t2::hooks::fps::OriginalFpsUpdate, t2::hooks::fps::FpsUpdateHook);
    DetourAttach(&(PVOID&)t2::hooks::ShapeBase::OriginalGetEyeTransform, t2::hooks::ShapeBase::GetEyeTransformHook);
    DetourAttach(&(PVOID&)t2::abstraction::hooks::GameConnection::OriginalSetControlObject, t2::abstraction::hooks::GameConnection::SetControlObjectHook);
    DetourAttach(&(PVOID&)t2::abstraction::hooks::Camera::OriginalProcessTick, t2::abstraction::hooks::Camera::ProcessTickHook);
    DetourAttach(&(PVOID&)t2::abstraction::hooks::GameConnection::OriginalReadPacket, t2::abstraction::hooks::GameConnection::ReadPacketHook);
    DetourAttach(&(PVOID&)t2::hooks::game::OriginalSetCameraFOV, t2::hooks::game::SetCameraFOVHook);

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
	if (msg == WM_KEYDOWN) {
		keys::key_states[wParam] = true;
		if (wParam == VK_INSERT) {
			t2::settings::LoadSettings();
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