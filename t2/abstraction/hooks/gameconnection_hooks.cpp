#include <t2/abstraction/GameConnection.h>
#include <t2/hooks/global_hooks.h>
#include <string>
#include <t2/abstraction/SceneObject.h>


namespace t2 {
	namespace abstraction {
		namespace hooks {
			namespace GameConnection {
				SetControlObject OriginalSetControlObject = (SetControlObject)0x005FA970;
				void __fastcall SetControlObjectHook(void* this_gameconnection, void* _, void* object) {
					t2::abstraction::GameConnection game_connection(this_gameconnection);
					void* controlling_object_ = game_connection.controlling_object_;
					if (controlling_object_){
						t2::abstraction::SimObject sim_object(controlling_object_);
						if (sim_object.namespace_name_ && std::string(sim_object.namespace_name_) == "Camera") {
							return;
						}
					}
					return OriginalSetControlObject(this_gameconnection, object);
				}

				ReadPacket OriginalReadPacket = (ReadPacket)0x005FB9F0;
				void __fastcall ReadPacketHook(void* this_gameconnection, void* _, void* bitstream) {
					OriginalReadPacket(this_gameconnection, bitstream);
					float* damage_flash = (float*)GET_OBJECT_POINTER_TO_VARIABLE_BY_OFFSET(this_gameconnection, 8466 * 4);
					*damage_flash = 0;
				}
			}
		}
	}
}