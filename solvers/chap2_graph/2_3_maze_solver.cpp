#include <iostream>
#include <vector>
#include <string>
#include <utility>
using namespace std;

// 「座標」を表す型を整数値のペアとして定義する
using Cor = pair<int, int>;

// 上下左右への移動を定義しておく (0: 下、1: 右、2: 上、3: 左)
const vector<Cor> DIR = {Cor(1, 0), Cor(0, 1), Cor(-1, 0), Cor(0, -1)};

// 未確定であることを表す値
const int NOTIN = -1;

// 迷路ソルバー
// board: 迷路の盤面。マス (x, y) の情報は board[x][y] で取得する
// start: スタートマスの座標、goal: ゴールマスの座標
void solve(const vector<string>& board,
           const Cor& start, const Cor& goal) {
    // 盤面の縦サイズと横サイズ
    int H = board.size(), W = board[0].size();

    // nodes[k] := k と書き込まれるマスの集合
    vector<vector<Cor>> nodes;

    // dist[x][y] := マス (x, y) に書き込まれる数値
    vector<vector<int>> dist(H, vector<int>(W, NOTIN));

    // arrow[x][y] := マス (x, y) を始点とする矢印の終点の座標
    vector<vector<Cor>> arrow(H, vector<Cor>(W, Cor(NOTIN, NOTIN)));

    // スタート地点に関する情報で、探索情報を初期化する
    nodes.push_back(vector<Cor>(1, start));
    dist[start.first][start.second] = 0;

    // 幅優先探索する (最新の nodes が空になるまで実施する)
    while (!nodes.back().empty()) {
        // 最新の数値が書き込まれたマスの集合
        const vector<Cor>& cur = nodes.back();

        // 最新の数値が書き込まれたマスから 1 手で行けるマスを求める
        vector<Cor> nex;
        for (const Cor& cell : cur) {
            // cell の座標値
            int x = cell.first, y = cell.second;

            // cell の上下左右のマスを順に調べる
            for (int direction = 0; direction < 4; ++direction) {
                int nex_x = x + DIR[direction].first;
                int nex_y = y + DIR[direction].second;

                // 新たなマスが盤面外になる場合はスキップ
                if (nex_x < 0 || nex_x >= H) continue;
                if (nex_y < 0 || nex_y >= W) continue;

                // 新たなマスが壁である場合もスキップ
                if (board[nex_x][nex_y] == '#') continue;

                // 新たなマスにすでに数値が書き込まれている場合もスキップ
                if (dist[nex_x][nex_y] != NOTIN) continue;

                // 新たなマスに数値を書き込む (マス (x, y) より 1 手長い)
                dist[nex_x][nex_y] = dist[x][y] + 1;
                arrow[nex_x][nex_y] = Cor(x, y);
                nex.push_back(Cor(nex_x, nex_y));
            }
        }

        // 新たに数値が書き込まれたマスを nodes の末尾に追加する
        nodes.push_back(nex);
    }

    // スタートマスからゴールマスへ辿り着けない場合は、例外処理する
    if (dist[goal.first][goal.second] == NOTIN) {
        cout << "No Path" << endl;
        return;
    }

    // 矢印を辿ることで、経路復元を開始する
    vector<string> res = board;  // 答えを出力するための盤面
    int cur_x = goal.first, cur_y = goal.second;

    // スタート地点からは唯一、矢印が伸びないことに注意
    while (arrow[cur_x][cur_y] != Cor(NOTIN, NOTIN)) {
        // 最短経路の通路を 'o' で埋める
        res[cur_x][cur_y] = 'o';

        // 矢印をたどる
        int nex_x = arrow[cur_x][cur_y].first;
        int nex_y = arrow[cur_x][cur_y].second;
        cur_x = nex_x, cur_y = nex_y;
    }

    // 最短経路を出力する
    cout << "----- solution -----" << endl;
    for (const string& str : res) cout << str << endl;
    cout << "length = " << dist[goal.first][goal.second] << endl;
}

int main() {
    // 入力
    cout << "Maze Input: " << endl;
    int H, W;  // 縦の長さ、横の長さ
    cin >> H >> W;
    vector<string> board(H);  // 盤面
    for (int x = 0; x < H; ++x) cin >> board[x];

    // スタートマスとゴールマスを求める
    Cor start, goal;
    for (int x = 0; x < H; ++x) {
        for (int y = 0; y < W; ++y) {
            if (board[x][y] == 'S')
                start = Cor(x, y);
            else if (board[x][y] == 'G')
                goal = Cor(x, y);
        }
    }

    // 迷路を解く
    solve(board, start, goal);
}
