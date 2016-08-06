//最も左下にある点を見つけて、そこを左下頂点とする正方形を答えとして出力する。
//「一応正の点を取ろうな」というコンセプト
//目標多角形が小さいと異様にスコアが低くなる気がする

//yurahunaさんのコードを参考に、多倍長整数を含む入力にも対応した。

//BigRationalに書き換えた
//時計回り/反時計回りの判定を入れた

#include <iostream>
#include <boost/rational.hpp>
#include <boost/multiprecision/cpp_int.hpp>
using namespace std;
typedef boost::multiprecision::cpp_int BigInt;
typedef boost::rational<BigInt> BigRational;

#define rep(i,n) for (int i=0;i<(n);i++)
#define rep2(i,a,b) for (int i=(a);i<(b);i++)
#define rrep(i,n) for (int i=(n)-1;i>=0;i--)
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

BigRational cross(const P& a, const P& b) {
	return a.x * b.y - a.y * b.x;
}

BigRational dot(const P& a, const P& b) {
	return a.x * b.x + a.y * b.y;
}

bool counterClockwiseG(const G& g) {
	// 符号付き面積が正なら反時計回り
	BigRational area;
	rep(i, g.size()) area += cross(here(g, i), next(g, i));
	return area > BigInt("0");
}


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

int main() {
	int n;  // #polygons in silhoutte
	cin >> n;
	vector<G> silhoutte;
	rep(i, n) silhoutte.emplace_back(readG());
	int m; // #edges in skeleton
	cin >> m;
	vector<L> skeleton;
	rep(i, m) skeleton.emplace_back(readL());
	
//	cout << "silhoutte:" << endl;
//	for (auto&& g : silhoutte) {
//		printG(g);
//	}
//	
//	cout << "skeleton:" << endl;
//	for (auto&& l : skeleton) {
//		cout << l << endl;
//	}
	
//	// silhoutte内の多角形が、時計回りか反時計回りかを判定
//	// 判定しているだけでまだ使ってない
//	vector<bool> ccw_silhoutte(n);
//	rep(i, n) ccw_silhoutte[i] = counterClockwiseG(silhoutte[i]);
//	cout << "ccw_silhoutte:" << endl;
//	rep(i, n) {
//		cout << i << ": " << (ccw_silhoutte[i] ? "YES" : "NO") << endl;
//	}
//	cout << endl;
	
	BigRational max_x = silhoutte[0][0].x;
	BigRational max_y = silhoutte[0][0].y;
	BigRational min_x = silhoutte[0][0].x;
	BigRational min_y = silhoutte[0][0].y;
	
	// silhoutteに現れる全頂点について、x,yのmin,maxを求める
	// BigRationalのまま計算
	for (auto g : silhoutte) {
		for (auto p : g) {
			max_x = max(max_x, p.x);
			max_y = max(max_y, p.y);
			min_x = min(min_x, p.x);
			min_y = min(min_y, p.y);
		}
	}
	
	
	BigRational w = max_x - min_x;
	BigRational h = max_y - min_y;
	
	BigRational dw(1,1);
	BigRational dh(1,1);
	
	while(dw >= w*2)dw /= 2;
	while(dh >= h*2)dh /= 2;
	
//	cout<<dw<<endl;
//	cout<<dh<<endl;

	int hp_sum = boost::rational_cast<int>(1/dh)+1;
	int wp_sum = boost::rational_cast<int>(1/dw)+1;
	
	//頂点を出力
	cout<<hp_sum*wp_sum<<endl;
	for(BigRational hi = BigRational(0/1);hi<=1;hi+=dh){
		for(BigRational wi = BigRational(0/1);wi<=1;wi+=dw){
			cout<<hi<<","<<wi<<endl;
		}
	}
	
	//面(長方形)を出力
	cout<<(hp_sum-1)*(wp_sum-1)<<endl;
	rep(i,hp_sum-1){
		rep(j,wp_sum-1){
			cout<<4<<" "<<wp_sum*i+j<<" "<<wp_sum*i+j +1<<" "<<wp_sum*i+j +wp_sum+1<<" "<<wp_sum*i+j +wp_sum<<endl;
		}
	}

	
	rep(i,hp_sum){
		rep(j,wp_sum){
			if(i%2==0){
				if(j%2==0)	cout<<min_x   <<","<<min_y   <<endl;
				else		cout<<min_x+dw<<","<<min_y   <<endl;
			}
			else{
				if(j%2==0)	cout<<min_x   <<","<<min_y+dh<<endl;
				else		cout<<min_x+dw<<","<<min_y+dh<<endl;
			}
		}
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
//	
//	//以下出力
//	cout<<4<<endl;
//	cout<<"0,0"<<endl;
//	cout<<"1,0"<<endl;
//	cout<<"1,1"<<endl;
//	cout<<"0,1"<<endl;
//	
//	cout<<1<<endl;
//	cout<<"4 0 1 2 3"<<endl;
//	
//	cout<<min_x  <<","<<min_y  <<endl;
//	cout<<min_x+1<<","<<min_y  <<endl;
//	cout<<min_x+1<<","<<min_y+1<<endl;
//	cout<<min_x  <<","<<min_y+1<<endl;
//	
	// int V;  // #vertices in source position
	// int F;  // #facets in source position
	//
	// vector<string> p = {"0,0",
	//                     "1,0",
	//                     "1,1",
	//                     "0,1"};
	//
	// vector<string> q = {"1,1",
	//                     "2,1",
	//                     "2,2",
	//                     "1,2"};
	//
	// // 1回も折らない場合
	// V = 4;
	// F = 1;
	// cout << V << endl;
	// rep(i, V) cout << p[i] << endl;
	// cout << F << endl;
	// cout << "4 0 1 2 3" << endl;
	// rep(i, V) cout << q[i] << endl;
}
