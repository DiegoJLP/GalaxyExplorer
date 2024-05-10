#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <thread>



class Title {
public:
    Title(const std::string& texto, const sf::Font& fuente, unsigned int tamano, const sf::Vector2f& posicion)
            : m_texto(texto, fuente, tamano), m_posicion(posicion) {
        m_texto.setString(texto);
        m_texto.setCharacterSize(tamano);
        m_texto.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = m_texto.getLocalBounds();
        m_texto.setPosition(posicion.x - textBounds.width / 2, posicion.y - textBounds.height / 2);
    }

    void draw(sf::RenderWindow& ventana) {
        ventana.draw(m_texto);
    }

private:
    sf::Text m_texto;
    sf::Vector2f m_posicion;
};

class Subtitle {
public:
    Subtitle(const std::string& texto, const sf::Font& fuente, unsigned int tamano, const sf::Vector2f& posicion)
            : m_texto(texto, fuente, tamano), m_posicion(posicion) {
        m_texto.setString(texto);
        m_texto.setCharacterSize(tamano);
        m_texto.setFillColor(sf::Color::White);
        sf::FloatRect textBounds = m_texto.getLocalBounds();
        m_texto.setPosition(posicion.x - textBounds.width / 2, posicion.y - textBounds.height / 2);
    }

    void draw(sf::RenderWindow& ventana) {
        ventana.draw(m_texto);
    }

private:
    sf::Text m_texto;
    sf::Vector2f m_posicion;
};

class Button {
public:
    Button(const std::string& texto, const sf::Font& fuente, const sf::Vector2f& posicion, const sf::Vector2f& tamano, const std::function<void()>& accion)
            : m_texto(texto, fuente, 20), m_posicion(posicion), m_tamano(tamano), m_accion(accion) {
        // Configurar el texto
        m_texto.setFont(fuente);
        m_texto.setString(texto);
        m_texto.setCharacterSize(20);
        m_texto.setFillColor(sf::Color::White);
        m_texto.setStyle(sf::Text::Bold);

        // Calcular la posición del texto para que esté centrado en la caja
        sf::FloatRect textBounds = m_texto.getLocalBounds();
        m_texto.setPosition(posicion + sf::Vector2f((tamano.x - textBounds.width) / 2, (tamano.y - textBounds.height) / 2));


        // Configurar la caja
        sf::Color verdeConsola(9, 193, 23); // Rojo: 100, Verde: 200, Azul: 50

        m_caja.setSize(tamano);
        m_caja.setPosition(posicion);
        m_caja.setFillColor(verdeConsola);
        m_caja.setOutlineThickness(2);
        m_caja.setOutlineColor(sf::Color::White);
    }

    void draw(sf::RenderWindow& ventana) {
        ventana.draw(m_caja);
        ventana.draw(m_texto);
    }

    void handleEvent(const sf::Event& evento, sf::RenderWindow& ventana) {
        if (evento.type == sf::Event::MouseButtonPressed && evento.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos = ventana.mapPixelToCoords(sf::Vector2i(evento.mouseButton.x, evento.mouseButton.y));
            if (m_caja.getGlobalBounds().contains(mousePos)) {
                m_accion();
                ventana.close();
            }
        }
    }

private:
    sf::Text m_texto;
    sf::RectangleShape m_caja;
    sf::Vector2f m_posicion;
    sf::Vector2f m_tamano;
    std::function<void()> m_accion;
};

enum class CellState {
    Hidden,
    Revealed,
    Flagged
};

struct Cell {
    int x, y;
    CellState state;
    bool hasMine;
    int neighboringMines;
    sf::Texture texture; // Textura para la mina
    sf::Texture warning;
    sf::Texture numero1;
    sf::Texture numero2;
    sf::Texture numero3;
    sf::Texture numero4;
    sf::Texture numero5;
    sf::Texture numero6;
    sf::Texture numero7;
    sf::Texture numero8;


};

void placeMines(Cell board[][10], int* WIDTH, int* HEIGHT, int* NUM_MINES, int CELL_SIZE, const sf::Font& font) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> disX(0, *WIDTH - 1);
    std::uniform_int_distribution<int> disY(0, *HEIGHT - 1);

    int minesPlaced = 0;
    while (minesPlaced < *NUM_MINES) {
        int x = disX(gen);
        int y = disY(gen);
        if (!board[x][y].hasMine) {
            board[x][y].hasMine = true;
            minesPlaced++;
        }
    }
}
void placeNeighboringNumber(Cell board[][10], int* WIDTH, int* HEIGHT, int* NUM_MINES, int CELL_SIZE, const sf::Font& font) {

    // Calcular minas vecinas
    for (int x = 0; x < *WIDTH; ++x) {
        for (int y = 0; y < *HEIGHT; ++y) {
            if (!board[x][y].hasMine) {
                int count = 0;
                for (int i = -1; i <= 1; ++i) {
                    for (int j = -1; j <= 1; ++j) {
                        int nx = x + i;
                        int ny = y + j;
                        if (nx >= 0 && nx < *WIDTH && ny >= 0 && ny < *HEIGHT && board[nx][ny].hasMine) {
                            count++;
                        }
                    }
                }
                board[x][y].neighboringMines = count;
            }
        }
    }
}


void revealCell(Cell board[][10], int x, int y, int* WIDTH, int* HEIGHT, int CELL_SIZE, sf::RenderWindow& gameWindow, const sf::Font& font) {
    if (x < 0 || x >= *WIDTH || y < 0 || y >= *HEIGHT) return; // Out of bounds

    // Si la celda está oculta y se hace clic izquierdo, revelarla
    if (board[x][y].state == CellState::Hidden && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        board[x][y].state = CellState::Revealed;

        if (!board[x][y].hasMine) {
            // Mostrar el número de minas vecinas
            if (board[x][y].neighboringMines == 0) {
                // Si no hay minas vecinas, también revelar las celdas adyacentes
                for (int i = -1; i <= 1; ++i) {
                    for (int j = -1; j <= 1; ++j) {
                        int nx = x + i;
                        int ny = y + j;
                        revealCell(board, nx, ny, WIDTH, HEIGHT, CELL_SIZE, gameWindow, font);
                    }
                }
            }
        }
    }
    // Si la celda está oculta y se hace clic derecho, marcarla con bandera
    if (board[x][y].state == CellState::Hidden && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
        board[x][y].state = CellState::Flagged;
    } else {
        // Si la celda está marcada con bandera y se hace clic derecho, ocultarla
        if (board[x][y].state == CellState::Flagged && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
            board[x][y].state = CellState::Hidden;
        }
    }
}

void revealAllMines(Cell board[][10], int* WIDTH, int* HEIGHT) {
    for (int x = 0; x < *WIDTH; ++x) {
        for (int y = 0; y < *HEIGHT; ++y) {
            if (board[x][y].hasMine) {
                board[x][y].state = CellState::Revealed;
            }
        }
    }
}
void showBlueBox(sf::RenderWindow& window, const sf::Font& font) {
    sf::RectangleShape redBox(sf::Vector2f(300, 200));
    redBox.setFillColor(sf::Color::Blue);
    redBox.setPosition(window.getSize().x / 2 - 120, window.getSize().y / 2 - 80);
    window.draw(redBox);

    sf::Text texto("GALAXY EXPLORED", font, 18);
    texto.setFillColor(sf::Color::White);
    texto.setPosition(window.getSize().x / 2 - 90, window.getSize().y / 2  - 20);
    window.draw(texto);

    // Botón PLAY AGAIN
    sf::RectangleShape playAgainButton(sf::Vector2f(100, 50));
    playAgainButton.setFillColor(sf::Color::Black);
    playAgainButton.setPosition(window.getSize().x / 2 - 100, window.getSize().y / 2 + 30);
    window.draw(playAgainButton);

    sf::Text playAgainText("RESTART", font, 16);
    playAgainText.setFillColor(sf::Color::White);
    playAgainText.setPosition(window.getSize().x / 2 - 95, window.getSize().y / 2 + 50);
    window.draw(playAgainText);

    // Botón EXIT
    sf::RectangleShape exitButton(sf::Vector2f(100, 50));
    exitButton.setFillColor(sf::Color::Black);
    exitButton.setPosition(window.getSize().x / 2 + 50, window.getSize().y / 2 + 30);
    window.draw(exitButton);

    sf::Text exitText("EXIT", font, 16);
    exitText.setFillColor(sf::Color::Red);
    exitText.setPosition(window.getSize().x / 2 + 70, window.getSize().y / 2 + 50);
    window.draw(exitText);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            window.close();
        }
    }
}

void showRedBox(sf::RenderWindow& window, const sf::Font& font) {
    sf::RectangleShape redBox(sf::Vector2f(300, 200));
    redBox.setFillColor(sf::Color::Red);
    redBox.setPosition(window.getSize().x / 2 - 120, window.getSize().y / 2 - 80);
    window.draw(redBox);

    sf::Text texto("YOU'RE FIRED", font, 18);
    texto.setFillColor(sf::Color::White);
    texto.setPosition(window.getSize().x / 2 - 90, window.getSize().y / 2  - 20);
    window.draw(texto);

    // Botón PLAY AGAIN
    sf::RectangleShape playAgainButton(sf::Vector2f(100, 50));
    playAgainButton.setFillColor(sf::Color::Black);
    playAgainButton.setPosition(window.getSize().x / 2 - 100, window.getSize().y / 2 + 30);
    window.draw(playAgainButton);

    sf::Text playAgainText("RESTART", font, 16);
    playAgainText.setFillColor(sf::Color::White);
    playAgainText.setPosition(window.getSize().x / 2 - 95, window.getSize().y / 2 + 50);
    window.draw(playAgainText);

    // Botón EXIT
    sf::RectangleShape exitButton(sf::Vector2f(100, 50));
    exitButton.setFillColor(sf::Color::Black);
    exitButton.setPosition(window.getSize().x / 2 + 50, window.getSize().y / 2 + 30);
    window.draw(exitButton);

    sf::Text exitText("EXIT", font, 16);
    exitText.setFillColor(sf::Color::Red);
    exitText.setPosition(window.getSize().x / 2 + 70, window.getSize().y / 2 + 50);
    window.draw(exitText);

    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        if (exitButton.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
            window.close();
        }
    }
}
void introduccion(const sf::Font& font) {
    sf::RenderWindow window1(sf::VideoMode(675, 510), "Introduction Window");

    // Carga de la imagen del avatar como fondo de la ventana
    sf::Texture avatarTexture;
    if (!avatarTexture.loadFromFile("instructor.png")) {
        std::cerr << "Failed to load avatar image" << std::endl;
        return;
    }
    sf::Sprite avatar(avatarTexture);
    avatar.setScale(0.5f, 0.5f);
    avatar.setPosition(0, 0); // Coloca la imagen en la esquina superior izquierda

    // Configuración del texto

    sf::Text text1("Welcome aboard Liutenant!\n\nWe're glad to have you here\nAs you've been told our\nmission at UNSCGE is to \nexplore the galaxy in search \nof resourceful planets \n \nFrom here you'll see a board \njust as if you were playing \na classic Minesweeper game \nYou'll see a lot of cells \ndepending on the Galaxy you \nchoose \n \nTheese are quadrants.", font, 15);
    text1.setPosition(275, 100); // Ajusta la posición según tus necesidades
    text1.setFillColor(sf::Color::White); // Cambia el color del texto a blanco

    // Caja de texto para el primer texto
    sf::RectangleShape textBox1(sf::Vector2f(300, 510));
    textBox1.setPosition(250, 0); // Ajusta la posición según tus necesidades
    textBox1.setFillColor(sf::Color::Black); // Cambia el color de fondo de la caja de texto a negro

    // Configuración del texto 2
    sf::Text text2("Your job is to send probes to \nall quadrants that don't have\na black hole\n \nFeel free to flag the quadrants\nyou think have black holes to\navoid them\n\nIf you click a quadrant you'll \nsend a probe, and if you send\na probe to a black hole\nyou would've cost the agency\nbillions of dollars... \nin other words you're fired. \n\nReady? \n \nGood luck Liutenant!", font, 15);
    text2.setPosition(275, 100); // Ajusta la posición según tus necesidades
    text2.setFillColor(sf::Color::White); // Cambia el color del texto a blanco

    // Caja de texto para el segundo texto
    sf::RectangleShape textBox2(sf::Vector2f(300, 510));
    textBox2.setPosition(250, 0); // Ajusta la posición según tus necesidades
    textBox2.setFillColor(sf::Color::Black); // Cambia el color de fondo de la caja de texto a negro

    // Creación del botón "Next"
    sf::RectangleShape nextButton(sf::Vector2f(100, 50));
    nextButton.setFillColor(sf::Color::Blue);
    nextButton.setPosition(550, 450); // Ajusta la posición según tus necesidades

    sf::Text nextButtonText("NEXT", font, 20);
    nextButtonText.setPosition(563, 465); // Ajusta la posición según tus necesidades

    // Bucle principal
    bool showFirstPage = true;
    bool showSecondPage = false;
    bool showStartButton = false;
    while (window1.isOpen()) {
        sf::Event event;
        while (window1.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window1.close();
            else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window1.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
                    if (nextButton.getGlobalBounds().contains(mousePos)) {
                        if (showFirstPage) {
                            showFirstPage = false;
                            showSecondPage = true;
                        } else if (showSecondPage) {
                            window1.close();
                        }
                    }
                }
            }
        }

        window1.clear();

        // Dibuja el fondo de la ventana
        window1.draw(avatar);

        // Dibuja el texto y la caja de texto
        if (showFirstPage) {
            window1.draw(textBox1);
            window1.draw(text1);
        } else if (showSecondPage) {
            window1.draw(textBox2);
            window1.draw(text2);
        }

        // Dibuja el botón "Next"
        if (showFirstPage) {
            window1.draw(nextButton);
            window1.draw(nextButtonText);
        } else if (showSecondPage) {
            // Dibuja el botón "Start"
            nextButtonText.setString("Start");
            window1.draw(nextButton);
            window1.draw(nextButtonText);
        }

        window1.display();
    }
}

void selectDifficulty(const sf::Font& font, int*& WIDTH1, int*& HEIGHT1, int*& NUM_MINES1) {
    sf::RenderWindow window2(sf::VideoMode(500, 400), "Galaxy Explorer");

    sf::Vector2f tamanoBoton(200, 50);
    sf::Vector2f posicionFacil(150, 100);
    sf::Vector2f posicionIntermedio(150, 200);
    sf::Vector2f posicionDificil(150, 300);


    Button botonFacil("EASY", font, posicionFacil, tamanoBoton, [&]() {
        *WIDTH1 = 5;
        *HEIGHT1 = 5;
        *NUM_MINES1 = 7;
    });

    Button botonIntermedio("MEDIUM", font, posicionIntermedio, tamanoBoton, [&]() {
        *WIDTH1 = 8;
        *HEIGHT1 = 8;
        *NUM_MINES1 = 10;
    });

    Button botonDificil("HARD", font, posicionDificil, tamanoBoton, [&]() {
        *WIDTH1 = 10;
        *HEIGHT1 = 10;
        *NUM_MINES1 = 30;
    });

    Title titulo("WELCOME LIUTENANT", font, 24, sf::Vector2f(window2.getSize().x / 2, 30));
    Subtitle subtitulo("CHOOSE YOUR GALAXY", font, 18, sf::Vector2f(window2.getSize().x / 2, 60));

    while (window2.isOpen()) {
        sf::Event event;
        while (window2.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window2.close();
                return;
            }
            botonFacil.handleEvent(event, window2);
            botonIntermedio.handleEvent(event, window2);
            botonDificil.handleEvent(event, window2);
        }

        window2.clear(sf::Color::Black);

        titulo.draw(window2);
        subtitulo.draw(window2);

        botonFacil.draw(window2);
        botonIntermedio.draw(window2);
        botonDificil.draw(window2);

        window2.display();
    }
}

bool checkWinCondition(Cell board[][10], int WIDTH, int HEIGHT) {
    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            if (!board[x][y].hasMine && board[x][y].state != CellState::Revealed) {
                return false; // Todas las celdas sin minas no están reveladas
            }
        }
    }
    return true; // Todas las celdas sin minas están reveladas
}

int main() {
    bool allowInteractions = true;
    sf::Color fondoEspacio(0, 6, 35);
    sf::Font font;
    if (!font.loadFromFile("joystix.ttf")) {
        std::cerr << "Failed to load font" << std::endl;
        return 1;
    }
    introduccion(font);
    int* WIDTH1 = new int(0);
    int* HEIGHT1 = new int(0);
    int* NUM_MINES1 = new int(0);

    //Aqui debe iniciar el bucle de Play Again

    selectDifficulty(font, WIDTH1, HEIGHT1, NUM_MINES1);

    int WIDTH = *WIDTH1;
    int HEIGHT = *HEIGHT1;
    int NUM_MINES = *NUM_MINES1;
    // Initialize board
    const int CELL_SIZE = 80;
    Cell board[10][10];
    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            board[x][y].x = x;
            board[x][y].y = y;
            board[x][y].state = CellState::Hidden;
            board[x][y].hasMine = false;
            board[x][y].neighboringMines = 0;
            if (!board[x][y].texture.loadFromFile("Agujeronegro.png")) {
                std::cerr << "Failed to load mine texture\n";
                return 1;
            }
            if (!board[x][y].warning.loadFromFile("warning.jpeg")) {
                std::cerr << "Failed to load mine texture\n";
                return 1;
            }
            if (!board[x][y].numero1.loadFromFile("numero1.png")) {
                std::cerr << "Failed to load mine texture\n";
                return 1;
            }
            if (!board[x][y].numero2.loadFromFile("numero2.png")) {
                std::cerr << "Failed to load mine texture\n";
                return 1;
            }
            if (!board[x][y].numero3.loadFromFile("numero3.png")) {
                std::cerr << "Failed to load mine texture\n";
                return 1;
            }
            if (!board[x][y].numero4.loadFromFile("numero4.png")) {
                std::cerr << "Failed to load mine texture\n";
                return 1;
            }
            if (!board[x][y].numero5.loadFromFile("numero5.png")) {
                std::cerr << "Failed to load mine texture\n";
                return 1;
            }
            if (!board[x][y].numero6.loadFromFile("numero6.png")) {
                std::cerr << "Failed to load mine texture\n";
                return 1;
            }
            if (!board[x][y].numero7.loadFromFile("numero7.png")) {
                std::cerr << "Failed to load mine texture\n";
                return 1;
            }
            if (!board[x][y].numero8.loadFromFile("numero8.png")) {
                std::cerr << "Failed to load mine texture\n";
                return 1;
            }
        }
    }

    // Place mines randomly and calculate neighboring mines
    placeMines(board, &WIDTH, &HEIGHT, &NUM_MINES, CELL_SIZE, font);
    placeNeighboringNumber(board, &WIDTH, &HEIGHT, &NUM_MINES, CELL_SIZE, font);

    // Main loop
    sf::RenderWindow gameWindow(sf::VideoMode(WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE), "Buscaminas");

    while (gameWindow.isOpen()) {
        sf::Event event;
        while (gameWindow.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                gameWindow.close();
            else if (allowInteractions && event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left || event.mouseButton.button == sf::Mouse::Right) {
                    int mouseX = event.mouseButton.x / CELL_SIZE;
                    int mouseY = event.mouseButton.y / CELL_SIZE;
                    revealCell(board, mouseX, mouseY, &WIDTH, &HEIGHT, CELL_SIZE, gameWindow, font);
                }
            }
        }

        gameWindow.clear();

        for (int x = 0; x < WIDTH; ++x) {
            for (int y = 0; y < HEIGHT; ++y) {
                if (checkWinCondition(board, WIDTH, HEIGHT)) {
                    std::cout << "¡Has ganado!" << std::endl;
                    allowInteractions = false;
                    showBlueBox(gameWindow, font);
                    //Abrir la ventana Galaxy Explored con los 2 botones (Play again y exit)
                }
                sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
                cell.setPosition(x * CELL_SIZE, y * CELL_SIZE);
                cell.setOutlineColor(sf::Color::Red);
                cell.setOutlineThickness(2);
                if (board[x][y].state == CellState::Hidden) {
                    cell.setFillColor(sf::Color::Black);
                } else if (board[x][y].state == CellState::Revealed) {
                    if (board[x][y].hasMine) {
                        cell.setTexture(&board[x][y].texture); // Usar textura de mina
                        revealAllMines(board, &WIDTH, &HEIGHT);
                        allowInteractions = false;
                        showRedBox(gameWindow, font);
                        //Cerrar esta ventana y abrir el Youre fired, o mostrar caja con el texto Youre fired y 2 botones
                    } else {
                        switch (board[x][y].neighboringMines) {
                            case 1:
                                cell.setTexture(&board[x][y].numero1);
                                break;
                            case 2:
                                cell.setTexture(&board[x][y].numero2);
                                break;
                            case 3:
                                cell.setTexture(&board[x][y].numero3);
                                break;
                            case 4:
                                cell.setTexture(&board[x][y].numero4);
                                break;
                            case 5:
                                cell.setTexture(&board[x][y].numero5);
                                break;
                            case 6:
                                cell.setTexture(&board[x][y].numero6);
                                break;
                            case 7:
                                cell.setTexture(&board[x][y].numero7);
                                break;
                            case 8:
                                cell.setTexture(&board[x][y].numero8);
                                break;
                            default:
                                cell.setFillColor(fondoEspacio);
                                break;
                        }
                    }
                } else if (board[x][y].state == CellState::Flagged) {
                    cell.setTexture(&board[x][y].warning);
                }
                gameWindow.draw(cell); // Dibujar la celda en cualquier caso

            }
        }
        gameWindow.display();

    }
    delete WIDTH1;
    delete HEIGHT1;
    delete NUM_MINES1;


    return 0;
}
