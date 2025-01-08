#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <fstream>    // For file I/O (Save/Load)

// ----------------------------------------------------------
//                 GLOBAL GAME STATE VARIABLES
// ----------------------------------------------------------

// We add a simple integer to track the current level (1 or 2).
int currentLevel = 1;

// -- Player movement control --
float playerMoveInterval = 0.2f;
float playerTimer = 0.0f;

// -- Ghost movement control --
float ghostMoveIntervalSlow = 1.0f;
float ghostMoveIntervalFast = 0.1f;
float ghostTimers[4] = { 0.f, 0.f, 0.f, 0.f };

bool  gcounter = false;   // Power pellet effect
char  Direction = 'e';    // Player direction: 'l', 'r', 'u', 'd', 'e', etc.
int   score = 0;
float timer = 0.0f;       // Global time for power pellets, etc.
float time_checker = 0.0f; // Marks when we picked up a power pellet
int   LIVES = 3;

// -- Countdown Timer --
float totalTimeAllowed = 600.0f;  // e.g., 60 seconds to finish
float timeRemaining = totalTimeAllowed;

// Player position in the grid
int x = 1, y = 15;

// Ghost positions in the grid (gy= row, gx= column)
int gx[4] = { 13, 15, 13, 15 };
int gy[4] = { 13, 13, 15, 15 };

// Ghost speed states; 0 => slow, 1 => fast
int ghostSpeed[4] = { 0, 0, 0, 0 };

// The maze map (level 1 layout by default).
char arr[20][30] = {
    {'W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W'},
    {'W','X','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','X','W'},
    {'W','f','W','W','W','W','W','f','W','f','W','f','W','W','W','W','W','f','W','f','W','f','W','W','W','W','W','f','W'},
    {'W','f','W','f','f','f','f','f','W','f','W','f','W','B','W','B','W','f','W','f','W','f','f','f','f','f','W','f','W'},
    {'W','f','W','f','W','W','W','f','W','f','W','f','W','f','W','f','W','f','W','f','W','f','W','W','W','f','W','f','W'},
    {'W','f','W','f','f','X','W','f','W','f','W','f','W','f','W','f','W','f','W','f','W','f','W','X','f','f','W','f','W'},
    {'W','f','W','W','W','W','W','f','W','f','W','f','W','f','W','f','W','f','W','f','W','f','W','W','W','W','W','f','W'},
    {'W','f','W','f','f','f','f','f','W','f','W','f','W','f','W','f','W','f','W','f','W','f','f','f','f','f','W','f','W'},
    {'W','f','W','W','W','W','W','f','W','f','W','f','W','f','W','f','W','f','W','f','W','f','W','W','W','W','W','f','W'},
    {'W','f','f','f','W','f','f','f','W','f','W','f','W','f','W','f','W','f','W','f','W','f','f','f','W','f','f','f','W'},
    {'W','f','W','f','f','f','W','f','W','f','W','f','W','f','W','f','W','f','W','f','W','f','W','f','f','f','W','f','W'},
    {'W','f','W','W','W','W','W','f','W','f','W','f','f','f','f','f','f','f','W','f','W','f','W','W','W','W','W','f','W'},
    {'W','f','W','f','f','f','f','f','W','f','W','f','W','W','!','W','W','f','W','f','W','f','f','f','f','f','W','f','W'},
    {'W','f','W','W','W','W','W','f','W','f','W','f','W','g',' ','g','W','f','W','f','W','f','W','W','W','W','W','f','W'},
    {'W','f','W','f','f','X','W','f','W','f','W','f','W',' ',' ',' ','W','f','W','f','W','f','W','X','f','f','W','f','W'},
    {'W','f','W','f','W','W','W','f','W','f','W','f','W','g',' ','g','W','f','W','f','W','f','W','W','W','f','W','f','W'},
    {'W','f','W','f','f','f','f','f','W','f','W','f','W','W','W','W','W','f','W','f','W','f','f','f','f','f','W','f','W'},
    {'W','f','W','W','W','W','W','f','W','f','W','f','W','W','W','W','W','f','W','f','W','f','W','W','W','W','W','f','W'},
    {'W','X','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','f','X','W'},
    {'W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W','W'}
};

// ----------------------------------------------------------
//                  SAVE / LOAD FUNCTIONS (Unchanged)
// ----------------------------------------------------------
void SaveGame() {
    // ... same as your code ...
    std::ofstream outFile("savegame.txt");
    if (!outFile.is_open())
    {
        std::cout << "Error: Could not open savegame.txt for writing.\n";
        return;
    }

    // Write all relevant game variables in one line
    outFile << x << " " << y << " "
        << LIVES << " "
        << timer << " "
        << time_checker << " "
        << timeRemaining << " "
        << score << " "
        << gcounter << " "
        << Direction << "\n";

    // Write ghost data on the next line
    for (int i = 0; i < 4; i++)
    {
        outFile << gx[i] << " " << gy[i] << " " << ghostSpeed[i] << " "
            << ghostTimers[i] << " ";
    }
    outFile << "\n";

    // Write the entire maze (20 rows, each 30 chars, no spaces)
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 30; j++)
        {
            outFile << arr[i][j];
        }
        outFile << "\n";
    }

    outFile.close();
    std::cout << "Game saved successfully.\n";
}

void LoadGame() {
    // ... same as your code ...
    std::ifstream inFile("savegame.txt");
    if (!inFile.is_open())
    {
        std::cout << "Error: Could not open savegame.txt for reading. Make sure a save file exists.\n";
        return;
    }

    // x y LIVES timer time_checker timeRemaining score gcounter Direction
    inFile >> x >> y
        >> LIVES
        >> timer
        >> time_checker
        >> timeRemaining
        >> score
        >> gcounter
        >> Direction;

    // Read ghost data
    for (int i = 0; i < 4; i++)
    {
        inFile >> gx[i] >> gy[i] >> ghostSpeed[i] >> ghostTimers[i];
    }

    // Read the maze
    for (int i = 0; i < 20; i++)
    {
        std::string line;
        std::getline(inFile, line);
        if (line.size() < 30)
        {
            if (!std::getline(inFile, line))
            {
                std::cout << "Error: Maze data missing or malformed.\n";
                inFile.close();
                return;
            }
        }

        if (line.size() < 30)
        {
            std::cout << "Error: Maze data line is too short.\n";
            inFile.close();
            return;
        }

        for (int j = 0; j < 30; j++)
        {
            arr[i][j] = line[j];
        }
    }

    inFile.close();
    std::cout << "Game loaded successfully.\n";
}

// ----------------------------------------------------------
//               Level 2 (More Difficult) Setup
// ----------------------------------------------------------
// We'll define a function to overwrite arr[][] for a second level.
void LoadLevel2()
{
    // Wipe the array with spaces
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            arr[i][j] = ' ';
        }
    }

    // Outer boundary
    for (int j = 0; j < 30; j++) {
        arr[0][j] = 'W';
        arr[19][j] = 'W';
    }
    for (int i = 0; i < 20; i++) {
        arr[i][0] = 'W';
        arr[i][29] = 'W';
    }

    // Example more walls
    for (int j = 5; j < 25; j++) {
        arr[5][j] = 'W';
    }
    for (int i = 6; i < 14; i++) {
        arr[i][10] = 'W';
    }
    // Keep ghost home (center)
    arr[13][13] = 'g';
    arr[13][15] = 'g';
    arr[15][13] = 'g';
    arr[15][15] = 'g';

    // Fill spaces with f
    for (int i = 1; i < 19; i++) {
        for (int j = 1; j < 29; j++) {
            if (arr[i][j] == ' ') {
                arr[i][j] = 'f';
            }
        }
    }

    // Put some power pellets X in corners
    arr[1][1] = 'X';
    arr[1][28] = 'X';
    arr[18][1] = 'X';
    arr[18][28] = 'X';

    // Possibly keep some boosters or puzzle items
    // For example:
    arr[2][2] = 'B';

    // Make it more difficult: less time, faster ghosts
    timeRemaining = 40.0f;   // less time
    ghostMoveIntervalSlow = 0.7f;  // faster
    ghostMoveIntervalFast = 0.06f; // faster

    // Reset some states
    x = 1;  y = 15;  // reset player
    timer = 0.f;  time_checker = 0.f;
    gcounter = false;

    // Reset ghost positions, speeds, timers
    for (int i = 0; i < 4; i++) {
        gx[i] = (i % 2 == 0 ? 13 : 15);
        gy[i] = (i < 2 ? 13 : 15);
        ghostSpeed[i] = 0;
        ghostTimers[i] = 0.f;
    }

    std::cout << "Level 2 Loaded!\n";
}

// A small helper to see if the current level is done
bool isLevelComplete()
{
    // Check if any 'f' left
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 30; j++) {
            if (arr[i][j] == 'f') {
                return false;
            }
        }
    }
    return true;
}

// ----------------------------------------------------------
//                      HELPER FUNCTIONS
// ----------------------------------------------------------

// (No changes to the actual logic of collision/move below, just as your code.)
void PELLET_RESPAWN()
{
    arr[1][1] = 'X';  arr[1][27] = 'X';
    arr[18][1] = 'X';  arr[18][27] = 'X';
    arr[5][5] = 'X';  arr[5][23] = 'X';
    arr[14][5] = 'X';  arr[14][23] = 'X';
}

void CheckGhostCollisions()
{
    for (int i = 0; i < 4; i++)
    {
        if (gy[i] == x && gx[i] == y)
        {
            if (gcounter)
            {
                score += 200;
                gy[i] = 15;
                gx[i] = 15;
            }
            else
            {
                LIVES--;
                x = 1;
                y = 15;
            }
        }
    }
}

void MoveGhost(int index)
{
    while (true)
    {
        int move = rand() % 4;
        if (move == 0 && arr[gy[index] - 1][gx[index]] != 'W' && arr[gy[index] - 1][gx[index]] != 'z')
        {
            gy[index]--;
            break;
        }
        else if (move == 1 && arr[gy[index] + 1][gx[index]] != 'W' && arr[gy[index] + 1][gx[index]] != '!')
        {
            gy[index]++;
            break;
        }
        else if (move == 2 && arr[gy[index]][gx[index] - 1] != 'W' && arr[gy[index]][gx[index] - 1] != '!')
        {
            gx[index]--;
            break;
        }
        else if (move == 3 && arr[gy[index]][gx[index] + 1] != 'W' && arr[gy[index]][gx[index] + 1] != '!')
        {
            gx[index]++;
            break;
        }
    }

    if (arr[gy[index]][gx[index]] == 'B' && ghostSpeed[index] == 0)
    {
        ghostSpeed[index] = 1;
        arr[gy[index]][gx[index]] = ' ';
    }
}

void MovePlayer()
{
    int oldX = x, oldY = y;

    if (Direction == 'l') y--;
    else if (Direction == 'r') y++;
    else if (Direction == 'u') x--;
    else if (Direction == 'd') x++;

    if (arr[x][y] == 'W')
    {
        x = oldX;
        y = oldY;
    }

    if (arr[x][y] == 'f')
    {
        arr[x][y] = ' ';
        score++;
    }
    else if (arr[x][y] == 'X' && !gcounter)
    {
        arr[x][y] = ' ';
        score += 5;
        gcounter = true;
        time_checker = timer;
    }
    else
    {
        if (gcounter && timer > time_checker + 5.0f)
        {
            gcounter = false;
            PELLET_RESPAWN();
        }
    }
}

// ----------------------------------------------------------
//                       MAIN FUNCTION
// ----------------------------------------------------------
int main()
{
    srand(static_cast<unsigned>(time(nullptr)));

    // Create our SFML window
    sf::RenderWindow window(sf::VideoMode(800, 400), "PAC-MAN (Save/Load + 2 Levels)");

    // Loading textures & fonts (unchanged)
    sf::Texture texPlayer, texLeft, texUp, texDown;
    texPlayer.loadFromFile("player.png");
    texLeft.loadFromFile("left.png");
    texUp.loadFromFile("up.png");
    texDown.loadFromFile("down.png");

    sf::Texture texWall, texFood, texPellet, texBooster;
    texWall.loadFromFile("w.png");
    texFood.loadFromFile("pel.png");
    texPellet.loadFromFile("food1.png");
    texBooster.loadFromFile("BOOST.png");

    sf::Texture texGhostBlue, texGhostRed, texGhostGreen, texGhostYellow;
    texGhostBlue.loadFromFile("bg.png");
    texGhostRed.loadFromFile("rg.png");
    texGhostGreen.loadFromFile("gg.png");
    texGhostYellow.loadFromFile("yg.png");

    sf::Texture texGhostFrightened;
    texGhostFrightened.loadFromFile("dead.png");

    sf::Texture side;
    side.loadFromFile("LIVES.png");
    sf::Sprite sideScreen(side);
    sideScreen.setPosition(sf::Vector2f(600, 0));

    sf::Texture menuTexture, gameOverTexture;
    menuTexture.loadFromFile("PLAY.png");
    gameOverTexture.loadFromFile("GAMEOVER.png");
    sf::Sprite menuSprite(menuTexture);
    sf::Sprite gameOverSprite(gameOverTexture);

    // Optional victory texture
    sf::Texture victoryTexture;
    victoryTexture.loadFromFile("WIN.png");
    sf::Sprite victorySprite(victoryTexture);

    sf::Font font;
    font.loadFromFile("arial.ttf");
    sf::Text ScoreText("", font, 30);
    ScoreText.setFillColor(sf::Color::Blue);
    ScoreText.setStyle(sf::Text::Bold);

    sf::Text LivesText("", font, 30);
    LivesText.setFillColor(sf::Color::Blue);
    LivesText.setStyle(sf::Text::Bold);

    sf::Text TimerText("", font, 30);
    TimerText.setFillColor(sf::Color::Red);
    TimerText.setStyle(sf::Text::Bold);

    sf::Clock clock;
    bool menu = true;  // Start with the menu
    bool victory = false; // We'll show a victory screen if level 2 is completed

    while (window.isOpen())
    {
        float deltaTime = clock.restart().asSeconds();
        timer += deltaTime;

        // Decrement time
        timeRemaining -= deltaTime;
        if (timeRemaining < 0.f)
        {
            LIVES = 0; // time up => game over
        }

        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                window.close();
        }

        // ---------------- MENU LOGIC ----------------
        if (menu)
        {
            window.clear();
            menuSprite.setPosition(0, 0);
            window.draw(menuSprite);
            window.display();

            // Press P to start
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))
            {
                menu = false;
            }
            // Press L to load game
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
            {
                LoadGame();
                menu = false;
            }
            continue;
        }

        // -------------- SAVE/LOAD --------------
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
        {
            SaveGame();
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::L))
        {
            LoadGame();
        }

        // Power pellet expiry
        if (gcounter && timer > time_checker + 5.0f)
        {
            gcounter = false;
            PELLET_RESPAWN();
        }

        // Player direction
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))  Direction = 'l';
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) Direction = 'r';
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))    Direction = 'u';
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))  Direction = 'd';

        // Player move
        playerTimer += deltaTime;
        if (playerTimer >= playerMoveInterval)
        {
            MovePlayer();
            CheckGhostCollisions();
            playerTimer = 0.f;
        }

        // Ghost moves
        for (int i = 0; i < 4; i++)
        {
            ghostTimers[i] += deltaTime;
            float interval = (ghostSpeed[i] == 0) ? ghostMoveIntervalSlow : ghostMoveIntervalFast;
            if (ghostTimers[i] >= interval)
            {
                MoveGhost(i);
                CheckGhostCollisions();
                ghostTimers[i] = 0.f;
            }
        }

        // If out of lives => break to show game over
        if (LIVES <= 0)
        {
            break;
        }

        // ---------------- LEVEL COMPLETION ----------------
        // If all 'f' is eaten => next level or victory
        if (isLevelComplete())
        {
            if (currentLevel == 1)
            {
                // Move to second level
                currentLevel = 2;
                LoadLevel2();
            }
            else
            {
                // We completed level 2 => victory
                victory = true;
                break;
            }
        }

        // --------------- RENDERING ----------------
        window.clear(sf::Color::Black);

        window.draw(sideScreen);

        ScoreText.setString("Score: " + std::to_string(score));
        ScoreText.setPosition(610.f, 320.f);
        window.draw(ScoreText);

        LivesText.setString("Lives: " + std::to_string(LIVES));
        LivesText.setPosition(610.f, 280.f);
        window.draw(LivesText);

        int displayTime = (int)timeRemaining;
        if (displayTime < 0) displayTime = 0;
        TimerText.setString("Time: " + std::to_string(displayTime));
        TimerText.setPosition(610.f, 240.f);
        window.draw(TimerText);

        // Maze
        sf::Sprite spriteWall(texWall), spriteFood(texFood),
            spritePellet(texPellet), spriteBooster(texBooster);
        for (int i = 0; i < 20; i++)
        {
            for (int j = 0; j < 30; j++)
            {
                char c = arr[i][j];
                if (c == 'W')
                {
                    spriteWall.setPosition(j * 20.f, i * 20.f);
                    window.draw(spriteWall);
                }
                else if (c == 'f')
                {
                    spriteFood.setPosition(j * 20.f, i * 20.f);
                    window.draw(spriteFood);
                }
                else if (c == 'X')
                {
                    spritePellet.setPosition(j * 20.f, i * 20.f);
                    window.draw(spritePellet);
                }
                else if (c == 'B')
                {
                    spriteBooster.setPosition(j * 20.f, i * 20.f);
                    window.draw(spriteBooster);
                }
            }
        }

        // Player
        sf::Sprite spritePlayer;
        switch (Direction)
        {
        case 'u': spritePlayer.setTexture(texUp);    break;
        case 'l': spritePlayer.setTexture(texLeft);  break;
        case 'd': spritePlayer.setTexture(texDown);  break;
        default:  spritePlayer.setTexture(texPlayer); break;
        }
        spritePlayer.setPosition(y * 20.f, x * 20.f);
        window.draw(spritePlayer);

        // Ghosts
        for (int i = 0; i < 4; i++)
        {
            sf::Sprite ghostSprite;
            if (gcounter)
            {
                ghostSprite.setTexture(texGhostFrightened);
            }
            else
            {
                if (i == 0) ghostSprite.setTexture(texGhostGreen);
                if (i == 1) ghostSprite.setTexture(texGhostYellow);
                if (i == 2) ghostSprite.setTexture(texGhostRed);
                if (i == 3) ghostSprite.setTexture(texGhostBlue);
            }
            ghostSprite.setPosition(gx[i] * 20.f, gy[i] * 20.f);
            window.draw(ghostSprite);
        }

        window.display();
    }

    // ---------------- GAME OVER or VICTORY SCREEN ----------------
    window.clear();
    if (!victory)
    {
        // Game Over
        gameOverSprite.setPosition(0, 0);
        window.draw(gameOverSprite);

        sf::Text finalScoreText = ScoreText;
        finalScoreText.setString("Score: " + std::to_string(score));
        finalScoreText.setPosition(250.f, 162.f);
        window.draw(finalScoreText);

        sf::Text finalTimeText = TimerText;
        if (timeRemaining <= 0)
            finalTimeText.setString("Time's Up!");
        else
            finalTimeText.setString("Time Left: " + std::to_string((int)timeRemaining));
        finalTimeText.setPosition(250.f, 200.f);
        window.draw(finalTimeText);
    }
    else
    {
        // Victory
        victorySprite.setPosition(0, 0);
        window.draw(victorySprite);

        sf::Text finalScoreText = ScoreText;
        finalScoreText.setString("Final Score: " + std::to_string(score));
        finalScoreText.setPosition(250.f, 162.f);
        window.draw(finalScoreText);
    }

    window.display();

    // Keep final screen visible
    bool exitGame = false;
    while (!exitGame && window.isOpen())
    {
        sf::Event e;
        while (window.pollEvent(e))
        {
            if (e.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                exitGame = true;
        }
        sf::sleep(sf::milliseconds(50));
    }

    return 0;
}
