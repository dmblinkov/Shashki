#include <iostream>

using namespace std;

#define DESK_WIDTH 8
#define DESK_HEIGHT 8
#define CHECKERS_AMOUNT 12
#define PLAYERS_AMOUNT 2

#define CheckerData_Alive 0
#define CheckerData_CoordX 1
#define CheckerData_CoordY 2
#define CheckerData_Upgraded 3
#define CheckerData_Size 4

#define ERROR -1

#define DIRECTION_LEFT_BOTTOM 0
#define DIRECTION_LEFT_TOP 1
#define DIRECTION_RIGHT_TOP 2
#define DIRECTION_RIGHT_BOTTOM 3

#define CHECK_MOVE_RESULT_INVALID 0
#define CHECK_MOVE_RESULT_VALID 1
#define CHECK_MOVE_RESULT_KILL 2


int
    g_Checkers[PLAYERS_AMOUNT][CHECKERS_AMOUNT][CheckerData_Size],
    g_Turn_PlayerId;

char const g_CheckersTextures[] = { 'w','W','b','B' };
 
#define PLAYER_WHITE 0
#define PLAYER_BLACK 1

char const* PlayerName[] = { "White","Black" };

int getStrLen(char* str) {
    int len = 0;
    while (str[len++] != '\0')
        continue;

    return len - 1;
}

bool isCharInRange(char chFrom, char chIn, char chTo) {
    int idFrom = (int)chFrom;
    int idCheck = (int)chIn;
    int idTo = (int)chTo;
    return (idFrom <= idCheck && idCheck <= idTo);
}

void consoleClear() {
    cout << "\033c";
}

int* parseStrStep(char* str) {
    int* outData = new int[5];
    outData[0] = 1;

    int deskHeightLower = (int)'a';
    int deskHeightInput = (int)tolower(str[0]);
    int deskHeightHigher = (int)'h';
    bool isHeightInRange = (deskHeightLower <= deskHeightInput && deskHeightInput <= deskHeightHigher);

    if (getStrLen(str) != 4) {
        cout << "len" << endl;
    }
    if (!isCharInRange('1', str[1], '8') ||
        !isCharInRange('1', str[3], '8') ||
        !isCharInRange('a', str[0], 'h') ||
        !isCharInRange('a', str[2], 'h')) {
        outData[0] = 0;
        return outData;
    }
    outData[1] = 7 - ((int)str[1] - (int)'1'); // From X
    outData[2] = (int)tolower(str[0]) - (int)'a'; // From Y
    outData[3] = 7 - ((int)str[3] - (int)'1'); // To X
    outData[4] = (int)tolower(str[2]) - (int)'a'; // To Y

    return outData;
}

char tryGetCheckerTexture(int x, int y) {
    for (int i = 0; i < PLAYERS_AMOUNT; i++)
        for (int j = 0; j < CHECKERS_AMOUNT; j++) {
            if (g_Checkers[i][j][CheckerData_Alive] != 1) continue;
            if (g_Checkers[i][j][CheckerData_CoordX] != x) continue;
            if (g_Checkers[i][j][CheckerData_CoordY] != y) continue;

            return g_CheckersTextures[i * 2 + g_Checkers[i][j][CheckerData_Upgraded]];
        }

    return ERROR;
}

int vecToDirection(int vecX, int vecY) {
    if (vecX < 0 && vecY < 0) return DIRECTION_LEFT_TOP;
    if (vecX < 0 && vecY > 0) return DIRECTION_RIGHT_TOP;
    if (vecX > 0 && vecY < 0) return DIRECTION_LEFT_BOTTOM;
    if (vecX > 0 && vecY > 0) return DIRECTION_RIGHT_BOTTOM;
}

int* directionToVector(int dir) {
    int* vec = new int[2];

    switch (dir) {
    case DIRECTION_LEFT_BOTTOM: {
        vec[0] = 1;
        vec[1] = -1;
        break;
    }
    case DIRECTION_LEFT_TOP: {
        vec[0] = -1;
        vec[1] = -1;
        break;
    }
    case DIRECTION_RIGHT_TOP: {
        vec[0] = -1;
        vec[1] = 1;
        break;
    }
    case DIRECTION_RIGHT_BOTTOM: {
        vec[0] = 1;
        vec[1] = 1;
        break;
    }
    }
    return vec;
}

void turnInit() {
    g_Turn_PlayerId = PLAYER_WHITE;
}

void turnSwitch() {
    g_Turn_PlayerId++;
    if (g_Turn_PlayerId == PLAYERS_AMOUNT)
        g_Turn_PlayerId = PLAYER_WHITE;

}

void checkersSpawn() {
    for (int i = 0; i < PLAYERS_AMOUNT; i++)
        for (int j = 0; j < CHECKERS_AMOUNT; j++) {
            g_Checkers[i][j][CheckerData_Alive] = 1;
            g_Checkers[i][j][CheckerData_Upgraded] = 0;
        }

    int checkCounter = 0;
    for (int y = 0; y < 3; y++)
        for (int x = 0; x < 8; x += 2) {
            g_Checkers[0][checkCounter][CheckerData_CoordX] = y;
            g_Checkers[0][checkCounter][CheckerData_CoordY] = x + (y + 1) % 2;

            g_Checkers[1][checkCounter][CheckerData_CoordX] = 7 - y;
            g_Checkers[1][checkCounter++][CheckerData_CoordY] = x + (y) % 2;
        }
}

void gameInit() {
    turnInit();
    checkersSpawn();
}

void drawDesk() {
    cout << "\tCHESS" << endl;
    cout << "   a b c d e f g h" << endl;
    for (int i = 0; i < DESK_WIDTH; i++) {
        cout << " " << 8 - i << " ";

        for (int j = 0; j < DESK_HEIGHT; j++) {
            char entityTexture = tryGetCheckerTexture(i, j);
            if (entityTexture != ERROR) {
                cout << ' ' << entityTexture; // << endl;
            }
            else if (i % 2 == j % 2) cout << char(219) << char(219);
            else cout << "  ";
        }
        cout << endl;
    }
}

void killChecker(int playerId, int checkerId) {
    g_Checkers[playerId][checkerId][CheckerData_Alive] = 0;
}

bool isAnyCheckerHere(int x, int y) {
    for (int i = 0; i < PLAYERS_AMOUNT; i++)
        for (int j = 0; j < CHECKERS_AMOUNT; j++) {
            if (g_Checkers[i][j][CheckerData_Alive] != 1) continue;
            if (g_Checkers[i][j][CheckerData_CoordX] != x) continue;
            if (g_Checkers[i][j][CheckerData_CoordY] != y) continue;
            return true;
        }
    return false;
}

int tryGetPlayerCheckerByOrigin(int x, int y, int playerId) {
    for (int i = 0; i < CHECKERS_AMOUNT; i++) {
        if (g_Checkers[playerId][i][CheckerData_Alive] != 1) continue;
        if (g_Checkers[playerId][i][CheckerData_CoordX] == x &&
            g_Checkers[playerId][i][CheckerData_CoordY] == y) {
            return i;
        }
    }
    return ERROR;
}

bool isValidMoveVector(int vecX, int vecY) {
    return abs(vecX) == abs(vecY);
}

int getEnemyId(int playerId) {
    return (playerId == 0 ? 1 : 0);
}

bool isOutOfDesk(int x, int y) {
    return (x < 0 || x >= DESK_WIDTH || y < 0 || y >= DESK_HEIGHT);
}

bool isValidDir(int dir, int playerId) {
    if (playerId == PLAYER_WHITE) {
        if (dir != DIRECTION_LEFT_BOTTOM && dir != DIRECTION_RIGHT_BOTTOM)
            return false;
    }
    else
        if (dir != DIRECTION_LEFT_TOP && dir != DIRECTION_RIGHT_TOP)
            return false;

    return true;

}

bool isDirKillable(int dir, int checkerId, int playerId) {
    bool isUpgraded = g_Checkers[playerId][checkerId][CheckerData_Upgraded] == 1 ? true : false;
    int* dirVec = directionToVector(dir);
    int vecX = dirVec[0];
    int vecY = dirVec[1];
    int startX = g_Checkers[playerId][checkerId][CheckerData_CoordX];
    int startY = g_Checkers[playerId][checkerId][CheckerData_CoordY];

    int currentX = startX;
    int currentY = startY;
    int enemyId = getEnemyId(playerId);

    bool kill = false;
    int dist = 0;

    while (!isOutOfDesk(currentX, currentY)) {
        if (dist > 0 && !isUpgraded) {
            return false;
        }
        currentX += vecX;
        currentY += vecY;

        int enemyCheckerId = tryGetPlayerCheckerByOrigin(currentX, currentY, enemyId);
        if (enemyCheckerId != ERROR) {
            if (!isAnyCheckerHere(currentX + vecX, currentY + vecY) && !isOutOfDesk(currentX + vecX, currentY + vecY))
                return true;
            else
                dist++;
        }
        else {
            dist++;
        }
    }

    return false;
}

int checkVecMove(int dir, int checkerId, int playerId, int dist = -1, int endX = -1, int endY = -1) {
    bool isUpgraded = g_Checkers[playerId][checkerId][CheckerData_Upgraded] == 1 ? true : false;
    int* dirVec = directionToVector(dir);
    int vecX = dirVec[0];
    int vecY = dirVec[1];
    int startX = g_Checkers[playerId][checkerId][CheckerData_CoordX];
    int startY = g_Checkers[playerId][checkerId][CheckerData_CoordY];

    int currentX = startX;
    int currentY = startY;
    int enemyId = getEnemyId(playerId);

    bool kill = false;
    int freeSteps = 0;

    if (dist == -1) {
        int testX = startX;
        while (!isOutOfDesk(testX, startY)) {
            testX += vecX;
            dist++;
        }
    }

    for (int i = 0; i < dist; i++) {
        if (freeSteps > 0 && !isUpgraded) {
            return CHECK_MOVE_RESULT_INVALID;
        }

        currentX += vecX;
        currentY += vecY;

        int enemyCheckerId = tryGetPlayerCheckerByOrigin(currentX, currentY, enemyId);
        if (enemyCheckerId != ERROR) {
            if (isAnyCheckerHere(currentX + vecX, currentY + vecY)) {
                cout << "Blocked by another checker" << endl;
                return CHECK_MOVE_RESULT_INVALID;
            }

            kill = true;
        }
        else {
            if (!isUpgraded && !kill) {
                if (!isValidDir(dir, playerId)) {
                    cout << "Invalid move dir" << endl;
                    return CHECK_MOVE_RESULT_INVALID;
                }
            }
            freeSteps++;
        }

        if (isOutOfDesk(currentX, currentY)) {
            cout << "Out of the world?" << endl;
            return CHECK_MOVE_RESULT_INVALID;
        }

    }

    return kill ? CHECK_MOVE_RESULT_KILL : CHECK_MOVE_RESULT_VALID;
}

bool isCheckerMoveKills(int dir, int endX, int endY, int playerCheckerId) {
    bool isUpgraded = g_Checkers[g_Turn_PlayerId][playerCheckerId][CheckerData_Upgraded] == 1 ? true : false;
    int currentDist = 0;

    int* dirVec = directionToVector(dir);
    int vecX = dirVec[0];
    int vecY = dirVec[1];

    int startX = g_Checkers[g_Turn_PlayerId][playerCheckerId][CheckerData_CoordX];
    int startY = g_Checkers[g_Turn_PlayerId][playerCheckerId][CheckerData_CoordY];

    int currentX = startX;
    int currentY = startY;

    int enemyId = getEnemyId(g_Turn_PlayerId);
    int dist = 0;
    bool kill = false;
    while (currentX != endX && currentY != endY) {
        currentX += vecX;
        currentY += vecY;

        int enemyCheckerId = tryGetPlayerCheckerByOrigin(currentX, currentY, enemyId);
        if (enemyCheckerId != ERROR) {
            if (!isAnyCheckerHere(currentX + vecX, currentY + vecY)) {
                killChecker(enemyId, enemyCheckerId);
                kill = true;
            }
        }
        else dist++;
    }

    g_Checkers[g_Turn_PlayerId][playerCheckerId][CheckerData_CoordX] = currentX;
    g_Checkers[g_Turn_PlayerId][playerCheckerId][CheckerData_CoordY] = currentY;

    if (currentX == (g_Turn_PlayerId == PLAYER_WHITE ? (DESK_WIDTH - 1) : 0))
        g_Checkers[g_Turn_PlayerId][playerCheckerId][CheckerData_Upgraded] = 1;

    return kill;
}

bool anyAbleKillMove(int playerId) {
    for (int i = 0; i < CHECKERS_AMOUNT; i++) {
        if (g_Checkers[playerId][i][CheckerData_Alive] != 1)
            continue;

        if (isDirKillable(DIRECTION_LEFT_BOTTOM, i, playerId) ||
            isDirKillable(DIRECTION_LEFT_TOP, i, playerId) ||
            isDirKillable(DIRECTION_RIGHT_BOTTOM, i, playerId) ||
            isDirKillable(DIRECTION_RIGHT_TOP, i, playerId)) {
            return true;
        }
    }
    return false;
}

bool tryPlayerMove(int fromX, int fromY, int toX, int toY) {
    if (isAnyCheckerHere(toX, toY)) {
        cout << "End position blocked by another check" << endl;
        return false;
    }

    int vecX = toX - fromX;
    int vecY = toY - fromY;
    if (!isValidMoveVector(vecX, vecY)) {
        cout << "Move vector should be diagonal" << endl;
        return false;
    }

    int playerId = g_Turn_PlayerId;
    int playerCheckerId = tryGetPlayerCheckerByOrigin(fromX, fromY, playerId);
    if (playerCheckerId == ERROR) {
        cout << "There's no your checker at start position" << endl;
        return false;
    }

    int vecDir = vecToDirection(vecX, vecY);

    if (checkVecMove(vecDir, playerCheckerId, playerId, abs(vecX), toX, toY) == CHECK_MOVE_RESULT_INVALID) {
        return false;
    }

    if (isCheckerMoveKills(vecDir, toX, toY, playerCheckerId) && anyAbleKillMove(playerId))
        cout << "BONUS TURN! Can you find a kill?" << endl;
    else
        turnSwitch();

    return true;
}

void gameStart() {
    drawDesk();
    while (true) {

        char inputStr[6];
        cin >> inputStr;

        auto inputData = parseStrStep(inputStr);

        consoleClear();

        tryPlayerMove(inputData[1], inputData[2], inputData[3], inputData[4]);
        drawDesk();

        cout << "DEBUG: InputDataValid?:" << inputData[0] << "; From(x,y):" << inputData[1] << "," << inputData[2] << "; To(x,y):" << inputData[3] << "," << inputData[4] << endl;
        cout << "Before turn: " << PlayerName[g_Turn_PlayerId] << endl;
        cout << "New turn: " << PlayerName[g_Turn_PlayerId] << endl;
    }
}

int main() {
    gameInit();
    gameStart();
    return 0;
}