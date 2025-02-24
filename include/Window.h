#pragma once
#include"SDL.h"
#include "HighScoreManager.h"
#include "config.h"

enum Texture {
        TEX_SNAKE_TAIL,
        TEX_SNAKE_BODY,
        TEX_SNAKE_HEAD,
        TEX_BRICK,
        TEX_BLUE_DOT,
        TEX_RED_DOT,
        TEX_PORTAL
    };

class Window{
private:
        void processTextInput();
        void displayInputScreen();
        void displayHighScoreScreen();
        void loadConfig();

public:
        Window();
        void run();
        ~Window();
protected:
        char m_currentPlayerInput[MAX_NICKNAME_LENGTH];
        int m_currentInputLength;
        int m_currentScore;

        int m_t1,m_t2;
        float m_delta,m_timer,m_elapsedTime;
        bool m_quit ;
        bool m_gameOver;
        bool m_displayHighScoreScreen = false;
        bool m_activePlayerTextInput;

        SDL_Window* m_window ;
        SDL_Renderer *m_renderer ;
        SDL_Surface* m_screen ;
        SDL_Surface* m_charset;
        SDL_Surface* m_snakeBMP;;
        SDL_Texture *m_scrtex;

        HighScoreManager m_highScoreManager;
        GameConfig m_config;

        void drawStatic(int x,int y,Texture textureNr,float scale=1.0f);
        void drawString(int x, int y, const char *text);
        void drawDots(int x,int y,Texture textureNr);
        virtual void processEvents() = 0;
        virtual void onUpdate() = 0;
        virtual void onRender() = 0;
        virtual void drawInterface()=0;
};