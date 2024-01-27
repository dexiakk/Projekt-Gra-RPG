#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <fstream>

// Deklaracja zmiennej globalnej przechowuj¹cej pozycjê gracza
sf::Vector2f playerPosition(400.f, 300.f);

sf::Vector2f weaponPosition(0.f, 0.f);

class Mob {
public:
    Mob(const sf::Texture& texture, float speed) : sprite(texture), speed(speed) {
        // Ustawienie prêdkoœci moba
        velocity.x = speed;
        velocity.y = speed;
    }

    void spawn(float playerX, float playerY) {
        // Ustawienie pozycji moba w losowym miejscu
        sprite.setPosition(rand() % 16000, rand() % 12000);
        isFollowing = false;
        health = 100; // Dodane - pocz¹tkowe zdrowie moba
    }

    // Nowa funkcja zwracaj¹ca pozycjê moba
    sf::Vector2f getPosition() const {
        return sprite.getPosition();
    }

    // Nowa funkcja zwracaj¹ca zdrowie moba
    int getHealth() const {
        return health;
    }

    // Nowa funkcja zmniejszaj¹ca zdrowie moba
    void decreaseHealth(int amount) {
        health -= amount;
        if (health < 0) {
            health = 0;
        }
    }

    // Nowa funkcja do zadawania obra¿eñ mobowi
    void takeDamage(int damage) {
        health -= damage;
        if (health <= 0) {
            isFollowing = false; // Jeœli mob straci ca³e zdrowie, przestaje œledziæ gracza
            health = 0; // Ustawienie zdrowia na 0, aby unikn¹æ wartoœci ujemnej
        }
    }

    // Nowa funkcja do resetowania moba po teleportacji
    void resetMob() {
        sprite.setPosition(rand() % 1920, rand() % 1080);
        isFollowing = false;
        health = 100; // Przywrócenie pe³nego zdrowia moba
    }

    void update(float dt, const sf::Vector2f& playerPosition) {
        if (!isFollowing) {
            // SprawdŸ odleg³oœæ od gracza
            sf::Vector2f direction = playerPosition - sprite.getPosition();
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            // Jeœli odleg³oœæ jest mniejsza ni¿ 200, zacznij pod¹¿aæ za graczem
            if (distance < 600) {
                isFollowing = true;
            }
        }

        if (isFollowing) {
            // Pod¹¿anie za graczem tylko, gdy isFollowing jest ustawione na true
            sf::Vector2f direction = playerPosition - sprite.getPosition();
            float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            if (length != 0) {
                direction /= length;
            }
            sprite.move(direction * speed * dt);
        }

        // Sprawdzenie granic ekranu
        sf::Vector2f position = sprite.getPosition();
    }

    sf::Sprite getSprite() const {
        return sprite;
    }



private:
    sf::Sprite sprite;
    sf::Vector2f velocity;
    float speed;

    bool isFollowing; // Nowa flaga okreœlaj¹ca, czy mob œledzi gracza
    int health; // Nowa zmienna przechowuj¹ca zdrowie moba
};

class HealthBar {
public:
    HealthBar(const sf::Vector2f& position, float maxWidth, float height)
        : position(position), maxWidth(maxWidth), height(height) {
        // Ustawienie koloru i pozycji paska zdrowia
        bar.setFillColor(sf::Color::Green);
        bar.setSize(sf::Vector2f(maxWidth, height));
        bar.setPosition(position);
    }

    // Funkcja aktualizuj¹ca pasek zdrowia
    void update(float currentHealth, float maxHealth) {
        float ratio = currentHealth / maxHealth;
        bar.setSize(sf::Vector2f(maxWidth * ratio, height));
    }

    // Funkcja aktualizuj¹ca pozycjê paska zdrowia
    void updatePosition(const sf::Vector2f& newPosition) {
        position = newPosition;
        bar.setPosition(position);
    }

    // Funkcja rysuj¹ca pasek zdrowia
    void draw(sf::RenderWindow& window) const {
        window.draw(bar);
    }

private:
    sf::Vector2f position;
    float maxWidth;
    float height;
    sf::RectangleShape bar;
};

class Weapon {
public:
    Weapon(const sf::Texture& texture, float offset) : sprite(texture), offset(offset) {}

    void spawn(const sf::Vector2f& playerPos) {
        // Ustawienie pozycji broni obok gracza na podstawie przesuniêcia
        initialPosition = playerPos + sf::Vector2f(offset, 0.f);
        sprite.setPosition(initialPosition);

        sprite.setScale(0.2f, 0.2f);
    }

    void update(const sf::Vector2f& playerPos) {
        // Ustawienie pozycji broni na sta³¹ odleg³oœæ od gracza
        sprite.setPosition(playerPos + sf::Vector2f(offset, 0.f));

        // Obrót broni po naciœniêciu spacji
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !isWeaponRotated) {
            sprite.rotate(320); // Obrót o 90 stopni
            isWeaponRotated = true; // Ustawienie stanu obrotu na true
        }

        // Zresetowanie broni do pocz¹tkowej pozycji, gdy spacja jest zwolniona
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            isWeaponRotated = false; // Ustawienie stanu obrotu na false
            sprite.setRotation(0); // Zresetowanie obrotu do pocz¹tkowej pozycji
        }
    }

    sf::Sprite getSprite() const {
        return sprite;
    }

    void reset() {
        // Zresetowanie broni do pocz¹tkowej pozycji
        sprite.setPosition(initialPosition);
    }

private:
    sf::Sprite sprite;
    float offset;
    sf::Vector2f initialPosition;
    bool isWeaponRotated = false;
};





class Statistics {
public:
    Statistics() {
        loadStatsFromFile();
    }

    // Funkcja zapisuj¹ca statystyki gracza po zabiciu moba
    void savePlayerStats(const std::string& playerName) {
        if (playerStats.find(playerName) != playerStats.end()) {
            playerStats[playerName]++;
        }
        else {
            playerStats[playerName] = 1;
        }

        saveStatsToFile();
    }

    // Funkcja wyœwietlaj¹ca statystyki dla konkretnego gracza
    void displayStats(const std::string& playerName, int& moby) const {
        auto it = playerStats.find(playerName);
        if (it != playerStats.end()) {
            moby = it->second;
        }

    }


private:
    std::map<std::string, int> playerStats;

    // Funkcja wczytuj¹ca statystyki graczy z pliku
    void loadStatsFromFile() {
        std::ifstream statsFile("statistics.txt");
        if (statsFile.is_open()) {
            std::string playerName;
            int kills;

            while (statsFile >> playerName >> kills) {
                playerStats[playerName] = kills;
            }

            statsFile.close();
        }
    }

    // Funkcja zapisuj¹ca statystyki graczy do pliku
    void saveStatsToFile() const {
        std::ofstream statsFile("statistics.txt");
        if (statsFile.is_open()) {
            for (const auto& entry : playerStats) {
                statsFile << entry.first << " " << entry.second << std::endl;
            }

            statsFile.close();
        }
    }
};

class Mission {
public:
    Mission(const sf::Texture& texture, const sf::Vector2u& windowSize, const std::string& playerName)
        : sprite(texture), windowSize(windowSize) {
        // Ustawienie skali i pocz¹tkowej pozycji obrazka
        sprite.setScale(0.6f, 0.5f); // Ustawienie mniejszej skali obrazka
        // Ustawienie obrazka w prawym górnym rogu ekranu
        updatePosition(sf::Vector2f(0.f, 0.f), playerName);
    }
    sf::Font font;
    sf::Font font2;
    sf::Vector2u windowSize; // Rozmiar okna
    sf::Text titleText;


    void updatePosition(const sf::Vector2f& playerPos, const std::string& playerName) {
        int moby = 0;
        int todo = 0;
        Statistics gameStats;
        gameStats.displayStats(playerName, moby);
        int lvl = 0;

        if (moby < 10) {
            todo = 10;
            lvl = 1;
        }
        else if (moby >= 10 && moby < 100) {
            todo = 100;
            lvl = 2;
        }
        else if (moby >= 100 && moby < 300) {
            todo = 300;
            lvl = 3;
        }

        int left = todo - moby;
        std::string todoStr = std::to_string(left);
        std::string level = "\n         Poziom " + std::to_string(lvl);


        font.loadFromFile("C:/Windows/Fonts/arial.ttf");
        font2.loadFromFile("KnightWarrior.otf");
        titleText.setFont(font2);
        titleText.setCharacterSize(30);
        titleText.setFillColor(sf::Color::Black);
        // Ustawienie obrazka w prawym górnym rogu ekranu z uwzglêdnieniem pozycji gracza
        float x = playerPos.x + (windowSize.x / 2) - sprite.getGlobalBounds().width;
        float y = playerPos.y - 530; // Niewielkie przesuniêcie od górnej krawêdzi
        sprite.setPosition(x, y);
        titleText.setString("           MISJA\nZabij potwory: " + todoStr + level);
        titleText.setPosition(x + 240, y + 40);

    }
    sf::Sprite getSprite() const {
        return sprite;
    }


private:
    sf::Sprite sprite;

};

int Game(const std::string& playerName) {
    // Tworzenie okna
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "RPG");

    // £adowanie tekstury trawy
    sf::Texture grassTexture;
    if (!grassTexture.loadFromFile("grass_texture.png")) {
        // Obs³uga b³êdu ³adowania tekstury
        return -1;
    }

    // Pobranie rozmiarów tekstury trawy
    sf::Vector2u textureSize = grassTexture.getSize();

    // Utworzenie mapy powielaj¹c teksturê
    const int mapMultiplier = 10; // Mno¿nik powiêkszenia mapy
    const int mapWidth = textureSize.x * mapMultiplier;
    const int mapHeight = textureSize.y * mapMultiplier;

    sf::RenderTexture renderTexture;
    renderTexture.create(mapWidth, mapHeight);
    renderTexture.clear();

    // Ustawienie powielanej tekstury
    sf::Sprite sprite(grassTexture);
    for (int i = 0; i < mapMultiplier; ++i) {
        for (int j = 0; j < mapMultiplier; ++j) {
            sprite.setPosition(i * textureSize.x, j * textureSize.y);
            renderTexture.draw(sprite);
        }
    }
    bool check = false;
    renderTexture.display();


    // Tworzenie gracza
    sf::Texture playerTexture;
    if (!playerTexture.loadFromFile("player_texture.png")) {
        // Obs³uga b³êdu ³adowania tekstury gracza
        return -1;
    }
    sf::Sprite playerSprite(playerTexture);
    playerSprite.setScale(0.2f, 0.2f); // Ustawienie rozmiaru gracza


    sf::Vector2f playerPosition(400.f, 300.f); // Pozycja pocz¹tkowa gracza
    // Wczytanie pozycji gracza z pliku
    std::ifstream inFile("player_position.txt");
    inFile.seekg(0, std::ios::end);
    if (inFile.tellg() != 0) {
        // Plik nie jest pusty, wiêc wracamy na pocz¹tek i wczytujemy dane
        inFile.seekg(0, std::ios::beg);
        inFile >> playerPosition.x >> playerPosition.y;
        inFile.close();
    }
    else {
        std::cerr << "File is empty or cannot be opened for reading player position. Using default position." << std::endl;
    }

    // Tworzenie mobów
    sf::Texture mobTexture;
    if (!mobTexture.loadFromFile("bat.png")) {
        // Obs³uga b³êdu ³adowania tekstury moba
        return -1;
    }

    std::vector<Mob> mobs;
    for (int i = 0; i < 100; ++i) {
        mobs.emplace_back(mobTexture, 1.5f);
        mobs.back().spawn(playerPosition.x, playerPosition.y);
    }

    // Tworzenie broni
    sf::Texture weaponTexture;
    if (!weaponTexture.loadFromFile("sword.png")) {
        // Obs³uga b³êdu ³adowania tekstury broni
        return -1;
    }
    // Tworzenie obiektu Mission
    sf::Texture missionTexture;
    missionTexture.loadFromFile("papirus.png"); // Za³adowanie odpowiedniej tekstury
    Mission mission(missionTexture, window.getSize(), playerName);


    Weapon weapon(weaponTexture, 0.3f);
    weapon.spawn(playerPosition);

    // Ustawienia kamery
    sf::View view(sf::FloatRect(0, 0, 1920, 1080));
    view.setCenter(playerPosition);

    // Pozycje kamieni na mapie
    std::vector<sf::Vector2f> stonePositions;
    std::vector<int> stoneIndices;  // Indeksy kamieni
    sf::Texture stoneTexture;
    if (!stoneTexture.loadFromFile("stone_texture.png")) {
        // Obs³uga b³êdu ³adowania tekstury gracza
        return -1;
    }
    // Wczytaj pozycje kamieni z pliku lub rozmieœæ losowo, jeœli plik jest pusty
    std::ifstream stoneFile("stone_positions.txt");
    if (stoneFile.peek() == std::ifstream::traits_type::eof()) {
        // Plik jest pusty, rozmieœæ kamienie losowo
        srand(static_cast<unsigned>(time(0))); // Inicjalizacja losowego rozk³adu
        for (int i = 0; i < mapWidth; i += 700) {
            for (int j = 0; j < mapHeight; j += 700) {
                if (rand() % 17 == 0) { // Rzadsze pojawianie siê kamieni
                    stonePositions.push_back(sf::Vector2f(static_cast<float>(i), static_cast<float>(j)));
                    stoneIndices.push_back(static_cast<int>(stonePositions.size()) - 1);  // Dodaj indeks kamienia
                }
            }
        }
    }
    else {
        // Plik nie jest pusty, wczytaj pozycje kamieni z pliku
        while (!stoneFile.eof()) {
            float x, y;
            int index;
            stoneFile >> x >> y >> index;
            stonePositions.push_back(sf::Vector2f(x, y));
            stoneIndices.push_back(index);
        }
    }
    stoneFile.close();
    sf::Vector2f previousPlayerPosition;

    // Tworzenie kwiatków na mapie
    sf::Texture plantTexture;
    if (!plantTexture.loadFromFile("plant_texture.png")) {
        // Obs³uga b³êdu ³adowania tekstury kwiatka
        return -1;
    }
    sf::Sprite plantSprite(plantTexture);
    plantSprite.setScale(0.15f, 0.15f); // Zmniejszenie rozmiaru kwiatka

    // Pozycje kwiatków na mapie
    std::vector<sf::Vector2f> plantPositions;
    srand(static_cast<unsigned>(time(0))); // Inicjalizacja losowego rozk³adu
    for (int i = 0; i < mapWidth; i += 100) {
        for (int j = 0; j < mapHeight; j += 100) {
            if (rand() % 15 == 0) { // Rzadsze pojawianie siê kwiatków
                plantPositions.push_back(sf::Vector2f(static_cast<float>(i), static_cast<float>(j)));
            }
        }
    }

    HealthBar playerHealthBar(sf::Vector2f(20.f, 20.f), 100.f, 10.f);

    // Tworzenie paska zdrowia dla mobów
    std::vector<HealthBar> mobHealthBars;
    for (const auto& mob : mobs) {
        mobHealthBars.emplace_back(mob.getPosition(), 50.f, 5.f);
    }

    // G³ówna pêtla gry
    while (window.isOpen()) {
        // Obs³uga zdarzeñ
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        // Spowolnienie poruszania siê gracza
        float playerSpeed = 2.0f;

        Statistics gameStatistics;

        // Poruszanie siê gracza
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
            playerSpeed = playerSpeed + 1.2;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A) && playerPosition.x > 0) {
            playerPosition.x -= playerSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D) && playerPosition.x < mapWidth) {
            playerPosition.x += playerSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W) && playerPosition.y > 0) {
            playerPosition.y -= playerSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S) && playerPosition.y < mapHeight) {
            playerPosition.y += playerSpeed;
        }

        // Aktualizacja mobów
        for (auto& mob : mobs) {
            mob.update(0.4f, playerPosition);
        }

        // Ustawianie i ograniczanie pozycji kamery na gracza
        sf::Vector2f viewCenter = playerPosition;
        viewCenter.x = std::max(viewCenter.x, 1920 / 2.f);
        viewCenter.x = std::min(viewCenter.x, mapWidth - 1920 / 2.f);
        viewCenter.y = std::max(viewCenter.y, 1080 / 2.f);
        viewCenter.y = std::min(viewCenter.y, mapHeight - 1080 / 2.f);
        view.setCenter(viewCenter);


        // Weryfikacja granic mapy dla gracza
        if (playerPosition.x < 0) playerPosition.x = 0;
        if (playerPosition.y < 0) playerPosition.y = 0;
        if (playerPosition.x > mapWidth) playerPosition.x = mapWidth;
        if (playerPosition.y > mapHeight) playerPosition.y = mapHeight;



        sf::Sprite stoneSprite(stoneTexture);
        stoneSprite.setScale(1.f, 1.f);

        // Kolizja gracza z kamieniem
        sf::FloatRect playerBounds(playerPosition.x, playerPosition.y, playerSprite.getGlobalBounds().width, playerSprite.getGlobalBounds().height);
        for (const auto& stoneIndex : stoneIndices) {
            const auto& stonePos = stonePositions[stoneIndex];
            sf::FloatRect stoneBounds(stonePos.x, stonePos.y, stoneSprite.getGlobalBounds().width - 30.f, stoneSprite.getGlobalBounds().height - 30.f);
            sf::FloatRect stoneRect(stonePos.x, stonePos.y, stoneSprite.getGlobalBounds().width + 10.f, stoneSprite.getGlobalBounds().height + 10.f);
            sf::FloatRect stoneRect2(stonePos.x, stonePos.y, stoneSprite.getGlobalBounds().width - 30.f, stoneSprite.getGlobalBounds().height - 30.f);
            if (playerBounds.intersects(stoneRect)) {
                std::cout << "Mozliwosc " << stoneIndex << std::endl;
            }
            if (playerBounds.intersects(stoneRect2)) {
                std::cout << "Mozliwosc " << stoneIndex << std::endl;
            }

            if (playerBounds.intersects(stoneBounds)) {
                std::cout << "Kolizja z kamieniem " << stoneIndex << std::endl;
                playerPosition = previousPlayerPosition;
            }
        }

        // Kolizja gracza z mobami
        for (auto& mob : mobs) {
            sf::FloatRect mobBounds = mob.getSprite().getGlobalBounds();
            sf::FloatRect playerBounds(playerPosition.x, playerPosition.y, playerSprite.getGlobalBounds().width, playerSprite.getGlobalBounds().height);

            // Sprawdzenie kolizji gracza z mobem
            if (playerBounds.intersects(mobBounds)) {
                // Uderzenie moba przez gracza (np. naciœniêcie spacji)
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                    int lvl = 1;
                    int potworki;
                    gameStatistics.displayStats(playerName, potworki);

                    if (potworki < 10) {
                        lvl = 1;
                    }
                    else if (potworki >= 10 && potworki < 100) {
                        lvl = 2;
                    }
                    else if (potworki >= 100 && potworki < 300) {
                        lvl = 3;
                    }
                    if (lvl == 1) {
                        mob.takeDamage(1.3f);
                    }
                    else if (lvl == 2) {
                        mob.takeDamage(3.3f);
                    }
                    else if (lvl == 3) {
                        mob.takeDamage(5.3f);
                    }



                    // Sprawdzenie, czy zdrowie moba spad³o do zera
                    if (mob.getHealth() == 0) {
                        gameStatistics.savePlayerStats(playerName); // Zapis statystyk gracza po zabiciu moba
                        mob.resetMob(); // Jeœli tak, zresetuj moba do losowej pozycji z pe³nym zdrowiem
                    }
                }
            }
        }

        // Aktualizacja paska zdrowia gracza
        playerHealthBar.update(100, 100); // Wartoœci przyk³adowe, do dostosowania do logiki gry

        // Aktualizacja pasków zdrowia mobów
        for (size_t i = 0; i < mobs.size(); ++i) {
            mobHealthBars[i].update(mobs[i].getHealth(), 100); // Wartoœci przyk³adowe, do dostosowania do logiki gry
        }

        // Aktualizacja pozycji pasków zdrowia gracza
        playerHealthBar.updatePosition(playerPosition);

        // Aktualizacja pozycji pasków zdrowia mobów
        for (size_t i = 0; i < mobs.size(); ++i) {
            mobHealthBars[i].updatePosition(mobs[i].getPosition());
        }

        // Aktualizacja pozycji broni
        weapon.update(playerPosition);
        mission.updatePosition(view.getCenter(), playerName);



        // Wyczyszczenie okna
        window.clear();

        // Ustawienie widoku kamery
        window.setView(view);



        // Narysowanie mapy z powielon¹ tekstur¹ trawy
        sf::Sprite mapSprite(renderTexture.getTexture());
        window.draw(mapSprite);

        // Ustawienie pozycji gracza
        playerSprite.setPosition(playerPosition);



        // Rysowanie pasków zdrowia
        playerHealthBar.draw(window);
        for (const auto& mobHealthBar : mobHealthBars) {
            mobHealthBar.draw(window);
        }

        // Narysowanie kwiatków na mapie
        for (const auto& plantPos : plantPositions) {
            plantSprite.setPosition(plantPos);
            window.draw(plantSprite);
        }


        // Narysowanie kamieni na mapie
        for (const auto& stoneIndex : stoneIndices) {
            const auto& stonePos = stonePositions[stoneIndex];
            stoneSprite.setPosition(stonePos);
            window.draw(stoneSprite);
        }



        // Narysowanie gracza
        window.draw(playerSprite);

        // Narysowanie broni na mapie
        window.draw(weapon.getSprite());


        // Narysowanie mobów na mapie
        for (const auto& mob : mobs) {
            window.draw(mob.getSprite());
        }

        window.draw(mission.getSprite());
        window.draw(mission.titleText);

        // Wyœwietlenie wszystkich rysunków
        window.display();
        previousPlayerPosition = playerPosition;
    }
    // Zapis pozycji kamieni do pliku
    std::ofstream stoneFileOut("stone_positions.txt");
    if (stoneFileOut.is_open()) {
        for (size_t i = 0; i < stonePositions.size(); ++i) {
            stoneFileOut << stonePositions[i].x << " " << stonePositions[i].y << " " << stoneIndices[i] << std::endl;
        }
        stoneFileOut.close();
    }
    else {
        std::cerr << "Unable to open file for writing stone positions." << std::endl;
    }
    //Zapis pozycji playera
    std::ofstream outFile("player_position.txt");
    if (outFile.is_open()) {
        outFile << playerPosition.x << " " << playerPosition.y << std::endl;
        outFile.close();
    }
    else {
        std::cout << "Unable to open file for writing player position." << std::endl;
    }

    return 0;
}


class LoginScreen {
public:
    std::string playerName;
    std::string password;
    bool isEnteringUsername;

    LoginScreen(sf::RenderWindow& window) : window(window) {
        setupLoginScreen();
    }



    std::string run() {
        isEnteringUsername = true;

        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (event.type == sf::Event::TextEntered) {
                    handleTextEntered(event.text.unicode);
                }

                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Return) {
                        if (isEnteringUsername) {
                            isEnteringUsername = false;
                            titleText.setString("Wprowadz haslo gracza");
                        }
                        else {
                            return playerName;
                        }
                    }
                }
            }

            window.clear(sf::Color::Black);
            window.draw(backgroundSprite); // Rysowanie t³a
            window.draw(titleText);

            if (isEnteringUsername) {
                window.draw(usernameText);
            }
            else {
                window.draw(passwordText);
                window.draw(usernameText);
            }
            window.display();
        }

        return "";
    }

private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Font font2;
    sf::Text titleText;
    sf::Text usernameText;
    sf::Text passwordText;
    sf::Texture backgroundTexture; // Tekstura dla t³a
    sf::Sprite backgroundSprite;   // Sprite dla t³a

    void setupLoginScreen() {
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cerr << "Error loading font." << std::endl;
            return;
        }
        if (!font2.loadFromFile("KnightWarrior.otf")) {
            std::cerr << "Error loading font." << std::endl;
            return;
        }
        if (!backgroundTexture.loadFromFile("background_image_menu.jpg")) {
            std::cerr << "Error loading background texture." << std::endl;
            return;
        }
        backgroundSprite.setTexture(backgroundTexture); // Ustawienie sprite'a t³a


        titleText.setString("Wprowadz nazwe gracza");
        titleText.setFont(font2);
        titleText.setCharacterSize(40);
        titleText.setPosition((window.getSize().x - titleText.getLocalBounds().width) / 2.f, 50.f);
        titleText.setFillColor(sf::Color::White);

        usernameText.setString("");
        usernameText.setFont(font);
        usernameText.setCharacterSize(30);
        usernameText.setPosition((window.getSize().x - usernameText.getLocalBounds().width) / 2.f, 200.f);
        usernameText.setFillColor(sf::Color::White);

        passwordText.setString("");
        passwordText.setFont(font);
        passwordText.setCharacterSize(30);
        passwordText.setPosition((window.getSize().x - passwordText.getLocalBounds().width) / 2.f, 250.f);
        passwordText.setFillColor(sf::Color::White);
    }

    void handleTextEntered(sf::Uint32 unicode) {
        sf::Text& currentText = isEnteringUsername ? usernameText : passwordText;
        std::string& currentInput = isEnteringUsername ? playerName : password;

        if (unicode == 8 && !currentInput.empty()) { // Backspace
            currentInput.pop_back();
        }
        else if (unicode >= 32 && unicode < 128) {
            currentInput += static_cast<char>(unicode);
        }

        std::string displayString = currentInput;
        if (!isEnteringUsername) {
            // Zast¹p znaki has³a gwiazdkami
            displayString = std::string(currentInput.length(), '*');
        }

        currentText.setString(displayString);
        currentText.setPosition((window.getSize().x - currentText.getLocalBounds().width) / 2.f,
            isEnteringUsername ? 200.f : 250.f);
    }
};
void showStats(const std::string& playerName) {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Statystyki");
    sf::Font font;
    sf::Font font2;
    sf::Text playerText;
    sf::Text titleText;
    sf::Text playerNick;
    sf::Text playerMob;
    sf::Text exitText;
    sf::Texture backgroundTexture; // Tekstura dla t³a
    sf::Sprite backgroundSprite;   // Sprite dla t³a
    int moby;
    font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    font2.loadFromFile("KnightWarrior.otf");


    Statistics gameStats;
    gameStats.displayStats(playerName, moby);

    std::string mobyStr = std::to_string(moby);

    titleText.setString("Statystyki");
    titleText.setFont(font2);
    titleText.setCharacterSize(60);
    titleText.setPosition((window.getSize().x - titleText.getLocalBounds().width) / 2.f, 50.f);
    titleText.setFillColor(sf::Color::White);
    std::cout << "ilosc mobow" << moby << std::endl;
    playerNick.setString(playerName);
    playerNick.setFont(font2);
    playerNick.setCharacterSize(50);
    playerNick.setPosition((window.getSize().x - playerNick.getLocalBounds().width) / 2.f, 150.f);
    playerNick.setFillColor(sf::Color::White);

    playerMob.setString("Ilosc zabitych potworów: " + mobyStr);
    playerMob.setFont(font);
    playerMob.setCharacterSize(35);
    playerMob.setPosition((window.getSize().x - playerMob.getLocalBounds().width) / 2.f, 250.f);
    playerMob.setFillColor(sf::Color::White);

    exitText.setString("Wyjscie");
    exitText.setFont(font);
    exitText.setCharacterSize(40);
    exitText.setPosition((window.getSize().x - exitText.getLocalBounds().width) / 2.f, 450.f);
    exitText.setFillColor(sf::Color::White);

    backgroundTexture.loadFromFile("background_image_stats.jpg");
    backgroundSprite.setTexture(backgroundTexture);


    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    sf::FloatRect exitBounds = exitText.getGlobalBounds();

                    if (exitBounds.contains(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y))) {
                        window.close();
                        return; // WyjdŸ z funkcji po zamkniêciu okna
                    }
                }
            }
        }



        window.clear();
        window.draw(backgroundSprite);
        window.draw(titleText);
        window.draw(playerNick);
        window.draw(playerMob);
        window.draw(exitText);
        window.display();
    }
}


class MainMenu {
public:
    MainMenu(sf::RenderWindow& window, const std::string& playerName)
        : window(window), playerName(playerName) {
        setupMenuOptions();
    }

    void run() {
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }

                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Left) {
                        handleMouseClick(event.mouseButton.x, event.mouseButton.y);
                    }
                }
            }

            window.clear(sf::Color::Black);
            window.draw(backgroundSprite); // Rysowanie t³a
            window.draw(titleText);
            window.draw(playGameText);
            window.draw(statisticsText);
            window.draw(exitText);
            window.display();
        }
    }

private:
    sf::RenderWindow& window;
    sf::Font font;
    sf::Font font2;
    sf::Text titleText;
    sf::Text playGameText;
    sf::Text statisticsText;
    sf::Text exitText;
    std::string playerName;
    sf::Texture backgroundTexture; // Tekstura dla t³a
    sf::Sprite backgroundSprite;   // Sprite dla t³a

    void setupMenuOptions() {
        if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            std::cerr << "Error loading font." << std::endl;
            return;
        }
        if (!font2.loadFromFile("KnightWarrior.otf")) {
            std::cerr << "Error loading font." << std::endl;
            return;
        }
        if (!backgroundTexture.loadFromFile("background_image_menu.jpg")) {
            std::cerr << "Error loading background texture." << std::endl;
            return;
        }
        backgroundSprite.setTexture(backgroundTexture); // Ustawienie sprite'a t³a

        titleText.setString("TAMER");
        titleText.setFont(font2);
        titleText.setCharacterSize(60);
        titleText.setPosition((window.getSize().x - titleText.getLocalBounds().width) / 2.f, 50.f);
        titleText.setFillColor(sf::Color::White);

        playGameText.setString("Graj jako " + playerName);
        playGameText.setFont(font);
        playGameText.setCharacterSize(40);
        playGameText.setPosition((window.getSize().x - playGameText.getLocalBounds().width) / 2.f, 150.f);
        playGameText.setFillColor(sf::Color::White);

        statisticsText.setString("Statystyki");
        statisticsText.setFont(font);
        statisticsText.setCharacterSize(40);
        statisticsText.setPosition((window.getSize().x - statisticsText.getLocalBounds().width) / 2.f, 250.f);
        statisticsText.setFillColor(sf::Color::White);

        exitText.setString("Wyjscie");
        exitText.setFont(font);
        exitText.setCharacterSize(40);
        exitText.setPosition((window.getSize().x - exitText.getLocalBounds().width) / 2.f, 350.f);
        exitText.setFillColor(sf::Color::White);
    }

    void handleMouseClick(int mouseX, int mouseY) {
        sf::Vector2f mousePos(static_cast<float>(mouseX), static_cast<float>(mouseY));

        sf::FloatRect playGameBounds = playGameText.getGlobalBounds();
        sf::FloatRect statisticsBounds = statisticsText.getGlobalBounds();
        sf::FloatRect exitBounds = exitText.getGlobalBounds();

        if (playGameBounds.contains(mousePos)) {
            std::cout << "Launching the game..." << std::endl;
            Game(playerName);
        }
        else if (statisticsBounds.contains(mousePos)) {
            std::cout << "Viewing statistics..." << std::endl;
            showStats(playerName);
        }
        else if (exitBounds.contains(mousePos)) {
            std::cout << "Exiting the game..." << std::endl;
            window.close();
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Tamer");
    LoginScreen loginScreen(window);
    std::string playerName = loginScreen.run();

    if (!playerName.empty()) {
        MainMenu mainMenu(window, playerName);
        mainMenu.run();
    }

    return 0;
}