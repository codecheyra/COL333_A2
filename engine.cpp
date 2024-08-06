#include <algorithm>
#include <random>
#include <iostream>
#include <unordered_map>
using namespace std;
#include "board.hpp"
#include "engine.hpp"

// void Engine::find_best_move(const Board& b) {

//     // pick a random move
    
//     auto moveset = b.get_legal_moves();
//     if (moveset.size() == 0) {
//         this->best_move = 0;
//     }
//     else {
//         std::vector<U16> moves;
//         std::cout << all_boards_to_str(b) << std::endl;
//         for (auto m : moveset) {
//             std::cout << move_to_str(m) << " ";
//         }
//         std::cout << std::endl;
//         std::sample(
//             moveset.begin(),
//             moveset.end(),
//             std::back_inserter(moves),
//             1,
//             std::mt19937{std::random_device{}()}
//         );
//         this->best_move = moves[0];
//     }
// }



// attacking squares 
// for a given state check are there any pawns which we can attack
//Check if there is any black piece in our attacking area




double evaluation_function(const Board& board,PlayerColor bot){
    unordered_map<U16, int> points;
    PlayerColor player_color = board.data.player_to_play ;
    points[WHITE | PAWN] = 10;
    points[WHITE | ROOK] = 50;
    points[WHITE | BISHOP] = 30;
    points[WHITE | KING] = 0;

    points[BLACK | PAWN] = 10;
    points[BLACK | ROOK] = 50;
    points[BLACK | BISHOP] = 30;
    points[BLACK | KING] = 0;
    
    int white_score = 0;
    int black_score = 0;
    white_score -= abs((gety(board.data.w_king) - gety(board.data.w_pawn_bs))*2) + abs((gety(board.data.w_king) - gety(board.data.w_pawn_ws))*2);
    black_score -= abs(gety(board.data.b_king) - gety(board.data.b_pawn_bs))*2 + abs(gety(board.data.b_king) - gety(board.data.b_pawn_ws))*2; 
    for(int i = 0; i < 64; i++){
        U8 every_piece = board.data.board_0[i];
        if(every_piece != EMPTY && (every_piece & WHITE)){  //white piece
            if (every_piece == (WHITE | PAWN)){
                white_score += (6 - gety(board.data.w_pawn_ws));
                white_score += (5 - gety(board.data.w_pawn_bs));
            }
            white_score += points[every_piece];
        }
        if(every_piece != EMPTY && (every_piece & BLACK)){  //black piece
            black_score += points[every_piece];
        }
    }
    if (bot == WHITE){
        return (double) white_score - black_score;
    } else{
        return (double) black_score-white_score;
    }
    }


double evaluation_2(const Board& board, U16 move){
    unordered_map<U16, int> points;
    // PlayerColor player_color = board.data.player_to_play ;
    points[WHITE | PAWN] = 10;
    points[WHITE | ROOK] = 50;
    points[WHITE | BISHOP] = 30;
    points[WHITE | KING] = 100;

    points[BLACK | PAWN] = 10;
    points[BLACK | ROOK] = 50;
    points[BLACK | BISHOP] = 30;
    points[BLACK | KING] = 100;
    // points[] = 0

    U8 initial_pos = getp0(move);
    U8 final_pos = getp1(move);
    U8 our_piece = board.data.board_0[initial_pos];
    U8 enemy_piece = board.data.board_0[final_pos];

    int white_score = 0;
    int black_score = 0;

    // white_score += 

    if (points[our_piece] <= points[enemy_piece]){
        if (enemy_piece & BLACK){
            // cout<<"pawn is found";
            white_score += points[enemy_piece]*100;
    } else if (enemy_piece & WHITE){
            black_score += points[enemy_piece]*100;
    } 
    }


    Board* new_board = board.copy();
    new_board->do_move(move);

    for(int i = 0; i < 64; i++){
        U8 every_piece = new_board->data.board_0[i];
        if(every_piece != EMPTY && (every_piece & WHITE)){  //white piece
            if (every_piece == (WHITE | PAWN)){
                white_score += (6 - gety(new_board->data.w_pawn_ws))*100;
                white_score += (5 - gety(new_board->data.w_pawn_bs))*100;
            }
            white_score +=points[every_piece];
        }
        if(every_piece != EMPTY && (every_piece & BLACK)){  //black piece
            black_score += points[every_piece];
        }
    }
    // if (player_color == WHITE){
    //     // cout<< white_score-black_score;
    //     return (double) white_score - black_score;
    // }
    // } else{
    //     return (double) black_score-white_score;
    // }
    // }
    // cout<<"inside eval func , score = ";
    // cout<< white_score - black_score;
    return (double) white_score - black_score;
    // cout<< "taking black player as main";
    return -30;     
}







double alpha_beta_prune(Board& board, double depth, double alpha, double beta, bool maximizingPlayer, U16 move,PlayerColor bot){
    Board* base_case_board = board.copy();
    base_case_board->do_move(move);
    if (depth == 0){
        // cout<<"reached the leaf";
        return evaluation_function(*base_case_board, bot);
    }
    
    std::unordered_set<U16> legalMoves = base_case_board->get_legal_moves();
    // cout<<"len of legal moves is "<<legalMoves.size();

    // cout<<"maximizig color = ";
    // cout<<maximizingPlayer;
    // cout<<"depth = ";
    // cout<<depth;
    if (maximizingPlayer==true){
        double cur_max = -INFINITY;
        // cout<<"came1";
        for (auto move: legalMoves){
            Board* new_board = board.copy();
            // new_board->do_move(move);
            cur_max = alpha_beta_prune(*new_board, depth-1, alpha, beta, false, move, bot);
            // cout<<"came back ";
            alpha = std::max(alpha, cur_max);
            // cout<<"alpha = "<<alpha<<"beta = "<<beta;
            if (alpha >= beta){
                // cout<<"loop broken";
                break;
                }
        }
        return cur_max;

        }
    if (maximizingPlayer==false){
        double cur_min = INFINITY;
        // cout<<"came2";
        for (auto move: legalMoves){
                Board* new_board = board.copy();
                // new_board->do_move(move);
                cur_min = alpha_beta_prune(*new_board, depth-1, alpha, beta, true, move,bot);
                // cout<<"came back ";

                beta = std::min(cur_min, beta);
                // cout<<"alpha = "<<alpha<<"beta = "<<beta;
                if (alpha >= beta){
                    // cout<<"loop broken";
                    break;
                }
        }
        return cur_min;
    }
    cout<<"came here to -1";
    return -1;
}







void Engine::find_best_move(const Board& b) {

    // pick a random move
    // cout<<"entra idhi";
    PlayerColor bot = b.data.player_to_play;
    auto moveset = b.get_legal_moves();
    if (moveset.size() == 0) {
        this->best_move = 0;
    }
    else {
        
        // PlayerColor color = b.data.player_to_play;
        double alpha = -INFINITY;
        double beta = INFINITY;
        std::unordered_set<U16> legalMoves = b.get_legal_moves();
        U16 bestmove = 0;
        double bestevaluated = -INFINITY;
        int k = 0;
        for(auto move : legalMoves){
            cout<< "Checking for a new move";
            k += 1;
            Board* new_board = b.copy();         

            // new_board->do_move(move);
            double evaluated = alpha_beta_prune(*new_board, 5, alpha, beta, false ,move,bot);
            // cout<< "evaluated for this is ";
            // cout<<evaluated;
            cout<<"evaluated scores ";
            cout<<evaluated;
            cout<<" ";
            if(evaluated > bestevaluated){
                bestevaluated = evaluated;
                bestmove = move;
                cout<<"best evaluated score is   ";
                cout<<bestevaluated;
                cout<<"     ";
                cout<<move;
                cout<<"   ";
            }
            alpha = std::max(alpha, evaluated);
            }
        this->best_move = bestmove;
    }
}


// void Engine::best_move(const Board& board){
//     PlayerColor color = board.data.player_to_play;
//     double alpha = INFINITY;
//     double beta = -INFINITY;
//     std::unordered_set<U16> legalMoves = board.get_legal_moves();
//     U16 bestmove = 0;
//     U16 bestevaluated = -INFINITY;
//     for(auto move : legalMoves){
//         Board* new_board = board.copy();              
//         new_board->do_move(move);
//         double evaluated = alpha_beta_prune(*new_board, 4, alpha, beta, true);
//         if(evaluated > bestevaluated){
//             bestevaluated = evaluated;
//             bestmove = move;
//         }
//         alpha = std::max(alpha, evaluated);
//     }
//     // return bestmove
// }