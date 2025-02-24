#include "Window.h"

#include "config.h"
#include <cstring>
#include <cstdio>
#include <ctime>
#include <cmath>


Window::Window()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        return;
    }

    loadConfig();

    const int SCREEN_WIDTH = m_config.boardWidth * TILE_SIZE;
    const int SCREEN_HEIGHT = m_config.boardHeight* TILE_SIZE + STATUS_BAR_HEIGHT;


    int rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH,SCREEN_HEIGHT,0,&m_window,&m_renderer);
    if(rc != 0) {
        SDL_Quit();
        printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
        return;
    };
    m_scrtex = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_ARGB8888,
                                   SDL_TEXTUREACCESS_STREAMING,
                                   SCREEN_WIDTH, SCREEN_HEIGHT);
    m_screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
                              0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    m_charset = SDL_LoadBMP("cs8x8.bmp");
    m_snakeBMP = SDL_LoadBMP("snake.bmp");

    SDL_SetColorKey(m_charset, true, 0x000000);
    m_timer = 0;

    srand(time(NULL));
    m_gameOver = false;
    m_quit = false;
    m_activePlayerTextInput = false;
    m_currentInputLength = 0;
    m_currentScore = 0;

    memset(m_currentPlayerInput, 0, MAX_NICKNAME_LENGTH);

}

Window::~Window()
{
    SDL_FreeSurface(m_screen);
    SDL_FreeSurface(m_charset);
    SDL_FreeSurface(m_snakeBMP);
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}
void Window::drawStatic(int x,int y,Texture textureNr,float scale) {
    //size of single texture in pixels
    const int textureSize = 16;

    SDL_Rect src = {textureNr * textureSize,0,textureSize,textureSize};

    SDL_Rect dest;
    dest.w = TILE_SIZE * scale;
    dest.h = TILE_SIZE * scale;

    // Adjust x and y to keep the pulsation centered
    dest.x = x - (dest.w - TILE_SIZE) / 2;
    dest.y = y - (dest.h - TILE_SIZE) / 2;

    SDL_BlitScaled(m_snakeBMP, &src, m_screen, &dest);
}
void Window::drawDots(int x,int y,Texture textureNr) {
    // Size of a single texture in pixels
    const int textureSize = 16;

    const float pulsationPeriod = 1.0f;
    const float pulseScale = 0.75f + 0.125f * (1 + sin(m_elapsedTime * 2 * M_PI / pulsationPeriod));

    // Source rectangle from the texture
    SDL_Rect src = {textureNr * textureSize, 0, textureSize, textureSize};

    SDL_Rect dest;
    dest.w = TILE_SIZE * pulseScale;
    dest.h = TILE_SIZE * pulseScale;

    // Adjust x and y to keep the pulsation centered
    dest.x = x - (dest.w - TILE_SIZE) / 2;
    dest.y = y - (dest.h - TILE_SIZE) / 2;

    // Draw the scaled texture
    SDL_BlitScaled(m_snakeBMP, &src, m_screen, &dest);

}
void Window::drawString(int x, int y, const char *text) {
    const int scale = 2;
    int px, py, c;
    SDL_Rect s, d;
    s.w = 8;
    s.h = 8;
    d.w = 8 * scale;
    d.h = 8 * scale;

    while (*text) {
        c = *text & 255;
        px = (c % 16) * 8;
        py = (c / 16) * 8;
        s.x = px;
        s.y = py;
        d.x = x;
        d.y = y;

        SDL_BlitScaled(m_charset, &s, m_screen, &d);

        x += 8 * scale;
        text++;
    }
}
void Window::displayHighScoreScreen() {
    SDL_FillRect(m_screen, NULL, SDL_MapRGB(m_screen->format, 0x00, 0x00, 0x00));

    const HighScore * highscores= m_highScoreManager.getHighScores();
    int highScoresCount = m_highScoreManager.getCurrentHighScoreCount();

    drawString(10, 10, "High Scores:");
    for (int i = 0; i < highScoresCount; ++i) {
        char text[64];
        snprintf(text, sizeof(text), "%d. %s %d", i + 1, highscores[i].name, highscores[i].score);
        drawString(10, 40 + (i * 30), text);
    }
    drawString(10, 150, "Press 'N' for a new game or 'ESC' to quit.");
}
void Window::loadConfig() {
    const char *configFilename = "config.txt";
    FILE *file = fopen(configFilename, "r");
    if (!file) {
        printf("Failed to open config file: %s\n", configFilename);
        return;
    }

    char line[128];
    while (fgets(line, sizeof(line), file)) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "\n");

        if (key && value) {
            if (strcmp(key, "boardWidth") == 0) m_config.boardWidth = atoi(value);
            else if (strcmp(key, "boardHeight") == 0) m_config.boardHeight = atoi(value);
            else if (strcmp(key, "initialSpeed") == 0) m_config.initialSpeed = atof(value);
            else if (strcmp(key, "initialSnakeLength") == 0) m_config.initialSnakeLength = atoi(value);
            else if (strcmp(key, "bonusFrequency") == 0) m_config.bonusFrequency = atoi(value);
            else if (strcmp(key, "pointsPerRedDot") == 0) m_config.pointsPerRedDot = atoi(value);
            else if (strcmp(key, "pointsPerBlueDot") == 0) m_config.pointsPerBlueDot = atoi(value);
            else if (strcmp(key, "speedUpFactor") == 0) m_config.speedUpFactor = atof(value);
            else if (strcmp(key, "slowDownFactor") == 0) m_config.slowDownFactor = atof(value);
        }
    }

    fclose(file);
}

void Window::processTextInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            m_quit = true;
        }

        if (m_activePlayerTextInput) {
            if (event.type == SDL_TEXTINPUT) {
                // Reject spaces and append text only if valid
                if (m_currentInputLength < MAX_NICKNAME_LENGTH - 1 &&
                    strchr(event.text.text, ' ') == NULL)
                    {
                        strcat(m_currentPlayerInput, event.text.text);
                        m_currentInputLength += strlen(event.text.text);
                    }
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_BACKSPACE && m_currentInputLength > 0) {
                    // Remove the last character
                    m_currentPlayerInput[--m_currentInputLength] = '\0';
                } else if (event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_KP_ENTER) {
                    // When Enter is pressed, finalize the input
                    m_activePlayerTextInput = false;
                    SDL_StopTextInput();
                    // Save the nickname in the high score manager or perform other actions
                    m_highScoreManager.addHighScore(m_currentPlayerInput, m_currentScore);
                }
            }
        }

    }
}
void Window::displayInputScreen() {
    SDL_FillRect(m_screen, NULL, SDL_MapRGB(m_screen->format, 0x00, 0x00, 0x00));
    drawString(10, 10, "Congratulations! You achieved a high score.");
    drawString(10, 30, "Please enter your nickname and press ENTER:");
    drawString(10, 50, m_currentPlayerInput);
}
void Window::run(){
    int czarny = SDL_MapRGB(m_screen->format, 0x00, 0x00, 0x00);

    m_t1 = SDL_GetTicks();
    while (!m_quit) {
        m_t2 = SDL_GetTicks();
        m_delta = (m_t2 - m_t1) * 0.001;
        m_t1 = m_t2;

        m_timer += m_delta;
        m_elapsedTime += m_delta;

        SDL_FillRect(m_screen, NULL, czarny);

        if(m_activePlayerTextInput) {
            displayInputScreen();
            processTextInput();
        }
        else if(m_displayHighScoreScreen) {
            displayHighScoreScreen();
            processEvents();
        }
        else if(!m_gameOver) {
            processEvents();
            onUpdate();
            onRender();
            drawInterface();
        }

        SDL_UpdateTexture(m_scrtex, NULL, m_screen->pixels, m_screen->pitch);
        SDL_RenderCopy(m_renderer, m_scrtex, NULL, NULL);
        SDL_RenderPresent(m_renderer);
    }
}