#include "Nxna.h"
#include "Game1.h"

int main(int argc, char** argv)
{
	Game1* game = new Game1();
	game->Run();
	delete game;


	return 0;
}
