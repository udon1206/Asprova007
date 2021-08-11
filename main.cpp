#include <bits/stdc++.h>
using ll = long long;
using std::cin;
using std::cout;
using std::endl;
std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
template<class T> inline bool chmax(T& a, T b) { if (a < b) { a = b; return 1; } return 0; }
template<class T> inline bool chmin(T& a, T b) { if (a > b) { a = b; return 1; } return 0; }
const int inf = (int)1e9 + 7;
const long long INF = 1LL << 60;

struct KKT89
{
  int r_len, r_time, id;
  KKT89(int r_len, int r_time, int id) : r_len(r_len), r_time(r_time), id(id) {}
};

uint32_t xor64(void)
{
  static uint64_t x = 88172645463325252ULL;
  x = x ^ (x << 13); x = x ^ (x >> 7);
  return x = x ^ (x << 17);
}

double start_temp1 = 1, end_temp1 = 0.01, start_temp2 = 1, end_temp2 = 0.01;
std::chrono::system_clock::time_point start, end;
const double deadline = 1870;
void solve()
{
  start = std::chrono::system_clock::now();
  int m, S, T, L; cin >> m >> S >> T >> L;
  std::vector t(m, std::vector<int>(S));
  for (int i = 0; i < m; ++i)
  {
    for (int j = 0; j < S; ++j)
    {
      cin >> t[i][j];
    }
  }
  std::vector<int> x(S), n(m);
  for (int i = 0; i < S; ++i)
  {
    cin >> x[i];
  }
  for (int i = S - 1; i > 0; --i)
  {
    x[i] -= x[i - 1];
  }
  for (int i = 0; i < m; ++i)
  {
    cin >> n[i];
  }
  const int ALL = std::accumulate(n.begin(), n.end(), 0);
  std::vector<std::deque<KKT89>> vq(S);
  std::vector<int> exe_idx(S), cnt(m), loss(m);
  auto run = [&](const std::vector<int> &v, bool gettime = false)->std::pair<double, int>
  {
    int real_time = 0;
    int idle_time = 0;
    vq.clear(); vq.resize(S);
    exe_idx.clear(); exe_idx.resize(S);
    cnt.clear(); cnt.resize(m);
    loss.assign(m, 0);
    int push_time = 0;
    int idx = 0;
    int done = 0;
    while(real_time < L)
    {
      bool stop = false;
      if(push_time == 0 and idx < (int)v.size())
      {
        push_time += T;
        vq[0].emplace_back(x[0] + idle_time, t[v[idx]][0], v[idx]);
        idx += 1;
      }
      int exe_time = inf;
      for (int i = 0; i < S; ++i)
      {
        auto &ei = exe_idx[i];
        while(ei < (int)vq[i].size() and vq[i][ei].r_time == 0)
          ei += 1;
        if(not vq[i].empty() and vq[i].front().r_len - idle_time == 0 and vq[i].front().r_time > 0)
        {
          stop = true;
          loss[vq[i].front().id] += vq[i].front().r_time;
          if(exe_time == inf)
          {
            exe_time = vq[i].front().r_time;
          }
          else
            chmax(exe_time, vq[i].front().r_time);
        }
      }
      if(not stop)
      {
        for (int i = 0; i < S; ++i)
        {
          auto &ei = exe_idx[i];
          if(not stop and not vq[i].empty())
          {
            chmin(exe_time, vq[i][0].r_len - idle_time);
          }
          if(i == S - 1 and ei < (int)vq[i].size())
          {
            chmin(exe_time, vq[i][ei].r_time);
          }
        }
        if(not stop and idx < (int)v.size())
        {
          chmin(exe_time, push_time);
        }
      }
      if(exe_time > L - real_time)
        break;
      if(idx < (int)v.size() and not stop)
      {
        push_time -= exe_time;
      }
      if(not stop)
        idle_time += exe_time;
      real_time += exe_time;
      for (int i = S - 1; i >= 0; --i)
      {
        auto &ei = exe_idx[i];
        int r_exe_time = exe_time;
        while(ei < (int)vq[i].size() and r_exe_time > 0)
        {
          const int min = std::min(vq[i][ei].r_time, r_exe_time);
          vq[i][ei].r_time -= min;
          r_exe_time -= min;
          if(i == S - 1 and vq[i][ei].r_time == 0)
          {
            done += 1;
            cnt[vq[i][ei].id] += 1;
          }
          if(vq[i][ei].r_time == 0)
            ei += 1;
        }
        if(not vq[i].empty() and vq[i].front().r_len - idle_time == 0 and vq[i].front().r_time == 0)
        {
          const int id = vq[i].front().id;
          vq[i].pop_front();
          if(i + 1 < S)
          {
            vq[i + 1].emplace_back(x[i + 1] + idle_time, t[id][i + 1], id);
          }
          if(ei > 0)
            ei -= 1;
        }
      }
    }
    if(gettime)
    {
      return std::make_pair(0.0, real_time);
    }
    double score = 0;
    for (int i = 0; i < m; ++i)
    {
      score += std::sqrt((double)cnt[i] / (double)n[i]);
    }
    score /= m;
    return std::make_pair(score, done);
  };
  std::vector<int> res;
  std::vector<int> iota(m);
  for (int i = 0; i < m; ++i)
  {
    iota[i] = i;
  }

  auto annealing = [&](std::vector<int> &v, double d_time = 800, bool climing = false, double start_temp = 1000, double end_temp = 0.01)
  {
    auto start = std::chrono::system_clock::now();
    int best_get_time = run(v, true).second;
    int pre_get_time = best_get_time;
    auto best_v = v;
    for (int jupi_loves_kkt = 0;; ++jupi_loves_kkt)
    {
      auto end = std::chrono::system_clock::now();
      const double time = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
      if(time > d_time)
        break;
      const int id1 = xor64() % (int)v.size();
      const int id2 = xor64() % (int)v.size();
      std::swap(v[id1], v[id2]);
      const int get_time = run(v, true).second;
      if(climing)
      {
        if(not chmin(best_get_time, get_time))
        {
          std::swap(v[id1], v[id2]);
        }
      }
      else
      {
        const double temp = start_temp + (end_temp - start_temp) * time / d_time;
        const double prob = std::exp((pre_get_time - get_time) / temp);
        if(get_time < best_get_time)
        {
          best_v = v;
          best_get_time = get_time;
          pre_get_time = get_time;
        }
        else if(prob < (double)(xor64() % inf) / (double)inf)
        {
          std::swap(v[id1], v[id2]);
        }
        else
        {
          pre_get_time = get_time;
        }
      }
    }
    if(not climing)
      std::swap(v, best_v);
  };
  annealing(iota, 400, true);
  {
    auto t = n;
    while((int)res.size() < ALL)
    {
      for(const auto &e : iota)
      {
        if(t[e] > 0)
        {
          res.emplace_back(e);
          t[e] -= 1;
        }
      }
    }
  }
  auto [bs, bc] = run(res);
  if(bs > 0.95)
  {
    annealing(iota, 1300, true);
    std::tie(bs, bc) = run(res);
  }
  else
  {
    auto v = iota;
    auto ans = res;
    std::sort(v.begin(), v.end(), [&](auto i, auto j)
    {
      return loss[i] > loss[j];
    });
    std::vector<int> vskip; vskip.reserve((int)v.size() - 1);
    {
      vskip.clear();
      res.clear();
      auto t = n;
      int limit = std::max(v[0] * 3 / 10, v[1] * 3 / 10);
      for (int i = 0; i < limit; ++i)
      {
        for (int j = 0;j < 2; j++)
        {
          if(t[v[j]] > 0)
          {
            res.emplace_back(v[j]);
            t[v[j]] -= 1;
          }
        }
      }
      for (int i = 2; i < (int)v.size(); ++i)
      {
        if(t[v[i]] == 0)
          continue;
        vskip.emplace_back(v[i]);
      }
      annealing(vskip, 1300, true);
      while((int)res.size() < ALL - t[v[0]] - t[v[1]])
      {
        for(const auto &e : vskip)
        {
          if(t[e] > 0)
          {
            res.emplace_back(e);
            t[e] -= 1;
          }
        }
      }
      while((int)res.size() < ALL)
      {
        for (int i = 0; i < 2; ++i)
        {
          if(t[v[i]] > 0)
          {
            res.emplace_back(v[i]);
            t[v[i]] -= 1;
          }
        }
      }

      const auto [ts, tc] = run(res);
      if(ts > bs)
      {
        ans = res;
        bs = ts;
        bc = tc;
      }
      std::swap(res, ans);
    }
  }
  res.resize(bc);
  cout << res.size() << "\n";
  for (int i = 0; i < (int)res.size(); ++i)
  {
    cout << res[i] + 1 << " \n"[i + 1 == (int)res.size()];
  }
}
int main(int argc, char *argv[])
{
  std::cin.tie(nullptr);
  std::ios::sync_with_stdio(false);

  if(argc > 1)
  {
    start_temp1 = std::stod(std::string(argv[1]));
    end_temp1 = std::stod(std::string(argv[2]));
    start_temp2 = std::stod(std::string(argv[3]));
    end_temp2 = std::stod(std::string(argv[4]));
  }
  int kkt = 1; 
  // cin >> kkt;
  while(kkt--)
    solve();
  return 0;
  
}