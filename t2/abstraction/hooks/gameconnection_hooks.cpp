#include <t2/abstraction/Camera.h>
#include <t2/abstraction/GameConnection.h>
#include <t2/abstraction/SceneObject.h>
#include <t2/game data/demo.h>
#include <t2/hooks/global_hooks.h>
#include <t2/settings/settings.h>
#include <t2/abstraction/Player.h>

#include <string>

namespace t2 {
namespace abstraction {
namespace hooks {
namespace GameConnection {
SetControlObject OriginalSetControlObject = (SetControlObject)0x005FA970;

// This is basically only called via ReadPacket
void __fastcall SetControlObjectHook(void* this_gameconnection, void* _, void* object) {
    t2::abstraction::GameConnection game_connection(this_gameconnection);
    t2::abstraction::SimObject target_control_object(object);

    std::string target_control_object_namespace(target_control_object.namespace_name_);

    // Always set the game connections control object to the camera when the game tries to - usually this will only happen when the player is dead. No need to check what our view target is - if it's already a camera then the control object doesn't change. If it's a player then we should set the control object to the camera anyway so it looks cleaner, and when the player spawns back in the control object will be set back to the player
    if (target_control_object_namespace == "Camera") {
        // [IGNORE - Read update] If we're already in camera mode a camera has been found, don't change to the new camera (not sure if a camera is created each death or if the same one is used)
        // [Update] We don't really care if a new camera is created. It doesn't seem to be. The cameras transform (mObjToWorld) is updated on death. The call sequence is roughly Camera::SetTransform -> Camera::SetPosition -> SceneObject::SetTransform -> modify mObjToWorld. We could hook one of the camera functions in the chain (ie. Camera::SetTransform or Camera::SetPosition to return without calling the original function to prevent these changes. Is there any point to doing that? I doubt it. The ShapeBase::getEyeTransform function will be called regularly enough that we can just update the camera transform there. We can't hook getEyeTransform to return a fake a value, it seems the cameras actual transform in mObjToWorld is being referenced when drawing the camera PoV so mObjToWorld MUST be modified.
        /*
        // Comment this section due to the [Update] above
        if (t2::game_data::demo::camera && t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera) {
                return;
        }
        */
        t2::game_data::demo::camera = object;
        t2::game_data::demo::is_player_alive = false;
    } else if (target_control_object_namespace == "Player") {
        t2::game_data::demo::player = object;
        t2::game_data::demo::is_player_alive = true;

        // We want to view the camera, so don't change the game connections control object to the player. This only happens if the camera exists of course. The camera will be found by latest immediately after the player dies for the first time in the demo
        if (t2::game_data::demo::camera && t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera) {
            return;
        }
    }

    return OriginalSetControlObject(this_gameconnection, object);
}

ReadPacket OriginalReadPacket = (ReadPacket)0x005FB9F0;

void __fastcall ReadPacketHook(void* this_gameconnection, void* _, void* bitstream) {
    //*((unsigned int*)0x007A1A40) = false;
    // ReadPacket is called quite early in the demo (if not immediately, so we set our demo connection from this function
    t2::game_data::demo::game_connection = this_gameconnection;
    t2::game_data::demo::is_player_alive = false;

    t2::abstraction::GameConnection game_connection2(this_gameconnection);
    t2::abstraction::SceneObject target_control_object2(game_connection2.controlling_object_);
    // Call the original ReadPacket first so it can read all the moves and packet data and apply a new control object, damage flash, etc.
    OriginalReadPacket(this_gameconnection, bitstream);

    // Right now we have to do this after calling original ReadPacket as the variables in the abstract object are not pointers and so the values wouldn't update if we created this abstract object prior to calling the original ReadPacket
    t2::abstraction::GameConnection game_connection(this_gameconnection);

    // Disable any damage flashes
    // if (t2::game_data::demo::camera && t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera)
    *(game_connection.damage_flash_) = 0;

    t2::abstraction::SceneObject target_control_object(game_connection.controlling_object_);
    std::string target_control_object_namespace(target_control_object.namespace_name_);

    if (target_control_object_namespace == "Camera" && false) {
        if (!t2::game_data::demo::camera) {
            t2::game_data::demo::camera = game_connection.controlling_object_;
            t2::game_data::demo::camera_position = target_control_object2.object_to_world_->GetColumn(3);
            t2::game_data::demo::camera_direction = target_control_object2.object_to_world_->GetColumn(1);
            t2::math::Vector empty;

            t2::math::Vector rot_ = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Vector, target_control_object2.pointer_to_torque_simobject_class_, 2380);
            t2::math::Vector head_ = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Vector, target_control_object2.pointer_to_torque_simobject_class_, 2368);

            t2::game_data::demo::camera_rotation = rot_;
            t2::game_data::demo::camera_rotation.z_ += t2::game_data::demo::camera_yaw_offset;

            // t2::game_data::demo::camera_rotation = *target_control_object2
            t2::abstraction::hooks::Camera::OriginalSetPosition(game_connection.controlling_object_, &t2::game_data::demo::camera_position, &empty);
        }

        t2::game_data::demo::is_player_alive = false;
    } else if (target_control_object_namespace == "Player") {
        if (!t2::game_data::demo::player) {
            // t2::game_data::demo::camera = game_connection.controlling_object_;
            t2::game_data::demo::camera_position = target_control_object.object_to_world_->GetColumn(3);
            t2::game_data::demo::camera_direction = target_control_object.object_to_world_->GetColumn(1);
            t2::math::Vector empty;

            t2::math::Vector rot_ = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Vector, target_control_object.pointer_to_torque_simobject_class_, 2380);
            t2::math::Vector head_ = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Vector, target_control_object.pointer_to_torque_simobject_class_, 2368);

            t2::game_data::demo::camera_rotation = rot_;
            t2::game_data::demo::camera_rotation.z_ += t2::game_data::demo::camera_yaw_offset;

            // t2::game_data::demo::camera_rotation = *target_control_object2
            // t2::abstraction::hooks::Camera::OriginalSetPosition(game_connection.controlling_object_, &t2::game_data::demo::camera_position, &empty);
        }

        t2::game_data::demo::demo_player = game_connection.controlling_object_; //probably want to comment this out when spectating someone other than ourself
        t2::game_data::demo::is_player_alive = true;
        t2::game_data::demo::player_matrix = *target_control_object.object_to_world_;

        /*
        if (t2::game_data::demo::camera && t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera) {
                OriginalSetControlObject(this_gameconnection, t2::game_data::demo::camera);
        }
        */
    }

    return;
}

DemoPlayBackComplete OriginalDemoPlayBackComplete = (DemoPlayBackComplete)0x005FB950;
// If a demo is finished or early exited DemoPlaybackComplete is called. We'll clean up/reset our states here
void __fastcall DemoPlayBackCompleteHook(void* this_gameconnection, void* _) {
    PLOG_DEBUG << "Demo play back complete";
    t2::game_data::demo::game_connection = NULL;
    t2::game_data::demo::view_target = t2::game_data::demo::ViewTarget::kPlayer;
    t2::game_data::demo::is_player_alive = false;
    t2::game_data::demo::player = NULL;
    t2::game_data::demo::camera = NULL;
    t2::game_data::demo::initialised = false;
    OriginalDemoPlayBackComplete(this_gameconnection);
    t2::game_data::demo::first_person = true;

    t2::abstraction::hooks::Player::player_names.clear();
    t2::abstraction::hooks::Player::player_name_index = -1;
    // t2::game_data::demo::camera_movement_state = t2::game_data::demo::CameraMovementState::kStopped;
}

bool __fastcall GetControlCameraTransformHook(void* this_gameconnection, void* _, float dt, void* matrix) {
    /*
    if (t2::game_data::demo::first_person) {
            *((unsigned int*)0x007A1A40) = true;
    }
    else {
            *((unsigned int*)0x007A1A40) = false; // Disable first person
    }
    */

    /*
    *((float*)0x762A3C) = 179;
    *((float*)0x762C68) = 179;
    *((float*)0x00763E08) = 179;
    *((float*)0x0077CAD8) = 179;
    *((float*)0x00795C08) = 179;

    *((float*)0x762A40) = 1;
    */

    OriginalGetControlCameraTransform(this_gameconnection, dt, matrix);
    t2::math::Matrix* mat = (t2::math::Matrix*)matrix;

    if (t2::game_data::demo::player) {
        static char name_str_buffer[256];
        t2::hooks::other_unknown::OriginalGetGameObjectName(t2::game_data::demo::player, name_str_buffer, 256);
        PLOG_DEBUG << name_str_buffer;

        if (t2::game_data::demo::player != t2::game_data::demo::demo_player) {
            t2::abstraction::SceneObject player_scene_object(t2::game_data::demo::player);

            //::math::Matrix eye_matrix;
            // t2::hooks::ShapeBase::OriginalGetEyeTransform(t2::game_data::demo::player, &eye_matrix);
            t2::abstraction::GameConnection game_connection2(this_gameconnection);
            t2::abstraction::SceneObject target_control_object2(game_connection2.controlling_object_);

            t2::abstraction::hooks::GameConnection::OriginalSetControlObject(this_gameconnection, t2::game_data::demo::player);
			OriginalGetControlCameraTransform(this_gameconnection, dt, matrix);
			t2::abstraction::hooks::GameConnection::OriginalSetControlObject(this_gameconnection, game_connection2.controlling_object_);
            return true;
        }
    }
    // if (!t2::game_data::demo::camera) {
    // mat->SetColumn(3, { -100,-300, 1000 });
    //}
    // else {
    if (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera /* && t2::game_data::demo::camera */) {
        // t2::hooks::ShapeBase::OriginalGetEyeTransform(t2::game_data::demo::camera, &(query_->cameraMatrix));
        // t2::hooks::ShapeBase::OriginalGetEyeTransform(t2::game_data::demo::camera, mat);
        *mat = t2::game_data::demo::camera_matrix;
        // t2::math::Vector v = mat->GetColumn(3);
        // PLOG_DEBUG << "GetCameraControlTransformHook\t" << v.x_ << "\t" << v.y_ << "\t" << v.z_;
    } else if (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kPlayer && !t2::game_data::demo::first_person && t2::game_data::demo::is_player_alive && true) {
        //*mat = t2::game_data::demo::camera_matrix;
        // return true;

        t2::math::Matrix matrix_bak = *mat;
        t2::math::Vector position = matrix_bak.GetColumn(3);
        t2::math::Vector player_direction = matrix_bak.GetColumn(1);
        player_direction.z_ = 0;
        player_direction = player_direction.Unit();
        position = position + (player_direction * t2::game_data::demo::debug_third_person_offset_scalar);
        // position.z_ -= 50;
        t2::math::Vector dir = t2::game_data::demo::camera_matrix.GetColumn(1);
        position -= dir * t2::game_data::demo::third_person_distance;
        // matrix_bak.SetColumn(1, dir);
        // matrix_bak.SetColumn(2, t2::game_data::demo::camera_matrix.GetColumn(2));
        matrix_bak.SetColumn(3, position);
        t2::game_data::demo::camera_matrix.SetColumn(3, position);
        *mat = t2::game_data::demo::camera_matrix;
        //*mat = t2::game_data::demo::camera_matrix;
        return true;
    }

    /*
    void* control_object = t2::abstraction::GameConnection(this_gameconnection).controlling_object_;
    t2::abstraction::SceneObject sim_object(control_object);
    *sim_object.object_to_world_ = *mat;
    *(t2::math::Matrix*)((unsigned int)control_object + 424) = *mat;
    *(t2::math::Matrix*)((unsigned int)control_object + 360) = *mat;
    */

    //}
    return true;
}
GetControlCameraTransform OriginalGetControlCameraTransform = (GetControlCameraTransform)0x005FAA80;

}  // namespace GameConnection
}  // namespace hooks
}  // namespace abstraction
}  // namespace t2