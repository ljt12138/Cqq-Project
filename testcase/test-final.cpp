#include<iostream>
#include<cstdio>
using namespace std;

int this_is_a_solution_to_the_A_plus_B_problem_set_by_vfleaking;

int INF;

int N, A[2200], B[2200], W[2200], L[2200], R[2200], P[2200];
int As[2200];

int As_QS(int i, int j)
{
  int l, r, x;
  l = i; r = j;
  x = As[(i + j) / 2];
  while (i <= j)
    {
      while (As[i] < x) i = i + 1;
      while (As[j] > x) j = j - 1;
      if (i <= j)
        {
          int t;
          t = As[i];
          As[i] = As[j];
          As[j] = t;
          i = i + 1;
          j = j - 1;
        }
    }
  if (i < r) As_QS(i, r);
  if (l < j) As_QS(l, j);
  return 0;
}

int As_lower_bound(int x)
{
  int l, r;
  l = 1; r = N + 1;
  while (l != r)
    {
      int mid;
      if (As[mid = (l + r) / 2] >= x)
        r = mid;
      else
        l = mid + 1;
    }
  return l;
}

int here_comes_the_SEGMENT_TREE;
int sc, sroot[2200], s[30000][2], inS[2200], prev[30000];

int emp(int l, int r)
{
  int cur;
  cur = sc = sc + 1;
  if (l != r)
    {
      int mid;
      mid = (l + r) / 2;
      s[cur][0] = emp(l, mid);
      s[cur][1] = emp(mid + 1, r);
    }
  return cur;
}

int insert(int cur, int l, int r, int b, int c)
{
  int nx;
  nx = sc = sc + 1;
  prev[nx] = cur;
  s[nx][0] = s[cur][0];
  s[nx][1] = s[cur][1];
  if (l == r)
    inS[c] = nx;
  else
    {
      int mid;
      mid = (l + r) / 2;
      if (b <= mid)
        s[nx][0] = insert(s[nx][0], l, mid, b, c);
      else
        s[nx][1] = insert(s[nx][1], mid + 1, r, b, c);
    }
  return nx;
}

int here_comes_the_GRAPH_and_the_NETWORK_FLOW;

int source()
{
  return 1;
}

int sink()
{
  return 2;
}

int point_lv_1(int x)
{
  return 2 + x;
}

int point_lv_2(int x)
{
  return 2 + N + x;
}

int segnode(int x)
{
  return 2 + (N * 2) + x;
}

int e[120000], cp[120000], enxt[120000], head[30000], ec, nc;

int partner(int x)
{
  if (x % 2 == 1)
    return x + 1;
  else
    return x - 1;
}

int regi(int u, int v, int w)
{
  ec = ec + 1;
  e[ec] = v; cp[ec] = w;
  enxt[ec] = head[u]; head[u] = ec;
}

int directed(int u, int v, int w)
{
  regi(u, v, w);
  regi(v, u, 0);
}

int ANS;

int travel(int cur, int L, int R, int l, int r, int x)
{
  if ((L == l) && (R == r))
    {
      directed(point_lv_2(x), segnode(cur), INF);
      return 0;
    }
  int Mid; Mid = (L + R) / 2;
  if (r <= Mid)
    {
      travel(s[cur][0], L, Mid, l, r, x);
      return 0;
    }
  if (l > Mid)
    {
      travel(s[cur][1], Mid + 1, R, l, r, x);
      return 0;
    }
  travel(s[cur][0], L, Mid, l, Mid, x);
  travel(s[cur][1], Mid + 1, R, Mid + 1, r, x);
}

int pushflow(int E, int F)
{
  cp[E] = cp[E] - F;
  cp[partner(E)] = cp[partner(E)] + F;
}

int Nid, tag[30000], lyr[30000], cur[30000], pre[30000], prn[30000], supp[30000], comp[30000], Aug, Gap;

int relabel(int x)
{
  int ol, nl, E;
  ol = tag[x]; nl = nc;
  for (E = head[x]; E; E = enxt[E])
    if ((cp[E]) && (tag[x] <= tag[e[E]] + 1))
      if (nl > tag[e[E]] + 1)
        nl = tag[e[E]] + 1;
  lyr[nl] = lyr[nl] + 1;
  lyr[ol] = lyr[ol] - 1;
  tag[x] = nl;
  if (lyr[ol] == 0) Gap = 1;
  cur[x] = head[x];
}

int sap_initialize()
{
  int i;
  for (i = 1; i <= nc; i = i + 1)
    {
      tag[i] = lyr[i] = 0;
      cur[i] = head[i];
    }
  lyr[0] = nc;
  Nid = source();
  Gap = 0;
  supp[Nid] = INF;
  comp[Nid] = 0;
}

int sap()
{
  int ans; ans = 0;
  while ((tag[source()] != nc) && (Gap == 0))
    {
      if (Nid == sink())
        {
          int increment;
          increment = supp[Nid] - comp[Nid];
          ans = ans + increment;
          int x;
          for (x = sink(); x != source(); x = prn[x])
            {
              comp[x] = comp[x] + increment;
              pushflow(pre[x], increment);
            }
        }
      Aug = 0;
      if (supp[Nid] > comp[Nid])
        for (; (cur[Nid] != 0) && (Aug == 0); cur[Nid] = enxt[cur[Nid]])
          {
            int E;
            E = cur[Nid];
            if ((cp[E]) && (tag[Nid] == tag[e[E]] + 1))
              {
                Aug = 1;
                if (cp[E] < supp[Nid] - comp[Nid])
                  supp[e[E]] = cp[E];
                else
                  supp[e[E]] = supp[Nid] - comp[Nid];
                comp[e[E]] = 0;
                prn[e[E]] = Nid;
                pre[e[E]] = E;
                Nid = e[E];
              }
          }
      if (Aug == 0)
        {
          if (Nid != sink()) relabel(Nid);
          if (Nid == source())
            comp[Nid] = 0;
          else
            Nid = prn[Nid];
        }
      else
        cur[Nid] = head[Nid];
    }
  return ans;
}

int main()
{
  INF = 1000000000;

  cin >> N;
  int i;
  for (i = 1; i <= N; i = i + 1)
    cin >> A[i] >> B[i] >> W[i] >> L[i] >> R[i] >> P[i];
  ANS = 0;
  for (i = 1; i <= N; i = i + 1)
    ANS = ANS + B[i] + W[i];
  for (i = 1; i <= N; i = i + 1)
    As[i] = A[i];
  As_QS(1, N);
  sc = 0;
  sroot[0] = emp(1, N);
  for (i = 1; i <= N; i = i + 1)
    sroot[i] = insert(sroot[i - 1], 1, N, As_lower_bound(A[i]), i);
  nc = segnode(sc);
  for (i = 1; i <= nc; i = i + 1)
    head[i] = 0;
  ec = 0;
  for (i = 1; i <= N; i = i + 1)
    {
      directed(source(), point_lv_1(i), B[i]);
      directed(point_lv_1(i), sink(), W[i]);
      directed(point_lv_1(i), point_lv_2(i), P[i]);
    }
  for (i = 1; i <= sc; i = i + 1)
    {
      if ((s[i][0] != 0) && (prev[i]))
        directed(segnode(i), segnode(prev[i]), INF);
      if (s[i][0]) directed(segnode(i), segnode(s[i][0]), INF);
      if (s[i][1]) directed(segnode(i), segnode(s[i][1]), INF);
    }
  for (i = 1; i <= N; i = i + 1)
    directed(segnode(inS[i]), point_lv_1(i), INF);
  for (i = 1; i <= N; i = i + 1)
    {
      int l, r;
      l = As_lower_bound(L[i]);
      r = As_lower_bound(R[i] + 1);
      if (l < r)
        travel(sroot[i - 1], 1, N, l, r - 1, i);
    }
  sap_initialize();
  ANS = ANS - sap();
  cout << ANS << endl;
  return 0;
}

int orzvfk;
int mo_bai_lv_kai_feng;
