#include <iostream>
#include <unordered_map>
#include <map>

using namespace std;

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
};

const int NUM_ITERATIONS = 10;
const int NUM_CARDS = (int)RANKS::NUM * (int)SUITS::NUM;
const int BET_AMOUNT = 2;
const bool HARD_CODE_FLOP = true;
const int HARD_CODE_FLOP0 = 48;
const int HARD_CODE_FLOP1 = 49;
const int HARD_CODE_FLOP2 = 50;

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
            std::round(strategy[(int)ACTIONS::BET] * 1000.0) / 1000.0 << " Bet: " <<
            "\n";
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

string CardToString(int card)
{
    int rank = card / (int)SUITS::NUM;
    RANKS rankEnum = (RANKS)rank;
    int suit = card % (int)SUITS::NUM;
    SUITS suitEnum = (SUITS)suit;
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
        default:
            break;
    }
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
        (ranks[0] == ranks[1]) ||
        (ranks[1] == ranks[2]) << 1 ||
        (ranks[2] == ranks[3]) << 2 ||
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
        
        // add this decision-point node to the map
        string infoSet =
            CardToString(deck[0]) +
            CardToString(deck[1]) +
            CardToString(deck[2]) +
            CardToString(deck[3 + player * 2]) +
            CardToString(deck[4 + player * 2]) +
            history;
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
            
            //cfr+
            nodePointer->regretSums[a] = max(0.0, nodePointer->regretSums[a]);
        }

        return nodeEV;
    }
    
    void ShuffleDeck(int deck[])
    {
        for (int i = 0; i < NUM_CARDS; i++)
        {
            deck[i] = i;
        }
        if (HARD_CODE_FLOP)
        {
            swap(deck[HARD_CODE_FLOP0], deck[0]);
            swap(deck[HARD_CODE_FLOP1], deck[1]);
            swap(deck[HARD_CODE_FLOP2], deck[2]);
            for (int c1 = NUM_CARDS - 1; c1 > 3; c1--)
            {
                int c2 = rand() % (c1 - 2) + 3;
                swap(deck[c1], deck[c2]);
            }
        }
        else
        {
            for (int c1 = NUM_CARDS - 1; c1 > 0; c1--)
            {
                int c2 = rand() % (c1 + 1);
                swap(deck[c1], deck[c2]);
            }
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
            node.Print();
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


