#include "aimtracker.h"
#include <imgui/imgui.h>
#include <chrono>
#include <vector>

namespace t2 {
	namespace game_data {
        namespace aimtracker {

            bool enabled = false;

            struct AimTrackerSettings {
                ImVec2 window_size = { 400, 400 };
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
                    }
                    else {
                        return v_aim_ticks[i_aim_tick_max_count + i_index];
                    }

                    // return v_aim_ticks[(i_aim_tick_index + index)%i_aim_tick_max_count];
                }
            } o_AimTrackerTickManager, o_AimTrackerPredictionTickManager;
        }  // namespace aimtracker

        using namespace aimtracker;
	}
}