// librairies
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <stdbool.h>

// Définition des pièces
#define black_king   'K'
#define black_queen  'Q'
#define black_rook   'R'
#define black_bishop 'B'
#define black_knight 'N'
#define black_pawn   'P'

#define white_king   'k'
#define white_queen  'q'
#define white_rook   'r'
#define white_bishop 'b'
#define white_knight 'n'
#define white_pawn   'p'

// Structure qui stocke la position de départ d'une pièce.
struct start_position {
    char start_position[3];
    int absice_position;
    int ordonne_position;
};

// Structure qui stocke la position d'arrivée potentielle (avant confirmation de la légalité du coup) d'une pièce.
struct end_position {
    char potential_end_position[3];
    int absice_position;
    int ordonne_position;
};

// Structure qui stocke la position x, y en direct de chaque roi.
struct positon_of_kings {
    int x_position_white_king;
    int y_position_white_king;
    int x_position_black_king;
    int y_position_black_king;
};

wchar_t board[8][8] = {
    { black_rook, black_knight, black_bishop, black_queen, black_king, black_bishop,  black_knight, black_rook },
    { black_pawn, black_pawn, black_pawn, black_pawn, black_pawn, black_pawn, black_pawn, black_pawn },
    { L' ', L' ', L' ', L' ', L' ', L' ',  L' ',  L' '},
    { L' ', L' ', L' ', L' ', L' ', L' ', L' ',  L' ' },
    { L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ' },
    { L' ', L' ', L' ', L' ', L' ', L' ', L' ', L' ' },
    { white_pawn, white_pawn, white_pawn, white_pawn, white_pawn, white_pawn, white_pawn, white_pawn },
    { white_rook, white_knight, white_bishop, white_queen, white_king, white_bishop, white_knight, white_rook }
};

void Show_table(); // fonction qui imprime le chessboard
wchar_t find_piece(struct start_position s); // fonction qui trouve la pièce dans le plateau à partir de la position reçue et retourne la pièce en question
void update_board(struct start_position s, struct end_position e, struct positon_of_kings *p); // cette fonction update le tableau et vérifie la légalité du coup en fonction de la start-x, start-y, end-x, end-y.
void Convertor_Position_to_index_array(char position[3], int *absice_position, int *ordonne_position); // cette fonction va convertir la case choisie par le joueur et les convertit en index abscisse pour le x horizontal et l'ordonnée pour le y vertical

// ces fonctions vérifient la légalité de chaque coup en fonction de la pièce choisie par le joueur
int bishop_verification(int start_x, int start_y, int end_x, int end_y);
int rook_verification(int start_x, int start_y, int end_x, int end_y);
int check_if_it_black_or_white(wchar_t piece);
int pawn_verification(int start_x, int start_y, int end_x, int end_y, wchar_t piece);
int knight_verification(int start_x, int start_y, int end_x, int end_y);

// ces fonctions vérifient si sur la case donnée il y a un possible échec. Elles sont utiles pour le roi et permettent de vérifier si le déplacement du roi vers une case sera échec sur le roi, donc déplacement non autorisé, et permettent de savoir aussi si le roi est en échec sur sa position actuelle.
int check_diagonal_verification(int start_x, int start_y, int end_x, int end_y);
int check_rook_verification(int start_x, int start_y, int end_x, int end_y);
int check_pawn_verification(int start_x, int start_y, int end_x, int end_y);
int check_knight_verification(int start_x, int start_y, int end_x, int end_y);
int king_verification(int start_x, int start_y, int end_x, int end_y);

// cette fonction va vérifier les fonctions précédentes et va vérifier si le roi est en échec sur sa position actuelle et activera un flag qui permettra dans les fonctions de vérification de checkmate si le roi est déjà en échec.
int check_echec_verification(int start_x, int start_y, int end_x, int end_y, int i, int j);
int verification_check_on_the_king_or_not(struct positon_of_kings *p);

// cette fonction vérifie si le roi a une possibilité de bouger vers une autre case sans qu'il soit en échec. Si il n'a pas de case où il peut bouger et que le flag_check est activé cela veut dire que le roi est en échec et mat.
int verification_mat_on_the_white_king_or_not(struct positon_of_kings *p, wchar_t piece);
int verification_mat_on_the_black_king_or_not(struct positon_of_kings *p, wchar_t piece);

int verification_it_will_be_check(int start_x, int start_y, int end_x, int end_y, wchar_t piece);

// promotion du pion.
char proposer_promotion_white();
char proposer_promotion_black();

// vérifier si le coup du castling entre le roi et la tour est selon les règles
int verfication_if_it_is_correct_castling(int start_x, int start_y, int end_x, struct positon_of_kings *p);

int flag_correct_moove = 0;
int turn = 1; // 1-white 0-black
int i = 0;
int first_move_pawn = 0; // une ou deux cases en option de mouvement seulement pour son premier coup.

// flags pour vérifier l'échec et mat sur le roi
int flag_echec_king = 0;
int flag_echec_on_white_king = 0;
int flag_echec_on_black_king = 0;

// flags pour la validité du coup de mon passant
int flag_mon_passant_white = 0;
int flag_mon_passant_black = 0;

// vérifier si le castling est selon les règles
int first_move_black_king = 0;
int first_move_black_castle = 0;
int first_move_white_king = 0;
int first_move_white_castle = 0;


//differencier la zone de verification entre une verfication de check et une verfication de mat 
int flag = 0;

int main(void) {
    struct start_position s;
    struct end_position e;
    char choice;
    int current_player = 1; // 1 pour le joueur blanc, 2 pour le joueur noir

    struct positon_of_kings p;
    // Initialisation des membres de la structure
    p.x_position_white_king = 4;
    p.y_position_white_king = 7;
    p.x_position_black_king = 4;
    p.y_position_black_king = 0;

    do {
        printf("\033[H\033[J"); // Clear the terminal
        Show_table();

        if (current_player == 1) {
            printf("\nPlayer 1's turn (white)\n");
        } else {
            printf("\nPlayer 2's turn (black)\n");
        }
        printf("%d", p.x_position_white_king);
        printf("%d", p.y_position_white_king);
        printf("%d", flag_echec_on_white_king);

        printf("Enter the piece that you want to move: ");
        scanf("%s", s.start_position);
        printf("To which position? : ");
        scanf("%s", e.potential_end_position);

        Convertor_Position_to_index_array(s.start_position, &s.absice_position, &s.ordonne_position);
        Convertor_Position_to_index_array(e.potential_end_position, &e.absice_position, &e.ordonne_position);

        if ((current_player == 1 && check_if_it_black_or_white(find_piece(s)) == -1) || (current_player == 2 && check_if_it_black_or_white(find_piece(s)) == 1)) { // cette condition vérifie si le joueur que c'est au tour de jouer a choisi une pièce de sa propre couleur.
            update_board(s, e, &p);

            // on vérifie si les rois ne sont pas en échec pour activer les flags dans les fonctions de vérification de checkmate sur le roi
            verification_check_on_the_king_or_not(&p);

            if (flag_correct_moove != 0) // bon move on vérifie s'il y a mat sur l'un des rois
                if ((verification_mat_on_the_white_king_or_not(&p, white_king) != -1) && (verification_mat_on_the_black_king_or_not(&p, black_king) != -1)) {
                    current_player = (current_player == 1) ? 2 : 1; // Passer au joueur suivant
                } else
                    break;
            else
                printf("\n");

        } else {
            printf("Invalid move. Make sure you move your own pieces.\n");
        }

        printf("\nDo you want to continue? (y/n): ");
        scanf(" %c", &choice); // Notez l'espace avant %c pour consommer le saut de ligne restant

    } while ((choice == 'y' || choice == 'Y'));

    if (verification_mat_on_the_white_king_or_not(&p, white_king) == -1) {
        printf("\033[H\033[J"); // Clear the terminal
        Show_table();
        printf("checkMate\n");
        printf("The Second player won (black)");
    } else if (verification_mat_on_the_black_king_or_not(&p, black_king) == -1) {
        printf("\033[H\033[J"); // Clear the terminal
        Show_table();
        printf("checkMate\n");
        printf("The first player won (white)");
    }

    return 0;
}


void Convertor_Position_to_index_array(char position[3], int *absice_position, int *ordonne_position)
{
    *absice_position = 0;
    *ordonne_position = 0;

    switch (position[0]) {
        case 'a':
        case 'A':
            *absice_position = 0;
            break;
        case 'b':
        case 'B':
            *absice_position = 1;
            break;
        case 'c':
        case 'C':
            *absice_position = 2;
            break;
        case 'd':
        case 'D':
            *absice_position = 3;
            break;
        case 'e':
        case 'E':
            *absice_position = 4;
            break;
        case 'f':
        case 'F':
            *absice_position = 5;
            break;
        case 'g':
        case 'G':
            *absice_position = 6;
            break;
        case 'h':
        case 'H':
            *absice_position = 7;
            break;
        default:
            break;
    }
    // sur les position de l'ordonner (dans quel ligne se situe la case designer par le joueur) puisque les numero commeence du bas en haut et du plus petit au plus grand
    // donc  la ligne numero 1 sur le board chess sera la dernier ligne dans le array donc 8 ligne mais on commence par la ligne Zero dans un array ducoup sa sera la 7 ligne et ainsi suite.
    switch (position[1]) {
        case '1':
            *ordonne_position = 7;
            break;
        case '2':
            *ordonne_position = 6;
            break;
        case '3':
            *ordonne_position = 5;
            break;
        case '4':
            *ordonne_position = 4;
            break;
        case '5':
            *ordonne_position = 3;
            break;
        case '6':
            *ordonne_position = 2;
            break;
        case '7':
            *ordonne_position = 1;
            break;
        case '8':
            *ordonne_position = 0;
            break;
        default:
            break;
    }
}

wchar_t find_piece(struct start_position s)
{
    // cette fonction va recevoir la struct de la start position. (il y'aura que les piece des start qu'on aura besoin de find)

    int absice_position = s.absice_position;
    int ordonne_position = s.ordonne_position;
    
    // puis on verifier si les donner de absice et ordonne se situe dans les limite du plateau d'echec. (puis on return l'unicode de la piece en question)
    if (absice_position >= 0 && absice_position < 8 && ordonne_position >= 0 && ordonne_position < 8) {
        return board[ordonne_position][absice_position];
    } else {
        return L' ';
    }
}
int check_pawn_verification(int start_x, int start_y, int end_x, int end_y); //declaration de la fonction pour le compiler puisse la detecter

int verification_check_on_the_king_or_not(struct positon_of_kings *p)
{
    // cette fonction va faire en sorte de verifier a quel moment le rois est en check et va principalemt etre utile a la fonction qui s'occupe la de verfication des mat sur le echec et aide a diferencier les moments ou le rois ne peut pas bouger comme les debut de partie au moment ou il y'a un vrai echeck mat puisque le rois et deja en echeck cette fonction va activer des global flag qui vont etre recupere dans la fonction de verfication de mat sur les rois 
    if (check_diagonal_verification(p->x_position_white_king,  p->y_position_white_king, p->x_position_white_king,  p->y_position_white_king) == -1 || 
        check_rook_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king,  p->y_position_white_king) == -1 || 
        check_knight_verification(p->x_position_white_king,  p->y_position_white_king, p->x_position_white_king, p->y_position_white_king) == -1  ||  
        check_pawn_verification(p->x_position_white_king,  p->y_position_white_king, p->x_position_white_king,  p->y_position_white_king) == -1) {
        flag_echec_on_white_king = 1;
        flag = 1;
    }
    else if(check_diagonal_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king, p->y_position_black_king) == -1 || 
            check_rook_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king, p->y_position_black_king) == -1 || 
            check_knight_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king, p->y_position_black_king) == -1  ||  
            check_pawn_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king, p->y_position_black_king) == -1) {
        flag_echec_on_black_king = 1;
    }
    else {
        flag_echec_on_black_king = 0;
        flag_echec_on_white_king = 0;
    }
}

int verification_mat_on_the_white_king_or_not(struct positon_of_kings *p ,wchar_t piece)
{
    int count_numbers_of_check = 0;
    // |---------------------------------|
    // |     1     |          |          |
    // |---------------------------------|
    // |          |     k     |          |
    // |---------------------------------|
    // |          |           |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king - 1, p->y_position_white_king - 1) == -1 || 
       king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king - 1, p->y_position_white_king - 1) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |     2     |          |
    // |---------------------------------|
    // |          |     k     |          |
    // |---------------------------------|
    // |          |           |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king, p->y_position_white_king -1) == -1 || 
       king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king, p->y_position_white_king - 1) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |           |    3     |
    // |---------------------------------|
    // |          |     k     |          |
    // |---------------------------------|
    // |          |           |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king + 1, p->y_position_white_king - 1) == -1 || 
       king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king + 1, p->y_position_white_king - 1) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |           |          |
    // |---------------------------------|
    // |          |     k     |     4    |
    // |---------------------------------|
    // |          |           |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king + 1, p->y_position_white_king) == -1 || 
       king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king + 1, p->y_position_white_king) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |           |          |
    // |---------------------------------|
    // |    5     |     k     |          |
    // |---------------------------------|
    // |          |           |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king - 1, p->y_position_white_king) == -1 || 
       king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king - 1, p->y_position_white_king) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |           |          |
    // |---------------------------------|
    // |          |     k     |          |
    // |---------------------------------|
    // |     6    |           |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king - 1, p->y_position_white_king + 1) == -1 || 
       king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king - 1, p->y_position_white_king + 1) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |           |          |
    // |---------------------------------|
    // |          |     k     |          |
    // |---------------------------------|
    // |          |     7     |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king, p->y_position_white_king + 1) == -1 || 
       king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king, p->y_position_white_king + 1) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |           |          |
    // |---------------------------------|
    // |          |     k     |          |
    // |---------------------------------|
    // |          |           |    8     |
    // |----------------|----------------|

    if(king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king + 1, p->y_position_white_king + 1) == -1 || 
       king_verification(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king + 1, p->y_position_white_king + 1) == 0) {
        count_numbers_of_check++;
    }

    if(count_numbers_of_check == 8 && flag_echec_on_white_king == 1)
    {
        return -1;
    }
    else 
       return 1;
}

int verification_mat_on_the_black_king_or_not(struct positon_of_kings *p, wchar_t piece)
{
    int count_numbers_of_check = 0;
    // |---------------------------------|
    // |     1     |          |          |
    // |---------------------------------|
    // |          |     k     |          |
    // |---------------------------------|
    // |          |           |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king - 1, p->y_position_black_king - 1) == -1 || 
       king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king - 1, p->y_position_black_king - 1) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |     2     |          |
    // |---------------------------------|
    // |          |     k     |          |
    // |---------------------------------|
    // |          |           |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king, p->y_position_black_king - 1) == -1 || 
       king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king, p->y_position_black_king - 1) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |           |    3     |
    // |---------------------------------|
    // |          |     k     |          |
    // |---------------------------------|
    // |          |           |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king + 1, p->y_position_black_king - 1) == -1 || 
       king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king + 1, p->y_position_black_king - 1) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |           |          |
    // |---------------------------------|
    // |          |     k     |     4    |
    // |---------------------------------|
    // |          |           |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king + 1, p->y_position_black_king) == -1 || 
       king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king + 1, p->y_position_black_king) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |           |          |
    // |---------------------------------|
    // |    5     |     k     |          |
    // |---------------------------------|
    // |          |           |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king - 1, p->y_position_black_king) == -1 || 
       king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king - 1, p->y_position_black_king) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |           |          |
    // |---------------------------------|
    // |          |     k     |          |
    // |---------------------------------|
    // |     6    |           |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king - 1, p->y_position_black_king + 1 == -1) || 
       king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king - 1, p->y_position_black_king + 1) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |           |          |
    // |---------------------------------|
    // |          |     k     |          |
    // |---------------------------------|
    // |          |     7     |          |
    // |----------------|----------------|

    if(king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king, p->y_position_black_king + 1) == -1 || 
       king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king, p->y_position_black_king + 1) == 0) {
        count_numbers_of_check++;
    }

    // |---------------------------------|
    // |          |           |          |
    // |---------------------------------|
    // |          |     k     |          |
    // |---------------------------------|
    // |          |           |    8     |
    // |----------------|----------------|

    if(king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king + 1, p->y_position_black_king + 1) == -1 || 
       king_verification(p->x_position_black_king, p->y_position_black_king, p->x_position_black_king + 1, p->y_position_black_king + 1) == 0) {
        count_numbers_of_check++;
    }

    if(count_numbers_of_check == 8 && flag_echec_on_black_king == 1) {
        return -1;
    } else {
        return 1;
    }
}

int bishop_verification(int start_x, int start_y, int end_x, int end_y) {
    // cette fonction verifier si il y'a une piece en chemin dans la trajectoir diagonal. je l'utilise pour le fous et la reine.
    // |----------------|----------------|
    // |      |         |        1       |
    // |----------------♝----------------|
    // |      |         |        |       |
    // |----------------|----------------|
    if (start_y > end_y && start_x < end_x) {
        int i = start_x + 1;
        int j = start_y - 1;
        while (i <= end_x && j >= end_y) {
           if (i == end_x && j == end_y) {
                // si oui je verifie qu'il ne sont pas de la meme couleur
                if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                   return 1; // pas de la meme couleur 
                else 
                   return 0; // de la meme couleur donc coup illegal
           } else if (board[j][i] != L' ') {
              return -1; // Il y a une pièce sur le chemin
           }
           i++;
           j--;
    }
    }
    // |----------------|----------------|
    // |      2         |        |       |
    // |----------------♝----------------|
    // |      |         |        |       |
    // |----------------|----------------|
    else if (start_y > end_y && start_x > end_x) {
        int i = start_x - 1;
        int j = start_y - 1;

        while (i >= end_x && j >= end_y) {
           if (i == end_x && j == end_y) {
                // si oui je verifie qu'il ne sont pas de la meme couleur
                if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                  return 1; // pas de la meme couleur 
                else 
                  return 0; // de la meme couleur donc coup illegal
           } else if (board[j][i] != L' ') {
              return -1; // Il y a une pièce sur le chemin
           }
           i--;
           j--;
    }

    }
    // |----------------|----------------|
    // |       |        |        |       |
    // |----------------♝----------------|
    // |       3        |        |       |
    // |----------------|----------------|
    else if (start_y < end_y && start_x > end_x) {
        int i = start_x - 1;
        int j = start_y + 1;

        while (i >= end_x && j <= end_y) {
           if (i == end_x && j == end_y) {
                // si oui je verifie qu'il ne sont pas de la meme couleur
                if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                   return 1; // pas de la meme couleur 
                else 
                   return 0; // de la meme couleur donc coup illegal
           } else if (board[j][i] != L' ') {
              return -1; // Il y a une pièce sur le chemin
           }
           i--;
           j++;
    }

    }
    // |----------------|----------------|
    // |       |        |        |       |
    // |----------------♝----------------|
    // |       |        |        4       |
    // |----------------|----------------|
    else if (start_y < end_y && start_x < end_x) {
        int i = start_x + 1;
        int j = start_y + 1;

        
        while (i <= end_x && j <= end_y) {
           if (i == end_x && j == end_y) {
                // si oui je verifie qu'il ne sont pas de la meme couleur
                if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                   return 1; // pas de la meme couleur 
                else 
                   return 0; // de la meme couleur donc coup illegal
           } else if (board[j][i] != L' ') {
              return -1; // Il y a une pièce sur le chemin
           }
           i++;
           j++;
    }
}



    return 0;
}
int rook_verification(int start_x, int start_y, int end_x, int end_y)
{
    // |---------------------------------|
    // |                1                |
    // |----------------♜----------------|
    // |      |         |        |       |
    // |----------------|----------------|
    if(end_y < start_y)
    {
        if(start_x == end_x)
        {
           int i = start_x;
           int j = start_y - 1;

           while (j >= end_y) {
              if (j == end_y) {
                    // si oui je verifie qu'il ne sont pas de la meme couleur
                    if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                      return 1; // pas de la meme couleur 
                    else 
                      return 0; // de la meme couleur donc coup illegal
              } else if (board[j][i] != L' ') {
                  return -1; // Il y a une pièce sur le chemin
              }
              j--;// j'enleve 1 a l'index y a chaque iteration parseque lorsque je verifie la partie haut du tableau plus je m'aproche du haut plus les index y descent
            }
        }
        else 
         return -1;
    }
    // |----------------|----------------|
    // |       |        |                |
    // |----------------♜       2        |
    // |       |        |                |
    // |----------------|----------------|
    else if(end_x > start_x)
    {
        if(start_y == end_y)
        {
           int i = start_x + 1;
           int j = start_y;

           while (i <= end_x) {
              if (i == end_x) {
                if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                  return 1; 
                else 
                  return 0;
              } else if (board[j][i] != L' ') {
                  return -1; // Il y a une pièce sur le chemin
              }
              i++;// j'ajoute 1 au index x a chaque iteration parsque lorque je verifie la partie droite du board plus on se rapproche du cote droite et s'eloigne du debut plus les index x monte (0,1,2,3)
            }
        }
        else 
         return -1;
    }
    // |----------------|----------------|
    // |                |                |
    // |----------------♜----------------|
    // |                3                |
    // |---------------------------------|
    else if(end_y > start_y)
    {
        if(start_x == end_x)
        {
           int i = start_x;
           int j = start_y + 1;

           while (j <= end_y) {
              if (j == end_y) {
                    // si oui je verifie qu'il ne sont pas de la meme couleur
                    if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                       return 1; // pas de la meme couleur 
                    else 
                       return 0; // de la meme couleur donc coup illegal
              } else if (board[j][i] != L' ') {
                  return -1; // Il y a une pièce sur le chemin
              }
              j++;// j'ajoute 1 au y index a chaque iteration parseque je verifie la partie du bah du board et plus en se rapproche du bas plus les index y monte
            }
        }
        else 
         return -1;
    }
    // |----------------|----------------|
    // |                |                |
    // |        4       ♜---------------|
    // |                |                |
    // |---------------------------------|
    else if(end_x < start_x)
    {
        if(start_y == end_y)
        {
           int i = start_x - 1;
           int j = start_y;

           while (i >= end_x) {
              if (i == end_x) {
                    // si oui je verifie qu'il ne sont pas de la meme couleur
                    if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                        return 1; // pas de la meme couleur 
                    else 
                        return 0; // de la meme couleur donc coup illegal
              } else if (board[j][i] != L' ') {
                  return -1; // Il y a une pièce sur le chemin
              }
              i--;// on enleve 1 au x  a chaque iteration parseque on verifier le cote gauche du 2d array et plus on se rapproche du bord (gauche) plus les index  de x  descende
            }
        }
        else 
         return -1;

    }
}
int pawn_verification(int start_x, int start_y, int end_x, int end_y, wchar_t piece)
{
   if(piece == white_pawn)
   {
    if(end_y == start_y - 1) // le pion peut avant seulement d'une case en avant ducoup on verifie si le end_y et egale au start - 1 ce qui corespond a l'une des  case sur la ligne juste devant la ligne de depart
    {
        //mon passant rule
        if(abs(6 - end_y) == 4)
        {
             if((board[start_y][start_x + 1] == black_pawn && end_x == start_x + 1 && board[end_y][end_x] == L' ') || (board[start_y][start_x - 1] == black_pawn && end_x == start_x - 1 && board[end_y][end_x] == L' '))
             {

                 if(flag_mon_passant_white == start_x - 1  || flag_mon_passant_white == start_x + 1)
                    return 3;
                 else 
                    return -1;
             }

        }
        else if((board[start_y - 1][start_x + 1] != L' ' && end_x == start_x + 1 && end_y != 0) || (board[start_y - 1][start_x - 1] != L' ' && end_x == start_x - 1 && end_y != 0)) // la je verifie  si il y'a des piece en diagonal sur la ligne du dessus
        {
            //sauf dans le cas du end_y != 0 pour differencier entre une prise de piece en diagonal et une promotion de pion de pion apres une prise de piece en diagonal dans le array au index 0
                // si oui je verifie qu'il ne sont pas de la meme couleur
                if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                  return 1; // pas de la meme couleur donc il peut le manger 
                else 
                  return 0; // de la meme couleur donc coup illegal
        }
        else if(board[start_y - 1][start_x] == L' ' && end_y != 0)//apres avoir verifier les case en diagonal  on verifie si la case de devant est vide pour verifier le potentiel deplacement vers la case en questions.
        {//sauf dans le cas du end_y != 0 pour differencier entre une prise de piece en diagonal et une promotion de pion de pion apres une prise de piece en diagonal dans le array au index 0
            if(end_x == start_x)// on sait que le  y postion du pion est sur la ligne d'apres ducoup  on verifier si  il va bouger sur la case de devant et pas en diagonal
               return 1;
            else 
               return -1;
        }
        else if(end_y == 0)
        {
           if(end_x == start_x)
           {
              if(board[end_y][end_x] == L' ')
              {
                return 2;
              }
              else 
                return -1;
           }
           else if (board[start_y - 1][start_x - 1] != L' ' && end_x == start_x - 1)
           {
                // si oui je verifie qu'il ne sont pas de la meme couleur
                if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                  return 2; // pas de la meme couleur donc il peut le manger 
                else 
                  return 0; // de la meme couleur donc coup illegal 
           }
           else if (board[start_y - 1][start_x + 1] != L' ' && end_x == start_x + 1)
           {
                // si oui je verifie qu'il ne sont pas de la meme couleur
                if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                  return 2; // pas de la meme couleur donc il peut le manger 
                else 
                  return 0; // de la meme couleur donc coup illegal 
           }
           else 
               return -1;
        }
        else 
            return -1;
    }
    else if(start_y == 6 && end_y == start_y - 2)
    {
        if(board[start_y - 2][start_x] == L' ')// on verifie si la case deux case devant le pion  est vide puisque dans le cas ou la case n'est pas disponible le mouvenent du pion vers cette case ne sera pas possibles.
        {
            if(end_x == start_x)
            {
               if(board[end_y][end_x + 1] == black_pawn || board[end_y][end_x - 1] == black_pawn)
               {
                  flag_mon_passant_black = end_x;
               }
               return 1;
            }
            else 
               return -1;
        }
        else 
            return -1;
    }
    else 
       return -1;
   }
   else if(piece == black_pawn)
   {
    if(end_y == start_y + 1) // le pion peut avant seulement d'une case en avant ducoup on verifie si le end_y et egale au start - 1 ce qui corespond a l'une des  case sur la ligne juste devant la ligne de depart
    {
        //mon passant rule
        if(abs(end_y - 1) == 4)// si la difference entre la deuxieme ligne du plateau donc le array au index 1 moin le end-y du pion noir faire 4 ce la veux dire que le pion noir a bouger de trois case avant d'efectuer le potentiel coup de monpassant
        {
             if((board[start_y][start_x + 1] == white_pawn && end_x == start_x + 1 && board[end_y][end_x] == L' ') || (board[start_y][start_x - 1] == white_pawn && end_x == start_x - 1 && board[end_y][end_x] == L' '))
             {
                 if(flag_mon_passant_black == start_x - 1  || flag_mon_passant_black == start_x + 1)
                    return 3;
                 else 
                    return -1;
             }
             else 
                return -1;
        }
        else if((board[start_y + 1][start_x + 1] != L' ' && end_x == start_x + 1 && end_y != 7) || (board[start_y + 1][start_x - 1] != L' ' && end_x == start_x - 1 && end_y != 7)) // la je verifie  si il y'a des piece en diagonal sur la ligne du dessus
        {
            //sauf dans le cas du end_y != 7 pour differencier entre une prise de piece en diagonal et une promotion de pion de pion apres une prise de piece en diagonal dans le array au index 7
                // si oui je verifie qu'il ne sont pas de la meme couleur
                if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                  return 1; // pas de la meme couleur donc il peut le manger 
                else 
                  return 0; // de la meme couleur donc coup illegal

        }
        else if(board[start_y + 1][start_x] == L' ' && end_y != 7)
        {
            //sauf dans le cas du end_y != 7 pour differencier entre une prise de piece en diagonal et une promotion de pion de pion apres une prise de piece en diagonal dans le array au index 7
            if(end_x == start_x)// on sait que le  y postion du pion est sur la ligne d'apres ducoup  on verifier si  il va bouger sur la case de devant et pas en diagonal
               return 1;
            else 
               return -1;
        }
        else if(end_y == 7)
        {
           if(end_x == start_x)
           {
              if(board[end_y][end_x] == L' ')
              {
                return 2;
              }
              else 
                return -1;
           }
           else if (board[start_y + 1][start_x - 1] != L' ' && end_x == start_x - 1)
           {
                // si oui je verifie qu'il ne sont pas de la meme couleur
                if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                  return 2; // pas de la meme couleur donc il peut le manger 
                else 
                  return 0; // de la meme couleur donc coup illegal 
           }
           else if (board[start_y + 1][start_x + 1] != L' ' && end_x == start_x + 1)
           {
                // si oui je verifie qu'il ne sont pas de la meme couleur
                if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                  return 2; // pas de la meme couleur donc il peut le manger 
                else 
                  return 0; // de la meme couleur donc coup illegal 
           }
           else 
               return -1;
        }
        else 
            return -1;
    }
    else if(start_y == 1  && end_y == start_y + 2)// si c'est le premier changement de position pour le pion choisi le joeur a la posibilite de bouger de deux case en avant 
    {
        if(board[start_y + 2][start_x] == L' ')// on verifie si la case deux case devant le pion  est vide.
        {
            if(end_x == start_x)
            {
               if(board[end_y][end_x + 1] == white_pawn || board[end_y][end_x - 1] == white_pawn)// si quand je bouge le pion blanc il y'a un pion noir a droite ou a gauche je met dans le flag la valuer du end-x pour le cas ou le jouer prochain voudrai effectuer le coup de mon passant et donc je pourais verfier grace au flag si il s'agit de la bonne end-x
               {
                  flag_mon_passant_white = end_x;
               }
               return 1;
            }
            else 
               return -1;
        }
        else 
            return -1;
    }
    else 
       return -1;
     
   }
}

int knight_verification(int start_x, int start_y, int end_x, int end_y)
{
    // |---------------------------------|
    // |              - | -              |
    // |----------------♞---------------|
    // |      |         |        |       |
    // |----------------|----------------|
   if(start_y > end_y)
   {
      if(end_x == start_x - 1 ||  end_x == start_x + 1)
      {
        if(start_y - end_y == 2)
        {
            // si oui je verifie qu'il ne sont pas de la meme couleur
            if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                return 1; // pas de la meme couleur donc coup legal 
            else 
                return 0;  // de la meme couleur donc coup illegal (un cavalier ne peut pas manger un pion de la meme couleur)
        }
        else 
            return -1;
      }
      else if(end_y == start_y - 1)
      {
        if(abs(end_x - start_x) == 2)
        {
            // si oui je verifie qu'il ne sont pas de la meme couleur
            if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                return 1; // pas de la meme couleur donc coup legal 
            else 
                return 0; // de la meme couleur donc coup illegal (un cavalier ne peut pas manger un pion de la meme couleur)
        }
        else return -1;
      }
      else 
         return -1;
   }
    // |---------------------------------|
    // |                                 |
    // |----------------♞---------------|
    // |              - | -              |
    // |----------------|----------------|
   else if(end_y > start_y)
   {
      if(end_x == start_x - 1 ||  end_x == start_x + 1)
      {
        if(end_y  -  start_y == 2)
        {
            // si oui je verifie qu'il ne sont pas de la meme couleur
            if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                return 1; // pas de la meme couleur donc coup legal
            else 
                return 0;  // de la meme couleur donc coup illegal (un cavalier ne peut pas manger un pion de la meme couleur)
        }
        else 
            return -1;
      }
      else if(end_y == start_y +  1)
      {
        if(abs(end_x - start_x) == 2)
        {
            // si oui je verifie qu'il ne sont pas de la meme couleur
            if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2)// si la couleur du pion a l'arrive est la meme que la piece de depart il ne peut pas le manger puisque il sont de la meme couleur 
                return 1; // pas de la meme couleur  donc coup legal 
            else 
                return 0; // de la meme couleur donc coup illegal (un cavalier ne peut pas manger un pion de la meme couleur)
        }
        else return -1;
      }
      else 
         return -1;
   }


}
int check_diagonal_verification(int start_x, int start_y, int end_x, int end_y)
{
    
    // cette fonction va recevoir les start et en position puis va aller a la end postion et verifier toute les diagonal pour verifier si il y'a ps de fous ou reinE sur les traj diagonal si il y'a le rois ne pourra pas bouger vers cette case puisque sa sera un echec

    // |----------------|----------------|
    // |      |         |        ♝      |
    // |----------------♚---------------|
    // |      |         |        |       |
    // |----------------|----------------|

            int i;
            int j;

           if(flag == 0)
           {
             i = end_x + 1;
             j = end_y - 1;
           }
           else 
           {
             i = end_x + 2;
             j = end_y - 2;
           }

            

            
            while (i <= 7 && j >= 0) {
               if((board[j][i] ==  black_bishop || board[j][i] ==  white_bishop)  || (board[j][i] ==  black_queen || board[j][i] ==  white_queen))// ce sont tout les pion qui peuvent cause un echec au rois en diagonal
               {

                    if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[j][i])) != 2)//et la verrifier si le piece en diagonal qui peut mettre en echec le rois n'est pas de la meme couleur parseque si elle est de la meme couleur ce n'est tout simplement pas echec
                    {
                        return -1; //  pas de la  meme couleur =>  echec sur le rois par le fou  ou la reine  adverse.
                    }
                    else 
                        break;// si il y'a une piece en chemin qui de la meme couleur que le rois en question sa se ne sert a rien de continuier a verfier sur cette trajectoir puisque le echec est impossible sur se coup


                }
                else if(board[j][i] != L' ')// si en chemin il n'y pas de fou  ou reine  mais une autre piece peut importe si sur cette diagonal il y'a un fou qui juste deriere une autre piece sa ne mettra pas en echec le rois adverse du coup on sort de la loop parseque sur cette diagonal la verfication se termine 
                   break;
                i++;
                j--;      
            }
    // |----------------|----------------|
    // |     ♝         |        |       |
    // |----------------♚---------------|
    // |      |         |        |       |
    // |----------------|----------------|

            int i2;
            int j2;

           if(flag == 0)
           {
             i2 = end_x - 1;
             j2 = end_y - 1;
           }
           else 
           {
             i2 = end_x - 2;
             j2 = end_y - 2;
           }

            while (i2 >= 0 && j2 >= 0) {
               if((board[j2][i2] ==  black_bishop || board[j2][i2] ==  white_bishop)  || (board[j2][i2] ==  black_queen || board[j2][i2] ==  white_queen))// ce sont tout les pion qui peuvent cause un echec au rois en diagonal
               {  
                    if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[j2][i2])) != 2)//et la verrifier si le piece en diagonal qui peut mettre en echec le rois n'est pas de la meme couleur parseque  si elle est de la meme couleur ce n'est tout simplement pas echec
                    {
                        return -1; //  pas de la meme couleur =>  echec sur le rois par le fou ou la reine  adverse.
                    }
                    else
                      break;// si il y'a une piece en chemin qui de la meme couleur que le rois en question sa se ne sert a rien de continuier a verfier sur cette trajectoir puisque le echec est impossible sur se coup

                }
                else if(board[j2][i2] != L' ')// si en chemin il n'y pas de fou  ou reine  mais une autre piece peut importe si sur cette diagonal il y'a un fou qui juste deriere une autre piece sa ne mettra pas en echec le rois adverse du coup on sort de la loop parseque sur cette diagonal la verfication se termine
                   break;
                i2--;
                j2--;      
            }
    // |----------------|----------------|
    // |       |        |        |       |
    // |----------------♚---------------|
    // |       ♝       |        |       |
    // |----------------|----------------|
            int i3;
            int j3;

           if(flag == 0)
           {
             i3 = end_x - 1;
             j3 = end_y + 1;
           }
           else 
           {
             i3 = end_x - 2;
             j3 = end_y + 2;
           }
            while (i3 >= 0 && j3 <= 7) {
               
               if((board[j3][i3] ==  black_bishop || board[j3][i3] ==  white_bishop)  || (board[j3][i3] ==  black_queen || board[j3][i3] ==  white_queen))// ce sont tout les pion qui peuvent cause un echec au rois en diagonal
               {
                    if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[j3][i3])) != 2)//et la JE  verrifie si la piece en diagonal qui peut mettre en echec le rois n'est pas de la meme couleur parseque si  elle est de la meme couleur ce n'est tout simplement pas echec
                    {
                        return  -1; //  pas de la meme couleur =>  echec sur le rois par le fou  reine adverse.
                    }
                    else
                      break;// si il y'a une piece en chemin qui de la meme couleur que le rois en question sa se ne sert a rien de continuier a verfier sur cette trajectoir puisque le echec est impossible sur se coup

                }
                else if(board[j3][i3] != L' ')// si en chemin il n'y pas de fou  ou reine  mais une autre piece peut importe si sur cette diagonal il y'a un fou qui juste deriere une autre piece sa ne mettra pas en echec le rois adverse du coup on sort de la loop parseque sur cette diagonal la verfication se termine
                   break;
                i3--;
                j3++;      
            }
    // |----------------|----------------|
    // |       |        |        |       |
    // |----------------♚---------------|
    // |       |        |        ♝      |
    // |----------------|----------------|
            
            int i4;
            int j4;

           if(flag == 0)
           {
             i4 = end_x + 1;
             j4 = end_y + 1;
           }
           else 
           {
             i4 = end_x + 2;
             j4 = end_y + 2;
           }

            

            
            while (i4 <=7 && j4 <= 7) {
               
               if((board[j4][i4] ==  black_bishop || board[j4][i4] ==  white_bishop)  || (board[j4][i4] ==  black_queen || board[j4][i4] ==  white_queen))// ce sont tout les pion qui peuvent cause un echec au rois en diagonal
               {
                  if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[j4][i4])) != 2)//et la verrifier si le piece en diagonal qui peut mettre en echec le rois n'est pas de la meme couleur parseque si  elle est de la meme couleur ce n'est tout simplement pas echec
                  {
                    return  -1; //  pas de la meme couleur =>  echec sur le rois par le fou ou la reine adverse.
                  }
                  else 
                    break;// si il y'a une piece en chemin qui de la meme couleur que le rois en question sa se ne sert a rien de continuier a verfier sur cette trajectoir puisque le echec est impossible sur se coup

                }
                else if(board[j4][i4] != L' ')// si en chemin il n'y pas de fou  ou reine  mais une autre piece peut importe si sur cette diagonal il y'a un fou qui juste deriere une autre piece sa ne mettra pas en echec le rois adverse du coup on sort de la loop parseque sur cette diagonal la verfication se termine
                   break;
                i4++;
                j4++;      
            }
            return 1; // Il y a pas  une pièce sur tout les diagonal de la end postion qui peuvent mettre le rois en echec en cas de deplacement vers cette case du coup le rois peut ce deplacer vers celle ci 
 
}
int check_rook_verification(int start_x, int start_y, int end_x, int end_y)
{
    // |---------------------------------|
    // |                ♜               |
    // |----------------♚---------------|
    // |      |         |        |       |
    // |----------------|----------------|
    int i;
    int j;

    if(flag == 0)
    {
      i = end_x;
      j = end_y - 1;
    }
    else 
    {
     i = end_x;
     j = end_y - 2;
    }
    while (j >= 0) {

        if((board[j][i] ==  black_rook || board[j][i] ==  white_rook) || (board[j][i] ==  black_queen || board[j][i] ==  white_queen))// ce sont tout les pion qui peuvent cause un echec au rois en horizontals
        {
            if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[j][i])) != 2)//et la verrifier si la piece en vertical qui peut mettre en echec le rois n'est pas de la meme couleur parseque si  elle est de la meme couleur ce n'est tout simplement pas echec
            {
                return  -1; //  pas de la meme couleur =>  echec sur le rois par la tour ou reine  adverse.
            }
            else 
                break;// si il y'a une piece en chemin qui de la meme couleur que le rois en question sa se ne sert a rien de continuier a verfier sur cette trajectoir puisque le echec est impossible sur cette trajectoire

        }
        else if(board[j][i] != L' ')// si en chemin il n'y pas de rois ou reine mais une autre piece peut importe si sur cette vertical il y'a une tour ou reine qui juste deriere une autre piece se ne mettra pas en echec le rois adverse du coup on sort de la loop parseque sur cette vertical la verfication se termine 
                break;
        j--;// j'enleve 1 a l'index y a chaque iteration parseque lorsque je verifie la partie haut du tableau plus je m'aproche du haut plus les index y descent
    }

    // |----------------|----------------|
    // |       |        |                |
    // |----------------♚-------♜      |
    // |       |        |                |
    // |----------------|----------------|

    int i2;
    int j2;

    if(flag == 0)
    {
      i2 = end_x + 1;
      j2 = end_y;
    }
    else 
    {
     i2 = end_x + 2;
     j2 = end_y;
    }

    while (i2 <= 7) {

        if((board[j2][i2] ==  black_rook || board[j2][i2] ==  white_rook) || (board[j2][i2] ==  black_queen || board[j2][i2] ==  white_queen))// ce sont tout les pion qui peuvent cause un echec au rois en horizontals
        {
           if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[j2][i2])) != 2)//et la verrifier si la piece en vertical qui peut mettre en echec le rois n'est pas de la meme couleur parseque si  elle est de la meme couleur ce n'est tout simplement pas echec
           {
                return  -1; //  pas de la meme couleur =>  echec sur le rois par la tour ou reine  adverse.
           }
           else 
                break;// si il y'a une piece en chemin qui de la meme couleur que le rois en question sa se ne sert a rien de continuier a verfier sur cette trajectoir puisque le echec est impossible sur cette trajectoire

        }
        else if(board[j2][i2] != L' ')// si en chemin il n'y pas de rois ou reine mais une autre piece peut importe si sur cette vertical il y'a une tour ou reine qui juste deriere une autre piece se ne mettra pas en echec le rois adverse du coup on sort de la loop parseque sur cette vertical la verfication se termine 
                   break;
        i2++;// j'enleve 1 a l'index y a chaque iteration parseque lorsque je verifie la partie haut du tableau plus je m'aproche du haut plus les index y descent
    }
    // |----------------|----------------|
    // |                |                |
    // |----------------♚----------------|
    // |                ♜                |
    // |---------------------------------|
    int i3;
    int j3;

    if(flag == 0)
    {
      i3 = end_x;
      j3 = end_y + 1;
    }
    else 
    {
     i3 = end_x;
     j3 = end_y + 2;
    }

    while (j3 <= 7) {

        if((board[j3][i3] ==  black_rook || board[j3][i3] ==  white_rook) || (board[j3][i3] ==  black_queen || board[j3][i3] ==  white_queen))// ce sont tout les pion qui peuvent cause un echec au rois en horizontals
        {
                if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[j3][i3])) != 2)//et la verrifier si la piece en vertical qui peut mettre en echec le rois n'est pas de la meme couleur parseque si  elle est de la meme couleur ce n'est tout simplement pas echec
                {
                    return  -1; //  pas de la meme couleur =>  echec sur le rois par la tour ou reine  adverse.
                }
                else 
                    break;// si il y'a une piece en chemin qui est  de la meme couleur que le rois en question sa se ne sert a rien de continuier a verfier sur cette trajectoir puisque le echec est impossible sur cette trajectoire
        }
        else if(board[j3][i3] != L' ')// si en chemin il n'y pas de rois ou reine mais une autre piece peut importe si sur cette vertical il y'a une tour ou reine qui juste deriere une autre piece se ne mettra pas en echec le rois adverse du coup on sort de la loop parseque sur cette vertical la verfication se termine 
                   break;
        j3++;// j'enleve 1 a l'index y a chaque iteration parseque lorsque je verifie la partie haut du tableau plus je m'aproche du haut plus les index y descent
    }
    // |----------------|----------------|
    // |                |                |
    // |        ♜      ♚----------------|
    // |                |                |
    // |---------------------------------|
    int i4 = end_x - 1;
    int j4 = end_y;

    if(flag == 0)
    {
      i4;
      j4;
    }
    else 
    {
     i4 = end_x - 2;
     j4 = end_y;
    }

    while (i4 >= 0) {

        if((board[j4][i4] ==  black_rook || board[j4][i4] ==  white_rook) || (board[j4][i4] ==  black_queen || board[j4][i4] ==  white_queen))// ce sont tout les pion qui peuvent cause un echec au rois en horizontals
        {
            if(abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[j4][i4])) != 2)//et la verrifier si la piece en vertical qui peut mettre en echec le rois n'est pas de la meme couleur parseque si  elle est de la meme couleur ce n'est tout simplement pas echec
            {
                return  -1; //  pas de la meme couleur =>  echec sur le rois par la tour ou reine  adverse.
            }
            else // si il y'a une piece en chemin qui de la meme couleur que le rois en question sa se ne sert a rien de continuier a verfier sur cette trajectoir puisque le echec est impossible sur cette trajectoire
                break;

        }
        else if(board[j4][i4] != L' ')// si en chemin il n'y pas de rois ou reine mais une autre piece peut importe si sur cette vertical il y'a une tour ou reine  qui juste deriere une autre piece se ne mettra pas en echec le rois adverse du coup on sort de la loop parseque sur cette vertical la verfication se termine 
            break;
        i4--;// j'enleve 1 a l'index y a chaque iteration parseque lorsque je verifie la partie haut du tableau plus je m'aproche du haut plus les index y descent
    }
    
    return 1;
    

    
}
int check_echec_verification(int start_x, int start_y, int end_x, int end_y, int i, int j)
{
    int start_color = check_if_it_black_or_white(board[start_y][start_x]);
    int end_color = check_if_it_black_or_white(board[j][i]);

    if (start_color * end_color != -1)
    {
        return 1; // Les pièces sont de la même couleur, donc ce n'est pas une mise en échec du roi.
    }
    else
    {
        return -1; // Les pièces sont de couleurs différentes, ce qui peut mettre le roi en échec.
    }
}


int check_knight_verification(int start_x, int start_y, int end_x, int end_y)
{
    // cette fonction va verifier si la case vers la quelle le rois veut se diriger n'est pas une case qui peut se mettre en echec par un chevalier
    // |---------------------------------|
    // |              - | -              |
    // |----------------♞----------------|
    // |      |         |        |       |
    // |----------------|----------------|
   if((board[end_y - 2][end_x - 1] == black_knight || board[end_y - 2][end_x - 1] == white_knight))
   {
      if(check_echec_verification(start_x, start_y, end_x, end_y, end_y - 2,end_x - 1) == -1)
        return -1;
      else 
        return 1;
   }
   else if((board[end_y - 2][end_x + 1] == black_knight || board[end_y - 2][end_x + 1] == white_knight))
   {
     if(check_echec_verification(start_x, start_y, end_x, end_y, end_y - 2,end_x + 1) == -1)
        return -1;
     else 
        return 1;
   }
   else if((board[end_y - 1][end_x - 2] == black_knight || board[end_y - 1][end_x - 2] == white_knight))
   {
     if(check_echec_verification(start_x, start_y, end_x, end_y, end_y - 1,end_x  - 2) == -1)
        return -1;
     else 
        return 1;
   }
   else if((board[end_y - 1][end_x + 2] == black_knight || board[end_y - 1][end_x + 2] == white_knight))
   {
    if(check_echec_verification(start_x, start_y, end_x, end_y, end_y - 1,end_x + 2) == -1)
        return -1;
     else 
        return 1;
   }
    // |---------------------------------|
    // |                                 |
    // |----------------♞----------------|
    // |              - | -              |
    // |----------------|----------------|
   else if((board[end_y + 2][end_x - 1] == black_knight || board[end_y + 2][end_x - 1] == white_knight))
   {
     if(check_echec_verification(start_x, start_y, end_x, end_y, end_y + 2,end_x - 1) == -1)
        return -1;
     else 
        return 1;
   }
   else if((board[end_y + 2][end_x + 1] == black_knight || board[end_y + 2][end_x + 1] == white_knight))
   {
     if(check_echec_verification(start_x, start_y, end_x, end_y, end_y + 2,end_x + 1) == -1)
        return -1;
     else 
        return 1;
   }
   else if(board[end_y + 1][end_x - 2] == black_knight || board[end_y + 1][end_x - 2] == white_knight)
   {
     if(check_echec_verification(start_x, start_y, end_x, end_y, end_y + 1,end_x - 2) == -1)
        return -1;
     else 
        return 1;
   }
   else if(board[end_y + 1][end_x + 2] == black_knight || board[end_y + 1][end_x + 2] == white_knight)
   {
     if(check_echec_verification(start_x, start_y, end_x, end_y, end_y + 1,end_x + 2) == -1)
        return -1;
     else 
        return 1;
   }
   else 
      return 1;
}
int check_pawn_verification(int start_x, int start_y, int end_x, int end_y)
{
        // les diagnal du cote haut du plateau peuvent etre une potentiel menace pour le rois que dans le cas ou se sont des pions de couleur blanche parseque......DOIT CONTINUER
        if(board[end_y + 1][end_x + 1] == white_pawn)
        {
            if(check_echec_verification(start_x, start_y, end_x, end_y, end_y + 1,end_x + 1) == -1)
               return -1;
            else 
               return 1;
        }
        else if(board[end_y + 1][end_x - 1] == white_pawn)
        {
            if(check_echec_verification(start_x, start_y, end_x, end_y, end_y + 1,end_x - 1) == -1)
               return -1;
            else 
               return 1;
        }


        if(board[end_y - 1][end_x -  1] == black_pawn)
        {
            if(check_echec_verification(start_x, start_y, end_x, end_y, end_y - 1,end_x - 1) == -1)
               return -1;
            else 
               return 1;
        }
        else if(board[end_y - 1][end_x  +  1] == black_pawn)
        {
            if(check_echec_verification(start_x, start_y, end_x, end_y, end_y - 1,end_x + 1) == -1)
               return -1;
            else 
               return 1;
        }

}
int verfication_if_it_is_correct_castling(int start_x, int start_y, int end_x, struct positon_of_kings *p)
{
    if(board[start_y][start_x] == white_king)
    {
        if(first_move_white_castle == 0 && first_move_white_king == 0) // first rule - it has to be the first move for both
        {
            if(flag_echec_on_black_king == 0 && end_x > start_x && board[7][7] == white_rook) // the second rule - the king needs to not be in check / je verifie ensuite si le rook du cote kingside est a sa place pour proceder au potentiel castle 
            {
                int i = start_x + 1;
                while(i <= 6) // king side (à droite du roi)
                {
                    if(check_diagonal_verification(p->x_position_white_king, p->y_position_white_king, i, 7) == -1 || 
                       check_rook_verification(p->x_position_white_king, p->y_position_white_king, i, 7) == -1 || 
                       check_knight_verification(p->x_position_white_king, p->y_position_white_king, i, 7) == -1 || 
                       check_pawn_verification(p->x_position_white_king, p->y_position_white_king, i, 7) == -1) 
                    {
                        return -1;
                    }
                    if(board[start_y][i] != L' ')
                        return -1;
                    i++;
                }
                return 1; // good castle
            }
            else if(flag_echec_on_black_king == 0 && end_x < start_x && board[7][0] == white_rook)// the second rule - the king needs to not be in check / je verifie ensuite si le rook du cote queen side est a sa place pour proceder au potentiel castle
            {
                int j = start_x - 1;
                while(j >= 1) // Queen side (à gauche du roi)
                {
                    if(check_diagonal_verification(p->x_position_white_king, p->y_position_white_king, j, 7) == -1 || 
                       check_rook_verification(p->x_position_white_king, p->y_position_white_king, j, 7) == -1 || 
                       check_knight_verification(p->x_position_white_king, p->y_position_white_king, j, 7) == -1 || 
                       check_pawn_verification(p->x_position_white_king, p->y_position_white_king, j, 7) == -1) 
                    {
                        return -1;
                    }
                    if(board[start_y][j] != L' ')
                        return -1;
                    j--;
                }
                return 1; // good castle
            }
            else 
                return -1;
        }
        else 
            return -1;
    }
    else if(board[start_y][start_x] == black_king)
    {
        if(first_move_white_castle == 0 && first_move_white_king == 0) // first rule - it has to be the first move for both.
        {
            if(flag_echec_on_black_king == 0 && end_x > start_x && board[0][7] == black_rook) // the second rule - the king needs to not be in check / je verifie ensuite si le rook du cote kingside est a sa place pour proceder au potentiel castle 
            {
                int i2 = start_x + 1;
                while(i2 <= 6) // king side (à droite du roi)
                {
                    if(check_diagonal_verification(p->x_position_black_king, p->y_position_black_king, i2, 0) == -1 || 
                       check_rook_verification(p->x_position_black_king, p->y_position_black_king, i2, 0) == -1 || 
                       check_knight_verification(p->x_position_black_king, p->y_position_black_king, i2, 0) == -1 || 
                       check_pawn_verification(p->x_position_black_king, p->y_position_black_king, i2, 0) == -1) 
                    {
                        return -1;
                    }
                    if(board[start_y][i2] != L' ')
                        return -1;
                    i2++;
                }
                return 1; // good castle
            }
            else if(flag_echec_on_black_king == 0 && end_x < start_x && board[0][0] == black_rook )// the second rule - the king needs to not be in check / je verifie ensuite si le rook du cote queen side est a sa place pour proceder au potentiel castle
            {
                int j2 = start_x - 1;
                while(j2 >= 1) // Queen side (à gauche du roi)
                {
                    if(check_diagonal_verification(p->x_position_black_king, p->y_position_black_king, j2, 0) == -1 || 
                       check_rook_verification(p->x_position_black_king, p->y_position_black_king, j2, 0) == -1 || 
                       check_knight_verification(p->x_position_black_king, p->y_position_black_king, j2, 0) == -1 || 
                       check_pawn_verification(p->x_position_black_king, p->y_position_black_king, j2, 0) == -1) 
                    {
                        return -1;
                    }
                    if(board[start_y][j2] != L' ')
                        return -1;
                    j2--;
                }
                return 1; // good castle
            }
            else 
                return -1;
        }
        else 
            return -1;
    }
}


int king_verification(int start_x, int start_y, int end_x, int end_y) {
    // vérifier si les positions données ne sont pas hors limite du plateau
    if (end_x >= 0 && end_x < 8 && end_y >= 0 && end_y < 8) {
        // vérifier si le mouvement du roi est dans une case adjacente
        if ((abs(end_x - start_x) <= 1) && (abs(end_y - start_y) <= 1)) {
            // vérifier si la pièce n'est pas de la même couleur
            if (abs(check_if_it_black_or_white(board[start_y][start_x]) + check_if_it_black_or_white(board[end_y][end_x])) != 2) {
                // vérifier s'il n'y a pas d'échec sur la trajectoire
                if (check_diagonal_verification(start_x, start_y, end_x, end_y) == -1 || 
                    check_rook_verification(start_x, start_y, end_x, end_y) == -1 || 
                    check_knight_verification(start_x, start_y, end_x, end_y) == -1 ||  
                    check_pawn_verification(start_x, start_y, end_x, end_y) == -1) {
                    return -1; // échec sur la trajectoire
                } else {
                    return 1; // mouvement légal
                }
            } else {
                return 0; // pièces de la même couleur, mouvement illégal
            }
        } else if ((abs(end_x - start_x) == 2) && end_y == start_y  && (end_y == 7 || end_y == 0)) {
            return 3; // tentative de roque
        } else {
            return -1; // mouvement non valide pour le roi
        }
    } else {
        return -1; // position finale hors limite
    }
}


int verification_it_will_be_check(int start_x, int start_y, int end_x, int end_y, wchar_t piece) {
    //cette fonction va faire prendre en consideration que  un deplacemnt de chaque piece coller au rois vers une autre case qui peuvent provoque un potentiel echec sur le rois  par des fou, reine, tour, comme coup illegal.

     //si la piece n'est pas de la meme couleur du rois sur la traj en question il pourra mettre en echec le rois adverse c'est que une piece provoque un echec a son rois du a sont deplacement
    if (piece == black_knight || piece == white_knight) {
        if(check_if_it_black_or_white(board[end_y][end_x]) != check_if_it_black_or_white(piece))
        {
            return 1;
        }
        else if ((check_diagonal_verification(start_x, start_y, end_x, end_y) == -1 || check_rook_verification(start_x, start_y, end_x, end_y) == -1)) { //je verifie les trajectoire diagonal, vertical et horizontal que dans le cas ou la piece bouger va provoquer un  echec sur le rois de sa couleur ce qui est un coup illegal
            return -1;
        } else {
            return 1;
        }
    } else if (piece == black_queen || piece == white_queen) {
        //si la piece n'est pas de la meme couleur du rois sur la traj en question il pourra mettre en echec le rois adverse c'est que dans le cas une piece provoque un echec a son rois du a sont deplacement
        if(check_if_it_black_or_white(board[end_y][end_x]) != check_if_it_black_or_white(piece))
        {
            return 1;
        }
        else if ((check_diagonal_verification(start_x, start_y, end_x, end_y) == -1 || check_rook_verification(start_x, start_y, end_x, end_y) == -1)) { //je verifie les trajectoire diagonal, vertical et horizontal que dans le cas ou la piece bouger va provoquer un  echec sur le rois de sa couleur ce qui est un coup illegal
            return -1;
        } else {
            return 1;
        }
    } else if (piece == black_bishop || piece == white_bishop) {
         //si la piece n'est pas de la meme couleur du rois sur la traj en question il pourra mettre en echec le rois adverse c'est que dans le cas une piece provoque un echec a son rois du a sont deplacement
        if(check_if_it_black_or_white(board[end_y][end_x]) != check_if_it_black_or_white(piece))
        {
            return 1;
        }
        else if ((check_diagonal_verification(start_x, start_y, end_x, end_y) == -1 || check_rook_verification(start_x, start_y, end_x, end_y) == -1)) { //je verifie les trajectoire diagonal, vertical et horizontal que dans le cas ou la piece bouger va provoquer un  echec sur le rois de sa couleur ce qui est un coup illegal
            return -1;
        } else {
            return 1;
        }
    } else if (piece == black_rook || piece == white_rook) {
         //si la piece n'est pas de la meme couleur du rois sur la traj en question il pourra mettre en echec le rois adverse c'est que dans le cas  une piece provoque un echec a son rois du a sont deplacement
        if(check_if_it_black_or_white(board[end_y][end_x]) != check_if_it_black_or_white(piece))
        {
            return 1;
        }
        else if ((check_diagonal_verification(start_x, start_y, end_x, end_y) == -1 || check_rook_verification(start_x, start_y, end_x, end_y) == -1)) { //je verifie les trajectoire diagonal, vertical et horizontal que dans le cas ou la piece bouger va provoquer un  echec sur le rois de sa couleur ce qui est un coup illegal
            return -1;
        } else {
            return 1;
        }
    } 
    else if (piece == black_pawn|| piece == white_pawn) {
        //si la piece n'est pas de la meme couleur du rois sur la traj en question il pourra mettre en echec le rois adverse c'est que dans le cas  une piece provoque un echec a son rois du a sont deplacement
        if(check_if_it_black_or_white(board[end_y][end_x]) != check_if_it_black_or_white(piece))
        {
            return 1;
        }
        else if ((check_diagonal_verification(start_x, start_y, end_x, end_y) == -1 || check_rook_verification(start_x, start_y, end_x, end_y) == -1)) {//je verifie les trajectoire diagonal, vertical et horizontal que dans le cas ou la piece bouger va provoquer un  echec sur le rois de sa couleur ce qui est un coup illegal
            return -1; 
        } else {
            return 1;
        }
    } else {
        // Ajoutez une valeur de retour par défaut ici
        return 0; // Par exemple
    }
}
char proposer_promotion_white() {
    int choix;
    printf("Your pawn had been promoted. Choose an option:\n");
    printf("1: Queen\n");
    printf("2: Rook\n");
    printf("3: Bishop\n");
    printf("4: Knight\n");

    scanf("%d", &choix);

    switch (choix) {
        case 1: return white_queen;  // Reine
        case 2: return white_rook;  // Tour
        case 3: return white_bishop;  // Fou
        case 4: return white_knight;  // Cavalier
        default: return white_queen; // Par défaut, promouvoir à Reine
    }
}

char proposer_promotion_black() {
    int choix;
    printf("Your pawn had been promoted. Choose an option:\n");
    printf("1: Queen\n");
    printf("2: Rook\n");
    printf("3: Bishop\n");
    printf("4: Knight\n");

    scanf("%d", &choix);

    switch (choix) {
        case 1: return black_queen;  // Reine
        case 2: return black_rook;  // Tour
        case 3: return black_bishop;  // Fou
        case 4: return black_knight;  // Cavalier
        default: return black_queen; // Par défaut, promouvoir à Reine
    }
}

void update_board(struct start_position s, struct end_position e, struct positon_of_kings *p) {
    int start_x = s.absice_position;
    int start_y = s.ordonne_position;
    int end_x = e.absice_position;
    int end_y = e.ordonne_position;
    char choice;

    wchar_t piece = find_piece(s);
    printf("Piece at start position: %lc\n", piece);

    if (piece == white_bishop || piece == black_bishop) {
        // Verification du mouvement diagonal
        if (abs(start_x - end_x) == abs(start_y - end_y)) {
            // Appel a la fonction de verification specifique au fou
            if (bishop_verification(start_x, start_y, end_x, end_y) == 1) {
                   // Deplacement provisoire  de la piece en attente de verfication
                    board[end_y][end_x] = board[start_y][start_x];
                    board[start_y][start_x] = L' ';

                    if(verification_it_will_be_check(p->x_position_white_king,p->y_position_white_king, p->x_position_white_king, p->y_position_white_king, piece) != 1 || verification_it_will_be_check( p->x_position_black_king,  p->y_position_black_king,  p->x_position_black_king,  p->y_position_black_king, piece) != 1)
                    {
                        //retour a l'etat initial puisque le coup en question est illegal
                        board[start_y][start_x] = board[end_y][end_x];
                        board[end_y][end_x] = L' ';
                        printf("it's will be check.\n");
                        flag_correct_moove = 0;
                    }
                    else 
                    {
                        printf("Piece moved successfully.\n");
                        flag_correct_moove = 1;
                    } 
            } 
            else if(bishop_verification(start_x, start_y, end_x, end_y) == 0)
            {
                printf("Invalid move for the knight: You cannot eat a piece of your color\n");
                flag_correct_moove = 0;
            }
            else {
                printf("Invalid move for the bishop: there is a piece in your way\n");
                flag_correct_moove = 0;
            }
        } else {
            printf("Invalid move for the bishop: not a diagonal move.\n");
            flag_correct_moove = 0;
            
        }
    }
    else if(piece == white_rook || piece == black_rook) 
    {
         if (rook_verification(start_x, start_y, end_x, end_y) == 1) {
            // Deplacement provisoire  de la piece en attente de verfication
            board[end_y][end_x] = board[start_y][start_x];
            board[start_y][start_x] = L' ';

            if(verification_it_will_be_check(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king, p->y_position_white_king, piece) != 1 || verification_it_will_be_check( p->x_position_black_king,p->y_position_black_king,p->x_position_black_king,  p->y_position_black_king, piece) != 1)
            {
               //retour a l'etat initial puisque le coup en question est illegal
               board[start_y][start_x] = board[end_y][end_x];
               board[end_y][end_x] = L' ';
               printf("it's will be check.\n");
               flag_correct_moove = 0;
            }
            else 
            {
                printf("Piece moved successfully.\n");
                flag_correct_moove = 1;
            } 

        } 
        else if(rook_verification(start_x, start_y, end_x, end_y) == 0)
        {
            printf("Invalid move for the rook: You cannot eat a piece of your color\n");
            flag_correct_moove = 0;
        }
        else {
                printf("Invalid move for the rook: there is a piece in your way\n");
                flag_correct_moove = 0;
        }
    }
    else if(piece == black_queen || piece == white_queen)
    {
        if (rook_verification(start_x, start_y, end_x, end_y) == 1 || bishop_verification(start_x, start_y, end_x, end_y) == 1) {
            // Deplacement provisoire  de la piece en attente de verfication
            board[end_y][end_x] = board[start_y][start_x];
            board[start_y][start_x] = L' ';

            if(verification_it_will_be_check(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king, p->y_position_white_king, piece) != 1 || verification_it_will_be_check( p->x_position_black_king,p->y_position_black_king, p->x_position_black_king, p->y_position_black_king, piece) != 1)
            {
               //retour a l'etat initial puisque le coup en question est illegal
               board[start_y][start_x] = board[end_y][end_x];
               board[end_y][end_x] = L' ';
               printf("it's will be check.\n");
               flag_correct_moove = 0;
            }
            else 
            {
                printf("Piece moved successfully.\n");
                flag_correct_moove = 1;
            } 

        } 
        else if(rook_verification(start_x, start_y, end_x, end_y) == 0 || bishop_verification(start_x, start_y, end_x, end_y) == 0)
        {
            printf("Invalid move for the Queen: You cannot eat a piece of your color\n");
            flag_correct_moove = 0;
        }
        else {
                printf("Invalid move for the Queen: there is a piece in your way\n");
                flag_correct_moove = 0;
        }

    }
    else if (piece == black_pawn || piece == white_pawn)
    {
        if (pawn_verification(start_x, start_y, end_x, end_y, piece) == 1) {
            // Deplacement provisoire  de la piece en attente de verfication
            board[end_y][end_x] = board[start_y][start_x];
            board[start_y][start_x] = L' ';

            if(verification_it_will_be_check(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king, p->y_position_white_king, piece) != 1 || verification_it_will_be_check( p->x_position_black_king, p->y_position_black_king, p->x_position_black_king, p->y_position_black_king, piece) != 1)
            {
               //retour a l'etat initial puisque le coup en question est illegal
               board[start_y][start_x] = board[end_y][end_x];
               board[end_y][end_x] = L' ';
               printf("it's will be check.\n");
               flag_correct_moove = 0;
            }
            else 
            {
                printf("Piece moved successfully.\n");
                flag_correct_moove = 1;
            }   

        } 
        else if(pawn_verification(start_x, start_y, end_x, end_y, piece) == 2)
        {
  
            // Deplacement provisoire  de la piece en attente de verfication
            board[end_y][end_x] = board[start_y][start_x];
            board[start_y][start_x] = L' ';

            if(verification_it_will_be_check(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king, p->y_position_white_king, piece) != 1 || verification_it_will_be_check( p->x_position_black_king, p->y_position_black_king, p->x_position_black_king, p->y_position_black_king, piece) != 1)
            {
               //retour a l'etat initial puisque le coup en question est illegal
               board[start_y][start_x] = board[end_y][end_x];
               board[end_y][end_x] = L' ';
               printf("it's will be check.\n");
               flag_correct_moove = 0;
            }
            else 
            {
                 if(piece == white_pawn)
                 {
                   // Le mouvement est légal
                   flag_correct_moove = 1;
                   char promotion = proposer_promotion_white();
                   board[end_y][end_x] = promotion;
                   
                 }
                 else
                 {
                    // Le mouvement est légal
                   flag_correct_moove = 1;
                   char promotion = proposer_promotion_black();
                   board[end_y][end_x] = promotion;
                 }

            } 
        }
        else if(pawn_verification(start_x, start_y, end_x, end_y, piece) == 3)
        {
                        
            // Deplacement provisoire  de la piece en attente de verfication
            board[end_y][end_x] = board[start_y][start_x];
            board[start_y][start_x] = L' ';
            if(verification_it_will_be_check(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king, p->y_position_white_king, piece) != 1 || verification_it_will_be_check( p->x_position_black_king, p->y_position_black_king, p->x_position_black_king, p->y_position_black_king, piece) != 1)
            {
               //retour a l'etat initial puisque le coup en question est illegal
               board[start_y][start_x] = board[end_y][end_x];
               board[end_y][end_x] = L' ';
               printf("it's will be check.\n");
               flag_correct_moove = 0;
            }
            else 
            {
                //MON PASSANT
                if(end_x == start_x - 1)
                {
                    board[start_y][start_x - 1] = L' ';
                    printf("Piece moved successfully.\n");
                    flag_correct_moove = 1;
                }
                else if(end_x == start_x + 1)
                {
                    board[start_y][start_x + 1] = L' ';
                    printf("Piece moved successfully.\n");
                    flag_correct_moove = 1;
                }
            } 
        }
        else if(pawn_verification(start_x, start_y, end_x, end_y,  piece) == 0)
        {
            printf("Invalid move for the Pawn: You cannot eat a piece of your color\n");
            flag_correct_moove = 0;
        }
        else {
                printf("Invalid move for the Pawn: you cannot move in diagonal\n");
                flag_correct_moove = 0;
        }
    }
    else if (piece == black_knight || piece == white_knight)
    {
        if (knight_verification(start_x, start_y, end_x, end_y) == 1) {
            // Deplacement provisoire  de la piece en attente de verfication
            board[end_y][end_x] = board[start_y][start_x];
            board[start_y][start_x] = L' ';
            if(verification_it_will_be_check(p->x_position_white_king, p->y_position_white_king, p->x_position_white_king, p->y_position_white_king, piece) != 1 || verification_it_will_be_check( p->x_position_black_king, p->y_position_black_king, p->x_position_black_king, p->y_position_black_king, piece) != 1)
            {
               //retour a l'etat initial puisque le coup en question est illegal
               board[start_y][start_x] = board[end_y][end_x];
               board[end_y][end_x] = L' ';
               printf("it's will be check.\n");
               flag_correct_moove = 0;
            }
            else 
            {
                printf("Piece moved successfully.\n");
                flag_correct_moove = 1;
            }   
        } 
        else if(knight_verification(start_x, start_y, end_x, end_y) == 0)
        {
            printf("Invalid move for the knight: You cannot eat a piece of your color\n");
            flag_correct_moove = 0;
        }
        else {
                printf("Invalid move for the knight\n");
                flag_correct_moove = 0;
        }
    }
    else if (piece == black_king || piece == white_king) {

        int king_result = king_verification(start_x, start_y, end_x, end_y);

        if (king_result == 1) {

            // Déplacement simple du roi
            board[end_y][end_x] = board[start_y][start_x];
            board[start_y][start_x] = L' ';
            printf("Piece moved successfully.\n");
            flag_correct_moove = 1;

            if (piece == white_king) {
                p->x_position_white_king = end_x;
                p->y_position_white_king = end_y;
            } else {
                p->x_position_black_king = end_x;
                p->y_position_black_king = end_y;
            }
        } else if (king_result == 3) {
            // Vérification et réalisation du roque
            if (verfication_if_it_is_correct_castling(start_x, start_y, end_x, p) == 1) {

                // Mise à jour du roi (kingside)
                board[end_y][end_x] = board[start_y][start_x];
                board[start_y][start_x] = L' ';

                // Mise à jour de la tour
                if (end_x == 6) { // roque côté roi
                    board[end_y][5] = board[end_y][7];
                    board[end_y][7] = L' ';
                } else if (end_x == 2) { // roque côté dame (queen side)
                    board[end_y][3] = board[end_y][0];
                    board[end_y][0] = L' ';
                }
                printf("Castling move executed successfully.\n");
                flag_correct_moove = 1;

                if (piece == white_king) {
                    p->x_position_white_king = end_x;
                    p->y_position_white_king = end_y;
                } else {
                    p->x_position_black_king = end_x;
                    p->y_position_black_king = end_y;
                }
            } else {
                printf("Invalid castling move.\n");
                flag_correct_moove = 0;
            }
        } else {
            printf("Invalid move for the king.\n");
            flag_correct_moove = 0;
        }
    }
}

int check_if_it_black_or_white(wchar_t piece)
{ // si les deux sont noir il va rendre deux si les deux sont blanc il va rendre moin deux ducoup on fait un absolute du reslutat et pour verifier si c'est correct et bah c'est tout result sauf absolute du return de cette fonction sur les start position et end position 
  // si c'est un blanc noir sa va donner 0
  // si c'est un blanc sur une case vide sa donne -1;
  // et si c'est un noir sur une case vide sa +1;
    if(piece == black_rook || piece == black_knight || piece ==  black_bishop || piece == black_queen || piece == black_king || piece == black_pawn)
    {
        return 1;
    }
    else if (piece == white_rook || piece == white_knight || piece ==  white_bishop || piece == white_queen || piece == white_king || piece == white_pawn)
        return  -1;
    else 
        return 0;

}


void Show_table()
{
    setlocale(LC_ALL, "");

    // Affichage des coordonnées horizontales
    printf("  ");
    for (int i = 0; i < 8; i++) {
        printf("   %c   ", 'a' + i);
    }
    printf("\n");

    // Affichage du plateau de jeu
    for (int i = 0; i < 8; i++) {
        printf("  ");
        for (int k = 0; k < 8; k++) {
            printf("-------");
        }
        printf("-\n");
        printf("%d ", 8 - i); // Inverser l'ordre des lignes
        for (int j = 0; j < 8; j++) {
            // Définir la couleur du texte en fonction de la couleur de la pièce
            if (check_if_it_black_or_white(board[i][j]) == 1) {
                printf("\x1b[30m"); // Noir
            } else {
                printf("\x1b[37m"); // Blanc
            }
            // Afficher la pièce
            printf("|  %lc   ", board[i][j]);
            // Réinitialiser la couleur du texte
            printf("\x1b[0m");
        }
        printf("|\n");
    }
    printf("  ");
    for (int k = 0; k < 8; k++) {
        printf("-------");
    }
    printf("-\n");
}
