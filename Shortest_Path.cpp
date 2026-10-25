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

struct sp_policy{
    using dist = ll; using cost = ll;
    static dist add(const dist& d, const cost& w){ // dist + cost 
        return d + w;
    }
    static bool leq(const dist& a, const dist& b){ // dist a <= dist b 
        return a <= b;
    }
    static dist inf(){ // ll -> INF, pll -> {INF, INF}, tll -> {INF, INF, INF} 
        return INF;
    }
    static dist zero(){ // ll -> 0, pll -> {0, 0}, tll -> {0, 0, 0} 
        return 0;
    }
};

template<class policy = sp_policy>
class _dij {
    bool base;
private:
    using dist = typename policy::dist; using cost = typename policy::cost;
    struct edge{ cost w; int nxt; }; struct node{ dist d; int v; };
    struct cmp{ bool operator()(const node& a, const node& b) const{ return _dij::less(b.d, a.d); } };
    vector<vector<edge>> adj; int n; bool built = 0;
    vector <dist> d; vector<int> pre;
    priority_queue<node, vector<node>, cmp> pq;
    void cal(){
        built = 1;
        while (!pq.empty()) {
            auto [cd, cur] = pq.top(); pq.pop();
            if(!eq(cd, d[cur])) continue;
            for(const auto& i : adj[cur]) {
                auto& [w, nxt] = i;
                dist nd = policy::add(cd, w);
                if(policy::leq(d[nxt], nd)) continue;
                d[nxt] = nd; pre[nxt] = cur;
                pq.push({ nd, nxt });
            }
        }
    }
    void reset(int n){ pre.assign(n + 1, -1); d.assign(n + 1, policy::inf()); }
    void chk(int x) const{ assert(built); assert(x >= base && x <= n); }
public:
    static bool less(const dist& a, const dist& b){ return policy::leq(a, b) && !policy::leq(b, a); } // O(1)
    static bool eq(const dist& a, const dist& b){ return policy::leq(a, b) && policy::leq(b, a); }   // O(1)

    _dij(int n = 0, bool base = 1){ clear(n, base); } // O(n)
    void clear(int n, bool base = 1){ // O(n + m)
        this->n = n; this->base = base; adj.assign(n + 1, {}); reset(n);
        built = 0; while(!pq.empty()) pq.pop();
    }

    void addsol(int st, int en, cost c){ // O(1)
        assert(base <= st && st <= n && base <= en && en <= n);
        built = 0;
        adj[st].push_back({ c,en });
    }
    void add(int st, int en, cost c){ // O(1)
        addsol(st, en, c); addsol(en, st, c);
    }

    void init(int source){ // O((n + m) log n)
        assert(base <= source && source <= n);
        reset(n); d[source] = policy::zero();
        pq.push({ d[source], source }); cal();
    }

    void init(const vector <int>& source){ // O((n + m) log n)
        reset(n);
        for(const auto& i : source){
            assert(base <= i && i <= n);
            d[i] = policy::zero();
            pq.push({ d[i], i });
        } cal();
    }

    dist ret(int x) const{ // O(1)
        chk(x);
        return d[x];
    }

    bool reachable(int x) const{ // O(1)
        chk(x);
        return !eq(d[x], policy::inf());
    }

    template<class F>
    void it_path(int x, const F& f) const{ // O(n)
        chk(x); assert(reachable(x));
        vector<int> ret;
        for(int cur = x; cur != -1; cur = pre[cur]) ret.push_back(cur);
        reverse(all(ret));
        for(const auto& v : ret) f(v);
    }
};

template <class policy = sp_policy>
class _floyd {
    bool base;
private:
    using dist = typename policy::dist; using cost = typename policy::cost;
    vector <vector<dist>> d; vector <vector<int>> nxt; 
    int n; bool built = 0;

    static bool less(const dist& a, const dist& b){ return policy::leq(a, b) && !policy::leq(b, a); }
    static bool eq(const dist& a, const dist& b){ return policy::leq(a, b) && policy::leq(b, a); }
    void chk(int st, int en) const{ assert(built); assert(st >= base && st <= n); assert(en >= base && en <= n); }
public:
    _floyd(int n = 0, bool base = 1){ clear(n, base); } // O(n^2)
    void clear(int n, bool base = 1){ // O(n^2)
        this->n = n; this->base = base; built = 0;
        d.assign(n + 1, vector<dist>(n + 1, policy::inf()));
        nxt.assign(n + 1, vector<int>(n + 1, 0));
        for(int i = base; i <= n; i++){
            d[i][i] = policy::zero();
            nxt[i][i] = i;
        }
    }

    void add(int st, int en, cost c) { // O(1)
        addsol(st, en, c); addsol(en, st, c);
    }

    void addsol(int st, int en, cost c) { // O(1)
        assert(base <= st && st <= n && base <= en && en <= n);
        built = 0;
        if(!less(c, d[st][en])) return;
        d[st][en] = c; nxt[st][en] = en;
    }

    void init() { // O(n^3)
        built = 1;
        for (int k = base; k <= n; k++) {
            for (int i = base; i <= n; i++) {
                for (int j = base; j <= n; j++) {
                    if(eq(d[i][k], policy::inf()) || eq(d[k][j], policy::inf())) continue;
                    dist nd = policy::add(d[i][k], d[k][j]);
                    if (policy::leq(d[i][j], nd)) continue;
                    d[i][j] = nd; nxt[i][j] = nxt[i][k];
                }
            }
        }
    }

    dist ret(int st, int en) const{ // O(1)
        chk(st, en);
        return d[st][en];
    }

    bool reachable(int st, int en) const{ // O(1)
        chk(st, en);
        return !eq(d[st][en], policy::inf());
    }

    template<class F>
    void it_path(int st, int en, const F& f) const{ // O(n)
        chk(st, en); assert(reachable(st, en));
        for(int cur = st;; cur = nxt[cur][en]){
            f(cur);
            if(cur == en) break;
        }
    }
};

template <class policy = sp_policy>
class _spfa {
    bool base;
private:
    using dist = typename policy::dist; using cost = typename policy::cost;
    struct edge{ cost w; int nxt; };
    vector <int> pre, cnt; deque <int> q;
    vector <dist> d; vector <bool> in, cy;
    vector <vector <edge>> adj; int n; bool built = 0;

    static bool less(const dist& a, const dist& b){ return policy::leq(a, b) && !policy::leq(b, a); }
    static bool eq(const dist& a, const dist& b){ return policy::leq(a, b) && policy::leq(b, a); }
    void cal_cy(int x){
        if(cy[x]) return; cy[x] = 1;
        deque <int> q; q.push_back(x);
        while(!q.empty()){
            int cur = q.front(); q.pop_front();
            for(auto& e : adj[cur]){
                if(cy[e.nxt]) continue;
                cy[e.nxt] = 1; q.push_back(e.nxt);
            }
        }
    }
    
    bool cal(){
        built = 1; bool ret = 1;
        while (!q.empty()) {
            int cur = q.front(); q.pop_front();
            in[cur] = 0; dist cd = d[cur];
            if(cy[cur]) continue;
            for (auto& e : adj[cur]) {
                auto [co, nxt] = e;
                if(cy[nxt]) continue;
                dist nd = policy::add(cd, co);
                if (policy::leq(d[nxt], nd)) continue;
                d[nxt] = nd; pre[nxt] = cur;

                cnt[nxt] = cnt[cur] + 1;
                if(cnt[nxt] >= n - base + 1){
                    ret = 0; cal_cy(nxt);
                    continue;
                }
                if(!in[nxt]){ in[nxt] = 1; q.push_back(nxt); }
            }
        }
        return ret;
    }
    void reset(int n){
        pre.assign(n + 1, -1); cnt.assign(n + 1, 0);
        d.assign(n + 1, policy::inf()); q.clear();
        in.assign(n + 1, false); cy.assign(n + 1, false);
    }
    void chk(int x) const{ assert(built); assert(x >= base && x <= n); }
public:
    _spfa(int n = 0, bool base = 1){ clear(n, base); } // O(n)
    void clear(int n, bool base = 1){ // O(n + m)
        this->n = n; this->base = base; built = 0;
        reset(n); adj.assign(n + 1, {});
    }

    void addsol(int st, int en, cost c) { // O(1)
        assert(base <= st && st <= n && base <= en && en <= n);
        built = 0;
        adj[st].push_back({ c, en });
    }
    void add(int st, int en, cost c) { // O(1)
        addsol(st, en, c); addsol(en, st, c);
    }

    // cycle existed -> return 0 
    bool init(int source, dist fi = policy::zero()) { // O(nm)
        assert(base <= source && source <= n);
        reset(n); d[source] = fi; in[source] = 1;
        q.push_back(source);
        return cal();
    }

    // cycle existed -> return 0 
    bool init(const vector <int>& source, dist fi = policy::zero()) { // O(nm)
        reset(n); 
        for (const auto& st : source) {
            assert(base <= st && st <= n);
            d[st] = fi; in[st] = 1;
            q.push_back(st);
        }
        return cal();
    }

    dist ret(int x) const{ // O(1)
        chk(x);
        return d[x];
    }

    bool reachable(int x) const{ // O(1)
        chk(x);
        return !is_cycle(x) && !eq(d[x], policy::inf());
    }

    bool is_cycle(int x) const{ // O(1)
        chk(x);
        return cy[x];
    }

    template<class F>
    void it_path(int x, const F& f) const{ // O(n) 
        chk(x); assert(reachable(x)); assert(!is_cycle(x));
        vector <int> ret;
        for(int cur = x; cur != -1; cur = pre[cur]) ret.push_back(cur);
        for(auto it = ret.rbegin(); it != ret.rend(); ++it) f(*it);
    }
};
