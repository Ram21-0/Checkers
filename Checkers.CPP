#include <iostream.h>
#include <conio.h>
#include <graphics.h>

#define RADIUS 20

int color1 = LIGHTRED, color2 = CYAN, highlightColor = LIGHTBLUE;
int turn = 0;
int started = 0;
int true = 1, false = 0;

// int player0wins = false, player1wins = false, noMoreMoves = false;
/*
 * Point: Pixel coordinates (x,y)
 * IndexPoint: Actual Positions of the board - indexes (i,j) : 8 x 8
 * Cursor - the pointer which stores indexing on board
*/

struct Point {
	int x,y;
	Point() {}
	Point(int xx,int yy): x(xx), y(yy) {}
};

class IndexPoint {
	public:
		int x, y, highlighted, player, isKing, out; // player - 0: p0, 1: p1, -1: empty
		IndexPoint() {};
		IndexPoint(int xx,int yy): x(xx), y(yy), highlighted(false), player(-1), isKing(false), out(false) {}
		
		int getColor() {
			if(player == 0) return color1;
			else return color2;
		}
		
		int xCoordinate() { return 75 + 50*x; }
		int yCoordinate() { return 75 + 50*y; }
		Point coordinates() { 
			Point p; 
			p.x = xCoordinate(); 
			p.y = yCoordinate();
			return p;
		}
		
		/*
		 * highlights box with cc
		 * normally with highlightcolor
		 * otherwise with passed value (for options)
		 */
		void highlight(int cc = highlightColor,int box = true) {
			Point c = coordinates(),
				A(c.x-25,c.y-25),
				B(c.x-25,c.y+25),
				C(c.x+25,c.y+25),
				D(c.x+25,c.y-25);
			setcolor(cc);
			int width = box ? 3.5 : 3.5;
			setlinestyle(0,0,width);
			line(A.x,A.y,B.x,B.y);
			line(B.x,B.y,C.x,C.y);
			line(C.x,C.y,D.x,D.y);
			line(D.x,D.y,A.x,A.y);
			setlinestyle(0,0,1);
			setcolor(15);
		}
		void unhighlight(int cc = highlightColor) {
			setwritemode(XOR_PUT);
			highlight(cc);
			setwritemode(COPY_PUT);
		}

		/* 
		 * if del = true -> deletes crown
		 * else draws crown
		 */
		void drawCrown(int del = false) {
			Point c = coordinates();
			Point A(c.x+12,c.y+20),
				B(c.x-12,c.y+20),
				C(c.x-22,c.y-20),
				D(c.x-8,c.y-5),
				E(c.x,c.y-17),
				F(c.x+8,c.y-5),
				G(c.x+22,c.y-20);
			
			int col = del ? BLACK : getColor();
			setfillstyle(SOLID_FILL, col);

			line(A.x,A.y,B.x,B.y);
			line(B.x,B.y,C.x,C.y);
			line(C.x,C.y,D.x,D.y);
			line(D.x,D.y,E.x,E.y);
			line(E.x,E.y,F.x,F.y);
			line(F.x,F.y,G.x,G.y);
			line(G.x,G.y,A.x,A.y);

			floodfill(c.x,c.y,255);

			// only to make it disappear lol
			setcolor(BLACK);
			line(A.x,A.y,B.x,B.y);
			line(B.x,B.y,C.x,C.y);
			line(C.x,C.y,D.x,D.y);
			line(D.x,D.y,E.x,E.y);
			line(E.x,E.y,F.x,F.y);
			line(F.x,F.y,G.x,G.y);
			line(G.x,G.y,A.x,A.y);
			setcolor(WHITE);
		}

		/* 
		 * if del = true -> deletes circle
		 * else draws circle
		 */
		void drawCircle(int del=false) {
			Point p = coordinates();
			int col = del ? BLACK : getColor();
			if(!isKing) {
				setfillstyle(SOLID_FILL, col);
				circle(p.x,p.y,RADIUS);
				floodfill(p.x,p.y,255);

				// only to make it disappear lol
				setcolor(BLACK);
				circle(p.x,p.y,RADIUS);
				setcolor(WHITE);
			}
			else drawCrown(del);
		}

		void undraw() {
			setfillstyle(SOLID_FILL,BLACK);
			// highlight(CYAN,true);
			drawCircle(true);
			highlight(BLACK,true);
			// setcolor(WHITE);
		}
};

// Point pp0[12], pp1[12];

IndexPoint board[8][8];
IndexPoint selected(-1,-1);

class Cursor {
	public:
		int x,y; 
		Cursor(): x(1), y(0) {}
		Cursor(int xx,int yy): x(xx), y(yy) {}
		
		int up() { 
			if(y == 0) return 0;
			board[x][y].unhighlight();
			if(board[x][y].highlighted) board[x][y].highlight(YELLOW);
			y--;
			if(x > 0) x--; else x++;
			board[x][y].highlight();
			return 1;
		}
		int down() { 
			if(y == 7) return 0;
			board[x][y].unhighlight();
			if(board[x][y].highlighted) board[x][y].highlight(YELLOW);
			y++; 
			if(x > 0) x--; else x++;
			board[x][y].highlight();
			return 1;
		}
		int left() { 
			if(x <= 1) return 0;
			board[x][y].unhighlight();
			if(board[x][y].highlighted) board[x][y].highlight(YELLOW);
			x -= 2; 
			board[x][y].highlight();
			return 1;
		}
		int right() { 
			if(x >= 6) return 0;
			board[x][y].unhighlight();
			if(board[x][y].highlighted) board[x][y].highlight(YELLOW);
			x += 2; 
			board[x][y].highlight();
			return 1;
		}

		void unhighlightOptions() {
			for(int i=0;i<8;i++) {
				for(int j=0;j<8;j++) {
					if(board[i][j].highlighted) {
						board[i][j].unhighlight(YELLOW);
						board[i][j].highlighted = false;
					}
				}
			}
		}

		void options() {

			if(x > 0 && y > 0 && (board[x][y].isKing || board[x][y].player == 1)) {
				selected = IndexPoint(x,y);
				if(board[x-1][y-1].player == -1) {
					board[x-1][y-1].highlight(YELLOW);
					board[x-1][y-1].highlighted = true;
				}
				else if(board[x-1][y-1].player != board[x][y].player && x > 1 && y > 1
					&& board[x-2][y-2].player == -1) {
					board[x-2][y-2].highlight(YELLOW);
					board[x-2][y-2].highlighted = true;
				}
			}
			if(x < 7 && y < 7 && (board[x][y].isKing || board[x][y].player == 0)) {
				selected = IndexPoint(x,y);
				if(board[x+1][y+1].player == -1) {
					board[x+1][y+1].highlight(YELLOW);
					board[x+1][y+1].highlighted = true;
				}
				else if(board[x+1][y+1].player != board[x][y].player && x < 6 && y < 6
					&& board[x+2][y+2].player == -1) {
					board[x+2][y+2].highlight(YELLOW);
					board[x+2][y+2].highlighted = true;
				}
			}
			if(x > 0 && y < 7 && (board[x][y].isKing || board[x][y].player == 0)) {
				selected = IndexPoint(x,y);
				if(board[x-1][y+1].player == -1) {
					board[x-1][y+1].highlight(YELLOW);
					board[x-1][y+1].highlighted = true;
				}
				else if(board[x-1][y+1].player != board[x][y].player && x > 1 && y < 6
					&& board[x-2][y+2].player == -1) {
					board[x-2][y+2].highlight(YELLOW);
					board[x-2][y+2].highlighted = true;
				}
			}
			if(x < 7 && y > 0 && (board[x][y].isKing || board[x][y].player == 1)) {
				selected = IndexPoint(x,y);
				if(board[x+1][y-1].player == -1) {
					board[x+1][y-1].highlight(YELLOW);
					board[x+1][y-1].highlighted = true;
				}
				else if(board[x+1][y-1].player != board[x][y].player && x < 6 && y > 1
					&& board[x+2][y-2].player == -1) {
					board[x+2][y-2].highlight(YELLOW);
					board[x+2][y-2].highlighted = true;
				}
			}
		}
};

Cursor cursor(1,0);

void resetCoins() {
	for(int i=0;i<4;i++) {
		board[2*i+1][0].player = 0;
		board[2*i][7].player = 1;
	}
	int k = 0;
	for(i=4;i<8;i++) {
		board[k][1].player = 0;
		board[k+1][6].player = 1;
		k += 2;
	}
	k = 0;
	for(i=8;i<12;i++) {
		board[k+1][2].player = 0;
		board[k][5].player = 1;
		k += 2;
	}

	for(i=0;i<8;i++)
		for(int j=0;j<8;j++) 
			if(board[i][j].player != -1) 
				board[i][j].drawCircle();
}

void makeBoard() {

	setfillstyle(SOLID_FILL,RED);
	floodfill(0,0,255);

	for(int i=1,start=i,end=i+8;i<=end;i++) {
		line(start*50,i*50,end*50,i*50);
		line(i*50,start*50,i*50,end*50);
	}
	
	for(i=0;i<8;i++) {
		for(int j=i%2;j<8;j+=2) {
			setfillstyle(SOLID_FILL,WHITE);
			floodfill(75 + i*50,75 + j*50,255);
		}
		for(j=!(i%2);j<8;j+=2) {
			setfillstyle(SOLID_FILL,BLACK);
			floodfill(75 + i*50,75 + j*50,255);
		}
	}

	for(i=0;i<8;i++) {
		for(int j=0;j<8;j++) {
			board[i][j] = IndexPoint(i,j);
		}
	}
}

struct Result {
	int gameOver, winner;
	Result(int go,int w): gameOver(go), winner(w) {}
	Result(int go): gameOver(go) {}
};

Result allLost() {
	int n0 = 0, n1 = 0;
	for(int i=0;i<8;i++) {
		for(int j=0;j<8;j++) {
			if(board[i][j].player == 0) n0++;
			else if(board[i][j].player == 1) n1++;
		}
	}
	if(n1 == 0) return Result(true,0);
	else if(n0 == 0) return Result(true,1);
	return Result(false);
}

int noMoreMoves() {
	for(int x=0;x<8;x++) {
		for(int y=0;y<8;y++) {
			if(board[x][y].player == turn) {
				if(turn == 0) {
					if(x < 7 && y < 7) {
						if(board[x+1][y+1].player == -1) return false;
						if(x < 6 && y < 6 && board[x+1][y+1].player == 1 && board[x+2][y+2].player == -1) return false;
					}
					if(board[x][y].isKing && x < 7 && y > 0) {
						if(board[x+1][y-1].player == -1) return false;
						if(x < 6 && y > 1 && board[x+1][y-1].player == 1 && board[x+2][y-2].player == -1) return false;
					}
					if(board[x][y].isKing && x > 0 && y > 0) {
						if(board[x-1][y-1].player == -1) return false;
						if(x > 1 && y > 1 && board[x-1][y-1].player == 1 && board[x-2][y-2].player == -1) return false;
					}
					if(x > 0 && y < 7) {
						if(board[x-1][y+1].player == -1) return false;
						if(x > 1 && y < 7 && board[x-1][y+1].player == 1 && board[x-2][y+2].player == -1) return false;
					}
				}
				else {
					if(board[x][y].isKing && x < 7 && y < 7) {
						if(board[x+1][y+1].player == -1) return false;
						if(x < 6 && y < 6 && board[x+1][y+1].player == 0 && board[x+2][y+2].player == -1) return false;
					}
					if(x < 7 && y > 0) {
						if(board[x+1][y-1].player == -1) return false;
						if(x < 6 && y > 1 && board[x+1][y-1].player == 0 && board[x+2][y-2].player == -1) return false;
					}
					if(x > 0 && y > 0) {
						if(board[x-1][y-1].player == -1) return false;
						if(x > 1 && y > 1 && board[x-1][y-1].player == 0 && board[x-2][y-2].player == -1) return false;
					}
					if(board[x][y].isKing && x > 0 && y < 7) {
						if(board[x-1][y+1].player == -1) return false;
						if(x > 1 && y < 7 && board[x-1][y+1].player == 0 && board[x-2][y+2].player == -1) return false;
					}
				}
			}
		}
	}
	return true;
}

void displayResult(const char* str) {
	settextstyle(4,0,4);
	outtextxy(50,0,"                     ");
	outtextxy(50,0,str);
}
void go() {
	board[1][0].highlight();
	while(1) {
		Result result = allLost();
		if(result.gameOver && result.winner == 1) {
			displayResult("Player 2 wins!");
			return;
		}
		if(result.gameOver && result.winner == 0) {
			displayResult("Player 1 wins!");
			return;
		}
		if(noMoreMoves()) {
			int n1 = 0, n2 = 0;
			for(int i=0;i<8;i++) {
				for(int j=0;j<8;j++) {
					if(board[i][j].player == 0) n1++;
					else if(board[i][j].player == 1) n2++;
				}
			}
			if(n1 > n2) displayResult("Player 1 wins!");
			else if(n1 < n2) displayResult("Player 2 wins!");
			else displayResult("Draw game - no more moves!!!");
			return;
		}
		
		if(kbhit()) {
			int inp = (int) getch();
			int ok = 0;
			switch(inp) {
				case 72: ok = cursor.up();
						 break;
				case 80: ok = cursor.down();
						 break;
				case 75: ok = cursor.left(); 
						 break;
				case 77: ok = cursor.right();
					     break;
				case 13: if(board[cursor.x][cursor.y].highlighted) {
							 cursor.unhighlightOptions();
							 board[selected.x][selected.y].undraw();
							 board[cursor.x][cursor.y].player = board[selected.x][selected.y].player;
							 board[cursor.x][cursor.y].isKing = board[selected.x][selected.y].isKing;
							 board[selected.x][selected.y] = IndexPoint(selected.x,selected.y);
							 if(board[cursor.x][cursor.y].player == 0 && cursor.y == 7) {
							 	board[cursor.x][cursor.y].isKing = true;
							 }
							 else if(board[cursor.x][cursor.y].player == 1 && cursor.y == 0) {
							 	board[cursor.x][cursor.y].isKing = true;
							 }

							 board[cursor.x][cursor.y].drawCircle();
							 board[cursor.x][cursor.y].unhighlight(YELLOW);

							 int jumpX = selected.x - cursor.x, jumpY = selected.y - cursor.y;
							 if(jumpX == 2 || jumpX == -2) {
							 	int remX = (cursor.x + selected.x)/2;
							 	int remY = (cursor.y + selected.y)/2;
							 	board[remX][remY].player = -1;
							 	board[remX][remY].undraw();
							 }

							 turn = !turn;
							 break;
						 }
						 cursor.unhighlightOptions();
						 if(turn == board[cursor.x][cursor.y].player) {
						 	 cursor.options();
						 }
						 break;
				case 27: return;
			}
		}
	}
}

void intro() {
	setfillstyle(SOLID_FILL,BLACK);
	floodfill(0,0,255);

	settextstyle(1,0,8.5);
	setcolor(WHITE);
	outtextxy(100,100,"CHECKERS");

	setcolor(YELLOW);
	settextstyle(5,0,4);
	outtextxy(400,250,"V Sriram");
	outtextxy(400,300,"Tanmaya Yadav");
	
	setcolor(WHITE);
	settextstyle(0,0,2);
	outtextxy(120,400,"Press any key to start");
	getch();
}

void rules() {
	cleardevice();
	setcolor(LIGHTRED);
	settextstyle(4,0,5);
	outtextxy(250,50,"Rules");
	
	setcolor(WHITE);
	settextstyle(0,0,1.5);
	outtextxy(50,140,"1. Each piece can move only in the forward diagonal directions");
	outtextxy(50,170,"2. A piece can jump over an opponent and eliminate it");
	outtextxy(50,200,"3. A piece becomes a king on reaching the opposite end. A king can move");
	outtextxy(75,220,"backwards as well!");
	outtextxy(50,250,"4. The player with no coins left loses the game");
	outtextxy(50,280,"5. The player with more pieces wins if no moves are possible");

	setcolor(CYAN);
	settextstyle(0,0,2);
	outtextxy(120,400,"Press any key to start");
	setcolor(WHITE);
	getch();
}

void playGame() {
	cleardevice();
	makeBoard();
	resetCoins();
	go();
}

void main() {
	clrscr();
	int gd = DETECT, gm;
	initgraph(&gd,&gm,"c:\\turboc3\\bgi");

	intro();
	rules();
	playGame();

	getch();
}