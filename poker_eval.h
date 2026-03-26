#ifndef POKER_EVAL_H
#define POKER_EVAL_H

#include <stdlib.h>
#include <string.h>

// ---- Dependencies: your Card/Deck structs must be defined before including this header ----
// typedef struct { Suits suit; Variant variant; } Card;
// typedef struct { Card *cards; size_t count; size_t capacity; } Deck;
// Suits:   Club=0, Diamonds=1, Hearts=2, Spades=3
// Variant: _2=0, _3=1, ..., _A=12

// ---- Hand rankings ----

typedef enum {
    HIGH_CARD = 0,
    ONE_PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
    ROYAL_FLUSH
} HandRank;

typedef struct {
    HandRank rank;
    int      tiebreak[5];
} HandScore;

// ---- Internal helpers ----

static inline int _card_to_int(const Card *c) {
    return c->variant * 4 + c->suit;
}

static inline int _check_straight(int *present) {
    for (int i = 12; i >= 3; i--)
        if (present[i] && present[i-1] && present[i-2] && present[i-3] && present[i-4])
            return i;
    return -1;
}

// ---- Evaluate best 5-card hand out of 7 ----
// cards[i] = variant * 4 + suit

static HandScore evaluate_hand(int cards[7]) {
    int ranks[7], suits[7];
    for (int i = 0; i < 7; i++) {
        ranks[i] = cards[i] / 4;
        suits[i] = cards[i] % 4;
    }

    int freq[13]      = {0};
    int suit_freq[4]  = {0};
    for (int i = 0; i < 7; i++) {
        freq[ranks[i]]++;
        suit_freq[suits[i]]++;
    }

    // flush
    int flush_suit = -1;
    for (int i = 0; i < 4; i++)
        if (suit_freq[i] >= 5) { flush_suit = i; break; }

    int flush_ranks[7], flush_count = 0;
    if (flush_suit >= 0)
        for (int i = 0; i < 7; i++)
            if (suits[i] == flush_suit)
                flush_ranks[flush_count++] = ranks[i];

    // straight detection
    int present[14] = {0};
    for (int i = 0; i < 7; i++) {
        present[ranks[i]] = 1;
        if (ranks[i] == 12) present[13] = 1; // ace high
    }
    int straight_high = _check_straight(present);

    int sf_high = -1;
    if (flush_suit >= 0) {
        int fp[14] = {0};
        for (int i = 0; i < flush_count; i++) {
            fp[flush_ranks[i]] = 1;
            if (flush_ranks[i] == 12) fp[13] = 1;
        }
        sf_high = _check_straight(fp);
    }

    // pairs / trips / quads (descending order)
    int quads[13], quad_count = 0;
    int trips[13], trip_count = 0;
    int pairs[13], pair_count = 0;
    for (int r = 12; r >= 0; r--) {
        if      (freq[r] == 4) quads[quad_count++] = r;
        else if (freq[r] == 3) trips[trip_count++] = r;
        else if (freq[r] == 2) pairs[pair_count++] = r;
    }

    HandScore s = {0};

    if (sf_high >= 0) {
        s.rank         = (sf_high == 12) ? ROYAL_FLUSH : STRAIGHT_FLUSH;
        s.tiebreak[0]  = sf_high;
    } else if (quad_count > 0) {
        s.rank         = FOUR_OF_A_KIND;
        s.tiebreak[0]  = quads[0];
        for (int r = 12; r >= 0; r--)
            if (r != quads[0] && freq[r] > 0) { s.tiebreak[1] = r; break; }
    } else if (trip_count > 0 && (pair_count > 0 || trip_count > 1)) {
        s.rank         = FULL_HOUSE;
        s.tiebreak[0]  = trips[0];
        s.tiebreak[1]  = (trip_count > 1) ? trips[1] : pairs[0];
    } else if (flush_suit >= 0) {
        s.rank = FLUSH;
        for (int i = 0; i < flush_count - 1; i++)
            for (int j = i+1; j < flush_count; j++)
                if (flush_ranks[j] > flush_ranks[i]) {
                    int tmp = flush_ranks[i];
                    flush_ranks[i] = flush_ranks[j];
                    flush_ranks[j] = tmp;
                }
        for (int i = 0; i < 5; i++) s.tiebreak[i] = flush_ranks[i];
    } else if (straight_high >= 0) {
        s.rank         = STRAIGHT;
        s.tiebreak[0]  = straight_high;
    } else if (trip_count > 0) {
        s.rank         = THREE_OF_A_KIND;
        s.tiebreak[0]  = trips[0];
        int k = 1;
        for (int r = 12; r >= 0 && k < 3; r--)
            if (freq[r] == 1) s.tiebreak[k++] = r;
    } else if (pair_count >= 2) {
        s.rank         = TWO_PAIR;
        s.tiebreak[0]  = pairs[0];
        s.tiebreak[1]  = pairs[1];
        for (int r = 12; r >= 0; r--)
            if (freq[r] == 1) { s.tiebreak[2] = r; break; }
    } else if (pair_count == 1) {
        s.rank         = ONE_PAIR;
        s.tiebreak[0]  = pairs[0];
        int k = 1;
        for (int r = 12; r >= 0 && k < 4; r--)
            if (freq[r] == 1) s.tiebreak[k++] = r;
    } else {
        s.rank = HIGH_CARD;
        int k = 0;
        for (int r = 12; r >= 0 && k < 5; r--)
            if (freq[r] == 1) s.tiebreak[k++] = r;
    }

    return s;
}

// returns 1 if a wins, -1 if b wins, 0 if tie
static inline int compare_hands(HandScore a, HandScore b) {
    if (a.rank != b.rank) return (a.rank > b.rank) ? 1 : -1;
    for (int i = 0; i < 5; i++)
        if (a.tiebreak[i] != b.tiebreak[i])
            return (a.tiebreak[i] > b.tiebreak[i]) ? 1 : -1;
    return 0;
}

// ---- Win rate via Monte Carlo simulation ----

#ifndef POKER_SIMULATIONS
#define POKER_SIMULATIONS 10000
#endif

// player_hand: your 2 cards
// house:       0, 3, 4, or 5 community cards
// num_opponents: how many players you're up against
static float calculate_winrate(
    const Deck *player_hand,
    const Deck *house,
    int num_opponents
) {
    int remaining[52], rem_count = 52;
    for (int i = 0; i < 52; i++) remaining[i] = i;

    // remove known cards from the pool
    for (size_t i = 0; i < player_hand->count; i++) {
        int card = _card_to_int(&player_hand->cards[i]);
        for (int j = 0; j < rem_count; j++)
            if (remaining[j] == card) { remaining[j] = remaining[--rem_count]; break; }
    }
    for (size_t i = 0; i < house->count; i++) {
        int card = _card_to_int(&house->cards[i]);
        for (int j = 0; j < rem_count; j++)
            if (remaining[j] == card) { remaining[j] = remaining[--rem_count]; break; }
    }

    int hole[2] = {
        _card_to_int(&player_hand->cards[0]),
        _card_to_int(&player_hand->cards[1])
    };

    int community[5] = { -1, -1, -1, -1, -1 };
    for (size_t i = 0; i < house->count; i++)
        community[i] = _card_to_int(&house->cards[i]);

    int wins = 0;

    for (int sim = 0; sim < POKER_SIMULATIONS; sim++) {
        // shuffle remaining deck
        for (int i = rem_count - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            int tmp = remaining[i]; remaining[i] = remaining[j]; remaining[j] = tmp;
        }

        // fill unknown community cards
        int sim_community[5];
        memcpy(sim_community, community, sizeof(sim_community));
        int next = 0;
        for (int i = 0; i < 5; i++)
            if (sim_community[i] == -1)
                sim_community[i] = remaining[next++];

        int my_cards[7] = {
            hole[0], hole[1],
            sim_community[0], sim_community[1], sim_community[2],
            sim_community[3], sim_community[4]
        };
        HandScore my_score = evaluate_hand(my_cards);

        int won = 1;
        for (int o = 0; o < num_opponents && won; o++) {
            int opp_cards[7] = {
                remaining[next + o*2], remaining[next + o*2 + 1],
                sim_community[0], sim_community[1], sim_community[2],
                sim_community[3], sim_community[4]
            };
            if (compare_hands(my_score, evaluate_hand(opp_cards)) <= 0) won = 0;
        }

        if (won) wins++;
    }

    return (float)wins / POKER_SIMULATIONS;
}

// ---- Hand rank name (for display) ----

static inline const char *hand_rank_name(HandRank r) {
    switch (r) {
        case HIGH_CARD:       return "High Card";
        case ONE_PAIR:        return "One Pair";
        case TWO_PAIR:        return "Two Pair";
        case THREE_OF_A_KIND: return "Three of a Kind";
        case STRAIGHT:        return "Straight";
        case FLUSH:           return "Flush";
        case FULL_HOUSE:      return "Full House";
        case FOUR_OF_A_KIND:  return "Four of a Kind";
        case STRAIGHT_FLUSH:  return "Straight Flush";
        case ROYAL_FLUSH:     return "Royal Flush";
        default:              return "Unknown";
    }
}

// returns index of winning player, or -1 on tie
// players: array of Deck pointers (each with 2 hole cards)
// house:   must have exactly 5 cards (showdown)
static int showdown(Deck **players, int num_players, const Deck *house, HandScore *out_scores) {
    int community[5];
    for (int i = 0; i < 5; i++)
        community[i] = _card_to_int(&house->cards[i]);

    for (int i = 0; i < num_players; i++) {
        int cards[7] = {
            _card_to_int(&players[i]->cards[0]),
            _card_to_int(&players[i]->cards[1]),
            community[0], community[1], community[2],
            community[3], community[4]
        };
        out_scores[i] = evaluate_hand(cards);
    }

    int winner = 0;
    int tie    = 0;
    for (int i = 1; i < num_players; i++) {
        int result = compare_hands(out_scores[i], out_scores[winner]);
        if (result > 0) { winner = i; tie = 0; }
        else if (result == 0) tie = 1;
    }

    return tie ? -1 : winner;
}

#endif // POKER_EVAL_H
