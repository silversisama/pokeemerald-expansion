#include "global.h"
#include "constants/battle.h"
#include "constants/items.h"
#include "item.h"
#include "pokeball.h"
#include "string_util.h"
#include "text.h"
#include "strings.h"
#include "graphics.h"

#define MAX_CARDS 54
#define MAX_DRAWN 5


extern const u16 card_the_foolBitmap[];
extern const u16 card_cups_01Bitmap[];
extern const u16 card_cups_02Bitmap[];
extern const u16 card_cups_03Bitmap[];
extern const u16 card_cups_04Bitmap[];
extern const u16 card_cups_05Bitmap[];
extern const u16 card_cups_06Bitmap[];
extern const u16 card_cups_07Bitmap[];
extern const u16 card_cups_08Bitmap[];
extern const u16 card_wands_01Bitmap[];
extern const u16 card_wands_02Bitmap[];
extern const u16 card_wands_03Bitmap[];
extern const u16 card_wands_04Bitmap[];
extern const u16 card_wands_05Bitmap[];
extern const u16 card_wands_06Bitmap[];
extern const u16 card_wands_07Bitmap[];
extern const u16 card_wands_08Bitmap[];
extern const u16 card_swords_01Bitmap[];
extern const u16 card_swords_02Bitmap[];
extern const u16 card_swords_03Bitmap[];
extern const u16 card_swords_04Bitmap[];
extern const u16 card_swords_05Bitmap[];
extern const u16 card_swords_06Bitmap[];
extern const u16 card_swords_07Bitmap[];
extern const u16 card_swords_08Bitmap[];
extern const u16 card_coins_01Bitmap[];
extern const u16 card_coins_02Bitmap[];
extern const u16 card_coins_03Bitmap[];
extern const u16 card_coins_04Bitmap[];
extern const u16 card_coins_05Bitmap[];
extern const u16 card_coins_06Bitmap[];
extern const u16 card_coins_07Bitmap[];
extern const u16 card_coins_08Bitmap[];
extern const u16 card_the_worldBitmap[];
extern const u16 card_the_magicianBitmap[];
extern const u16 card_the_priestessBitmap[];
extern const u16 card_the_empressBitmap[];
extern const u16 card_the_emperorBitmap[];
extern const u16 card_the_hieroBitmap[];
extern const u16 card_the_loversBitmap[];
extern const u16 card_the_chariotBitmap[];
extern const u16 card_the_strengthBitmap[];
extern const u16 card_the_hermitBitmap[];
extern const u16 card_the_wheelBitmap[];
extern const u16 card_justiceBitmap[];
extern const u16 card_the_hangedBitmap[];
extern const u16 card_deathBitmap[];
extern const u16 card_temperanceBitmap[];
extern const u16 card_the_devilBitmap[];
extern const u16 card_the_towerBitmap[];
extern const u16 card_the_starBitmap[];
extern const u16 card_the_moonBitmap[];
extern const u16 card_the_sunBitmap[];
extern const u16 card_judgementBitmap[];

void shuffleAndDrawCards(int numCards) {
    // Fill index array
    int indices[MAX_CARDS];
    for (int i = 0; i < MAX_CARDS; i++) indices[i] = i;

    // Fisher–Yates shuffle
    for (int i = MAX_CARDS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    // Assign first numCards into drawnCards
    for (int i = 0; i < numCards; i++) {
        drawnCards[i] = &cards[indices[i]];
    }
}


void drawBitmap(const u16* bitmap, int x, int y, int width, int height) {
    u16* fb = (u16*)MODE3_FB;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            fb[(y + j) * SCREEN_WIDTH + (x + i)] = bitmap[j * width + i];
        }
    }
}


void drawFaceDownCards(int numCards) {
    int spacing = 80;
    int startX = (240 - ((numCards - 1) * spacing + 64)) / 2;

    for (int i = 0; i < numCards; i++) {
        int x = startX + i * spacing;
        drawBitmap(card_backBitmap, x, 40, 64, 64); // drawBitmap is your custom blitter
    }
}


int currentSelection = 0;


void drawSelectionCursor(int index, int total) {
    int x = getCardX(index, total);
    u16* fb = (u16*)MODE3_FB;
    for (int i = 0; i < 64; i++) {
        fb[(40 + 64 + 2) * SCREEN_WIDTH + (x + i)] = RGB5(31, 0, 0);
    }
}


void handleInput(int numCards) {
    scanKeys();
    u16 keys = keysDownRepeat();

    if (keys & KEY_LEFT && currentSelection > 0) {
        currentSelection--;
    } else if (keys & KEY_RIGHT && currentSelection < numCards - 1) {
        currentSelection++;
    } else if (keys & KEY_A) {
        revealSelectedCard(currentSelection);
    }
}


void revealSelectedCard(int index) {
    Card* chosen = drawnCards[index];
    drawBitmap(chosen->imageData, getCardX(index), 40, 64, 64);  // Flip card
    handleCardEffect(chosen);
}




typedef enum {
    WEATHER_NONE,
    WEATHER_SUN,
    WEATHER_RAIN,
    WEATHER_HAIL,
    WEATHER_SANDSTORM
} WeatherType;

typedef enum {
    ITEM_NONE,
    ITEM_ETHER,
    ITEM_FULL_HEAL,
    ITEM_POTION,
    ITEM_SUPER_POTION,
    ITEM_HYPER_POTION,
    ITEM_MAX_POTION,
    ITEM_FULL_RESTORE,
    ITEM_REVIVE,
    ITEM_MAX_REVIVE
} ItemHealType;

typedef enum {
    SUIT_NONE,
    SUIT_CUPS,
    SUIT_SWORDS,
    SUIT_COINS,
    SUIT_WANDS,
    SUIT_MAJOR
} SuitType;

typedef enum {
    ITEM_HP_UP,
    ITEM_PROTEIN,
    ITEM_IRON,
    ITEM_CALCIUM,
    ITEM_ZINC,
    ITEM_CARBOS,
    ITEM_PP_UP,
    ITEM_PP_MAX
} ItemVitamins;

typedef enum {
    EFFECT_NONE,
    EFFECT_REROLL,        // The Fool
    EFFECT_CHOOSE_SUIT,   // The Magician
    EFFECT_LEVEL_UP,      // The High Priest
    EFFECT_GRANT_HITEM,   // The Empress
    EFFECT_REROLL_MAJOR,  // The Emperor
    EFFECT_CHOOSE_TWO,    // The Hierophant
    EFFECT_DRAW_CASH,     // The Lovers
    EFFECT_CARBOS_GET,    // The Chariot
    EFFECT_GRANT_VITAMIN, // The Strength
    EFFECT_MAX_REPEL,     // The Hermit
    EFFECT_IMPROVE_SHINY, // The Wheel
    EFFECT_MORE_MONEY,    // Justice
    EFFECT_REMOVE_REPEL,  // The Hanged Man
    EFFECT_DEATHCARD,     // Death
    EFFECT_POKEBALLS,     // Temperance
    EFFECT_DEVILCARD,     // The Devil
    EFFECT_TOWERCARD,     // The Tower
    EFFECT_STARCARD,      // The Star
    EFFECT_SET_WEATHER,   // The Moon
    EFFECT_SKIP_BATTLE,   // The Sun
    EFFECT_MAX_LURE,      // Judgement
    EFFECT_REVEAL_MAP,    // The World
    EFFECT_SWORDS_TOKENS,  // Swords Suit
    EFFECT_CUPS_HEALING,  // Cups Suit
    EFFECT_COINS_GET,     // Coins Suit
    EFFECT_XP_BOOST,      // Wands Suit
} CardEffect;


typedef struct {
    const u16* imageData;
    const char* name;
    SuitType suit;      // Suit of the card (Cups, Swords, Coins, Wands)
    int rank;           // Rank of the card (1–8)
    CardEffect effect;  // Card effect (e.g., reroll, item grant)
    WeatherType weatherEffect;
    ItemType itemEffect;
    int bonus;          // Any additional bonuses
} Card;

Card cards[54];
Card* drawnCards[MAX_DRAWN];
Card* selectedCards[5];
int selectedCardCount = 0;


typedef struct {
    int swordsTokens;  // Number of swords tokens the player has
} Player;

Player player = {0};  // Player starts with 0 swords tokens

void startCardMinigame() {
    int cardsToDraw = 3 + rand() % 3;
    shuffleAndDrawCards(cardsToDraw);
    drawFaceDownCards(cardsToDraw);

    while (!cardChosen) {
        VBlankIntrWait();
        handleInput(cardsToDraw);
        drawSelectionCursor(currentSelection, cardsToDraw);
    }
}


int main() {
    irqInit();
    irqEnable(IRQ_VBLANK);
    SetMode(MODE_3 | BG2_ON);

    initializeTestCards();
    startCardMinigame();

    while (1) VBlankIntrWait();
}

void initializeCards() {
    // Basic cards
    cards[0] = (Card){card_the_foolBitmap, "The Fool", SUIT_MAJOR, 0, EFFECT_REROLL, WEATHER_NONE, ITEM_NONE, 0};
    cards[1] = (Card){card_cups_01Bitmap, "1 of Cups", SUIT_CUPS, 1, EFFECT_CUPS_HEALING, WEATHER_NONE, ITEM_NONE, 0};
    cards[2] = (Card){card_cups_02Bitmap, "2 of Cups", SUIT_CUPS, 2, EFFECT_CUPS_HEALING, WEATHER_NONE, ITEM_NONE, 0};
    cards[3] = (Card){card_cups_03Bitmap, "3 of Cups", SUIT_CUPS, 3, EFFECT_CUPS_HEALING, WEATHER_NONE, ITEM_NONE, 0};
    cards[4] = (Card){card_cups_04Bitmap, "4 of Cups", SUIT_CUPS, 4, EFFECT_CUPS_HEALING, WEATHER_NONE, ITEM_NONE, 0};
    cards[5] = (Card){card_cups_05Bitmap, "5 of Cups", SUIT_CUPS, 5, EFFECT_CUPS_HEALING, WEATHER_NONE, ITEM_NONE, 0};
    cards[6] = (Card){card_cups_06Bitmap, "6 of Cups", SUIT_CUPS, 6, EFFECT_CUPS_HEALING, WEATHER_NONE, ITEM_NONE, 0};
    cards[7] = (Card){card_cups_07Bitmap, "7 of Cups", SUIT_CUPS, 7, EFFECT_CUPS_HEALING, WEATHER_NONE, ITEM_NONE, 0};
    cards[8] = (Card){card_cups_08Bitmap, "8 of Cups", SUIT_CUPS, 8, EFFECT_CUPS_HEALING, WEATHER_NONE, ITEM_NONE, 0};
    cards[9] = (Card){card_wands_01Bitmap, "1 of Wands", SUIT_WANDS, 1, EFFECT_XP_BOOST, WEATHER_NONE, ITEM_NONE, 0};
    cards[10] = (Card){card_wands_02Bitmap, "2 of Wands", SUIT_WANDS, 2, EFFECT_XP_BOOST, WEATHER_NONE, ITEM_NONE, 0};
    cards[11] = (Card){card_wands_03Bitmap, "3 of Wands", SUIT_WANDS, 3, EFFECT_XP_BOOST, WEATHER_NONE, ITEM_NONE, 0};
    cards[12] = (Card){card_wands_04Bitmap, "4 of Wands", SUIT_WANDS, 4, EFFECT_XP_BOOST, WEATHER_NONE, ITEM_NONE, 0};
    cards[13] = (Card){card_wands_05Bitmap, "5 of Wands", SUIT_WANDS, 5, EFFECT_XP_BOOST, WEATHER_NONE, ITEM_NONE, 0};
    cards[14] = (Card){card_wands_06Bitmap, "6 of Wands", SUIT_WANDS, 6, EFFECT_XP_BOOST, WEATHER_NONE, ITEM_NONE, 0};
    cards[15] = (Card){card_wands_07Bitmap, "7 of Wands", SUIT_WANDS, 7, EFFECT_XP_BOOST, WEATHER_NONE, ITEM_NONE, 0};
    cards[16] = (Card){card_wands_08Bitmap, "8 of Wands", SUIT_WANDS, 8, EFFECT_XP_BOOST, WEATHER_NONE, ITEM_NONE, 0};
    cards[17] = (Card){card_swords_01Bitmap, "1 of Swords", SUIT_SWORDS, 1, EFFECT_SWORDS_TOKENS, WEATHER_NONE, ITEM_NONE, 0};
    cards[18] = (Card){card_swords_02Bitmap, "2 of Swords", SUIT_SWORDS, 2, EFFECT_SWORDS_TOKENS, WEATHER_NONE, ITEM_NONE, 0};
    cards[19] = (Card){card_swords_03Bitmap, "3 of Swords", SUIT_SWORDS, 3, EFFECT_SWORDS_TOKENS, WEATHER_NONE, ITEM_NONE, 0};
    cards[20] = (Card){card_swords_04Bitmap, "4 of Swords", SUIT_SWORDS, 4, EFFECT_SWORDS_TOKENS, WEATHER_NONE, ITEM_NONE, 0};
    cards[21] = (Card){card_swords_05Bitmap, "5 of Swords", SUIT_SWORDS, 5, EFFECT_SWORDS_TOKENS, WEATHER_NONE, ITEM_NONE, 0};
    cards[22] = (Card){card_swords_06Bitmap, "6 of Swords", SUIT_SWORDS, 6, EFFECT_SWORDS_TOKENS, WEATHER_NONE, ITEM_NONE, 0};
    cards[23] = (Card){card_swords_07Bitmap, "7 of Swords", SUIT_SWORDS, 7, EFFECT_SWORDS_TOKENS, WEATHER_NONE, ITEM_NONE, 0};
    cards[24] = (Card){card_swords_08Bitmap, "8 of Swords", SUIT_SWORDS, 8, EFFECT_SWORDS_TOKENS, WEATHER_NONE, ITEM_NONE, 0};
    cards[25] = (Card){card_coins_01Bitmap, "1 of Coins", SUIT_COINS, 1, EFFECT_COINS_GET, WEATHER_NONE, ITEM_NONE, 0};
    cards[26] = (Card){card_coins_02Bitmap, "2 of Coins", SUIT_COINS, 2, EFFECT_COINS_GET, WEATHER_NONE, ITEM_NONE, 0};
    cards[27] = (Card){card_coins_03Bitmap, "3 of Coins", SUIT_COINS, 3, EFFECT_COINS_GET, WEATHER_NONE, ITEM_NONE, 0};
    cards[28] = (Card){card_coins_04Bitmap, "4 of Coins", SUIT_COINS, 4, EFFECT_COINS_GET, WEATHER_NONE, ITEM_NONE, 0};
    cards[29] = (Card){card_coins_05Bitmap, "5 of Coins", SUIT_COINS, 5, EFFECT_COINS_GET, WEATHER_NONE, ITEM_NONE, 0};
    cards[30] = (Card){card_coins_06Bitmap, "6 of Coins", SUIT_COINS, 6, EFFECT_COINS_GET, WEATHER_NONE, ITEM_NONE, 0};
    cards[31] = (Card){card_coins_07Bitmap, "7 of Coins", SUIT_COINS, 7, EFFECT_COINS_GET, WEATHER_NONE, ITEM_NONE, 0};
    cards[32] = (Card){card_coins_08Bitmap, "8 of Coins", SUIT_COINS, 8, EFFECT_COINS_GET, WEATHER_NONE, ITEM_NONE, 0};
    cards[33] = (Card){card_the_worldBitmap, "The World", SUIT_MAJOR, 0, EFFECT_REVEAL_MAP, WEATHER_NONE, ITEM_NONE, 0};
    cards[34] = (Card){card_the_magicianBitmap, "The Magician", SUIT_MAJOR, 0, EFFECT_CHOOSE_SUIT, WEATHER_NONE, ITEM_NONE, 0};
    cards[35] = (Card){card_the_priestessBitmap, "The High Priestess", SUIT_MAJOR, 0, EFFECT_LEVEL_UP, WEATHER_NONE, ITEM_NONE, 0};
    cards[36] = (Card){card_the_empressBitmap, "The Empress", SUIT_MAJOR, 0, EFFECT_GRANT_HITEM, WEATHER_NONE, ITEM_NONE, 0};
    cards[37] = (Card){card_the_emperorBitmap, "The Emperor", SUIT_MAJOR, 0, EFFECT_REROLL_MAJOR, WEATHER_NONE, ITEM_NONE, 0};
    cards[38] = (Card){card_the_hieroBitmap, "The Hierophant", SUIT_MAJOR, 0, EFFECT_CHOOSE_TWO, WEATHER_NONE, ITEM_NONE, 0};
    cards[39] = (Card){card_the_loversBitmap, "The Lovers", SUIT_MAJOR, 0, EFFECT_DRAW_CASH, WEATHER_NONE, ITEM_NONE, 0};
    cards[40] = (Card){card_the_chariotBitmap, "The Chariot", SUIT_MAJOR, 0, EFFECT_CARBOS_GET, WEATHER_NONE, ITEM_NONE, 0};
    cards[41] = (Card){card_the_strengthBitmap, "The Strength", SUIT_MAJOR, 0, EFFECT_GRANT_VITAMIN, WEATHER_NONE, ITEM_NONE, 0};
    cards[42] = (Card){card_the_hermitBitmap, "The Hermit", SUIT_MAJOR, 0, EFFECT_MAX_REPEL, WEATHER_NONE, ITEM_NONE, 0};
    cards[43] = (Card){card_the_wheelBitmap, "Wheel of Fortune", SUIT_MAJOR, 0, EFFECT_IMPROVE_SHINY, WEATHER_NONE, ITEM_NONE, 0};
    cards[44] = (Card){card_justiceBitmap, "Justice", SUIT_MAJOR, 0, EFFECT_MONEY_BOOST, WEATHER_NONE, ITEM_NONE, 0};
    cards[45] = (Card){card_the_hangedBitmap, "The Hanged Man", SUIT_MAJOR, 0, EFFECT_REMOVE_REPEL, WEATHER_NONE, ITEM_NONE, 0};
    cards[46] = (Card){card_deathBitmap, "Death", SUIT_MAJOR, 0, EFFECT_DEATHCARD, WEATHER_NONE, ITEM_NONE, 0};
    cards[47] = (Card){card_temperanceBitmap, "Temperance", SUIT_MAJOR, 0, EFFECT_POKEBALLS, WEATHER_NONE, ITEM_NONE, 0};
    cards[48] = (Card){card_the_devilBitmap, "The Devil", SUIT_MAJOR, 0, EFFECT_DEVILCARD, WEATHER_NONE, ITEM_NONE, 0};
    cards[49] = (Card){card_the_towerBitmap, "The Tower", SUIT_MAJOR, 0, EFFECT_TOWERCARD, WEATHER_NONE, ITEM_NONE, 0};
    cards[50] = (Card){card_the_starBitmap, "The Star", SUIT_MAJOR, 0, EFFECT_STARCARD, WEATHER_NONE, ITEM_NONE, 0};
    cards[51] = (Card){card_the_moonBitmap, "The Moon", SUIT_MAJOR, 0, EFFECT_SET_WEATHER, WEATHER_NONE, ITEM_NONE, 0};
    cards[52] = (Card){card_the_sunBitmap, "The Sun", SUIT_MAJOR, 0, EFFECT_SUNCARD, WEATHER_NONE, ITEM_NONE, 0};
    cards[53] = (Card){card_judgementBitmap, "Judgement", SUIT_MAJOR, 0, EFFECT_MAX_LURE, WEATHER_NONE, ITEM_NONE, 0};

}

Card* selectedCards[3];
int selectedCardCount = 0;


void handleCoinsPokéDollars(Card* selectedCard) {
    if (selectedCard->suit == SUIT_COINS) {
        // Calculate the percentage increase based on the rank
        int percentageIncrease = 20 + (selectedCard->rank - 1) * 10;

        // Apply the increase to the player's current PokéDollars
        int pokedollarsGranted = player.pokedollars * percentageIncrease / 100;

        // Add the granted PokéDollars to the player's total
        player.pokedollars += pokedollarsGranted;

        // Print out the result
        printf("Coins card granted: %d%% increase\n", percentageIncrease);
        printf("Total PokéDollars after increase: %d\n", player.pokedollars);
    }
}

void handleSwordsTokens(Card* selectedCard) {
    if (selectedCard->suit == SUIT_SWORDS) {
        // Calculate the number of swords tokens based on the rank (2^(rank-1))
        int tokensGranted = 1 << (selectedCard->rank - 1);  // Equivalent to 2^(rank - 1)

        // Add the granted tokens to the player's swords tokens
        player.swordsTokens += tokensGranted;

        // Print out the result
        printf("Swords card granted: %d tokens\n", tokensGranted);
        printf("Total swords tokens: %d\n", player.swordsTokens);
    }
}

void handleCupsHealing(Card* selectedCard) {
    if (selectedCard->suit == SUIT_CUPS) {
        // Calculate healing percentage based on the card's rank
        int healingPercentage = 20 + (selectedCard->rank - 1) * 10;  // Rank 1 gives 20%, Rank 8 gives 100%
        if (healingPercentage > 100) healingPercentage = 100;  // Ensure it doesn't exceed 100%

        // Calculate the healing amount based on the current health
        int healingAmount = (party.maxHealth * healingPercentage) / 100;
        party.currentHealth += healingAmount;

        // Ensure current health does not exceed max health
        if (party.currentHealth > party.maxHealth) {
            party.currentHealth = party.maxHealth;
        }

        printf("Cups card healing: %d%%\n", healingPercentage);
        printf("Healed %d HP. Current health: %d/%d\n", healingAmount, party.currentHealth, party.maxHealth);
    }
}

void handleWandsXP(Card* selectedCard) {
    if (selectedCard->suit == SUIT_WANDS) {
        // Calculate XP percentage based on rank (Rank 1 = 20%, Rank 8 = 100%)
        int xpPercentage = 20 + (selectedCard->rank - 1) * 10;  // Rank 1 gives 20%, Rank 8 gives 100%
        if (xpPercentage > 100) xpPercentage = 100;  // Ensure it doesn't exceed 100%

        // Assume a base XP amount you want to apply
        int baseXP = 50;  // Example base XP amount for the Wands card
        int xpBoost = (baseXP * xpPercentage) / 100;

        // Add the boosted XP to the player's current XP
        player.currentXP += xpBoost;

        // Print the result
        printf("Wands card XP boost: %d%%\n", xpPercentage);
        printf("Gained %d XP. Current XP: %d/%d\n", xpBoost, player.currentXP, player.maxXP);

        // Check if the player leveled up
        if (player.currentXP >= player.maxXP) {
            player.level++;
            player.currentXP -= player.maxXP;  // Reset current XP to 0 and carry over the remainder
            player.maxXP = player.maxXP * 2;  // Increase max XP for next level
            printf("Level Up! New level: %d\n", player.level);
        }
    }
}

void handleCardEffect(Card* selectedCard) {
    switch (selectedCard->effect) {
        case EFFECT_CUPS_HEALING:  // Cups effect
            handleCupsHealing(selectedCard);
            break;
        case EFFECT_SWORDS_TOKENS:  // This effect will be used for Swords
            handleSwordsTokens(selectedCard);
            break;
        case EFFECT_XP_BOOST:  // This effect will be used for Wands
            handleWandsXP(selectedCard);
            break;
        case EFFECT_COINS_GET:  // This effect will be used for Coins
            handleCoins(selectedCard);
            break;
        case EFFECT_REROLL:
            // Reroll logic for Fool Arcana
            int newIndex = rand() % 54;
            printf("Rerolled to: %s\n", cards[newIndex].name);
            break;
        case EFFECT_CHOOSE_SUIT: 
            effectChooseSuitGain(); 
            break;
        case EFFECT_LEVEL_UP: 
            effectInstantLevelUp(); 
            break;
        case EFFECT_GRANT_HITEM:
            effectGiveHealingItem(); 
            break;
        case EFFECT_REROLL_MAJOR: 
            effectRerollIntoMajor(card); 
            break;
        case EFFECT_CHOOSE_TWO: 
            effectDrawTwoCards(); 
            break;
        case EFFECT_DRAW_CASH: 
            effectLoversBonus(); 
            break;
        case EFFECT_CARBOS_GET:
            effectChariotSpeedBoost();
            break;
        case EFFECT_GRANT_VITAMIN:
            effectStrengthVitamin();
            break;
        case EFFECT_MAX_REPEL:
            effectHermitRepel();
            break;
        case EFFECT_IMPROVE_SHINY:
            effectWheelShinyBoost();
            break;
        case EFFECT_MONEY_BOOST:
            effectJusticeMoneyBoost();
            break;
        case EFFECT_REMOVE_REPEL:
            effectHangedManRemoveRepel();
            break;
        case EFFECT_DEATHCARD: 
            effectDeathNextRun(); 
            break;
        case EFFECT_GIVE_BALLS: 
            effectTemperanceGiveBalls(); 
            break;
        case EFFECT_DEVILCARD: 
            effectDevil(); 
            break;
        case EFFECT_TOWERCARD: 
            effectTower(); 
            break;
        case EFFECT_STARCARD: 
            effectStarBoostSuits(); 
            break;
        case EFFECT_SUNCARD: 
            effectSunFullHealAndDraw(); 
            break;
        case EFFECT_MAX_LURE: 
            effectJudgmentGiveLure(); 
            break;
        case EFFECT_REVEAL_MAP: 
            effectWorldRevealMap(); 
            break;
        default:
            printf("No effect for this card.\n");
            break;
    }
}


void effectChooseSuitGain() {
    printf("Choose a suit to gain a card from:\n1. Cups\n2. Wands\n3. Swords\n4. Coins\n");
    int choice = wait_for_input();  // Replace with your input method

    SuitType chosenSuit;
    switch (choice) {
        case 1: chosenSuit = SUIT_CUPS; break;
        case 2: chosenSuit = SUIT_WANDS; break;
        case 3: chosenSuit = SUIT_SWORDS; break;
        case 4: chosenSuit = SUIT_COINS; break;
        default: return;
    }

    // Find a random card of that suit
    Card* result = NULL;
    while (!result) {
        Card* candidate = &cards[rand() % MAX_CARDS];
        if (candidate->suit == chosenSuit) {
            result = candidate;
        }
    }

    printf("You gained: %s\n", result->name);
    give_card_to_player(result);
}


void effectInstantLevelUp() {
    printf("Select a Pokémon to level up instantly.\n");
    int selected = select_party_member();  // Your party selection function

    if (selected >= 0) {
        level_up_pokemon(selected);
        printf("Your Pokémon leveled up!\n");
    }
}


void effectGiveHealingItem() {
    ItemType healingItems[] = { ITEM_ETHER, ITEM_FULL_HEAL, ITEM_POTION, ITEM_SUPER_POTION, ITEM_HYPER_POTION, ITEM_MAX_POTION, ITEM_FULL_RESTORE, ITEM_REVIVE, ITEM_MAX_REVIVE };
    ItemType item = healingItems[rand() % 9];

    add_item_to_bag(item, 1);
    printf("You received a %s!\n", get_item_name(item));
}


void effectRerollIntoMajor(Card* currentCard) {
    Card* rerolled = NULL;
    while (!rerolled) {
        Card* candidate = &cards[rand() % MAX_CARDS];
        if (candidate->suit == SUIT_MAJOR && candidate != currentCard) {
            rerolled = candidate;
        }
    }

    printf("Card rerolled to: %s\n", rerolled->name);
    *currentCard = *rerolled;
}


void effectDrawTwoCards() {
    printf("You may draw 2 more cards.\n");

    for (int i = 0; i < 2 && selectedCardCount < 5; ++i) {
        Card* drawn = &cards[rand() % MAX_CARDS];
        selectedCards[selectedCardCount++] = drawn;
        printf("Drew card: %s\n", drawn->name);
    }
}


void effectLoversBonus() {
    printf("You may draw 1 more card.\n");

    if (selectedCardCount < 5) {
        Card* drawn = &cards[rand() % MAX_CARDS];
        selectedCards[selectedCardCount++] = drawn;
        printf("Drew card: %s\n", drawn->name);
    }

    // Now give a random Coins suit card
    Card* coinsCard = NULL;
    while (!coinsCard) {
        Card* candidate = &cards[rand() % MAX_CARDS];
        if (candidate->suit == SUIT_COINS) {
            coinsCard = candidate;
        }
    }

    printf("You received a bonus Coins card: %s\n", coinsCard->name);
    give_card_to_player(coinsCard);
}


void effectChariotSpeedBoost() {
    const ItemType speedVitamin = ITEM_CARBOS; // Or whichever constant is used
    add_item_to_bag(speedVitamin, 1);
    printf("You received a Carbos!\n");
}


void effectStrengthVitamin() {
    ItemType vitamins[] = {
        ITEM_PROTEIN,
        ITEM_IRON,
        ITEM_CARBOS,
        ITEM_CALCIUM,
        ITEM_ZINC,
        ITEM_HP_UP
    };
    ItemType chosen = vitamins[rand() % 6];
    add_item_to_bag(chosen, 1);
    printf("You received a %s!\n", get_item_name(chosen));
}


void effectHermitRepel() {
    const ItemType maxRepel = ITEM_MAX_REPEL; // Or whichever constant is used
    add_item_to_bag(maxRepel, 1);
    printf("You received a Max Repel!\n");
}


void effectWheelShinyBoost() {
    set_flag(FLAG_SHINY_BOOST);  // You'd need custom logic in wild Pokémon spawn
    printf("Your chances of finding a Shiny Pokémon have increased!\n");
}


void effectJusticeMoneyBoost() {
    set_flag(FLAG_MONEY_BOOST);
    printf("Trainers will now give extra prize money until you leave the area.\n");
}


void effectHangedManRemoveRepel() {
    clear_flag(FLAG_REPEL_ACTIVE);
    printf("All repel effects have been removed.\n");
}


void effectDeathNextRun() {
    set_flag(FLAG_NEXT_ENCOUNTER_FLEES);  // Hook into wild battle setup
    printf("A shadow looms... your next encounter will flee.\n");
}


void effectTemperanceGiveBalls() {
    int roll = rand() % 100;
    if (roll < 40) {
        add_item_to_bag(ITEM_POKEBALL, 1 + rand() % 5);
        printf("You received some Poké Balls.\n");
    } else if (roll < 70) {
        add_item_to_bag(ITEM_GREATBALL, 1 + rand() % 4);
        printf("You received some Great Balls.\n");
    } else if (roll < 95) {
        add_item_to_bag(ITEM_ULTRABALL, 1 + rand() % 3);
        printf("You received some Ultra Balls.\n");
    } else {
        add_item_to_bag(ITEM_MASTERBALL, 1);
        printf("Incredible! You received a Master Ball.\n");
    }
}


void effectDevil() {
    set_flag(FLAG_NO_MONEY_REWARD);
    printf("The Devil has taken your prize... No money this battle.\n");

    for (int i = 0; i < 2 && selectedCardCount < 5; ++i) {
        Card* drawn = &cards[rand() % MAX_CARDS];
        selectedCards[selectedCardCount++] = drawn;
        printf("Drew card: %s\n", drawn->name);
    }
}


void effectTower() {
    halve_party_hp();  // Custom function to reduce all party mons' HP by half
    printf("Calamity strikes! Your team’s HP was halved!\n");

    Card* wandsCard = NULL;
    Card* swordsCard = NULL;
    while (!wandsCard || !swordsCard) {
        Card* c = &cards[rand() % MAX_CARDS];
        if (!wandsCard && c->suit == SUIT_WANDS) wandsCard = c;
        else if (!swordsCard && c->suit == SUIT_SWORDS) swordsCard = c;
    }

    printf("You drew: %s and %s\n", wandsCard->name, swordsCard->name);
    give_card_to_player(wandsCard);
    give_card_to_player(swordsCard);
}


void effectStarBoostSuits() {
    set_flag(FLAG_SUIT_EFFECT_BOOSTED);
    printf("A shining power enhances all suits until you leave the area!\n");
}


void effectSunFullHealAndDraw() {
    struct Pokemon* party = (struct Pokemon*) 0x02024284;
    for (int i = 0; i < 6; i++) {
        if (GetMonData(&party[i], MON_DATA_SPECIES, NULL) != 0) {
            u16 maxHP = GetMonData(&party[i], MON_DATA_MAX_HP, NULL);
            SetMonData(&party[i], MON_DATA_HP, &maxHP);
            u8 maxPP;
            for (int j = 0; j < 4; ++j) {
                maxPP = GetMonData(&party[i], MON_DATA_MOVE1PP + j, NULL);
                SetMonData(&party[i], MON_DATA_MOVE1PP + j, &maxPP);
            }
        }
    }
    printf("The light of the Sun fully healed your party!\n");

    if (selectedCardCount < 5) {
        Card* drawn = &cards[rand() % MAX_CARDS];
        selectedCards[selectedCardCount++] = drawn;
        printf("You drew an extra card: %s\n", drawn->name);
    }
}


void effectJudgmentGiveLure() {
    add_item_to_bag(ITEM_MAX_LURE, 1);  // Replace with actual constant if needed
    printf("You received a Max Lure!\n");
}

void effectWorldRevealMap() {
    set_flag(FLAG_DUNGEON_MAP_REVEALED);  // Custom flag your map renderer checks
    printf("The World reveals the entire map to you!\n");
}


