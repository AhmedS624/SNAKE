#include <SFML/Audio.hpp>
#include <clocale>
#include <ncurses.h>
#include <unistd.h> // For usleep() function
#include <list>
#include <cstdlib> // for rand()

void shootBullet(int bulletDirection, int &bulletX, int &bulletY, wchar_t *screen, int nScreenWidth, int nScreenHeight);

// this copy is for expermentation (add bullets, changing the input, speed, sprites   )

int main()
{
  // Initialize ncurses
  initscr();
  cbreak();    // Disable line buffering
  noecho();    // Don't echo characters to the screen
  curs_set(0); // Hide the cursor
  setlocale(LC_ALL, "");

  // Set the window to non-blocking mode so getch doesn't block runtime
  nodelay(stdscr, TRUE);

  // Define your screen width and height
  int nScreenWidth = 120;
  int nScreenHeight = 40;
  int speed{100000};
  // Create a window of nScreenWidth columns and nScreenHeight rows
  //    WINDOW *win = newwin(nScreenHeight, nScreenWidth, 0, 0);

  struct sSnakeSegment
  {
    int x{};
    int y{};
  };
  // Allocate memory for the screen buffer
  wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];

  // Load the sound files
  sf::SoundBuffer death;
  sf::SoundBuffer womp;
  if (!death.loadFromFile("snake_death.wav") || !womp.loadFromFile("Womp.wav"))
  {
    // Failed to load the sound file
    endwin();        // End ncurses
    delete[] screen; // Free the memory allocated for the screen buffer
    return EXIT_FAILURE;
  }

  sf::Sound sound;
  sound.setBuffer(death);

  sf::Sound eat;
  eat.setBuffer(womp);

  while (true)
  {
    std::list<sSnakeSegment> snake = {{60, 15}, {61, 15}, {62, 15}, {63, 15}, {64, 15}, {65, 15}, {66, 15}, {67, 15}, {68, 15}, {69, 15}};
    int foodX{30};
    int foodY{15};
    int score{0};
    int snakeDirection{3};
    bool dead{false};

    // New shit

    int bulletX{15};
    int bulletY{15};
    int bulletDirection{3};

    // Game loop
    while (!dead)
    {
      usleep(speed); // Sleep for 100 milliseconds (adjust as needed)

      // Get Input
      int ch = getch();
      // Check if getch() returned a character or ERR
      if (ch != ERR)
      {
        switch (ch)
        {
        case 'a':
          if (snakeDirection != 1)
            snakeDirection = 3;
          break;

        case 'd':
          if (snakeDirection != 3)
            snakeDirection = 1;
          break;
        case 'w':
          if (snakeDirection != 2)
            snakeDirection = 0;
          break;

        case 's':
          if (snakeDirection != 0)
            snakeDirection = 2;
          break;
        case 'f':
          speed = 50000;
          break;
        case 27: // Escape key
          dead = true;
          break;
        }
      }
      else
        speed = 100000;

      // Game logic

      // Update Snake Position
      switch (snakeDirection)
      {
      case 0: // UP
        snake.push_front({snake.front().x, snake.front().y - 1});
        break;
      case 1: // RIGHT
        snake.push_front({snake.front().x + 1, snake.front().y});
        break;
      case 2: // DOWN
        snake.push_front({snake.front().x, snake.front().y + 1});
        break;
      case 3: // LEFT
        snake.push_front({snake.front().x - 1, snake.front().y});
        break;
      }

      // Collison Detection
      // Snake vs Window
      if (snake.front().x <= 0 || snake.front().x >= nScreenWidth - 1)
      {
        dead = true;
      }
      if (snake.front().y <= 3 || snake.front().y >= nScreenHeight - 1)
      {
        dead = true;
      }

      // Snake and bullets vs Food

      if (snake.front().x == foodX && snake.front().y == foodY || bulletX == foodX && bulletY == foodY)
      {
        score++;
        while (screen[foodY * nScreenWidth + foodX] != ' ')
        {
          eat.play();
          foodX = rand() % nScreenWidth;
          foodY = (rand() % nScreenHeight - 2) + 3;
        }
        for (int i{0}; i < 5; i++)
        {
          snake.push_back({snake.back().x, snake.back().y});
        }
      }

      // Snake vs Snake body
      for (auto segment = snake.begin(); segment != snake.end(); segment++)
      {
        if (segment != snake.begin() && segment->x == snake.front().x && segment->y == snake.front().y)
          dead = true;
      }
      // Check for snake death
      if (dead)
      {
        sound.play(); // Play the death sound
      }
      // Cut off the Tail Of The Sanke

      snake.pop_back();

      // Display to the player

      // Update the screen buffer (Example: Fill with 'X' characters)
      for (int i = 0; i < nScreenWidth * nScreenHeight; ++i)
      {
        screen[i] = ' ';
      }

      // Draw Border
      for (int i = 0; i < nScreenWidth; i++)
      {
        screen[i] = '=';
        screen[2 * nScreenWidth + i] = '=';
        screen[(nScreenHeight - 1) * nScreenWidth + i] = '=';
      }

      for (int i = 0; i < nScreenHeight; i++)
      {
        screen[i * nScreenWidth] = '|';
        screen[i * nScreenWidth - 1] = '|';
      }

      // Draw Snake Body
      for (auto s : snake)
      {
        screen[s.y * nScreenWidth + s.x] = dead ? '+' : 'O';
      }
      // Draw Snake Head
      screen[snake.front().y * nScreenWidth + snake.front().x] = dead ? '#' : '@';

      // Draw Food

      screen[foodY * nScreenWidth + foodX] = '$';

      // Draw Bullets

      if (ch != ERR && ch == 'c')
        switch (snakeDirection)
        {
        case 0:
          bulletX = snake.front().x;
          bulletY = snake.front().y - 1;
          bulletDirection = snakeDirection;
          screen[bulletY * nScreenWidth + bulletX] = '.';
          break;
        case 1:
          bulletX = snake.front().x + 1;
          bulletY = snake.front().y;
          bulletDirection = snakeDirection;
          screen[bulletY * nScreenWidth + bulletX] = '.';
          break;
        case 2:
          bulletX = snake.front().x;
          bulletY = snake.front().y + 1;
          bulletDirection = snakeDirection;
          screen[bulletY * nScreenWidth + bulletX] = '.';
          break;
        case 3:
          bulletX = snake.front().x - 1;
          bulletY = snake.front().y;
          bulletDirection = snakeDirection;
          screen[bulletY * nScreenWidth + bulletX] = '.';
          break;
        }

      // Move bullets

      shootBullet(bulletDirection, bulletX, bulletY, screen, nScreenWidth, nScreenHeight);


      // Print the screen buffer to the terminal
      for (int y = 0; y < nScreenHeight; ++y)
      {
        for (int x = 0; x < nScreenWidth; ++x)
        {
          mvaddch(y, x, screen[y * nScreenWidth + x]);
        }
      }
      // Draw title and score
      mvprintw(1, (nScreenWidth - 5) / 2, "SNAKE");
      mvprintw(1, (nScreenWidth - 10), "SCORE: %d", score);
      mvprintw(1, 1, "X: %d", snake.front().x);
      mvprintw(1, 7, "Y: %d", snake.front().y);

      if (dead)
      {

        mvprintw(5, 5, "PRESS 'SPACE' TO PLAY AGAIN");
        mvprintw(3, 5, "PRESS 'ESC' TO QUIT");
      }
      // Refresh the screen to display the changes
      refresh();
    }
    // Press space to play again or Esc to quit
    int playAgainKey{};
    while ((playAgainKey = getch()) != ' ' && playAgainKey != 27)
      ;

    // If Esc key is pressed, break out of the outer while loop
    if (playAgainKey == 27)
      break;
  }
  // End ncurses
  endwin();

  // Free the memory allocated for the screen buffer
  delete[] screen;
  return 0;
}

void shootBullet(int bulletDirection, int &bulletX, int &bulletY, wchar_t *screen, int nScreenWidth, int nScreenHeight)
{
  switch (bulletDirection)
  {
  case 0: // UP
    if (bulletY > 3)
    {
      screen[bulletY * nScreenWidth + bulletX] = ' ';
      bulletY--;
      screen[bulletY * nScreenWidth + bulletX] = '.';
    }
    break;
  case 1: // RIGHT
    if (bulletX < nScreenWidth - 2)
    {
      screen[bulletY * nScreenWidth + bulletX] = ' ';
      bulletX++;
      screen[bulletY * nScreenWidth + bulletX] = '.';
    }
    break;
  case 2: // DOWN
    if (bulletY < nScreenHeight - 2)
    {
      screen[bulletY * nScreenWidth + bulletX] = ' ';
      bulletY++;
      screen[bulletY * nScreenWidth + bulletX] = '.';
    }
    break;
  case 3: // LEFT
    if (bulletX > 1)
    {
      screen[bulletY * nScreenWidth + bulletX] = ' ';
      bulletX--;
      screen[bulletY * nScreenWidth + bulletX] = '.';
    }
    break;
  }
}
