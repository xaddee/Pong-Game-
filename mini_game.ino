#include "LedControl.h"
#include <LiquidCrystal.h>

LedControl lc = LedControl(12, 11, 10, 1);

LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

const int pot_pin = A0;
const int v0_pin = 9;
const int x_pin = A1;
const int y_pin = A2;
const int sw_pin = 8;
const int speed_val = 10;
const int default_delay = 400;
int pot_val, x_val, y_val, sw_val;

bool matrix[8][8] = {
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0}
};

struct Point
{
  int x, y;
  
  Point& operator= (const Point& other)
  {
     this->x = other.x;
     this->y = other.y;
     return *this;
  }
};

class Player
{
  private:
  Point m_position[3];
  
  public:
  void setPosition(Point, Point, Point);
  void moveRight();
  void moveLeft();
  Point showMidPosition();
};

Point Player::showMidPosition ()
{
  return m_position[1];
}

void Player::setPosition (Point left, Point middle, Point right)
{
  m_position[0] = left;
  m_position[1] = middle;
  m_position[2] = right;

  for (Point current: m_position)
  {
      matrix[current.x][current.y] = true;
  }
}

void Player::moveRight ()
{
  if (m_position[0].y > 0)
  {
    matrix[m_position[2].x][m_position[2].y] = false;

    m_position[2] = m_position[1];
    m_position[1] = m_position[0];
    m_position[0].y--;

    matrix[m_position[0].x][m_position[0].y] = true;
  }
}

void Player::moveLeft ()
{
  if (m_position[2].y < 7)
  {
    matrix[m_position[0].x][m_position[0].y] = false;

     m_position[0] = m_position[1];
     m_position[1] = m_position[2];
     m_position[2].y++;

     matrix[m_position[2].x][m_position[2].y] = true;
     
  }
}

Player player, computer;
Point ball, next_ball;

int ball_x_direction, ball_y_direction, player_score = 0, computer_score = 0;

long long int current_time, start_time, current_ball_time, start_ball_time, pot_delay = 0;

void setup ()
{
  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Press the button");
  lcd.setCursor(4, 1);
  lcd.print("to start!");
  
  lc.shutdown(0, false);
  lc.setIntensity(0,15);
  lc.clearDisplay(0);
  
  setPlayers();
  setBall();
  
  start_ball_time = millis();
  start_time = millis();

  pinMode(sw_pin, INPUT);
  digitalWrite(sw_pin, HIGH);
   
  pinMode(v0_pin, OUTPUT);
  analogWrite(v0_pin, 90);

  
  while(1)
  {
    sw_val = digitalRead(sw_pin);
    if(sw_val == LOW)
    break;    
  }
  lcd.clear();
  showScore();
}
void loop ()
{ 
  if (player_score == 5)
  {
    playerWins();
  }
  else if (computer_score == 5)
  {
    computerWins();
  }
  else
  {
    current_time = millis();
    current_ball_time = millis();
    
    if (current_time - start_time > default_time)
    {     
      movePlayer();
      moveComputer();
  
      start_time = current_time;
    }
    
    if (current_ball_time - start_ball_time > default_time - pot_delay)
    {

      pot_val = analogRead(pot_pin);
      if (pot_val > 900 && pot_delay <= 200) pot_delay += speed_val;
      else if (pot_val < 200 && pot_delay >= 0) pot_delay -= speed_val;
      
      moveBall();
      start_ball_time = current_ball_time;
      
      lightMatrix();
    }
  }
}

void setPlayers ()
{
  for (int  i = 0; i < 8; i++)
  {
    matrix[0][i] = false;
    matrix[7][i] = false;
  }
  
  Point left_player, middle_player, right_player, left_computer, middle_computer, right_computer;
  left_player.x = 7;
  middle_player.x = 7;
  right_player.x = 7;

  left_player.y = 2;
  middle_player.y = 3;
  right_player.y = 4;

  left_computer.x = 0;
  middle_computer.x = 0;
  right_computer.x = 0;

  left_computer.y = 2;
  middle_computer.y = 3;
  right_computer.y = 4;
  player.setPosition(left_player, middle_player, right_player);
  computer.setPosition(left_computer, middle_computer, right_computer);
}

void lightMatrix ()
{
  for (int i = 0; i < 8; i++)
  {
    for (int j = 0; j < 8; j++)
    {
      lc.setLed(0, i, j, matrix[i][j]);
    }
  }
}

void movePlayer ()
{
  x_val = analogRead(x_pin);

  if (x_val < 400) player.moveRight();
  if (x_val > 600) player.moveLeft();
}

void moveBall ()
{
  matrix[ball.x][ball.y] = false;
  
  next_ball.x = ball.x + ball_x_direction;
  next_ball.y = ball.y;

   if (matrix[next_ball.x][next_ball.y] == true)
  {
    ball_x_direction = -ball_x_direction;
    
    int random_var = random(2,5);
    if (random_var % 2) ball_y_direction = -ball_y_direction;

    if (ball.y + ball_y_direction > 7 || ball.y + ball_y_direction < 0)
    ball_y_direction = -ball_y_direction;
    
    ball.y += ball_y_direction;
    ball.x += ball_x_direction;
  }
  else
  {
    if (ball.y + ball_y_direction > 7 || ball.y + ball_y_direction < 0)
    ball_y_direction = -ball_y_direction;
  
    next_ball.y = ball.y + ball_y_direction;
  
    if (matrix[next_ball.x][next_ball.y] == true)
    {
      ball_x_direction = -ball_x_direction;
    
      int random_var = random(1,3);
      if (random_var % 2) ball_y_direction = -ball_y_direction;

      if (ball.y + ball_y_direction > 7 || ball.y + ball_y_direction < 0)
      ball_y_direction = -ball_y_direction;
      
      ball.y += ball_y_direction;
      ball.x += ball_x_direction;
    }
    else if (next_ball.x == 0 && matrix[next_ball.x][next_ball.y] == false)
    {
      player_score++;
      setBall();
      playerPlus(); //shows the diagonal of the matrix if computer scores
    }
    else if (next_ball.x == 7 && matrix[next_ball.x][next_ball.y] == false)
    {
      computer_score++;
      setBall();
      computerPlus(); //shows the other diagonal of the matrix if player scores
    }
    else 
    {
      ball.y += ball_y_direction;
      ball.x += ball_x_direction;
    }
  }
  
  matrix[ball.x][ball.y] = true;
}

void moveComputer ()
{
  Point computer_middle = computer.showMidPosition();

  Point future_ball = ball;
  future_ball.x += ball_x_direction;
  future_ball.y += ball_y_direction;
  
  if (future_ball.y > computer_middle.y) computer.moveLeft();
  if (future_ball.y < computer_middle.y) computer.moveRight();
}
void playerPlus ()
{
  showScore();  
  lc.clearDisplay(0);

  for (int i = 0; i < 8; i++)
    lc.setLed(0, i, i, true);

    delay(800);
}

void computerPlus ()
{
  showScore();
  lc.clearDisplay(0);

  for (int i = 0; i < 8; i++)
    lc.setLed(0, i, 8-i-1, true);

  delay(800);
}

void playerWins ()
{
  lc.clearDisplay(0);

  //displays letter 'P'

  lc.setLed(0, 7, 7, true);
  lc.setLed(0, 7, 6, true);
  lc.setLed(0, 7, 5, true);
  lc.setLed(0, 7, 4, true);
  lc.setLed(0, 7, 3, true);
  lc.setLed(0, 7, 2, true);
  lc.setLed(0, 6, 7, true);
  lc.setLed(0, 6, 4, true);
  lc.setLed(0, 5, 6, true);
  lc.setLed(0, 5, 5, true);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("You Win!");
  lcd.setCursor(0, 1);
  lcd.print("Button - Replay");
  
  while(1)
  {
    sw_val = digitalRead(sw_pin);
    if(sw_val == LOW)
    break;  
  }

  player_score = 0;
  computer_score = 0;
  showScore();
}

void computerWins ()
{
  lc.clearDisplay(0);

  // displays letter 'C'

  lc.setLed(0, 6, 7, true);
  lc.setLed(0, 5, 7, true);
  lc.setLed(0, 7, 6, true);
  lc.setLed(0, 7, 5, true);
  lc.setLed(0, 7, 4, true);
  lc.setLed(0, 7, 3, true);
  lc.setLed(0, 4, 6, true);
  lc.setLed(0, 6, 2, true);
  lc.setLed(0, 5, 2, true);
  lc.setLed(0, 4, 3, true);

  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("You Lose!");
  lcd.setCursor(0, 1);
  lcd.print("Button - Replay");
  
  while(1)
  {
    sw_val = digitalRead(sw_pin);
    if(sw_val == LOW)
    break;  
  }
  
  player_score = 0;
  computer_score = 0;
  showScore();
}

void setBall ()
{
  setPlayers();
   
  ball.x = 3;
  ball.y = 3;
  
  matrix[ball.x][ball.y] = true;

  int random_number_y, random_number_x;
  random_number_y = random(10, 21);
  random_number_x = random(10, 21);

  if (random_number_y % 2) ball_y_direction = 1;
  else ball_y_direction = -1;

  if (random_number_x % 2) ball_x_direction = 1;
  else ball_x_direction = -1;

}

void showScore ()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Score: Player  ");
  lcd.print(player_score);
  lcd.setCursor(6, 1);
  lcd.print("Computer ");
  lcd.print(computer_score);
}
