#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include "Chess.hpp"
using namespace std;
using namespace sf;

int main()
{
    int size = VideoMode::getDesktopMode().height/1.2;
	RenderWindow window(VideoMode(size,size,32), "Chess", Style::Titlebar|Style::Close);
	Game chess;
	bool whiteMove = true;

	while (window.isOpen()) {
		Event evt;
		while (window.pollEvent(evt)) {
			if (evt.type==Event::Closed)
				window.close();
		}

		//play chess

		sleep(milliseconds(30));
	}

	return 0;
}
