#include "Player.h"
namespace t2 {
	namespace abstraction {

		Player::Player(void* pointer_to_torque_player_class) : Parent(pointer_to_torque_player_class) {
			if (!pointer_to_torque_player_class)
				return;
		}
	}
}