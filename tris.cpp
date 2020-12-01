#include <iostream>
#include <ctime>

using namespace std;

/**
 * Insieme contentente i 4 stati possibili di un game
 */
enum GameStatus {
    IN_GAME,
    ENDED_WITH_WINNER,
    ENDED_IN_PARITY,
    EXITED
};

/**
 * Struttura rappresentante un game, con la propria matrice, lo stato attuale del game e il player corrente
 */
typedef struct {
    uint8_t trisMatrix[3][3]; // i valori assunti dalla matrice sono 3, 0 indica cella vuota oppure 1 e 2 indicano i player
    uint8_t currentPlayer; // i player sono 1 e 2
    GameStatus gameStatus; // stato attuale del gioco
} TrisGame;

/**
 * Mappa dei simboli associati ai giocatori, X per il primo giocatore e O per il secondo giocatore
 */
const char PLAYER_CHAR_MAP[2] = {'X', 'O'};


void startGame(TrisGame *trisGame);

void printGame(const TrisGame *trisGame);
void printGameHeader(const TrisGame *trisGame);
void printGameResult(const TrisGame *trisGame);

void updateGameStatus(TrisGame *trisGame);
void setCurrentPlayerCell(TrisGame *trisGame, int n);
void readAndSetCell(TrisGame *trisGame);
void askForNewGame(TrisGame *trisGame);
void resetGame(TrisGame *trisGame);

bool isCellFree(TrisGame *trisGame, int n);
bool isTrisMatrixFull(const TrisGame *trisGame);
GameStatus isGameFinished(TrisGame *trisGame);

int main() {
    srand(time(nullptr));

    TrisGame *trisGame = new TrisGame{};
    startGame(trisGame);

	delete trisGame;
    cout << "Grazie per aver giocato. Arrivederci!";
    return 0;
}

/**
 * Avvia il game passato, loopando fino a fine gioco
 * @param trisGame il game da avviare
 */
void startGame(TrisGame *trisGame) {
    if (trisGame) {
        trisGame->gameStatus = IN_GAME;
        trisGame->currentPlayer = rand() % 2 + 1; //seleziona semi-randomicamente un player tra 1 e 2

        do {
            printGame(trisGame);
            readAndSetCell(trisGame);
            updateGameStatus(trisGame);
        } while (trisGame->gameStatus == IN_GAME);

        if (trisGame->gameStatus != EXITED) {
            printGame(trisGame);
            printGameResult(trisGame);
            askForNewGame(trisGame);
        }
    } else {
        cerr << "Il game in input non puo essere NULL" << endl;
    }
}

/**
 * Printa la TUI(Text-based user interface) del gioco passato come parametro, printando in alto un header di gioco
 * @param trisGame il game che si sta giocando attualmente
 */
void printGame(const TrisGame *trisGame) {
    system("cls");
    printGameHeader(trisGame);

    int cellNumber = 1, i, j;
    for (i=0; i<3; i++) {
        cout << "-------------------" << endl << "| ";
        for (j=0; j<3; j++) {
            if (trisGame->trisMatrix[i][j] != 0) {
                char cur = PLAYER_CHAR_MAP[trisGame->trisMatrix[i][j] - 1];
                cout << " " << cur << " " << " | ";
            } else {
                cout << " " << cellNumber << " " << " | ";
            }
            if (j == 2) cout << endl;
            cellNumber++;
        }
    }
    cout << "-------------------" << endl;
}

/**
 * Printa l'header di gioco contenente lo stato attuale, Turno di X/0 oppure Parita'
 * @param trisGame il game che si sta giocando attualmente
 */
void printGameHeader(const TrisGame *trisGame) {
    switch (trisGame->gameStatus) {
        case IN_GAME:
            cout << "    [Turno di " << PLAYER_CHAR_MAP[trisGame->currentPlayer - 1] << "]" << endl;
            break;
        case ENDED_WITH_WINNER:
            cout << "    [" << PLAYER_CHAR_MAP[trisGame->currentPlayer - 1] << " ha vinto]" << endl;
            break;
        case ENDED_IN_PARITY:
            cout << "     [Parita']" << endl;
            break;
        default:
            break;
    }
}

/**
 * Dopo la fine di un game printa il risultato finale
 * @param trisGame il game che si sta giocando attualmente
 */
void printGameResult(const TrisGame *trisGame) {
    switch (trisGame->gameStatus) {
        case ENDED_WITH_WINNER:
            cout << "Il giocatore " << PLAYER_CHAR_MAP[trisGame->currentPlayer - 1] << " ha vinto. Complimenti!" << endl;
            break;
        case ENDED_IN_PARITY:
            cout << "Nessun giocatore ha vinto, parita'!" << endl;
            break;
        default:
            break;
    }
}

/**
 * Aggiorna ad ogni loop lo stato di gioco, controllando un eventuale vittoria e switchando tra i players
 * @param trisGame il game che si sta giocando attualmente
 */
void updateGameStatus(TrisGame *trisGame) {
    if (trisGame->gameStatus != EXITED) {
        trisGame->gameStatus = isGameFinished(trisGame);
        if (trisGame->gameStatus == IN_GAME) {
            trisGame->currentPlayer = trisGame->currentPlayer == 1 ? 2 : 1;
        }
    }
}

/**
 * Dato un numero da 1 a 9(a cui viene sottratto 1 per renderlo da 0 a 8) viene selezionata la casella individuata
 * dalla riga e dalla colonna calcolata
 * @param trisGame il game da cui selezionare la cella
 * @param n il valore lineare di una cella
 * @return un puntatore alla cella selezionata dalla matrice
 */
uint8_t* getLinearCell(TrisGame *trisGame, int n) {
    n--;
    int c = n % 3; // colonna
    int r = (n - c) / 3; // riga
    return &(trisGame->trisMatrix[r][c]);
}

/**
 * Assegna al giocatore corrente la casella selezionata
 * @param trisGame il game che si sta giocando attualmente
 * @param n il numero della cella selezionata dal player, a partire da 1
 */
void setCurrentPlayerCell(TrisGame *trisGame, int n) {
    *(getLinearCell(trisGame, n)) = trisGame->currentPlayer;
}

/**
 * Legge da terminale la casella selezionata dal player effettuando un controllo sull'input, e gliela assegna
 * @param trisGame il game che si sta giocando attualmente
 */
void readAndSetCell(TrisGame *trisGame) {
    int cell;
    bool isCellValid;
    cout << "E' il turno del giocatore " << PLAYER_CHAR_MAP[trisGame->currentPlayer - 1] << " [inserisci il numero della cella o -1 per uscire]: ";
    do {
        isCellValid = true;
        cin >> cell;

        cout << endl;
        if (cell < -1 || cell > 9 || !isCellFree(trisGame, cell)) {
            cout << "La selezione non e' valida. Riprovare: ";
            isCellValid = false;
        }
    } while (!isCellValid);

    if (cell == -1) {
        trisGame->gameStatus = EXITED;
        return;
    }

    setCurrentPlayerCell(trisGame, cell);
}

/**
 * Dopo la terminazione di un game(non uscita dal gioco) chiedi se si vuole continuare a giocare
 * @param trisGame il game che e' appena terminato
 */
void askForNewGame(TrisGame *trisGame) {
    if (trisGame->gameStatus != IN_GAME) {
        char answer;
        cout << "Vuoi giocare ancora [S/N]? ";
        cin >> answer;
        if (answer == 'S' || answer == 's') {
            resetGame(trisGame);
            startGame(trisGame);
        }
    } else {
        cerr << "La partita e' gia' in corso" << endl;
    }
}

/**
 * Pulisce la matrice di gioco
 * @param trisGame il game da resettare
 */
void resetGame(TrisGame *trisGame) {
    int i, j;
    for (i = 0; i < 3; ++i) {
        for (j = 0; j < 3; ++j) {
            trisGame->trisMatrix[i][j] = 0;
        }
    }
}

/**
 * Controlla se una cella in un determinato game è libera o meno
 * @param trisGame il game su cui controllare la cella
 * @param n valore lineare della cella
 * @return true se la cella è libera, altrimenti false
 */
bool isCellFree(TrisGame *trisGame, int n) {
    return *(getLinearCell(trisGame, n)) == 0;
}

/**
 * Controlla tutte le caselle per vedere se ce ne sono di libere
 * @param trisGame il game di cui bisogna controllare se la matrice è piena
 * @return true se la matrice è tutta piena, altrimenti false
 */
bool isTrisMatrixFull(const TrisGame *trisGame) {
    int i, j;
    for (i=0; i<3; i++) {
        for (j=0; j<3; j++) {
            if (trisGame->trisMatrix[i][j] == 0) {
                return false;
            }
        }
    }
    return true;
}

/**
 * Controlla se qualche player ha riempito una colonna, una riga o una diagonale
 * @param trisGame il game dove cercare un eventuale vincita
 * @return true se una colonna o una riga o una diagonale è stata riempita da un player
 */
GameStatus isGameFinished(TrisGame *trisGame) {
    uint8_t (*trisMatrix)[3] = trisGame->trisMatrix;
    uint8_t currentPlayer = trisGame->currentPlayer;
    int i;

    // controlla un eventuale vincita nelle colonne e nelle righe
    for (i = 0; i < 3; ++i) {
        if ((trisMatrix[i][0] == currentPlayer && trisMatrix[i][1] == currentPlayer && trisMatrix[i][2] == currentPlayer)
         || (trisMatrix[0][i] == currentPlayer && trisMatrix[1][i] == currentPlayer && trisMatrix[2][i] == currentPlayer)) {
            return ENDED_WITH_WINNER;
        }
    }

    // controlla un eventuale vincita nelle due diagonali
    if (trisMatrix[1][1] == currentPlayer &&
        ((trisMatrix[2][2] == currentPlayer && trisMatrix[0][0] == currentPlayer) ||
         (trisMatrix[0][2] == currentPlayer && trisMatrix[2][0] == currentPlayer))) {
        return ENDED_WITH_WINNER;;
    }

    // se la matrice è piena e non ci sono vincitori, finisce in parità
    if (isTrisMatrixFull(trisGame)) {
        return ENDED_IN_PARITY;
    }
    return IN_GAME;
}
