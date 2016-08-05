//最も左下にある点を見つけて、そこを左下頂点とする正方形を答えとして出力する。
//「一応正の点を取ろうな」というコンセプト
//目標多角形が小さいと異様にスコアが低くなる気がする

#include "bits/stdc++.h"
using namespace std;
typedef long long ll;
typedef pair<int,int> pii;
typedef pair<double,double> pdd;
#define rep(i,n) for(ll i=0;i<(ll)(n);i++)
#define all(a)  (a).begin(),(a).end()
#define vi vector<int>
#define pb push_back
#define INF 999999999
//#define INF (1LL<<59)


vector<string> split(string s,char ch){
	vector<string> ret;
	string tmp ="";
	rep(i,s.size()){
		if(s[i]==ch){
			ret.pb(tmp);
			tmp = "";
		}else tmp+=s[i];
	}
	if(tmp.size())ret.pb(tmp);
	return ret;
}


void output(double a, double b){
	int  decimal, sign;
	char *buffer;
	
	buffer = fcvt( a, 5, &decimal, &sign ); // C4996
  while(*buffer == '0') ++buffer;
  if(*buffer == '\0') printf("0,");
  else printf( "%s/100000,",buffer);

	buffer = fcvt( b, 5, &decimal, &sign ); // C4996
  while(*buffer == '0') ++buffer;
  if(*buffer == '\0') printf("0\n");
  else printf( "%s/100000\n",buffer);
}

int main(){
	int polygon_num;
	
	cin>>polygon_num;
	vector<pdd> points;
	rep(pol_loop,polygon_num){
		int v;
		cin>>v;
		rep(i,v){
			string s;
			cin>>s;
			vector<string> t = split(s,',');
			
			string a = t[0];
			string b = t[1];
			vector<string> va = split(a,'/');
			vector<string> vb = split(b,'/');
			
			
			double x,y;
			if(va.size()==1)x = stol(va[0]);
			else x = (double)stol(va[0])/stol(va[1]);
			if(vb.size()==1)y = stol(vb[0]);
			else y = (double)stol(vb[0])/stol(vb[1]);
			points.pb(pdd(x,y));
		}
	}
	
	//以下読み捨て
	int e;
	cin>>e;
	rep(i,4){
		string a,b;
		cin>>a>>b;
	}
	
	
	sort(all(points));
	
	double smallx = points[0].first;
	double smally = points[0].second;
	
	
	
	//以下出力
	cout<<4<<endl;
	cout<<"0,0"<<endl;
	cout<<"1,0"<<endl;
	cout<<"1,1"<<endl;
	cout<<"0,1"<<endl;

	cout<<1<<endl;
	cout<<"4 0 1 2 3"<<endl;
	
	output(smallx  ,smally  );
	output(smallx+1,smally  );
	output(smallx+1,smally+1);
	output(smallx  ,smally+1);
	
}

