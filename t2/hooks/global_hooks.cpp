#include "global_hooks.h"
#include <t2/settings/settings.h>
#include <t2/game data/demo.h>
#include <t2/abstraction/GameConnection.h>
#include <keys/keys.h>
#include <t2/abstraction/ShapeBase.h>
#include <t2/abstraction/Player.h>
#include <t2/game data/player.h>
#include <imgui/imgui.h>
#include <Windows.h>

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

			
		}

		namespace fps {
			FpsUpdate OriginalFpsUpdate = (FpsUpdate)0x00564570;
			void FpsUpdateHook(void) {
				*((unsigned int*)0x0079B3E4) = t2::settings::show_player_model;
				*((unsigned int*)0x0079B3E8) = t2::settings::show_weapon_model;
				//*((unsigned int*)0x008423D4) = true; / Hide HUD HACK
				
				//t2::hooks::con::OriginalExecutef(2, "HideHudHACK", true);
				
				//first person or third person
				if (t2::game_data::demo::game_connection) {
					//*((int*)GET_OBJECT_POINTER_TO_VARIABLE_BY_OFFSET(t2::game_data::demo::game_connection, 33372)) = NULL;
				}
				t2::hooks::game::OriginalSetCameraFOV(t2::settings::camera_fov);
				OriginalFpsUpdate();
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

				if (/*t2::game_data::demo::camera&&*/ t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera && t2::game_data::demo::player /* && t2::game_data::demo::is_player_alive*/) {
					//t2::abstraction::Camera camera_object(t2::game_data::demo::camera);
					//t2::abstraction::hooks::Camera::OriginalSetFlyMode(t2::game_data::demo::camera);


					//PLOG_DEBUG << std::hex << t2::game_data::demo::camera << std::dec;

					//static t2::math::Vector m_rot = *camera_object.m_rot_;
					static const float max_pitch = 1.3962;

					if (keys::mouse_states[3] >= 1) {
						t2::game_data::demo::camera_rotation.x_ += settings::camera_rotation_speed_pitch * keys::mouse_states[3];
					}
					else if (keys::key_states[0x49]) {
						t2::game_data::demo::camera_rotation.x_ += settings::camera_rotation_speed_pitch;
					}

					if (keys::mouse_states[3] <= -1) {
						t2::game_data::demo::camera_rotation.x_ -= settings::camera_rotation_speed_pitch * abs(keys::mouse_states[3]);
					}
					else if (keys::key_states[0x4B]) {
						t2::game_data::demo::camera_rotation.x_ -= settings::camera_rotation_speed_pitch;
					}

					/*/
					if (m_rot.x_ > max_pitch)
						m_rot.x_ = max_pitch;
					else if (m_rot.x_ < -max_pitch)
						m_rot.x_ = -max_pitch;
					*/

					if (keys::mouse_states[2] >= 1) {
						t2::game_data::demo::camera_rotation.z_ += settings::camera_rotation_speed_yaw * keys::mouse_states[2];
					}
					else if (keys::key_states[0x4C]) {
						t2::game_data::demo::camera_rotation.z_ += settings::camera_rotation_speed_yaw;
					}

					if (keys::mouse_states[2] <= -1) {
						t2::game_data::demo::camera_rotation.z_ -= settings::camera_rotation_speed_yaw * abs(keys::mouse_states[2]);
					}
					else if (keys::key_states[0x4A]) {
						t2::game_data::demo::camera_rotation.z_ -= settings::camera_rotation_speed_yaw;
					}

					//static t2::math::Vector position = camera_object.object_to_world_->GetColumn(3);
					//static t2::math::Vector direction = camera_object.object_to_world_->GetColumn(1);

					t2::math::Vector position = t2::game_data::demo::camera_position;
					t2::math::Vector direction = t2::game_data::demo::camera_direction;
					if (!t2::game_data::demo::initialised) {
						//position = camera_object.object_to_world_->GetColumn(3);
						//direction = camera_object.object_to_world_->GetColumn(1);
						t2::game_data::demo::initialised = true;
					}

					if (keys::key_states[0x57]) {
						//direction.z_ = 0;
						position += direction.Unit() * settings::camera_move_speed_xy;
					}
					if (keys::key_states[0x53]) {
						//direction.z_ = 0;
						position += direction.Unit() * -1 * settings::camera_move_speed_xy;
					}
					if (keys::key_states[0x41]) {
						direction = t2::math::Vector(direction.y_, -direction.x_, 0).Unit(); // right vector
						position += direction.Unit() * -1 * settings::camera_move_speed_xy;

					}
					if (keys::key_states[0x44]) {
						direction = t2::math::Vector(direction.y_, -direction.x_, 0).Unit(); // right vector
						position += direction.Unit() * settings::camera_move_speed_xy;

					}

					if (keys::mouse_states[0]) {
						direction = t2::math::Vector(0, 0, 1).Unit();
						position += direction.Unit() * -1 * settings::camera_move_speed_z;
					}
					if (keys::mouse_states[1]) {
						direction = t2::math::Vector(0, 0, 1).Unit();
						position += direction.Unit() * 1 * settings::camera_move_speed_z;
					}

					//position = { 0,0, 400 };

					t2::math::Matrix mrender_obj_backup = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Matrix, t2::game_data::demo::player, 360); //mRenderObjToWorld
					
					t2::math::Matrix target_matrix;
					t2::math::Matrix xRot, zRot;
					xRot.Set(t2::math::Vector(t2::game_data::demo::camera_rotation.x_, 0, 0));
					zRot.Set(t2::math::Vector(0, 0, t2::game_data::demo::camera_rotation.z_));
					t2::math::Matrix temp;
					temp.Mul(zRot, xRot);
					temp.SetColumn(3, position);
					target_matrix = temp;

					t2::abstraction::hooks::SceneObject::OriginalSetRenderTransform(t2::game_data::demo::player, &target_matrix);
					//t2::abstraction::hooks::Player::OriginalPlayerSetRenderPosition(t2::game_data::demo::player, &position, &t2::game_data::demo::camera_rotation, 0);
					//t2::abstraction::hooks::Camera::OriginalSetPosition(t2::game_data::demo::camera, &position, &t2::game_data::demo::camera_rotation);

					t2::math::Matrix m = GET_OBJECT_VARIABLE_BY_OFFSET(t2::math::Matrix, t2::game_data::demo::player, 360);
					//t2::hooks::ShapeBase::OriginalGetEyeTransform(t2::game_data::demo::player, &m);
						//GET_OBJECT_POINTER_TO_VARIABLE_BY_OFFSET(t2::game_data::demo::player, 464);

					direction = m.GetColumn(1);
					position = m.GetColumn(3);

					t2::game_data::demo::camera_position = position;
					t2::game_data::demo::camera_direction = direction;

					t2::math::Matrix* mm = (t2::math::Matrix*)GET_OBJECT_POINTER_TO_VARIABLE_BY_OFFSET(t2::game_data::demo::player, 360);
					*mm = mrender_obj_backup;
					t2::game_data::demo::camera_matrix = m;
					//PLOG_DEBUG << "GetCameraControlTransformHook\t" << position.x_ << "\t" << position.y_ << "\t" << position.z_;
				}
				
				if (t2::game_data::demo::player && t2::game_data::demo::is_player_alive) {
					//t2::abstraction::hooks::Player::OriginalPlayerSetRenderPosition()
					//t2::abstraction::hooks::SceneObject::OriginalSetRenderTransform(t2::game_data::demo::player , &player_matrix);
				}

				/*
				if (t2::game_data::demo::game_connection && t2::game_data::demo::is_player_alive) {
					t2::abstraction::hooks::GameConnection::OriginalSetControlObject(t2::game_data::demo::game_connection, t2::game_data::demo::player);
				}
				*/
			}
		}

		namespace game {
			SetCameraFOV OriginalSetCameraFOV = (SetCameraFOV)0x005BB080;
			void __cdecl SetCameraFOVHook(float fov) {
				return;
			}

			GameProcessCameraQuery OriginalGameProcessCameraQuery = (GameProcessCameraQuery)0x005BBCC0;
			bool __cdecl GameProcessCameraQueryHook(void* query) {
				//return true;
				struct CameraQuery
				{
					CameraQuery()
					{
						ortho = false;
					}

					void* object;
					float         nearPlane;
					float         farPlane;
					float         fov;
					t2::math::Matrix     cameraMatrix;

					float         leftRight;
					float         topBottom;
					bool        ortho;
					//Point3F position;
					//Point3F viewVector;
					//Point3F upVector;
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
				query_->farPlane = 10000000;
				query_->fov = 180;
				


				if (!t2::game_data::demo::camera) {
					//query_->cameraMatrix.SetColumn(3, { -100,-300, 1000 });
				}
				else {
					if (t2::game_data::demo::view_target == t2::game_data::demo::ViewTarget::kCamera) {
						bool v = OriginalGameProcessCameraQuery(query);
						t2::hooks::ShapeBase::OriginalGetEyeTransform(t2::game_data::demo::camera, &(query_->cameraMatrix));
						//t2::math::Matrix m = query_->cameraMatrix;
						
						//query_->cameraMatrix = m;
						//query_->object = t2::game_data::demo::camera;
						
						//t2::abstraction::hooks::GameConnection::OriginalSetControlObject(t2::game_data::demo::game_connection, t2::game_data::demo::camera);
						return v;
					}


					
					/*
					player_matrix = *t2::abstraction::SceneObject(t2::game_data::demo::player).object_to_world_;
					t2::abstraction::hooks::SceneObject::OriginalSetRenderTransform(t2::game_data::demo::player, &(query_->cameraMatrix));
					*t2::abstraction::SceneObject(t2::game_data::demo::player).object_to_world_ = query_->cameraMatrix;
					*/
				}
				return OriginalGameProcessCameraQuery(query);
				//memset(&(query_->cameraMatrix), 0, sizeof(t2::math::Matrix));
				return true;
			}
		}

		namespace platform {
			SetWindowLocked OriginalSetWindowLocked = (SetWindowLocked)0x005600A0;
			void __cdecl SetWindowLockedHook(bool locked) {
				//PLOG_DEBUG << "SetWindowLockedHook";
				//OriginalSetWindowLocked(false);
			}
		}

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
					
					//projection_buffer.push_back({ (float)*winx, (float)*winy });
					
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
					//ImGui::End();

				}
				return result;
			}
		}

		namespace guicanvas {
			extern RenderFrame OriginalRenderFrame = (RenderFrame)0x004B07F0;
			void __fastcall RenderFrameHook(void* this_gui_object, void* _, bool prerenderonly) {
				t2::abstraction::SimObject sim_obj(this_gui_object);
				PLOG_DEBUG << sim_obj.namespace_name_;

				for (DWORD* i = (DWORD*)*((DWORD*)this_gui_object + 14); i != (DWORD*)(*((DWORD*)this_gui_object + 14) + 4 * *((DWORD*)this_gui_object + 12)); ++i) {
					DWORD* some_obj = (DWORD*)*i;
					t2::abstraction::SimObject some_sim_obj(some_obj);
					PLOG_DEBUG << "\t" << some_sim_obj.namespace_name_;
				}

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
				//return;
				OriginalRenderFrame(this_gui_object, prerenderonly);
				
			}
		}

		namespace guicontrol {
			OnRender OriginalOnRender = (OnRender)0x4B6EE0;
			void __fastcall OnRenderHook(void* this_obj, void* _, void* arg1, void* arg2, void* arg3) {

				t2::abstraction::SimObject sim_obj(this_obj);
				PLOG_DEBUG << sim_obj.namespace_name_;

				//*(BYTE*)(((unsigned int*)this_obj)[18] + 55) = true;
				//*(BYTE*)(((unsigned int*)this_obj)[18] + 68) = true;

				OriginalOnRender(this_obj, arg1, arg2, arg3);
				return;
			}

			OnRender OriginalOnRender2 = (OnRender)0x68A450;
			void __fastcall OnRenderHook2(void* this_obj, void* _, void* arg1, void* arg2, void* arg3) {

				t2::abstraction::SimObject sim_obj(this_obj);
				PLOG_DEBUG << sim_obj.namespace_name_;

				//*(BYTE*)(((unsigned int*)this_obj)[18] + 55) = true;
				//*(BYTE*)(((unsigned int*)this_obj)[18] + 68) = true;

				OriginalOnRender2(this_obj, arg1, arg2, arg3);
				return;
			}

			RenderChildControls OriginalRenderChildControls = (RenderChildControls)0x4B6F60;
			void __fastcall RenderChildControlsHook(void* this_gui_object, void* _, void* arg1, void* arg2, void* arg3) {

				if (t2::game_data::demo::game_connection){

					t2::abstraction::SimObject sim_obj(this_gui_object);
					PLOG_DEBUG << sim_obj.namespace_name_;

					for (DWORD* i = (DWORD*)*((DWORD*)this_gui_object + 14); i != (DWORD*)(*((DWORD*)this_gui_object + 14) + 4 * *((DWORD*)this_gui_object + 12)); ++i) {
						DWORD* some_obj = (DWORD*)*i;
						t2::abstraction::SimObject some_sim_obj(some_obj);
						PLOG_DEBUG << "\t" << some_sim_obj.namespace_name_;

						if (std::string(some_sim_obj.namespace_name_) == "navHud" /* || std::string(some_sim_obj.namespace_name_) == "helpTextGui"*/) {
							*(BYTE*)((unsigned int)some_obj + 76) = true;
						}
						else {
							*(BYTE*)((unsigned int)some_obj + 76) = false;
						}
					}
				}

				//*(BYTE*)(((unsigned int*)this_obj)[18] + 55) = true;
				//*(BYTE*)(((unsigned int*)this_obj)[18] + 68) = true;

				OriginalRenderChildControls(this_gui_object, arg1, arg2, arg3);
				return;
			}

			
		}
	}
}