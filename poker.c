#include <stdio.h>
#include <stdlib.h>
#include <time.h>


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

typedef struct {
  Suits suit;
  Variant variant;
} Card;

typedef struct {
  Card *cards;
  size_t count;
  size_t capacity;
} Deck;

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

void assign_cards(Player *player, Deck *main_deck) {
  int rand_index = random_int(0, main_deck->count - 1);
  player->deck->cards = malloc(sizeof(Card) * 2);
  player->deck->cards[0] = main_deck->cards[rand_index];
  card_pop(main_deck, rand_index);
  rand_index = random_int(0, main_deck->count -1);
  player->deck->cards[1] = main_deck->cards[rand_index];
  card_pop(main_deck, rand_index);
}

const char *suit_symbols[] = { "♣", "♦", "♥", "♠" };
const char *rank_symbols[] = {
    "2","3","4","5","6","7","8","9","10","J","Q","K","A"
};

void display_card(const Card *c) {
    const char *suit = suit_symbols[c->suit];
    const char *rank = rank_symbols[c->variant];
    int pad = (c->variant == _10) ? 0 : 1;

    printf("┌────┐\n");
    printf("│%-2s  │\n", rank);
    printf("│ %s  │\n", suit);
    printf("│  %*s│\n", pad + 1, rank);
    printf("└────┘\n");
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

int main() { 
  Deck *main_deck= deck_initializer();
  GameState *gameS = malloc(sizeof(GameState));
  Player *player_1 = malloc(sizeof(Player));
  Player *player_2 = malloc(sizeof(Player));
  Player *player_3 = malloc(sizeof(Player));

  Deck *deck_1 = malloc(sizeof(Deck));
  Deck *deck_2 = malloc(sizeof(Deck));
  Deck *deck_3 = malloc(sizeof(Deck));
  Deck *deck_house = malloc(sizeof(Deck));
  player_1->deck = deck_1;
  player_2->deck = deck_2;
  player_3->deck = deck_3;
  deck_1->capacity = 2;
  deck_2->capacity = 2;
  deck_3->capacity = 2;
  deck_house->capacity = 5;
  deck_1->count = 0;
  deck_2->count = 0;
  deck_3->count = 0;
  deck_house->count = 0;

  gameS->current_step = PreFlop;
  /* do {
    switch(gameS->current_step) {
      case PreFlop:
        { */
          // Deal two cards to each player
          assign_cards(player_1, main_deck);
          assign_cards(player_2, main_deck);
          assign_cards(player_3, main_deck);
       /* }
        break;
      case Flop:
        {
        }
        break;
      case Turn:
        {
        }
        break;
      case River:
        {
        }
        break;
      case Showdown:
        {
        }
        break;
    }

  } while(1);*/
  display_card(&player_1->deck->cards[0]);
  display_card(&player_1->deck->cards[1]);
  display_card(&player_2->deck->cards[0]);
  display_card(&player_2->deck->cards[1]);
  display_card(&player_3->deck->cards[0]);
  display_card(&player_3->deck->cards[1]);

  return 0;
}
