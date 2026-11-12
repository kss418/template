#include <bits/stdc++.h>
#define all(x) (x).begin(), (x).end()
#define x first 
#define y second
using namespace std; using ll = long long; using u64 = uint64_t;
using ld = long double; using pld = pair<ld, ld>;
using i128 = __int128_t; using f128 = __float128; 
using pll = pair<ll, ll>; using tll = tuple<ll, ll, ll>;
constexpr ll INF = 0x3f3f3f3f3f3f3f3f;
constexpr ll MINF = 0xc0c0c0c0c0c0c0c0;

template <signed_integral T = ll>
class _flow{
private:
    bool base;
public:
    static consteval T make_inf() {
        T x = 0;
        for (size_t i = 0; i < sizeof(T); ++i) x = (x << 8) | T(0x3f);
        return x;
    }
    static constexpr T INF = make_inf();
    class edge{
    public:
        int nxt, rev; T c, cap;
        edge(int nxt, int rev, T c) : nxt(nxt), rev(rev), c(c), cap(c) {}
    };

    class state{
    private:
        friend class _flow;
        _flow* owner;
        int cur, idx;
        state(_flow* owner, int cur, int idx) : owner(owner), cur(cur), idx(idx){}
    public:
        int from() const{ return cur; } // O(1)
        int to() const{ return owner->adj[cur][idx].nxt; } // O(1)
        bool operator==(const state& a) const = default; // O(1)

        T get_flow() const{ auto& e = owner->adj[cur][idx]; return e.cap - e.c; } // O(1)
        void add_flow(T delta){ set_flow(get_flow() + delta); } // O(1)
        void set_flow(T f){ // O(1)
            auto& e = owner->adj[cur][idx], &re = owner->adj[e.nxt][e.rev];
            e.c = e.cap - f; re.c = f;
        }

        T get_cap() const{ return owner->adj[cur][idx].cap; } // O(1)
        void add_cap(T delta){ set_cap(get_cap() + delta); } // O(1)
        void set_cap(T cap){ // O(1)
            auto& e = owner->adj[cur][idx], &re = owner->adj[e.nxt][e.rev];
            T f = e.cap - e.c; e.cap = cap;
            e.c = cap - f; re.c = f;
        }
    };
private:
    vector <int> d, w; int n; T result;
    vector <vector<edge>> adj;
    vector <vector<int>> ids;
    bool bfs(int st, int en) { 
        memset(d.data(), -1, sizeof(int) * (n + 1));
        queue <int> q; q.push(st); d[st] = 0;
        while (!q.empty()) {
            int cur = q.front(); q.pop();
            for (auto& e : adj[cur]) {
                if(e.c <= 0) continue;
                int nxt = e.nxt;
                if (d[nxt] != -1) continue;
                d[nxt] = d[cur] + 1;
                if(nxt == en) return true;
                q.push(nxt);
            }
        }
        return d[en] != -1;
    }

    T dfs(int cur, int en, T val) { 
        if (cur == en) return val;
        int &i = w[cur], sz = (int)adj[cur].size();
        for(;i < sz;++i){
            edge &e = adj[cur][i];
            if(e.c <= 0) continue;
            int nxt = e.nxt;
            if(d[nxt] != d[cur] + 1) continue;
            T mn = dfs(nxt, en, min(val, e.c));
            if(mn <= 0) continue;
            e.c -= mn; adj[nxt][e.rev].c += mn;
            return mn;
        }
        return 0;
    }

    T augment(int st, int en, T f = INF){ // O(n^2 * m)
        assert(base <= st && st <= n && base <= en && en <= n && st != en);
        T r = 0;
        while(f && bfs(st, en)){
            memset(w.data(), 0, sizeof(int) * (n + 1));
            while(f){
                T now = dfs(st, en, f);
                if(!now) break; r += now; f -= now;
            }
        }
        return r;
    }
public:
    _flow(int n = 0, bool base = 1){ clear(n, base); } // O(n)
    void clear(int n, bool base = 1){ // O(n)
        this->n = n; this->base = base; adj.assign(n + 1, {}); ids.assign(n + 1, {});
        d.assign(n + 1, -1); w.assign(n + 1, 0); result = 0;
    }

    void reset(){ // O(n + m)
        result = 0;
        for(int i = base;i <= n;i++){
            for(auto& e : adj[i]) e.c = e.cap;
        }
    }

    state addsol(int st, int en, T c = INF){  // O(1)
        assert(base <= st && st <= n && base <= en && en <= n);
        int re = (int)adj[en].size() + (st == en), rs = (int)adj[st].size();
        adj[st].push_back({en, re, c});
        adj[en].push_back({st, rs, 0});
        ids[st].push_back(rs);
        return {this, st, rs};
    }

    pair <state, state> add(int st, int en, T c = INF){ // O(1)
        auto h1 = addsol(st, en, c); auto h2 = addsol(en, st, c);
        return {h1, h2};
    }

    vector <bool> min_cut(int st) const{ // O(n + m)
        vector <bool> v(n + 1, false); 
        queue <int> q; v[st] = 1; q.push(st);
        while(!q.empty()){
            int cur = q.front(); q.pop();
            for(const auto& e : adj[cur]){
                if(e.c <= 0 || v[e.nxt]) continue;
                v[e.nxt] = 1; q.push(e.nxt);
            }
        }
        return v;
    }

    bool reachable(int st, int en) const{ return min_cut(st)[en]; } // O(n + m)

    template <class F>
    void it_idx(int idx, const F& f){ // O(|ids[idx]|)
        for(auto i : ids[idx]) f(state(this, idx, i));
    }

    template <class F>
    void it_all(const F& f){ // O(n + m)
        for(int i = base;i <= n;i++) it_idx(i, f);
    }

    const edge& get_edge(const state& h) const{ return adj[h.cur][h.idx]; } // O(1)
    const edge& get_rev(const state& h) const{ auto& e = get_edge(h); return adj[e.nxt][e.rev]; } // O(1)
    void build(int st, int en, T f = INF){ reset(); result = augment(st, en, f); } // O(n^2 * m)
    T ret() const{ return result; } // O(1)
};

template <signed_integral F = ll, signed_integral C = ll>
class _mcmf{
private:
    bool base;
public:
    template <class T>
    static consteval T make_inf() {
        T x = 0;
        for (size_t i = 0; i < sizeof(T); ++i) x = (x << 8) | T(0x3f);
        return x;
    }
    static constexpr F FINF = make_inf<F>();
    static constexpr C CINF = make_inf<C>();
    class edge{
    public:
        int nxt, rev; F c, cap; C cost;
        edge(int nxt, int rev, F c, C cost) : nxt(nxt), rev(rev), c(c), cap(c), cost(cost) {}
    };

    class state{
    private:
        friend class _mcmf;
        _mcmf* owner;
        int cur, idx;
        state(_mcmf* owner, int cur, int idx) : owner(owner), cur(cur), idx(idx){}
    public:
        int from() const{ return cur; } // O(1)
        int to() const{ return owner->adj[cur][idx].nxt; } // O(1)
        bool operator==(const state& a) const = default; // O(1)

        F get_flow() const{ auto& e = owner->adj[cur][idx]; return e.cap - e.c; } // O(1)
        void add_flow(F delta){ set_flow(get_flow() + delta); } // O(1)
        void set_flow(F f){ // O(1)
            auto& e = owner->adj[cur][idx], &re = owner->adj[e.nxt][e.rev];
            e.c = e.cap - f; re.c = f;
        }

        F get_cap() const{ return owner->adj[cur][idx].cap; } // O(1)
        void add_cap(F delta){ set_cap(get_cap() + delta); } // O(1)
        void set_cap(F cap){ // O(1)
            auto& e = owner->adj[cur][idx], &re = owner->adj[e.nxt][e.rev];
            F f = e.cap - e.c; e.cap = cap;
            e.c = cap - f; re.c = f;
        }

        C get_cost() const{ return owner->adj[cur][idx].cost; } // O(1)
        void add_cost(C delta){ set_cost(get_cost() + delta); } // O(1)
        void set_cost(C cost){ // O(1)
            auto& e = owner->adj[cur][idx], &re = owner->adj[e.nxt][e.rev];
            e.cost = cost; re.cost = -cost;
        }
    };
private:
    vector<vector<edge>> adj; vector<C> dist, pot; 
    vector<int> work, lv; int n; bool use_spfa;
    vector<vector<int>> ids; pair<F, C> result;
    void spfa(int st) {
        fill(all(pot), CINF);
        vector <bool> in(n + 1, 0); queue <int> q;
        pot[st] = 0; q.push(st); in[st] = 1;
        while (!q.empty()) {
            int cur = q.front(); q.pop(); in[cur] = 0;
            for(auto &e : adj[cur]){
                if (e.c <= 0) continue; int nxt = e.nxt;
                if (pot[nxt] <= pot[cur] + e.cost) continue;
                pot[nxt] = pot[cur] + e.cost;
                if (!in[nxt]) q.push(nxt), in[nxt] = 1;
            }
        }
        for(int i = base; i <= n; i++) if(pot[i] >= CINF) pot[i] = 0;
    }

    bool dijkstra(int st, int en){
        using pci = pair<C, int>;
        fill(all(dist), CINF); dist[st] = 0;
        priority_queue<pci, vector<pci>, greater<pci>> pq;
        pq.push({0, st});
        while (!pq.empty()) {
            auto [cd, cur] = pq.top(); pq.pop();
            if (cd != dist[cur]) continue;
            for (int i = 0;i < (int)adj[cur].size();i++) {
                auto &e = adj[cur][i];
                if(e.c <= 0) continue; int nxt = e.nxt;
                C nd = cd + e.cost + pot[cur] - pot[nxt];
                if(nd >= dist[nxt]) continue;
                dist[nxt] = nd; pq.push({nd, nxt});
            }
        }
        if (dist[en] >= CINF) return false;
        for (int i = base;i <= n;i++) {
            if (dist[i] < CINF) pot[i] += dist[i];
        }
        return true;
    }

    bool bfs(int st, int en){
        fill(all(lv), -1); queue <int> q;
        lv[st] = 0; q.push(st);
        while(!q.empty()){
            int cur = q.front(); q.pop();
            for(auto& e : adj[cur]){
                if(e.c <= 0) continue;
                int nxt = e.nxt;
                if(lv[nxt] != -1 || e.cost + pot[cur] - pot[nxt] != 0) continue;
                lv[nxt] = lv[cur] + 1;
                if(nxt == en) return true;
                q.push(nxt);
            }
        }
        return lv[en] != -1;
    }

    F dfs(int cur, int en, F f){
        if(cur == en) return f;
        for(int &i = work[cur]; i < (int)adj[cur].size(); i++){
            auto &e = adj[cur][i];
            if(e.c <= 0) continue; int nxt = e.nxt;
            if(e.cost + pot[cur] - pot[nxt] != 0) continue;
            if(lv[nxt] != lv[cur] + 1) continue; 
            F now = dfs(nxt, en, min(f, e.c));
            if(now <= 0) continue;
            e.c -= now; adj[nxt][e.rev].c += now;
            return now;
        }
        return 0;
    }

    pair<F, C> augment(int st, int en, F limit = FINF){
        assert(base <= st && st <= n && base <= en && en <= n && st != en);
        F flow = 0; C cost = 0;
        if(limit && use_spfa) spfa(st);
        while(flow < limit && dijkstra(st, en)) {
            while(flow < limit && bfs(st, en)){
                fill(all(work), 0);
                while(flow < limit){
                    F now = dfs(st, en, limit - flow);
                    if(now <= 0) break;
                    flow += now; cost += now * (pot[en] - pot[st]);
                }
            }
        }
        return {flow, cost};
    }
public:
    _mcmf(int n = 0, bool use_spfa = true, bool base = 1){ clear(n, use_spfa, base); } // O(n)
    void clear(int n, bool use_spfa = true, bool base = 1){ // O(n)
        this->n = n; this->base = base; this->use_spfa = use_spfa; adj.assign(n + 1, {}); ids.assign(n + 1, {});
        dist.assign(n + 1, CINF); pot.assign(n + 1, 0);
        work.assign(n + 1, 0); lv.assign(n + 1, -1); result = {0, 0};
    }

    void reset(){ // O(n + m)
        result = {0, 0};
        for(auto& row : adj) for(auto& e : row) e.c = e.cap;
        fill(all(pot), 0); fill(all(dist), CINF); fill(all(work), 0); fill(all(lv), -1);
    }

    pair<state, state> add(int st, int en, F cap, C cost){ // O(1)
        auto h1 = addsol(st, en, cap, cost), h2 = addsol(en, st, cap, cost);
        return {h1, h2};
    }

    state addsol(int st, int en, F cap, C cost){ // O(1)
        assert(base <= st && st <= n && base <= en && en <= n);
        int a = (int)adj[st].size(), b = (int)adj[en].size() + (st == en);
        adj[st].push_back({en, b, cap, cost});
        adj[en].push_back({st, a, 0, -cost});
        ids[st].push_back(a);
        return {this, st, a};
    }

    template<class T>
    void it_idx(int idx, const T& f){ // O(|ids[idx]|)
        for(auto i : ids[idx]) f(state(this, idx, i));
    }

    template<class T>
    void it_all(const T& f){ // O(n + m)
        for(int i = base;i <= n;i++) it_idx(i, f);
    }

    const edge& get_edge(const state& h) const{ return adj[h.cur][h.idx]; } // O(1)
    const edge& get_rev(const state& h) const{ auto& e = get_edge(h); return adj[e.nxt][e.rev]; } // O(1)
    void build(int st, int en, F limit = FINF){ reset(); result = augment(st, en, limit); }
    pair<F, C> ret() const{ return result; } // O(1)
};
