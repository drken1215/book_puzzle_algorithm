#include <iostream>
#include <vector>
#include <string>
#include <utility>
using namespace std;

// 二部マッチングを解くためのクラス
class BipartiteMatching {
 private:
    // 左側頂点数と右側頂点数
    int size_left, size_right;

    // list[l]: 左側頂点 l に隣接する右側頂点の集合
    vector<vector<int>> list;

    // 計算結果
    // l2r[l]: 左側頂点 l とペアにする右側頂点
    // r2l[r]: 右側頂点 r とペアにする左側頂点
    // マッチングしていない部分は -1 とする
    vector<int> l2r, r2l;

 public:
    // コンストラクタ
    BipartiteMatching(int sl, int sr) :
        size_left(sl), size_right(sr), list(sl) {}

    // 二部グラフの辺を追加していく
    // 左側頂点 l と右側頂点 r を結ぶ
    void add_edge(int l, int r) { list[l].push_back(r); }

    // 左側頂点 l を始点とした増加路を探索する
    // seen[l]: 左側頂点 l からの探索が実施済みかどうか
    // 返り値: 存在するとき true、しないとき false
    bool dfs(int l, vector<bool>& seen) {
        // すでに探索済みの場合は false を返す
        if (seen[l]) return false;

        // 探索済みの状態にする
        seen[l] = true;

        // 右側頂点を順に探索する
        for (int r : list[l]) {
            // 右側頂点 r がフリーである場合は true を返す
            // または、すでにマッチングしている左側頂点へ向かって
            // 増加路が見つかる場合も true を返す
            if (r2l[r] == -1 || dfs(r2l[r], seen)) {
                l2r[l] = r;
                r2l[r] = l;
                return true;
            }
        }

        // 増加路が見つからない場合は false を返す
        return false;
    }

    // 二部マッチング問題を解く
    // 返り値: マッチング辺 (左側頂点, 右側頂点) の集合
    vector<pair<int, int>> solve() {
        // マッチング辺
        vector<pair<int, int>> res;

        // 初期化
        l2r.assign(size_left, -1);
        r2l.assign(size_right, -1);

        // 増加路が見つからなくなるまで反復する
        while (true) {
            // 更新が発生したかどうかを表すフラグ変数
            bool update = false;

            // 同じ左側頂点を重複して探索することを防ぐ
            vector<bool> seen(size_left, false);

            // 左側頂点 l を順に探索する
            for (int l = 0; l < size_left; ++l) {
                // すでにマッチング済みの場合はスキップする
                if (l2r[l] != -1) continue;

                // 増加路が見つかった場合、フラグを立てて繰り返す
                if (dfs(l, seen)) {
                    update = true;
                    break;
                }
            }

            // 増加路が見つからない場合は終了する
            if (!update) break;
        }

        // マッチング辺を復元する
        for (int l = 0; l < size_left; ++l) {
            if (l2r[l] != -1) res.emplace_back(l, l2r[l]);
        }
        return res;
    }
};

// ドミノタイリングソルバー (盤面にタイリングを埋め込む)
// 返り値は、敷き詰めたドミノの個数
int solve(vector<string>& board) {
    // 上下左右の 4 マスへの移動を表すベクトル
    static const vector<int> dx = {1, 0, -1, 0};
    static const vector<int> dy = {0, 1, 0, -1};

    // 盤面のサイズ
    int H = board.size(), W = board[0].size();

    // 二部グラフを構築する
    BipartiteMatching bm(H * W, H * W);
    for (int i = 0; i < H; ++i) {
        for (int j = 0; j < W; ++j) {
            // 'x' の場合はスキップする
            if (board[i][j] == 'x') continue;

            // 黒色マスでない場合はスキップする
            if ((i + j) % 2) continue;

            // 上下左右を調べる
            for (int dir = 0; dir < 4; ++dir) {
                int ni = i + dx[dir];
                int nj = j + dy[dir];

                // 盤面外の場合はスキップする
                if (ni < 0 || ni >= H || nj < 0 || nj >= W) continue;

                // 'x' の場合はスキップする
                if (board[ni][nj] == 'x') continue;

                // 二部グラフの辺を追加する
                bm.add_edge(i * W + j, ni * W + nj);
            }
        }
    }

    // 二部マッチングの解を得る
    const vector<pair<int, int>>& res = bm.solve();

    // 盤面にタイリングを埋め込む
    for (const pair<int, int>& lr : res) {
        int l = lr.first, r = lr.second;
        int li = l / W, lj = l % W;
        int ri = r / W, rj = r % W;

        if (li == ri) {
            // 横に並べるとき
            board[li][lj] = '-';
            board[ri][rj] = '-';
        } else {
            // 縦に並べるとき
            board[li][lj] = '|';
            board[ri][rj] = '|';
        }
    }
    return res.size();
}

int main() {
    // 入力
    cout << "Domino Tiling Input:" << endl;
    int H, W;
    cin >> H >> W;
    vector<string> board(H);
    for (string& row : board) cin >> row;

    // ドミノタイリングを解く
    int max_num = solve(board);

    // 解を出力
    cout << "num of domino: " << max_num << endl;
    for (const string& row : board) cout << row << endl;
}
