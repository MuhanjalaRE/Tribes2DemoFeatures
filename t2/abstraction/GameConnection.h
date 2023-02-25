#pragma once

#include "NetConnection.h"
#include "ShapeBase.h"

namespace t2 {
	namespace abstraction {
		class GameConnection : public NetConnection {
			typedef NetConnection Parent;
		public:
			void* controlling_object_ = NULL;
			float* damage_flash_ = NULL;
			GameConnection(void* pointer_to_torque_gameconnection_class);
		};

		namespace hooks {
			namespace GameConnection {
				void __fastcall SetControlObjectHook(void*, void*, void*);
				typedef void(__thiscall* SetControlObject)(void*, void*);
				extern SetControlObject OriginalSetControlObject;

				void __fastcall ReadPacketHook(void*, void*, void*);
				typedef void(__thiscall* ReadPacket)(void*, void*);
				extern ReadPacket OriginalReadPacket;

				void __fastcall DemoPlayBackCompleteHook(void*, void*);
				typedef void(__thiscall* DemoPlayBackComplete)(void*);
				extern DemoPlayBackComplete OriginalDemoPlayBackComplete;

				bool __fastcall GetControlCameraTransformHook(void*, void*, float, void*);
				typedef bool(__thiscall* GetControlCameraTransform)(void*, float, void*);
				extern GetControlCameraTransform OriginalGetControlCameraTransform;

			}
		}
	}
}