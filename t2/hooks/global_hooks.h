#pragma once

// #include <t2/abstraction/SimObject.h>
#include <Windows.h>
#include <imgui/imgui.h>
#include <t2/abstraction/SceneObject.h>
#include <t2/math.h>

#include <tuple>
#include <utility>

namespace t2 {
namespace hooks {

typedef BOOL(__stdcall* QueryPerformanceCounter_)(LARGE_INTEGER* lpPerformanceCount);
extern QueryPerformanceCounter_ OriginalQueryPerformanceCounter;

namespace con {
int __stdcall PrintfHook(char* format, ...);
typedef int(__stdcall* Printf)(char* format, ...);
extern Printf OriginalPrintf;

typedef const char*(__cdecl* Executef)(int argc, ...);
extern Executef OriginalExecutef;
}  // namespace con

namespace fps {
typedef void(__cdecl* FpsUpdate)(void);
extern FpsUpdate OriginalFpsUpdate;
void FpsUpdateHook(void);
}  // namespace fps

namespace game {
typedef void(__cdecl* SetCameraFOV)(float fov);
extern SetCameraFOV OriginalSetCameraFOV;
void __cdecl SetCameraFOVHook(float fov);

typedef bool(__cdecl* GameProcessCameraQuery)(void*);
extern GameProcessCameraQuery OriginalGameProcessCameraQuery;
bool __cdecl GameProcessCameraQueryHook(void*);
}  // namespace game

namespace platform {
typedef void(__cdecl* SetWindowLocked)(bool);
extern SetWindowLocked OriginalSetWindowLocked;
void __cdecl SetWindowLockedHook(bool locked);
}  // namespace platform

namespace opengl {
extern std::vector<ImVec2> projection_buffer;
extern HANDLE game_mutex;
typedef int(__stdcall* GluProject)(double objx, double objy, double objz, const double modelMatrix[16], const double projMatrix[16], const int viewport[4], double* winx, double* winy, double* winz);
extern GluProject OriginalGluProject;
int __stdcall GluProjectHook(double objx, double objy, double objz, const double modelMatrix[16], const double projMatrix[16], const int viewport[4], double* winx, double* winy, double* winz);
}  // namespace opengl

namespace guicanvas {
typedef void(__thiscall* RenderFrame)(void*, bool);
extern RenderFrame OriginalRenderFrame;
void __fastcall RenderFrameHook(void*, void*, bool);
}  // namespace guicanvas

namespace guicontrol {
typedef void(__thiscall* OnRender)(void*, void*, void*, void*);
extern OnRender OriginalOnRender;
void __fastcall OnRenderHook(void*, void*, void*, void*, void*);
extern OnRender OriginalOnRender2;
void __fastcall OnRenderHook2(void*, void*, void*, void*, void*);
typedef void(__thiscall* RenderChildControls)(void*, void*, void*, void*);
extern RenderChildControls OriginalRenderChildControls;
void __fastcall RenderChildControlsHook(void*, void*, void*, void*, void*);

typedef char*(__thiscall* sub_505740)(void*, void*, void*, void*);
extern sub_505740 Originalsub_505740;
char* __fastcall sub_505740Hook(void*, void*, void*, void*, void*);

typedef char(__thiscall* sub_506870)(void*, void*, void*, void*);
extern sub_506870 Originalsub_506870;
char __fastcall sub_506870Hook(void*, void*, void*, void*, void*);

typedef int(__thiscall* sub_5046A0)(void*, void*, void*, void*, void*, void*);
extern sub_5046A0 Originalsub_5046A0;
int(__fastcall sub_5046A0Hook)(void*, void*, void*, void*, void*, void*, void*);

typedef int(__thiscall* sub_509370)(void*, void*);
extern sub_509370 Originalsub_509370;
int __fastcall sub_509370Hook(void*, void*, void*);

typedef int(__thiscall* sub_505380)(void*, void*);
extern sub_505380 Originalsub_505380;
int __fastcall sub_505380Hook(void*, void*, void*);

extern bool early_exit_sub_506870;
extern bool early_exit_sub_509370;
extern bool early_exit_sub_505740;
extern bool early_exit_sub_505380;
}  // namespace guicontrol

namespace wintimer {
typedef void(__stdcall* GetElapsedMS)(void);
extern GetElapsedMS OriginalGetElapsedMS;
void __stdcall GetElapsedMSHook(void);
}  // namespace wintimer

namespace simbase {
typedef void(__cdecl* Dump)(void* sim_object);
extern Dump OriginalDump;
}  // namespace simbase

namespace dgl {
// 00446070
extern std::vector<std::tuple<std::string, ImVec2, ImColor>> string_projection_buffer;
typedef int(__cdecl* dglDrawTextN)(void*, void*, void*, void*, void*, void*);
extern dglDrawTextN OriginaldglDrawTextN;
int __cdecl dglDrawTextNHook(void*, void*, void*, void*, void*, void*);
}  // namespace dgl

namespace other_unknown {
typedef char(__thiscall* GetGameObjectName)(void* object, char* string_buffer, int max_length);
extern GetGameObjectName OriginalGetGameObjectName;
}  // namespace other_unknown
}  // namespace hooks
}  // namespace t2