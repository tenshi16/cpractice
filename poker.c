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

// #include "poker_eval.h"

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

typedef enum {PreFlop, Flop, Turn, River, Showdown } Step; 

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

void draw_card(const Card *card)
{
  const char *suit = suit_symbols[card->suit];
  const char *rank = rank_symbols[card->variant];
  DrawText(rank, card->x + default_card_padding, card->y + default_card_padding, 36, BLACK);

  switch(card->suit) {
    case Club:
    {
      int shape_radius = 15;
      Vector2 club_base = {card->x + card->width / 2, card->y + card->height / 2 + 25};
      DrawCircle(card->x + card->width / 2, card->y + card->height / 2, 15, BLACK);
      DrawCircle(card->x + card->width / 2 - default_card_padding, card->y + card->height / 2, shape_radius, BLACK);
      DrawCircle(card->x + card->width / 2 + default_card_padding, card->y + card->height / 2, shape_radius, BLACK);
      DrawCircle(card->x + card->width / 2, card->y + card->height / 2 - default_card_padding, shape_radius, BLACK);
      DrawPoly(club_base, 3, shape_radius, 30, BLACK);
    }
    break;
    case Diamonds:
    {

    }
    break;
    {

    }
    case Hearts:
    {

    }
    break;
    case Spades:
    {

    }
    break;
  }
}

void draw_deck(const Deck *player_deck)
{
  Color color = {243, 241, 243, 255};
  for (int i = 0; i < player_deck->count; i++)
  {
    Rectangle rec = {player_deck->cards[i].x, player_deck->cards[i].y, player_deck->cards[i].width, player_deck->cards[i].height};
    DrawRectangleRounded(rec, 0.1f, 8, color);
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

void card_to_lines(const Card *c, char lines[CARD_LINES][32]) {
    const char *suit = suit_symbols[c->suit];
    const char *rank = rank_symbols[c->variant];
    int pad = (c->variant == _10) ? 0 : 1;

    snprintf(lines[0], 32, "┌────┐ ");
    snprintf(lines[1], 32, "│%-2s  │ ", rank);
    snprintf(lines[2], 32, "│ %s  │ ", suit);
    snprintf(lines[3], 32, "│  %*s│ ", pad + 1, rank);
    snprintf(lines[4], 32, "└────┘ ");
}

void display_cards(const Card *cards, size_t count) {
    char lines[CARD_LINES][32];
    char all_lines[CARD_LINES][256];  // accumulate full rows here

    // initialize empty
    for (int i = 0; i < CARD_LINES; i++)
        all_lines[i][0] = '\0';

    for (size_t c = 0; c < count; c++) {
        card_to_lines(&cards[c], lines);
        for (int i = 0; i < CARD_LINES; i++)
            strcat(all_lines[i], lines[i]);
    }

    for (int i = 0; i < CARD_LINES; i++)
        printf("%s\n", all_lines[i]);
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

  gameS->current_step = PreFlop;
  bool running = true;

  // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    assign_cards(player_1, main_deck);
    player_cards_init(player_1->deck);

    SetTargetFPS(60);               // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
      // Update
      //----------------------------------------------------------------------------------
      // TODO: Update your variables here
      //----------------------------------------------------------------------------------

      // Draw
      //----------------------------------------------------------------------------------

      BeginDrawing();

      ClearBackground(LIGHTGRAY);
      draw_deck(player_1->deck);


      EndDrawing();
      //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

  /* do {
    switch(gameS->current_step) {
      case PreFlop:
        { 
          system("clear");
          // Deal two cards to each player
          assign_cards(player_1, main_deck);
          assign_cards(player_2, main_deck);
          assign_cards(player_3, main_deck);

          // Display only the cards of the Player 1 
          display_cards(player_1->deck->cards, player_1->deck->count);
          printf("Pre-flop: %.1f%%\n", calculate_winrate(player_1->deck, house_deck, 2) * 100);
          gameS->current_step = Flop;
          getchar();
        }
        break;
      case Flop:
        {
          system("clear");
          display_cards(player_1->deck->cards, player_1->deck->count);
          printf("Flop: %.1f%%\n", calculate_winrate(player_1->deck, house_deck, 2) * 100);
          printf("\n\n");
          // Show The First 3 cards 
          int rand_index = random_int(0, main_deck->count - 1);
          house_deck->cards = malloc(sizeof(Card) * 2);
          house_deck->cards[0] = main_deck->cards[rand_index];
          card_pop(main_deck, rand_index);


          rand_index = random_int(0, main_deck->count - 1);
          house_deck->cards[1] = main_deck->cards[rand_index];
          card_pop(main_deck, rand_index);


          rand_index = random_int(0, main_deck->count - 1);
          house_deck->cards[2] = main_deck->cards[rand_index];
          card_pop(main_deck, rand_index);

          house_deck->count = 3;

          display_cards(house_deck->cards, house_deck->count);
          gameS->current_step = Turn;
          getchar();
        }
        break;
      case Turn:
        {
          system("clear");
          display_cards(player_1->deck->cards, player_1->deck->count);
          printf("Turn: %.1f%%\n", calculate_winrate(player_1->deck, house_deck, 2) * 100);
          printf("\n\n");

          int rand_index = random_int(0, main_deck->count - 1);
          house_deck->cards[3] = main_deck->cards[rand_index];
          card_pop(main_deck, rand_index);
          house_deck->count = 4;
          display_cards(house_deck->cards, house_deck->count);
          gameS->current_step = River;
          getchar();
        }
        break;
      case River:
        {
          system("clear");
          display_cards(player_1->deck->cards, player_1->deck->count);
          printf("River: %.1f%%\n", calculate_winrate(player_1->deck, house_deck, 2) * 100);
          printf("\n\n");

          int rand_index = random_int(0, main_deck->count - 1);
          house_deck->cards[4] = main_deck->cards[rand_index];
          card_pop(main_deck, rand_index);
          house_deck->count = 5;
          display_cards(house_deck->cards, house_deck->count);
          gameS->current_step = Showdown;
          getchar();
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
         printf("\nPlayer 2\n");
         display_cards(player_2->deck->cards, player_2->deck->count);
         printf("\nPlayer 3\n");
         display_cards(player_3->deck->cards, player_3->deck->count);
         running = false;
         break;
        }
        break;
    }

  } while(running); */

  return 0;
}
