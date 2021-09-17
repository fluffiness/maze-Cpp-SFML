// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <SFML/Graphics.hpp>
#include <utility>
#include <vector>
#include <iostream>
#include <map>

class Maze {
private:
    sf::RenderWindow m_Window;
    sf::Vector2<int> m_GridSize;
    int m_BlockSize;
    int m_WallWidth;
    int m_WindowHeight;
    int m_WindowWidth;
    sf::Vector2<int> m_Begin;
    sf::Vector2<int> m_End;
    std::map<sf::Vector2<int>, bool> m_Visited;
    std::map <sf::Vector2<int>, std::vector<sf::Vector2<int>>> m_Adj;
    std::vector<std::vector<sf::RectangleShape>> m_MazeGrid;
    sf::RectangleShape m_Background;

    void Update() {};
    void Reset() {};

    void Initialize() {
        m_Window.create(sf::VideoMode(m_WindowHeight, m_WindowWidth), "Maze"); // initialize window

        m_Background.setSize(sf::Vector2f(m_WindowHeight, m_WindowWidth)); // set BG
        m_Background.setFillColor(sf::Color::Blue); //set BG color
        m_Background.setPosition(0.0, 0.0);

        // initialize maze grid
        // construct new grid
        std::vector<std::vector<sf::RectangleShape>> grid(
            m_GridSize.x, std::vector<sf::RectangleShape>(
                m_GridSize.y, sf::RectangleShape(sf::Vector2f(m_BlockSize, m_BlockSize))
                )
        );
        m_MazeGrid = grid; // assign
        for (int i = 0; i < m_GridSize.x; i++) {
            for (int j = 0; j < m_GridSize.y; j++) {
                m_MazeGrid[i][j].setFillColor(sf::Color::White); // set coloe
                m_MazeGrid[i][j].setPosition(i * (m_BlockSize + m_WallWidth), j * (m_BlockSize + m_WallWidth)); // set position
            }
        }
    }

public:
    Maze()
        :m_GridSize(sf::Vector2<int>(10, 8)), m_BlockSize(50), m_WallWidth(5), 
        m_WindowHeight(m_GridSize.x* (m_BlockSize + m_WallWidth) - m_WallWidth), m_WindowWidth(m_GridSize.y* (m_BlockSize + m_WallWidth) - m_WallWidth)
    {
    }

    Maze(sf::Vector2<int> gridSize, int blockSize, int wallWidth)
        :m_GridSize(gridSize), m_BlockSize(blockSize), m_WallWidth(wallWidth),
        m_WindowHeight(m_GridSize.x* (m_BlockSize + m_WallWidth) - m_WallWidth), m_WindowWidth(m_GridSize.y* (m_BlockSize + m_WallWidth) - m_WallWidth)
    {
    }

    void Start() {
        Initialize();

        // first draw
        m_Window.draw(m_Background);
        for (std::vector<sf::RectangleShape> column : m_MazeGrid) {
            for (sf::RectangleShape block : column) {
                m_Window.draw(block);
            }
        }

        m_Window.display();

        while (m_Window.isOpen())
        {
            sf::Event event;
            while (m_Window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    m_Window.close();
            }

            // Draw everything
            m_Window.clear();

            m_Window.draw(m_Background);
            for (std::vector<sf::RectangleShape> column : m_MazeGrid) {
                for (sf::RectangleShape block : column) {
                    m_Window.draw(block);
                }
            }
            
            m_Window.display();
        }
    }
    ~Maze() {};
};

int main()
{
    std::vector<std::vector<sf::RectangleShape>> grid;
    Maze maze;
    maze.Start();
    return 0;
}