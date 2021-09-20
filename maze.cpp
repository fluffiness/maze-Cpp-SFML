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
#include <time.h>
#include <random>
#include <queue>

// provide less than operator for std::map to work
// the operator must be defined in the same namespace as the class ( in this case, Vector2<int> ) of the objects being operated on
namespace sf{
    template<typename T>
    bool operator< (const Vector2<T>& left, const Vector2<T>& right) {
        return (left.x < right.x) || ((left.x == right.x) && (left.y < right.y));
    }
}

class Maze {
private:
    sf::RenderWindow mWindow;
    sf::Vector2<int> mGridSize; // in blocks
    int mBlockSize; // in pixels
    int mWallWidth; // in pixels
    int mWindowHeight; // in pixels
    int mWindowWidth; // in pixels
    int mFrameDuration; // in milliseconds
    sf::Vector2<int> mBegin; // default is (0, 0)
    sf::Vector2<int> mEnd; // default is (mGridSize.x-1, mGridSize.y-1)
    std::map<sf::Vector2<int>, bool> mVisited;
    std::map<sf::Vector2<int>, std::vector<sf::Vector2<int>>> mAdj; // adjacency list for later graph traversal
    std::vector<std::vector<sf::RectangleShape>> mMazeGrid; // grid of squares shown on window
    std::vector<sf::RectangleShape> mGenFiller; // fills the gaps between blocks that are connected during generation
    std::vector<sf::RectangleShape> mSolFiller; // fills the gaps between blocks that are connected during solution
    sf::RectangleShape mBackground; // backgroud i.e. wall
    std::stack<sf::Vector2<int>> mStack; // stack for maze generation and DFS
    enum class Stage {
        INITIALIZATION,
        GENERATION, 
        FINDLONGEST,
        FIRSTSTEP, 
        SOLUTION
    };
    Stage mCurrentStage = Stage::INITIALIZATION;
    int mBlockCounter = 0; // for counting blocks already in maze
    std::mt19937 mRNG;
    std::uniform_int_distribution<int> mUInt4{ 0, 3 };

    const sf::Color BLOCKCOLOR = sf::Color(255, 255, 255);
    const sf::Color WALLCOLOR = sf::Color(0, 0, 0);
    const sf::Color PATHCOLOR = sf::Color(0, 0, 255);
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
                mVisited[sf::Vector2<int>(0, 0)] = false;
            }
        }

        // initialize mRNG
        mRNG.seed(time(NULL));
    }

    sf::RectangleShape FillBetween(sf::Vector2<int> v, sf::Vector2<int> w, sf::Color c) {
        //returns a rectangle that fills the gap between v and w
        int x = (v.x == w.x) ? (v.x * (mWallWidth + mBlockSize) + mWallWidth) : (std::max(v.x, w.x) * (mWallWidth + mBlockSize));
        int y = (v.y == w.y) ? (v.y * (mWallWidth + mBlockSize) + mWallWidth) : (std::max(v.y, w.y) * (mWallWidth + mBlockSize));
        int width = (v.x == w.x) ? mBlockSize : mWallWidth;
        int height = (v.x == w.x) ? mWallWidth : mBlockSize;
        sf::RectangleShape tempRec(sf::Vector2f(width, height));
        tempRec.setFillColor(c);
        tempRec.setPosition(x, y);
        return tempRec;
    }

    sf::Vector2<int> Rotate(const sf::Vector2<int> &dir) {
        return sf::Vector2<int>((-1) * dir.y, dir.x);
    }

    bool CheckValid(sf::Vector2<int> pos) {
        // checks if pos is unvisited and valid
        if (pos.x < 0 || pos.y < 0 || pos.x >= mGridSize.x || pos.y >= mGridSize.y) return false;
        return !mVisited[pos];
    }

    bool FindNext(sf::Vector2<int>& next) {
        // checks for valid next blocks in the four adjacent blocks, starts from a random direction and goes anti-clockwise
        // side effects: assigns the next position to the next variable

        int r = mUInt4(mRNG); // choose a random initial direction
        bool found = false;
        for (int i = 0; i < 4; i++) {// rotate anti-clockwise
            found = CheckValid(mStack.top() + Directions[(r + i) % 4]);
            if (found) {
                next = mStack.top() + Directions[(r + i) % 4];
                break;
            }
        }
        return found;
    }

    void GenerateUpdate() {
        // generates the next frame to display while constructing the graph representation of the maze
        // mStack.top() is the current position
        // choose a random valid (not out of bound and not visited) neighbor of current block and updates it (colors it, fills the gap, etc)
        // if no valid neighbors are found, pop from stack and check previous block for a valid neighbor
        // pop untill a valid neighbor is found
        // if the stack is empty, we are done

        sf::Vector2<int> next; // new block position
        bool foundValid = FindNext(next);

        while (!FindNext(next)) { // while no valid next block found
            mStack.pop(); //  continue popomg from mStack until a valid block is found, the valid block will be assigned to "next"
            if (mStack.empty()) {
                mCurrentStage = Stage::FINDLONGEST;
                return; // return if mStack is empty
            }
        }

        // if valid block found
        mAdj[mStack.top()].push_back(next); // connect new block with previous in mAdj
        mAdj[next].push_back(mStack.top()); // connect new block with previous in mAdj
        mMazeGrid[next.x][next.y].setFillColor(PATHCOLOR); // change color of new block
        mVisited[next] = true; // set visited to true
        mGenFiller.push_back(FillBetween(mStack.top(), next, PATHCOLOR)); // fill gap
        mStack.push(next); // push new block position to stack
    }

    void FindLongest() {
        // Finds longest path in the maze and chooses each end as mBegin and mEnd

        //reset mVisited for BFS
        for (std::map<sf::Vector2<int>, bool>::iterator it = mVisited.begin(); it != mVisited.end(); it++)
            it->second = false;

        std::queue<sf::Vector2<int>> toBeTraversed; // queue for BFS

        toBeTraversed.push(sf::Vector2<int>(0, 0)); // start BFS from (0, 0)
        sf::Vector2<int> currentBlock;
        // first BFS
        while (!toBeTraversed.empty()) {
            currentBlock = toBeTraversed.front();
            for (sf::Vector2<int> neighbor : mAdj[currentBlock]) {
                if (!mVisited[neighbor]) toBeTraversed.push(neighbor); // push to queue if not visited
            }
            mVisited[currentBlock] = true;
            toBeTraversed.pop();
        }
        // first BFS done. currentBlock is the last block to be visited, and thus is the furthest away from (0,0)

        mBegin = currentBlock;
        mMazeGrid[mBegin.x][mBegin.y].setFillColor(sf::Color(255, 0, 0));
        
        //reset mVisited for BFS
        for (std::map<sf::Vector2<int>, bool>::iterator it = mVisited.begin(); it != mVisited.end(); it++)
            it->second = false;

        toBeTraversed.push(sf::Vector2<int>(currentBlock)); // start BFS from currentBlock
        // second BFS
        while (!toBeTraversed.empty()) {
            currentBlock = toBeTraversed.front();
            for (sf::Vector2<int> neighbor : mAdj[currentBlock]) {
                if (!mVisited[neighbor]) toBeTraversed.push(neighbor); // push to queue if not visited
            }
            mVisited[currentBlock] = true;
            toBeTraversed.pop();
        }
        
        mEnd = currentBlock;
        mMazeGrid[mEnd.x][mEnd.y].setFillColor(sf::Color(0, 255, 0));

        mCurrentStage = Stage::FIRSTSTEP;
    }

public:
    Maze()
        :mGridSize({ 10, 8 }), mBlockSize(50), mWallWidth(5), mFrameDuration(100),
        mWindowHeight(mGridSize.x* (mBlockSize + mWallWidth) + mWallWidth), mWindowWidth(mGridSize.y* (mBlockSize + mWallWidth) + mWallWidth), 
        mBegin({ 0, 0 }), mEnd({ mGridSize.x-1, mGridSize.y-1 })
    {
    }

    Maze(sf::Vector2<int> gridSize, int blockSize, int wallWidth, int frameDuration)
        :mGridSize(gridSize), mBlockSize(blockSize), mWallWidth(wallWidth), mFrameDuration(frameDuration),
        mWindowHeight(mGridSize.x* (mBlockSize + mWallWidth) + mWallWidth), mWindowWidth(mGridSize.y* (mBlockSize + mWallWidth) + mWallWidth), 
        mBegin({ 0, 0 }), mEnd({ mGridSize.x - 1, mGridSize.y - 1 })
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
        
        sf::Clock clock;
        while (clock.getElapsedTime().asMilliseconds() < 2000.0);

        ////////////////// mainloop //////////////////
        while (mWindow.isOpen())
        {
            clock.restart();
            sf::Event event;
            while (mWindow.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    mWindow.close();
            }

            // Update shapes
            switch (mCurrentStage) {
            case Stage::INITIALIZATION:
                // initialize stack and visited
                mStack.push(sf::Vector2<int>(0, 0));
                mVisited[sf::Vector2<int>(0, 0)] = true;
                // change color of (0, 0)
                mMazeGrid[0][0].setFillColor(PATHCOLOR);
                mBlockCounter++;
                mCurrentStage = Stage::GENERATION;
                break;
            case Stage::GENERATION:
                GenerateUpdate();
                break;
            case Stage::FINDLONGEST:
                FindLongest();
                break;
            case Stage::FIRSTSTEP:
                // reset mVisited for DFS
                for (std::map<sf::Vector2<int>, bool>::iterator it = mVisited.begin(); it != mVisited.end(); it++)
                    it->second = false;
                
                // initialization for DFS
                mStack.push(mBegin);
                mVisited[mBegin] = true;
                mCurrentStage = Stage::SOLUTION;
                break;
            case Stage::SOLUTION:
                // solve maze using DFS
                if (mStack.top() != mEnd) {
                    bool found = false;
                    for (sf::Vector2<int> neighbor : mAdj[mStack.top()]) { //check for unvisited neighbors
                        if (!mVisited[neighbor]) {
                            found = true;
                            mMazeGrid[neighbor.x][neighbor.y].setFillColor(sf::Color(255, 0, 0));
                            mVisited[neighbor] = true;
                            mSolFiller.push_back(FillBetween(mStack.top(), neighbor, sf::Color(255, 0, 0)));
                            mStack.push(neighbor);
                            break;
                        }
                    }
                    if (!found) {
                        mStack.pop();
                    }
                }
                break;
            }
            
            // Draw everything
            mWindow.clear();
            // draw background i.e. walls
            mWindow.draw(mBackground);
            // draw blocks
            for (std::vector<sf::RectangleShape> column : mMazeGrid) {
                for (sf::RectangleShape block : column) {
                    mWindow.draw(block);
                }
            }
            // draw gen-fillers
            for (sf::RectangleShape filler : mGenFiller) {
                mWindow.draw(filler);
            }

            // drat sol-fillers
            for (sf::RectangleShape filler : mSolFiller) {
                mWindow.draw(filler);
            }

            mWindow.display();
            while (clock.getElapsedTime().asMilliseconds() < mFrameDuration);
        }
        //////////////////////////////////////////////
    }
    ~Maze() {};
};


int main()
{
    Maze maze;
    maze.Start();
    std::cin.get();
    return 0;
}