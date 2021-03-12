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
            p += 1-t*t;
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
    double limit = 4.5;

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

    double time = 0;
    double temp_start = 1'000'000'000./200;
    double temp_end = 0.;
    double temp = 0;

    int best = 0;
    vector<vector<int>> best_ans(4, vector<int>(n));

    int iter;
    for (iter=0; ; iter++)
    {
        if (iter%0x100000)
        {
            chrono::system_clock::time_point now = chrono::system_clock::now();
            time = chrono::duration_cast<chrono::microseconds>(now-start).count()*1e-6/limit;
            if (time>=1.0)
                break;
            temp = temp_start+(temp_end-temp_start)*time;
        }

        int p = xor64()%n;
        if (ad[p].s()>=ad[p].r)
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
                if (xor64()%(ad[p].w()+ad[p].h())<ad[p].w())
                    ed = xor64()%2*2+1;
                else
                    ed = xor64()%2*2;

            el = W;
            switch (ed)
            {
            case 0:
                el = min(el, ad[p].x1);
                el = min(el, ad[p].r/ad[p].h()-ad[p].w());
                for (int i=0; i<n; i++)
                    if (i!=p && ad[p].y1<=ad[i].y && ad[i].y<ad[p].y2 && ad[i].x<ad[p].x1)
                        el = min(el, ad[p].x1-ad[i].x-1);
                break;
            case 1:
                el = min(el, ad[p].y1);
                el = min(el, ad[p].r/ad[p].w()-ad[p].h());
                for (int i=0; i<n; i++)
                    if (i!=p && ad[p].x1<=ad[i].x && ad[i].x<ad[p].x2 && ad[i].y<ad[p].y1)
                        el = min(el, ad[p].y1-ad[i].y-1);
                break;
            case 2:
                el = min(el, W-ad[p].x2);
                el = min(el, ad[p].r/ad[p].h()-ad[p].w());
                for (int i=0; i<n; i++)
                    if (i!=p && ad[p].y1<=ad[i].y && ad[i].y<ad[p].y2 && ad[p].x2<=ad[i].x)
                        el = min(el, ad[i].x-ad[p].x2);
                break;
            case 3:
                el = min(el, H-ad[p].y2);
                el = min(el, ad[p].r/ad[p].w()-ad[p].h());
                for (int i=0; i<n; i++)
                    if (i!=p && ad[p].x1<=ad[i].x && ad[i].x<ad[p].x2 && ad[p].y2<=ad[i].y)
                        el = min(el, ad[i].y-ad[p].y2);
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
                if (xor64()%(ad[p].w()+ad[p].h())<ad[p].w())
                    ed = xor64()%2*2;
                else
                    ed = xor64()%2*2+1;

            el = -W;
            switch (ed)
            {
            case 0:
                el = max(el, ad[p].x1-ad[p].x);
                break;
            case 1:
                el = max(el, ad[p].y1-ad[p].y);
                break;
            case 2:
                el = max(el, ad[p].x-ad[p].x2+1);
                break;
            case 3:
                el = max(el, ad[p].y-ad[p].y2+1);
                break;
            }
            if (el==0)
                continue;
            el = -(xor64()%-el+1);
        }

        int score_old = score;
        //  拡張／縮小
        score -= ad[p].score();
        Q.push_back(make_pair(p, ad[p]));
        ad[p].x1 += dx1[ed]*el;
        ad[p].y1 += dy1[ed]*el;
        ad[p].x2 += dx2[ed]*el;
        ad[p].y2 += dy2[ed]*el;
        score += ad[p].score();

        //  衝突している広告を縮める
        if (el>0)
            for (int i=0; i<n; i++)
                if (i!=p && intersect(ad[p], ad[i]))
                {
                    int c = 0;
                    Ad cand[4];
                    for (int sd=0; sd<4; sd++)
                        switch (sd)
                        {
                        case 0:
                            if (ad[i].x1<ad[p].x2 && ad[p].x2<=ad[i].x)
                            {
                                cand[c] = ad[i];
                                cand[c].x1 = ad[p].x2;
                                c++;
                            }
                            break;
                        case 1:
                            if (ad[i].y1<ad[p].y2 && ad[p].y2<=ad[i].y)
                            {
                                cand[c] = ad[i];
                                cand[c].y1 = ad[p].y2;
                                c++;
                            }
                            break;
                        case 2:
                            if (ad[i].x2>ad[p].x1 && ad[p].x1>ad[i].x)
                            {
                                cand[c] = ad[i];
                                cand[c].x2 = ad[p].x1;
                                c++;
                            }
                            break;
                        case 3:
                            if (ad[i].y2>ad[p].y1 && ad[p].y1>ad[i].y)
                            {
                                cand[c] = ad[i];
                                cand[c].y2 = ad[p].y1;
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
