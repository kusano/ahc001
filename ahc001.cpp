#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdio>
using namespace std;

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

vector<vector<int>> solve(int n, vector<int> x, vector<int> y, vector<int> r)
{
    vector<int> a(n), b(n), c(n), d(n);

    for (int i=0; i<n; i++)
    {
        a[i] = x[i];
        b[i] = y[i];
        c[i] = x[i]+1;
        d[i] = y[i]+1;
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

        vector<vector<int>> t = solve(n, x, y, r);
        vector<int> a = t[0];
        vector<int> b = t[1];
        vector<int> c = t[2];
        vector<int> d = t[3];

        ofstream out(string("out\\")+fname);
        for (int j=0; j<n; j++)
            out<<a[j]<<" "<<b[j]<<" "<<c[j]<<" "<<d[j]<<endl;

        int s = score(n, x, y, r, a, b, c, d);
        printf("%2d: %10d\n", i, s);

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
