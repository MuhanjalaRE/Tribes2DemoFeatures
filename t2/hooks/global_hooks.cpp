#include "global_hooks.h"

#include <Windows.h>
#include <imgui/imgui.h>
#include <keys/keys.h>
#include <t2/abstraction/GameConnection.h>
#include <t2/abstraction/Player.h>
#include <t2/abstraction/ShapeBase.h>
#include <t2/game data/demo.h>
#include <t2/game data/player.h>
#include <t2/settings/settings.h>

t2::math::Matrix player_matrix;

namespace t2 {
namespace hooks {
namespace con {
Printf OriginalPrintf = (Printf)0x00425F30;
int __stdcall PrintfHook(char* format, ...) {
    va_list argptr;
    va_start(argptr, format);
    int ret = vfprintf(stdout, format, argptr);
    fprintf(stdout, "\n");
    va_end(argptr);
    return ret;
}

Executef OriginalExecutef = (Executef)0x004268D0;

}  // namespace con

namespace fps {
FpsUpdate OriginalFpsUpdate = (FpsUpdate)0x00564570;
void FpsUpdateHook(void) {
    OriginalFpsUpdate();

    *((unsigned int*)0x0079B3E4) = t2::settings::show_player_model;
    *((unsigned int*)0x0079B3E8) = t2::settings::show_weapon_model;

    // t2::game_data::demo::fps = 1.0f/(*(float*)0x0083F380);
    // PLOG_DEBUG << "FPS\t:\t" << t2::game_data::demo::fps;

    static LARGE_INTEGER performance_count;
    static LARGE_INTEGER previous_performance_count = {0};
    static LARGE_INTEGER frequency = {0};
    if (frequency.QuadPart == 0) {
        QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
    }

    QueryPerformanceCounter(&performance_count);
    double delta_time = (((double)(performance_count.QuadPart - previous_performance_count.QuadPart)) / frequency.QuadPart) * 1000.0f;

    previous_performance_count = performance_count;

    t2::game_data::demo::fps = 1000.0f / delta_time;
    // PLOG_DEBUG << "FPS\t:\t" << t2::game_data::demo::fps;

    /*
    *((float*)0x762A3C) = 179;
    *((float*)0x762C68) = 179;
    *((float*)0x00763E08) = 179;
    *((float*)0x0077CAD8) = 179;
    *((float*)0x00795C08) = 179;

    *((float*)0x762A40) = 1;
    */

    if (keys::key_states[0x5A]) {  // z key - zoom out
        t2::settings::camera_fov += (t2::game_data::demo::zoom_fov_delta_per_second / t2::game_data::demo::fps) / t2::game_data::demo::speed_hack_scale;
        if (t2::settings::camera_fov > t2::game_data::demo::max_fov) {
            t2::settings::camera_fov = t2::game_data::demo::max_fov;
        }
    }
    if (keys::key_states[0x58]) {  // x key - zoom in
        t2::settings::camera_fov -= (t2::game_data::demo::zoom_fov_delta_per_second / t2::game_data::demo::fps) / t2::game_data::demo::speed_hack_scale;
        if (t2::settings::camera_fov < t2::game_data::demo::min_fov) {
            t2::settings::camera_fov = t2::game_data::demo::min_fov;
        }
    }

    //*((unsigned int*)0x008423D4) = true; / Hide HUD HACK

    // t2::hooks::con::OriginalExecutef(2, "HideHudHACK", true);

    // first person or third person
    if (t2::game_data::demo::game_connection) {
        //*((int*)GET_OBJECT_POINTER_TO_VARIABLE_BY_OFFSET(t2::game_data::demo::game_connection, 33372)) = NULL;
    }
    // t2::hooks::game::OriginalSetCameraFOV(t2::settings::camera_fov);

    /*
    if (t2::game_data::demo::game_connection){
            t2::abstraction::GameConnection game_connection(t2::game_data::demo::game_connection);

            t2::abstraction::SimObject target_control_object(game_connection.controlling_object_);
            std::string target_control_object_namespace(target_control_object.namespace_name_);

            if (target_control_object_namespace == "Camera") {
                    t2::game_data::demo::camera = game_connection.controlling_object_;
                    t2::game_data::demo::is_player_alive = false;
            }
            else if (target_control_object_namespace == "Player") {
                    t2::game_data::demo::player = game_connection.controlling_object_;
                    t2::game_data::demo::is_player_alive = true;
                    if (t2::game_data::demo::camera && t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera) {
                            t2::abstraction::hooks::GameConnection::OriginalSetControlObject(t2::game_data::demo::game_connection, t2::game_data::demo::camera);
                    }
            }
    }
    */
    t2::game_data::player::players.clear();

    /*
    if (!t2::game_data::demo::camera || t2::game_data::demo::view_target != t2::game_data::demo::ViewTarget::kCamera) {
            return;
    }
    */

    if (/*t2::game_data::demo::camera&&*/ (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera || (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kPlayer && !t2::game_data::demo::first_person)) && t2::game_data::demo::player /* && t2::game_data::demo::is_player_alive*/) {
        // t2::abstraction::Camera camera_object(t2::game_data::demo::camera);
        // t2::abstraction::hooks::Camera::OriginalSetFlyMode(t2::game_data::demo::camera);

        // PLOG_DEBUG << std::hex << t2::game_data::demo::camera << std::dec;

        // static t2::math::Vector m_rot = *camera_object.m_rot_;
        static const float max_pitch = 1.3962;

        if (keys::mouse_states[3] >= 1) {
            if (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera)
                t2::game_data::demo::camera_rotation.x_ += settings::camera_rotation_speed_pitch * keys::mouse_states[3];
            else {
                t2::game_data::demo::camera_rotation.x_ += settings::camera_rotation_speed_pitch * keys::mouse_states[3];
                // t2::game_data::demo::camera_rotation.x_ = sin(t2::game_data::demo::camera_rotation.x_);
            }
        } else if (keys::key_states[0x49]) {
            if (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera)
                t2::game_data::demo::camera_rotation.x_ += settings::camera_rotation_speed_pitch;
            else {
                t2::game_data::demo::camera_rotation.x_ += settings::camera_rotation_speed_pitch;
                // t2::game_data::demo::camera_rotation.x_ = sin(t2::game_data::demo::camera_rotation.x_);
            }
        }

        if (keys::mouse_states[3] <= -1) {
            if (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera)
                t2::game_data::demo::camera_rotation.x_ -= settings::camera_rotation_speed_pitch * abs(keys::mouse_states[3]);
            else {
                t2::game_data::demo::camera_rotation.x_ -= settings::camera_rotation_speed_pitch * abs(keys::mouse_states[3]);
                // t2::game_data::demo::camera_rotation.x_ = sin(t2::game_data::demo::camera_rotation.x_);
            }
        } else if (keys::key_states[0x4B]) {
            if (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera)
                t2::game_data::demo::camera_rotation.x_ -= settings::camera_rotation_speed_pitch;
            else {
                t2::game_data::demo::camera_rotation.x_ -= settings::camera_rotation_speed_pitch;
                // t2::game_data::demo::camera_rotation.x_ = sin(t2::game_data::demo::camera_rotation.x_);
            }
        }

        /*/
        if (m_rot.x_ > max_pitch)
                m_rot.x_ = max_pitch;
        else if (m_rot.x_ < -max_pitch)
                m_rot.x_ = -max_pitch;
        */

        if (keys::mouse_states[2] >= 1) {
            if (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera)
                t2::game_data::demo::camera_rotation.z_ += settings::camera_rotation_speed_yaw * keys::mouse_states[2];
            else {
                t2::game_data::demo::camera_rotation.z_ += settings::camera_rotation_speed_yaw * keys::mouse_states[2];
                // t2::game_data::demo::camera_rotation.z_ = sin(t2::game_data::demo::camera_rotation.z_);
            }
        } else if (keys::key_states[0x4C]) {
            if (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera)
                t2::game_data::demo::camera_rotation.z_ += settings::camera_rotation_speed_yaw;
            else {
                t2::game_data::demo::camera_rotation.z_ += settings::camera_rotation_speed_yaw;
                // t2::game_data::demo::camera_rotation.z_ = sin(t2::game_data::demo::camera_rotation.z_);
            }
        }

        if (keys::mouse_states[2] <= -1) {
            if (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera)
                t2::game_data::demo::camera_rotation.z_ -= settings::camera_rotation_speed_yaw * abs(keys::mouse_states[2]);
            else {
                t2::game_data::demo::camera_rotation.z_ -= settings::camera_rotation_speed_yaw * abs(keys::mouse_states[2]);
                // t2::game_data::demo::camera_rotation.z_ - sin(t2::game_data::demo::camera_rotation.z_);
            }
        } else if (keys::key_states[0x4A]) {
            if (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera)
                t2::game_data::demo::camera_rotation.z_ -= settings::camera_rotation_speed_yaw;
            else {
                t2::game_data::demo::camera_rotation.z_ -= settings::camera_rotation_speed_yaw;
                // t2::game_data::demo::camera_rotation.z_ - sin(t2::game_data::demo::camera_rotation.z_);
            }
        }

        // static t2::math::Vector position = camera_object.object_to_world_->GetColumn(3);
        // static t2::math::Vector direction = camera_object.object_to_world_->GetColumn(1);

        t2::math::Vector position = t2::game_data::demo::camera_position;
        t2::math::Vector direction = t2::game_data::demo::camera_direction;
        if (!t2::game_data::demo::initialised) {
            // position = camera_object.object_to_world_->GetColumn(3);
            // direction = camera_object.object_to_world_->GetColumn(1);
            t2::game_data::demo::initialised = true;
        }

        static LARGE_INTEGER current_performance_counter;
        QueryPerformanceCounter(&current_performance_counter);

        /*
// this is the time in MILLISECONDS!!!
// convert it to seconds!!!
double delta_time_for_camera_movement = (((double)(current_performance_counter.QuadPart - t2::game_data::demo::camera_movement_start_time.QuadPart)) / frequency.QuadPart) * 1000.0f;
// convert to seconds
delta_time_for_camera_movement /= 1000.0f;
        */
        // use v = u + a*t
        //  set a time for the deceleration to occur until v = 0 at a certain t -> use interpolation between the start of deceleration and that end
        //  make sure to clamp v after a certain amount of t
        //  Always start with zero U when the camera is stopped

        // Works good, add deceleration tomorrow -> same thing, just opposite direction
        // make acceleration rate frame rate independent (like zoom)?
        if (t2::game_data::demo::camera_axis_movement_y.state != t2::game_data::demo::CameraAxisMovement::State::kStopped) {
            if (t2::game_data::demo::camera_axis_movement_y.direction == t2::game_data::demo::CameraAxisMovement::Direction::kPositive) {  // forward
                                                                                                                                           // direction.z_ = 0;
                // double delta_time_for_camera_movement = t2::game_data::demo::camera_axis_movement_y.state == t2::game_data::demo::CameraAxisMovement::State::kAccelerating ? (((double)(current_performance_counter.QuadPart - t2::game_data::demo::camera_axis_movement_y.acceleration_timestamp.QuadPart)) / frequency.QuadPart) : (((double)(current_performance_counter.QuadPart - t2::game_data::demo::camera_axis_movement_y.deceleration_timestamp.QuadPart)) / frequency.QuadPart);

                if (t2::game_data::demo::camera_axis_movement_y.state == t2::game_data::demo::CameraAxisMovement::State::kAccelerating) {
                    double delta_time_for_camera_movement = (((double)(current_performance_counter.QuadPart - t2::game_data::demo::camera_axis_movement_y.acceleration_timestamp.QuadPart)) / frequency.QuadPart);

                    float current_velocity = t2::math::clamp(0 + t2::game_data::demo::camera_axis_movement_y.acceleration_per_second * delta_time_for_camera_movement /* / t2::game_data::demo::fps*/, 0 * t2::game_data::demo::camera_axis_movement_y.minimum_velocity, t2::game_data::demo::camera_axis_movement_y.maximum_velocity);

                    t2::game_data::demo::camera_axis_movement_y.current_velocity = current_velocity;

                    PLOG_DEBUG << "Accelerating forward at " << current_velocity << ", delta = " << delta_time_for_camera_movement;
                    position += direction.Unit() * t2::game_data::demo::camera_axis_movement_y.current_velocity;
                } else if (t2::game_data::demo::camera_axis_movement_y.state == t2::game_data::demo::CameraAxisMovement::State::kDecelerating) {
                    double delta_time_for_camera_movement = (((double)(current_performance_counter.QuadPart - t2::game_data::demo::camera_axis_movement_y.deceleration_timestamp.QuadPart)) / frequency.QuadPart);

                    float current_velocity = t2::math::clamp(t2::game_data::demo::camera_axis_movement_y.velocity_before_deceleration - t2::game_data::demo::camera_axis_movement_y.deceleration_per_second * delta_time_for_camera_movement /*/ t2::game_data::demo::fps */, 0 * t2::game_data::demo::camera_axis_movement_y.minimum_velocity, t2::game_data::demo::camera_axis_movement_y.maximum_velocity);

                    t2::game_data::demo::camera_axis_movement_y.current_velocity = current_velocity;
                    if (current_velocity == 0) {
                        t2::game_data::demo::camera_axis_movement_y.state = t2::game_data::demo::CameraAxisMovement::State::kStopped;
                        t2::game_data::demo::camera_axis_movement_y.direction = t2::game_data::demo::CameraAxisMovement::Direction::kNone;
                    }

                    PLOG_DEBUG << "Decelerating forward at " << current_velocity << ", delta = " << delta_time_for_camera_movement;

                    position += direction.Unit() * t2::game_data::demo::camera_axis_movement_y.current_velocity;
                }
            }
            if (t2::game_data::demo::camera_axis_movement_y.direction == t2::game_data::demo::CameraAxisMovement::Direction::kNegative) {  // backward
                // direction.z_ = 0;
                if (t2::game_data::demo::camera_axis_movement_y.state == t2::game_data::demo::CameraAxisMovement::State::kAccelerating) {
                    double delta_time_for_camera_movement = (((double)(current_performance_counter.QuadPart - t2::game_data::demo::camera_axis_movement_y.acceleration_timestamp.QuadPart)) / frequency.QuadPart);

                    float current_velocity = t2::math::clamp(0 + t2::game_data::demo::camera_axis_movement_y.acceleration_per_second * delta_time_for_camera_movement /* / t2::game_data::demo::fps */, 0 * t2::game_data::demo::camera_axis_movement_y.minimum_velocity, t2::game_data::demo::camera_axis_movement_y.maximum_velocity);

                    t2::game_data::demo::camera_axis_movement_y.current_velocity = current_velocity;

                    position -= direction.Unit() * t2::game_data::demo::camera_axis_movement_y.current_velocity;
                } else if (t2::game_data::demo::camera_axis_movement_y.state == t2::game_data::demo::CameraAxisMovement::State::kDecelerating) {
                    double delta_time_for_camera_movement = (((double)(current_performance_counter.QuadPart - t2::game_data::demo::camera_axis_movement_y.deceleration_timestamp.QuadPart)) / frequency.QuadPart);

                    float current_velocity = t2::math::clamp(t2::game_data::demo::camera_axis_movement_y.velocity_before_deceleration - t2::game_data::demo::camera_axis_movement_y.deceleration_per_second * delta_time_for_camera_movement /* / t2::game_data::demo::fps */, 0 * t2::game_data::demo::camera_axis_movement_y.minimum_velocity, t2::game_data::demo::camera_axis_movement_y.maximum_velocity);

                    t2::game_data::demo::camera_axis_movement_y.current_velocity = current_velocity;
                    if (current_velocity == 0) {
                        t2::game_data::demo::camera_axis_movement_y.state = t2::game_data::demo::CameraAxisMovement::State::kStopped;
                        t2::game_data::demo::camera_axis_movement_y.direction = t2::game_data::demo::CameraAxisMovement::Direction::kNone;
                    }

                    position -= direction.Unit() * t2::game_data::demo::camera_axis_movement_y.current_velocity;
                }
            }
        }

        if (t2::game_data::demo::camera_axis_movement_y.state != t2::game_data::demo::CameraAxisMovement::State::kStopped) {
            if (t2::game_data::demo::camera_axis_movement_x.direction == t2::game_data::demo::CameraAxisMovement::Direction::kNegative) {  // left
                direction = t2::math::Vector(direction.y_, -direction.x_, 0).Unit();                                                       // right vector
                if (t2::game_data::demo::camera_axis_movement_x.state == t2::game_data::demo::CameraAxisMovement::State::kAccelerating) {
                    double delta_time_for_camera_movement = (((double)(current_performance_counter.QuadPart - t2::game_data::demo::camera_axis_movement_x.acceleration_timestamp.QuadPart)) / frequency.QuadPart);

                    float current_velocity = t2::math::clamp(0 + t2::game_data::demo::camera_axis_movement_x.acceleration_per_second * delta_time_for_camera_movement / t2::game_data::demo::fps, 0 * t2::game_data::demo::camera_axis_movement_x.minimum_velocity, t2::game_data::demo::camera_axis_movement_x.maximum_velocity);

                    t2::game_data::demo::camera_axis_movement_x.current_velocity = current_velocity;

                    position -= direction.Unit() * t2::game_data::demo::camera_axis_movement_x.current_velocity;
                } else if (t2::game_data::demo::camera_axis_movement_x.state == t2::game_data::demo::CameraAxisMovement::State::kDecelerating) {
                    double delta_time_for_camera_movement = (((double)(current_performance_counter.QuadPart - t2::game_data::demo::camera_axis_movement_x.deceleration_timestamp.QuadPart)) / frequency.QuadPart);

                    float current_velocity = t2::math::clamp(t2::game_data::demo::camera_axis_movement_x.velocity_before_deceleration - t2::game_data::demo::camera_axis_movement_x.deceleration_per_second * delta_time_for_camera_movement / t2::game_data::demo::fps, 0 * t2::game_data::demo::camera_axis_movement_x.minimum_velocity, t2::game_data::demo::camera_axis_movement_x.maximum_velocity);

                    t2::game_data::demo::camera_axis_movement_x.current_velocity = current_velocity;
                    if (current_velocity == 0) {
                        t2::game_data::demo::camera_axis_movement_x.state = t2::game_data::demo::CameraAxisMovement::State::kStopped;
                        t2::game_data::demo::camera_axis_movement_x.direction = t2::game_data::demo::CameraAxisMovement::Direction::kNone;
                    }

                    position -= direction.Unit() * t2::game_data::demo::camera_axis_movement_x.current_velocity;
                }
            }

            if (t2::game_data::demo::camera_axis_movement_x.direction == t2::game_data::demo::CameraAxisMovement::Direction::kPositive) {  // right
                direction = t2::math::Vector(direction.y_, -direction.x_, 0).Unit();                                                       // right vector
                if (t2::game_data::demo::camera_axis_movement_x.state == t2::game_data::demo::CameraAxisMovement::State::kAccelerating) {
                    double delta_time_for_camera_movement = (((double)(current_performance_counter.QuadPart - t2::game_data::demo::camera_axis_movement_x.acceleration_timestamp.QuadPart)) / frequency.QuadPart);

                    float current_velocity = t2::math::clamp(0 + t2::game_data::demo::camera_axis_movement_x.acceleration_per_second * delta_time_for_camera_movement / t2::game_data::demo::fps, 0 * t2::game_data::demo::camera_axis_movement_x.minimum_velocity, t2::game_data::demo::camera_axis_movement_x.maximum_velocity);

                    t2::game_data::demo::camera_axis_movement_x.current_velocity = current_velocity;

                    position += direction.Unit() * t2::game_data::demo::camera_axis_movement_x.current_velocity;
                } else if (t2::game_data::demo::camera_axis_movement_x.state == t2::game_data::demo::CameraAxisMovement::State::kDecelerating) {
                    double delta_time_for_camera_movement = (((double)(current_performance_counter.QuadPart - t2::game_data::demo::camera_axis_movement_x.deceleration_timestamp.QuadPart)) / frequency.QuadPart);

                    float current_velocity = t2::math::clamp(t2::game_data::demo::camera_axis_movement_x.velocity_before_deceleration - t2::game_data::demo::camera_axis_movement_x.deceleration_per_second * delta_time_for_camera_movement / t2::game_data::demo::fps, 0 * t2::game_data::demo::camera_axis_movement_x.minimum_velocity, t2::game_data::demo::camera_axis_movement_x.maximum_velocity);

                    t2::game_data::demo::camera_axis_movement_x.current_velocity = current_velocity;
                    if (current_velocity == 0) {
                        t2::game_data::demo::camera_axis_movement_x.state = t2::game_data::demo::CameraAxisMovement::State::kStopped;
                        t2::game_data::demo::camera_axis_movement_x.direction = t2::game_data::demo::CameraAxisMovement::Direction::kNone;
                    }

                    position += direction.Unit() * t2::game_data::demo::camera_axis_movement_x.current_velocity;
                }
            }
        }

        if (keys::mouse_states[0]) {
            direction = t2::math::Vector(0, 0, 1).Unit();
            position += direction.Unit() * -1 * settings::camera_move_speed_z;
        }
        if (keys::mouse_states[1]) {
            direction = t2::math::Vector(0, 0, 1).Unit();
            position += direction.Unit() * 1 * settings::camera_move_speed_z;
        }

        /*
        if (!t2::game_data::demo::first_person && false) {
                t2::math::Matrix eye;
                t2::math::Vector bottom = t2::abstraction::ShapeBase(t2::game_data::demo::player).object_to_world_->GetColumn(3);
                t2::hooks::ShapeBase::OriginalGetEyeTransform(t2::game_data::demo::player, &eye);


                t2::math::Vector original_position = eye.GetColumn(3);
                original_position.z_ = ((eye.GetColumn(3).z_ + bottom.z_) / 2);
                original_position -= (direction * t2::game_data::demo::third_person_distance * 10);
                position = original_position;
                PLOG_DEBUG << "x = " << position.x_ << "\ty = " << position.y_ << "\tz = " << position.z_;
        }
        */

        // position = { 0,0, 400 };

        t2::math::Matrix mrender_obj_backup = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Matrix, t2::game_data::demo::player, 360);  // mRenderObjToWorld

        t2::math::Matrix target_matrix;
        t2::math::Matrix xRot, zRot;
        xRot.Set(t2::math::Vector(t2::game_data::demo::camera_rotation.x_, 0, 0));
        zRot.Set(t2::math::Vector(0, 0, t2::game_data::demo::camera_rotation.z_));
        t2::math::Matrix temp;
        temp.Mul(zRot, xRot);
        temp.SetColumn(3, position);
        target_matrix = temp;

        t2::abstraction::hooks::SceneObject::OriginalSetRenderTransform(t2::game_data::demo::player, &target_matrix);
        // t2::abstraction::hooks::Player::OriginalPlayerSetRenderPosition(t2::game_data::demo::player, &position, &t2::game_data::demo::camera_rotation, 0);
        // t2::abstraction::hooks::Camera::OriginalSetPosition(t2::game_data::demo::camera, &position, &t2::game_data::demo::camera_rotation);

        t2::math::Matrix m = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Matrix, t2::game_data::demo::player, 360);
        // t2::hooks::ShapeBase::OriginalGetEyeTransform(t2::game_data::demo::player, &m);
        // GET_OBJECT_POINTER_TO_VARIABLE_BY_OFFSET(t2::game_data::demo::player, 464);

        direction = m.GetColumn(1);
        position = m.GetColumn(3);

        t2::game_data::demo::camera_position = position;
        t2::game_data::demo::camera_direction = direction;

        t2::math::Matrix* mm = (t2::math::Matrix*)GET_OBJECT_POINTER_TO_VARIABLE_BY_OFFSET(t2::game_data::demo::player, 360);
        *mm = mrender_obj_backup;
        t2::game_data::demo::camera_matrix = m;
        // PLOG_DEBUG << "GetCameraControlTransformHook\t" << position.x_ << "\t" << position.y_ << "\t" << position.z_;
    }

    if (t2::game_data::demo::player && t2::game_data::demo::is_player_alive) {
        // t2::abstraction::hooks::Player::OriginalPlayerSetRenderPosition()
        // t2::abstraction::hooks::SceneObject::OriginalSetRenderTransform(t2::game_data::demo::player , &player_matrix);
    }

    /*
    if (t2::game_data::demo::game_connection && t2::game_data::demo::is_player_alive) {
            t2::abstraction::hooks::GameConnection::OriginalSetControlObject(t2::game_data::demo::game_connection, t2::game_data::demo::player);
    }
    */
}
}  // namespace fps

namespace game {
SetCameraFOV OriginalSetCameraFOV = (SetCameraFOV)0x005BB080;
void __cdecl SetCameraFOVHook(float fov) {
    return;
}

GameProcessCameraQuery OriginalGameProcessCameraQuery = (GameProcessCameraQuery)0x005BBCC0;
bool __cdecl GameProcessCameraQueryHook(void* query) {
    // return true;
    struct CameraQuery {
        CameraQuery() {
            ortho = false;
        }

        void* object;
        float nearPlane;
        float farPlane;
        float fov;
        t2::math::Matrix cameraMatrix;

        float leftRight;
        float topBottom;
        bool ortho;
        // Point3F position;
        // Point3F viewVector;
        // Point3F upVector;
    };

    CameraQuery* query_ = (CameraQuery*)query;

    /*
    if (t2::game_data::player::players.size() > 0 && t2::game_data::demo::game_connection) {
            t2::abstraction::GameConnection game_connection(t2::game_data::demo::game_connection);
            void* original_control_object = game_connection.controlling_object_;
            if (t2::game_data::player::players[0].pointer_to_torque_simobject_class_ && *(unsigned int*)t2::game_data::player::players[0].pointer_to_torque_simobject_class_)
                    t2::abstraction::hooks::GameConnection::OriginalSetControlObject(t2::game_data::demo::game_connection, t2::game_data::player::players[0].pointer_to_torque_simobject_class_);
            OriginalGameProcessCameraQuery(query);
            if (original_control_object)
                    t2::abstraction::hooks::GameConnection::OriginalSetControlObject(t2::game_data::demo::game_connection, original_control_object);
            //query_->cameraMatrix = *(t2::game_data::player::players[0].object_to_world_);
            return true;
    }
    */

    bool result = OriginalGameProcessCameraQuery(query);
    // query_->farPlane = 10000000;
    if (result) {
        query_->fov = t2::math::mDegToRad(t2::settings::camera_fov);
    }
    // memset(&(query_->cameraMatrix), 0, sizeof(t2::math::Matrix));
    return result;
}
}  // namespace game

namespace platform {
SetWindowLocked OriginalSetWindowLocked = (SetWindowLocked)0x005600A0;
void __cdecl SetWindowLockedHook(bool locked) {
    // PLOG_DEBUG << "SetWindowLockedHook";
    // OriginalSetWindowLocked(false);
}
}  // namespace platform

namespace opengl {
std::vector<ImVec2> projection_buffer;
HANDLE game_mutex = CreateMutex(NULL, false, NULL);

GluProject OriginalGluProject = (GluProject)0x0;
int __stdcall GluProjectHook(double objx, double objy, double objz, const double modelMatrix[16], const double projMatrix[16], const int viewport[4], double* winx, double* winy, double* winz) {
    bool result = OriginalGluProject(objx, objy, objz, modelMatrix, projMatrix, viewport, winx, winy, winz);
    //*winz = (0+1)/2;
    if (result && !t2::game_data::demo::show_iffs) {
        *winz = -9999;

        DWORD dwWaitResult = WaitForSingleObject(game_mutex, INFINITE);

        // projection_buffer.push_back({ (float)*winx, (float)*winy });

        ReleaseMutex(game_mutex);
        result = false;
        /*
        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize({ 1920, 1080 });
        ImGui::Begin("window", NULL, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBringToFrontOnFocus);
        */
        /*
        ImDrawList* imgui_draw_list = ImGui::GetWindowDrawList();
        //opengl projects from bottom left of screen
        imgui_draw_list->AddCircleFilled({ (float)*winx, (float)*winy }, 3, ImColor(255, 255, 255, 255), 0);
        */
        // ImGui::End();
    }
    return result;
}
}  // namespace opengl

namespace guicanvas {
extern RenderFrame OriginalRenderFrame = (RenderFrame)0x004B07F0;
void __fastcall RenderFrameHook(void* this_gui_object, void* _, bool prerenderonly) {
    /*
    t2::abstraction::SimObject sim_obj(this_gui_object);

    PLOG_DEBUG << sim_obj.namespace_name_;

    for (DWORD* i = (DWORD*)*((DWORD*)this_gui_object + 14); i != (DWORD*)(*((DWORD*)this_gui_object + 14) + 4 * *((DWORD*)this_gui_object + 12)); ++i) {
            DWORD* some_obj = (DWORD*)*i;
            t2::abstraction::SimObject some_sim_obj(some_obj);
            PLOG_DEBUG << "\t" << some_sim_obj.namespace_name_;
    }
    */
    /*
    for (DWORD** j = (DWORD**)*((DWORD*)this_gui_object + 14); j != (DWORD**)(*((DWORD*)this_gui_object + 14) + 4 * *((DWORD*)this_gui_object + 12)); ++j) {
            DWORD* some_obj = *j;
            t2::abstraction::SimObject some_sim_obj(some_obj);
            PLOG_DEBUG << "\t" << some_sim_obj.namespace_name_;
    }

    if ((unsigned int)this_gui_object == *(unsigned int*)0x009E9190) {

    }
    else {
            //return;
    }
    */
    // return;
    OriginalRenderFrame(this_gui_object, prerenderonly);
}
}  // namespace guicanvas

namespace guicontrol {
OnRender OriginalOnRender = (OnRender)0x4B6EE0;
bool is_demo_gui = false;
bool is_retcenter_hud = false;
void __fastcall OnRenderHook(void* this_obj, void* _, void* arg1, void* arg2, void* arg3) {
    t2::abstraction::SimObject sim_obj(this_obj);
    // PLOG_DEBUG << sim_obj.namespace_name_;

    if (std::string(sim_obj.namespace_name_) == "DemoPlaybackDlg") {
        is_demo_gui = true;
    } else {
        is_demo_gui = false;
    }

    /*
    if (std::string(sim_obj.namespace_name_) == "retCenterHud") {
            is_retcenter_hud = true;
            //return;
    }
    else {
            is_retcenter_hud = false;
    }
    */
    // early exiting doesnt stop from children being drawn

    /*
    //*(BYTE*)(((unsigned int*)this_obj)[18] + 55) = true;
    //*(BYTE*)(((unsigned int*)this_obj)[18] + 68) = true;
    */
    OriginalOnRender(this_obj, arg1, arg2, arg3);
    return;
}

OnRender OriginalOnRender2 = (OnRender)0x68A450;
void __fastcall OnRenderHook2(void* this_obj, void* _, void* arg1, void* arg2, void* arg3) {
    t2::abstraction::SimObject sim_obj(this_obj);
    // PLOG_DEBUG << sim_obj.namespace_name_;
    if (std::string(sim_obj.namespace_name_) == "DemoPlaybackDlg") {
        is_demo_gui = true;
    } else {
        is_demo_gui = false;
    }
    /*
    //*(BYTE*)(((unsigned int*)this_obj)[18] + 55) = true;
    //*(BYTE*)(((unsigned int*)this_obj)[18] + 68) = true;
    */
    OriginalOnRender2(this_obj, arg1, arg2, arg3);
    return;
}

RenderChildControls OriginalRenderChildControls = (RenderChildControls)0x4B6F60;
void __fastcall RenderChildControlsHook(void* this_gui_object, void* _, void* arg1, void* arg2, void* arg3) {
    /*
    if (t2::game_data::demo::game_connection){
            t2::abstraction::SimObject sim_obj(this_gui_object);
            for (DWORD* i = (DWORD*)*((DWORD*)this_gui_object + 14); i != (DWORD*)(*((DWORD*)this_gui_object + 14) + 4 * *((DWORD*)this_gui_object + 12)); ++i) {
                    DWORD* some_obj = (DWORD*)*i;
                    t2::abstraction::SimObject some_sim_obj(some_obj);
                    if (std::string(some_sim_obj.namespace_name_) == "navHud") {
                            *(BYTE*)((unsigned int)some_obj + 76) = true;
                    }
                    else {
                            *(BYTE*)((unsigned int)some_obj + 76) = false;
                    }
            }
    }
    */
    //*(BYTE*)(((unsigned int*)this_obj)[18] + 55) = true;
    //*(BYTE*)(((unsigned int*)this_obj)[18] + 68) = true;

    if (t2::game_data::demo::game_connection) {
        for (DWORD* i = (DWORD*)*((DWORD*)this_gui_object + 14); i != (DWORD*)(*((DWORD*)this_gui_object + 14) + 4 * *((DWORD*)this_gui_object + 12)); ++i) {
            DWORD* some_obj = (DWORD*)*i;
            t2::abstraction::SimObject some_sim_obj(some_obj);
            std::string some_sim_obj_name(some_sim_obj.namespace_name_);
            // PLOG_DEBUG << some_sim_obj.namespace_name_;
            if (some_sim_obj_name == "navHud" || is_demo_gui) {
                *(BYTE*)((unsigned int)some_obj + 76) = true;
            } else {
                *(BYTE*)((unsigned int)some_obj + 76) = false;
            }

            /*
            if (some_sim_obj_name == "helpTextGui" || some_sim_obj_name == "damageHud" || some_sim_obj_name == "GuiTextCtrl" || some_sim_obj_name == "reticleHud") {
                    *(BYTE*)((unsigned int)some_obj + 76) = false;
            }
            */

            /*
            if (is_retcenter_hud) {
                    *(BYTE*)((unsigned int)some_obj + 76) = true;
            }
            else {
                    *(BYTE*)((unsigned int)some_obj + 76) = false;
            }*/
        }
    }
    OriginalRenderChildControls(this_gui_object, arg1, arg2, arg3);
    return;
}

sub_505740 Originalsub_505740 = (sub_505740)0x505740;
char* __fastcall sub_505740Hook(void*, void*, void*, void*, void*) {
    return NULL;
}

sub_506870 Originalsub_506870 = (sub_506870)0x506870;
char __fastcall sub_506870Hook(void*, void*, void*, void*, void*) {
    return '\0';
}

extern sub_5046A0 Originalsub_5046A0 = (sub_5046A0)0x5046A0;
int(__fastcall sub_5046A0Hook)(void*, void*, void*, void*, void*, void*, void*) {
    return 0;
}

}  // namespace guicontrol

namespace wintimer {
GetElapsedMS OriginalGetElapsedMS = (GetElapsedMS)0x005618D0;

void __stdcall GetElapsedMSHook(void) {
    OriginalGetElapsedMS();

    static LARGE_INTEGER PerformanceCount;
    static LARGE_INTEGER* stru_83C154 = ((LARGE_INTEGER*)0x0083C154);
    static LARGE_INTEGER Frequency = {0};
    if (Frequency.QuadPart == 0) {
        QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);
    }

    short v4 = 3;
    short v5 = 8;

    QueryPerformanceCounter(&PerformanceCount);
    double v0 = (PerformanceCount.QuadPart - stru_83C154->QuadPart);
    if ((((PerformanceCount.QuadPart - stru_83C154->QuadPart) >> 32) & 0x80000000) != 0i64)
        v0 = v0 + *((double*)0x78C56C);
    double v1 = v0 * *((double*)0x78C570);
    double v2 = Frequency.QuadPart;
    if (Frequency.HighPart < 0)
        v2 = v2 + *((double*)0x78C56C);
    int v3 = (v1 / v2);  // ms elapsed

    double v6 = v3;
    if (v3 > (2.0f * t2::game_data::demo::speed_hack_scale)) {
        *stru_83C154 = PerformanceCount;
        //(*(*dword_9E8294 + 16))(dword_9E8294, &v4);
        typedef int(__thiscall * *some_func)(int, short*);
        // some_func some_func_ = (some_func)(*(DWORD*)0x9E8294 + 16);
        unsigned int* obj = (unsigned int*)0x9E8294;
        // some_func_((void*)*obj, &v4);
        (*(some_func)((*(DWORD*)(*(DWORD*)0x9E8294)) + 16))(*obj, &v4);
    }
}

void __stdcall GetElapsedMSHook2(void) {
    // unsigned int elapsed_time = OriginalGetElapsedMS();
    // return 1;

    static LARGE_INTEGER mPerfCountNext;
    static LARGE_INTEGER mPerfCountCurrent = {0};
    static LARGE_INTEGER mFrequency;
    QueryPerformanceFrequency((LARGE_INTEGER*)&mFrequency);

    QueryPerformanceCounter((LARGE_INTEGER*)&mPerfCountNext);
    double elapsed = (double)(1000.0f * double(mPerfCountNext.QuadPart - mPerfCountCurrent.QuadPart) / double(mFrequency.QuadPart));
    double delta1 = double(mPerfCountNext.QuadPart - mPerfCountCurrent.QuadPart);
    /*
    e = 1000 * (current_quad - old_quad) / frequency_quad
    e * frequency_quad = 1000 * (current_quad - old_quad)
    current_quad - old_quad = (e*frequency_quad)/1000
    current_quad = (e_ms * frequency_quad / 1000) + old_quad
    */

    static LARGE_INTEGER speed_hack_value;
    double delta = ((double)(elapsed * t2::game_data::demo::speed_hack_scale) * double(mFrequency.QuadPart) / 1000.0f);
    speed_hack_value.QuadPart = delta + (mPerfCountCurrent.QuadPart);

    mPerfCountCurrent = mPerfCountNext;

    *((LARGE_INTEGER*)0x0083C154) = mPerfCountCurrent;
    typedef int(__thiscall * some_func)(void*, void*);
    some_func some_func_ = (some_func)0x403E30;

    // double div =

    unsigned int* obj = (unsigned int*)0x9E8294;
    int three = 3;
    some_func_((void*)*obj, &three);
}
}  // namespace wintimer
}  // namespace hooks
}  // namespace t2