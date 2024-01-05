#include <iostream>
#include <unordered_map>
#include <map>
#include <set>

#include "../OMPEval/omp/HandEvaluator.h"

using namespace std;
using namespace omp;

enum class ACTIONS
{
    PASS = 0,
    BET,
    NUM
};

enum class RANKS
{
    TWO = 0,
    THREE,
    FOUR,
    FIVE,
    SIX,
    SEVEN,
    EIGHT,
    NINE,
    TEN,
    J,
    Q,
    K,
    A,
    NUM,
};

enum class SUITS
{
    CLUBS = 0,
    DIAMONDS,
    HEARTS,
    SPADES,
    NUM,
    X = 10,
    Y,
    Z,
    W,
};

const int NUM_CARDS = (int)RANKS::NUM * (int)SUITS::NUM;

enum class STRATEGY_ACTIONS
{
    OOP_BET = 0,
    OOP_CHECK_CALL,
    OOP_CHECK_FOLD,
    IP_CALL,
    IP_FOLD,
    IP_BET,
    IP_CHECK_BACK,
    NOT_A_ROOT_NODE,
    NUM
};

const vector<string> strategyActionStrings = {
    "OOP_BET",
    "OOP_CHECK_CALL",
    "OOP_CHECK_FOLD",
    "IP_CALL",
    "IP_FOLD",
    "IP_BET",
    "IP_CHECK_BACK",
    "NOT_A_ROOT_NODE",
};

enum class STRAIGHT_DRAW_CATEGORY
{
    NONE = 0,
    GUTSHOT_ONE_CARD,
    GUTSHOT_TWO_CARD,
    OPEN_ENDED_ONE_CARD,
    DOUBLE_GUTSHOT_ONE_CARD,
    OPEN_ENDED_TWO_CARD,
    DOUBLE_GUTSHOT_TWO_CARD,
};

enum class FLUSH_DRAW_CATEGORY
{
    BACK_DOOR_TWO_CARD,
    FRONT_DOOR_TWO_CARD,
    FRONT_DOOR_ONE_CARD,
    NONE,
};

enum class HAND_CATEGORY
{
    HIGH_CARD = 0,
    PAIR_ON_BOARD,
    POCKET_PAIR,
    PAIR_WITH_ONE_CARD_IN_HAND,
    TWO_PAIR_WITH_POCKET_PAIR,
    TWO_PAIR_ONE_ON_HOLE,
    TWO_PAIR_BOTH_IN_HOLE,
    BOARD_TRIPS,
    TRIPS,
    SET,
    STRAIGHT,
    FLUSH,
    BOAT,
    QUADS,
    STRAIGHT_FLUSH,
    NUM
};

// linear with global iteration number
// 1,000,000 -0.0347588, -0.0341119
// 2,000,000 -0.0218729
// 5,000,000 -0.0121249
// 10,000,000 -0.00782353, -0.00765364
// 20,000,000
// 40,000,000

// normal cfr
// 1,000,000 -0.0401731
// 2,000,000 -0.0314868,
// 5,000,000 -0.0215416
// 10,000,000 -0.0125153
// 20,000,000
// 40,000,000

// normal cfr, sampling ev during just last half
// 1,000,000 -0.0259818 -0.0278989
// 2,000,000 -0.0216574
// 5,000,000 -0.00917349

// normal cfr, sampling ev during just last tenth
// 1,000,000 -0.018369 -0.0187016 -0.0193096
// 2,000,000 -0.0256384 -0.00998227 -0.0072947 ??? all over the place
// 5,000,000

// linear within each node, sampling ev just last tenth:
// 1,000,000 -0.0290438 -0.023262 -0.016356 -0.0318114 -0.030205
// 2,000,000 -0.0140967 -0.00641423 -0.0190033
// 5,000,000 -0.00488722

// linear within each node, sampling ev just last fifth:
// 1,000,000 -0.0268436 -0.0241628 -0.0284217
// 2,000,000 -0.0150206 -0.0188565 -0.0142538
// 5,000,000 -0.00906665 -0.00542614 -0.00493395
// 10,000,000 -0.00446246 -0.00498335
// 20,000,000  -0.00144748

// linear within each node, sampling ev just last fifth, using t^2:
// 1,000,000 -0.022089 -0.029191 -0.0218712
// 2,000,000 -0.0172322, -0.0196545 -0.0154876
// 5,000,000 -0.00927754 -0.00847713
// 10,000,000 -0.00816232
// 20,000,000 -0.00121892

// linear within each node, using t^2, normalizing strategy at end:
// 1,000,000
// 2,000,000
// 5,000,000 -0.0102154 ... nah, better
// 10,000,000 0.00559911 0.00483644
// 20,000,000 0.00582565
// 40,000,000

// 5 mil seems enough with 2 for ev calc

// next up: put these parameters in the solver itself instead of defines up top
// make two: one for GTO, one for dealer strategy
// when printing out, show the dealer strategy and specify with each node if it's a deviation

set<string> hardCodedHands = {
    "2x2y",
    "2x2z",
    "2y2z",
    "2z2x",
    "2z2y",
    "2z2w",
    "3x2x",
    "3x2y",
    "3x2z",
    "3z2x",
    "3z2y",
    "3z2z",
    "3z2w",
    "4y2y",
    "4x3x",
    "4x3z",
    "4y3x",
    "4y3z",
    "4z3x",
    "4z3z",
    "4z3w",
    "4x4y",
    "4x4z",
    "4y4z",
    "4z4x",
    "4z4y",
    "4z4w",
    "5y2y",
    "5x3x",
    "5x3z",
    "5y3x",
    "5y3z",
    "5z3x",
    "5z3z",
    "5z3w",
    "5x4x",
    "5x4y",
    "5x4z",
    "5y4x",
    "5y4y",
    "5y4z",
    "5z4x",
    "5z4y",
    "5z4z",
    "5z4w",
    "5x5y",
    "5x5z",
    "5y5z",
    "5z5x",
    "5z5y",
    "5z5w",
    "6y2y",
    "6x3x",
    "6x3z",
    "6y3x",
    "6y3z",
    "6z3x",
    "6z3z",
    "6z3w",
    "6x4x",
    "6x4y",
    "6x4z",
    "6y4x",
    "6y4y",
    "6y4z",
    "6z4x",
    "6z4y",
    "6z4z",
    "6z4w",
    "6x5x",
    "6x5y",
    "6x5z",
    "6y5x",
    "6y5y",
    "6y5z",
    "6z5x",
    "6z5y",
    "6z5z",
    "6z5w",
    "6x6y",
    "6x6z",
    "6y6z",
    "6z6x",
    "6z6y",
    "6z6w",
    "7x2x",
    "7x2y",
    "7x2z",
    "7z2x",
    "7z2y",
    "7z2z",
    "7z2w",
    "7z3x",
    "7z3z",
    "7z3w",
    "7x4x",
    "7x4y",
    "7x4z",
    "7z4x",
    "7z4y",
    "7z4z",
    "7z4w",
    "7x5x",
    "7x5y",
    "7x5z",
    "7z5x",
    "7z5y",
    "7z5z",
    "7z5w",
    "7x6x",
    "7x6y",
    "7x6z",
    "7z6x",
    "7z6y",
    "7z6z",
    "7z6w",
    "7x7z",
    "7z7w",
    "8y2y",
    "8x3x",
    "8x3z",
    "8y3x",
    "8y3z",
    "8z3x",
    "8z3z",
    "8z3w",
    "8y4y",
    "8y5y",
    "8x6x",
    "8x6y",
    "8x6z",
    "8y6x",
    "8y6y",
    "8y6z",
    "8z6x",
    "8z6y",
    "8z6z",
    "8z6w",
    "8x7x",
    "8x7z",
    "8z7x",
    "8z7z",
    "8z7w",
    "8x8y",
    "8x8z",
    "8y8z",
    "8z8x",
    "8z8y",
    "8z8w",
    "9y2y",
    "9x3x",
    "9x3z",
    "9y3x",
    "9y3z",
    "9z3x",
    "9z3z",
    "9z3w",
    "9y4y",
    "9y5y",
    "9x6x",
    "9x6y",
    "9x6z",
    "9y6x",
    "9y6y",
    "9y6z",
    "9z6x",
    "9z6y",
    "9z6z",
    "9z6w",
    "9x7x",
    "9x7z",
    "9y7x",
    "9z7x",
    "9z7z",
    "9z7w",
    "9x8x",
    "9x8y",
    "9x8z",
    "9y8x",
    "9y8y",
    "9y8z",
    "9z8x",
    "9z8y",
    "9z8z",
    "9z8w",
    "9x9z",
    "9z9x",
    "9z9w",
    "Ty2x",
    "Ty2z",
    "Tz2x",
    "Tz2y",
    "Tz2z",
    "Tz2w",
    "Tz3x",
    "Tz3z",
    "Tz3w",
    "Ty4z",
    "Tz4x",
    "Tz4y",
    "Tz4z",
    "Tz4w",
    "Ty5z",
    "Tz5x",
    "Tz5y",
    "Tz5z",
    "Tz5w",
    "Tz6x",
    "Tz6z",
    "Tz6w",
    "Tz7x",
    "Tz7z",
    "Tz7w",
    "Ty8x",
    "Ty8z",
    "Tz8x",
    "Tz8z",
    "Tz8w",
    "Ty9x",
    "Tz9x",
    "Tz9z",
    "Tz9w",
    "TzTy",
    "Jy2y",
    "Jx3x",
    "Jx3z",
    "Jy3x",
    "Jy3z",
    "Jz3x",
    "Jz3z",
    "Jz3w",
    "Jx4x",
    "Jy4x",
    "Jy4y",
    "Jy4z",
    "Jz4y",
    "Jx5x",
    "Jy5x",
    "Jy5y",
    "Jy5z",
    "Jx6x",
    "Jx6y",
    "Jy6x",
    "Jy6y",
    "Jy6z",
    "Jz6y",
    "Jx7x",
    "Jx7z",
    "Jy7z",
    "Jz7x",
    "Jz7z",
    "Jz7w",
    "Jx8x",
    "Jx8y",
    "Jx8z",
    "Jy8x",
    "Jy8z",
    "Jz8x",
    "Jz8y",
    "Jz8z",
    "Jz8w",
    "Jx9x",
    "Jx9y",
    "Jx9z",
    "Jy9x",
    "Jy9z",
    "Jz9x",
    "Jz9y",
    "Jz9z",
    "Jz9w",
    "JzTz",
    "JzTw",
    "Qx3x",
    "Qx3z",
    "Qy3x",
    "Qy3z",
    "Qz3x",
    "Qz3z",
    "Qz3w",
    "Qx4x",
    "Qy4x",
    "Qy4z",
    "Qx5x",
    "Qy5x",
    "Qy5z",
    "Qx6x",
    "Qy6x",
    "Qy6z",
    "Qz6y",
    "Qx7x",
    "Qx7z",
    "Qy7x",
    "Qy7z",
    "Qz7x",
    "Qz7z",
    "Qz7w",
    "Qx8x",
    "Qx9x",
    "Qx9y",
    "Qx9z",
    "Qy9z",
    "Qz9x",
    "Qz9z",
    "Qz9w",
    "QxTz",
    "QzTz",
    "QzTw",
    "QxJx",
    "QxJy",
    "QxJz",
    "QyJx",
    "QyJz",
    "QzJx",
    "QzJy",
    "QzJz",
    "QzJw",
    "Kx3x",
    "Kx3z",
    "Ky3x",
    "Ky3z",
    "Kz3x",
    "Kz3z",
    "Kz3w",
    "Kx7x",
    "Kx7z",
    "Ky7x",
    "Ky7z",
    "Kz7x",
    "Kz7z",
    "Kz7w",
    "Kx8z",
    "Kz8x",
    "Kz8z",
    "Kz8w",
    "Kx9x",
    "Kx9y",
    "Kx9z",
    "Ky9x",
    "Kz9x",
    "Kz9y",
    "Kz9z",
    "Kz9w",
    "KxTz",
    "KzTz",
    "KzTw",
    "KxJx",
    "KxJy",
    "KxJz",
    "KyJx",
    "KyJz",
    "KzJx",
    "KzJy",
    "KzJz",
    "KzJw",
    "KxQx",
    "KxQy",
    "KxQz",
    "KyQx",
    "KyQz",
    "KzQx",
    "KzQy",
    "KzQz",
    "KzQw",
    "Ax2x",
    "Ax2y",
    "Ax2z",
    "Ay2x",
    "Ay2z",
    "Az2x",
    "Az2y",
    "Az2w",
    "Ax3x",
    "Ax3z",
    "Ay3x",
    "Ay3z",
    "Az3x",
    "Az3z",
    "Az3w",
    "Ax4x",
    "Ax4y",
    "Ax4z",
    "Ay4x",
    "Ay4z",
    "Az4x",
    "Az4y",
    "Az4z",
    "Az4w",
    "Ax5x",
    "Ax5y",
    "Ax5z",
    "Ay5x",
    "Ay5z",
    "Az5x",
    "Az5y",
    "Az5z",
    "Az5w",
    "Ax6x",
    "Ax6y",
    "Ax6z",
    "Ay6x",
    "Ay6z",
    "Az6x",
    "Az6y",
    "Az6z",
    "Az6w",
    "Ax7x",
    "Ax7z",
    "Ay7x",
    "Ay7z",
    "Az7x",
    "Az7z",
    "Az7w",
    "Ax8x",
    "Ax8y",
    "Ax8z",
    "Ay8x",
    "Ay8y",
    "Ay8z",
    "Az8x",
    "Az8y",
    "Az8z",
    "Az8w",
    "Ax9x",
    "Ax9y",
    "Ax9z",
    "Ay9x",
    "Ay9y",
    "Ay9z",
    "Az9x",
    "Az9y",
    "Az9z",
    "Az9w",
    "AxTy",
    "AxTz",
    "AyTz",
    "AzTz",
    "AzTw",
    "AxJx",
    "AxJy",
    "AxJz",
    "AyJx",
    "AyJy",
    "AyJz",
    "AzJx",
    "AzJy",
    "AzJz",
    "AzJw",
    "AxQx",
    "AxQy",
    "AxQz",
    "AyQx",
    "AyQy",
    "AyQz",
    "AzQx",
    "AzQy",
    "AzQz",
    "AzQw",
    "AxKx",
    "AxKy",
    "AxKz",
    "AyKx",
    "AyKy",
    "AyKz",
    "AzKx",
    "AzKy",
    "AzKz",
    "AzKw",
};

string CategoriesToString(HAND_CATEGORY handCategory, STRAIGHT_DRAW_CATEGORY straightDrawCategory, FLUSH_DRAW_CATEGORY flushDrawCategory)
{
    switch(handCategory)
    {
        case HAND_CATEGORY::STRAIGHT_FLUSH:
            return "STRAIGHT_FLUSH";
        case HAND_CATEGORY::QUADS:
            return "QUADS";
        case HAND_CATEGORY::BOAT:
            return "BOAT";
        case HAND_CATEGORY::FLUSH:
            return "FLUSH";
        case HAND_CATEGORY::STRAIGHT:
            return "STRAIGHT";
        case HAND_CATEGORY::SET:
            return "SET";
        case HAND_CATEGORY::TRIPS:
            return "TRIPS";
//        case HAND_CATEGORY::BOARD_TRIPS:
//            return "BOARD_TRIPS";
        case HAND_CATEGORY::TWO_PAIR_BOTH_IN_HOLE:
            return "TWO_PAIR_BOTH_IN_HOLE";
        case HAND_CATEGORY::TWO_PAIR_ONE_ON_HOLE:
            return "TWO_PAIR_ONE_ON_HOLE";
        case HAND_CATEGORY::TWO_PAIR_WITH_POCKET_PAIR:
            return "TWO_PAIR_WITH_POCKET_PAIR";
        case HAND_CATEGORY::PAIR_WITH_ONE_CARD_IN_HAND:
            return "PAIR_WITH_ONE_CARD_IN_HAND";
        case HAND_CATEGORY::POCKET_PAIR:
            return "POCKET_PAIR";
//        case HAND_CATEGORY::PAIR_ON_BOARD:
//            return "PAIR_ON_BOARD";
        default:
        {
            if (flushDrawCategory == FLUSH_DRAW_CATEGORY::FRONT_DOOR_TWO_CARD)
            {
                return "FRONT_DOOR_TWO_CARD";
            }
            if (flushDrawCategory == FLUSH_DRAW_CATEGORY::FRONT_DOOR_ONE_CARD)
            {
                return "FRONT_DOOR_ONE_CARD";
            }
            if (straightDrawCategory == STRAIGHT_DRAW_CATEGORY::OPEN_ENDED_TWO_CARD ||
                straightDrawCategory == STRAIGHT_DRAW_CATEGORY::DOUBLE_GUTSHOT_TWO_CARD)
            {
                return "OPEN_ENDED_TWO_CARD";
            }
            if (straightDrawCategory == STRAIGHT_DRAW_CATEGORY::GUTSHOT_TWO_CARD)
            {
                if (flushDrawCategory == FLUSH_DRAW_CATEGORY::BACK_DOOR_TWO_CARD)
                {
                    return "GUTSHOT_TWO_CARD_WITH_BACK_DOOR";
                }
                else
                {
                    return "GUTSHOT_TWO_CARD";
                }
            }
            if (straightDrawCategory == STRAIGHT_DRAW_CATEGORY::OPEN_ENDED_ONE_CARD ||
                straightDrawCategory == STRAIGHT_DRAW_CATEGORY::DOUBLE_GUTSHOT_ONE_CARD)
            {
                return "OPEN_ENDED_ONE_CARD";
            }
            if (straightDrawCategory == STRAIGHT_DRAW_CATEGORY::GUTSHOT_ONE_CARD)
            {
                if (flushDrawCategory == FLUSH_DRAW_CATEGORY::BACK_DOOR_TWO_CARD)
                {
                    return "GUTSHOT_ONE_CARD_WITH_BACK_DOOR";
                }
                else
                {
                    return "GUTSHOT_ONE_CARD";
                }
            }
            if (flushDrawCategory == FLUSH_DRAW_CATEGORY::BACK_DOOR_TWO_CARD)
            {
                return "BACK_DOOR";
            }
            return "HIGH_CARD";
        }
    }
}


HandEvaluator handEvaluator;

class Node
{
public:
    double regretSums[(int)ACTIONS::NUM];
    double strategySums[(int)ACTIONS::NUM];
    string infoSet;
    int timesNodeVisited = 0;
    
    Node(string _infoSet)
    {
        infoSet = _infoSet;
        for (int i = 0; i < (int)ACTIONS::NUM; i++)
        {
            regretSums[i] = 0;
            strategySums[i] = 0;
        }
    }

    Node(string _infoSet, ACTIONS hardCodedAction)
    : Node(_infoSet)
    {
        regretSums[(int)hardCodedAction] = 1;
        strategySums[(int)hardCodedAction] = 1;
    }

    void GetNormalized(double strategy[], const double input[]) const
    {
        double totatInput = 0;
        for (int i = 0; i < (int)ACTIONS::NUM; i++)
        {
            totatInput += input[i];
        }
        if (totatInput == 0)
        {
            for (int i = 0; i < (int)ACTIONS::NUM; i++)
            {
                strategy[i] = 1.0 / (int)ACTIONS::NUM;
            }
        }
        else
        {
            for (int i = 0; i < (int)ACTIONS::NUM; i++)
            {
                strategy[i] = input[i] / totatInput;
            }
        }
    }
    
    void GetStrategy(double strategy[]) const
    {
        GetNormalized(strategy, regretSums);
    }
    
    void GetAverageStrategy(double strategy[]) const
    {
        GetNormalized(strategy, strategySums);
    }
    
    void UpdateStrategySums(double strategy[], double realizationWeight)
    {
        timesNodeVisited++;
        for (int i = 0; i < (int)ACTIONS::NUM; i++)
        {
            strategySums[i] += realizationWeight * strategy[i] * timesNodeVisited * timesNodeVisited;
        }
    }
    
    // not used in this implementation
    int GetActionFromStrategy(double strategy[]) const
    {
        double randomNumber = (double)rand() / RAND_MAX;
        double cumulativePercent = 0;
        for (int i = 0; i < (int)ACTIONS::NUM; i++)
        {
            cumulativePercent += strategy[i];
            if (randomNumber <= cumulativePercent)
            {
                return i;
            }
        }
        return (int)ACTIONS::NUM - 1;
    }
    
    void Print() const
    {
        double strategy[(int)ACTIONS::NUM];
        GetAverageStrategy(strategy);
        std::cout << infoSet << " Pass: " <<
            std::round(strategy[(int)ACTIONS::PASS] * 1000.0) / 1000.0 << " Bet: " <<
            std::round(strategy[(int)ACTIONS::BET] * 1000.0) / 1000.0 <<
            "\n";
    }
};

string RankToString(int rank)
{
    RANKS rankEnum = (RANKS)rank;
    string c = to_string(rank + 2);
    switch(rankEnum)
    {
        case RANKS::A:
            c = "A";
            break;
        case RANKS::K:
            c = "K";
            break;
        case RANKS::Q:
            c = "Q";
            break;
        case RANKS::J:
            c = "J";
            break;
        case RANKS::TEN:
            c = "T";
            break;
        default:
            break;
    }
    return c;
}

int CharToRank(char rankChar)
{
    RANKS rankEnum = RANKS::NUM;
    switch(rankChar)
    {
        case '2':
            rankEnum = RANKS::TWO;
            break;
        case '3':
            rankEnum = RANKS::THREE;
            break;
        case '4':
            rankEnum = RANKS::FOUR;
            break;
        case '5':
            rankEnum = RANKS::FIVE;
            break;
        case '6':
            rankEnum = RANKS::SIX;
            break;
        case '7':
            rankEnum = RANKS::SEVEN;
            break;
        case '8':
            rankEnum = RANKS::EIGHT;
            break;
        case '9':
            rankEnum = RANKS::NINE;
            break;
        case 'T':
            rankEnum = RANKS::TEN;
            break;
        case 'J':
            rankEnum = RANKS::J;
            break;
        case 'Q':
            rankEnum = RANKS::Q;
            break;
        case 'K':
            rankEnum = RANKS::K;
            break;
        case 'A':
            rankEnum = RANKS::A;
            break;
        default:
            break;
    }
    return (int)rankEnum;
}

string SuitToString(int suit)
{
    SUITS suitEnum = (SUITS)suit;
    string s;
    switch(suitEnum)
    {
        case SUITS::CLUBS:
            s = "c";
            break;
        case SUITS::DIAMONDS:
            s = "d";
            break;
        case SUITS::HEARTS:
            s = "h";
            break;
        case SUITS::SPADES:
            s = "s";
            break;
        case SUITS::X:
            s = "x";
            break;
        case SUITS::Y:
            s = "y";
            break;
        case SUITS::Z:
            s = "z";
            break;
        case SUITS::W:
            s = "w";
            break;
        default:
            break;
    }
    return s;
}

int CharToSuit(char suitChar)
{
    SUITS suitEnum = SUITS::NUM;
    switch(suitChar)
    {
        case 'c':
        case 'x':
            suitEnum = SUITS::CLUBS;
            break;
        case 'd':
        case 'y':
            suitEnum = SUITS::DIAMONDS;
            break;
        case 'h':
        case 'z':
            suitEnum = SUITS::HEARTS;
            break;
        case 's':
        case 'w':
            suitEnum = SUITS::SPADES;
            break;
        default:
            break;
    }
    return (int)suitEnum;
}

string CardToString(int rank, int suit)
{
    string c = RankToString(rank);
    string s = SuitToString(suit);
    return c + s;
}

string CardToString(int card)
{
    int rank = card / (int)SUITS::NUM;
    int suit = card % (int)SUITS::NUM;
    return CardToString(rank, suit);
}

int StringToCard(string card)
{
    if (card.size() != 2) return -1;
    int rank = CharToRank(card[0]);
    int suit = CharToSuit(card[1]);
    return rank * (int)SUITS::NUM + suit;
}

STRAIGHT_DRAW_CATEGORY CheckForStraightDraws(int flop0, int flop1, int flop2, int hand0, int hand1)
{
    //note: this is incorrect when one of the hole cards matches the board, but that doesn't happen to affect us.

    //todo: check for double gutshots.

    int flop0Rank = flop0 / (int)SUITS::NUM;
    int flop1Rank = flop1 / (int)SUITS::NUM;
    int flop2Rank = flop2 / (int)SUITS::NUM;
    int hand0Rank = hand0 / (int)SUITS::NUM;
    int hand1Rank = hand1 / (int)SUITS::NUM;

    bool rankExists[13];
    for (int i = 0; i < 13; i++)
    {
        rankExists[i] = false;
    }

    rankExists[flop0Rank] = true;
    rankExists[flop1Rank] = true;
    rankExists[flop2Rank] = true;
    rankExists[hand0Rank] = true;
    rankExists[hand1Rank] = true;
    
    STRAIGHT_DRAW_CATEGORY bestDraw = STRAIGHT_DRAW_CATEGORY::NONE;
    
    for (int i = 0; i < 9; i++)
    {
        int existingRanksForStraightStartingHere = 0;
        if (rankExists[i]) existingRanksForStraightStartingHere++;
        if (rankExists[i+1]) existingRanksForStraightStartingHere++;
        if (rankExists[i+2]) existingRanksForStraightStartingHere++;
        if (rankExists[i+3]) existingRanksForStraightStartingHere++;
        if (rankExists[i+4]) existingRanksForStraightStartingHere++;
        if (existingRanksForStraightStartingHere == 4)
        {
            bool bothHoleCardsContribute =
                (hand0Rank >= i && hand0Rank <= i + 4 &&
                 hand1Rank >= i && hand1Rank <= i + 4);

            if (!rankExists[i+1] ||
                !rankExists[i+2] ||
                !rankExists[i+3])
            {
                bestDraw = max(bestDraw, bothHoleCardsContribute ?
                               STRAIGHT_DRAW_CATEGORY::GUTSHOT_TWO_CARD :
                               STRAIGHT_DRAW_CATEGORY::GUTSHOT_ONE_CARD);
            }
            else if (!rankExists[i])
            {
                if (i == (int)RANKS::TEN)
                {
                    bestDraw = max(bestDraw, bothHoleCardsContribute ?
                                   STRAIGHT_DRAW_CATEGORY::GUTSHOT_TWO_CARD :
                                   STRAIGHT_DRAW_CATEGORY::GUTSHOT_ONE_CARD);
                }
                else
                {
                    bestDraw = max(bestDraw, bothHoleCardsContribute ?
                                   STRAIGHT_DRAW_CATEGORY::OPEN_ENDED_TWO_CARD :
                                   STRAIGHT_DRAW_CATEGORY::OPEN_ENDED_ONE_CARD);
                }
            }
            else
            {
                bestDraw = max(bestDraw, bothHoleCardsContribute ?
                               STRAIGHT_DRAW_CATEGORY::OPEN_ENDED_TWO_CARD :
                               STRAIGHT_DRAW_CATEGORY::OPEN_ENDED_ONE_CARD);
            }
        }
    }
    
    // check for wheel
    if (rankExists[(int)RANKS::A])
    {
        int existingRanksForStraightStartingHere = 1;
        if (rankExists[0]) existingRanksForStraightStartingHere++;
        if (rankExists[1]) existingRanksForStraightStartingHere++;
        if (rankExists[2]) existingRanksForStraightStartingHere++;
        if (rankExists[3]) existingRanksForStraightStartingHere++;
        if (existingRanksForStraightStartingHere == 4)
        {
            // make sure both hold cards contribute
            bool bothHoleCardsContribute = ((hand0Rank == (int)RANKS::A || (hand0Rank >= 0 && hand0Rank <= 3)) &&
                                       (hand1Rank == (int)RANKS::A || (hand1Rank >= 0 && hand1Rank <= 3)));
            bestDraw = max(bestDraw, bothHoleCardsContribute ?
                           STRAIGHT_DRAW_CATEGORY::GUTSHOT_TWO_CARD :
                           STRAIGHT_DRAW_CATEGORY::GUTSHOT_ONE_CARD);
        }
    }
    
    return bestDraw;
}

FLUSH_DRAW_CATEGORY CheckForFlushDraws(int flop0, int flop1, int flop2, int hand0, int hand1)
{
    int flop0Suit = flop0 % (int)SUITS::NUM;
    int flop1Suit = flop1 % (int)SUITS::NUM;
    int flop2Suit = flop2 % (int)SUITS::NUM;
    int hand0Suit = hand0 % (int)SUITS::NUM;
    int hand1Suit = hand1 % (int)SUITS::NUM;
    
    if (hand0Suit == hand1Suit)
    {
        int boardMatchedSuit = 0;
        
        if (hand0Suit == flop0Suit) boardMatchedSuit++;
        if (hand0Suit == flop1Suit) boardMatchedSuit++;
        if (hand0Suit == flop2Suit) boardMatchedSuit++;
        if (boardMatchedSuit == 2)
        {
            return FLUSH_DRAW_CATEGORY::FRONT_DOOR_TWO_CARD;
        }
        else if (boardMatchedSuit == 1)
        {
            return FLUSH_DRAW_CATEGORY::BACK_DOOR_TWO_CARD;
        }
    }
    if (flop0Suit == flop1Suit && flop0Suit == flop2Suit)
    {
        if (hand0Suit == flop0Suit || hand1Suit == flop0Suit)
        {
            return FLUSH_DRAW_CATEGORY::FRONT_DOOR_ONE_CARD;
        }
    }
    return FLUSH_DRAW_CATEGORY::NONE;
}



// 0 is draw.  1 means player 0 wins.  -1 means player 1 wins
// community cards are in slots 0,1,2.  player cards are in 3,4 and 5,6
int CheckShowdown(int deck[])
{
    Hand playerHand0 = Hand::empty();
    Hand playerHand1 = Hand::empty();
    playerHand0 +=
        Hand(deck[0]) +
        Hand(deck[1]) +
        Hand(deck[2]) +
        Hand(deck[3]) +
        Hand(deck[4]) +
        Hand(deck[7]) +
        Hand(deck[8]);
    playerHand1 +=
        Hand(deck[0]) +
        Hand(deck[1]) +
        Hand(deck[2]) +
        Hand(deck[5]) +
        Hand(deck[6]) +
        Hand(deck[7]) +
        Hand(deck[8]);
    
    uint16_t handValue0 = handEvaluator.evaluate(playerHand0);
    uint16_t handValue1 = handEvaluator.evaluate(playerHand1);
    
    return (handValue0 > handValue1) ? 1 : ((handValue0 < handValue1) ? -1 : 0);
}

void ReplaceSuits(int &flop0Suit, int &flop1Suit, int &flop2Suit, int &hand0Suit, int &hand1Suit, int suitToReplace, int newSuit)
{
    if (flop0Suit == suitToReplace) flop0Suit = newSuit;
    if (flop1Suit == suitToReplace) flop1Suit = newSuit;
    if (flop2Suit == suitToReplace) flop2Suit = newSuit;
    if (hand0Suit == suitToReplace) hand0Suit = newSuit;
    if (hand1Suit == suitToReplace) hand1Suit = newSuit;
}


string ConstructInfoSet(int flop0, int flop1, int flop2, int hand0, int hand1, string history)
{
    int flop0Rank = flop0 / (int)SUITS::NUM;
    int flop0Suit = flop0 % (int)SUITS::NUM;
    int flop1Rank = flop1 / (int)SUITS::NUM;
    int flop1Suit = flop1 % (int)SUITS::NUM;
    int flop2Rank = flop2 / (int)SUITS::NUM;
    int flop2Suit = flop2 % (int)SUITS::NUM;
    int hand0Rank = hand0 / (int)SUITS::NUM;
    int hand0Suit = hand0 % (int)SUITS::NUM;
    int hand1Rank = hand1 / (int)SUITS::NUM;
    int hand1Suit = hand1 % (int)SUITS::NUM;

    if (hand1Rank > hand0Rank)
    {
        swap(hand0Rank, hand1Rank);
        swap(hand0Suit, hand1Suit);
    }
    if (hand1Rank == hand0Rank && hand1Suit > hand0Suit)
    {
        swap(hand0Rank, hand1Rank);
        swap(hand0Suit, hand1Suit);
    }
    if (flop1Rank > flop0Rank)
    {
        swap(flop0Rank, flop1Rank);
        swap(flop0Suit, flop1Suit);
    }
    if (flop2Rank > flop1Rank)
    {
        swap(flop1Rank, flop2Rank);
        swap(flop1Suit, flop2Suit);
    }
    if (flop1Rank > flop0Rank)
    {
        swap(flop0Rank, flop1Rank);
        swap(flop0Suit, flop1Suit);
    }

    if (flop0Rank == flop1Rank && flop0Rank == flop2Rank)
    {
        if (hand0Rank == hand1Rank && hand0Suit != flop0Suit && hand0Suit != flop1Suit && hand0Suit != flop2Suit)
        {
            swap(hand0Rank, hand1Rank);
            swap(hand0Suit, hand1Suit);
        }
        if (flop1Suit == hand0Suit)
        {
            swap(flop0Rank, flop1Rank);
            swap(flop0Suit, flop1Suit);
        }
        else if (flop2Suit == hand0Suit)
        {
            swap(flop0Rank, flop2Rank);
            swap(flop0Suit, flop2Suit);
        }
        if (flop0Suit != hand0Suit)
        {
            if (flop1Suit == hand1Suit)
            {
                swap(flop0Rank, flop1Rank);
                swap(flop0Suit, flop1Suit);
            }
            else if (flop2Suit == hand1Suit)
            {
                swap(flop0Rank, flop2Rank);
                swap(flop0Suit, flop2Suit);
            }
        }
        else if (flop2Suit == hand1Suit)
        {
            swap(flop1Rank, flop2Rank);
            swap(flop1Suit, flop2Suit);
        }
    }
    else if (flop0Rank == flop1Rank)
    {
        if (flop1Suit == flop2Suit)
        {
            swap(flop0Rank, flop1Rank);
            swap(flop0Suit, flop1Suit);
        }
        if (flop0Suit != flop2Suit)
        {
            if (flop1Suit == hand0Suit)
            {
                swap(flop0Rank, flop1Rank);
                swap(flop0Suit, flop1Suit);
            }
            if (flop0Suit != hand0Suit && flop1Suit == hand1Suit)
            {
                swap(flop0Rank, flop1Rank);
                swap(flop0Suit, flop1Suit);
            }
        }
    }
    else if (flop2Rank == flop1Rank)
    {
        if (flop2Suit == flop0Suit)
        {
            swap(flop1Rank, flop2Rank);
            swap(flop1Suit, flop2Suit);
        }
        if (flop2Suit != flop0Suit)
        {
            if (flop2Suit == hand0Suit)
            {
                swap(flop1Rank, flop2Rank);
                swap(flop1Suit, flop2Suit);
            }
            if (flop1Suit != hand0Suit && flop2Suit == hand1Suit)
            {
                swap(flop1Rank, flop2Rank);
                swap(flop1Suit, flop2Suit);
            }
        }
    }

    // Now the suits are all in priority order.  We can change them to x,y,z,w now
    
    int nextNewSuit = (int)SUITS::X;

    ReplaceSuits(flop0Suit, flop1Suit, flop2Suit, hand0Suit, hand1Suit, flop0Suit, nextNewSuit);
    if (flop1Suit < (int)SUITS::NUM)
    {
        nextNewSuit++;
        ReplaceSuits(flop0Suit, flop1Suit, flop2Suit, hand0Suit, hand1Suit, flop1Suit, nextNewSuit);
    }
    if (flop2Suit < (int)SUITS::NUM)
    {
        nextNewSuit++;
        ReplaceSuits(flop0Suit, flop1Suit, flop2Suit, hand0Suit, hand1Suit, flop2Suit, nextNewSuit);
    }
    if (hand0Suit < (int)SUITS::NUM)
    {
        nextNewSuit++;
        ReplaceSuits(flop0Suit, flop1Suit, flop2Suit, hand0Suit, hand1Suit, hand0Suit, nextNewSuit);
    }
    if (hand1Suit < (int)SUITS::NUM)
    {
        nextNewSuit++;
        ReplaceSuits(flop0Suit, flop1Suit, flop2Suit, hand0Suit, hand1Suit, hand1Suit, nextNewSuit);
    }

    return
        CardToString(flop0Rank, flop0Suit) +
        CardToString(flop1Rank, flop1Suit) +
        CardToString(flop2Rank, flop2Suit) +
        CardToString(hand0Rank, hand0Suit) +
        CardToString(hand1Rank, hand1Suit) +
        history;
}

int CountHighCardRanksBelowHighest(int flop0, int flop1, int flop2, int hand0, int hand1)
{
    int flop0Rank = flop0 / (int)SUITS::NUM;
    int flop1Rank = flop1 / (int)SUITS::NUM;
    int flop2Rank = flop2 / (int)SUITS::NUM;
    int hand0Rank = hand0 / (int)SUITS::NUM;
    int hand1Rank = hand1 / (int)SUITS::NUM;
    
    int handHighCard = (hand0Rank > hand1Rank) ? hand0Rank : hand1Rank;
    
    int ranksUnderHighest = (int)RANKS::A - handHighCard;
    
    if (flop0Rank > handHighCard) ranksUnderHighest--;
    if (flop1Rank > handHighCard && flop1Rank != flop0Rank) ranksUnderHighest--;
    if (flop2Rank > handHighCard && flop2Rank != flop0Rank && flop2Rank != flop1Rank) ranksUnderHighest--;

    return ranksUnderHighest;
}

bool CheckForBluffCatcherOOP(int flop0, int flop1, int flop2, int hand0, int hand1)
{
    // K high or A high, unless the board has one
    return (CountHighCardRanksBelowHighest(flop0, flop1, flop2, hand0, hand1) <= 1);
}

void GetHandInfo(int flop0, int flop1, int flop2, int hand0, int hand1, int &holeCardsMatchingBoard, HAND_CATEGORY &handCategory, STRAIGHT_DRAW_CATEGORY &straightDrawCategory, FLUSH_DRAW_CATEGORY &flushDrawCategory, int &highCardRank)
{
    int flop0Rank = flop0 / (int)SUITS::NUM;
    int flop1Rank = flop1 / (int)SUITS::NUM;
    int flop2Rank = flop2 / (int)SUITS::NUM;
    int hand0Rank = hand0 / (int)SUITS::NUM;
    int hand1Rank = hand1 / (int)SUITS::NUM;

    holeCardsMatchingBoard = 0;
    if (hand0Rank == flop0Rank ||
        hand0Rank == flop1Rank ||
        hand0Rank == flop2Rank)
    {
        holeCardsMatchingBoard++;
    }
    if (hand1Rank == flop0Rank ||
        hand1Rank == flop1Rank ||
        hand1Rank == flop2Rank)
    {
        holeCardsMatchingBoard++;
    }
    
    bool pocketPair = (hand0Rank == hand1Rank);

    highCardRank = CountHighCardRanksBelowHighest(flop0, flop1, flop2, hand0, hand1);

    Hand hand = Hand::empty();
    hand += Hand(flop0) + Hand(flop1) + Hand(flop2) + Hand(hand0) + Hand(hand1);

    uint16_t handValue = handEvaluator.evaluate(hand);
    if (handValue >= STRAIGHT_FLUSH)
    {
        handCategory = HAND_CATEGORY::STRAIGHT_FLUSH;
    }
    else if (handValue >= FOUR_OF_A_KIND)
    {
        handCategory = HAND_CATEGORY::QUADS;
    }
    else if (handValue >= FULL_HOUSE)
    {
        handCategory = HAND_CATEGORY::BOAT;
    }
    else if (handValue >= FLUSH)
    {
        handCategory = HAND_CATEGORY::FLUSH;
    }
    else if (handValue >= STRAIGHT)
    {
        handCategory = HAND_CATEGORY::STRAIGHT;
    }
    else if (handValue >= THREE_OF_A_KIND)
    {
        if (holeCardsMatchingBoard == 2)
        {
            handCategory = HAND_CATEGORY::SET;
        }
        else if (holeCardsMatchingBoard == 1)
        {
            handCategory = HAND_CATEGORY::TRIPS;
        }
        else
        {
            handCategory = HAND_CATEGORY::BOARD_TRIPS;
        }
    }
    else if (handValue >= TWO_PAIR)
    {
        if (holeCardsMatchingBoard == 2)
        {
            handCategory = HAND_CATEGORY::TWO_PAIR_BOTH_IN_HOLE;
        }
        else if (holeCardsMatchingBoard == 1)
        {
            handCategory = HAND_CATEGORY::TWO_PAIR_ONE_ON_HOLE;
        }
        else
        {
            handCategory = HAND_CATEGORY::TWO_PAIR_WITH_POCKET_PAIR;
        }
    }
    else if (handValue >= PAIR)
    {
        if (pocketPair)
        {
            handCategory = HAND_CATEGORY::POCKET_PAIR;
        }
        else if (holeCardsMatchingBoard == 1)
        {
            handCategory = HAND_CATEGORY::PAIR_WITH_ONE_CARD_IN_HAND;
        }
        else if (holeCardsMatchingBoard == 0)
        {
            handCategory = HAND_CATEGORY::PAIR_ON_BOARD;
        }
    }
    else
    {
        handCategory = HAND_CATEGORY::HIGH_CARD;
    }

    straightDrawCategory = CheckForStraightDraws(flop0, flop1, flop2, hand0, hand1);
    flushDrawCategory = CheckForFlushDraws(flop0, flop1, flop2, hand0, hand1);
}

// slow play straight or higher or set
bool CheckForSlowPlay(int flop0, int flop1, int flop2, int hand0, int hand1, int holeCardsMatchingBoard, HAND_CATEGORY handCategory)
{
    int hand0Rank = hand0 / (int)SUITS::NUM;
    int hand1Rank = hand1 / (int)SUITS::NUM;

    if (holeCardsMatchingBoard == 2 && hand0Rank == hand1Rank)
    {
        return true;
    }
/*
    if (hand0Rank == hand1Rank)
    {
        // any pocket pair J or higher slow plays
        if (hand0Rank >= (int)RANKS::J)
        {
            return true;
        }
    }
*/
    if (handCategory >= HAND_CATEGORY::STRAIGHT)
    {
        return true;
    }

    return false;
}

ACTIONS GetOOPFixedStrategyActionFirstToAct(int deck[])
{
    const bool CAN_SLOW_PLAY = true;
    
    int flop0 = deck[0];
    int flop1 = deck[1];
    int flop2 = deck[2];
    int hand0 = deck[3];
    int hand1 = deck[4];
    
    int holeCardsMatchingBoard, highCardRank;
    HAND_CATEGORY handCategory;
    STRAIGHT_DRAW_CATEGORY straightDrawCategory;
    FLUSH_DRAW_CATEGORY flushDrawCategory;
    GetHandInfo(flop0, flop1, flop2, hand0, hand1, holeCardsMatchingBoard, handCategory, straightDrawCategory, flushDrawCategory, highCardRank);
    if (CAN_SLOW_PLAY && CheckForSlowPlay(flop0, flop1, flop2, hand0, hand1, holeCardsMatchingBoard, handCategory))
    {
        return ACTIONS::PASS;
    }
    
    int flop0Rank = flop0 / (int)SUITS::NUM;
    int flop1Rank = flop1 / (int)SUITS::NUM;
    int flop2Rank = flop2 / (int)SUITS::NUM;
    int hand0Rank = hand0 / (int)SUITS::NUM;
    int hand1Rank = hand1 / (int)SUITS::NUM;

    if (handCategory >= HAND_CATEGORY::STRAIGHT || holeCardsMatchingBoard > 0 || hand0Rank == hand1Rank)
    {
        return ACTIONS::BET;
    }
    
    // A high
    if (highCardRank <= 0)
    {
        return ACTIONS::BET;
    }

    if (CheckForBluffCatcherOOP(flop0, flop1, flop2, hand0, hand1))
    {
        return ACTIONS::PASS;
    }
    
    if (straightDrawCategory != STRAIGHT_DRAW_CATEGORY::NONE ||
        flushDrawCategory == FLUSH_DRAW_CATEGORY::FRONT_DOOR_TWO_CARD ||
        flushDrawCategory == FLUSH_DRAW_CATEGORY::BACK_DOOR_TWO_CARD)
    {
        return ACTIONS::BET;
    }
    
    int lowestRank0 = 0;
    int lowestRank1 = 1;
    int lowestRank2 = 2;
    int lowestRank3 = 3;
    while (lowestRank0 == flop0Rank ||
           lowestRank0 == flop1Rank ||
           lowestRank0 == flop2Rank)
    {
        lowestRank0++;
        lowestRank1++;
        lowestRank2++;
        lowestRank3++;
    }
    while (lowestRank1 == flop0Rank ||
           lowestRank1 == flop1Rank ||
           lowestRank1 == flop2Rank)
    {
        lowestRank1++;
        lowestRank2++;
        lowestRank3++;
    }
    while (lowestRank2 == flop0Rank ||
           lowestRank2 == flop1Rank ||
           lowestRank2 == flop2Rank)
    {
        lowestRank2++;
        lowestRank3++;
    }
    while (lowestRank3 == flop0Rank ||
           lowestRank3 == flop1Rank ||
           lowestRank3 == flop2Rank)
    {
        lowestRank3++;
    }
/*
    // any two low cards
    if ((hand0Rank == lowestRank0 ||
        hand0Rank == lowestRank1 ||
        hand0Rank == lowestRank2 ||
        hand0Rank == lowestRank3) &&
        (hand1Rank == lowestRank0 ||
         hand1Rank == lowestRank1 ||
         hand1Rank == lowestRank2 ||
         hand1Rank == lowestRank3))
    {
        return ACTIONS::BET;
    }
*/
    return ACTIONS::PASS;
}

ACTIONS GetOOPFixedStrategyActionWhenBetTo(int deck[])
{
    if (CheckForBluffCatcherOOP(deck[0], deck[1], deck[2], deck[3], deck[4]))
    {
        return ACTIONS::BET;
    }

    int holeCardsMatchingBoard, highCardRank;
    HAND_CATEGORY handCategory;
    STRAIGHT_DRAW_CATEGORY straightDrawCategory;
    FLUSH_DRAW_CATEGORY flushDrawCategory;
    GetHandInfo(deck[0], deck[1], deck[2], deck[3], deck[4], holeCardsMatchingBoard, handCategory, straightDrawCategory, flushDrawCategory, highCardRank);
    if (handCategory >= HAND_CATEGORY::STRAIGHT || holeCardsMatchingBoard > 0)
    {
        return ACTIONS::BET;
    }
    return ACTIONS::PASS;
}

ACTIONS GetIPFixedStrategyActionWhenCheckedTo(int deck[])
{
    int flop0 = deck[0];
    int flop1 = deck[1];
    int flop2 = deck[2];
    int hand0 = deck[5];
    int hand1 = deck[6];
    int flop0Rank = flop0 / (int)SUITS::NUM;
    int flop1Rank = flop1 / (int)SUITS::NUM;
    int flop2Rank = flop2 / (int)SUITS::NUM;
    int hand0Rank = hand0 / (int)SUITS::NUM;
    int hand1Rank = hand1 / (int)SUITS::NUM;

    int holeCardsMatchingBoard, highCardRank;
    HAND_CATEGORY handCategory;
    STRAIGHT_DRAW_CATEGORY straightDrawCategory;
    FLUSH_DRAW_CATEGORY flushDrawCategory;
    GetHandInfo(flop0, flop1, flop2, hand0, hand1, holeCardsMatchingBoard, handCategory, straightDrawCategory, flushDrawCategory, highCardRank);

    if (handCategory >= HAND_CATEGORY::STRAIGHT || holeCardsMatchingBoard > 0 || hand0Rank == hand1Rank)
    {
        return ACTIONS::BET;
    }

    // A high or K high
    if (highCardRank <= 1)
    {
        return ACTIONS::BET;
    }
    
    if (straightDrawCategory != STRAIGHT_DRAW_CATEGORY::NONE ||
        flushDrawCategory == FLUSH_DRAW_CATEGORY::FRONT_DOOR_TWO_CARD ||
        flushDrawCategory == FLUSH_DRAW_CATEGORY::FRONT_DOOR_ONE_CARD)
//        ||
//        flushDrawCategory == FLUSH_DRAW_CATEGORY::BACK_DOOR_TWO_CARD)
        // optimally, also bet with multiple backdoors
    {
        return ACTIONS::BET;
    }

    int lowestRank0 = 0;
    int lowestRank1 = 1;
    int lowestRank2 = 2;
    int lowestRank3 = 3;
    while (lowestRank0 == flop0Rank ||
           lowestRank0 == flop1Rank ||
           lowestRank0 == flop2Rank)
    {
        lowestRank0++;
        lowestRank1++;
        lowestRank2++;
        lowestRank3++;
    }
    while (lowestRank1 == flop0Rank ||
           lowestRank1 == flop1Rank ||
           lowestRank1 == flop2Rank)
    {
        lowestRank1++;
        lowestRank2++;
        lowestRank3++;
    }
    while (lowestRank2 == flop0Rank ||
           lowestRank2 == flop1Rank ||
           lowestRank2 == flop2Rank)
    {
        lowestRank2++;
        lowestRank3++;
    }
    while (lowestRank3 == flop0Rank ||
           lowestRank3 == flop1Rank ||
           lowestRank3 == flop2Rank)
    {
        lowestRank3++;
    }

    // any two low cards
    if ((hand0Rank == lowestRank0 ||
        hand0Rank == lowestRank1 ||
        hand0Rank == lowestRank2 ||
        hand0Rank == lowestRank3) &&
        (hand1Rank == lowestRank0 ||
         hand1Rank == lowestRank1 ||
         hand1Rank == lowestRank2 ||
         hand1Rank == lowestRank3))
    {
        return ACTIONS::BET;
    }

    return ACTIONS::PASS;
}

ACTIONS GetIPFixedStrategyActionWhenBetTo(int deck[])
{
    int flop0 = deck[0];
    int flop1 = deck[1];
    int flop2 = deck[2];
    int hand0 = deck[5];
    int hand1 = deck[6];
    int hand0Rank = hand0 / (int)SUITS::NUM;
    int hand1Rank = hand1 / (int)SUITS::NUM;

    int holeCardsMatchingBoard, highCardRank;
    HAND_CATEGORY handCategory;
    STRAIGHT_DRAW_CATEGORY straightDrawCategory;
    FLUSH_DRAW_CATEGORY flushDrawCategory;
    GetHandInfo(flop0, flop1, flop2, hand0, hand1, holeCardsMatchingBoard, handCategory, straightDrawCategory, flushDrawCategory, highCardRank);
    
    if (handCategory >= HAND_CATEGORY::STRAIGHT || holeCardsMatchingBoard > 0 || hand0Rank == hand1Rank)
    {
        return ACTIONS::BET;
    }
    
    if (straightDrawCategory == STRAIGHT_DRAW_CATEGORY::OPEN_ENDED_TWO_CARD ||
        straightDrawCategory == STRAIGHT_DRAW_CATEGORY::OPEN_ENDED_ONE_CARD ||
        straightDrawCategory == STRAIGHT_DRAW_CATEGORY::DOUBLE_GUTSHOT_TWO_CARD ||
        straightDrawCategory == STRAIGHT_DRAW_CATEGORY::DOUBLE_GUTSHOT_ONE_CARD ||
        flushDrawCategory == FLUSH_DRAW_CATEGORY::FRONT_DOOR_TWO_CARD ||
        flushDrawCategory == FLUSH_DRAW_CATEGORY::FRONT_DOOR_ONE_CARD)
    {
        return ACTIONS::BET;
    }

    // A high or K high
    // really, it's better to check if the other card is an over to bottom
    // something like A and K high, but not if the other card is an undercard
    if (highCardRank <= 1)
    {
        return ACTIONS::BET;
    }
    
    return ACTIONS::PASS;
}

char PokerCompareCharHelper(char c)
{
    if (c == 'T') c = 'Z' + 1;
    if (c == 'J') c = 'Z' + 2;
    if (c == 'Q') c = 'Z' + 3;
    if (c == 'K') c = 'Z' + 4;
    if (c == 'A') c = 'Z' + 5;
    if (c == 'w') c = 'z' + 1;
    return c;
}

bool PokerCompareChar(char a, char b)
{
    a = PokerCompareCharHelper(a);
    b = PokerCompareCharHelper(b);
    return a < b;
}

struct pokerStringCompare {
    bool operator()(const std::string& a, const std::string& b) const
    {
        return std::lexicographical_compare(a.begin(), a.end(),
                                            b.begin(), b.end(), PokerCompareChar);
    }
};

class Solver
{
    unordered_map<string, Node> nodes;
    double estimatedEV = 0;
    
public:

    int NUM_ITERATIONS = 5000000;
    int NUM_ITERATIONS_FOR_EV_CALCULATION = 2000000;
    int BET_AMOUNT = 2;
    bool HARD_CODE_FLOP = true;

    int HARD_CODE_FLOP0 = (int)RANKS::THREE * (int)SUITS::NUM + (int)SUITS::HEARTS;
    int HARD_CODE_FLOP1 = (int)RANKS::NINE * (int)SUITS::NUM + (int)SUITS::CLUBS;
    int HARD_CODE_FLOP2 = (int)RANKS::J * (int)SUITS::NUM + (int)SUITS::CLUBS;
    
    bool INSERT_FIXED_STRATEGY_NODES = true;
    bool OOP_USES_FIXED_STRATEGY_FIRST_TO_ACT = false;
    bool OOP_USES_HARD_CODED_HANDS_STRATEGY_FIRST_TO_ACT = false;
    bool OOP_USES_FIXED_STRATEGY_WHEN_BET_TO = false;

    bool IP_USES_FIXED_STRATEGY_WHEN_CHECKED_TO = true;
    bool IP_USES_FIXED_STRATEGY_WHEN_BET_TO = true;

    double CFR(int deck[], string history, double probability0, double probability1, bool justCalculateEv)
    {
        int plays = (int)history.length();
        int player = plays % 2;
//        int opponent = 1 - player;
        
        // check if terminal node
        if (plays > 1) {
            int winner = CheckShowdown(deck);
            int player0Winnings = player == 0 ? winner : -winner;
            if (history == "pp")
            {
                return player0Winnings;
            }
            else if (history == "bp" || history == "pbp")
            {
                return 1;
            }
            else if (history == "bb" || history == "pbb")
            {
                return player0Winnings * (BET_AMOUNT + 1);
            }
        }
        
        ACTIONS fixedStrategyAction = ACTIONS::NUM;
        
        if (OOP_USES_FIXED_STRATEGY_FIRST_TO_ACT && player == 0 && plays == 0)
        {
            fixedStrategyAction = GetOOPFixedStrategyActionFirstToAct(deck);
        }
        else if (OOP_USES_HARD_CODED_HANDS_STRATEGY_FIRST_TO_ACT && player == 0 && plays == 0)
        {
            string infoSet = ConstructInfoSet(deck[0], deck[1], deck[2], deck[3 + player * 2], deck[4 + player * 2], history);
            string hand = infoSet.substr(6, 4);

            fixedStrategyAction = hardCodedHands.contains(hand) ?
                fixedStrategyAction = ACTIONS::BET :
                ACTIONS::PASS;
        }
        else if (OOP_USES_FIXED_STRATEGY_WHEN_BET_TO && player == 0 && plays == 2)
        {
            fixedStrategyAction = GetOOPFixedStrategyActionWhenBetTo(deck);
        }
        else if (IP_USES_FIXED_STRATEGY_WHEN_CHECKED_TO && player == 1 && history == "p")
        {
            fixedStrategyAction = GetIPFixedStrategyActionWhenCheckedTo(deck);
        }
        else if (IP_USES_FIXED_STRATEGY_WHEN_BET_TO && player == 1 && history == "b")
        {
            fixedStrategyAction = GetIPFixedStrategyActionWhenBetTo(deck);
        }
        
        if (fixedStrategyAction != ACTIONS::NUM)
        {
            string nextHistory = history + (fixedStrategyAction == ACTIONS::PASS ? "p" : "b");
            double ev = - CFR(deck, nextHistory, probability0, probability1, justCalculateEv);

            if (INSERT_FIXED_STRATEGY_NODES)
            {
                string infoSet = ConstructInfoSet(deck[0], deck[1], deck[2], deck[3 + player * 2], deck[4 + player * 2], history);
                
                auto iter = nodes.find(infoSet);
                if (iter == nodes.end())
                {
                    nodes.insert({infoSet, Node(infoSet, fixedStrategyAction)});
                }
            }

            return ev;
        }
        else
        {
            // add this decision-point node to the map
            
            string infoSet = ConstructInfoSet(deck[0], deck[1], deck[2], deck[3 + player * 2], deck[4 + player * 2], history);
            
            Node *nodePointer;
            
            auto iter = nodes.find(infoSet);
            if (iter == nodes.end())
            {
                auto inserted = nodes.insert({infoSet, Node(infoSet)});
                nodePointer = &(inserted.first->second);
            }
            else
            {
                nodePointer = &(iter->second);
            }
            
            double strategy[(int)ACTIONS::NUM];
            nodePointer->GetStrategy(strategy);
            if (!justCalculateEv)
            {
                nodePointer->UpdateStrategySums(strategy, player == 0 ? probability0 : probability1);
            }
            
            // traverse to the next nodes and calculate ev
            double ev[(int)ACTIONS::NUM];
            double nodeEV = 0;
            for (int a = 0; a < (int)ACTIONS::NUM; a++)
            {
                string nextHistory = history + (a == (int)ACTIONS::PASS ? "p" : "b");
                double nextNodeProbability0 = (player == 0) ? probability0 * strategy[a] : probability0;
                double nextNodeProbability1 = (player == 1) ? probability1 * strategy[a] : probability1;
                // ev is inverted here because the next node's ev is from the perspective of the opponent
                ev[a] = - CFR(deck, nextHistory, nextNodeProbability0, nextNodeProbability1, justCalculateEv);
                nodeEV += strategy[a] * ev[a];
            }
            
            if (!justCalculateEv)
            {
                double probabilityWeGetHere = player == 0 ? probability1 : probability0;
                for (int a = 0; a < (int)ACTIONS::NUM; a++)
                {
                    double regret = ev[a] - nodeEV;
                    nodePointer->regretSums[a] += probabilityWeGetHere * regret;
                    
                    //cfr+   this helps it resolve faster
                    nodePointer->regretSums[a] = max(0.0, nodePointer->regretSums[a]);
                }
            }
            return nodeEV;
        }
    }
    
    void ShuffleDeck(int deck[])
    {
        for (int i = 0; i < NUM_CARDS; i++)
        {
            deck[i] = i;
        }
        int cardsToReserve = 0;
        if (HARD_CODE_FLOP)
        {
            swap(deck[HARD_CODE_FLOP0], deck[0]);
            swap(deck[HARD_CODE_FLOP1], deck[1]);
            swap(deck[HARD_CODE_FLOP2], deck[2]);
            cardsToReserve += 3;
        }
        for (int c1 = NUM_CARDS - 1; c1 > cardsToReserve; c1--)
        {
            int c2 = rand() % (c1 + 1 - cardsToReserve) + cardsToReserve;
            swap(deck[c1], deck[c2]);
        }
    }
    
    STRATEGY_ACTIONS GetStrategyActionFromNode(string key, const Node &node, double &percentage, double &secondaryPercentage) const
    {
        percentage = -1;
        secondaryPercentage = -1;
        
        string flop = key.substr(0, 7);
        string hand = key.substr(7, 5);
        string history = key.substr(12);
        
        double strategy[(int)ACTIONS::NUM];
        node.GetAverageStrategy(strategy);
        percentage = strategy[(int)ACTIONS::BET];
        if (history == "")
        {
            if (strategy[(int)ACTIONS::BET] >= 0.5)
            {
                return STRATEGY_ACTIONS::OOP_BET;
            }
            else
            {
                string secondaryKey = node.infoSet + "pb";
                auto iter = nodes.find(secondaryKey);
                if (iter != nodes.end())
                {
                    const Node &node2 = iter->second;
                    double strategy2[(int)ACTIONS::NUM];
                    node2.GetAverageStrategy(strategy2);

                    secondaryPercentage = strategy2[(int)ACTIONS::BET];

                    if (strategy2[(int)ACTIONS::BET] >= 0.5)
                    {
                        return STRATEGY_ACTIONS::OOP_CHECK_CALL;
                    }
                    else
                    {
                        return STRATEGY_ACTIONS::OOP_CHECK_FOLD;
                    }
                }
                cout << "error: missing node\n";
                return STRATEGY_ACTIONS::OOP_CHECK_FOLD;
            }
        }
        else if (history == "p")
        {
            if (strategy[(int)ACTIONS::BET] >= 0.5)
            {
                return STRATEGY_ACTIONS::IP_BET;
            }
            else
            {
                return STRATEGY_ACTIONS::IP_CHECK_BACK;
            }
        }
        else if (history == "b")
        {
            if (strategy[(int)ACTIONS::BET] >= 0.5)
            {
                return STRATEGY_ACTIONS::IP_CALL;
            }
            else
            {
                return STRATEGY_ACTIONS::IP_FOLD;
            }
        }
        return STRATEGY_ACTIONS::NOT_A_ROOT_NODE;
    }
    
    void PrintActions(const map<string, Node, pokerStringCompare> &sortedNodes,
                      const Solver *solverGTO)
    {
        bool needComma = false;
        for (const auto & [ key, node ] : sortedNodes)
        {
            double percentage, secondaryPercentage;
            STRATEGY_ACTIONS strategyAction = GetStrategyActionFromNode(key, node, percentage, secondaryPercentage);

            STRATEGY_ACTIONS strategyActionGTO = STRATEGY_ACTIONS::NUM;
            double percentageGTO = -1;
            double secondaryPercentageGTO = -1;
            if (solverGTO)
            {
                auto iter = solverGTO->nodes.find(node.infoSet);
                if (iter != solverGTO->nodes.end())
                {
                    const Node &nodeGTO = iter->second;
                    strategyActionGTO = solverGTO->GetStrategyActionFromNode(key, nodeGTO, percentageGTO, secondaryPercentageGTO);
                }
            }

            if (needComma)
            {
                std::cout << ",\n";
            }
            string hand = key.substr(7, 5);
            if (secondaryPercentage >= 0)
            {
                if (strategyActionGTO == STRATEGY_ACTIONS::NUM)
                {
                    if (solverGTO)
                    {
                        std::cout << "\"" <<
                            hand << " " <<
                            std::round(percentage * 1000.0) / 1000.0 << " " <<
                            std::round(secondaryPercentage * 1000.0) / 1000.0 <<
                            " GTO node not found\"";
                    }
                    else
                    {
                        std::cout << "\"" <<
                            hand << " " <<
                            std::round(percentage * 1000.0) / 1000.0 << " " <<
                            std::round(secondaryPercentage * 1000.0) / 1000.0 <<
                            "\"";
                    }
                }
                else
                {
                    if (strategyAction == strategyActionGTO)
                    {
                        std::cout << "\"" <<
                            hand << " " <<
                            std::round(percentage * 1000.0) / 1000.0 << " " <<
                            std::round(secondaryPercentage * 1000.0) / 1000.0 <<
                            "\"";
                    }
                    else
                    {
                        std::cout << "\"" <<
                            hand << " " <<
                            std::round(percentage * 1000.0) / 1000.0 << " " <<
                            std::round(secondaryPercentage * 1000.0) / 1000.0 <<
                            " deviation from " <<
                            strategyActionStrings[(int)strategyActionGTO] << " " <<
                            std::round(percentageGTO * 1000.0) / 1000.0 << " " <<
                            std::round(secondaryPercentageGTO * 1000.0) / 1000.0 <<
                            "\"";
                    }
                }
            }
            else
            {
                if (strategyActionGTO == STRATEGY_ACTIONS::NUM)
                {
                    if (solverGTO)
                    {
                        std::cout << "\"" <<
                            hand << " " <<
                            std::round(percentage * 1000.0) / 1000.0 <<
                            " GTO node not found\"";
                    }
                    else
                    {
                        std::cout << "\"" <<
                            hand << " " <<
                            std::round(percentage * 1000.0) / 1000.0 <<
                            "\"";
                    }
                }
                else
                {
                    if (strategyAction == strategyActionGTO)
                    {
                        std::cout << "\"" <<
                            hand << " " <<
                            std::round(percentage * 1000.0) / 1000.0 <<
                            "\"";
                    }
                    else
                    {
                        std::cout << "\"" <<
                            hand << " " <<
                            std::round(percentage * 1000.0) / 1000.0 <<
                            " deviation from " <<
                            strategyActionStrings[(int)strategyActionGTO] << " " <<
                            std::round(percentageGTO * 1000.0) / 1000.0 << " " <<
                            std::round(secondaryPercentageGTO * 1000.0) / 1000.0 <<
                            "\"";
                    }
                }
            }
            needComma = true;
        }
    }
    
    void Print(const Solver *solverGTO)
    {
        map<string, Node, pokerStringCompare> sortedNodes;
        map<STRATEGY_ACTIONS, map<string, map<string, Node, pokerStringCompare> > > nodesPerSpot;
        
        for (const auto & [ key, node ] : nodes)
        {
            string flop = node.infoSet.substr(0, 6);
            string hand = node.infoSet.substr(6, 4);
            string history = node.infoSet.substr(10);
            
            string humanReadableFlop;
            humanReadableFlop += flop[0];
            humanReadableFlop += flop[2];
            humanReadableFlop += flop[4];
            humanReadableFlop += " ";
            humanReadableFlop += flop[1];
            humanReadableFlop += flop[3];
            humanReadableFlop += flop[5];

            string humanReadableHand;
            humanReadableHand += hand[0];
            humanReadableHand += hand[2];
            humanReadableHand += " ";
            humanReadableHand += hand[1];
            humanReadableHand += hand[3];
            
            string humanReadableInfoSet = humanReadableFlop + humanReadableHand + history;

            sortedNodes.insert({humanReadableInfoSet, node});
        }
        
        for (const auto & [ key, node ] : sortedNodes)
        {
            double percentage, secondaryPercentage;
            STRATEGY_ACTIONS strategyAction = GetStrategyActionFromNode(key, node, percentage, secondaryPercentage);
            
            int flop0 = StringToCard(node.infoSet.substr(0,2));
            int flop1 = StringToCard(node.infoSet.substr(2,2));
            int flop2 = StringToCard(node.infoSet.substr(4,2));
            int hand0 = StringToCard(node.infoSet.substr(6,2));
            int hand1 = StringToCard(node.infoSet.substr(8,2));
            int holeCardsMatchingBoard, highCardRank;
            HAND_CATEGORY handCategory;
            STRAIGHT_DRAW_CATEGORY straightDrawCategory;
            FLUSH_DRAW_CATEGORY flushDrawCategory;
            GetHandInfo(flop0, flop1, flop2, hand0, hand1, holeCardsMatchingBoard, handCategory, straightDrawCategory, flushDrawCategory, highCardRank);
            string handCategoryString = CategoriesToString(handCategory, straightDrawCategory, flushDrawCategory);
            nodesPerSpot[strategyAction][handCategoryString].insert({key, node});
        }
        
        std::cout << "\n{\n\"Iterations\": " <<
            NUM_ITERATIONS <<
            ",\n\"Total ev\": " <<
            estimatedEV <<
            ",\n\"GTO ev\": " <<
            solverGTO->estimatedEV <<
            ",\n";

        string firstInfoSet = sortedNodes.begin()->first;
        string flop = firstInfoSet.substr(0, 7);
        std::cout << "\"flop\": \"" << flop << "\",\n\n";

        std::cout << "\"actions\":{";
        bool needsCommaOuter = false;
        for (int action = 0; action < (int)STRATEGY_ACTIONS::NOT_A_ROOT_NODE; action++)
        {
            if (needsCommaOuter)
            {
                std::cout << ",\n";
            }
            std::cout << "\n\n\"" << strategyActionStrings[(int)action] << "\": {\n\n";

            auto nodesPerHandCategoryString = nodesPerSpot[(STRATEGY_ACTIONS)action];
            bool needsComma = false;
            for (const auto & [ handCategoryString, nodesForThisHandCategory ] : nodesPerHandCategoryString)
            {
                if (needsComma)
                {
                    std::cout << ",\n";
                }
                std::cout << "\"" << handCategoryString << "\":[\n";
                
                PrintActions(nodesForThisHandCategory, solverGTO);
                std::cout << "\n]";
                needsComma = true;
            }
            std::cout << "\n}\n";
            needsCommaOuter = true;
        }
        std::cout << "}\n\n}\n";
    }
    
    void Solve()
    {
        int deck[NUM_CARDS];
        cout << "--------------------\n";
        for (int iteration = 0; iteration < NUM_ITERATIONS; iteration++)
        {
            if (iteration % (NUM_ITERATIONS/20) == 0)
            {
                cout << ".";
            }
            
            ShuffleDeck(deck);
            CFR(deck, "", 1, 1, false);
        }
        cout << "\n";
    }
    void NormalizeStrategy()
    {
        for (auto & [ key, node ] : nodes)
        {
            if (node.strategySums[1] > node.strategySums[0])
            {
                node.strategySums[1] = 100;
                node.strategySums[0] = 0;
                node.regretSums[1] = 100;
                node.regretSums[0] = 0;
            }
            else
            {
                node.strategySums[1] = 0;
                node.strategySums[0] = 100;
                node.regretSums[1] = 1;
                node.regretSums[0] = 100;
            }
        }
    }
    
    void estimateEV()
    {
        int deck[NUM_CARDS];
        estimatedEV = 0;

        for (int iteration = 0; iteration < NUM_ITERATIONS_FOR_EV_CALCULATION; iteration++)
        {
            if (iteration % (NUM_ITERATIONS_FOR_EV_CALCULATION/20) == 0)
            {
                cout << ".";
            }
            
            ShuffleDeck(deck);
            estimatedEV += CFR(deck, "", 1, 1, true);
        }
        cout << "\n";
        estimatedEV /= NUM_ITERATIONS_FOR_EV_CALCULATION;
    }
};

int main(int argc, const char * argv[]) {
    srand((unsigned int)time(0));
    
    
    Solver solverGTO;
    solverGTO.NUM_ITERATIONS = 10000000;
    solverGTO.HARD_CODE_FLOP0 = (int)RANKS::FOUR * (int)SUITS::NUM + (int)SUITS::DIAMONDS;
    solverGTO.HARD_CODE_FLOP1 = (int)RANKS::J * (int)SUITS::NUM + (int)SUITS::SPADES;
    solverGTO.HARD_CODE_FLOP2 = (int)RANKS::A * (int)SUITS::NUM + (int)SUITS::HEARTS;
    solverGTO.IP_USES_FIXED_STRATEGY_WHEN_CHECKED_TO = false;
    solverGTO.IP_USES_FIXED_STRATEGY_WHEN_BET_TO = false;
    solverGTO.Solve();
    solverGTO.estimateEV();

    

    Solver solver;
    solver.NUM_ITERATIONS = 10000000;
    solver.HARD_CODE_FLOP0 = (int)RANKS::FOUR * (int)SUITS::NUM + (int)SUITS::DIAMONDS;
    solver.HARD_CODE_FLOP1 = (int)RANKS::J * (int)SUITS::NUM + (int)SUITS::SPADES;
    solver.HARD_CODE_FLOP2 = (int)RANKS::A * (int)SUITS::NUM + (int)SUITS::HEARTS;
    solver.IP_USES_FIXED_STRATEGY_WHEN_CHECKED_TO = true;
    solver.IP_USES_FIXED_STRATEGY_WHEN_BET_TO = true;
    solver.Solve();

    solver.NormalizeStrategy();

    solver.estimateEV();
    solver.Print(&solverGTO);

    return 0;
}


