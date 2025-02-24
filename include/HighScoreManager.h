#pragma once

#define MAX_NICKNAME_LENGTH 30
#define HIGHSCORES_COUNT 3
#define HIGHSCORES_FILENAME "bestscores.txt"

struct HighScore {
    int score;
    char name[MAX_NICKNAME_LENGTH];
};

class HighScoreManager {
private:
    int m_currentHighScoreCount;
    HighScore m_highScores[HIGHSCORES_COUNT];

public:
    HighScoreManager();
    void loadHighScores();
    void addHighScore(const char *name,int score);
    void saveHighScores();
    int getCurrentHighScoreCount()const;
    const HighScore * getHighScores()const ;

    bool qualifiesForHighScore(int score) const;
};