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

vector<vector<int>> solve(int n, vector<int> x, vector<int> y, vector<int> r)
{
    vector<int> x1(n), y1(n), x2(n), y2(n);

    for (int i=0; i<n; i++)
    {
        x1[i] = x[i];
        y1[i] = y[i];
        x2[i] = x[i]+1;
        y2[i] = y[i]+1;
    }

    auto area = [&](int p)
    {
        return (x2[p]-x1[p])*(y2[p]-y1[p]);
    };

    int dx1[] = {-1, 0, 0, 0};
    int dy1[] = {0, -1, 0, 0};
    int dx2[] = {0, 0, 1, 0};
    int dy2[] = {0, 0, 0, 1};

    for (int iter=0; iter<10'000'000; iter++)
    {
        int p = xor64()%n;
        if (area(p)<r[p])
        {
            int d = xor64()%4;
            bool ok = true;
            if (x1[p]+dx1[d]<0 || W<x2[p]+dx2[d] ||
                y1[p]+dy1[d]<0 || H<y2[p]+dy2[d])
                ok = false;

            for (int i=0; i<n && ok; i++)
                if (i!=p)
                    if (!(x2[i]<=x1[p]+dx1[d] ||
                          x1[i]>=x2[p]+dx2[d] ||
                          y2[i]<=y1[p]+dy1[d] ||
                          y1[i]>=y2[p]+dy2[d]))
                        ok = false;
            if (ok)
            {
                x1[p] += dx1[d];
                y1[p] += dy1[d];
                x2[p] += dx2[d];
                y2[p] += dy2[d];
            }
        }
    }

    return {x1, y1, x2, y2};
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
    evaluate();
    return 0;

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
}
