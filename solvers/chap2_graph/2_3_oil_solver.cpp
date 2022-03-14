#include <iostream>
#include <vector>
#include <map>
#include <queue>
#include <algorithm>
using namespace std;

// 各壺の油の量を頂点とする
using Node = vector<int>;

// 油分け算ソルバー
void solve(const Node& cap, const Node& start, const Node& goal) {
    // dist：探索の始点 start から、各頂点への最短距離
    map<Node, int> dist;
    dist[start] = 0;

    // arrow：経路復元のための変数
    map<Node, Node> arrow;

    // オープンリスト todo に頂点 start を挿入する
    queue<Node> todo;
    todo.push(start);

    // オープンリストが空になるまで探索を行う
    while (!todo.empty()) {
        // オープンリストから頂点を取り出す
        Node cur = todo.front();
        todo.pop();

        // 油の分け方をすべて調べる (壺 from から壺 to へ)
        for (int from = 0; from < 3; ++from) {
            for (int to = 0; to < 3; ++to) {
                if (from == to) continue;

                // 移し替えたあとの状態を求める
                Node nex = cur;
                if (nex[to] + nex[from] <= cap[to]) {
                    // 壺 from の油をすべて移しても溢れない場合
                    nex[to] += nex[from];
                    nex[from] = 0;
                } else {
                    // 溢れる場合
                    nex[from] = nex[from] + nex[to] - cap[to];
                    nex[to] = cap[to];
                }

                // すでに発見済みの頂点はスキップ
                if (dist.count(nex)) continue;

                // 新たな頂点を探索済みにして、オープンリストに挿入する
                dist[nex] = dist[cur] + 1;
                arrow[nex] = cur;
                todo.push(nex);
            }
        }
    }

    // 不可能の場合
    if (!dist.count(goal)) {
        cout << "Impossible" << endl;
        return;
    }

    // 経路復元
    vector<Node> res;
    Node cur = goal;
    while (arrow.count(cur)) {
        res.push_back(cur);
        cur = arrow[cur];
    }
    res.push_back(cur);

    // 出力
    reverse(res.begin(), res.end());  // 反転する
    for (int i = 0; i < res.size(); ++i) {
        cout << i << " th: ";
        for (int val : res[i]) cout << val << " ";
        cout << endl;
    }
}

int main() {
    // 各壺の容量の入力
    cout << "Cap: ";
    Node cap(3);
    for (int& val : cap) cin >> val;

    // 各壺に最初に入っている油の容量の入力
    cout << "Start: ";
    Node start(3);
    for (int& val : start) cin >> val;

    // 実現したい、各壺の油の容量の入力
    cout << "Goal: ";
    Node goal(3);
    for (int& val : goal) cin >> val;

    // 油分け算を解く
    solve(cap, start, goal);
}
