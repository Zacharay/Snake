#include "HighScoreManager.h"
#include <cstdio>
#include <cstring>

HighScoreManager::HighScoreManager() {
    m_currentHighScoreCount = 0;
    for (int i = 0; i < HIGHSCORES_COUNT; i++) {
        m_highScores[i].score = 0;
        m_highScores[i].name[0] = '\0';
    }
    loadHighScores();
}
void HighScoreManager::loadHighScores() {
    FILE *file = fopen(HIGHSCORES_FILENAME, "r");
    if (!file) {
        printf("Could not open file %s. Starting with an empty high score table.\n", HIGHSCORES_FILENAME);
        m_currentHighScoreCount = 0;
        return;
    }

    int i = 0;
    while (i < HIGHSCORES_COUNT && fscanf(file, "%29s %d", m_highScores[i].name, &m_highScores[i].score) == 2) {
        i++;
    }
    m_currentHighScoreCount = i;

    fclose(file);
}

void HighScoreManager::addHighScore(const char *name, int score) {
    // Check if the score qualifies for the high score table
    if (!qualifiesForHighScore(score)) {
        return;
    }

    // Find the position to insert the new score
    int position = m_currentHighScoreCount;
    for (int i = 0; i < m_currentHighScoreCount; i++) {
        if (score > m_highScores[i].score) {
            position = i;
            break;
        }
    }

    // If the table is full, shift scores down to make room
    if (m_currentHighScoreCount == HIGHSCORES_COUNT) {
        for (int i = HIGHSCORES_COUNT - 1; i > position; i--) {
            m_highScores[i] = m_highScores[i - 1];
        }
    } else {
        // Increment high score count if there's space
        m_currentHighScoreCount++;
        for (int i = m_currentHighScoreCount - 1; i > position; i--) {
            m_highScores[i] = m_highScores[i - 1];
        }
    }

    // Insert the new high score
    strncpy(m_highScores[position].name, name, sizeof(m_highScores[position].name) - 1);
    m_highScores[position].name[sizeof(m_highScores[position].name) - 1] = '\0';
    m_highScores[position].score = score;

    // Save the updated high scores to the file
    saveHighScores();
}

// Saves the high scores to a file
void HighScoreManager::saveHighScores() {
    FILE *file = fopen(HIGHSCORES_FILENAME, "w");
    if (!file) {
        printf("Could not open file %s for writing.\n", HIGHSCORES_FILENAME);
        return;
    }

    for (int i = 0; i < m_currentHighScoreCount; i++) {
        fprintf(file, "%s %d\n", m_highScores[i].name, m_highScores[i].score);
    }

    fclose(file);
}
bool HighScoreManager::qualifiesForHighScore(int score)const {
    if(m_currentHighScoreCount < HIGHSCORES_COUNT) {
        return true;
    }

    for(int i=0;i< m_currentHighScoreCount;i++) {
        if (score > m_highScores[i].score) {
            return true;
        }
    }
    return false;
}
int HighScoreManager::getCurrentHighScoreCount() const{
    return m_currentHighScoreCount;
}
const HighScore* HighScoreManager::getHighScores() const{
    return m_highScores;
};