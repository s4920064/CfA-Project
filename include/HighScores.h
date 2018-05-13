#ifndef HIGHSCORES_H__
#define HIGHSCORES_H__

#include <vector>

class Highscores
{
public :
  Highscores();
  ~Highscores();

  // adds a new score to the list of highscores (replaces 11th) and sorts
  void newScore(int _score);

  // returns the score at place #
  int getHighscore(int _place);

private :
  // the array of high scores
  int m_highscores[10];
  // flags whether a new high score has been set
  bool m_newHighscore;
}

#endif
