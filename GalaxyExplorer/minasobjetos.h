//const int FACIL[2]{5,5};
//const int MEDIO[2]{10,10};
//const int DIFICIL[2]{15,15};
const int WIDTH = 10;
const int HEIGHT = 10;
const int CELL_SIZE = 30;

enum class EstadoCelda{
    Oculto,
    Visible,
    Bandera
};

class Celda{
public:
    int x, y;
    EstadoCelda Celda;
    bool tieneMina;
    int minasVecinas;
};

class Game
{
public:
    Game();
    void run();

private:

    void processEvents();
    void update();
    void render();

private:
    sf::RenderWindow mWindow;
};

Game::Game()
        : mWindow(sf::VideoMode(WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE), "Buscatontas")
{
}

class Grid
{
public:
    void drawGrid();


};

void Game::processEvents()
{
    sf::Event event;
    while (mWindow.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            mWindow.close();
    }
}

void Game::run()
{
    while (mWindow.isOpen())
    {
        processEvents();
        update();
        render();
    }
}
void Game::update()
{
}

void Game::render()
{
    mWindow.clear();
    mWindow.draw(mGrid);
    mWindow.display();
}

int main()
{
    Game game;
    game.run();
    return 0;
}
