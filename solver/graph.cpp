#include "icfpclib.h"
#include <boost/geometry/geometries/polygon.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/algorithms/area.hpp>
using namespace icfpc;
#define rep(i, n) for (int i = 0; i < (int)(n); ++i)

vector<L> segmentArrangement(const vector<L> &skeleton) {
  const auto &ss = skeleton;
  vector<P> ps;
  for (int i = 0; i < (int)skeleton.size(); ++i) {
    ps.push_back(ss[i].a);
    ps.push_back(ss[i].b);
    for (int j = i + 1; j < (int)ss.size(); ++j) {
      // iSSは端を含むが、後でuniqueするので気にしない
      if (isIntersectSS(ss[i], ss[j])) {
        ps.push_back(crosspointLL(ss[i], ss[j]));
      }
    }
  }
  sort(ps.begin(), ps.end());
  ps.erase(unique(ps.begin(), ps.end()), ps.end());

  vector<L> retval;
  for (int i = 0; i < (int)ss.size(); ++i) {
    vector<pair<BigRational, int>> li;
    for (int j = 0; j < (int)ps.size(); ++j) {
      if (isIntersectSP(ss[i], ps[j])) {
        BigRational dx = ss[i].a.x - ps[j].x;
        BigRational dy = ss[i].a.y - ps[j].y;
        BigRational norm2 = dx * dx + dy * dy;
        li.emplace_back(norm2, j);
      }
    }
    sort(li.begin(), li.end());
    for (int j = 0; j < (int)li.size() - 1; ++j) {
      int a = li[j].second;
      int b = li[j + 1].second;
      retval.emplace_back(ps[a], ps[b]);
    }
  }
  return retval;
}

bool isShareEdge(const L &s, const L &t) {
  return ((isIntersectSP(s, t.a) && isIntersectSP(s, t.b)) || (isIntersectSP(t, s.a) && isIntersectSP(t, s.b)));
}

vector<vector<int>> createGraph(const vector<G> &g) {
  vector<vector<int>> retval(g.size());
  for (int i = 0; i < (int)g.size(); ++i) {
    for (int j = i + 1; j < (int)g.size(); ++j) {
      rep(p1, g[i].size()) {
        rep(p2, g[j].size()) {
          L s(g[i][p1], g[i][(p1 + 1) % g[i].size()]);
          L t(g[j][p2], g[j][(p2 + 1) % g[j].size()]);
          if (isShareEdge(s, t)) {
            retval[i].push_back(j);
            retval[j].push_back(i);
          }
        }
      }
    }
  }
  return retval;
}

void printForVisualizer(const vector<L> &skeleton) {
  cout << "== output ==" << endl;
  cout << 0 << endl;
  cout << skeleton.size() << endl;
  for (auto &&l : skeleton) {
    cout << l.a.x << ',' << l.a.y << ' ' << l.b.x << ',' << l.b.y << endl;
  }
}

P otherSide(const L &l, const P &p) {
  if (l.a == p)
    return l.b;
  return l.a;
}

P turnRight(const vector<L> &skeleton, const vector<int> &intersection, const P &st, const P &en) {
  assert(intersection.size() >= 2);
  vector<P> crossMinus, crossPlus;
  rep(i, intersection.size()) {
    const P &p = otherSide(skeleton[intersection[i]], en);
    if (p == st) {
      continue;
    }
    const int c = ccw(st, en, p);
    assert(c == 1 || c == -1 || c == -2);
    if (c == -1) {
      crossMinus.push_back(p);
    } else {
      crossPlus.push_back(p);
    }
  }
  vector<P> target;
  if (crossMinus.size()) {
    target = std::move(crossMinus);
  } else {
    target = std::move(crossPlus);
  }
  int idx = 0;
  for (int j = 1; j < (int)target.size(); ++j) {
    const int c = ccw(en, target[idx], target[j]);
    assert(c == 1 || c == -1);
    if (c == -1)
      idx = j;
  }
  return target[idx];
}

P turnLeft(const vector<L> &skeleton, const vector<int> &intersection, const P &st, const P &en) {
  assert(intersection.size() >= 2);
  vector<P> crossMinus, crossPlus;
  rep(i, intersection.size()) {
    const P &p = otherSide(skeleton[intersection[i]], en);
    if (p == st) {
      continue;
    }
    const int c = ccw(st, en, p);
    assert(c == 1 || c == -1 || c == -2);
    if (c == -1) {
      crossMinus.push_back(p);
    } else {
      crossPlus.push_back(p);
    }
  }
  vector<P> target;
  if (crossPlus.size()) {
    target = std::move(crossPlus);
  } else {
    target = std::move(crossMinus);
  }
  int idx = 0;
  for (int j = 1; j < (int)target.size(); ++j) {
    const int c = ccw(en, target[idx], target[j]);
    assert(c == 1 || c == -1);
    if (c == 1)
      idx = j;
  }
  return target[idx];
}

bool include(const G &g, const G &h) {
  typedef boost::geometry::model::d2::point_xy<BigRational> point_type;
  typedef boost::geometry::model::polygon<point_type> polygon;
  polygon polyG, polyH;
  for(const auto &p : g) polyG.outer().push_back(point_type(p.x, p.y));
  for(const auto &p : h) polyH.outer().push_back(point_type(p.x, p.y));

  vector<polygon> inters;
  boost::geometry::intersection(polyG, polyH, inters);
  if(inters.size() == 0) return false;
  return boost::geometry::area(h) == boost::geometry::area(inters[0]);
}

vector<G> splitFacet(const vector<L> &skeleton) {
  map<P, vector<int>> intersection;
  rep(i, skeleton.size()) {
    intersection[skeleton[i].a].push_back(i);
    intersection[skeleton[i].b].push_back(i);
  }
  vector<G> retval;
  rep(i, skeleton.size()) {
    // turn right
    P st, en, p, q;
    G g1;
    st = skeleton[i].a;
    en = skeleton[i].b;
    p = st, q = en;
    g1.push_back(p);
    while (q != st) {
      g1.push_back(q);
      P r = turnRight(skeleton, intersection[q], p, q);
      p = q;
      q = r;
    }
    retval.push_back(g1);

    G g2;
    st = skeleton[i].a;
    en = skeleton[i].b;
    p = st, q = en;
    g2.push_back(p);
    while (q != st) {
      g2.push_back(q);
      P r = turnLeft(skeleton, intersection[q], p, q);
      p = q;
      q = r;
    }
    retval.push_back(g2);

    G g3;
    st = skeleton[i].b;
    en = skeleton[i].a;
    p = st, q = en;
    g3.push_back(p);
    while (q != st) {
      g3.push_back(q);
      P r = turnRight(skeleton, intersection[q], p, q);
      p = q;
      q = r;
    }
    retval.push_back(g3);

    G g4;
    st = skeleton[i].b;
    en = skeleton[i].a;
    p = st, q = en;
    g4.push_back(p);
    while (q != st) {
      g4.push_back(q);
      P r = turnLeft(skeleton, intersection[q], p, q);
      p = q;
      q = r;
    }
    retval.push_back(g4);
  }

  vector<bool> live(retval.size(), true);
  for(int i = 0; i < (int)retval.size(); ++i) {
    for(int j = i + 1; j < (int)retval.size(); ++j) {
      if(live[j] && include(retval[i], retval[j]) && include(retval[j], retval[i])) {
        live[j] = false;
      }
    }
  }
  rep(i, retval.size()) {
    if(live[i]) {
      cerr << "[" << i << "] " << retval[i] << endl;
    }
  }
  vector<G> tmp;
  rep(i, retval.size()) {
    if(live[i] == false) continue;
    rep(j, retval.size()) {
      if(j == i) continue;
      if(live[j] == false) continue;
      if(include(retval[i], retval[j])) {
        cerr << i << " include " << j << endl;
        live[i] = false;
        break;
      }
    }
    if(live[i]) {
      tmp.push_back(retval[i]);
    }
  }

  return tmp;
  // return retval;
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

  vector<L> skeleton2 = segmentArrangement(skeleton);
  vector<G> facets = splitFacet(skeleton2);
  for(const auto &g : facets) {
    cout << g << endl;
  }
}
