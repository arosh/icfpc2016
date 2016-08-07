#include "icfpclib.h"
using namespace icfpc;
#define rep(i, n) for(int i = 0; i < (int)(n); ++i)

vector<L> splitSkeleton(const vector<L> &skeleton) {
  vector<L> tmp = skeleton;
  while (true) {
    bool update = false;
    for(int i = 0; i < (int)tmp.size(); ++i) {
      for(int j = i + 1; j < (int)tmp.size(); ++j) {
        // 線分の端を含んで交差判定した時，trueになるのは3通りある
        // 1. 十字形
        // 2. T字型
        // 3. a - b - c 型
        // ここでは1と2だけを気にする必要がある
        if(isIntersectSS(tmp[i], tmp[j])
            && tmp[i].a != tmp[j].a && tmp[i].a != tmp[j].b
            && tmp[i].b != tmp[j].a && tmp[i].b != tmp[j].b) {
          P p = crosspointLL(tmp[i], tmp[j]);
          vector<L> extend;
          if(tmp[i].a != p) extend.push_back(L(tmp[i].a, p));
          if(tmp[i].b != p) extend.push_back(L(tmp[i].b, p));
          if(tmp[j].a != p) extend.push_back(L(tmp[j].a, p));
          if(tmp[j].b != p) extend.push_back(L(tmp[j].b, p));
          if(extend.size() > 2) {
            // vectorから複数要素を消すのむずい
            tmp.erase(tmp.begin() + i);
            tmp.erase(tmp.begin() + j - 1);
            tmp.insert(tmp.end(), extend.begin(), extend.end());
            update = true;
            break;
          }
        }
      }
      if(update) break;
    }
    if(!update) break;
  }
  return tmp;
}

bool isShareEdge(const L &s, const L &t) {
  return ((isIntersectSP(s.a, s.b, t.a) && isIntersectSP(s.a, s.b, t.b))
      || (isIntersectSP(t.a, t.b, s.a) && isIntersectSP(t.a, t.b, s.b)));
}

vector<vector<int>> createGraph(const vector<G> &g) {
  vector<vector<int>> retval(g.size());
  for(int i = 0; i < (int)g.size(); ++i) {
    for(int j = i + 1; j < (int)g.size(); ++j) {
      rep(p1, g[i].size()) {
        rep(p2, g[j].size()) {
          L s(g[i][p1], g[i][(p1 + 1) % g[i].size()]);
          L t(g[j][p2], g[j][(p2 + 1) % g[j].size()]);
          if(isShareEdge(s, t)) {
            retval[i].push_back(j);
            retval[j].push_back(i);
          }
        }
      }
    }
  }
  return retval;
}

int main() {
  int n; // #polygons in silhoutte
  cin >> n;
  vector<G> silhoutte;
  rep(i, n) silhoutte.emplace_back(readG());
  
  int m; // #edges in skeleton
  cin >> m;
  vector<L> skeleton;
  rep(i, m) skeleton.emplace_back(readL());

  vector<L> skeleton2 = splitSkeleton(skeleton);
  cout << "== output ==" << endl;
  cout << 0 << endl;
  cout << skeleton2.size() << endl;
  for(auto &&l : skeleton2) {
    // ビジュアライザに貼り付ける用のフォーマット
    cout << l.a.x << ',' << l.a.y << ' ' << l.b.x << ',' << l.b.y << endl;
  }
}
