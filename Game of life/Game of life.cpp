//Version 3.2:221
#include <iostream>
#include <algorithm>
#include <vector>
#include <SFML/Graphics.hpp>

//game options
const uint8_t startSpeed = 100; //start game speed, gens per second
const uint8_t speedStep = 50; //increase of game speed
const bool isLoopCheck = true; //if true, game will check for end (slower)
const bool newGameMode = false; //if true, each cell have only 4 neighbours (up, down, left and right)

//advanced game options
const uint8_t aliveNum = 3; //if cell have neighbours, that equal this number, it will alive
const uint8_t lonelyNum = 1; //if cell have less or equal neighbours, than this number, it will die
const uint8_t overpopulationNum = 4; //if cell have more or equal neighbours, than this number, it will die

//visible options
const uint16_t height = 30; // height of game field
const uint16_t whith = 30; // whith of game field
const uint16_t size = 20; //size of each sell
const uint64_t cellColor = 0x00C0C0FF; //color of cell in HEX format (0x11223344: 11 - red color, 22 - green color, 33 - blue color, 44 - alpha channel)
const uint64_t backgroundColor = 0xFFFFFFFF; //color of background in HEX format (0x11223344: 11 - red color, 22 - green color, 33 - blue color, 44 - alpha channel)
const uint64_t textColor = 0x000000FF; //color of text in HEX format (0x11223344: 11 - red color, 22 - green color, 33 - blue color, 44 - alpha channel)
const uint64_t bordersColor = 0xFFFFFFFF; //color of borders in HEX format (0x11223344: 11 - red color, 22 - green color, 33 - blue color, 44 - alpha channel)
const uint64_t fieldBorderColor = 0x000000FF; //color of field border in HEX format (0x11223344: 11 - red color, 22 - green color, 33 - blue color, 44 - alpha channel)
const uint8_t borderThinkness = 2; //border thinkness, set to 0, if you don't want to see borders

void manual() {
    std::cout << "Game of life - version 3.1:221\n";
    std::cout << "To pause - press 'P', Space or Enter\n";
    std::cout << "To see previous or next gen - press arror left or arror right\n";
    std::cout << "To increase game speed - press '+' or arror up\n";
    std::cout << "To slow down game speed - press '-' or arror down\n";
    std::cout << "To make new game field - press 'N'\n";
    std::cout << "To fill the game field - press 'F'\n";
    std::cout << "To invert the game field - press 'I'\n";
    std::cout << "To reset game speed - press 'S'\n";
    std::cout << "To quit game - press 'Q' or close game window\n";
    return;
}

void fillZeroGen(std::vector<std::vector<std::vector<bool>>>& field, const bool& state) {
    for (int j = 0; j < height; ++j) {
        for (int k = 0; k < whith; ++k) field[0][j][k] = state;
    }
    return;
}

void delPrevGens(std::vector<std::vector<std::vector<bool>>>& field, const uint64_t& curGen) {
    if (curGen != 0) {
        for (int i = 0; i < height; ++i) {
            for (int j = 0; j < whith; ++j) field[0][i][j] = field[curGen][i][j];
        }
    }
    return;
}

void invert(std::vector<std::vector<std::vector<bool>>>& field, const uint64_t& curGen) {
    delPrevGens(field, curGen);
    for (int j = 0; j < height; ++j) {
        for (int k = 0; k < whith; ++k) field[0][j][k] = !field[0][j][k];
    }
    return;
}

void nextGen(std::vector<std::vector<std::vector<bool>>>& field, uint64_t& curGen) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < whith; ++j) {
            int neighbours = 0, up, down, left, right;
            if (i == 0) up = height - 1;
            else up = i - 1;
            if (i == height - 1) down = 0;
            else down = i + 1;
            if (j == 0) left = whith - 1;
            else left = j - 1;
            if (j == whith - 1) right = 0;
            else right = j + 1;

            neighbours += field[curGen][up][j];
            neighbours += field[curGen][down][j];
            neighbours += field[curGen][i][left];
            neighbours += field[curGen][i][right];
            if (!newGameMode) {
                neighbours += field[curGen][up][left];
                neighbours += field[curGen][up][right];
                neighbours += field[curGen][down][left];
                neighbours += field[curGen][down][right];
            }

            if (field[curGen][i][j]) {
                if (neighbours <= lonelyNum) field[curGen + 1][i][j] = false;
                else if (neighbours >= overpopulationNum) field[curGen + 1][i][j] = false;
                else field[curGen + 1][i][j] = true;
            }
            else if (neighbours == aliveNum) field[curGen + 1][i][j] = true;
            else field[curGen + 1][i][j] = false;
        }
    }
    ++curGen;
    return;
}

uint32_t aliveCells(const std::vector<std::vector<std::vector<bool>>>& field, const uint64_t& curGen) {
    uint32_t alive = 0;
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < whith; ++j) {
            alive += field[curGen][i][j];
        }
    }
    return alive;
}

uint64_t check(const std::vector<std::vector<std::vector<bool>>>& field, const uint64_t& curGen) {
    if (aliveCells(field, curGen) == 0 && curGen != 0) return 0;
    else {
        for (int i = curGen - 1; i >= 0; --i) {
            bool coincedence = true;
            for (int j = 0; j < height; ++j) {
                for (int k = 0; k < whith; ++k) {
                    if (field[i][j][k] != field[curGen][j][k]) coincedence = false;
                }
            }
            if (coincedence) {
                if (curGen - i == 1) return 2;
                else return (curGen - i) * 2;
            }
            if (!isLoopCheck) break;
        }
        return 1;
    }
}

bool invCell(std::vector<std::vector<std::vector<bool>>>& field, const sf::Vector2i& pos, const bool& isMouseReleased) {
    static uint16_t lastX = whith + 1, lastY = height + 1;
    uint16_t x, y;
    x = (pos.y - size * 1.5) / (size + borderThinkness); y = pos.x / (size + borderThinkness);

    if (lastX != x || lastY != y || isMouseReleased) {
        field[0][x][y] = !field[0][x][y];
        lastX = x; lastY = y;
        return true;
    }
    return false;
}

void lol() {
    sf::RenderWindow lolWindow(sf::VideoMode(size * 8.5, size * 1.5), "Lol", sf::Style::Close);
    while (lolWindow.isOpen()) {
        sf::Event event;
        while (lolWindow.pollEvent(event)) if (event.type == sf::Event::Closed) lolWindow.close();
        lolWindow.clear(sf::Color(backgroundColor));
        sf::Font font;
        font.loadFromFile("../resourses/Chava.ttf");
        sf::Text lolText;
        lolText.setFont(font);
        lolText.setCharacterSize(size);
        lolText.setFillColor(sf::Color(textColor));
        lolText.setString("You found me!");
        lolText.setPosition(0.25 * size, 0.25 * size);
        lolWindow.draw(lolText);
        lolWindow.display();
    }
    return;
}

void clickAnalyse(const sf::Vector2i& pos) {
    if (pos.x > 0.25 * size && pos.x < (0.25 + 69 / 6) * size) {
        if (pos.y > 0.25 * size && pos.y < 1.25 * size) {
            lol();
        }
    }
    return;
}

void drawBorders(sf::RenderWindow& window) {
    for (int i = 0; i <= height; ++i) {
        sf::RectangleShape rect;
        rect.setOutlineThickness(0);
        if (i == 0 || i == height) {
            rect.setSize(sf::Vector2f((whith * size) + (whith + 1) * borderThinkness, borderThinkness));
            rect.setPosition(0, size * i + i * borderThinkness + size * 1.5);
            rect.setFillColor(sf::Color(fieldBorderColor));
        }
        else {
            rect.setSize(sf::Vector2f((whith * size) + (whith - 1) * borderThinkness, borderThinkness));
            rect.setPosition(borderThinkness, size * i + i * borderThinkness + size * 1.5);
            rect.setFillColor(sf::Color(bordersColor));
        }
        window.draw(rect);
    }
    for (int i = 0; i <= whith; ++i) {
        sf::RectangleShape rect;
        rect.setOutlineThickness(0);
        if (i == 0 || i == whith) {
            rect.setSize(sf::Vector2f(borderThinkness, (height * size) + (height + 1) * borderThinkness));
            rect.setPosition(size * i + i * borderThinkness, size * 1.5);
            rect.setFillColor(sf::Color(fieldBorderColor));
        }
        else {
            rect.setSize(sf::Vector2f(borderThinkness, (height * size) + (height - 1) * borderThinkness));
            rect.setPosition(size * i + i * borderThinkness, borderThinkness +size * 1.5);
            rect.setFillColor(sf::Color(bordersColor));
        }
        window.draw(rect);
    }
    return;
}

void showGen(sf::RenderWindow& window, const std::vector<std::vector<std::vector<bool>>>& field, const uint64_t& curGen) {
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < whith; ++j) {
            int color;

            if (field[curGen][i][j] == true) color = cellColor;
            else continue;

            sf::RectangleShape rect;
            rect.setSize(sf::Vector2f(size, size));
            rect.setOutlineThickness(0);
            rect.setPosition(j * size + (j + 1) * borderThinkness, i * size + (i + 1) * borderThinkness + (size * 1.5));
            rect.setFillColor(sf::Color(color));
            window.draw(rect);
        }
    }
    return;
}

void showText(sf::RenderWindow& window, const std::vector<std::vector<std::vector<bool>>>& field, const uint64_t& curGen, const uint64_t& repeatGen, const bool& isRepeat, const uint8_t& speed, const std::vector<uint64_t>& state, uint32_t alive) {
    sf::Font font;
    font.loadFromFile("../resourses/Chava.ttf");

    sf::Text stateText, genText, speedText;
    stateText.setFont(font); genText.setFont(font); speedText.setFont(font);
    stateText.setCharacterSize(size); genText.setCharacterSize(size); speedText.setCharacterSize(size);
    stateText.setFillColor(sf::Color(textColor)); genText.setFillColor(sf::Color(textColor)); speedText.setFillColor(sf::Color(textColor));

    std::string genStr = "Gen #";
    genStr += std::to_string(curGen);
    genStr += ", alive cells: ";
    genStr += std::to_string(alive);
    std::string speedStr = "Game speed limit: ";
    if (speed == 0) speedStr += "unlimited";
    else speedStr += std::to_string(speed);
    uint64_t curState;
    if (isRepeat) curState = state[repeatGen];
    else curState = state[curGen];
    
    if (curGen == 0 && !isRepeat) stateText.setString("Please, set 0 gen to start");
    else {
        switch (curState) {
        case 0: 
            stateText.setString("Game over: everybody dead :(");
            break;
        case 1:
            stateText.setString("It't all ok");
            break;
        case 2:
            stateText.setString("Game over: static state");
            break;
        default:
            uint64_t period = curState / 2;
            std::string state = "Game over: cycle of ";
            state += std::to_string(period);
            state += " gens has happened";
            stateText.setString(state);
            break;
        }
    }
    genText.setString(genStr);
    speedText.setString(speedStr);
    stateText.setPosition(0, (height * size) + (height + 1) * borderThinkness + size * 1.5);
    genText.setPosition(0, (height + 1) * size + (height + 1) * borderThinkness + size * 1.5);
    speedText.setPosition(0, (height + 2) * size + (height + 1) * borderThinkness + size * 1.5);

    window.draw(stateText); window.draw(genText); window.draw(speedText);
    return;
}

void showMenu(sf::RenderWindow& window) {
    sf::Texture logoT;
    sf::Sprite logoS;
    logoT.loadFromFile("../resourses/grafics/logo.png");
    logoT.setSmooth(false);
    logoS.setTexture(logoT);
    logoS.setPosition(size * 0.25, size * 0.25);
    logoS.setScale(sf::Vector2f(size / 6, size / 6));
    window.draw(logoS);
    return;
}

void eventProcessing(sf::RenderWindow& window, sf::Event& event, std::vector<std::vector<std::vector<bool>>>& field, uint8_t& speed, bool& isRunning, uint64_t& curGen, uint64_t& repeatGen, bool& isRepeat, std::vector<uint64_t>& state) {
    static bool isMouseReleased = true;
    if (event.type == sf::Event::Closed) window.close();
    else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Equal || event.key.code == sf::Keyboard::Add || event.key.code == sf::Keyboard::Up) {
            if (speed + speedStep < 256) speed += speedStep;
        }
        else if (event.key.code == sf::Keyboard::Hyphen || event.key.code == sf::Keyboard::Subtract || event.key.code == sf::Keyboard::Down) {
            if (speed >= speedStep) speed -= speedStep;
        }
        else if (event.key.code == sf::Keyboard::P || event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Enter) isRunning = !isRunning;
        else if (event.key.code == sf::Keyboard::N) {
            fillZeroGen(field, false);
            isRunning = false; isRepeat = false;
            curGen = 0;
        }
        else if (event.key.code == sf::Keyboard::F) {
            fillZeroGen(field, true);
            isRunning = false;  isRepeat = false;
            curGen = 0;
        }
        else if (event.key.code == sf::Keyboard::I) {
            invert(field, curGen);
            isRunning = false; isRepeat = false;
            curGen = 0;
        }
        else if (event.key.code == sf::Keyboard::Left) {
            if (curGen > 0) --curGen;
            isRunning = false;
            if (state[curGen] == 1) isRepeat = false;
        }
        else if (event.key.code == sf::Keyboard::Right) {
            isRunning = false;
            if (!isRepeat) {
                nextGen(field, curGen);
                state[curGen] = check(field, curGen);
                if (state[curGen] != 1) {
                    isRepeat = true;
                    repeatGen = curGen;
                }
            }
            else {
                if (curGen == repeatGen) {
                    if (state[curGen] > 2) curGen = curGen - (state[repeatGen] / 2) + 1;
                }
                else ++curGen;
            }
        }
        else if (event.key.code == sf::Keyboard::S) speed = startSpeed;
        else if (event.key.code == sf::Keyboard::Q) window.close();
    }
    else if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
        sf::Vector2i pos = sf::Mouse::getPosition(window);
        if (pos.x > 0 && pos.y > size * 1.5) {
            if (pos.x < (whith * size) + (whith + 1) * borderThinkness && pos.y < (height * size) + (height + 1) * borderThinkness + size * 1.5) {
                delPrevGens(field, curGen);
                if (invCell(field, pos, isMouseReleased)) {
                    curGen = 0;
                    isRunning = false; isRepeat = false;
                }
            }
            else if (isMouseReleased) {
                clickAnalyse(pos);
                isMouseReleased = false;
            }
        }
        else if (isMouseReleased) {
                clickAnalyse(pos);
                isMouseReleased = false;
        }
        isMouseReleased = false;
    }
    else if (event.type == sf::Event::MouseButtonReleased) isMouseReleased = true;
    return;
}

int main() {
    uint8_t speed = startSpeed;
    uint64_t curGen = 0, repeatGen = 0;
    bool isRunning = false, isRepeat = false;
    std::vector<uint64_t> state = {1};
    std::vector<std::vector<std::vector<bool>>> field;
    field.assign(1, std::vector<std::vector<bool>>(height, std::vector<bool>(whith)));
    manual();

    sf::RenderWindow gameWindow(sf::VideoMode((whith * size) + (whith + 1) * borderThinkness, (height * size) + (height + 1) * borderThinkness + (size * 4.75)), "Game of life - version 3.2:221", sf::Style::Close);
    sf::Image icon;
    icon.loadFromFile("../resourses/grafics/icon.png");
    gameWindow.setIcon(48, 48, icon.getPixelsPtr());
    gameWindow.setVerticalSyncEnabled(true);

    while (gameWindow.isOpen()) {
        sf::Event gameEvent;
        uint32_t alive = aliveCells(field, curGen);
        field.resize(field.size() + 1, std::vector<std::vector<bool>>(height, std::vector<bool>(whith)));
        state.resize(state.size() + 1);
       
        while (gameWindow.pollEvent(gameEvent)) eventProcessing(gameWindow, gameEvent, field, speed, isRunning, curGen, repeatGen, isRepeat, state);
        if (isRunning) {
            if (!isRepeat) {
                nextGen(field, curGen);
                state[curGen] = check(field, curGen);
                if (state[curGen] != 1) {
                    isRepeat = true;
                    repeatGen = curGen;
                }
            }
            else {
                if (curGen == repeatGen) {
                    if (state[curGen] > 2) curGen = curGen - (state[repeatGen] / 2) + 1;
                }
                else ++curGen;
            }
        }

        gameWindow.clear(sf::Color(backgroundColor));
        showMenu(gameWindow);
        drawBorders(gameWindow);
        showGen(gameWindow, field, curGen);
        showText(gameWindow, field, curGen, repeatGen, isRepeat, speed, state, alive);
        gameWindow.display();
        if (isRunning && speed != 0) sf::sleep(sf::milliseconds(1000 / (speed / 10)));
    }
    std::cout << "Goodbye!";
    return 0;
}