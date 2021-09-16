#include <SFML/Graphics.hpp>
#include <utility>
#include <vector>
#include <iostream>

class Maze {
private:
    std::pair<int, int> m_GridSize;
    int m_BlockSize;
    int m_WallWidth;
    std::pair<int, int> m_Begin;
    std::pair<int, int> m_End;
    void Update() {};
    void Reset() {};
    std::pair<int, int> getCoordinate(int i, int j) {
        return std::make_pair((m_BlockSize + m_WallWidth) * i, (m_BlockSize + m_WallWidth) * j);
    }
public:
    Maze(std::pair<int, int> gridSize = {10, 8}, int blockSize = 50, int wallWidth = 5) {
        m_GridSize.first = gridSize.first;
        m_GridSize.second = gridSize.second;
        m_BlockSize = blockSize;
        m_WallWidth = wallWidth;
    }
    void Start() {
        int windowHeight = m_GridSize.first * (m_BlockSize + m_WallWidth) - m_WallWidth;
        int windowWidth = m_GridSize.second * (m_BlockSize + m_WallWidth) - m_WallWidth;

        sf::RenderWindow window(sf::VideoMode(windowHeight, windowWidth), "Maze");

        std::vector<std::vector<sf::RectangleShape>> grid(
            m_GridSize.first, std::vector<sf::RectangleShape>(
                m_GridSize.second, sf::RectangleShape(sf::Vector2f(m_BlockSize, m_BlockSize))
            )
        );

        for (int i = 0; i < m_GridSize.first; i++) {
            for (int j = 0; j < m_GridSize.second; j++) {
                grid[i][j].setFillColor(sf::Color::Blue);
                grid[i][j].setPosition((float)i * (m_BlockSize + m_WallWidth), (float)j * (m_BlockSize + m_WallWidth));
            }
        }


        sf::RectangleShape rectangle(sf::Vector2f(120.f, 50.f));
        sf::CircleShape shape(100.f);
        shape.setFillColor(sf::Color::Green);

        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
            }

            // Draw everything
            window.clear();

            for (std::vector<sf::RectangleShape> column : grid) {
                for (sf::RectangleShape block : column) {
                    window.draw(block);
                }
            }
            
            window.display();
        }
    }
    ~Maze() {};
};

int main()
{
    Maze maze({5, 4}, 30, 5);
    maze.Start();
    return 0;
}