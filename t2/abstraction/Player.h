#pragma once

#include "ShapeBase.h"
#include "GameConnection.h"

#include <unordered_set>
#include <string>

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
                extern std::unordered_set<std::string> player_names;
                extern std::string spectator_player_name;
                extern int player_name_index;

                void __fastcall SetRenderPositionHook(void* this_player, void* _, void* arg1, void* arg2, void* arg3);
                typedef void(__thiscall* PlayerSetRenderPosition)(void*, void*, void*, void*);
                extern PlayerSetRenderPosition OriginalPlayerSetRenderPosition;
            }
        }
    }
}