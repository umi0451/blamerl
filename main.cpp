#include "console.h"
#include "game.h"
#include "log.h"
#include <cstdlib>

int main(int argc, char ** argv)
{
    srand(time(NULL));
	Console console;
	Game game(80, 23);
    log("Game started.");

	while(true) {
		console.draw_game(game);

		Control control = console.get_control();
		if(!game.process_control(control)) {
			break;
		}
	}

	return 0;
}

