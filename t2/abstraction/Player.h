#pragma once

#include "ShapeBase.h"
#include "GameConnection.h"

namespace t2 {
    namespace abstraction {
        class Player : public ShapeBase {
            typedef ShapeBase Parent;
        public:
            Player(void* pointer_to_torque_player_class);
        };
    }
}

namespace t2 {
    namespace abstraction {
        namespace hooks {
            namespace Player {
                void __fastcall SetRenderPositionHook(void* this_player, void* _, void* arg1, void* arg2, void* arg3);
                typedef void(__thiscall* PlayerSetRenderPosition)(void*, void*, void*, void*);
                extern PlayerSetRenderPosition OriginalPlayerSetRenderPosition;
            }
        }
    }
}