#include <t2/abstraction/Player.h>
#include <t2/game data/player.h>
#include <t2/hooks/global_hooks.h>
#include <t2/game data/demo.h>

namespace t2 {
namespace abstraction {
namespace hooks {
namespace Player {

    std::unordered_set<std::string> player_names;
    std::string spectator_player_name;
    int player_name_index = -1;

void __fastcall SetRenderPositionHook(void* this_player, void* _, void* arg1, void* arg2, void* arg3) {
    /*
    t2::abstraction::Player player(this_player);

    if (true) {
            if (player.controlling_client_) {

            }
            else {
                    if (GET_OBJECT_VARIABLE_BY_OFFSET(int, this_player, 512 * 4) || !GET_OBJECT_VARIABLE_BY_OFFSET(unsigned int, this_player, 0x26C)) {
                    } else {
                            t2::game_data::player::players.push_back(player);
                    }
            }
    }
    */

    if (GET_OBJECT_VARIABLE_BY_OFFSET(int, this_player, 512 * 4) /*isDead or something*/ || !GET_OBJECT_VARIABLE_BY_OFFSET(unsigned int, this_player, 0x26C) /*team variable exists*/) {
        
    } else {

        DWORD dwWaitResult = WaitForSingleObject(t2::hooks::opengl::game_mutex, INFINITE);

		static char name_str_buffer[256];
        t2::hooks::other_unknown::OriginalGetGameObjectName(this_player, name_str_buffer, 256);
        PLOG_DEBUG << name_str_buffer;

		//if (name_str_buffer[0+2] == 'R' && name_str_buffer[1+2] == '!' && name_str_buffer[2+2] == 'v' && name_str_buffer[3+2] == '3' && name_str_buffer[4+2] == 'r'){
		//	t2::game_data::demo::player = this_player;
		//}

        if (std::string(name_str_buffer) == t2::abstraction::hooks::Player::spectator_player_name) {
            t2::game_data::demo::player = this_player;
        }

        t2::abstraction::hooks::Player::player_names.insert(std::string(name_str_buffer));

        ReleaseMutex(t2::hooks::opengl::game_mutex);
	}

    OriginalPlayerSetRenderPosition(this_player, arg1, arg2, arg3);
}
PlayerSetRenderPosition OriginalPlayerSetRenderPosition = (PlayerSetRenderPosition)0x005D98C0;
}  // namespace Player
}  // namespace hooks
}  // namespace abstraction
}  // namespace t2