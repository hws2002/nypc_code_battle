//main.cpp
#include "game.h"
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#define DEBUG

using namespace std;

// 게임 상태를 관리하는 클래스

// 표준 입력을 통해 명령어를 처리하는 메인 함수
int main()
{
    Game game;
    bool first = false;

    while (true)
    {
        string line;
        getline(cin, line);

        istringstream iss(line);
        string command;
        if (!(iss >> command))
            continue;

        if (command == "READY")
        {
            // 선공 여부 확인
            string turn;
            iss >> turn;
            first = (turn == "FIRST");
            cout << "OK" << endl;
            continue;
        }

        if (command == "INIT")
        {
            // 보드 초기화
            vector<vector<int>> board;
            string row;
            while (iss >> row)
            {
                vector<int> boardRow;
                for (char c : row)
                {
                    boardRow.push_back(c - '0'); // 문자 → 숫자 변환
                }
                board.push_back(boardRow);
            }
			cout<<"board size : "<<board.size()<<","<<board[0].size()<<endl;
            game = Game(board, first);
			#ifdef DEBUG
			cout<<" board initialization done"<<endl;
			#endif
            continue;
        }

        if (command == "TIME")
        {
            // 내 차례: 수 계산 및 출력
            int myTime, oppTime;
            iss >> myTime >> oppTime;

            vector<int> ret = game.calculateMove(myTime, oppTime);
            game.updateMove(ret[0], ret[1], ret[2], ret[3], true);

            cout << ret[0] << " " << ret[1] << " " << ret[2] << " " << ret[3] << endl; // 내 행동 출력
            continue;
        }

        if (command == "OPP")
        {
            // 상대 행동 반영
            int r1, c1, r2, c2, time;
            iss >> r1 >> c1 >> r2 >> c2 >> time;
            game.updateOpponentAction({r1, c1, r2, c2}, time);
            continue;
        }

        if (command == "FINISH")
        {
            // 게임 종료
            break;
        }

        // 알 수 없는 명령 처리
        cerr << "Invalid command: " << command << endl;
        return 1;
    }

    return 0;
}