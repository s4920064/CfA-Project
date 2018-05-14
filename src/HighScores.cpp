#include "HighScores.h"
#include <algorithm>

Highscores::Highscores()
{
  m_newHighscore = false;
}

void Highscores::newScore(int _score)
{
  // if the new score is higher than the lowest high score
  if(_score > m_highscores[9])
  {
    // replace the last score with the new score
    m_highscores[9] = _score;
    // sort the list of high scores
    std::sort(m_highscores, m_highscores + 10);
  }
}
