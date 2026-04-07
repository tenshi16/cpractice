#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>
#include <raylib.h>

#define CARD_LINES 5
#define CARD_WIDTH 7  // "┌────┐ " including trailing space
#define PADDING 50

typedef struct {
  float x;
  float y;
} Size;

const Size Window_Size = {1920, 1080};
const char *suit_symbols[] = {"♣", "♦", "♥", "♠"};
const char *rank_symbols[] = {
    "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};

const int default_card_padding = 20;
const Color card_color = {243, 241, 243, 255};

int random_int(int min, int max) {
    return rand() % (max - min + 1) + min;
}

typedef enum  { Club = 0, Diamonds = 1, Hearts = 2, Spades = 3 } Suits;
typedef enum {
  _2 = 0,
  _3 = 1,
  _4 = 2,
  _5 = 3,
  _6 = 4,
  _7 = 5,
  _8 = 6,
  _9 = 7,
  _10 = 8,
  _J = 9,
  _Q = 10,
  _K = 11,
  _A = 12
} Variant;

typedef struct
{
  Suits suit;
  Variant variant;
  float x;      
  float y;      
  float width;  
  float height; 
} Card;

typedef struct {
  Card *cards;
  size_t count;
  size_t capacity;
} Deck;

#include "poker_eval.h"

void card_pop(Deck *deck, size_t index) {
  for(size_t i = index; i < deck->count - 1; i++) {
    deck->cards[i] = deck->cards[i + 1];
  }
  deck->count--;
}


typedef struct {
  Deck *deck;
  float winrate;
  int games_won;
} Player;

typedef enum {Flop, Turn, River, Showdown } Step; 

typedef struct {
  Step current_step;
} GameState;

void player_cards_init(Deck *player_deck) {
  // Set default size
  player_deck->cards[0].width= 200;
  player_deck->cards[1].width= 200;
  player_deck->cards[0].height= 300;
  player_deck->cards[1].height = 300;

  // Set default position
  player_deck->cards[0].x = Window_Size.x / 2 - player_deck->cards[0].width - PADDING;
  player_deck->cards[0].y = Window_Size.y - player_deck->cards[0].height - PADDING;
  player_deck->cards[1].x = Window_Size.x / 2;
  player_deck->cards[1].y = Window_Size.y - player_deck->cards[1].height - PADDING;
}

void opponent_cards_init(Deck *opponent_card) {
  // Set default size
  opponed_card->cards[0].width= 100;
  opponed_card->cards[1].width= 100;
  opponed_card->cards[0].height= 150;
  opponed_card->cards[1].height = 150;

  // Set default position
  opponed_card->cards[0].x = Window_Size.x - opponent_card->cards[0].width - PADDING;
  opponed_card->cards[0].y = Window_Size.y - PADDING;
  opponed_card->cards[1].x = Window_Size.x;
  opponed_card->cards[1].y = Window_Size.y  - PADDING;
}

void house_cards_init(Deck *house_deck) {
  for(size_t i = 0; i < house_deck->count; i++){
    if (house_deck->cards[i].width ==  0.0f) {
      // Set size
      house_deck->cards[i].width = 200;
      house_deck->cards[i].height= 300;
      house_deck->cards[i].x = house_deck->cards[i].width * ((i+1) * 1.5)  - PADDING;
      house_deck->cards[i].y = Window_Size.y - house_deck->cards[i].height * 3;
    }
  } 
  // Set default size
}

void draw_card(const Card *card)
{
  // const char *suit = suit_symbols[card->suit];
  const char *rank = rank_symbols[card->variant];
  const int shape_radius = 15;
  const int symbol_y_offset = 25; 

  switch(card->suit) {
    case Club:
    {
      DrawText(rank, card->x + default_card_padding, card->y + default_card_padding, 36, BLACK);
      Vector2 club_base = {card->x + card->width / 2, card->y + card->height / 2 + symbol_y_offset};
      DrawCircle(card->x + card->width / 2, card->y + card->height / 2, shape_radius, BLACK);
      DrawCircle(card->x + card->width / 2 - default_card_padding, card->y + card->height / 2, shape_radius, BLACK);
      DrawCircle(card->x + card->width / 2 + default_card_padding, card->y + card->height / 2, shape_radius, BLACK);
      DrawCircle(card->x + card->width / 2, card->y + card->height / 2 - default_card_padding, shape_radius, BLACK);
      DrawPoly(club_base, 3, shape_radius, 30, BLACK);
    }
    break;
    case Diamonds:
    {
      DrawText(rank, card->x + default_card_padding, card->y + default_card_padding, 36, RED);
      Vector2 diamond_top = {card->x + card->width / 2, card->y + card->height / 2 };
      Vector2 diamond_bottom = {card->x + card->width / 2, card->y + card->height / 2 + symbol_y_offset + 5 };
      DrawPoly(diamond_top, 3, shape_radius * 2, 30, RED);
      DrawPoly(diamond_bottom, 3, shape_radius * 2, 90, RED);
    }
    break;
    case Hearts:
    {
      Vector2 diamond_bottom = {card->x + card->width / 2, card->y + card->height / 2 + symbol_y_offset + 5 };
      DrawText(rank, card->x + default_card_padding, card->y + default_card_padding, 36, RED);
      DrawCircle(card->x + (card->width / 2) * 0.8, card->y + card->height / 2, shape_radius * 1.5, RED);
      DrawCircle(card->x + card->width / 2 + default_card_padding , card->y + card->height / 2, shape_radius * 1.5, RED);
      DrawEllipse(card->x + (card->width / 2), card->y + card->height / 2 - (symbol_y_offset * 2), shape_radius, shape_radius * 2, card_color);
      DrawPoly(diamond_bottom, 3, shape_radius * 3, 90, RED);
      // Clip
      DrawCircle(card->x + (symbol_y_offset * 1.8), card->y + card->height * 0.7, shape_radius * 3, card_color);
      DrawCircle(card->x + (symbol_y_offset * 2.1), card->y + card->height * 0.75, shape_radius * 3, card_color);
      DrawCircle((card->x + card->width) - (symbol_y_offset * 1.8), card->y + card->height * 0.7, shape_radius * 3, card_color);
      DrawCircle((card->x + card->width) - (symbol_y_offset * 2.1), card->y + card->height * 0.75, shape_radius * 3, card_color);
    }
    break;
    case Spades:
    {
      Vector2 diamond_top = {card->x + card->width / 2, card->y + card->height / 4 + symbol_y_offset + 20 };
      Vector2 diamond_bottom= {card->x + card->width / 2, card->y + card->height / 2 + symbol_y_offset + 10};
      DrawText(rank, card->x + default_card_padding, card->y + default_card_padding, 36, BLACK);
      DrawCircle(card->x + (card->width / 2) * 0.8, card->y + card->height / 2, shape_radius * 1.5, BLACK);
      DrawCircle(card->x + card->width / 2 + default_card_padding , card->y + card->height / 2, shape_radius * 1.5, BLACK);
      DrawPoly(diamond_top, 3, shape_radius * 3, 30, BLACK);
      DrawPoly(diamond_bottom, 3, shape_radius * 2, 30, BLACK);
    }
    break;
  }
}

void draw_deck(const Deck *player_deck)
{
  for (size_t i = 0; i < player_deck->count; i++)
  {
    Rectangle rec = {player_deck->cards[i].x, player_deck->cards[i].y, player_deck->cards[i].width, player_deck->cards[i].height};
    DrawRectangleRounded(rec, 0.1f, 8, card_color);
    draw_card(&player_deck->cards[i]);
  }
}

void assign_cards(Player *player, Deck *main_deck) {
  int rand_index = random_int(0, main_deck->count - 1);
  player->deck->cards = malloc(sizeof(Card) * 2);
  player->deck->cards[0] = main_deck->cards[rand_index];
  card_pop(main_deck, rand_index);
  rand_index = random_int(0, main_deck->count -1);
  player->deck->cards[1] = main_deck->cards[rand_index];
  card_pop(main_deck, rand_index);
  player->deck->count = 2;
}


Deck *deck_initializer() {
  Deck *deck = malloc(sizeof(Deck));
  deck->count = 52;
  deck->capacity = 52;
  deck->cards= malloc(sizeof(Card) * deck->count);
  int variant_acc = 0;
  int idx = 0;
  for(int i = 0; i<4; i++){
    for(int j=0; j<13; j++) {
      if(variant_acc == 13) variant_acc = 0;
      deck->cards[idx++] = (Card){.suit = (Suits)i, .variant = (Variant)variant_acc };
      variant_acc++;
    }
  }
  return deck;
}

int main(void) {
  srand(time(NULL));
  Deck *main_deck= deck_initializer();

  GameState *gameS = malloc(sizeof(GameState));
  Player *player_1 = malloc(sizeof(Player));
  Player *player_2 = malloc(sizeof(Player));
  Player *player_3 = malloc(sizeof(Player));

  Deck *deck_1 = malloc(sizeof(Deck));
  Deck *deck_2 = malloc(sizeof(Deck));
  Deck *deck_3 = malloc(sizeof(Deck));
  Deck *house_deck= malloc(sizeof(Deck));
  player_1->deck = deck_1;
  player_2->deck = deck_2;
  player_3->deck = deck_3;
  deck_1->capacity = 2;
  deck_2->capacity = 2;
  deck_3->capacity = 2;
  house_deck->capacity = 5;
  deck_1->count = 0;
  deck_2->count = 0;
  deck_3->count = 0;
  house_deck->count = 0;

  gameS->current_step = Flop;

  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 1920;
  const int screenHeight = 1080;

  InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
  assign_cards(player_1, main_deck);
  assign_cards(player_2, main_deck);
  assign_cards(player_3, main_deck);
  player_cards_init(player_1->deck);
  printf("Pre-flop: %.1f%%\n", calculate_winrate(player_1->deck, house_deck, 2) * 100);

  SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
                                  //--------------------------------------------------------------------------------------

                                  // Main game loop
  while (!WindowShouldClose())    // Detect window close button or ESC key
  {
    if (GetKeyPressed()) {
      switch(gameS->current_step) {
        case Flop:
          {
            // Show The First 3 cards 
            int rand_index = random_int(0, main_deck->count - 1);
            house_deck->cards = malloc(sizeof(Card) * 5);
            house_deck->cards[0] = main_deck->cards[rand_index];
            card_pop(main_deck, rand_index);
            rand_index = random_int(0, main_deck->count - 1);
            house_deck->cards[1] = main_deck->cards[rand_index];
            card_pop(main_deck, rand_index);
            rand_index = random_int(0, main_deck->count - 1);
            house_deck->cards[2] = main_deck->cards[rand_index];
            card_pop(main_deck, rand_index);
            house_deck->count = 3;
            house_cards_init(house_deck);
            printf("Flop: %.1f%%\n", calculate_winrate(player_1->deck, house_deck, 2) * 100);
            gameS->current_step = Turn;
          }
          break;
        case Turn:
          {
            int rand_index = random_int(0, main_deck->count - 1);
            house_deck->cards[3] = main_deck->cards[rand_index];
            card_pop(main_deck, rand_index);
            house_deck->count = 4;
            house_cards_init(house_deck);
            printf("Turn: %.1f%%\n", calculate_winrate(player_1->deck, house_deck, 2) * 100);
            gameS->current_step = River;
          }
          break;
        case River:
          {
            int rand_index = random_int(0, main_deck->count - 1);
            house_deck->cards[4] = main_deck->cards[rand_index];
            card_pop(main_deck, rand_index);
            house_deck->count = 5;
            house_cards_init(house_deck);
            printf("River: %.1f%%\n", calculate_winrate(player_1->deck, house_deck, 2) * 100);
            gameS->current_step = Showdown;
          }
          break;
        case Showdown:
          {
            Deck *players[] = { player_1->deck, player_2->deck, player_3->deck };
            HandScore scores[3];
            int winner = showdown(players, 3, house_deck, scores);

            if (winner == -1)
              printf("Tie!\n");
            else
              printf("Player %d wins with %s!\n", winner + 1,
                  hand_rank_name(scores[winner].rank));
          }
      } 
    }
    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    draw_deck(house_deck);
    draw_deck(player_1->deck);
    EndDrawing();
  }

  CloseWindow();        // Close window and OpenGL context

  return 0;
}
