#include "icfpclib.h"
using namespace icfpc;
#define rep(i, n) for(int i = 0; i < (int)(n); ++i)

vector<L> splitSkeleton(const vector<L> &skeleton) {
  const auto &ss = skeleton;
  vector<P> ps;
  for(int i = 0; i < (int)skeleton.size(); ++i) {
    ps.push_back(ss[i].a);
    ps.push_back(ss[i].b);
    for(int j = i + 1; j < (int)ss.size(); ++j) {
      // iSSは端を含むが、後でuniqueするので気にしない
      if(isIntersectSS(ss[i], ss[j])) {
        ps.push_back(crosspointLL(ss[i], ss[j]));
      }
    }
  }
  sort(ps.begin(), ps.end());
  ps.erase(unique(ps.begin(), ps.end()), ps.end());

  vector<L> retval;
  for(int i = 0; i < (int)ss.size(); ++i) {
    vector<pair<BigRational,int>> li;
    for(int j = 0; j  < (int)ps.size(); ++j) {
      if(isIntersectSP(ss[i], ps[j])) {
        BigRational dx = ss[i].a.x - ps[j].x;
        BigRational dy = ss[i].a.y - ps[j].y;
        BigRational norm2 = dx*dx + dy*dy;
        li.emplace_back(norm2, j);
      }
    }
    sort(li.begin(), li.end());
    for(int j = 0; j < (int)li.size() - 1; ++j) {
      int a = li[j].second;
      int b = li[j+1].second;
      retval.emplace_back(ps[a], ps[b]);
    }
  }
  return retval;
}

bool isShareEdge(const L &s, const L &t) {
  return ((isIntersectSP(s, t.a) && isIntersectSP(s, t.b))
      || (isIntersectSP(t, s.a) && isIntersectSP(t, s.b)));
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
