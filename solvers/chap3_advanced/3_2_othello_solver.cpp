#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
using namespace std;

// オセロ盤の各マスの座標と、手番を整数値で表す
using Cell = int;
using Color = int;

// オセロ盤の石の座標の集合も整数値 (0 以上 2^16 未満) で表す
using Stones = int;

// オセロ盤のサイズと、無限大を表す値
const int SIZE = 4;
const int INF = SIZE * SIZE;  // 評価値の理論最大値

// 色を定義する
const Color BLACK = 1;
const Color WHITE = 0;

// マス ce の隣接マスを求める (方向 dir)
Cell move(Cell cell, int dir) {
    // 周囲 8 マスへの移動ベクトル
    static const vector<int> DX = {1, 0, -1, 0, 1, 1, -1, -1};
    static const vector<int> DY = {0, 1, 0, -1, 1, -1, -1, 1};

    // マス cell の隣接マスの x 座標と y 座標を求める
    int x = cell / SIZE + DX[dir];
    int y = cell % SIZE + DY[dir];

    // 盤外に出る場合は -1 とする
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE)
        return -1;
    else
        return x * SIZE + y;
}

// 盤面が (black, white) のとき、マス cell の色が col かどうか
bool iscolor(Stones black, Stones white, Color col, Cell cell) {
    if (cell == -1)
        return false;
    if (col == BLACK)
        return ((black >> cell) & 1);
    else
        return ((white >> cell) & 1);
}

// 盤面が (black, white) のとき、マス cell に色 col の石を置く
// 反転するマスの集合を返す (置けないときは 0 を返す)
Cell put(Stones black, Stones white, Color col, Cell cell) {
    // マス cell にすでに石がある場合は置けない
    if (((black | white) >> cell) & 1) return 0;

    // 八方向それぞれのひっくり返す石の集合を求めて合わせる
    Stones res = 0;
    for (int dir = 0; dir < 8; ++dir) {
        // 方向 dir についてひっくり返す石の集合を rev とする
        Stones rev = 0;

        // 方向 dir を順に見ていく
        Cell cell2 = move(cell, dir);
        while (iscolor(black, white, 1 - col, cell2)) {
            rev |= 1 << cell2;
            cell2 = move(cell2, dir);
        }

        // きちんと自分の色で挟めている場合、rev を res に加算する
        if (iscolor(black, white, col, cell2)) {
            res |= rev;
        }
    }
    return res;
}

// 終局時の得点計算 (手盤が col 側)
int calc(Stones black, Stones white, Color col) {
    // 終局時の黒石、白石、空きマスの個数を数える
    int num_black = 0, num_white = 0, num_empty = 0;
    for (Cell cell = 0; cell < SIZE * SIZE; ++cell) {
        if ((black >> cell) & 1)
            ++num_black;
        else if ((white >> cell) & 1)
            ++num_white;
        else
            ++num_empty;
    }

    // 勝利側に空きマスの個数を加算する
    if (num_black > num_white)
        num_black += num_empty;
    else if (num_black < num_white)
        num_white += num_empty;

    // 手番に応じて得点を返す
    if (col == BLACK)
        return num_black - num_white;
    else
        return num_white - num_black;
}

// 盤面が (black, white)、手番が col である局面の評価値を求める
int rec(int alpha, int beta, Stones black, Stones white, Color col) {
    // 石の置ける場所を求める (mine: 自分、opp: 相手)
    vector<Cell> mine, opp;
    for (Cell cell = 0; cell < SIZE * SIZE; ++cell) {
        if (put(black, white, col, cell))
            mine.push_back(cell);
        if (put(black, white, 1 - col, cell))
            opp.push_back(cell);
    }

    // 終局の場合は石差を数えて、手番に応じた得点を返す
    if (mine.empty() && opp.empty()) {
        return calc(black, white, col);
    }

    // 打てる手がないときはパスする
    if (mine.empty()) {
        return -rec(-beta, -alpha, black, white, 1 - col);
    }

    // 無限小を表す値で評価値を初期化する
    int res = -INF;

    // 打てる手を順に調べていく
    for (Cell cell : mine) {
        // 手を打ったあとの盤面を求める
        Stones rev = put(black, white, col, cell);
        Stones black2 = black ^ rev;
        Stones white2 = white ^ rev;

        // マス cell に石を置く
        if (col == BLACK)
            black2 |= 1 << cell;
        else
            white2 |= 1 << cell;

        // 遷移局面の評価値を符号反転して受け取る
        int score = -rec(-beta, -alpha, black2, white2, 1 - col);

        // その値が最大となる手を選びたい
        res = max(res, score);

        // 枝刈り
        if (res >= beta) return res;

        // 手番側がこれ以上の得点が得られることは保証できるアルファ値の更新
        alpha = max(alpha, res);
    }
    return res;
}

int main() {
    // 初期配置 (盤面サイズに依存して変わる)
    Stones black = (1 << 6) | (1 << 9);  // 黒石の配置
    Stones white = (1 << 5) | (1 << 10);  // 白石の配置

    // α = -16 (理論最小値)、β = 16 (理論最大値)、先手は黒
    int score = rec(-INF, INF, black, white, BLACK);
    cout << score << endl;
}
