#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>
#include <climits>
#include <algorithm>
#include <random>
#include <ctime>
#include <string>

using namespace std;

typedef vector<int> Board;
const Board GOAL = {1, 2, 3, 8, 0, 4, 6, 5, 7};
const int N = 3;
const vector<pair<int, int>> MOVES = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

void err(){
    if (cin.fail()) { // kiểm tra xem cin có lỗi không
        cin.clear(); // làm sạch các cờ lỗi
        cin.ignore(INT_MAX, '\n'); // bỏ qua các ký tự còn lại trong bộ đệm
    }
}


void printBoard(const Board& board) {
    for (int i = 0; i < 9; i++) {
        cout << (board[i] == 0 ? "_" : to_string(board[i])) << " ";
        if (i % 3 == 2) cout << endl;
    }
    cout << endl;
}
bool isValidMove(const Board& board, int num) {
    int zeroPos = find(board.begin(), board.end(), 0) - board.begin();
    int numPos = find(board.begin(), board.end(), num) - board.begin();

    int zeroX = zeroPos / N;
    int zeroY = zeroPos % N;

    int numX = numPos / N;
    int numY = numPos % N;

    // Kiểm tra xem số có kề cạnh ô trống không
    return abs(zeroX - numX) + abs(zeroY - numY) == 1;
}
void manualSolve(Board& board) {
    while (board != GOAL) {
        printBoard(board);
        cout << "Chọn một số để di chuyển vào ô trống (hoặc -1 để thoát): ";
        int choice;
        cin >> choice;

        if (choice == -1) {
            cout << "Bạn đã chọn thoát giữa chừng.\n";
            return;
        }

	while (choice < 1 || choice > 8 || !isValidMove(board, choice)) {
    cout << "Di chuyển không hợp lệ. Chọn một số khác: ";
    cin >> choice;
    err();


    if (choice == -1) {
        cout << "Bạn đã chọn thoát giữa chừng.\n";
        return;
    }
}

        int zeroPos = find(board.begin(), board.end(), 0) - board.begin();
        int numPos = find(board.begin(), board.end(), choice) - board.begin();

        swap(board[zeroPos], board[numPos]);
    }

    cout << "Chúc mừng, bạn đã giải xong!" << endl;
}

// Hàm trộn bằng cách di chuyển số 0
Board controlledShuffleBoard() {
    Board board = GOAL;
    int zeroPos = find(board.begin(), board.end(), 0) - board.begin();
    int x = zeroPos / N;
    int y = zeroPos % N;

    random_device rd;
    mt19937 g(rd());

    for (int i = 0; i < 1000; ++i) {
        vector<pair<int, int>> possibleMoves;

        for (auto move : MOVES) {
            int newX = x + move.first;
            int newY = y + move.second;

            if (newX >= 0 && newX < N && newY >= 0 && newY < N) {
                possibleMoves.push_back({newX, newY});
            }
        }

        uniform_int_distribution<> dis(0, possibleMoves.size() - 1);
        int choice = dis(g);
        int newX = possibleMoves[choice].first;
        int newY = possibleMoves[choice].second;

        // Swap 0 with the chosen position
        swap(board[x * N + y], board[newX * N + newY]);

        // Update x and y for the next iteration
        x = newX;
        y = newY;
    }

    return board;
}

string boardToString(const Board& board) {
    string s;
    for (int num : board) {
        s += to_string(num);
    }
    return s;
}

// Hàm heuristic: số ô không đúng vị trí
int heuristic(const Board& board) {
    int cost = 0;
    for (int i = 0; i < 9; ++i) {
        if (board[i] && board[i] != GOAL[i]) {
            cost++;
        }
    }
    return cost;
}

struct Node {
    Board board;
    int cost;
    int g; // Chi phí từ bắt đầu đến trạng thái hiện tại
    Node* parent;
    string action; // Move

    Node(Board b, int g_val, Node* p, string a) : board(b), g(g_val), parent(p), action(a) {
        cost = g + heuristic(b);
    }

    bool operator>(const Node& other) const {
        return cost > other.cost;
    }
};

vector<string> astar(const Board& start) {
    priority_queue<Node, vector<Node>, greater<Node>> open;
    unordered_set<string> closed;

    Node startNode(start, 0, nullptr, "");
    open.push(startNode);

    while (!open.empty()) {
        Node current = open.top();
        open.pop();

        if (current.board == GOAL) {
            vector<string> path;
            while (current.parent) {
                path.push_back(current.action);
                current = *current.parent;
            }
            reverse(path.begin(), path.end());
            return path;
        }

        closed.insert(boardToString(current.board));

        int zeroPos = find(current.board.begin(), current.board.end(), 0) - current.board.begin();
        int x = zeroPos / N;
        int y = zeroPos % N;

        for (auto move : MOVES) {
            int newX = x + move.first;
            int newY = y + move.second;

            if (newX >= 0 && newX < N && newY >= 0 && newY < N) {
                Board newBoard = current.board;
                swap(newBoard[x * N + y], newBoard[newX * N + newY]);

                string newState = boardToString(newBoard);
                if (closed.find(newState) == closed.end()) {
                    string action = to_string(newBoard[zeroPos]) + " -> 0";
                    Node newNode(newBoard, current.g + 1, new Node(current), action);
                    open.push(newNode);
                }
            }
        }
    }
    return {}; // Không tìm thấy đường đi
}

int main() {
    Board currentBoard;
    vector<string> solution;

    while (true) {
        cout << "Menu:\n";
        cout << "1. Trộn và giải thủ công\n";
        cout << "2. Trộn và dùng thuật toán A star \n";
        cout << "3. Xem các bước giải\n";
        cout << "4. Thoát\n";
        cout << "Lựa chọn của bạn: ";
        int choice;
        cin >> choice;

	err();

        switch (choice) {
		case 1:
		{
    		  currentBoard = controlledShuffleBoard();
    		  manualSolve(currentBoard);
    		break;
		}
		case 2:
    {
        currentBoard = controlledShuffleBoard();
        cout << "Bảng đã trộn:" << endl;
        printBoard(currentBoard);
        solution = astar(currentBoard);
        break;
    }
		case 3:
		    {
    			Board state = currentBoard;
    			cout << "Bắt đầu:" << endl;
    			printBoard(state);
    		for (const string& move : solution) {
		cout << "Di chuyển: " << move << endl;
		int from = find(state.begin(), state.end(), 0) - state.begin();
                int x = from / N;
        	int y = from % N;
        
      		  // Tìm hướng di chuyển dựa trên mô tả
		pair<int, int> dir;
        	int targetNum = stoi(move.substr(0, move.find(" ->")));
        	int targetPos = find(state.begin(), state.end(), targetNum) - state.begin();
        	dir.first = (targetPos / N) - x;
        	dir.second = (targetPos % N) - y;

        	// Thực hiện di chuyển trên trạng thái hiện tại
        swap(state[x * N + y], state[(x + dir.first) * N + (y + dir.second)]);
        
        printBoard(state);
    }
	cout << "Tổng số bước giải: " << solution.size() << endl;
    break;
		    }
            case 4:
		    {
                return 0;
            default:
                cout << "Lựa chọn không hợp lệ. Vui lòng chọn lại.\n";
		    }
        }
    }
    return 0;
}
