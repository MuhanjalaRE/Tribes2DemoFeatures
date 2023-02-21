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