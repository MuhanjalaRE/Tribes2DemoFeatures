#include <t2/abstraction/GameConnection.h>
#include <t2/hooks/global_hooks.h>
#include <string>
#include <t2/abstraction/SceneObject.h>
#include <t2/settings/settings.h>
#include <t2/game data/demo.h>
#include <t2/abstraction/Camera.h>


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
					if (target_control_object_namespace == "Camera"){
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
					} else if (target_control_object_namespace == "Player"){
						t2::game_data::demo::player = object;
						t2::game_data::demo::is_player_alive = true;

						// We want to view the camera, so don't change the game connections control object to the player. This only happens if the camera exists of course. The camera will be found by latest immediately after the player dies for the first time in the demo
						if (t2::game_data::demo::camera && t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera){
							return;
						}
					}

					return OriginalSetControlObject(this_gameconnection, object);
				}

				ReadPacket OriginalReadPacket = (ReadPacket)0x005FB9F0;

				void __fastcall ReadPacketHook(void* this_gameconnection, void* _, void* bitstream) {
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
					//if (t2::game_data::demo::camera && t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera)
						*(game_connection.damage_flash_) = 0;

					
					t2::abstraction::SceneObject target_control_object(game_connection.controlling_object_);
					std::string target_control_object_namespace(target_control_object.namespace_name_);

					if (target_control_object_namespace == "Camera" && false) {
						if (!t2::game_data::demo::camera){
							t2::game_data::demo::camera = game_connection.controlling_object_;
							t2::game_data::demo::camera_position = target_control_object2.object_to_world_->GetColumn(3);
							t2::game_data::demo::camera_direction = target_control_object2.object_to_world_->GetColumn(1);
							t2::math::Vector empty;

							t2::math::Vector rot_ = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Vector, target_control_object2.pointer_to_torque_simobject_class_, 2380);
							t2::math::Vector head_ = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Vector, target_control_object2.pointer_to_torque_simobject_class_, 2368);

							t2::game_data::demo::camera_rotation = rot_;
							t2::game_data::demo::camera_rotation.z_ += t2::game_data::demo::camera_yaw_offset;

							//t2::game_data::demo::camera_rotation = *target_control_object2
							t2::abstraction::hooks::Camera::OriginalSetPosition(game_connection.controlling_object_, &t2::game_data::demo::camera_position, &empty);
						}



						t2::game_data::demo::is_player_alive = false;
					}
					else if (target_control_object_namespace == "Player") {

						if (!t2::game_data::demo::player) {
							//t2::game_data::demo::camera = game_connection.controlling_object_;
							t2::game_data::demo::camera_position = target_control_object.object_to_world_->GetColumn(3);
							t2::game_data::demo::camera_direction = target_control_object.object_to_world_->GetColumn(1);
							t2::math::Vector empty;

							t2::math::Vector rot_ = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Vector, target_control_object.pointer_to_torque_simobject_class_, 2380);
							t2::math::Vector head_ = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Vector, target_control_object.pointer_to_torque_simobject_class_, 2368);

							t2::game_data::demo::camera_rotation = rot_;
							t2::game_data::demo::camera_rotation.z_ += t2::game_data::demo::camera_yaw_offset;

							//t2::game_data::demo::camera_rotation = *target_control_object2
							//t2::abstraction::hooks::Camera::OriginalSetPosition(game_connection.controlling_object_, &t2::game_data::demo::camera_position, &empty);
						}

						t2::game_data::demo::player = game_connection.controlling_object_;
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
					t2::game_data::demo::view_target = t2::game_data::demo::ViewTarget::kCamera;
					t2::game_data::demo::is_player_alive = false;
					t2::game_data::demo::player = NULL;
					t2::game_data::demo::camera = NULL;
					t2::game_data::demo::initialised = false;
					OriginalDemoPlayBackComplete(this_gameconnection);
				}


				bool __fastcall GetControlCameraTransformHook(void* this_gameconnection, void* _, float dt, void* matrix) {
					OriginalGetControlCameraTransform(this_gameconnection, dt, matrix);
					t2::math::Matrix* mat = (t2::math::Matrix*)matrix;
					//if (!t2::game_data::demo::camera) {
						//mat->SetColumn(3, { -100,-300, 1000 });
					//}
					//else {
					if (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera /* && t2::game_data::demo::camera */) {
						//t2::hooks::ShapeBase::OriginalGetEyeTransform(t2::game_data::demo::camera, &(query_->cameraMatrix));
						//t2::hooks::ShapeBase::OriginalGetEyeTransform(t2::game_data::demo::camera, mat);
						*mat = t2::game_data::demo::camera_matrix;
						//t2::math::Vector v = mat->GetColumn(3);
						//PLOG_DEBUG << "GetCameraControlTransformHook\t" << v.x_ << "\t" << v.y_ << "\t" << v.z_;
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

			}
		}
	}
}