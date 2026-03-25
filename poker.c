#include <stdio.h>
#include <stdlib.h>

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
  Deck *deck = deck_initializer();
  for(int i = 0; i<52; i++){
    display_card(&deck->cards[i]);
  }
  return 0; 
}
