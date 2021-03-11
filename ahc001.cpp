#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
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
    int s() const {return (x2-x1)*(y2-y1);}
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
    vector<Ad> ad(n);
    for (int i=0; i<n; i++)
    {
        ad[i].x = x_[i];
        ad[i].y = y_[i];
        ad[i].r = r_[i];
    }

    for (int i=0; i<n; i++)
    {
        ad[i].x1 = ad[i].x;
        ad[i].y1 = ad[i].y;
        ad[i].x2 = ad[i].x+1;
        ad[i].y2 = ad[i].y+1;
    }

    int dx1[] = {-1, 0, 0, 0};
    int dy1[] = {0, -1, 0, 0};
    int dx2[] = {0, 0, 1, 0};
    int dy2[] = {0, 0, 0, 1};

    for (int iter=0; iter<10'000'000; iter++)
    {
        int p = xor64()%n;
        if (ad[p].s()<ad[p].r)
        {
            int d = xor64()%4;

            Ad tmp = ad[p];
            tmp.x1 += dx1[d];
            tmp.y1 += dy1[d];
            tmp.x2 += dx2[d];
            tmp.y2 += dy2[d];

            bool ok = true;
            if (tmp.x1<0 || W<tmp.x2 ||
                tmp.y1<0 || H<tmp.y2)
                ok = false;

            for (int i=0; i<n && ok; i++)
                if (i!=p && intersect(tmp, ad[i]))
                    ok = false;
            if (ok)
                ad[p] = tmp;
        }
    }

    vector<int> a, b, c, d;
    for (int i=0; i<n; i++)
    {
        a.push_back(ad[i].x1);
        b.push_back(ad[i].y1);
        c.push_back(ad[i].x2);
        d.push_back(ad[i].y2);
    }
    return {a, b, c, d};
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
