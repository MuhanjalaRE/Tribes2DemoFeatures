#define USE_IMGUI

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
#include "t2/game data/demo.h"
#include <imgui/imgui.h>
//#include <imgui/imgui_impl_glut.h>
#ifdef USE_IMGUI
#include <imgui/imgui_impl_opengl2.h>
#include <imgui/imgui_impl_win32.h>
#endif

#ifdef _DEBUG
#include <iostream>
#endif

#ifdef USE_IMGUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
WNDPROC original_windowproc_callback = NULL;
LRESULT WINAPI CustomWindowProcCallback(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
typedef LRESULT(__stdcall* SetWindowLongPtr_)(HWND, int, long);
SetWindowLongPtr_ OriginalSetWindowLongPtr = NULL;
typedef BOOL(__stdcall* wglSwapBuffers)(int*);
wglSwapBuffers OriginalwglSwapBuffers = NULL;
bool show_imgui_demo_window = false;

#ifdef USE_IMGUI
BOOL __stdcall wglSwapBuffersHook(int* arg1) {
	//PLOG_DEBUG << "HDC = " << (unsigned int)arg1;
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (show_imgui_demo_window)
		ImGui::ShowDemoWindow(&show_imgui_demo_window);

	DWORD dwWaitResult = WaitForSingleObject(t2::hooks::opengl::game_mutex, INFINITE);

	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ 1920, 1080 });
	ImGui::Begin("window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus);

	ImDrawList* imgui_draw_list = ImGui::GetWindowDrawList();
	//opengl projects from bottom left of screen
	for (int i = 0; i < t2::hooks::opengl::projection_buffer.size(); i++) {
		imgui_draw_list->AddCircleFilled({ t2::hooks::opengl::projection_buffer[i].x, (float)t2::hooks::opengl::projection_buffer[i].y }, 6, ImColor(255, 255, 0, 255), 0);
	}

	ImGui::End();

	t2::hooks::opengl::projection_buffer.clear();

	ReleaseMutex(t2::hooks::opengl::game_mutex);


	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
	return OriginalwglSwapBuffers(arg1);
}
#endif

LRESULT __stdcall SetWindowLongPtrHook(HWND hWnd, int arg1, long arg2) {
	LRESULT res;
	//res = OriginalSetWindowLongPtr(hWnd, arg1, arg2);

#ifdef USE_IMGUI
	if (arg1 == GWL_WNDPROC) {
		ImGui_ImplWin32_Shutdown();
		ImGui_ImplWin32_Init(hWnd);
	}
#endif

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


	HMODULE hModule = GetModuleHandle(L"User32.dll");
	if (hModule) {
		unsigned int setwindowlongptr_address = (unsigned int)GetProcAddress(hModule, "SetWindowLongW");
		OriginalSetWindowLongPtr = (SetWindowLongPtr_)setwindowlongptr_address;
	}

#ifdef USE_IMGUI
	hModule = GetModuleHandle(L"opengl32.dll");
	if (hModule) {
		unsigned int wglswapbuffers_address = (unsigned int)GetProcAddress(hModule, "wglSwapBuffers");
		OriginalwglSwapBuffers = (wglSwapBuffers)wglswapbuffers_address;
	}
#endif

	hModule = GetModuleHandle(L"glu32.dll");
	if (hModule) {
		unsigned int gluproject_address = (unsigned int)GetProcAddress(hModule, "gluProject");
		t2::hooks::opengl::OriginalGluProject = (t2::hooks::opengl::GluProject)gluproject_address;
	}

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());

#ifdef _DEBUG
	DetourAttach(&(PVOID&)t2::hooks::con::OriginalPrintf, t2::hooks::con::PrintfHook);
#endif
	DetourAttach(&(PVOID&)t2::hooks::fps::OriginalFpsUpdate, t2::hooks::fps::FpsUpdateHook);
	//DetourAttach(&(PVOID&)t2::hooks::ShapeBase::OriginalGetEyeTransform, t2::hooks::ShapeBase::GetEyeTransformHook);
	// DetourAttach(&(PVOID&)t2::abstraction::hooks::GameConnection::OriginalSetControlObject, t2::abstraction::hooks::GameConnection::SetControlObjectHook);
	// This will probably get called in ReadPacket when a camera is the control object. Hook this and disable it so we don't have any damage flashes?
	DetourAttach(&(PVOID&)t2::abstraction::hooks::Camera::OriginalProcessTick, t2::abstraction::hooks::Camera::ProcessTickHook); //Hook this to disable any calls that update/lock the camera position

	DetourAttach(&(PVOID&)t2::abstraction::hooks::GameConnection::OriginalReadPacket, t2::abstraction::hooks::GameConnection::ReadPacketHook);
	//DetourAttach(&(PVOID&)t2::hooks::game::OriginalSetCameraFOV, t2::hooks::game::SetCameraFOVHook);


	DetourAttach(&(PVOID&)t2::abstraction::hooks::GameConnection::OriginalDemoPlayBackComplete, t2::abstraction::hooks::GameConnection::DemoPlayBackCompleteHook);
	/*
	DetourAttach(&(PVOID&)t2::abstraction::hooks::NetConnection::OriginalStartDemoRecord, t2::abstraction::hooks::NetConnection::StartDemoRecordHook);
	DetourAttach(&(PVOID&)t2::abstraction::hooks::NetConnection::OriginalStopDemoRecord, t2::abstraction::hooks::NetConnection::StopDemoRecordHook);
	*/

	//DetourAttach(&(PVOID&)t2::hooks::game::OriginalGameProcessCameraQuery, t2::hooks::game::GameProcessCameraQueryHook);
	DetourAttach(&(PVOID&)t2::abstraction::hooks::GameConnection::OriginalGetControlCameraTransform, t2::abstraction::hooks::GameConnection::GetControlCameraTransformHook);

	DetourAttach(&(PVOID&)t2::abstraction::hooks::Camera::OriginalSetPosition, t2::abstraction::hooks::Camera::SetPositionHook);

	//DetourAttach(&(PVOID&)t2::abstraction::hooks::Player::OriginalPlayerSetRenderPosition, t2::abstraction::hooks::Player::SetRenderPositionHook);

	DetourAttach(&(PVOID&)t2::hooks::platform::OriginalSetWindowLocked, t2::hooks::platform::SetWindowLockedHook);

	if (OriginalSetWindowLongPtr)
		DetourAttach(&(PVOID&)OriginalSetWindowLongPtr, SetWindowLongPtrHook);

#ifdef USE_IMGUI
	if (OriginalwglSwapBuffers) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hWnd);
		ImGui_ImplOpenGL2_Init();
		DetourAttach(&(PVOID&)OriginalwglSwapBuffers, wglSwapBuffersHook);
	}
#endif

	if (t2::hooks::opengl::OriginalGluProject)
		DetourAttach(&(PVOID&)t2::hooks::opengl::OriginalGluProject, t2::hooks::opengl::GluProjectHook);

	DetourTransactionCommit();

	t2::settings::LoadSettings();

	/*
	float f = *((float*)(0x00467E95 + 3));
	DWORD protection;
	VirtualProtect((void*)(0x00467E95 +3), sizeof(float), PAGE_EXECUTE_READWRITE, &protection);

	*((float*)(0x00467E95 + 3)) = PI;
	VirtualProtect((void*)(0x00467E95 + 3), sizeof(float), protection, &protection);

	*((float*)(0x0075C838)) = 2100*100;
	*/
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
	PLOG_DEBUG << "HWND = " << (unsigned int)hWnd;

	bool* window_locked = (bool*)0x0083BFE5;

	if (msg == WM_KEYDOWN) {
		if (wParam == VK_INSERT) {
			t2::settings::LoadSettings();
			show_imgui_demo_window = !show_imgui_demo_window;
		}
		if (wParam == 0x43){
			//t2::settings::set_camera = !t2::settings::set_camera;
			t2::game_data::demo::ToggleViewTarget();
		}
		if (wParam == VK_F3) {
			t2::game_data::demo::ToggleRecording();
		}
	}
	
	if (*window_locked && !show_imgui_demo_window){
		if (msg == WM_KEYDOWN) {
			keys::key_states[wParam] = true;
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

			/*
			RECT r;
			GetWindowRect(hWnd, &r);

			PLOG_DEBUG << "x = " << ((r.right + r.left) >> 1) << "\ty = " << ((r.bottom + r.top) >> 1);
			*/

		

			static int camera_mouse_x = 0;
			static int camera_mouse_y = 0;
			static int mouse_x_previous = 0;
			static int mouse_y_previous = 0;
			static int counter = 0;

			int mouse_x = LOWORD(lParam);
			int mouse_y = HIWORD(lParam);

			//PLOG_DEBUG << "x = " << mouse_x << "\ty = " << mouse_y;

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
	}

#ifdef USE_IMGUI
	ImGuiIO& io = ImGui::GetIO();
	if (show_imgui_demo_window) {
		io.MouseDrawCursor = true;
		if (t2::game_data::demo::game_connection && *window_locked) {
			t2::hooks::platform::OriginalSetWindowLocked(false);
			*window_locked = false;
		}
		//if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		//	return true;
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return true;
	}
	else {
		io.MouseDrawCursor = false;
		if (t2::game_data::demo::game_connection && *window_locked == false) {
			t2::hooks::platform::OriginalSetWindowLocked(true);
			*window_locked = true;
		}
	}
#endif
	return CallWindowProc(original_windowproc_callback, hWnd, msg, wParam, lParam);
}