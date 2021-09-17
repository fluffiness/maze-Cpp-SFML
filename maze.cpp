// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <SFML/Graphics.hpp>
#include <utility>
#include <vector>
#include <iostream>
#include <map>
#include <algorithm>
#include <stack>
#include <stdlib.h>

// provide less than operator for std::map to work
template<typename T>
bool operator< (const sf::Vector2<T>& left, const sf::Vector2<T>& right) {
    if (left.x < right.x) return true;
    if (left.y < right.y) return true;
    return false;
}


class Maze {
private:
    sf::RenderWindow mWindow;
    sf::Vector2<int> mGridSize;
    int mBlockSize;
    int mWallWidth;
    int mWindowHeight;
    int mWindowWidth;
    sf::Vector2<int> mBegin;
    sf::Vector2<int> mEnd;
    std::map<sf::Vector2<int>, bool> mVisited;
    std::map<sf::Vector2<int>, std::vector<sf::Vector2<int>>> mAdj;
    std::vector<std::vector<sf::RectangleShape>> mMazeGrid;
    std::vector<sf::RectangleShape> mFiller; // filles the gap between blocks that are connected
    sf::RectangleShape mBackground;
    std::stack<sf::Vector2<int>> mStack;
    bool made = false;
    bool longestfound = false;
    bool solved = false;

    const sf::Color BLOCKCOLOR = sf::Color(255, 255, 255);
    const sf::Color WALLCOLOR = sf::Color(0, 0, 0);
    const sf::Vector2<int> Directions[4] = { sf::Vector2<int>(1, 0), sf::Vector2<int>(0, 1), sf::Vector2<int>(-1, 0), sf::Vector2<int>(0, -1) };

    void Update() {};
    void Reset() {};

    void Initialize() {
        mWindow.create(sf::VideoMode(mWindowHeight, mWindowWidth), "Maze"); // initialize window

        mBackground.setSize(sf::Vector2f(mWindowHeight, mWindowWidth)); // set BG
        mBackground.setFillColor(WALLCOLOR); //set BG color
        mBackground.setPosition(0.0, 0.0);

        // initialize maze grid
        // construct temp grid
        std::vector<std::vector<sf::RectangleShape>> grid(
            mGridSize.x, std::vector<sf::RectangleShape>(
                mGridSize.y, sf::RectangleShape(sf::Vector2f(mBlockSize, mBlockSize))
                )
        );
        mMazeGrid = grid; // assign temp to mMazeGrid
        for (int i = 0; i < mGridSize.x; i++) {
            for (int j = 0; j < mGridSize.y; j++) {
                mMazeGrid[i][j].setFillColor(BLOCKCOLOR); // set coloe
                mMazeGrid[i][j].setPosition(i * (mBlockSize + mWallWidth) + mWallWidth, j * (mBlockSize + mWallWidth) + mWallWidth); // set position
            }
        }

        // initialize mVisited
        for (int i = 0; i < mGridSize.x;i++){
            for (int j = 0; j < mGridSize.y; j++) {
                mVisited[sf::Vector2<int>(i, j)] = false;
            }
        }

        // initialize stack and visited
        mStack.push(sf::Vector2<int>(0, 0));
        mVisited[sf::Vector2<int>(0, 0)] = true;
    }

    sf::RectangleShape FillBetween(sf::Vector2<int> v, sf::Vector2<int> w) {
        //returns a rectangle that fills the gap between v and w
        int x = (v.x == w.x) ? v.x : std::min(v.x, w.x) + mBlockSize;
        int y = (v.y == w.y) ? v.y : std::min(v.y, w.y) + mBlockSize;
        int width = (v.x == w.x) ? mBlockSize : mWallWidth;
        int height = (v.x == w.x) ? mWallWidth : mBlockSize;
        sf::RectangleShape tempRec(sf::Vector2f(width, height));
        tempRec.setFillColor(BLOCKCOLOR);
        tempRec.setPosition(x, y);
        return tempRec;
    }

    sf::Vector2<int> Rotate(const sf::Vector2<int> &dir) {
        return sf::Vector2<int>((-1) * dir.y, dir.x);
    }

    bool CheckValid(sf::Vector2<int> pos) {
        // checks if pos is unvisited and valid
        if (!(pos.x > 0 && pos.y > 0 && pos.x < mGridSize.x && pos.y < mGridSize.y)) return false;
        return !mVisited[pos];
    }

    void MakeUpdate() {
        // mStack.top() is the current position

        int r = std::rand() % 4; // choose a random initial direction
        sf::Vector2<int> step;
        bool foundValid = false;
        sf::Vector2<int> next;
        for (int i = 0; i < 4; i++) {// rotate anti-clockwise
            step = Directions[(r + i) % 4];
            foundValid = CheckValid(mStack.top() + step);
            if (foundValid) {
                next = mStack.top() + step;
                break;
            }
        }

        //
    }

    void FindLongest() {

    }

    void SolveUpdate() {

    }

public:
    Maze()
        :mGridSize(sf::Vector2<int>(10, 8)), mBlockSize(50), mWallWidth(5), 
        mWindowHeight(mGridSize.x* (mBlockSize + mWallWidth) + mWallWidth), mWindowWidth(mGridSize.y* (mBlockSize + mWallWidth) + mWallWidth)
    {
    }

    Maze(sf::Vector2<int> gridSize, int blockSize, int wallWidth)
        :mGridSize(gridSize), mBlockSize(blockSize), mWallWidth(wallWidth),
        mWindowHeight(mGridSize.x* (mBlockSize + mWallWidth) + mWallWidth), mWindowWidth(mGridSize.y* (mBlockSize + mWallWidth) + mWallWidth)
    {
    }

    void Start() {
        Initialize();

        // first draw
        mWindow.draw(mBackground);
        for (std::vector<sf::RectangleShape> column : mMazeGrid) {
            for (sf::RectangleShape block : column) {
                mWindow.draw(block);
            }
        }
        mWindow.display();
        
        // mainloop
        int counter = 0;
        sf::Clock clock;
        while (mWindow.isOpen())
        {
            clock.restart();
            sf::Event event;
            while (mWindow.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    mWindow.close();
            }

            
            // Draw everything
            mWindow.clear();

            if (counter == 0) {
                mWindow.draw(mBackground);
                for (std::vector<sf::RectangleShape> column : mMazeGrid) {
                    for (sf::RectangleShape block : column) {
                        mWindow.draw(block);
                    }
                }
            }
            else {
                mWindow.draw(mBackground);
            }
            
            
            mWindow.display();
            while (clock.getElapsedTime().asMilliseconds() < 1000.0);
            counter = (counter + 1) % 2;
        }
    }
    ~Maze() {};
};



int main()
{
    sf::Vector2<int> vec_a(4, 3);
    sf::Vector2<int> vec_b(3, 4);
    if(vec_a < vec_b){
        std::cout << "less than" << std::endl;
    }
    else {
        std::cout << "more than" << std::endl;
    }
    Maze maze;
    maze.Start();
    std::cin.get();
    return 0;
}