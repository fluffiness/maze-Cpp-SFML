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
        sf::RenderWindow window(sf::VideoMode(m_GridSize.first * m_BlockSize, m_GridSize.second * m_BlockSize), "Maze");

        std::vector<std::vector<sf::RectangleShape>> grid(m_GridSize.first, std::vector<sf::RectangleShape>(m_GridSize.second, sf::Vector2f(m_BlockSize, m_BlockSize));

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

            
            


            window.display();
        }
    }
    ~Maze() {};
};

int main()
{
    Maze maze;
    maze.Start();
    return 0;
}