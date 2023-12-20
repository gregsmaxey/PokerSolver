#include <iostream>
#include <unordered_map>
#include <map>

using namespace std;

const int NUM_ITERATIONS = 1000000;
const int BET_AMOUNT = 2;
const bool HARD_CODE_FLOP = true;
const bool HARD_CODE_HAND = false;
const int HARD_CODE_FLOP0 = 48;
const int HARD_CODE_FLOP1 = 49;
const int HARD_CODE_FLOP2 = 50;
const int HARD_CODE_HAND0 = 51;
const int HARD_CODE_HAND1 = 20;
const bool DEALER_USES_FIXED_STRATEGY = true;
const bool PRINT_SOME = true;
const string HISTORY_TO_PRINT = "p";

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

class Node
{
public:
    double regretSums[(int)ACTIONS::NUM];
    double strategySums[(int)ACTIONS::NUM];
    string infoSet;
    
    Node(string _infoSet)
    {
        infoSet = _infoSet;
        for (int i = 0; i < (int)ACTIONS::NUM; i++)
        {
            regretSums[i] = 0;
            strategySums[i] = 0;
        }
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
        for (int i = 0; i < (int)ACTIONS::NUM; i++)
        {
            strategySums[i] += realizationWeight * strategy[i];
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
    
    void PrintSome() const
    {
        string flop = infoSet.substr(0, 6);
        string hand = infoSet.substr(6, 4);
        string history = infoSet.substr(10);
        
        if (history == HISTORY_TO_PRINT)
        {
            double strategy[(int)ACTIONS::NUM];
            GetAverageStrategy(strategy);
            std::cout << hand << " " << std::round(strategy[(int)ACTIONS::BET] * 1000.0) / 1000.0 << "\n";
        }
    }
};

void SortHand(int _playerHand[])
{
    int playerHand[5];
    playerHand[0] = _playerHand[0];
    playerHand[1] = _playerHand[1];
    playerHand[2] = _playerHand[2];
    playerHand[3] = _playerHand[3];
    playerHand[4] = _playerHand[4];
    
    if (playerHand[0] > playerHand[1])
        swap(playerHand[0], playerHand[1]);
    if (playerHand[2] > playerHand[3])
        swap(playerHand[2], playerHand[3]);
    
    if (playerHand[0] > playerHand[2])
    {
        swap(playerHand[0], playerHand[2]);
        swap(playerHand[1], playerHand[3]);
    }
    
    if (playerHand[4] > playerHand[2])
    {
        if (playerHand[4] > playerHand[3])  // playerHand[0] playerHand[2] playerHand[3] playerHand[4]
        {
            if (playerHand[1] > playerHand[3])
            {
                if (playerHand[1] > playerHand[4])
                {
                    _playerHand[0] = playerHand[0];
                    _playerHand[1] = playerHand[2];
                    _playerHand[2] = playerHand[3];
                    _playerHand[3] = playerHand[4];
                    _playerHand[4] = playerHand[1];
                }
                else
                {
                    _playerHand[0] = playerHand[0];
                    _playerHand[1] = playerHand[2];
                    _playerHand[2] = playerHand[3];
                    _playerHand[3] = playerHand[1];
                    _playerHand[4] = playerHand[4];
                }
            }
            else
            {
                if (playerHand[1] < playerHand[2])
                {
                    _playerHand[0] = playerHand[0];
                    _playerHand[1] = playerHand[1];
                    _playerHand[2] = playerHand[2];
                    _playerHand[3] = playerHand[3];
                    _playerHand[4] = playerHand[4];
                }
                else
                {
                    _playerHand[0] = playerHand[0];
                    _playerHand[1] = playerHand[2];
                    _playerHand[2] = playerHand[1];
                    _playerHand[3] = playerHand[3];
                    _playerHand[4] = playerHand[4];
                }
            }
        }
        else  // playerHand[0] playerHand[2] playerHand[4] playerHand[3]
        {
            if (playerHand[1] > playerHand[4])
            {
                if (playerHand[1] > playerHand[3])
                {
                    _playerHand[0] = playerHand[0];
                    _playerHand[1] = playerHand[2];
                    _playerHand[2] = playerHand[4];
                    _playerHand[3] = playerHand[3];
                    _playerHand[4] = playerHand[1];
                }
                else
                {
                    _playerHand[0] = playerHand[0];
                    _playerHand[1] = playerHand[2];
                    _playerHand[2] = playerHand[4];
                    _playerHand[3] = playerHand[1];
                    _playerHand[4] = playerHand[3];
                }
            }
            else
            {
                if (playerHand[1] < playerHand[2])
                {
                    _playerHand[0] = playerHand[0];
                    _playerHand[1] = playerHand[1];
                    _playerHand[2] = playerHand[2];
                    _playerHand[3] = playerHand[4];
                    _playerHand[4] = playerHand[3];
                }
                else
                {
                    _playerHand[0] = playerHand[0];
                    _playerHand[1] = playerHand[2];
                    _playerHand[2] = playerHand[1];
                    _playerHand[3] = playerHand[4];
                    _playerHand[4] = playerHand[3];
                }
            }
        }
    }
    else
    {
        if (playerHand[4] < playerHand[0])  // playerHand[4] playerHand[0] playerHand[2] playerHand[3]
        {
            if (playerHand[1] > playerHand[2])
            {
                if (playerHand[1] > playerHand[3])
                {
                    _playerHand[0] = playerHand[4];
                    _playerHand[1] = playerHand[0];
                    _playerHand[2] = playerHand[2];
                    _playerHand[3] = playerHand[3];
                    _playerHand[4] = playerHand[1];
                }
                else
                {
                    _playerHand[0] = playerHand[4];
                    _playerHand[1] = playerHand[0];
                    _playerHand[2] = playerHand[2];
                    _playerHand[3] = playerHand[1];
                    _playerHand[4] = playerHand[3];
                }
            }
            else
            {
                _playerHand[0] = playerHand[4];
                _playerHand[1] = playerHand[0];
                _playerHand[2] = playerHand[1];
                _playerHand[3] = playerHand[2];
                _playerHand[4] = playerHand[3];
            }
        }
        else  // playerHand[0] playerHand[4] playerHand[2] playerHand[3]
        {
            if (playerHand[1] > playerHand[2])
            {
                if (playerHand[1] > playerHand[3])
                {
                    _playerHand[0] = playerHand[0];
                    _playerHand[1] = playerHand[4];
                    _playerHand[2] = playerHand[2];
                    _playerHand[3] = playerHand[3];
                    _playerHand[4] = playerHand[1];
                }
                else
                {
                    _playerHand[0] = playerHand[0];
                    _playerHand[1] = playerHand[4];
                    _playerHand[2] = playerHand[2];
                    _playerHand[3] = playerHand[1];
                    _playerHand[4] = playerHand[3];
                }
            }
            else
            {
                if (playerHand[1] < playerHand[4])
                {
                    _playerHand[0] = playerHand[0];
                    _playerHand[1] = playerHand[1];
                    _playerHand[2] = playerHand[4];
                    _playerHand[3] = playerHand[2];
                    _playerHand[4] = playerHand[3];
                }
                else
                {
                    _playerHand[0] = playerHand[0];
                    _playerHand[1] = playerHand[4];
                    _playerHand[2] = playerHand[1];
                    _playerHand[3] = playerHand[2];
                    _playerHand[4] = playerHand[3];
                }
            }
        }
    }
}

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

string CardToString(int card)
{
    int rank = card / (int)SUITS::NUM;
    int suit = card % (int)SUITS::NUM;
    string c = RankToString(rank);
    string s = SuitToString(suit);
    return c + s;
}

string CardToString(int rank, int suit)
{
    string c = RankToString(rank);
    string s = SuitToString(suit);
    return c + s;
}

enum class HAND_VALUE_TYPE
{
    HIGH_CARD = 0,
    PAIR,
    TWO_PAIR,
    THREE_OF_A_KIND,
    STRAIGHT,
    FLUSH,
    FULL_HOUSE,
    FOUR_OF_A_KIND,
    STRAIGHT_FLUSH,
};

class HandValue
{
public:
    HAND_VALUE_TYPE handValueType;
    int rank;   // the high card of a straight, the rank of a pair, trips, or quads, the top rank of a two-pair, the trips in a full house.
    int secondaryRank;      // the bottom pair in a two-pair.  the pair in a full house.
    int kickerValue;  // constructed from the ranks of all five cards
    
    bool operator==(const HandValue& other) {
        return handValueType == other.handValueType &&
        rank == other.rank &&
        secondaryRank == other.secondaryRank &&
        kickerValue == other.kickerValue;
    }
    
    HandValue(int playerHand[])
    {
        SortHand(playerHand);
        
        int ranks[5];
        int suits[5];
        for (int i = 0; i < 5; i++)
        {
            // 0 refers to "2".  12 refers to "A"
            ranks[i] = playerHand[i] / (int)SUITS::NUM;
            suits[i] = playerHand[i] % (int)SUITS::NUM;
        }
        
        rank = 0;
        secondaryRank = 0;
        kickerValue =
        playerHand[0] +
        (playerHand[1] << 4) +
        (playerHand[2] << 8) +
        (playerHand[3] << 12) +
        (playerHand[4] << 16);
        
        bool hasFlush = false;
        bool hasStraight = false;
        int straightRank = 0;
        if (ranks[0] == ranks[1] - 1 &&
            ranks[0] == ranks[2] - 2 &&
            ranks[0] == ranks[3] - 3 &&
            ranks[0] == ranks[4] - 4)
        {
            hasStraight = true;
            straightRank = ranks[4];
        }
        if (ranks[4] == (int)RANKS::A &&
            ranks[0] == (int)RANKS::TWO &&
            ranks[1] == (int)RANKS::THREE &&
            ranks[2] == (int)RANKS::FOUR &&
            ranks[3] == (int)RANKS::FIVE)
        {
            hasStraight = true;
            straightRank = (int)RANKS::FIVE;
        }
        hasFlush = (suits[0] == suits[1] &&
                    suits[0] == suits[2] &&
                    suits[0] == suits[3] &&
                    suits[0] == suits[4]);
        
        HAND_VALUE_TYPE pairType = HAND_VALUE_TYPE::HIGH_CARD;
        int pairRank = 0;
        int pairSecondaryRank = 0;
        
        int pairConfiguration =
        (ranks[0] == ranks[1]) |
        (ranks[1] == ranks[2]) << 1 |
        (ranks[2] == ranks[3]) << 2 |
        (ranks[3] == ranks[4]) << 3;
        
        switch(pairConfiguration)
        {
            case 0b0001:
                pairType = HAND_VALUE_TYPE::PAIR;
                pairRank = ranks[0];
                break;
            case 0b0010:
                pairType = HAND_VALUE_TYPE::PAIR;
                pairRank = ranks[1];
                break;
            case 0b0100:
                pairType = HAND_VALUE_TYPE::PAIR;
                pairRank = ranks[2];
                break;
            case 0b1000:
                pairType = HAND_VALUE_TYPE::PAIR;
                pairRank = ranks[3];
                break;
            case 0b0011:
                pairType = HAND_VALUE_TYPE::THREE_OF_A_KIND;
                pairRank = ranks[0];
                break;
            case 0b0110:
                pairType = HAND_VALUE_TYPE::THREE_OF_A_KIND;
                pairRank = ranks[1];
                break;
            case 0b1100:
                pairType = HAND_VALUE_TYPE::THREE_OF_A_KIND;
                pairRank = ranks[2];
                break;
            case 0b0111:
                pairType = HAND_VALUE_TYPE::FOUR_OF_A_KIND;
                pairRank = ranks[0];
                break;
            case 0b1110:
                pairType = HAND_VALUE_TYPE::FOUR_OF_A_KIND;
                pairRank = ranks[1];
                break;
            case 0b1011:
                pairType = HAND_VALUE_TYPE::FULL_HOUSE;
                pairRank = ranks[0];
                pairSecondaryRank = ranks[3];
                break;
            case 0b1101:
                pairType = HAND_VALUE_TYPE::FULL_HOUSE;
                pairRank = ranks[2];
                pairSecondaryRank = ranks[0];
                break;
            case 0b0101:
                pairType = HAND_VALUE_TYPE::TWO_PAIR;
                pairRank = ranks[2];
                pairSecondaryRank = ranks[0];
                break;
            case 0b1001:
                pairType = HAND_VALUE_TYPE::TWO_PAIR;
                pairRank = ranks[3];
                pairSecondaryRank = ranks[0];
                break;
            case 0b1010:
                pairType = HAND_VALUE_TYPE::TWO_PAIR;
                pairRank = ranks[3];
                pairSecondaryRank = ranks[1];
                break;
        }
        
        if (hasStraight && hasFlush)
        {
            handValueType = HAND_VALUE_TYPE::STRAIGHT_FLUSH;
            rank = straightRank;
            return;
        }
        if (pairType == HAND_VALUE_TYPE::FOUR_OF_A_KIND)
        {
            handValueType = HAND_VALUE_TYPE::FOUR_OF_A_KIND;
            rank = pairRank;
            return;
        }
        if (pairType == HAND_VALUE_TYPE::FULL_HOUSE)
        {
            handValueType = HAND_VALUE_TYPE::FULL_HOUSE;
            rank = pairRank;
            secondaryRank = pairSecondaryRank;
            return;
        }
        if (hasFlush)
        {
            handValueType = HAND_VALUE_TYPE::FLUSH;
            return;
        }
        if (hasStraight)
        {
            handValueType = HAND_VALUE_TYPE::STRAIGHT;
            rank = straightRank;
            return;
        }
        if (pairType == HAND_VALUE_TYPE::THREE_OF_A_KIND)
        {
            handValueType = HAND_VALUE_TYPE::THREE_OF_A_KIND;
            rank = pairRank;
            return;
        }
        if (pairType == HAND_VALUE_TYPE::TWO_PAIR)
        {
            handValueType = HAND_VALUE_TYPE::TWO_PAIR;
            rank = pairRank;
            secondaryRank = pairSecondaryRank;
            return;
        }
        if (pairType == HAND_VALUE_TYPE::PAIR)
        {
            handValueType = HAND_VALUE_TYPE::PAIR;
            rank = pairRank;
            return;
        }
        handValueType = HAND_VALUE_TYPE::HIGH_CARD;
    }
    
    int Compare(const HandValue& other)
    {
        if (handValueType > other.handValueType)
        {
            return 1;
        }
        else if (handValueType < other.handValueType)
        {
            return -1;
        }
        if (rank > other.rank)
        {
            return 1;
        }
        else if (rank < other.rank)
        {
            return -1;
        }
        if (secondaryRank > other.secondaryRank)
        {
            return 1;
        }
        else if (secondaryRank < other.secondaryRank)
        {
            return -1;
        }
        if (kickerValue > other.kickerValue)
        {
            return 1;
        }
        else if (kickerValue < other.kickerValue)
        {
            return -1;
        }
        return 0;
    }
};

// 0 is draw.  1 means player 0 wins.  -1 means player 1 wins
// community cards are in slots 0,1,2.  player cards are in 3,4 and 5,6
int CheckShowdown(int deck[])
{
    int playerHand0[5];
    int playerHand1[5];
    playerHand0[0] = deck[0];
    playerHand0[1] = deck[1];
    playerHand0[2] = deck[2];
    playerHand0[3] = deck[3];
    playerHand0[4] = deck[4];
    playerHand1[0] = deck[0];
    playerHand1[1] = deck[1];
    playerHand1[2] = deck[2];
    playerHand1[3] = deck[5];
    playerHand1[4] = deck[6];

    HandValue handValue0(playerHand0);
    HandValue handValue1(playerHand1);
    
    return handValue0.Compare(handValue1);
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
    if (flop2Rank > flop0Rank)
    {
        swap(flop0Rank, flop2Rank);
        swap(flop0Suit, flop2Suit);
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

bool CheckForBluffCatcher(int deck[])
{
    int flop0Rank = deck[0] / (int)SUITS::NUM;
    int flop1Rank = deck[1] / (int)SUITS::NUM;
    int flop2Rank = deck[2] / (int)SUITS::NUM;
    int hand0Rank = deck[3] / (int)SUITS::NUM;
    int hand1Rank = deck[4] / (int)SUITS::NUM;
    
    // check when having a high card of the highest or second highest rank
    int highestRank = 12;
    int secondHighestRank = 11;
    while (highestRank == flop0Rank ||
        highestRank == flop1Rank ||
        highestRank == flop2Rank)
    {
        highestRank--;
        secondHighestRank--;
    }
    while (secondHighestRank == flop0Rank ||
           secondHighestRank == flop1Rank ||
           secondHighestRank == flop2Rank)
    {
        secondHighestRank--;
    }
    
    return (hand0Rank == highestRank ||
            hand1Rank == highestRank ||
            hand0Rank == secondHighestRank ||
            hand1Rank == secondHighestRank);
}

bool CheckForStraightDraws(int deck[])
{
    int flop0Rank = deck[0] / (int)SUITS::NUM;
    int flop1Rank = deck[1] / (int)SUITS::NUM;
    int flop2Rank = deck[2] / (int)SUITS::NUM;
    int hand0Rank = deck[3] / (int)SUITS::NUM;
    int hand1Rank = deck[4] / (int)SUITS::NUM;
    
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

    for (int i = 0; i < 9; i++)
    {
        if (rankExists[i])
        {
            int existingRanksForStraightStartingHere = 1;
            if (rankExists[i+1]) existingRanksForStraightStartingHere++;
            if (rankExists[i+2]) existingRanksForStraightStartingHere++;
            if (rankExists[i+3]) existingRanksForStraightStartingHere++;
            if (rankExists[i+4]) existingRanksForStraightStartingHere++;
            if (existingRanksForStraightStartingHere == 4)
            {
                // make sure both hold cards contribute
                if (hand0Rank >= i && hand0Rank <= i + 4 &&
                  hand1Rank >= i && hand1Rank <= i + 4)
                {
                    return true;
                }
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
            if ((hand0Rank == (int)RANKS::A || (hand0Rank >= 0 && hand0Rank <= 3)) &&
                (hand1Rank == (int)RANKS::A || (hand1Rank >= 0 && hand1Rank <= 3)))
            {
                return true;
            }
        }
    }
    
    return false;
}

ACTIONS GetDealerFixedStrategyActionFirstToAct(int deck[])
{
    int hand[5];
    hand[0] = deck[0];
    hand[1] = deck[1];
    hand[2] = deck[2];
    hand[3] = deck[3];
    hand[4] = deck[4];
    HandValue handValue(hand);
    
    // any straight or higher
    if (handValue.handValueType >= HAND_VALUE_TYPE::STRAIGHT)
    {
        return ACTIONS::BET;
    }

    int flop0Rank = deck[0] / (int)SUITS::NUM;
    int flop0Suit = deck[0] % (int)SUITS::NUM;
    int flop1Rank = deck[1] / (int)SUITS::NUM;
    int flop1Suit = deck[1] % (int)SUITS::NUM;
    int flop2Rank = deck[2] / (int)SUITS::NUM;
    int flop2Suit = deck[2] % (int)SUITS::NUM;
    int hand0Rank = deck[3] / (int)SUITS::NUM;
    int hand0Suit = deck[3] % (int)SUITS::NUM;
    int hand1Rank = deck[4] / (int)SUITS::NUM;
    int hand1Suit = deck[4] % (int)SUITS::NUM;

    // any pocket pair
    if (hand0Rank == hand1Rank)
    {
        return ACTIONS::BET;
    }
    
    // any pocket card matching the board
    if (hand0Rank == flop0Rank ||
        hand0Rank == flop1Rank ||
        hand0Rank == flop2Rank ||
        hand1Rank == flop0Rank ||
        hand1Rank == flop1Rank ||
        hand1Rank == flop2Rank)
    {
        return ACTIONS::BET;
    }
    
    if (CheckForBluffCatcher(deck))
    {
        return ACTIONS::PASS;
    }

    // if both pocket cards are the same suit, if they match any suit on the board (flush draw)
    if (hand0Suit == hand1Suit &&
        (hand0Suit == flop0Suit ||
         hand0Suit == flop1Suit ||
         hand0Suit == flop2Suit))
    {
        return ACTIONS::BET;
    }

    // any gutshot or better straight draws that both hole cards contribute to
    if (CheckForStraightDraws(deck))
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

ACTIONS GetDealerFixedStrategyActionWhenBetTo(int deck[])
{
    if (CheckForBluffCatcher(deck))
    {
        return ACTIONS::BET;
    }
    // maybe I could run this one without fixed strategy to see what's best
    return ACTIONS::PASS;
}

class Solver
{
    unordered_map<string, Node> nodes;
public:
            
    double CFR(int deck[], string history, double probability0, double probability1)
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
        
        if (DEALER_USES_FIXED_STRATEGY && player == 0)
        {
            ACTIONS action;
            if (plays == 0)
            {
                action = GetDealerFixedStrategyActionFirstToAct(deck);
            }
            else
            {
                action = GetDealerFixedStrategyActionWhenBetTo(deck);
            }
            string nextHistory = history + (action == ACTIONS::PASS ? "p" : "b");
            double ev = - CFR(deck, nextHistory, probability0, probability1);
            return ev;
        }
        else
        {
            // add this decision-point node to the map
            
            string infoSet = ConstructInfoSet(deck[0], deck[1], deck[2], deck[3 + player * 2], deck[4 + player * 2], history);
            
            // make a better infoset.  all aces should be AxAyAz.  player hands should be sorted and suits x,y,z,w
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
            nodePointer->UpdateStrategySums(strategy, player == 0 ? probability0 : probability1);
            
            // traverse to the next nodes and calculate ev
            double ev[(int)ACTIONS::NUM];
            double nodeEV = 0;
            for (int a = 0; a < (int)ACTIONS::NUM; a++)
            {
                string nextHistory = history + (a == (int)ACTIONS::PASS ? "p" : "b");
                double nextNodeProbability0 = (player == 0) ? probability0 * strategy[a] : probability0;
                double nextNodeProbability1 = (player == 1) ? probability1 * strategy[a] : probability1;
                // ev is inverted here because the next node's ev is from the perspective of the opponent
                ev[a] = - CFR(deck, nextHistory, nextNodeProbability0, nextNodeProbability1);
                nodeEV += strategy[a] * ev[a];
            }
            
            double probabilityWeGetHere = player == 0 ? probability1 : probability0;
            for (int a = 0; a < (int)ACTIONS::NUM; a++)
            {
                double regret = ev[a] - nodeEV;
                nodePointer->regretSums[a] += probabilityWeGetHere * regret;
                
                //cfr+   this helps it resolve faster
                nodePointer->regretSums[a] = max(0.0, nodePointer->regretSums[a]);
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

            if (HARD_CODE_HAND)
            {
                swap(deck[HARD_CODE_HAND0], deck[3]);
                swap(deck[HARD_CODE_HAND1], deck[4]);
                cardsToReserve += 2;
            }
        }
        for (int c1 = NUM_CARDS - 1; c1 > cardsToReserve; c1--)
        {
            int c2 = rand() % (c1 + 1 - cardsToReserve) + cardsToReserve;
            swap(deck[c1], deck[c2]);
        }
    }
    
    void Print()
    {
        map<string, Node> sortedNodes;
        for (const auto & [ key, node ] : nodes)
        {
            sortedNodes.insert({key, node});
        }
        for (const auto & [ key, node ] : sortedNodes)
        {
            if (PRINT_SOME)
            {
                node.PrintSome();
            }
            else
            {
                node.Print();
            }
        }
    }
    
    void Solve()
    {
        int deck[NUM_CARDS];
        double ev = 0;
        for (int iteration = 0; iteration < NUM_ITERATIONS; iteration++)
        {
            // clear out strategy halfway through for efficiency.  cfr+ has a better weighted sum
            if (iteration == NUM_ITERATIONS/2)
            {
                for (auto & [ key, node ] : nodes)
                {
                    for (int i = 0; i < (int)ACTIONS::NUM; i++)
                    {
                        node.strategySums[i] = 0;
                    }
                }
            }
            
            ShuffleDeck(deck);
            ev += CFR(deck, "", 1, 1);
        }
        ev /= NUM_ITERATIONS;

        std::cout << "Total ev: " << ev << "\n";
        Print();
    }
};

int main(int argc, const char * argv[]) {
    srand((unsigned int)time(0));
    
    Solver solver;
    solver.Solve();
    return 0;
}


