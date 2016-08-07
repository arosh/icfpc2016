#include <boost/multiprecision/cpp_int.hpp>
#include <boost/rational.hpp>
namespace icfpc {
using namespace std;
typedef boost::multiprecision::cpp_int BigInt;
typedef boost::rational<BigInt> BigRational;
struct P {
  BigRational x, y;
  P() {}
  P(BigRational _x, BigRational _y) : x(_x), y(_y) {}
  P operator+(const P &p) const {
    return P(this->x + p.x, this->y + p.y);
  }
  P operator-(const P &p) const {
    return P(this->x - p.x, this->y - p.y);
  }
  P operator*(const BigRational &r) const {
    return P(this->x * r, this->y * r);
  }
  bool operator==(const P &p) const {
    return this->x == p.x && this->y == p.y;
  }
  bool operator!=(const P &p) const {
    return !(*this == p);
  }
  bool operator <(const P &p) const {
    return tie(this->x, this->y) < tie(p.x, p.y);
  }
  friend std::ostream &operator<<(std::ostream &os, const P &p) {
    os << "(" << p.x << "," << p.y << ")";
    return os;
  }
};

struct L {
  P a, b, v;
  L() {}
  L(P _a, P _b) : a(_a), b(_b), v(b - a) {}
  L(BigRational _ax, BigRational _ay, BigRational _bx, BigRational _by) : L(P(_ax, _ay), P(_bx, _by)) {}
  friend std::ostream &operator<<(std::ostream &os, const L &l) {
    os << "{" << l.a << "," << l.b << "}";
    return os;
  }
};

typedef L S;
typedef vector<P> G;

///////////// 入出力関数 //////////////

// 座標に出てくる分数(分母は省略されているかも)をBigRationalにして返す
BigRational parse(const string &s) {
  stringstream ss(s);
  if (s.find('/') == string::npos) {
    return BigRational(BigInt(s));
  } else {
    string sa, sb;
    getline(ss, sa, '/');
    getline(ss, sb, '/');
    return BigRational(BigInt(sa), BigInt(sb));
  }
}

P readP() {
  string s;
  cin >> s;
  stringstream ss(s);
  string sx;
  getline(ss, sx, ',');
  BigRational x = parse(sx);
  string sy;
  getline(ss, sy, ',');
  BigRational y = parse(sy);
  return P(x, y);
}

L readL() {
  P pa = readP();
  P pb = readP();
  return L(pa, pb);
}

G readG() {
  int n;
  cin >> n;
  G g;
  for(int i = 0; i < n; ++i) {
    g.emplace_back(readP());
  }
  return g;
}

// vector用出力器(G, vector<T> などに使える)
template <class T>
ostream &operator<<(ostream &os, const vector<T> &xs) {
  bool first = true;
  for (const auto &x : xs) {
    if (!first)
      os << ' ';
    first = false;
    os << x;
  }
  return os;
}

/////////////// 計算 ///////////////

BigRational cross(const P &a, const P &b) {
  return a.x * b.y - a.y * b.x;
}

BigRational dot(const P &a, const P &b) {
  return a.x * b.x + a.y * b.y;
}

BigRational norm(const P &a) {
  return a.x * a.x + a.y * a.y;
}

bool counterClockwiseG(const G &g) {
  // 符号付き面積が正なら反時計回り
  BigRational area;
  for(int i = 0; i < (int)g.size(); ++i) {
    area += cross(g[i], g[(i + 1) % g.size()]);
  }
  return area > BigInt("0");
}

int ccw(P a, P b, P c) {
  b = b - a;
  c = c - a;
  if (cross(b, c) > 0)
    return +1; // counter clockwise
  if (cross(b, c) < 0)
    return -1; // clockwise
  if (dot(b, c) < 0)
    return +2; // c--a--b on line
  if (norm(b) < norm(c))
    return -2; // a--b--c on line
  return 0;
}

bool isIntersectSS(const L &s, const L &t) {
  // 端を含む
  return ccw(s.a, s.b, t.a) * ccw(s.a, s.b, t.b) <= 0 &&
    ccw(t.a, t.b, s.a) * ccw(t.a, t.b, s.b) <= 0;
}

bool isIntersectSP(L s, P p) {
    return ccw(s.a, s.b, p) == 0;
}

P crosspointLL(const L &l, const L &m) {
  BigRational A = cross(l.v, m.v);
  BigRational B = cross(l.v, l.b - m.a);
  return m.a + m.v * (B / A);
}

// gをlで切断し、lの方向ベクトルから見て左側の方を返す
// 直線を逆向きにして呼び出すと多角形の反対側がとれる
G convex_cut(const G &g, const L &l) {
  G h;
  for (int i = 0; i < (int)g.size(); ++i) {
    P p = g[i], q = g[(i + 1) % g.size()];
    if (ccw(l.a, l.b, p) != -1)
      h.push_back(p);
    if (ccw(l.a, l.b, p) * ccw(l.a, l.b, q) < 0)
      h.push_back(crosspointLL(L(p, q), l));
  }
  return h;
}

P projection(const P &p, const L &l) {
  return l.v * (dot(p, l.v) / norm(l.v));
}

// 線対称な点
P symmTransPL(P p, L l) {
  p = p - l.a;
  P t = projection(p, l);
  return t * BigRational(BigInt("2")) - p + l.a;
}

// 線対称な図形
G symmTransGL(const G &g, const L &l) {
  G h;
  for(int i = 0; i < (int)g.size(); ++i) {
    h.emplace_back(symmTransPL(g[i], l));
  }
  return h;
}

// 多角形が直線に対してどちら側にあるか
int relationGL(const G &g, const L &l) {
  for(int i = 0; i < (int)g.size(); ++i) {
    // ccwから時計回り or 反時計回りの結果が返ってきたら、それをそのまま返す
    int t = ccw(l.a, l.b, g[i]);
    if (t == +1 || t == -1)
      return t;
  }
  assert(false);
  return 0; // ここには来ないはず
}

///////////// Origami ////////////////

typedef G Facet;
static const Facet initial_origami = {
  P(BigRational(BigInt("0")), BigRational(BigInt("0"))),
  P(BigRational(BigInt("1")), BigRational(BigInt("0"))),
  P(BigRational(BigInt("1")), BigRational(BigInt("1"))),
  P(BigRational(BigInt("0")), BigRational(BigInt("1")))
};

class Origami {
private:
  vector<Facet> facets;
  // 各面に対し、切断されるならば2つに分ける
  void cutFacets(const L &l) {
    vector<Facet> nxt_facets;
    for (auto &&f : facets) {
      G g = convex_cut(f, l);
      nxt_facets.emplace_back(g);
      if (g != f) {
        // 切断されていたら反対側も入れる
        nxt_facets.emplace_back(convex_cut(f, L(l.b, l.a)));
      }
    }
    facets = nxt_facets;
  }
  // 各面に対し、直線の右側にあるならば左側に移動
  void transFacets(const L &l) {
    vector<Facet> nxt_facets;
    for (auto &&f : facets) {
      nxt_facets.emplace_back((relationGL(f, l) == +1 ? symmTransGL(f, l) : f));
    }
    facets = nxt_facets;
  }

public:
  Origami() {
    facets.emplace_back(initial_origami);
  }
  void update(const L &l) {
    cutFacets(l);
    transFacets(l);
  }
  friend std::ostream &operator<<(std::ostream &os, const Origami &o) {
    for (auto &&f : o.facets)
      os << f << endl;
    os << endl;
    return os;
  }
};
};
