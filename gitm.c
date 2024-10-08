// name: Matthew Lazar
// unikey: mlaz7837
// SID: 490431454

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>

#define BOARD_SIZE 19
#define LONGEST_POSSIBLE_GAME_LEN 19 * 19 * 3 // one turn for each slot on board, x3 accounts for long coordinates
#define MAX_LINE_LEN 16 // length of the longest possible valid command + 1 for the '\0' byte
#define MAX_PLACE_CMD_LEN 9
#define MIN_PLACE_CMD_LEN 8


/* USYD CODE CITATION ACKNOWLEDGEMENT
 * I declare that the following lines of code have been copied from the
 * website titled: " Stack Overflow: How to read arbitrary length input from stdin properly?" 
 * and it is not my own work. It was modified slightly by me.
 * 
 * Original URL
 * https://stackoverflow.com/questions/55794557/how-to-read-arbitrary-length-input-from-stdin-properly
 * Last access March, 2023
*/

/*
 * places at most buffer_len chars into buffer 
 * returns the number of bytes read from the buffer
 */
int get_line(char *buffer, size_t buffer_len) {
    memset(buffer, 0, buffer_len); // clears buffer so string is null-terminated

    int c;
    int bytes_read = 0;
    int whitespaces = 0;
    // read one char at a time until '\n' is read
    while ((c = fgetc(stdin)) != '\n') {
        if (c == EOF) {
            return -1;
        }
        if (bytes_read < buffer_len - 1) {
            buffer[bytes_read] = (char) c;
        }
        if ((char) c == ' ') {
            whitespaces++;
        }
        bytes_read++;
    }
    if (whitespaces > 1) {
        // no valid input has more than 1 whitespace, treat these as invalid
        return 0;
    }
    return bytes_read;
}
/* end of copied code */

/*
 * determines if the given string is a place command
 * that is, it begins with place and has the expected number of whitespaces
 * DOES NOT validate the coordinates, see place_stone function for that
 * returns 1 if string is valid, 0 otherwise
*/ 
int is_place_command(char input[]) {
    // handles short place commands 
    // -2 since place commands of length 6 or less are missing
    // proper formatting or coordinates
    if (strlen(input) <= MIN_PLACE_CMD_LEN - 2) {
        return 0;
    }

    // clone the input buffer the input line is not modified
    char input_copy[MAX_LINE_LEN];
    strncpy(input_copy, input, MAX_LINE_LEN);

    // check if the command lable is 'place'
    char *input_ptr = strtok(input_copy, " ");
    if (strcmp(input_ptr, "place") != 0) {
        return 0;
    }

    return 1;
}

/* 
 * takes in place str containing the coordinates and validates coordinates
 * then marks those coords as occupied by the  player that called the command
 * returns 1 if the place was successful, 0 if the coordinates were occupied
 * and -1 if the coordinates were out of the board range
 */
int place_stone(char valid_place_cmd[], int turn_flag,
                char board[BOARD_SIZE][BOARD_SIZE], int chars_read, 
                int *mist_center_row, int *mist_center_col) {
    /*
     * we expect only two valid string lengths: 9 or 10
     * hence, first check which case the given string falls under 
     * then we can locate the coordinate string using indexing
    */

   // check that the command is not too long or too short
   if (chars_read > MAX_PLACE_CMD_LEN || chars_read < MIN_PLACE_CMD_LEN) {
    return -1;
   }

    // extract col and row values
    char column = valid_place_cmd[6]; // col val will always be at this index
    int row;
    // - 64 to convert the letter to its corresponding index in the alphabet
    int column_index = column - 64;
    if (column < 'A' || column > 'S') {
        // column out of bounds
        return -1;
    }
    if (strlen(valid_place_cmd) == MIN_PLACE_CMD_LEN) {
        // row cordinate is a single digit number
        row = valid_place_cmd[7] - '0';
        if (row < 1 || row > 9) {
            // row out of bounds
            return -1;
        }

    } else if (strlen(valid_place_cmd) == MAX_PLACE_CMD_LEN) {
        // row coordinate is a double digit number
        char row_str[3];
        row_str[0] = valid_place_cmd[7];
        row_str[1] = valid_place_cmd[8];
        row_str[2] = '\0';
        row = atoi(row_str);
        if (row < 10 || row > 19) {
            // row out of bounds
            return -1;
        }
    }

        // check that the requested spot is empty
    if (board[19 - row][column_index - 1] == 'o' || 
        board[19 - row][column_index - 1] == '#') {
        return 0;
    }

    // updating board
    // note: the row coordinate is 19 - row since the matrix 
    // represents the rows as upside down
    if (turn_flag) {
        board[19 - row][column_index - 1] = 'o';
    } else {
        board[19 - row][column_index - 1] = '#';
    }

    // update the mist center coords
    // x coordinate
    *mist_center_col = (1 + (5 * (column_index * column_index) + 
                        3 * column_index + 4) % 19) - 1;
    // y coordinate
    *mist_center_row = 19 - (1 + (4 * (row * row) + 2 * row - 4) % 19);

    return 1;
}

/*
 * creates temp string that is filled with the chars that make up the last move
 * then concatenates this string to the history string
 */
void update_history(char *history, char *place_cmd) {
    if (strlen(place_cmd) == 8) {
        char move[3];
        move[0] = place_cmd[6];
        move[1] = place_cmd[7];
        move[2] = '\0';
        strcat(history, move);
    } else if (strlen(place_cmd) == 9) {
        char move[4];
        move[0] = place_cmd[6];
        move[1] = place_cmd[7];
        move[2] = place_cmd[8];
        move[3] = '\0';
        strcat(history, move);
    }
}

// flips the turn flag
void update_turn_flag(int *turn_flag) {
    if (*turn_flag) {
        *turn_flag = 0;
    } else {
        *turn_flag = 1;
    }
}

// ends the game and prints corresponding messages
void end_game(int turn_flag, char *history) {
    if (turn_flag) {
        printf("Black wins!\n");
    } else {
        printf("White wins!\n");
    }

    printf("%s\n", history);
    printf("Thank you for playing!\n");
}

// initalizes each cell of the board as empty .
void initialize_board(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = '.';
        }
    }

}

/*
 * checks if a winning position has been reached by searching for all possible 
 * locations a 5-in-a-row has been make, for each type of win
 * (horizontal, vertical, positive diagonal, negative diagonal)
 * Note: -4 from a loop upper bound means that no win 
 * is possible in the outer 4 rows / cols
 * Similarly, starting an index a 4 means that no win 
 * is possible in the initial 4 rows / cols
 */
int check_game_state(char board[BOARD_SIZE][BOARD_SIZE], int turn_flag) {
    // determine the value of piece
    char piece;
    if (turn_flag) {
        piece = 'o';
    } else {
        piece = '#';
    }

    // check for vertical 5-in-a-row
    for (int i = 0; i < BOARD_SIZE - 4; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == piece && board[i + 1][j] == piece &&
                board[i + 2][j] == piece && board[i + 3][j] == piece &&
                board[i + 4][j] == piece) {
                return 1;
            }
        }
    }

    // check for horizontal 5-in-a-row
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE - 4; j++) {
            if (board[i][j] == piece && board[i][j + 1] == piece &&
                board[i][j + 2] == piece  && 
                board[i][j + 3] == piece && board[i][j + 4] == piece) {
                return 1;
            }
        }
    }

    // check for positive diagonal 5-in-a-row
    for (int i = 4; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE - 4; j++) {
            if (board[i][j] == piece && board[i - 1][j + 1] == piece && 
                board[i - 2][j + 2] == piece && board[i - 3][j + 3] == piece && 
                board[i - 4][j + 4] == piece) {
                return 1;
            }
        }
    }

    // check for negative diagonal 5-in-a-row
    for (int i = 4; i < BOARD_SIZE; i++) {
        for (int j = 4; j < BOARD_SIZE - 4; j++) {
            if (board[i][j] == piece && board[i - 1][j - 1] == piece && 
                board[i - 2][j - 2] == piece && board[i - 3][j - 3] == piece && 
                board[i - 4][j - 4] == piece) {
                return 1;
            }
        }
    }

    return 0;
}

/*
 * prints the view string in the specified format
 */
void view_hole(char board[BOARD_SIZE][BOARD_SIZE], int mist_center_row, 
               int mist_center_col) {
    // +65 to convert index to corresponding capital letter
    // 19 - since rows on board matrix are top-to-bottom
    printf("%c%d,", mist_center_col + 65, 19 - mist_center_row);
    for (int i = mist_center_row - 3; i < mist_center_row + 4; i++) {
        for (int j = mist_center_col - 3; j < mist_center_col + 4; j++) {
            if (i < 0 || i >= BOARD_SIZE || j < 0 || j >= BOARD_SIZE) {
                printf("x");
            } else {
                printf("%c", board[i][j]);
            }
        }
    }
    printf("\n");
}

// !! Functions here are for testing !!
// prints board
// void display_board(char board[BOARD_SIZE][BOARD_SIZE], int mist_center_row, int mist_center_col) {
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         printf("row: %d ", 19 - i);
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             printf("%c", board[i][j]);
//         }
//         printf("\n");
//     }
// }

// displays board covered in mist
// void display_board_mist(char board[BOARD_SIZE][BOARD_SIZE], int mist_center_row, int mist_center_col) {
//     for (int i = 0; i < BOARD_SIZE; i++) {
//         printf("row: %d ", 19 - i);
//         for (int j = 0; j < BOARD_SIZE; j++) {
//             if (i == mist_center_row && j == mist_center_col) {
//                 printf("A");
//             } else if (((i > (mist_center_row - 4) && i < (mist_center_row + 4)) && (j > (mist_center_col - 4) && j < (mist_center_col + 4)))) {
//                 printf("%c", board[i][j]);
//             } else {
//                 printf("@");
//             }
//         }
//         printf("\n");
//     }
// }
// !! Functions here are for testing !!

int main(int argc, char *argv[]) {

    // setting up board
    char board[BOARD_SIZE][BOARD_SIZE];
    initialize_board(board);

    // initializing mist center
    int mist_center_row = 9;
    int mist_center_col = 9;

    // setting up buffers and turn_flag
    int turn_flag = 0; // 0 = black's turn, 1 = white's turn
    int turn_count = 1;
    int game_state = 0; // 0 = in-play, 1 = winning position reached
    char history[LONGEST_POSSIBLE_GAME_LEN + 1] = { '\0' }; 
    char input[MAX_LINE_LEN];

    // main loop for parsing user input
    // accept input as long as no winning state has been reached
    while (!game_state) {
        if (turn_count > (19 * 19)) {
            // tie occurs after all spaces on board are occupied 
            // and a win has not been reached
            printf("Wow, a tie!\n");
            printf("%s\n", history);
            printf("Thank you for playing!\n");
            return 0;
        }

        int chars_read = get_line(input, MAX_LINE_LEN);
        if (chars_read == -1) {
            // terminate once reaching EOF, mainly for testing
            return 0;
        }
        if (chars_read <= 1) {
            printf("Invalid!\n");

        } else if (strcmp(input, "who") == 0) {
            if (turn_flag) {
                printf("W\n");
            } else {
                printf("B\n");
            }

        } else if (strcmp(input, "term") == 0) {
            return 1;

        } else if (strcmp(input, "resign") == 0) {
            end_game(turn_flag, history);
            return 0;

        } else if (strcmp(input, "history") == 0) {
            printf("%s\n", history);

        } else if (strcmp(input, "view") == 0) {
            view_hole(board, mist_center_row, mist_center_col);

        } else if (is_place_command(input)) {
            // handle placing the stone, or rejecting if invalid coordinates are
            // given / line is too long
            int successful_place = place_stone(input, turn_flag, board, 
                                               chars_read, &mist_center_row, 
                                               &mist_center_col);
            if (successful_place == 1) {
                // add successfully placed stone to history, 
                // then check for a winning position
                update_history(history, input);
                game_state = check_game_state(board, turn_flag);
                update_turn_flag(&turn_flag);
                turn_count++;

            } else if (successful_place == 0) {
                printf("Occupied coordinate\n");

            } else if (successful_place == -1) {
                printf("Invalid coordinate\n");

            }
        } else {
            printf("Invalid!\n");
            
        }
    }
    
    end_game(turn_flag, history);
    return 0;
}