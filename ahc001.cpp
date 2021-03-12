#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <utility>
#include <cstdio>
using namespace std;

const int W = 10000;
const int H = 10000;

int score(int n, vector<int> x, vector<int> y, vector<int> r,
    vector<int> a, vector<int> b, vector<int> c, vector<int> d)
{
    double p = 0;
    //  TODO: 重複チェック
    for (int i=0; i<n; i++)
    {
        if (a[i]<=x[i] && x[i]<c[i] &&
            b[i]<=y[i] && y[i]<d[i])
        {
            int s = (c[i]-a[i])*(d[i]-b[i]);
            double t = (1-(double)min(r[i], s)/max(r[i], s));
            //cout<<1-t*t<<" "<<r[i]-s<<endl;
            p += 1-t*t;
        }
        else
        {
            cerr<<"!!!"<<endl;
        }
    }
    return int(1e9*p/n+.5);
}

int xor64(void) {
    static uint64_t x = 88172645463325252ULL;
    x ^= x<<13;
    x ^= x>> 7;
    x ^= x<<17;
    return int(x&0x7fffffff);
}

struct Ad
{
    int x, y, r;
    int x1, y1, x2, y2;
    int w() const {return x2-x1;}
    int h() const {return y2-y1;}
    int s() const {return w()*h();}
    int score() const
    {
        // (1-(1-min/max)**2)*1e9/n
        // = (1-(max-min)**2/max**2)*1e9/n
        // = 1e9/n-(max-min)**2*1e9/n/max**2
        long long mx = max(r, s());
        long long mn = min(r, s());
        long long t = 1'000'000'000/200;
        return int(t-t*(mx-mn)/mx*(mx-mn)/mx);
    }
};

bool intersect(const Ad &a1, const Ad &a2)
{
    return !(
        a1.x2<=a2.x1 ||
        a2.x2<=a1.x1 ||
        a1.y2<=a2.y1 ||
        a2.y2<=a1.y1);
}

vector<vector<int>> solve(int n, vector<int> x_, vector<int> y_, vector<int> r_)
{
    chrono::system_clock::time_point start = chrono::system_clock::now();

    vector<Ad> ad(n);
    for (int i=0; i<n; i++)
    {
        ad[i].x = x_[i];
        ad[i].y = y_[i];
        ad[i].r = r_[i];
    }

    int score = 0;
    for (int i=0; i<n; i++)
    {
        ad[i].x1 = ad[i].x;
        ad[i].y1 = ad[i].y;
        ad[i].x2 = ad[i].x+1;
        ad[i].y2 = ad[i].y+1;
        score += ad[i].score();
    }

    //  ←↑→↓
    int dx1[] = {-1, 0, 0, 0};
    int dy1[] = {0, -1, 0, 0};
    int dx2[] = {0, 0, 1, 0};
    int dy2[] = {0, 0, 0, 1};

    vector<pair<int, Ad>> Q;

    double limit = 4.8;
    double time = 0;
    double temp_start = 1'000'000'000./200/10;
    double temp_end = 1'000'000'000./200/10000;
    double temp = 0;

    vector<vector<int>> X(W);
    vector<vector<int>> Y(H);
    for (Ad &a: ad)
    {
        X[a.y].push_back(a.x);
        Y[a.x].push_back(a.y);
    }

    int best = 0;
    vector<vector<int>> best_ans(4, vector<int>(n));

    const int iter_max = -1; //5000000;
    int iter;
    for (iter=0; ; iter++)
    {
        if (iter%0x10000==0)
        {
            if (iter_max<0)
            {
                chrono::system_clock::time_point now = chrono::system_clock::now();
                time = chrono::duration_cast<chrono::microseconds>(now-start).count()*1e-6/limit;
            }
            else
                time = (double)iter/iter_max;
            if (time>=1.0)
                break;
            temp = temp_start+(temp_end-temp_start)*time;
        }

        int p = xor64()%n;
        Ad &a = ad[p];
        if (a.s()>=a.r)
            continue;

        //  拡張方向
        int ed;
        //  拡張する長さ
        //  伸ばす場合は以下の最小値
        //  - 盤外に出ない
        //  - スコアが最大
        //  - 他の(x, y)を覆わない
        //  縮める場合は以下の最大値
        //  - 自分の(x, y)を覆ったまま
        int el;
        if (xor64()%4!=0)
        {
            if (xor64()%4==0)
                ed = xor64()%4;
            else
                if (xor64()%(a.w()+a.h())<a.w())
                    ed = xor64()%2*2+1;
                else
                    ed = xor64()%2*2;

            el = W;
            switch (ed)
            {
            case 0:
                el = min(el, a.x1);
                el = min(el, a.r/a.h()-a.w());
                for (int x=a.x1-1; x>=a.x1-el; x--)
                    for (int y: Y[x])
                        if (a.y1<=y && y<a.y2)
                            el = a.x1-x-1;
                break;
            case 1:
                el = min(el, a.y1);
                el = min(el, a.r/a.w()-a.h());
                for (int y=a.y1-1; y>=a.y1-el; y--)
                    for (int x: X[y])
                        if (a.x1<=x && x<a.x2)
                            el = a.y1-y-1;
                break;
            case 2:
                el = min(el, W-a.x2);
                el = min(el, a.r/a.h()-a.w());
                for (int x=a.x2; x<a.x2+el; x++)
                    for (int y: Y[x])
                        if (a.y1<=y && y<a.y2)
                            el = x-a.x2;
                break;
            case 3:
                el = min(el, H-a.y2);
                el = min(el, a.r/a.w()-a.h());
                for (int y=a.y2; y<a.y2+el; y++)
                    for (int x: X[y])
                        if (a.x1<=x && x<a.x2)
                            el = y-a.y2;
                break;
            }
            if (el==0)
                continue;
            el = xor64()%el+1;
        }
        else
        {
            if (xor64()%4==0)
                ed = xor64()%4;
            else
                if (xor64()%(a.w()+a.h())<a.w())
                    ed = xor64()%2*2;
                else
                    ed = xor64()%2*2+1;

            el = -W;
            switch (ed)
            {
            case 0:
                el = max(el, a.x1-a.x);
                break;
            case 1:
                el = max(el, a.y1-a.y);
                break;
            case 2:
                el = max(el, a.x-a.x2+1);
                break;
            case 3:
                el = max(el, a.y-a.y2+1);
                break;
            }
            if (el==0)
                continue;
            el = -(xor64()%-el+1);
        }

        int score_old = score;
        //  拡張／縮小
        score -= a.score();
        Q.push_back(make_pair(p, a));
        a.x1 += dx1[ed]*el;
        a.y1 += dy1[ed]*el;
        a.x2 += dx2[ed]*el;
        a.y2 += dy2[ed]*el;
        score += a.score();

        //  衝突している広告を縮める
        if (el>0)
            for (int i=0; i<n; i++)
                if (i!=p && intersect(a, ad[i]))
                {
                    int c = 0;
                    Ad cand[4];
                    for (int sd=0; sd<4; sd++)
                        switch (sd)
                        {
                        case 0:
                            if (ad[i].x1<a.x2 && a.x2<=ad[i].x)
                            {
                                cand[c] = ad[i];
                                cand[c].x1 = a.x2;
                                c++;
                            }
                            break;
                        case 1:
                            if (ad[i].y1<a.y2 && a.y2<=ad[i].y)
                            {
                                cand[c] = ad[i];
                                cand[c].y1 = a.y2;
                                c++;
                            }
                            break;
                        case 2:
                            if (ad[i].x2>a.x1 && a.x1>ad[i].x)
                            {
                                cand[c] = ad[i];
                                cand[c].x2 = a.x1;
                                c++;
                            }
                            break;
                        case 3:
                            if (ad[i].y2>a.y1 && a.y1>ad[i].y)
                            {
                                cand[c] = ad[i];
                                cand[c].y2 = a.y1;
                                c++;
                            }
                            break;
                        }
                    int m = -1;
                    for (int j=0; j<c; j++)
                        if (m==-1 || cand[j].score()>cand[m].score())
                            m = j;
                    score -= ad[i].score();
                    Q.push_back(make_pair(i, ad[i]));
                    ad[i] = cand[m];
                    score += ad[i].score();
                }

        if (score>best)
        {
            best = score;
            for (int i=0; i<n; i++)
            {
                best_ans[0][i] = ad[i].x1;
                best_ans[1][i] = ad[i].y1;
                best_ans[2][i] = ad[i].x2;
                best_ans[3][i] = ad[i].y2;
            }
        }

        double prob = exp(double(score-score_old)/temp);
        if (prob<(double)(xor64()%0x10000)/0x10000)
        {
            score = score_old;
            for (auto &q: Q)
                ad[q.first] = q.second;
        }
        Q.clear();
    }
#ifdef LOCAL
    cout<<"iter: "<<iter<<endl;
#endif

    return best_ans;
}

void evaluate()
{
    int N = 10;
    long long sum = 0;
    for (int i=0; i<N; i++)
    {
        char fname[10];
        sprintf(fname, "%04d.txt", i);
        ifstream in(string("in\\")+fname);
        int n;
        in>>n;
        vector<int> x(n), y(n), r(n);
        for (int j=0; j<n; j++)
            in>>x[j]>>y[j]>>r[j];

        chrono::system_clock::time_point start = chrono::system_clock::now();
        vector<vector<int>> t = solve(n, x, y, r);
        chrono::system_clock::time_point end = chrono::system_clock::now();

        vector<int> a = t[0];
        vector<int> b = t[1];
        vector<int> c = t[2];
        vector<int> d = t[3];

        ofstream out(string("out\\")+fname);
        for (int j=0; j<n; j++)
            out<<a[j]<<" "<<b[j]<<" "<<c[j]<<" "<<d[j]<<endl;

        long long tm = chrono::duration_cast<chrono::milliseconds>(end-start).count();
        int s = score(n, x, y, r, a, b, c, d);
        printf("%2d: %5.3f %10d\n", i, tm*1e-3, s);

        sum += s;
    }
    printf("sum: %lld\n", sum);
}


int main()
{
#ifdef LOCAL
    evaluate();
#else
    int n;
    cin>>n;
    vector<int> x(n), y(n), r(n);
    for (int i=0; i<n; i++)
        cin>>x[i]>>y[i]>>r[i];

    vector<vector<int>> t = solve(n, x, y, r);

    vector<int> a = t[0];
    vector<int> b = t[1];
    vector<int> c = t[2];
    vector<int> d = t[3];
    for (int i=0; i<n; i++)
        cout<<a[i]<<" "<<b[i]<<" "<<c[i]<<" "<<d[i]<<endl;
#endif
}
