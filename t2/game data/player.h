#pragma once
#include <t2/abstraction/Player.h>
#include <vector>

namespace t2 {
	namespace game_data {
		namespace player{
			extern t2::abstraction::Player my_player;
			extern std::vector<t2::abstraction::Player> players;
		}
	}
}

//https://stackoverflow.com/questions/3729515/visual-studio-2010-2008-cant-handle-source-files-with-identical-names-in-diff