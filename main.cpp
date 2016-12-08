#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include "Chess.hpp"
#include "AI.hpp"
using namespace std;
using namespace sf;

int main()
{
    int size = VideoMode::getDesktopMode().height/1.2;
	RenderWindow window(VideoMode(size,size,32), "Chess | White to move", Style::Titlebar|Style::Close);
	Music badMove;
	RectangleShape whiteSquare(Vector2f(size/8,size/8)), blackSquare(Vector2f(size/8,size/8)), highlight(Vector2f(size/8,size/8));
	CircleShape dot(size/(8*4));
	Texture pieceTxtrs[2][6];
	Sprite pieceImages[2][6];

	badMove.openFromFile("res/badMove.wav");
	whiteSquare.setFillColor(sf::Color::White);
	blackSquare.setFillColor(sf::Color(80,80,80));
	highlight.setFillColor(sf::Color(0,255,0,64));
	dot.setFillColor(sf::Color(40,40,40,120));
	pieceTxtrs[White][Pawn].loadFromFile("res/whitePawn.png");
	pieceTxtrs[White][Rook].loadFromFile("res/whiteRook.png");
	pieceTxtrs[White][Knight].loadFromFile("res/whiteKnight.png");
	pieceTxtrs[White][Bishop].loadFromFile("res/whiteBishop.png");
	pieceTxtrs[White][Queen].loadFromFile("res/whiteQueen.png");
	pieceTxtrs[White][King].loadFromFile("res/whiteKing.png");
	pieceTxtrs[Black][Pawn].loadFromFile("res/blackPawn.png");
	pieceTxtrs[Black][Rook].loadFromFile("res/blackRook.png");
	pieceTxtrs[Black][Knight].loadFromFile("res/blackKnight.png");
	pieceTxtrs[Black][Bishop].loadFromFile("res/blackBishop.png");
	pieceTxtrs[Black][Queen].loadFromFile("res/blackQueen.png");
	pieceTxtrs[Black][King].loadFromFile("res/blackKing.png");
	for (int i = 0; i<2; ++i) {
		for (int j = 0; j<6; ++j) {
			pieceImages[i][j].setTexture(pieceTxtrs[i][j]);
			double s = size/8;
			pieceImages[i][j].setScale(s/pieceTxtrs[i][j].getSize().x,s/pieceTxtrs[i][j].getSize().y);
		}
	}

	Game chess;
	AI ai(chess,Black);
    vector<Coord> moves;
	Coord selPos(-1,-1);
	bool whiteMove = true;
	bool done = false;
	int natDelay = 0;

	while (window.isOpen()) {
		Event evt;
		while (window.pollEvent(evt)) {
			if (evt.type==Event::Closed)
				window.close();
		}

		if (Mouse::isButtonPressed(Mouse::Left) && !done && whiteMove) {
			Vector2i pos = Mouse::getPosition(window);
			if (pos.x>=0 && pos.y>=0 && pos.x<size && pos.y<size) {
				Coord square(pos.x*8/size,pos.y*8/size);
                ::Color col = chess.squareOccupied(square);
				if ((whiteMove && col==White) || (!whiteMove && col==Black) || selPos!=Coord(-1,-1)) {
					if (selPos==Coord(-1,-1)) {
						selPos = square;
						moves = chess.getLegalMoves(selPos);
                        sleep(milliseconds(250));
					}
					else {
						::Color moveCol = whiteMove?(White):(Black);
						if (!chess.makeMove(moveCol,selPos,square))
							badMove.play();
						else {
							whiteMove = !whiteMove;
							if (chess.inStalemate(White) || chess.inStalemate(Black)) {
								done = true;
								window.setTitle("Chess | DRAW");
							}
							else if (chess.inCheckmate(White)) {
								done = true;
								window.setTitle("Chess | Black WINS!");
							}
							else if (chess.inCheckmate(Black)) {
								done = true;
								window.setTitle("Chess | White WINS!");
							}
							else {
								if (whiteMove)
									window.setTitle("Chess | White to move");
								else
									window.setTitle("Chess | Black to move");
							}
						}
						selPos = Coord(-1,-1);
						moves.clear();
                        sleep(milliseconds(250));
					}
				}
			}
		}
		if (!whiteMove && natDelay>33 && !done) {
			natDelay = 0;
			whiteMove = true;
			pair<Coord,Coord> mv = ai.getRandomMove();
			chess.makeMove(Black,mv.first,mv.second,Queen);
		}

		bool drawBlack = false;
		Board temp[2] = {chess.getPieces(White),chess.getPieces(Black)};
		for (int x = 0; x<8; ++x) {
			for (int y = 0; y<8; ++y) {
				whiteSquare.setPosition(x*size/8,y*size/8);
				blackSquare.setPosition(x*size/8,y*size/8);
				highlight.setPosition(x*size/8,y*size/8);
				if (drawBlack)
					window.draw(blackSquare);
				else
					window.draw(whiteSquare);
				if (Coord(x,y)==selPos)
					window.draw(highlight);
				drawBlack = !drawBlack;

				for (int i = 0; i<2; ++i) {
                    if (temp[i].pieces[x][y]!=Empty) {
						pieceImages[i][temp[i].pieces[x][y]].setPosition(x*size/8,y*size/8);
						window.draw(pieceImages[i][temp[i].pieces[x][y]]);
                    }
				}
                for (unsigned int i = 0; i<moves.size(); ++i) {
					if (moves[i].x==x && moves[i].y==y) {
						dot.setPosition(x*size/8+size/32,y*size/8+size/32);
						window.draw(dot);
					}
                }
			}
			drawBlack = !drawBlack;
		}
		window.display();

		sleep(milliseconds(30));
		if (!whiteMove)
			natDelay++;
	}

	return 0;
}
