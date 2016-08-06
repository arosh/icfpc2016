//最も左下にある点を見つけて、そこを左下頂点とする正方形を答えとして出力する。
//「一応正の点を取ろうな」というコンセプト
//目標多角形が小さいと異様にスコアが低くなる気がする

//yurahunaさんのコードを参考に、多倍長整数を含む入力にも対応した。

#include <iostream>
#include <boost/rational.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>	//多倍長小数点数
using namespace std;
typedef boost::multiprecision::cpp_int BigInt;
typedef boost::multiprecision::cpp_dec_float_100 BigDouble;
typedef boost::rational<BigInt> BigRational;
#define rep(i,n) for (int i=0;i<(n);i++)
#define all(a) (a).begin(),(a).end()

struct P {
	BigRational x, y;
	P(){}
	P(BigRational _x, BigRational _y) : x(_x), y(_y) {}
	const P operator+ (const P& p) {
		return P(this->x + p.x, this->y + p.y);
	}
	const P operator- (const P& p) {
		return P(this->x - p.x, this->y - p.y);
	}
	friend std::ostream& operator<<(std::ostream& os, const P& p) {
		os << "(" << p.x << "," << p.y << ")";
		return os;
	}
};

struct L {
	P a, b, v;
	L(){}
	L(P _a, P _b) : a(_a), b(_b), v(b - a) {}
	L(BigRational _ax, BigRational _ay, BigRational _bx, BigRational _by) : L(P(_ax, _ay), P(_bx, _by)) {}
	friend std::ostream& operator<<(std::ostream& os, const L& l) {
		os << "{" << l.a << "," << l.b << "}";
		return os;
	}
};

typedef vector<P> G;
#define here(g, i) g[i]
#define next(g, i) g[(i + 1) % g.size()]
#define prev(g, i) g[(i - 1 + g.size()) % g.size()]
#define printG(g) rep(i, g.size()){cout << g[i] << " ";} cout << endl;

// BigRational cross(const P& a, const P& b) {
//     return a.x * b.y - a.y * b.x;
// }
//
// BigRational dot(const P& a, const P& b) {
//     return a.x * b.x + a.y * b.y;
// }
//
// bool counterClockwiseG(const G& g) {
//     // 符号付き面積が正なら反時計回り
//     BigRational area;
//     rep(i, g.size()) area += cross(here(g, i), next(g, i));
//     return area > BigInt("0");
// }
//

// 座標に出てくる分数(分母は省略されているかも)をBigRationalにして返す
BigRational parse(const string& s) {
	stringstream ss(s);
	if (s.find('/') == string::npos) {
		return BigRational(BigInt(s), BigInt(1));
	}
	else {
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
	rep(i, n) g.emplace_back(readP());
	return g;
}

// void printL(const L& l) {
//     cerr << "{";
//     printP(l.a);
//     printP(l.b);
//     cerr << "}" << endl;
// }
//
// void printG(const G& g) {
//     for (auto&& p : g) printP(p);
// }

int main() {
	int n;  // #polygons in silhoutte
	cin >> n;
	vector<G> silhoutte;
	rep(i, n) silhoutte.emplace_back(readG());
	int m; // #edges in skeleton
	cin >> m;
	vector<L> skeleton;
	rep(i, m) skeleton.emplace_back(readL());
	
	//silhoutte内の多角形が、時計回りか反時計回りかを判定する必要がありそう
	
	BigRational max_x = silhoutte[0][0].x;
	BigRational max_y = silhoutte[0][0].y;
	BigRational min_x = silhoutte[0][0].x;
	BigRational min_y = silhoutte[0][0].y;

	//silhoutte[0]が外枠であると想定。全頂点の凸包したほうがいいかも
	for(int point = 1;point<silhoutte[0].size();point++){
		BigDouble tmp_x = boost::rational_cast<BigDouble>(silhoutte[0][point].x);
		BigDouble tmp_y = boost::rational_cast<BigDouble>(silhoutte[0][point].y);

		BigDouble cur_max_x = boost::rational_cast<BigDouble>(max_x);
		BigDouble cur_max_y = boost::rational_cast<BigDouble>(max_y);
		BigDouble cur_min_x = boost::rational_cast<BigDouble>(min_x);
		BigDouble cur_min_y = boost::rational_cast<BigDouble>(min_y);
		
		if(tmp_x>cur_max_x) max_x = silhoutte[0][point].x;
		if(tmp_x<cur_min_x) min_x = silhoutte[0][point].x;
	
		if(tmp_y>cur_max_y)	max_y = silhoutte[0][point].y;
		if(tmp_y<cur_min_y)	min_y = silhoutte[0][point].y;
	}
	
//	cout<<max_x<<endl;
//	cout<<max_y<<endl;
//	cout<<min_x<<endl;
//	cout<<min_y<<endl;
	
	
	//以下出力
	cout<<4<<endl;
	cout<<"0,0"<<endl;
	cout<<"1,0"<<endl;
	cout<<"1,1"<<endl;
	cout<<"0,1"<<endl;
	
	cout<<1<<endl;
	cout<<"4 0 1 2 3"<<endl;
	
	cout<<min_x  <<","<<min_y  <<endl;
	cout<<min_x+1<<","<<min_y  <<endl;
	cout<<min_x+1<<","<<min_y+1<<endl;
	cout<<min_x  <<","<<min_y+1<<endl;
	
}
