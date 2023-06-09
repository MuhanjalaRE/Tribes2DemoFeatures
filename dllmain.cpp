#define USE_IMGUI

#include <Windows.h>
#include <imgui/imgui.h>
#include <keys/keys.h>
#include <t2/abstraction/Camera.h>
#include <t2/abstraction/GameConnection.h>
#include <t2/abstraction/NetConnection.h>
#include <t2/abstraction/Player.h>
#include <t2/abstraction/SceneObject.h>
#include <t2/abstraction/ShapeBase.h>
#include <t2/game data/demo.h>
#include <t2/hooks/global_hooks.h>
#include <t2/settings/settings.h>

#include <unordered_map>

#include "detours/detours.h"
#include "t2/game data/demo.h"
// #include <imgui/imgui_impl_glut.h>
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
// typedef BOOL(__stdcall* QueryPerformanceCounter_)(LARGE_INTEGER* lpPerformanceCount);
// QueryPerformanceCounter_ OriginalQueryPerformanceCounter = NULL;
DWORD game_thread_id = NULL;
LARGE_INTEGER initial_performance_counter = {0};
LARGE_INTEGER current_performance_counter = {0};
LARGE_INTEGER previous_performance_counter = {0};
typedef BOOL(__stdcall* wglSwapBuffers)(int*);
wglSwapBuffers OriginalwglSwapBuffers = NULL;
bool show_imgui_demo_window = false;

std::unordered_map<DWORD, LARGE_INTEGER> thread_counter_map;
std::unordered_map<DWORD, LARGE_INTEGER> previous_thread_counter_map;

BOOL __stdcall QueryPerformanceCounterHook(LARGE_INTEGER* lpPerformanceCount) {
    // if (GetCurrentThreadId() == game_thread_id){

    t2::hooks::OriginalQueryPerformanceCounter(&current_performance_counter);
    DWORD thread_id = GetCurrentThreadId();
    if (thread_counter_map.find(thread_id) == thread_counter_map.end()) {
        thread_counter_map[thread_id] = {0};
        // OriginalQueryPerformanceCounter(&thread_counter_map[thread_id]);
        thread_counter_map[thread_id] = current_performance_counter;
        previous_thread_counter_map[thread_id] = thread_counter_map[thread_id];
    }

    // PLOG_DEBUG << "1\t" << double(current_performance_counter.QuadPart - previous_performance_counter.QuadPart);
    // PLOG_DEBUG << "2\t" << (long long)(double(current_performance_counter.QuadPart - previous_performance_counter.QuadPart) * t2::game_data::demo::speed_hack_scale);

    thread_counter_map[thread_id].QuadPart += (long long)(double(current_performance_counter.QuadPart - previous_thread_counter_map[thread_id].QuadPart) * t2::game_data::demo::speed_hack_scale);

    // PLOG_DEBUG << "3\t" << initial_performance_counter.QuadPart - previous_performance_counter.QuadPart;
    //  << "4\t" << initial_performance_counter.QuadPart - current_performance_counter.QuadPart;

    previous_thread_counter_map[thread_id] = current_performance_counter;

    *lpPerformanceCount = thread_counter_map[thread_id];
    //}
    // else {
    //	OriginalQueryPerformanceCounter(lpPerformanceCount);
    //}
    return true;
}

// #define USE_AIMTRACKER

#ifdef USE_AIMTRACKER
#include <imgui/imgui.h>
#include <t2/game data/aimtracker.h>

#include <chrono>
#include <vector>
namespace t2 {
namespace game_data {
namespace aimtracker {

bool enabled = true;

struct AimTrackerSettings {
    ImVec2 window_size = {400, 400};
} aimtracker_settings;

class AimTrackerTick {
   private:
    float f_pitch, f_yaw, f_roll;
    std::chrono::steady_clock::time_point t_tick_time;

   public:
    AimTrackerTick(float pitch, float yaw, float roll) {
        this->f_pitch = pitch;
        this->f_yaw = FixYaw(yaw);
        this->f_roll = roll;
        this->t_tick_time = std::chrono::steady_clock::now();
    }

    static float FixYaw(float yaw) { return yaw; }

    // Getters
    float GetPitch(void) { return f_pitch; }

    float GetYaw(void) { return f_yaw; }

    float GetRoll(void) { return f_roll; }

    std::chrono::steady_clock::time_point GetTickTime(void) { return t_tick_time; }

    // Setters
    void SetPitch(float pitch) { this->f_pitch = pitch; }

    void SetYaw(float yaw) { this->f_yaw = yaw; }

    void SetRoll(float roll) { this->f_roll = roll; }

    void SetTickTime(std::chrono::steady_clock::time_point time) { this->t_tick_time = time; }
};

static double GetTimeDifference(const std::chrono::steady_clock::time_point& end_time, const std::chrono::steady_clock::time_point& start_time) {
    return std::chrono::duration<double>(end_time - start_time).count();
}

class AimTrackerTickManager {
   private:
    std::vector<AimTrackerTick> v_aim_ticks;
    int i_aim_tick_index;
    int i_aim_tick_max_count;
    int i_aim_tick_count;
    float f_lifetime;
    float f_zoom_yaw;
    float f_zoom_pitch;
    int i_ticks_per_second;
    float f_delay_per_tick;
    std::chrono::steady_clock::time_point t_last_tick_time;

   public:
    // static float f_lifetime;

    AimTrackerTickManager(void) {
        SetMaxCount(150 * 3);
        SetLifeTime(3.0);
        SetZoomPitch(2.0);
        SetZoomYaw(2.0);
        SetTicksPerSecond(150);
        t_last_tick_time = std::chrono::steady_clock::now();
    }

    void AddTick(AimTrackerTick tick) {
        /*
        AimTrackerTick* existing_tick = v_aim_ticks[i_aim_tick_index];
        if (existing_tick){
                        delete existing_tick;
        }
        */

        // v_aim_ticks[i_aim_tick_index] = tick;

        if (i_aim_tick_count < i_aim_tick_max_count) {
            v_aim_ticks.push_back(AimTrackerTick(0, 0, 0));
        }

        AimTrackerTick& existing_tick = v_aim_ticks[i_aim_tick_index];
        existing_tick.SetPitch(tick.GetPitch());
        existing_tick.SetYaw(tick.GetYaw());
        existing_tick.SetRoll(tick.GetRoll());
        existing_tick.SetTickTime(std::chrono::steady_clock::now());

        existing_tick = AimTrackerTick(tick.GetPitch(), tick.GetYaw(), tick.GetRoll());

        i_aim_tick_index++;
        i_aim_tick_index = i_aim_tick_index % i_aim_tick_max_count;

        i_aim_tick_count++;
        if (i_aim_tick_count >= i_aim_tick_max_count) {
            i_aim_tick_count = i_aim_tick_max_count;
        }

        t_last_tick_time = std::chrono::steady_clock::now();
    }

    // Setters

    void SetTicksPerSecond(int ticks_per_second) {
        this->i_ticks_per_second = ticks_per_second;
        this->f_delay_per_tick = 1.0 / ticks_per_second;
    }

    void SetMaxCount(int max_count) {
        i_aim_tick_index = 0;
        i_aim_tick_max_count = max_count;
        v_aim_ticks.clear();
        v_aim_ticks.reserve(i_aim_tick_max_count);
        /*
        for (int i=0;i<i_aim_tick_max_count;i++){
                        v_aim_ticks[i] = NULL;
        }
        */
        // v_aim_ticks.push_back(AimTrackerTick(0, 0, 0));
        i_aim_tick_count = 0;
    }

    void SetLifeTime(float lifetime) { this->f_lifetime = lifetime; }

    void SetZoomYaw(float zoom) { this->f_zoom_yaw = zoom; }

    void SetZoomPitch(float zoom) { this->f_zoom_pitch = zoom; }

    // Getters
    std::chrono::steady_clock::time_point GetLastTickTime(void) { return t_last_tick_time; }

    int GetTicksPerSecond(void) { return i_ticks_per_second; }

    float GetDelayPerTick(void) { return f_delay_per_tick; }

    float GetZoomPitch(void) { return f_zoom_pitch; }

    float GetZoomYaw(void) { return f_zoom_yaw; }

    float GetLifeTime(void) { return f_lifetime; }

    int GetCurrentIndex(void) { return i_aim_tick_index; }

    int GetCount(void) { return i_aim_tick_count; }

    int GetMaxCount(void) { return i_aim_tick_max_count; }

    std::vector<AimTrackerTick>& GetTicks(void) { return v_aim_ticks; }

    AimTrackerTick& GetTick(int index) {
        int i_index = i_aim_tick_index - 1 - index;
        if (i_index >= 0) {
            return v_aim_ticks[i_index];
        } else {
            return v_aim_ticks[i_aim_tick_max_count + i_index];
        }

        // return v_aim_ticks[(i_aim_tick_index + index)%i_aim_tick_max_count];
    }
} o_AimTrackerTickManager, o_AimTrackerPredictionTickManager;
}  // namespace aimtracker

using namespace aimtracker;
}  // namespace game_data
}  // namespace t2
#endif

BOOL __stdcall wglSwapBuffersHook(int* arg1) {
    // PLOG_DEBUG << "HDC = " << (unsigned int)arg1;

#ifdef USE_IMGUI
    static ImGuiIO* io = nullptr;
    if (!io) {
        io = &ImGui::GetIO();
    }

    static ImFont* font = NULL;
    if (!font && io && false) {
        ImFontConfig config_;
        config_.SizePixels = (int)(((ImFont*)io->Fonts->AddFontDefault())->FontSize * 2);

        font = io->Fonts->AddFontFromFileTTF("C:\\Users\\Administrator0\\AppData\\Local\\Microsoft\\Windows\\Fonts\\mononoki-Regular.ttf", 16, NULL, io->Fonts->GetGlyphRangesDefault());
        ImFontConfig config;
        config.MergeMode = true;
        config.GlyphMinAdvanceX = 0.0f;  // Use if you want to make the icon monospaced
        static const ImWchar icon_ranges[] = {0x25A0, 0x25FF, 0};
        io->Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\L_10646.ttf", 32, &config, icon_ranges);
        io->Fonts->Build();
    }

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    /*
    if (show_imgui_demo_window)
            ImGui::ShowDemoWindow(&show_imgui_demo_window);
    */

    DWORD dwWaitResult = WaitForSingleObject(t2::hooks::opengl::game_mutex, INFINITE);

    if (show_imgui_demo_window) {
        ImGui::Begin("Settings", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        // ImGui::SliderFloat("Camera yaw offset", &t2::game_data::demo::camera_yaw_offset, -2 * PI, 2 * PI);
        // ImGui::SliderFloat("debug_third_person_scalar", &t2::game_data::demo::debug_third_person_offset_scalar, -100, 100);
        if (ImGui::SliderFloat("Camera FOV", &t2::settings::camera_fov, 1, 179)) {
            if (t2::settings::camera_fov > t2::game_data::demo::max_fov) {
                t2::settings::camera_fov = t2::game_data::demo::max_fov;
            } else if (t2::settings::camera_fov < t2::game_data::demo::min_fov) {
                t2::settings::camera_fov = t2::game_data::demo::min_fov;
            }
        }

        if (ImGui::SliderFloat("Min FOV", &t2::game_data::demo::min_fov, 1, 179)) {
            if (t2::settings::camera_fov < t2::game_data::demo::min_fov) {
                t2::settings::camera_fov = t2::game_data::demo::min_fov;
            }
            if (t2::game_data::demo::min_fov > t2::game_data::demo::max_fov) {
                t2::game_data::demo::min_fov = t2::game_data::demo::max_fov;
            }
        }

        if (ImGui::SliderFloat("Max FOV", &t2::game_data::demo::max_fov, 1, 179)) {
            if (t2::settings::camera_fov > t2::game_data::demo::max_fov) {
                t2::settings::camera_fov = t2::game_data::demo::max_fov;
            }
            if (t2::game_data::demo::max_fov < t2::game_data::demo::min_fov) {
                t2::game_data::demo::max_fov = t2::game_data::demo::min_fov;
            }
        }

        ImGui::SliderFloat("FOV zoom change per second", &t2::game_data::demo::zoom_fov_delta_per_second, 1, 179);

        ImGui::Separator();

        static bool camera_movement_use_acceleration = t2::game_data::demo::camera_axis_movement_type == t2::game_data::demo::CameraAxisMovementType::KAcceleration;
        ImGui::Checkbox("Use acceleration for camera movement", &camera_movement_use_acceleration);
        if (camera_movement_use_acceleration) {
            t2::game_data::demo::camera_axis_movement_type = t2::game_data::demo::CameraAxisMovementType::KAcceleration;
            ImGui::SliderFloat("Camera movement acceleration per second (Forward/Backward)", &t2::game_data::demo::camera_axis_movement_y.acceleration_per_second, 1E-2, 1, "%.3f", ImGuiSliderFlags_::ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat("Camera movement deceleration per second (Forward/Backward)", &t2::game_data::demo::camera_axis_movement_y.deceleration_per_second, 1E-2, 1, "%.3f", ImGuiSliderFlags_::ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat("Camera movement maximum velocity (Forward/Backward)", &t2::game_data::demo::camera_axis_movement_y.maximum_velocity, 1E-2, 1E0, "%.3f", ImGuiSliderFlags_::ImGuiSliderFlags_AlwaysClamp);

            ImGui::Separator();

            ImGui::SliderFloat("Camera movement acceleration per second (Left/Right)", &t2::game_data::demo::camera_axis_movement_x.acceleration_per_second, 1E-2, 1, "%.3f", ImGuiSliderFlags_::ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat("Camera movement deceleration per second (Left/Right)", &t2::game_data::demo::camera_axis_movement_x.deceleration_per_second, 1E-2, 1, "%.3f", ImGuiSliderFlags_::ImGuiSliderFlags_AlwaysClamp);
            ImGui::SliderFloat("Camera movement maximum velocity (Left/Right)", &t2::game_data::demo::camera_axis_movement_x.maximum_velocity, 1E-2, 1E0, "%.3f", ImGuiSliderFlags_::ImGuiSliderFlags_AlwaysClamp);
        } else {
            t2::game_data::demo::camera_axis_movement_type = t2::game_data::demo::CameraAxisMovementType::kInstant;
            ImGui::SliderFloat("Camera movement speed", &t2::settings::camera_move_speed_xy, 1E-2, 1E0);
        }

        ImGui::Separator();
        ImGui::Checkbox("Show player model", &t2::settings::show_player_model);
        ImGui::Checkbox("Show weapon model", &t2::settings::show_weapon_model);
        ImGui::Separator();
        ImGui::SliderFloat("Speed scale", &t2::game_data::demo::speed_hack_scale, 0.1, 10, "%.3f", ImGuiSliderFlags_::ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_::ImGuiSliderFlags_AlwaysClamp);

        ImGui::Separator();
        /*
        ImGui::Checkbox("early_exit_sub_506870", &t2::hooks::guicontrol::early_exit_sub_506870);
        ImGui::Checkbox("early_exit_sub_509370", &t2::hooks::guicontrol::early_exit_sub_509370);
        ImGui::Checkbox("early_exit_sub_505740", &t2::hooks::guicontrol::early_exit_sub_505740);
        ImGui::Checkbox("early_exit_sub_505380", &t2::hooks::guicontrol::early_exit_sub_505380);
        */


        static std::vector<std::string> player_names;
        player_names.clear();
        for (std::unordered_set<std::string>::iterator i = t2::abstraction::hooks::Player::player_names.begin(); i != t2::abstraction::hooks::Player::player_names.end(); i++) {
            //ImGui::Text(i->c_str());
            player_names.push_back(*i);
        }
        

        ImGui::Text("Spectate player");
        if (ImGui::BeginCombo("Spectator", t2::abstraction::hooks::Player::spectator_player_name.c_str(), 0))
        {
            for (int n = 0; n < t2::abstraction::hooks::Player::player_names.size(); n++)
            {
                const bool is_selected = (t2::abstraction::hooks::Player::player_name_index == n);
                if (ImGui::Selectable(player_names[n].c_str(), is_selected)){
                    t2::abstraction::hooks::Player::player_name_index = n;
                    t2::abstraction::hooks::Player::spectator_player_name = player_names[n];
                }
                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }



#ifdef USE_AIMTRACKER
        ImGui::BeginGroup();

        ImGui::Checkbox("Enabled", &t2::game_data::aimtracker::enabled);

        static float f_lifetime = t2::game_data::aimtracker::o_AimTrackerTickManager.GetLifeTime();
        static int i_maxcount = t2::game_data::aimtracker::o_AimTrackerTickManager.GetMaxCount();
        static int i_ticks_per_second = t2::game_data::aimtracker::o_AimTrackerTickManager.GetTicksPerSecond();

        static float f_zoom_pitch = t2::game_data::aimtracker::o_AimTrackerTickManager.GetZoomPitch();
        static float f_zoom_yaw = t2::game_data::aimtracker::o_AimTrackerTickManager.GetZoomYaw();

        static float f_tool_window_size = t2::game_data::aimtracker::aimtracker_settings.window_size.x;

        ImGui::PushItemWidth(100);

        if (ImGui::SliderInt("Max items", &i_maxcount, 1, 10000)) {
            t2::game_data::aimtracker::o_AimTrackerTickManager.SetMaxCount(i_maxcount);
            t2::game_data::aimtracker::o_AimTrackerPredictionTickManager.SetMaxCount(i_maxcount);
        }

        if (ImGui::SliderFloat("Lifetime", &f_lifetime, 0, 60)) {
            t2::game_data::aimtracker::o_AimTrackerTickManager.SetLifeTime(f_lifetime);
            t2::game_data::aimtracker::o_AimTrackerPredictionTickManager.SetLifeTime(f_lifetime);
        }

        if (ImGui::SliderInt("Ticks per second", &i_ticks_per_second, 1, 300)) {
            t2::game_data::aimtracker::o_AimTrackerTickManager.SetTicksPerSecond(i_ticks_per_second);
            t2::game_data::aimtracker::o_AimTrackerPredictionTickManager.SetTicksPerSecond(i_ticks_per_second);
        }

        if (ImGui::SliderFloat("Pitch zoom", &f_zoom_pitch, 1, 500)) {
            t2::game_data::aimtracker::o_AimTrackerTickManager.SetZoomPitch(f_zoom_pitch);
            t2::game_data::aimtracker::o_AimTrackerPredictionTickManager.SetZoomPitch(f_zoom_pitch);
        }

        if (ImGui::SliderFloat("Yaw zoom", &f_zoom_yaw, 1, 500)) {
            t2::game_data::aimtracker::o_AimTrackerTickManager.SetZoomYaw(f_zoom_yaw);
            t2::game_data::aimtracker::o_AimTrackerPredictionTickManager.SetZoomYaw(f_zoom_yaw);
        }

        if (ImGui::SliderFloat("Window size", &f_tool_window_size, 200, 1200)) {
            t2::game_data::aimtracker::aimtracker_settings.window_size = {f_tool_window_size, f_tool_window_size};
        }

        ImGui::PopItemWidth();

        ImGui::EndGroup();
#endif
        ImGui::End();
    }

#ifdef USE_AIMTRACKER
    if (t2::game_data::aimtracker::enabled && t2::game_data::aimtracker::GetTimeDifference(std::chrono::steady_clock::now(), t2::game_data::aimtracker::o_AimTrackerTickManager.GetLastTickTime()) > t2::game_data::aimtracker::o_AimTrackerTickManager.GetDelayPerTick()) {
        if (t2::game_data::demo::is_player_alive) {
            t2::math::Vector rot_ = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Vector, t2::game_data::demo::player, 2380);
            t2::math::Vector head_ = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Vector, t2::game_data::demo::player, 2368);

            // Math::printRotator(r, "Rotation");
            // (r.Pitch > 270) {  // Looking down
            //	r.Pitch = -(360 - r.Pitch);
            //}

            while (rot_.z_ > M_2PI)
                rot_.z_ -= M_2PI;
            while (rot_.z_ < -M_2PI)
                rot_.z_ += M_2PI;
            t2::game_data::aimtracker::o_AimTrackerTickManager.AddTick(t2::game_data::aimtracker::AimTrackerTick(head_.x_, rot_.z_, 0));
        }
    }

    if (t2::game_data::aimtracker::enabled) {
        ImGui::SetNextWindowSize(t2::game_data::aimtracker::aimtracker_settings.window_size);

        //::PushStyleColor(ImGuiCol_WindowBg, visuals::radar_visual_settings.window_background_colour.Value);

        ImGui::Begin("aimtracker", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImVec2 window_position = ImGui::GetWindowPos();
        ImVec2 window_size = ImGui::GetWindowSize();

        ImVec2 center(window_position.x + window_size.x / 2, window_position.y + window_size.y / 2);

        ImDrawList* pDrawList = ImGui::GetWindowDrawList();

        pDrawList->AddLine({window_position.x, window_position.y + window_size.y / 2}, {window_position.x + window_size.x, window_position.y + window_size.y / 2}, ImColor(65, 65, 65, 255), 2);
        pDrawList->AddLine({window_position.x + window_size.x / 2, window_position.y}, {window_position.x + window_size.x / 2, window_position.y + window_size.y}, ImColor(65, 65, 65, 255), 2);
        pDrawList->AddCircleFilled(center, 3, ImColor(65, 65, 65, 125));

        for (int i = 0; i < t2::game_data::aimtracker::o_AimTrackerPredictionTickManager.GetCount(); i++) {
            t2::game_data::aimtracker::AimTrackerTick& tick = t2::game_data::aimtracker::o_AimTrackerPredictionTickManager.GetTick(i);
            double d = t2::game_data::aimtracker::GetTimeDifference(std::chrono::steady_clock::now(), tick.GetTickTime());

            if (d > t2::game_data::aimtracker::o_AimTrackerPredictionTickManager.GetLifeTime())
                break;

            pDrawList->AddCircleFilled({center.x + tick.GetYaw() * t2::game_data::aimtracker::o_AimTrackerPredictionTickManager.GetZoomYaw(), center.y - tick.GetPitch() * t2::game_data::aimtracker::o_AimTrackerPredictionTickManager.GetZoomPitch()}, (1 - d / t2::game_data::aimtracker::o_AimTrackerPredictionTickManager.GetLifeTime()) * 3, ImColor(d / t2::game_data::aimtracker::o_AimTrackerTickManager.GetLifeTime() * 0, 255, d / t2::game_data::aimtracker::o_AimTrackerPredictionTickManager.GetLifeTime() * 0, (1 - d / t2::game_data::aimtracker::o_AimTrackerPredictionTickManager.GetLifeTime()) * 255));
            // pDrawList->AddText({center.x + tick.GetYaw() * o_AimTrackerPredictionTickManager.GetZoomYaw(), center.y - tick.GetPitch() * o_AimTrackerPredictionTickManager.GetZoomPitch()}, ImCol(255,255,255,255), to_string()
        }

        for (int i = 0; i < t2::game_data::aimtracker::o_AimTrackerTickManager.GetCount(); i++) {
            t2::game_data::aimtracker::AimTrackerTick& tick = t2::game_data::aimtracker::o_AimTrackerTickManager.GetTick(i);
            double d = t2::game_data::aimtracker::GetTimeDifference(std::chrono::steady_clock::now(), tick.GetTickTime());

            if (d > t2::game_data::aimtracker::o_AimTrackerTickManager.GetLifeTime())
                break;

            pDrawList->AddCircleFilled({center.x + tick.GetYaw() * t2::game_data::aimtracker::o_AimTrackerTickManager.GetZoomYaw(), center.y - tick.GetPitch() * t2::game_data::aimtracker::o_AimTrackerTickManager.GetZoomPitch()}, (1 - d / t2::game_data::aimtracker::o_AimTrackerTickManager.GetLifeTime()) * 3, ImColor(255, d / t2::game_data::aimtracker::o_AimTrackerTickManager.GetLifeTime() * 255, d / t2::game_data::aimtracker::o_AimTrackerTickManager.GetLifeTime() * 255, (1 - d / t2::game_data::aimtracker::o_AimTrackerTickManager.GetLifeTime()) * 255));
        }

        ImGui::End();

        // ImGui::PopStyleColor();
    }

#endif

    ImGui::SetNextWindowPos({0, 0});
    ImVec2 display_size = ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowSize(display_size);
    ImGui::Begin("overlay_window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (font) {
        ImGui::PushFont(font);
    }

    ImDrawList* imgui_draw_list = ImGui::GetWindowDrawList();
    // opengl projects from bottom left of screen
    for (int i = 0; i < t2::hooks::opengl::projection_buffer.size(); i++) {
        imgui_draw_list->AddCircleFilled({t2::hooks::opengl::projection_buffer[i].x, (float)t2::hooks::opengl::projection_buffer[i].y}, 6, ImColor(255, 255, 0, 255), 0);
    }

    for (int i = 0; i < t2::hooks::dgl::string_projection_buffer.size(); i++) {
        imgui_draw_list->AddText(std::get<1>(t2::hooks::dgl::string_projection_buffer[i]), std::get<2>(t2::hooks::dgl::string_projection_buffer[i]), std::get<0>(t2::hooks::dgl::string_projection_buffer[i]).c_str());
    }
    
    ImGui::GetFont()->Scale = 3;
    ImGui::PushFont(ImGui::GetFont());
    auto windowWidth = ImGui::GetWindowSize().x;
    auto textWidth = ImGui::CalcTextSize(std::string("Observing ").append(t2::abstraction::hooks::Player::spectator_player_name).c_str()).x;
    imgui_draw_list->AddText(ImVec2((windowWidth - textWidth) * 0.5f, 0.8 * ImGui::GetWindowSize().y), ImColor(255, 255, 255, 255), std::string("Observing ").append(t2::abstraction::hooks::Player::spectator_player_name).c_str());
    ImGui::GetFont()->Scale = 1;
    ImGui::PopFont();
    
    if (font) {
        ImGui::PopFont();
    }
    ImGui::End();

    t2::hooks::opengl::projection_buffer.clear();
    t2::hooks::dgl::string_projection_buffer.clear();
    //t2::abstraction::hooks::Player::player_names.clear();

    ReleaseMutex(t2::hooks::opengl::game_mutex);

    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

#endif

    return OriginalwglSwapBuffers(arg1);
}

LRESULT __stdcall SetWindowLongPtrHook(HWND hWnd, int arg1, long arg2) {
    LRESULT res;
    // res = OriginalSetWindowLongPtr(hWnd, arg1, arg2);

#ifdef USE_IMGUI
    if (arg1 == GWL_WNDPROC) {
        ImGui_ImplWin32_Shutdown();
        ImGui_ImplWin32_Init(hWnd);
    }
#endif

    if (arg1 != GWL_WNDPROC) {
        return OriginalSetWindowLongPtr(hWnd, arg1, arg2);
    } else {
        original_windowproc_callback = (WNDPROC)OriginalSetWindowLongPtr(hWnd, arg1, arg2);
        OriginalSetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)CustomWindowProcCallback);
        return (LRESULT)original_windowproc_callback;

        /*
        //OriginalSetWindowLongPtr(hWnd, arg1, arg2);
        original_windowproc_callback = (WNDPROC)OriginalSetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)CustomWindowProcCallback);
        return (LRESULT)original_windowproc_callback;
        */
    }
    // original_windowproc_callback = (WNDPROC)SetWindowLongPtr(hWnd, GWL_WNDPROC, (LONG_PTR)CustomWindowProcCallback);
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

    hModule = GetModuleHandle(L"Kernel32.dll");
    if (hModule) {
        unsigned int queryperformancecounter_address = (unsigned int)GetProcAddress(hModule, "QueryPerformanceCounter");
        t2::hooks::OriginalQueryPerformanceCounter = (t2::hooks::QueryPerformanceCounter_)queryperformancecounter_address;
        t2::hooks::OriginalQueryPerformanceCounter(&initial_performance_counter);
        previous_performance_counter = initial_performance_counter;
        // t2::game_data::demo::OriginalQueryPerformanceCounter = (t2::game_data::demo::QueryPerformanceCounter_)queryperformancecounter_address;
        game_thread_id = GetCurrentThreadId();
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

#ifdef _DEBUG
    DetourAttach(&(PVOID&)t2::hooks::con::OriginalPrintf, t2::hooks::con::PrintfHook);
#endif
    DetourAttach(&(PVOID&)t2::hooks::fps::OriginalFpsUpdate, t2::hooks::fps::FpsUpdateHook);
    // DetourAttach(&(PVOID&)t2::hooks::ShapeBase::OriginalGetEyeTransform, t2::hooks::ShapeBase::GetEyeTransformHook);
    //  DetourAttach(&(PVOID&)t2::abstraction::hooks::GameConnection::OriginalSetControlObject, t2::abstraction::hooks::GameConnection::SetControlObjectHook);
    //  This will probably get called in ReadPacket when a camera is the control object. Hook this and disable it so we don't have any damage flashes?
    DetourAttach(&(PVOID&)t2::abstraction::hooks::Camera::OriginalProcessTick, t2::abstraction::hooks::Camera::ProcessTickHook);  // Hook this to disable any calls that update/lock the camera position

    DetourAttach(&(PVOID&)t2::abstraction::hooks::GameConnection::OriginalReadPacket, t2::abstraction::hooks::GameConnection::ReadPacketHook);
    DetourAttach(&(PVOID&)t2::hooks::game::OriginalSetCameraFOV, t2::hooks::game::SetCameraFOVHook);

    DetourAttach(&(PVOID&)t2::abstraction::hooks::GameConnection::OriginalDemoPlayBackComplete, t2::abstraction::hooks::GameConnection::DemoPlayBackCompleteHook);
    /*
    DetourAttach(&(PVOID&)t2::abstraction::hooks::NetConnection::OriginalStartDemoRecord, t2::abstraction::hooks::NetConnection::StartDemoRecordHook);
    DetourAttach(&(PVOID&)t2::abstraction::hooks::NetConnection::OriginalStopDemoRecord, t2::abstraction::hooks::NetConnection::StopDemoRecordHook);
    */

    DetourAttach(&(PVOID&)t2::hooks::game::OriginalGameProcessCameraQuery, t2::hooks::game::GameProcessCameraQueryHook);
    DetourAttach(&(PVOID&)t2::abstraction::hooks::GameConnection::OriginalGetControlCameraTransform, t2::abstraction::hooks::GameConnection::GetControlCameraTransformHook);

    DetourAttach(&(PVOID&)t2::abstraction::hooks::Camera::OriginalSetPosition, t2::abstraction::hooks::Camera::SetPositionHook);

    DetourAttach(&(PVOID&)t2::abstraction::hooks::Player::OriginalPlayerSetRenderPosition, t2::abstraction::hooks::Player::SetRenderPositionHook);  // Just hooking this to get all players and then get their names from there

    DetourAttach(&(PVOID&)t2::hooks::platform::OriginalSetWindowLocked, t2::hooks::platform::SetWindowLockedHook);

    // DetourAttach(&(PVOID&)t2::hooks::guicanvas::OriginalRenderFrame, t2::hooks::guicanvas::RenderFrameHook);

    DetourAttach(&(PVOID&)t2::hooks::guicontrol::OriginalOnRender, t2::hooks::guicontrol::OnRenderHook);
    DetourAttach(&(PVOID&)t2::hooks::guicontrol::OriginalOnRender2, t2::hooks::guicontrol::OnRenderHook2);
    DetourAttach(&(PVOID&)t2::hooks::guicontrol::OriginalRenderChildControls, t2::hooks::guicontrol::RenderChildControlsHook);

    // DetourAttach(&(PVOID&)t2::hooks::wintimer::OriginalGetElapsedMS, t2::hooks::wintimer::GetElapsedMSHook);

    /*
    DetourAttach(&(PVOID&)t2::hooks::guicontrol::Originalsub_505740, t2::hooks::guicontrol::sub_505740Hook); // called by sub_506870, this is what draws the IFFs ONLY
    //DetourAttach(&(PVOID&)t2::hooks::guicontrol::Originalsub_5046A0, t2::hooks::guicontrol::sub_5046A0Hook); // called by sub_506870, this is required to draw both IFFS and player name and health bar
    DetourAttach(&(PVOID&)t2::hooks::guicontrol::Originalsub_506870, t2::hooks::guicontrol::sub_506870Hook); // this calls sub_505740 & sub_509370 , this draws the player name and health bar, this is the main function
    DetourAttach(&(PVOID&)t2::hooks::guicontrol::Originalsub_509370, t2::hooks::guicontrol::sub_509370Hook); // testing purposes

    DetourAttach(&(PVOID&)t2::hooks::guicontrol::Originalsub_505380, t2::hooks::guicontrol::sub_505380Hook); // testing purposes, blocking this results in NAME + healthbar being hidden
    */

    DetourAttach(&(PVOID&)t2::hooks::dgl::OriginaldglDrawTextN, t2::hooks::dgl::dglDrawTextNHook);

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


    /*
    This is just obliterate all IFF related stuff -> so we hide player chevrons, player names, mission markers (waypoints) and flag/flag bases all at once. We can't pick and choose what to hide -> so let's find one of the GUI classes OnRender functions and try to hide things separately from there
    
    if (t2::hooks::opengl::OriginalGluProject)
        DetourAttach(&(PVOID&)t2::hooks::opengl::OriginalGluProject, t2::hooks::opengl::GluProjectHook);
    */

    if (t2::hooks::OriginalQueryPerformanceCounter) {
        DetourAttach(&(PVOID&)t2::hooks::OriginalQueryPerformanceCounter, QueryPerformanceCounterHook);
        // t2::game_data::demo::OriginalQueryPerformanceCounter = (t2::game_data::demo::QueryPerformanceCounter_)OriginalQueryPerformanceCounter;
    }

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
    // PLOG_DEBUG << "CustomWindowProcCallback";
    // PLOG_DEBUG << "HWND = " << (unsigned int)hWnd;

    bool* window_locked = (bool*)0x0083BFE5;

    if (msg == WM_KEYDOWN) {
        if (wParam == VK_INSERT) {
            // t2::settings::LoadSettings();
            show_imgui_demo_window = !show_imgui_demo_window;

            // "when having a directional key pressed while pressing "insert", it keeps going in that direction"
            for (std::unordered_map<unsigned int, bool>::iterator i = keys::key_states.begin(); i != keys::key_states.end(); i++) {
                i->second = false;
            }
        }
    }

    if (*window_locked && !show_imgui_demo_window) {
        if (msg == WM_KEYDOWN) {
            keys::key_states[wParam] = true;

            if (wParam == 0x43) {
                // t2::settings::set_camera = !t2::settings::set_camera;
                t2::game_data::demo::ToggleViewTarget();
            }
            if (wParam == VK_F3) {
                t2::game_data::demo::ToggleRecording();
            }
            if (wParam == 0x56) {
                t2::game_data::demo::show_iffs = !t2::game_data::demo::show_iffs;
            }
            if (wParam == 0x54) {
                t2::game_data::demo::first_person = !t2::game_data::demo::first_person;
            }

            if (wParam == 0x52) {
                t2::game_data::demo::speed_hack_scale = 1;
            }

            if (t2::game_data::demo::camera_axis_movement_type == t2::game_data::demo::CameraAxisMovementType::KAcceleration) {
                // If we tried to move the camera left or right
                if (wParam == 0x41 || wParam == 0x44) {
                    // if the camera is stopped
                    if (t2::game_data::demo::camera_axis_movement_x.state == t2::game_data::demo::CameraAxisMovement::State::kStopped) {
                        PLOG_DEBUG << "Accelerating on X axis";
                        if (wParam == 0x41) {  // if we 're moving left, we're going negative along the x axis
                            t2::game_data::demo::camera_axis_movement_x.direction = t2::game_data::demo::CameraAxisMovement::Direction::kNegative;
                        } else if (wParam == 0x44) {  // if we're moving right, we're going positive along the x axis
                            t2::game_data::demo::camera_axis_movement_x.direction = t2::game_data::demo::CameraAxisMovement::Direction::kPositive;
                        }
                        // QueryPerformanceCounter(&t2::game_data::demo::camera_axis_movement_x.acceleration_timestamp);
                        t2::hooks::OriginalQueryPerformanceCounter(&t2::game_data::demo::camera_axis_movement_x.acceleration_timestamp);
                        t2::game_data::demo::camera_axis_movement_x.state = t2::game_data::demo::CameraAxisMovement::State::kAccelerating;
                        t2::game_data::demo::camera_axis_movement_x.current_velocity = 0;
                    } /* else if (t2::game_data::demo::camera_axis_movement_x.state != t2::game_data::demo::CameraAxisMovement::State::kStopped) {
                        // if the camera is already moving in one direction but we press a key in the other direction, we should ignore the new keypress
                        if ((t2::game_data::demo::camera_axis_movement_x.direction == t2::game_data::demo::CameraAxisMovement::Direction::kPositive && wParam == 0x41) || (t2::game_data::demo::camera_axis_movement_x.direction == t2::game_data::demo::CameraAxisMovement::Direction::kNegative && wParam == 0x44)) {
                            keys::key_states[wParam] = false;
                        }
                    }*/
                }

                // if we tried to move the camera forward or backward
                if (wParam == 0x57 || wParam == 0x53) {
                    // if the camera is stopped

                    if (t2::game_data::demo::camera_axis_movement_y.state == t2::game_data::demo::CameraAxisMovement::State::kStopped) {
                        PLOG_DEBUG << "Accelerating";
                        if (wParam == 0x57) {  // if we 're moving forward, we're going positive along the y axis
                            t2::game_data::demo::camera_axis_movement_y.direction = t2::game_data::demo::CameraAxisMovement::Direction::kPositive;
                        } else if (wParam == 0x53) {  // if we're moving backwards, we're going negative along the y axis
                            t2::game_data::demo::camera_axis_movement_y.direction = t2::game_data::demo::CameraAxisMovement::Direction::kNegative;
                        }
                        // QueryPerformanceCounter(&t2::game_data::demo::camera_axis_movement_y.acceleration_timestamp);
                        t2::hooks::OriginalQueryPerformanceCounter(&t2::game_data::demo::camera_axis_movement_y.acceleration_timestamp);
                        t2::game_data::demo::camera_axis_movement_y.state = t2::game_data::demo::CameraAxisMovement::State::kAccelerating;
                        t2::game_data::demo::camera_axis_movement_y.current_velocity = 0;
                    } /* else if (t2::game_data::demo::camera_axis_movement_y.state != t2::game_data::demo::CameraAxisMovement::State::kStopped) {
                        // if the camera is already moving in one direction but we press a key in the other direction, we should ignore the new keypress
                        if ((t2::game_data::demo::camera_axis_movement_y.direction == t2::game_data::demo::CameraAxisMovement::Direction::kPositive && wParam == 0x53) || (t2::game_data::demo::camera_axis_movement_y.direction == t2::game_data::demo::CameraAxisMovement::Direction::kNegative && wParam == 0x57)) {
                            keys::key_states[wParam] = false;
                        }
                    } */
                }
            } else if (t2::game_data::demo::camera_axis_movement_type == t2::game_data::demo::CameraAxisMovementType::kInstant) {
            }

            PLOG_DEBUG << "Key pressed : " << std::hex << wParam << std::dec;

        } else if (msg == WM_KEYUP) {
            PLOG_DEBUG << "Key unpressed : " << std::hex << wParam << std::dec;
            if (t2::game_data::demo::camera_axis_movement_type == t2::game_data::demo::CameraAxisMovementType::KAcceleration) {
                // If we tried to move the camera left or right
                if (wParam == 0x41 || wParam == 0x44) {
                    // just an extra sanity check to ensure we only care about key ups when the camera is accelerating
                    if (t2::game_data::demo::camera_axis_movement_x.state == t2::game_data::demo::CameraAxisMovement::State::kAccelerating) {
                        // We are moving in a certain direction and the key associated to mvoving in that direction has been released
                        if ((t2::game_data::demo::camera_axis_movement_x.direction == t2::game_data::demo::CameraAxisMovement::Direction::kNegative && wParam == 0x41) || (t2::game_data::demo::camera_axis_movement_x.direction == t2::game_data::demo::CameraAxisMovement::Direction::kPositive && wParam == 0x44)) {
                            PLOG_DEBUG << "Decelerating on X axis";
                            t2::game_data::demo::camera_axis_movement_x.state = t2::game_data::demo::CameraAxisMovement::State::kDecelerating;
                            // QueryPerformanceCounter(&t2::game_data::demo::camera_axis_movement_x.deceleration_timestamp);
                            t2::hooks::OriginalQueryPerformanceCounter(&t2::game_data::demo::camera_axis_movement_x.deceleration_timestamp);
                            t2::game_data::demo::camera_axis_movement_x.velocity_before_deceleration = t2::game_data::demo::camera_axis_movement_x.current_velocity;
                        }
                    }
                }

                // If we tried to move the camera forward or backward
                if (wParam == 0x57 || wParam == 0x53) {
                    // just an extra sanity check to ensure we only care about key ups when the camera is accelerating
                    if (t2::game_data::demo::camera_axis_movement_y.state == t2::game_data::demo::CameraAxisMovement::State::kAccelerating) {
                        // We are moving in a certain direction and the key associated to mvoving in that direction has been released
                        if ((t2::game_data::demo::camera_axis_movement_y.direction == t2::game_data::demo::CameraAxisMovement::Direction::kPositive && wParam == 0x57) || (t2::game_data::demo::camera_axis_movement_y.direction == t2::game_data::demo::CameraAxisMovement::Direction::kNegative && wParam == 0x53)) {
                            PLOG_DEBUG << "Decelerating";
                            t2::game_data::demo::camera_axis_movement_y.state = t2::game_data::demo::CameraAxisMovement::State::kDecelerating;
                            // QueryPerformanceCounter(&t2::game_data::demo::camera_axis_movement_y.deceleration_timestamp);
                            t2::hooks::OriginalQueryPerformanceCounter(&t2::game_data::demo::camera_axis_movement_y.deceleration_timestamp);
                            t2::game_data::demo::camera_axis_movement_y.velocity_before_deceleration = t2::game_data::demo::camera_axis_movement_y.current_velocity;
                        }
                    }
                }
            } else if (t2::game_data::demo::camera_axis_movement_type == t2::game_data::demo::CameraAxisMovementType::kInstant) {
            }

            keys::key_states[wParam] = false;

        } else if (msg == WM_LBUTTONDOWN) {
            keys::mouse_states[0] = true;
        } else if (msg == WM_LBUTTONUP) {
            keys::mouse_states[0] = false;
        } else if (msg == WM_RBUTTONDOWN) {
            keys::mouse_states[1] = true;
        } else if (msg == WM_RBUTTONUP) {
            keys::mouse_states[1] = false;
        } else if (msg == WM_MOUSEMOVE) {
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

            // PLOG_DEBUG << "x = " << mouse_x << "\ty = " << mouse_y;

            if (mouse_x == mouse_x_previous && mouse_y == mouse_y_previous) {
                counter++;
                if (counter >= 50) {
                    camera_mouse_x = mouse_x;
                    camera_mouse_y = mouse_y;
                }
            } else {
                counter = 0;
            }

            mouse_x_previous = mouse_x;
            mouse_y_previous = mouse_y;

            keys::mouse_states[2] = 0;
            keys::mouse_states[3] = 0;

            if (mouse_x > camera_mouse_x) {
                keys::mouse_states[2] = 1 * abs(camera_mouse_x - mouse_x);
            } else if (mouse_x < camera_mouse_x) {
                keys::mouse_states[2] = -1 * abs(camera_mouse_x - mouse_x);
            }

            if (mouse_y > camera_mouse_y) {
                keys::mouse_states[3] = 1 * abs(camera_mouse_y - mouse_y);
            } else if (mouse_y < camera_mouse_y) {
                keys::mouse_states[3] = -1 * abs(camera_mouse_y - mouse_y);
            }
        }

        if (msg == WM_MOUSEWHEEL) {
            int wparam_int = (int)wParam;
            int wparam_int_hi_word = HIWORD(wparam_int);
            if (wparam_int < 0) {
                wparam_int_hi_word = HIWORD(-wparam_int);
            }
            if (wparam_int > 0) {
                t2::game_data::demo::third_person_distance -= (t2::settings::third_person_zoom_rate * wparam_int_hi_word);
            }
            if (wparam_int < 0) {
                t2::game_data::demo::third_person_distance += (t2::settings::third_person_zoom_rate * abs(wparam_int_hi_word));
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
        // if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        //	return true;
        ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
        return true;
    } else {
        io.MouseDrawCursor = false;
        if (t2::game_data::demo::game_connection && *window_locked == false) {
            t2::hooks::platform::OriginalSetWindowLocked(true);
            *window_locked = true;
        }
    }
#endif
    return CallWindowProc(original_windowproc_callback, hWnd, msg, wParam, lParam);
}