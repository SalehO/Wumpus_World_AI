
#ifndef MYAI_LOCK
#define MYAI_LOCK

#include "Agent.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <queue>
#include <map>
#include <stack>
#include <unordered_set>
#include <limits>

using namespace std;

class MyAI : public Agent
{
public:
	MyAI ( void );
	
	Action getAction
	(
		bool stench,
		bool breeze,
		bool glitter,
		bool bump,
		bool scream
	);
	

private: 
	struct searchNode
	{
		int x, y, gx, gy, px, py, g, h, f;
		size_t dir, pdir;

		searchNode(int _x, int _y, int _gx, int _gy,int _px, int _py, int _g, size_t _dir, size_t _pdir)
		{
			x = _x;
			y = _y;
			gx = _gx;
			gy = _gy;
			px = _px;
			py = _py;
			g = _g;
			dir = _dir;
			pdir = _pdir;
			if(px == -1)
			{
				costToThisTile();
			}
			else
				g = 0;
			manhattanDistance();
			f = h+g;
		}
		searchNode()
		{
			x = -1; y = -1; gx = -1; gy = -1; g = -1; h = -1; f = numeric_limits<int>::max();
			px = -1;
			py = -1;
			dir = 0;
			pdir = 0;
		}
		searchNode& operator=(searchNode n)
		{
			x = n.x;
			y = n.y;
			gx = n.gx;
			gy = n.gy;
			px = n.px;
			py = n.py;
			g = n.g;
			h = n.h;
			f = n.f;
			dir = n.dir;
			pdir = n.pdir;
			return *this;
		}
		bool operator==(searchNode other) const
		{
			if(x != other.x)
				return false;
			if(y != other.y)
				return false;
			return true;
		}
		void manhattanDistance()
		{
			h = abs(x - gx) + abs(y - gy);
		}
		void costToThisTile()
		{
			//cost going left
			int temp = dir;
			int left = 0;
			int right = 0;
			while(temp != pdir)
			{
				++left;
				if(--temp > 3)
					temp = 3;
			}
			temp = dir;
			while(temp != pdir)
			{
				++right;
				if(++temp > 3)
					temp = 0;
			}
			if(right <= left)
				g += right;
			else
				g += left;
			++g;//1 more for cost to move foward
		}
	};

	struct Node
	{
		int x;
		int y;
		int prevX;
		int prevY;
		Node(int _x, int _y, int x1, int y1)
		{
			x = _x;
			y = _y;
			prevX = x1;
			prevY = y1;
		}
		bool operator==(Node other) const
		{
			if(x != other.x)
				return false;
			if(y != other.y)
				return false;
			if(prevX != other.prevX)
				return false;
			if(prevY != other.prevY)
				return false;
			return true;
		}
	};
	struct OpenList
	{
		vector<searchNode> pv;
		int front;
		int size;

		OpenList(){front = -1; size = 0;}

		void push(searchNode n)
		{
			++size;
			pv.emplace_back(n);
			if(front != -1)
			{
				if(pv[front].f > pv[pv.size() - 1].f)
					front = size - 1;
			}
			else
			{
				front = 0;
				for(int i = 1; i < pv.size(); ++i)
				{
					if(pv[front].f > pv[i].f)
						front = i;
				}
			}
		}

		void pop()
		{
			if(size == 0)
				return;
			pv.erase(pv.begin() + front);
			--size;
			if(size != 0)
			{
				front = 0;
				for(int i = 1; i < pv.size(); ++i)
				{
					if(pv[front].f > pv[i].f)
						front = i;
				}
			}
			else
				front = -1;
		}

		searchNode top()
		{
			////cout <<"Top: size: "<< size << "front: " << front << endl;
			if(size == 0)
				return searchNode();
			return pv[front];

		}

		int contains(searchNode n)
		{
			for(int i = 0; i < pv.size(); ++i)
			{
				if(n == pv[i])
					return i;
			}
			return -1;
		}

		bool replace_if(searchNode n)
		{
			int index = contains(n);
			if(index == -1)
				return false;
			if(n.f  < pv[index].f)
			{
				pv[index] = n;
			}
			return true;
		}

		bool empty()
		{
			return size == 0;
		}
	};

	struct ClosedList
	{
		vector<searchNode> v;

		ClosedList(){}

		void push(searchNode n)
		{
			v.emplace_back(n);
		}

		bool contains(searchNode n)
		{
			for(int i = 0; i < v.size(); ++i)
			{
				if(n == v[i])
					return true;
			}
			return false;
		}
	};

	struct Move
	{
		double cost;
		queue<Action> actions;
		Move()
		{
			cost = 0;
		}
		bool operator<(Move other) const
		{
			return cost > other.cost;
		}
		void pushAction(Action act)
		{
			actions.push(act);
		}
		Action nextAction()
		{
			if(!actions.empty())
			{
				Action ret = actions.front();
				actions.pop();
				return ret;
			}
			else
			{
				////cout << "error no moves" << endl;
				return CLIMB;
			}
		}
		bool empty()
		{
			return actions.empty();
		}
		void setCost(int c)
		{
			cost = c;
		}
	};

	struct tile
	{
		bool wumpus;
		bool realWumpus;
		bool realPitfall;
		bool canBeWumpus;
		bool pitfall;
		bool canBePtifall;
		bool is_stench;
		bool is_breeze;
		bool visited;
		bool good;
		bool updated;
		tile()
		{
			wumpus = false;
			canBeWumpus = false;
			realWumpus = false;
			pitfall = false;
			canBePtifall = false;
			is_stench = false;
			is_breeze = false;
			visited = false;
			good = false;
			updated = false;
		}
		bool isGood()
		{
			return good;
		}
		bool isVisited(){return visited;}
		bool isUpdated(){return updated;}
		void setUpdated(bool u){updated = u;}
		bool isPitfall(){return pitfall;}
		bool isWumpus(){return wumpus;}
		bool isRealWumpus(){return realWumpus;}
		void setRealWumpus(bool w){ realWumpus = w;}
		bool canWumpus(){return canBeWumpus;}
		bool canPitfall(){return canBePtifall;}
		bool isStench(){return is_stench;}
		bool isBreeze(){return is_breeze;}
		void setVisited(bool u){visited = u;}
		void setPitfall(bool p){pitfall = p;}
		void setWumpus(bool w){wumpus = w;}
		void setCanBeWumpus(bool cbw)
		{
			if(!good && !updated)
				canBeWumpus = cbw;
		}
		void setCanBePitfall(bool cbp)
		{
			if(!good && !updated)
				canBePtifall = cbp;
		}
		void setStench(bool s){is_stench = s;}
		void setBreeze(bool b){is_breeze = b;}
		void setGood(bool g)
		{
			if(g)
			{
				canBePtifall = false;
				if(!realWumpus)
					canBeWumpus = false;
			}
			if(!realWumpus)
				good = g;
		}
	};
	struct Player
	{
		int x;
		int y;
		size_t direction;
		int destX;
		int destY;
		Player()
		{
			x = 0;
			y = 0;
			direction = 0;
			destX = 0;
			destY = 0;
		}
		int getX(){return x;}
		int getY(){return y;}
		void moveDown(){--y;}
		void moveUp(){++y;}
		void moveRight(){++x;}
		void moveLeft(){--x;}
		size_t getDir(){return direction;}
		void turnRight()
		{
			if(++direction > 3)
				direction = 0;
		}
		void turnLeft()
		{
			if(--direction > 3)
				direction = 3;
		}
		void goForward()
		{
			if(direction == 0)//move right
			{
				moveRight();
			}
			else if(direction == 1)//move down
			{
				moveDown();
			}
			else if(direction == 2)//move left
			{
				moveLeft();
			}
			if(direction == 3)//move up
			{
				moveUp();
			}
		}
	};
	struct node_hash
	{
    	inline std::size_t operator()(const Node & n) const 
    	{
        	return (n.x + n.prevX)*31+(n.y + n.prevY);
    	}
	};
	struct World
	{
		int xLimit;
		int yLimit;
		Action lastMove;
		bool gold;
		bool exit;
		bool foundWumpus;
		bool killedWumpus;
		bool killingWumpus;
		bool has_arrow;
		int wX;
		int wY;
		vector<vector<tile>> Tiles{7, vector<tile>(7, tile())};
		queue<Action> actions;
		priority_queue<Move, vector<Move>, less<vector<Move>::value_type>> goodMoves;
		Move moves;
		Player player;
		map<pair<int, int>, int> bestDir;
		World()
		{
			foundWumpus = false;
			killedWumpus = false;
			killingWumpus = false;
			has_arrow = true;
			wX = -1;
			wY = -1;
			xLimit = -1;
			yLimit = -1;
			lastMove = CLIMB;
			gold = false;
			exit = false;
			bestDir[make_pair(0, 0)] = -1;
			bestDir[make_pair(0, 1)] = 0;
			bestDir[make_pair(0, 2)] = 0;
			bestDir[make_pair(0, 3)] = 1;
			bestDir[make_pair(1, 0)] = 1;
			bestDir[make_pair(1, 1)] = -1;
			bestDir[make_pair(1, 2)] = 0;
			bestDir[make_pair(1, 3)] = 0;
			bestDir[make_pair(2, 0)] = 1;
			bestDir[make_pair(2, 1)] = 1;
			bestDir[make_pair(2, 2)] = -1;
			bestDir[make_pair(2, 3)] = 0;
			bestDir[make_pair(3, 0)] = 0;
			bestDir[make_pair(3, 1)] = 1;
			bestDir[make_pair(3, 2)] = 1;
			bestDir[make_pair(3, 3)] = -1;
		}
		Player getPlayer(){return player;}
		void print()
		{
			int i = 6;	
			int jlimit = 7;
			if(yLimit != -1)
				i = yLimit -1;
			if(xLimit != -1)
				jlimit = xLimit;
			for(; i > -1; --i)
			{
				for(int j = 0; j < jlimit; ++j)
				{
					bool known = false;
					if(Tiles[j][i].canWumpus())
					{
						known = true;
						//cout << "W?";
					}
					if(Tiles[j][i].isGood())
					{
						known = true;
						//cout << ".";
					}
					if(Tiles[j][i].canPitfall())
					{
						known = true;
						//cout << "P?";
					}
					if(Tiles[j][i].isVisited())
					{
						known = true;
						//cout << "x";
					}
					if(Tiles[j][i].isUpdated())
					{
						known = true;
						//cout << "!";
					}
					if(i == player.getY() && j == player.getX())
					{
						known = true;
						//cout << "@";
					}
					if(!known)
					{}
						//cout << "?";
					//cout << "	";
				}
				//cout << endl << endl;
			}
		}
		bool checkUp(int x, int y)
		{
			return (yLimit == -1 && y < 6) || (yLimit != -1 && y < yLimit - 1);
		}
		bool checkRight(int x, int y)
		{
			return (xLimit == -1 && x < 6) || (xLimit != -1 && x < xLimit - 1);
		}
		bool checkLeft(int x, int y)
		{
			return x > 0;
		}
		bool checkDown(int x, int y)
		{
			return y > 0;
		}
		void updateWorld(bool& s, const bool& b, const bool& g, const bool& bump, const bool& sc)
		{
			////cout << "updateWorld" << endl;
			checkLimits(bump);
			if(g)
			{
				Move m;
				m.pushAction(GRAB);
				exit = true;
				moves = m;
			}
			if(sc)
			{
				killedWumpus = true;
				wX = -1;
				wY = -1;
				markWumpus();
			}
			if(!has_arrow && !killedWumpus)
			{
				foundWumpus = true;
				wX = 0;
				wY = 1;
				markWumpus();
			}
			if(killedWumpus)
				s = false;
			if(!b && s && player.x == 0 && player.y == 0 && has_arrow)
			{
				Move m;
				m.pushAction(SHOOT);
				moves = m;
				has_arrow = false;
			}
			if(!Tiles[player.getX()][player.getY()].isVisited())
			{
				Tiles[player.getX()][player.getY()].setStench(s);
				Tiles[player.getX()][player.getY()].setBreeze(b);
				Tiles[player.getX()][player.getY()].setGood(true);
				Tiles[player.getX()][player.getY()].setVisited(true);
				markLeftTile(s, b);
				markRightTile(s, b);
				markTopTile(s, b);
				markBottomTile(s, b);
				Tiles[player.getX()][player.getY()].setUpdated(true);
			}
			clearTiles();
			////cout << "clearedTiles" << endl;
		}
		void markLeftTile(const bool& s, const bool& b)
		{
			if(!b && ! s)
			{
				if(checkLeft(player.getX(), player.getY()))
					Tiles[player.getX()-1][player.getY()].setGood(true);
			}
			else
			{
				if(checkLeft(player.getX(), player.getY()))
				{
					if(!foundWumpus)
						Tiles[player.getX()-1][player.getY()].setCanBeWumpus(s);
					Tiles[player.getX()-1][player.getY()].setCanBePitfall(b);		
				}
			}
			if(checkLeft(player.getX(), player.getY()))
			{
				Tiles[player.getX()-1][player.getY()].setUpdated(true);
			}
		}
		void markRightTile(const bool& s, const bool& b)
		{
			if(!b && ! s)
			{
				if(checkRight(player.getX(), player.getY()))
					Tiles[player.getX()+1][player.getY()].setGood(true);
			}
			else
			{
				if(checkRight(player.getX(), player.getY()))
				{
					if(!foundWumpus)
						Tiles[player.getX()+1][player.getY()].setCanBeWumpus(s);
					Tiles[player.getX()+1][player.getY()].setCanBePitfall(b);		
				}
			}
			if(checkRight(player.getX(), player.getY()))
			{
				Tiles[player.getX()+1][player.getY()].setUpdated(true);
			}
		}
		void markTopTile(const bool& s, const bool& b)
		{	
			if(!b && ! s)
			{
				if(checkUp(player.getX(), player.getY()))
					Tiles[player.getX()][player.getY()+1].setGood(true);
			}
			else
			{
				if(checkUp(player.getX(), player.getY()))
				{
					if(!foundWumpus)
						Tiles[player.getX()][player.getY()+ 1].setCanBeWumpus(s);
					Tiles[player.getX()][player.getY()+ 1].setCanBePitfall(b);		
				}
			}
			if(checkUp(player.getX(), player.getY()))
			{
				Tiles[player.getX()][player.getY()+1].setUpdated(true);
			}
		}
		void markBottomTile(const bool& s, const bool& b)
		{
			if(!b && ! s)
			{
				if(checkDown(player.getX(), player.getY()))
					Tiles[player.getX()][player.getY()-1].setGood(true);
			}
			else
			{
				if(checkDown(player.getX(), player.getY()))
				{
					if(!foundWumpus)
						Tiles[player.getX()][player.getY() - 1].setCanBeWumpus(s);
					Tiles[player.getX()][player.getY() - 1].setCanBePitfall(b);		
				}
			}
			if(checkDown(player.getX(), player.getY()))
			{
				Tiles[player.getX()][player.getY()-1].setUpdated(true);
			}
		}
		bool isMakingMove()
		{
			return !actions.empty();
		}
		void checkLimits(const bool& bump)
		{
			if(lastMove == FORWARD && (player.getDir() == 0 || player.getDir() == 3) && bump)//reset x or y and set x limit
			{
				if(player.getDir() == 0)
				{
					xLimit = player.getX();
					player.moveLeft();
				}
				else
				{
					yLimit = player.getY();
					player.moveDown();
				}
			}
		}
		void decideNextMove()
		{
			////cout << "First for loop" << endl;
			if(exit)
			{
				////cout << "lets exit we got gold" << endl;
				moves = getMoves(0 , 0);
			}
			else
			{
				for(int i = 0; i < 7; ++i)
				{
					if(xLimit == -1 || i < xLimit)
					{
						for(int j = 0; j < 7; ++j)
						{
							if(yLimit == -1 || j < yLimit)
							{
								if((!Tiles[i][j].isVisited() && Tiles[i][j].isGood()))
								{
									////cout << "about to push" << endl;
									goodMoves.push(getMoves(i, j));
									////cout << "Finished push" << endl;
								}
							}
						}
					}
				}
				////cout << "out first for loop" << endl;
				if(!goodMoves.empty())//goodMoves left and we not trying to exit
				{
					////cout << "goodMoves" << endl;
					moves = goodMoves.top();
					while(!goodMoves.empty())
						goodMoves.pop();
				}
				else//no good moves exit
				{
					////cout << "no goodMoves" << endl;
					if(!killedWumpus && has_arrow && foundWumpus && !Tiles[wX][wY].canPitfall())//HEREEEEEEREEREREE
					{
						////cout << "Kill The Wumpus!" << endl;
						killingWumpus = true;
						////cout << "wX: " << wX << " wY: " << wY << endl;
						Tiles[wX][wY].setRealWumpus(true);
						moves = getMoves(wX, wY);
						killedWumpus = true;
						has_arrow = false;
					}
					else
					{
						exit = true;
						if(player.getX() == 0 && player.getY() == 0)
						{
							Move m;
							m.pushAction(CLIMB);
							moves = m;
						}
						else
							moves = getMoves(0 , 0);
					}
				}
			}
		}
		Move getMoves(const int dX,const int dY)
		{
			////cout << "dX: " << dX << " dY: " << dY  << endl;
			// int tempX = player.getX();
			// int tempY = player.getY();
			// size_t tempDir = player.getDir();
			// Move ret;
		 // 	int cost = 0;
			// unordered_set<Node, node_hash> visited;
			// queue<Node> frontier;
			// stack<Node> path;
			// stack<pair<int, int>> realPath;
			// frontier.push(Node(tempX, tempY, -1, -1));
			// while(!frontier.empty())
			// {
			// 	////cout << "in while" << endl;
			// 	Node currentTile = frontier.front();
			// 	frontier.pop();
			// 	visited.emplace(currentTile);
			// 	path.push(currentTile);
			// 	if(checkRight(currentTile.x, currentTile.y) && ((Tiles[currentTile.x +1][currentTile.y].isVisited() || Tiles[currentTile.x+1][currentTile.y].isGood()) || (Tiles[currentTile.x +1][currentTile.y].isRealWumpus() && killingWumpus)))//check if good tile on right 
			// 	{
			// 		{
			// 			Node n(currentTile.x + 1, currentTile.y, currentTile.x, currentTile.y);
			// 			if(n.x == dX && n.y == dY)//goal test
			// 			{
			// 				////cout << "right" << endl;
			// 				path.push(n);
			// 				break;
			// 			}
			// 			else//else push onto frontier
			// 				frontier.push(n);

			// 		}
			// 	}
			// 	if(checkLeft(currentTile.x, currentTile.y) && ((Tiles[currentTile.x - 1][currentTile.y].isVisited() || Tiles[currentTile.x - 1][currentTile.y].isGood()) ||  (Tiles[currentTile.x - 1][currentTile.y].isRealWumpus() && killingWumpus)))//check if good tile on right 
			// 	{
			// 		////cout << "left-> x: " << currentTile.x -1 << " y: " << currentTile.y << endl;
			// 		//if(currentTile.x != tempX || currentTile.y != tempY)//if tile is not first tile again
			// 		{
			// 			Node n(currentTile.x - 1, currentTile.y, currentTile.x, currentTile.y);
			// 			if(n.x == dX && n.y == dY)//goal test
			// 			{
			// 				path.push(n);
			// 				break;
			// 			}
			// 			else//else push onto frontier
			// 				frontier.push(n);
			// 		}
			// 	}
			// 	if(checkDown(currentTile.x, currentTile.y) && ((Tiles[currentTile.x][currentTile.y-1].isVisited() || Tiles[currentTile.x ][currentTile.y - 1].isGood()) ||  (Tiles[currentTile.x][currentTile.y- 1].isRealWumpus() && killingWumpus)))//check if good tile on right 
			// 	{
			// 		////cout << "down-> x: " << currentTile.x << " y: " << currentTile.y -1 << endl;
			// 		//if(currentTile.x != tempX || currentTile.y != tempY)//if tile is not first tile again
			// 		{
			// 			Node n(currentTile.x, currentTile.y - 1, currentTile.x, currentTile.y);
			// 			if(n.x == dX && n.y == dY)//goal test
			// 			{
			// 				path.push(n);
			// 				break;
			// 			}
			// 			else//else push onto frontier
			// 				frontier.push(n);
			// 		}
			// 	}
			// 	if(checkUp(currentTile.x, currentTile.y) && ((Tiles[currentTile.x][currentTile.y+1].isVisited() || Tiles[currentTile.x][currentTile.y+1].isGood()) ||  (Tiles[currentTile.x][currentTile.y+1].isRealWumpus() && killingWumpus)))//check if good tile on right 
			// 	{
			// 		////cout << "up-> x: " << currentTile.x << " y: " << currentTile.y + 1 << endl;
			// 		//if(currentTile.x != tempX || currentTile.y != tempY)//if tile is not first tile again
			// 		{
			// 			Node n(currentTile.x, currentTile.y + 1, currentTile.x, currentTile.y);
			// 			if(n.x == dX && n.y == dY)//goal test
			// 			{
			// 				////cout << "up" << endl;
			// 				path.push(n);
			// 				break;
			// 			}
			// 			else//else push onto frontier
			// 				frontier.push(n);
			// 		}
			// 	}
			// }
			// if(!path.empty())//found a path
			// {
			// 	////cout << "We found a path" << endl;
			// 	int pX = path.top().prevX;
			// 	int pY = path.top().prevY;
			// 	realPath.push(make_pair(path.top().x , path.top().y));
			// 	path.pop();
			// 	while(!path.empty())
			// 	{
			// 		if(path.top().x == pX && path.top().y == pY)
			// 		{
			// 			pX = path.top().prevX;
			// 			pY = path.top().prevY;	
			// 			realPath.push(make_pair(path.top().x, path.top().y));
			// 			path.pop();
			// 		}
			// 		else
			// 			path.pop();
			// 	}
			// }
			// else
			// 	//cout << "error no path found!" << endl;
			// while(!realPath.empty())
			// {
			// 	if(realPath.top().first == tempX && realPath.top().second == tempY)
			// 	{
			// 		////cout << "pop current tile and get it out the way" << endl;
			// 		realPath.pop();
			// 	}
			// 	else
			// 	{
			// 		if(realPath.top().first == tempX)//same row
			// 		{
			// 			if(realPath.top().second > tempY)//Up
			// 			{
			// 				////cout << "going up" << endl;
			// 				while(tempDir != 3)
			// 				{
			// 					if(bestDir[make_pair(tempDir, 3)] == 0)// turn right
			// 					{
			// 						ret.pushAction(TURN_RIGHT);
			// 						if(++tempDir > 3) 
			// 							tempDir = 0;
			// 						++cost;
			// 					}
			// 					else//turn left
			// 					{
			// 						ret.pushAction(TURN_LEFT);
			// 						if(--tempDir > 3) 
			// 							tempDir = 3;
			// 						++cost;
			// 					}
			// 				}
			// 				++tempY;
			// 				if(killingWumpus && tempY == dY && tempX == dX)
			// 				{
			// 					killingWumpus = false;
			// 					killedWumpus = true;
			// 					wX = -1;
			// 					wY = -1;
			// 					markWumpus();
			// 					ret.pushAction(SHOOT);
			// 				}
			// 				else
			// 					ret.pushAction(FORWARD);
			// 				++cost;
			// 			}
			// 			else//down
			// 			{
			// 				////cout << "going down" << endl;
			// 				while(tempDir != 1)
			// 				{
			// 					if(bestDir[make_pair(tempDir, 1)] == 0)// turn right
			// 					{
			// 						ret.pushAction(TURN_RIGHT);
			// 						if(++tempDir > 3) 
			// 							tempDir = 0;
			// 						++cost;
			// 					}
			// 					else//turn left
			// 					{
			// 						ret.pushAction(TURN_LEFT);
			// 						if(--tempDir > 3) 
			// 							tempDir = 3;
			// 						++cost;
			// 					}
			// 				}
			// 				--tempY;
			// 				if(killingWumpus && tempY == dY && tempX == dX)
			// 				{
			// 					killingWumpus = false;
			// 					killedWumpus = true;
			// 					wX = -1;
			// 					wY = -1;
			// 					markWumpus();
			// 					ret.pushAction(SHOOT);
			// 				}
			// 				else
			// 					ret.pushAction(FORWARD);
			// 				++cost;
			// 			}
			// 		}
			// 		else if(realPath.top().second == tempY)//same column
			// 		{
			// 			if(realPath.top().first > tempX)//Right
			// 			{
			// 				////cout << "going right" << endl;
			// 				while(tempDir != 0)
			// 				{
			// 					if(bestDir[make_pair(tempDir, 0)] == 0)// turn right
			// 					{
			// 						ret.pushAction(TURN_RIGHT);
			// 						if(++tempDir > 3) 
			// 							tempDir = 0;
			// 						++cost;
			// 					}
			// 					else//turn left
			// 					{
			// 						ret.pushAction(TURN_LEFT);
			// 						if(--tempDir > 3) 
			// 							tempDir = 3;
			// 						++cost;
			// 					}
			// 				}
			// 				++tempX;
			// 				if(killingWumpus && tempY == dY && tempX == dX)
			// 				{
			// 					killingWumpus = false;
			// 					killedWumpus = true;
			// 					wX = -1;
			// 					wY = -1;
			// 					markWumpus();
			// 					ret.pushAction(SHOOT);
			// 				}
			// 				else
			// 					ret.pushAction(FORWARD);
			// 				++cost;
			// 			}
			// 			else//left
			// 			{
			// 				////cout << "going left" << endl;
			// 				while(tempDir != 2)
			// 				{
			// 					if(bestDir[make_pair(tempDir, 2)] == 0)// turn right
			// 					{
			// 						ret.pushAction(TURN_RIGHT);
			// 						if(++tempDir > 3) 
			// 							tempDir = 0;
			// 						++cost;
			// 					}
			// 					else//turn left
			// 					{
			// 						ret.pushAction(TURN_LEFT);
			// 						if(--tempDir > 3) 
			// 							tempDir = 3;
			// 						++cost;
			// 					}
			// 				}
			// 				--tempX;
			// 				if(killingWumpus && tempY == dY && tempX == dX)
			// 				{
			// 					killingWumpus = false;
			// 					killedWumpus = true;
			// 					wX = -1;
			// 					wY = -1;
			// 					markWumpus();
			// 					ret.pushAction(SHOOT);
			// 				}
			// 				else
			// 					ret.pushAction(FORWARD);
			// 				++cost;
			// 			}
			// 		}
			// 	}
			// }
			// ret.setCost(cost);
			// if(exit)
			// 	ret.pushAction(CLIMB);
			// return ret;
			////cout << "getMoves" <<endl;
			searchNode start(player.getX(), player.getY(), dX, dY, -1, -1, 0, player.getDir() , player.getDir());
			////cout << "after making start searchNode" <<endl;
			searchNode goal(dX, dY, dX, dY, -1, -1, 0, player.getDir(), player.getDir());
			////cout << "before calling AStarSearch" <<endl;
			stack<searchNode> realPath = AStarSearch(start, goal);
			////cout << "dX: " << dX << " dY: " << dY  << endl;
			int tempX = player.getX();
			int tempY = player.getY();
			size_t tempDir = player.getDir();
			Move ret;
		 	int cost = 0;
			while(!realPath.empty())
			{
				if(realPath.top().x == tempX && realPath.top().y == tempY)
				{
					////cout << "pop current tile and get it out the way" << endl;
					realPath.pop();
				}
				else
				{
					if(realPath.top().x == tempX)//same row
					{
						if(realPath.top().y > tempY)//Up
						{
							////cout << "going up" << endl;
							while(tempDir != 3)
							{
								if(bestDir[make_pair(tempDir, 3)] == 0)// turn right
								{
									ret.pushAction(TURN_RIGHT);
									if(++tempDir > 3) 
										tempDir = 0;
									++cost;
								}
								else//turn left
								{
									ret.pushAction(TURN_LEFT);
									if(--tempDir > 3) 
										tempDir = 3;
									++cost;
								}
							}
							++tempY;
							if(killingWumpus && tempY == dY && tempX == dX)
							{
								killingWumpus = false;
								killedWumpus = true;
								wX = -1;
								wY = -1;
								markWumpus();
								ret.pushAction(SHOOT);
							}
							else
								ret.pushAction(FORWARD);
							++cost;
						}
						else//down
						{
							////cout << "going down" << endl;
							while(tempDir != 1)
							{
								if(bestDir[make_pair(tempDir, 1)] == 0)// turn right
								{
									ret.pushAction(TURN_RIGHT);
									if(++tempDir > 3) 
										tempDir = 0;
									++cost;
								}
								else//turn left
								{
									ret.pushAction(TURN_LEFT);
									if(--tempDir > 3) 
										tempDir = 3;
									++cost;
								}
							}
							--tempY;
							if(killingWumpus && tempY == dY && tempX == dX)
							{
								killingWumpus = false;
								killedWumpus = true;
								wX = -1;
								wY = -1;
								markWumpus();
								ret.pushAction(SHOOT);
							}
							else
								ret.pushAction(FORWARD);
							++cost;
						}
					}
					else if(realPath.top().y == tempY)//same column
					{
						if(realPath.top().x > tempX)//Right
						{
							////cout << "going right" << endl;
							while(tempDir != 0)
							{
								if(bestDir[make_pair(tempDir, 0)] == 0)// turn right
								{
									ret.pushAction(TURN_RIGHT);
									if(++tempDir > 3) 
										tempDir = 0;
									++cost;
								}
								else//turn left
								{
									ret.pushAction(TURN_LEFT);
									if(--tempDir > 3) 
										tempDir = 3;
									++cost;
								}
							}
							++tempX;
							if(killingWumpus && tempY == dY && tempX == dX)
							{
								killingWumpus = false;
								killedWumpus = true;
								wX = -1;
								wY = -1;
								markWumpus();
								ret.pushAction(SHOOT);
							}
							else
								ret.pushAction(FORWARD);
							++cost;
						}
						else//left
						{
							////cout << "going left" << endl;
							while(tempDir != 2)
							{
								if(bestDir[make_pair(tempDir, 2)] == 0)// turn right
								{
									ret.pushAction(TURN_RIGHT);
									if(++tempDir > 3) 
										tempDir = 0;
									++cost;
								}
								else//turn left
								{
									ret.pushAction(TURN_LEFT);
									if(--tempDir > 3) 
										tempDir = 3;
									++cost;
								}
							}
							--tempX;
							if(killingWumpus && tempY == dY && tempX == dX)
							{
								killingWumpus = false;
								killedWumpus = true;
								wX = -1;
								wY = -1;
								markWumpus();
								ret.pushAction(SHOOT);
							}
							else
								ret.pushAction(FORWARD);
							++cost;
						}
					}
				}
			}
			ret.setCost(cost);
			if(exit)
				ret.pushAction(CLIMB);
			return ret;	
		}
		bool movesLeft()
		{
			return !moves.empty();
		}
		Action getMove()
		{
			lastMove = moves.nextAction();
			if(lastMove == TURN_RIGHT)
				player.turnRight();
			else if(lastMove == TURN_LEFT)
				player.turnLeft();
			else if(lastMove == FORWARD)
				player.goForward();
			return lastMove;
		}
		void clearTiles()
		{
			if(!foundWumpus)
			{
				for(int i = 0; i < 7; ++i)
				{
					if(xLimit == -1 || i < xLimit)
					{
						for(int j = 0; j < 7; ++j)
						{
							if(yLimit == -1 || j < yLimit)
							{
							
								if(Tiles[i][j].canWumpus())//if can be a wumpus
								{
									if(checkUp(i, j) && Tiles[i][j+ 1].isVisited() && !Tiles[i][j+1].isStench())
									{
										Tiles[i][j].setCanBeWumpus(false);
										if(!Tiles[i][j].canPitfall())//if no potential pitfall then mark Tile as good
										{
											Tiles[i][j].setGood(true);
										}	
									}
									if(checkDown(i, j) && Tiles[i][j - 1].isVisited() && !Tiles[i][j-1].isStench())
									{
										Tiles[i][j].setCanBeWumpus(false);
										if(!Tiles[i][j].canPitfall())//if no potential pitfall then mark Tile as good
										{
											Tiles[i][j].setGood(true);
										}	
									}
									if(checkLeft(i, j) && Tiles[i - 1][j].isVisited() && !Tiles[i-1][j].isStench())
									{
										Tiles[i][j].setCanBeWumpus(false);
										if(!Tiles[i][j].canPitfall())//if no potential pitfall then mark Tile as good
										{
											Tiles[i][j].setGood(true);
										}	
									}
									if(checkRight(i, j) && Tiles[i+ 1][j].isVisited() && !Tiles[i+1][j].isStench())
									{
										Tiles[i][j].setCanBeWumpus(false);
										if(!Tiles[i][j].canPitfall())//if no potential pitfall then mark Tile as good
										{
											Tiles[i][j].setGood(true);
										}	
									}
								}
							}
							if(Tiles[i][j].canPitfall())//if can be a pitfall
							{
								if(checkUp(i, j) && Tiles[i][j+ 1].isVisited() && !Tiles[i][j+1].isBreeze())
								{
									Tiles[i][j].setCanBePitfall(false);
									if(!Tiles[i][j].canWumpus())//if no potential pitfall then mark Tile as good
									{
										Tiles[i][j].setGood(true);
									}	
								}
								if(checkDown(i, j) && Tiles[i][j - 1].isVisited() && !Tiles[i][j-1].isBreeze())
								{
									Tiles[i][j].setCanBePitfall(false);
									if(!Tiles[i][j].canWumpus())//if no potential pitfall then mark Tile as good
									{
										Tiles[i][j].setGood(true);
									}	
								}
								if(checkLeft(i, j) && Tiles[i - 1][j].isVisited() && !Tiles[i-1][j].isBreeze())
								{
									Tiles[i][j].setCanBePitfall(false);
									if(!Tiles[i][j].canWumpus())//if no potential pitfall then mark Tile as good
									{
										Tiles[i][j].setGood(true);
									}	
								}
								if(checkRight(i, j) && Tiles[i+ 1][j].isVisited() && !Tiles[i+1][j].isBreeze())
								{
									Tiles[i][j].setCanBePitfall(false);
									if(!Tiles[i][j].canWumpus())//if no potential pitfall then mark Tile as good
									{
										Tiles[i][j].setGood(true);
									}	
								}
							}
						}
					}
				}
			}
			//check left and right wall Wumpus
			if(!foundWumpus)//special case
			{
				if(Tiles[0][1].isStench() && Tiles[1][0].isStench())
				{
					foundWumpus= true;
					wX = 1;
					wY = 1;
                    Tiles[wY][wX].setRealWumpus(true);
					////cout << "0: wX: " << wX << " wY: " << wY << endl;
				}
			}
			if(!foundWumpus)
			{
				for(int j = 0; j < 7; ++j)
				{
					if(yLimit == -1 || j < yLimit)
					{
						if(Tiles[0][j].canWumpus())
						{
							if(Tiles[0][j-1].isVisited() && Tiles[0][j-1].isStench())//0,0 cant be wumpus
							{
								//that means either top or right can be wumpus
								//so lets check if thr right has been cleared
								if(j-1 > 0 && Tiles[1][j-2].isVisited() &&!Tiles[1][j- 2].isStench())
								{
									foundWumpus = true;
									wX = 0;
									wY = j;
                                    Tiles[wY][wX].setRealWumpus(true);
									////cout << "1: wX: " << wX << " wY: " << wY << endl;
								}
							}
						}
						if(!foundWumpus && xLimit != -1 && Tiles[xLimit - 1][j].canWumpus())
						{
							if(j > 0 && Tiles[xLimit - 1][j-1].isVisited() && Tiles[xLimit - 1][j-1].isStench())//0,0 cant be wumpus
							{
								//that means either top or right can be wumpus
								//so lets check if thr right has been cleared
								if(j-1 > 0 && Tiles[xLimit - 2][j-2].isVisited() &&!Tiles[xLimit - 2][j- 2].isStench())
								{
									foundWumpus = true;
									wX = xLimit - 1;
									wY = j;
                                    Tiles[wY][wX].setRealWumpus(true);
									////cout << "2: wX: " << wX << " wY: " << wY << endl;
								}
							}
						}
					}
				}
			}
			//check top and bottom Walls
			if(!foundWumpus)
			{
				for(int i = 0; i < 7; ++i)
				{
					if(xLimit == -1 || i < xLimit)
					{
						if(Tiles[i][0].canWumpus())//wumpus bottom wall
						{
							if(Tiles[i - 1][0].isVisited() && Tiles[i - 1][0].isStench())
							{
								if(i - 1 > 0 && Tiles[i - 2][1].isVisited() && !Tiles[i-2][1].isStench())
								{
									foundWumpus = true;
									wX = i;
									wY = 0;
                                    Tiles[wY][wX].setRealWumpus(true);
									////cout << "3: wX: " << wX << " wY: " << wY << endl;
								}
							}
						}
						if(!foundWumpus && yLimit != -1 && Tiles[i][yLimit - 1].canWumpus())
						{
							if(i > 0 && Tiles[i - 1][yLimit - 1].isVisited() && Tiles[i - 1][yLimit-1].isStench())//0,0 cant be wumpus
							{
								//that means either top or right can be wumpus
								//so lets check if thr right has been cleared
								if(i-1 > 0 && Tiles[i - 2][yLimit-2].isVisited() &&!Tiles[i- 2][yLimit- 2].isStench())
								{
									foundWumpus = true;
									wX = i;
									wY = yLimit - 1;
                                    Tiles[wY][wX].setRealWumpus(true);
									////cout << "4: wX: " << wX << " wY: " << wY << endl;
								}
							}
						}
					}
				}
			}
            if(!foundWumpus)
            {
                for(int i = 0; i < 7; ++i)
                {
                    if(xLimit == -1 || i < xLimit)
                    {
                        for(int j = 0; j < 7; ++j)
                        {
                            if(yLimit == -1 || j < yLimit)
                            {
                                int count = 0;
                                if(Tiles[i][j].canWumpus())
                                {
                                    if(checkUp(i, j) && Tiles[i][j+ 1].isVisited() && Tiles[i][j+1].isStench())
                                    {
                                        ++count;
                                    }
                                    if(checkDown(i, j) && Tiles[i][j - 1].isVisited() && Tiles[i][j-1].isStench())
                                    {
                                        ++count;
                                    }
                                    if(checkLeft(i, j) && Tiles[i - 1][j].isVisited() && Tiles[i-1][j].isStench())
                                    {
                                        ++count;
                                    }
                                    if(checkRight(i, j) && Tiles[i+ 1][j].isVisited() && Tiles[i+1][j].isStench())
                                    {
                                        ++count;
                                    }
                                    if(count > 1)
                                    {
                                        Tiles[i][j].setRealWumpus(true);
                                        foundWumpus = true;
                                        wX = i;
                                        wY = j;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if(foundWumpus)
                markWumpus();
		}
		void markWumpus()
		{
			for(int i = 0; i < 7; ++i)
			{
				if(xLimit == -1 || i < xLimit)
				{
					for(int j = 0; j < 7; ++j)
					{
						if(yLimit == -1 || j < yLimit)
						{
							if(wX == i && wY == j)
							{
								Tiles[i][j].setRealWumpus(true);
							}
							else
							{
								if(Tiles[i][j].isRealWumpus())
									Tiles[i][j].setRealWumpus(false);
								if(Tiles[i][j].canWumpus())
								{
									if(!Tiles[i][j].canPitfall())//if no potential pitfall then mark Tile as good
									{
										////cout << "x: " << i << "y:" << j << " marked as good!" << endl;
										Tiles[i][j].setGood(true);
									}
									else
										Tiles[i][j].setCanBeWumpus(false);
								}
							}	
						}
					}
				}
			}
		}
		stack<searchNode> AStarSearch(searchNode start, searchNode goal)
		{
			////cout << "AStarSearch" << endl;
			OpenList OPEN;
			OPEN.push(start);
			ClosedList CLOSED;
			stack<searchNode> path;

			while(!OPEN.empty())
			{
				////cout << "AStarSearchWhileLoop" << endl;
				searchNode current = OPEN.top();
				path.push(current);
				//cout << "x: " <<current.x  << " y: " << current.y<< endl;
				if(current == goal)
					return construct_path(path);
				OPEN.pop();
				CLOSED.push(current);
				if(checkRight(current.x, current.y) && ((Tiles[current.x +1][current.y].isVisited() || Tiles[current.x+1][current.y].isGood()) || (Tiles[current.x +1][current.y].isRealWumpus() && killingWumpus)))//check if good tile on right 
				{
					searchNode neighbor(current.x+1, current.y, goal.x, goal.y, current.x, current.y, current.g, 0, current.dir);
					if(!CLOSED.contains(neighbor))//not in closed List
					{
						if(!OPEN.replace_if(neighbor))//not in open 
						{
							////cout << "Neighbor -> x: " <<neighbor.x  << " y: " << neighbor.y<< endl;
							OPEN.push(neighbor);
						}
					}
				}
				////cout << "Neighbor outside -> x: " << OPEN.top().x  << " y: " << OPEN.top().y<< endl;
				if(checkLeft(current.x, current.y) && ((Tiles[current.x - 1][current.y].isVisited() || Tiles[current.x - 1][current.y].isGood()) ||  (Tiles[current.x - 1][current.y].isRealWumpus() && killingWumpus)))//check if good tile on right 
				{
					searchNode neighbor(current.x-1, current.y, goal.x, goal.y, current.x, current.y, current.g, 2, current.dir);
					if(!CLOSED.contains(neighbor))//not in closed List
					{
						if(!OPEN.replace_if(neighbor))//not in open 
						{
							////cout << "Neighbor -> x: " <<neighbor.x  << " y: " << neighbor.y<< endl;
							OPEN.push(neighbor);
						}
					}
				}
				if(checkDown(current.x, current.y) && ((Tiles[current.x][current.y-1].isVisited() || Tiles[current.x ][current.y - 1].isGood()) ||  (Tiles[current.x][current.y- 1].isRealWumpus() && killingWumpus)))//check if good tile on right 
				{
					searchNode neighbor(current.x, current.y-1, goal.x, goal.y, current.x, current.y, current.g, 1, current.dir);
					if(!CLOSED.contains(neighbor))//not in closed List
					{
						if(!OPEN.replace_if(neighbor))//not in open 
						{
							////cout << "Neighbor -> x: " <<neighbor.x  << " y: " << neighbor.y<< endl;
							OPEN.push(neighbor);
						}
					}
				}
				if(checkUp(current.x, current.y) && ((Tiles[current.x][current.y+1].isVisited() || Tiles[current.x][current.y+1].isGood()) ||  (Tiles[current.x][current.y+1].isRealWumpus() && killingWumpus)))//check if good tile on right 
				{
					searchNode neighbor(current.x, current.y+1, goal.x, goal.y, current.x, current.y, current.g, 3, current.dir);
					if(!CLOSED.contains(neighbor))//not in closed List
					{
						if(!OPEN.replace_if(neighbor))//not in open 
						{
							////cout << "Neighbor -> x: " <<neighbor.x  << " y: " << neighbor.y<< endl;
							OPEN.push(neighbor);
						}
					}
				}

			}
			return stack<searchNode>();
		}
		stack<searchNode> construct_path(stack<searchNode> path)
		{
			////cout << "construct_path" << endl;
			searchNode current = path.top();
			path.top();
			stack<searchNode> realpath;
			realpath.push(current);
			////cout << "current -> x: " << current.x  << " y: " << current.y<< endl;
			while(!path.empty())
			{
				current = path.top();
				/////cout << "current - > x: " <<current.x  << " y: " << current.y<< endl;
				////cout << "realpath Top - > x: " << realpath.top().px  << " y: " << realpath.top().py<< endl;
				path.pop();
				while(current.x != realpath.top().px || current.y  != realpath.top().py)
				{
					current = path.top();
					path.pop();
				}
			//	//cout << "x: " <<current.x  << " y: " << current.y<< endl;
				realpath.push(current);
			}
			return realpath;
		}

	};

	World world;
public:

};

#endif
