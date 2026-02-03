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

// LCA
struct lca_policy{
    struct node{
        ll v;
        node(ll v = MINF) : v(v){}
    };
    struct cost{
        bool v;
        cost(bool v = false) : v(v){}
    };
    static constexpr bool use_node = false;
    static node lift(const cost& c) { return node(c.v); }
    static node merge(const node& a, const node& b){ return { max(a.v, b.v) }; }
};

template <class policy = lca_policy>
class _lca { // 1-based index
private:
    using node = typename policy::node;
    using cost = typename policy::cost;
    using edge = pair<int, cost>;
    vector <vector<int>> p; vector <int> d, comp, ord; 
    vector <vector<node>> arr;
    vector <vector<edge>> adj; int n, lg, cc;
    template<bool USE = policy::use_node>
    void shift_both(int& a, int& b, node* v = nullptr){
        for(int i = lg - 1;i >= 0;i--){
            if(p[i][a] == -1) continue;
            if(p[i][a] == p[i][b]) continue;
            if constexpr (USE){
                *v = policy::merge(*v, arr[i][a]);
                *v = policy::merge(*v, arr[i][b]);
            }
            a = p[i][a]; b = p[i][b];
        }
    }

    void build_comp(int root, int cid) {
        stack <int> st; st.push(root); comp[root] = cid;
        while(!st.empty()){
            int cur = st.top(); st.pop(); ord.push_back(cur);
            for(auto& [nxt, co] : adj[cur]){
                if(d[nxt] != -1) continue;
                p[0][nxt] = cur; d[nxt] = d[cur] + 1; comp[nxt] = cid;
                if constexpr (policy::use_node) arr[0][nxt] = policy::lift(co);
                st.push(nxt);
            }
        }
    }

    template<bool USE = false>
    int shift(int cur, int diff, node* v = nullptr){ // O(log n)
        if (diff <= 0) return cur;
        for (int i = lg;i >= 0; i--) {
            if(cur == -1) break;
            if(!(diff & (1ll << i))) continue;
            if constexpr (USE) *v = policy::merge(*v, arr[i][cur]);
            cur = p[i][cur];
        }
        return cur;
    }
public:
    _lca(int n = 0){ clear(n); } // O(1)
    void clear(int n){ // O(n)
        this->n = n; lg = 1; cc = 0;
        while ((1ll << lg) <= n) lg++; 
        p.assign(lg + 1, vector<int>(n + 1, -1)); 
        d.assign(n + 1, -1); adj.assign(n + 1, {}); comp.assign(n + 1, 0);
        ord.clear(); ord.reserve(n);
        if constexpr (policy::use_node) arr.assign(lg + 1, vector<node>(n + 1, policy::node()));
    }

    void add(int a, int b, cost co = cost()){ addsol(a, b, co); addsol(b, a, co); } // O(1)
    void addsol(int a, int b, cost co = cost()){ adj[a].push_back({b, co}); } // O(1)
    void build(){ // O(n log n)
        for(int i = 1;i <= n;i++){
            if(d[i] != -1) continue;
            d[i] = 0; ++cc; build_comp(i, cc);
        }
        for (int i = 1; i <= lg; i++) {
            for (int j = 1; j <= n; j++) {
                if (p[i - 1][j] == -1) continue;
                p[i][j] = p[i - 1][p[i - 1][j]];
                if constexpr (policy::use_node) arr[i][j] = policy::merge(arr[i - 1][j], arr[i - 1][p[i - 1][j]]);
            }
        }
    }

    int component(int v) const { return comp[v]; } // O(1)
    bool same_comp(int a, int b) const { return comp[a] == comp[b]; } // O(1)

    int depth(int v) const { return d[v]; } // O(1)
    int parent(int v) const { return p[0][v]; } // O(1)
    int ancestor(int v, int k){  // O(log n)
        if(k < 0 || k > d[v]) return -1;
        return shift<false>(v, k); 
    }

    // REV == true / child -> parent, REV == false / parent -> child
    template<bool REV = true, class F>
    void it(const F& f) const{ // O(n)
        if constexpr (!REV) for(auto& v : ord) f(v);
        else for(auto iter = ord.rbegin(); iter != ord.rend();++iter) f(*iter);
    }
   
    int ret(int a, int b) { // O(log n)
        if(!same_comp(a, b)) return -1;
        if(d[a] < d[b]) swap(a, b);
        a = shift<false>(a, d[a] - d[b]);
        if (a == b) return a;
        shift_both<false>(a, b);
        return p[0][a];
    }

    int jump(int a, int b, int k){ // O(log n)
        if(!same_comp(a, b)) return -1;
        int l = ret(a, b), da = d[a] - d[l], db = d[b] - d[l];
        if(k < 0 || k > da + db) return -1;
        if(k <= da) return shift<false>(a, k);
        return shift<false>(b, da + db - k);
    }

    int dist(int a, int b){ // O(log n)
        if(!same_comp(a, b)) return -1;
        return d[a] + d[b] - 2 * d[ret(a, b)];
    }

    node query(int a, int b){ // O(log n)
        static_assert(policy::use_node); node ret = node();
        if(!same_comp(a, b)) return ret;
        if(d[a] < d[b]) swap(a, b);
        a = shift<true>(a, d[a] - d[b], &ret);
        if(a == b) return ret;
        shift_both<true>(a, b, &ret);
        ret = policy::merge(ret, arr[0][a]);
        return policy::merge(ret, arr[0][b]);
    }
};

// Sparse Table
class _st { // 0-base index
private:
    vector <vector<ll>> sum;
    vector <vector<int>> lift; ll m; int n, lg; 
public:
    _st(int n = 0, ll max_step = 0){ clear(n, max_step); }; // O(n log m)
    void clear(int n, ll m = 60){ // O(n log m)
        lg = 1; this->n = n; this->m = m;
        while((1ll << lg) <= m) lg++;
        lift.assign(lg + 1, vector<int>(n + 2, n + 1));
        sum.assign(lg + 1, vector<ll>(n + 2, 1));
    }

    void add(int st, int en, ll co = 1) { lift[0][st] = en; sum[0][st] = co; } // O(1)
    void build(){ // O(n log m)
        for(int i = 1;i <= lg;i++){
            for(int j = 0;j <= n;j++){
                int mid = lift[i - 1][j];
                lift[i][j] = lift[i - 1][mid];
                sum[i][j] = min<int>(sum[i - 1][j] + sum[i - 1][mid], 0x3f3f3f3f);
            }
        }
    }

    int next(int v) const{ return lift[0][v]; } // O(1)
    pair<int, ll> ret(int cur, ll k) const{ // O(log m)
        ll co = 0;
        for(int i = lg; i >= 0; i--){
            if(k < sum[i][cur]) continue;
            co += sum[i][cur]; k -= sum[i][cur]; cur = lift[i][cur];
        }
        return {cur, co}; // 도착 정점, 사용한 비용
    }
};

// MST
struct mst_policy{
    using cost = ll;
    static cost zero(){ return 0; }
    static cost add(const cost& a, const cost& b){ return a + b; }
};

template <class policy = mst_policy>
class _mst{
private:
    using cost = typename policy::cost;
    cost zero() const{ return policy::zero(); }
    cost add(const cost& a, const cost& b) const{ return policy::add(a, b); }
    cost result = zero(); int n; bool built = 0;
    class edge{
    public:
        int s, e; cost c;
        bool operator<(const edge& ot) const{ return c < ot.c; }
    };
    class _uf{
    private:
        vector <int> p, sz; int n;
    public:
        _uf(int n = 0){ clear(n); } // O(n)
        void clear(int n){ this->n = n; p.assign(n + 1, -1); sz.assign(n + 1, 1); }  // O(n)
        int find(int x){ // O(1)
            if(p[x] == -1) return x;
            return p[x] = find(p[x]);
        }

        bool merge(int a, int b){ // O(1) 
            a = find(a); b = find(b);
            if(a == b) return false;
            if(sz[a] < sz[b]) swap(a, b);
            p[b] = a; sz[a] += sz[b];
            return true;
        }
    }; _uf uf; vector<edge> adj;

    bool cal(int num = 0){ 
        assert(!built); built = 1;
        sort(all(adj)); int cnt = 0;
        for(auto& i : adj){
            auto& [st, en, c] = i;
            if(cnt >= n - num - 1) return 1;
            if(!uf.merge(st, en)) continue;
            result = add(result, c); cnt++;
        }
        return cnt >= n - num - 1;
    }
public:
    _mst(int n = 0){ clear(n); } // O(n)
    void clear(int n){ // O(n)
        this->n = n; result = zero(); 
        uf.clear(n); built = 0; adj.clear();
    }

    void add(int st, int en, cost c){ // O(1)
        assert(!built);
        adj.push_back({st, en, c}); 
    }

    bool init(int num = 0){ return cal(num);} // O(m log m)
    cost ret(){ // O(1)
        assert(built);
        return result; 
    }
};

//SCC
class _scc{ // select base
private:
    vector <vector<int>> adj, rev, group;
    vector <int> comp, ord; vector <bool> v; int n, base, cc;
    void dfs1(int cur){
        v[cur] = 1;
        for(auto& nxt : adj[cur]) if(!v[nxt]) dfs1(nxt);
        ord.push_back(cur);
    }

    void dfs2(int cur, int cid){
        comp[cur] = cid; group[cid].push_back(cur);
        for(auto& nxt : rev[cur]) if(comp[nxt] == -1) dfs2(nxt, cid);
    }
public:
    _scc(int max_index = 0, int base = 1){ clear(max_index, base); } // O(n)
    void clear(int max_index, int base = 1){ // O(n)
        this->n = max_index; this->base = base;
        cc = 0; adj.assign(n + 1, {}); rev.assign(n + 1, {});
        comp.assign(n + 1, -1); group.clear(); ord.clear();
        v.assign(n + 1, false); ord.reserve(n + 1);
    }

    void addsol(int s, int e){ adj[s].push_back(e); rev[e].push_back(s); } // O(1)
    void build(){ // O(n + m)
        for(int i = base;i <= n;i++) if(!v[i]) dfs1(i);
        for(int i = (int)ord.size() - 1;i >= 0;i--){
            int cur = ord[i]; if(comp[cur] != -1) continue;
            group.push_back({});
            dfs2(cur, cc); cc++;
        }
    }

    int group_size(int cid) const{ return (int)group[cid].size(); } // O(1)
    int component(int x) const{ return comp[x]; } // O(1)
    bool same(int a, int b) const{ return comp[a] == comp[b]; } // O(1)
    int comp_cnt() const{ return cc; } // O(1)
    int root(int cid) const{ return group[cid][0]; } // O(1)

    vector <int> ind() const{ // O(n + m)
        vector<int> ret(cc, 0);
        it_dag([&](int a, int b){ ret[b]++; });
        return ret;
    }

    vector <int> outd() const{ // O(n + m)
        vector<int> ret(cc, 0);
        it_dag([&](int a, int b){ ret[a]++; });
        return ret;
    }

    template <bool REV = false, class F>
    void it_cid(const F& f) const{ // O(n)
        if constexpr(!REV) for(int i = 0;i < cc;i++) f(i);
        else for(int i = cc - 1;i >= 0;i--) f(i);
    }

    template <bool REV = false, class F>
    void it_all(const F& f) const{ // O(n)
        it_cid<REV>([&](int cid){ for(auto& i : group[cid]) f(i); });
    }

    template<class F>
    void it_group(int cid, const F& f) const{ // O(n)
        for(auto& cur : group[cid]) f(cur);
    }

    template <class F>
    void it_dag(const F& f) const{ // O(n + m)
        vector<int> num(cc, -1);
        for(int i = 0;i < cc;i++){
            for(auto& cur : group[i]){
                for(auto& nxt : adj[cur]){
                    int j = comp[nxt];
                    if(i == j || i == num[j]) continue;
                    num[j] = i; f(i, j);
                }
            }
        }
    }

    template <bool REV = true, class F, class G>
    void it_top(const F& merge, const G& done) const{ // O(n + m)
        vector<vector<int>> adj(cc); vector<int> ind(cc, 0);
        it_dag([&](int a, int b){ 
            if constexpr (REV) swap(a, b);
            adj[a].push_back(b), ind[b]++; 
        });
        deque <int> q; for(int i = 0;i < cc;i++) if(!ind[i]) q.push_back(i);
        while(!q.empty()){
            auto cur = q.front(); q.pop_front(); done(cur);
            for(auto& nxt : adj[cur]){
                merge(cur, nxt);
                if(!--ind[nxt]) q.push_back(nxt);
            }
        }
    }
};

//2SAT
class _2sat{ // selcet base
private:
    int n = 0; bool base = 1;
public:
    _scc scc; 
    _2sat(int max_index = 0){ clear(max_index); } // O(n)
    void clear(int max_index, bool base = 1){ // O(n)
        this->n = max_index; this->base = base;
        scc.clear(2 * n + base - 1, base);
    }

    int pos(int x) const{ return x; } // O(1)
    int neg(int x) const{ return x + n; } // O(1)
    int op(int x) const{ return x < n + base ? x + n : x - n; } // O(1)

    void add(int a, int b){ scc.addsol(a, b); } // O(1)
    void add_imp(int a, int b){ add_or(op(a), b); } // O(1)
    void add_or(int a, int b){ add(op(a), b); add(op(b), a); } // O(1)
    void add_true(int x){ add(op(x), x); } // O(1)
    void add_false(int x){ add_true(op(x)); } // O(1)
    void add_eq(int a, int b){ add(a, b); add(b, a); } // O(1)
    void add_xor(int a, int b){ add_or(a, b); add_or(op(a), op(b)); } // O(1)
    void add_most1(int a, int b){ add_or(op(a), op(b)); } // O(1)
    
    void add_most1(int a, int b, int c){ add_most1(a, b); add_most1(b, c); add_most1(a, c); } // O(1)
    void add_least2(int a, int b, int c){ add_or(a, b); add_or(a, c); add_or(b, c); } // O(1)

    bool solve(){ // O(n + m)
        scc.build(); 
        for(int i = base;i < base + n;i++){
            if(scc.same(pos(i), neg(i))) return false;
        }
        return true;
    }

    template<class F>
    void it(const F& f) const{ // O(n)
        for(int i = base; i < base + 2 * n; i++){
            int x = (scc.component(i) > scc.component(op(i))) ? 1 : 0;
            f(i, x);
        }
    }
};


//이분 매칭
class _bm { // 0-based index
private:
    vector<vector<int>> adj; int as, bs;
    vector<int> av, bv, dist;
    static constexpr int INF = 0x3f3f3f3f;
    bool bfs(){
        queue <int> q;
        for(int i = 0;i <= as;i++){
            if(av[i] == -1) dist[i] = 0, q.push(i);
            else dist[i] = INF;
        }
        int mn = INF;
        while(!q.empty()){
            int cur = q.front(); q.pop();
            if(dist[cur] >= mn) continue;
            for(auto& nxt : adj[cur]){
                int tmp = bv[nxt];
                if(tmp == -1) mn = dist[cur] + 1;
                else if(dist[tmp] >= INF) dist[tmp] = dist[cur] + 1, q.push(tmp);
            }
        }
        return mn < INF;
    }

    bool dfs(int cur){
        for(auto& nxt : adj[cur]){
            int tmp = bv[nxt];
            if(tmp == -1 || (dist[tmp] == dist[cur] + 1 && dfs(tmp))){
                av[cur] = nxt; bv[nxt] = cur;
                return true;
            }
        }
        dist[cur] = INF; return false;
    }
public:
    _bm(int a = 0, int b = 0){ clear(a, b); } // O(n)
    void clear(int a, int b){ // O(n)
        as = a; bs = b;
        adj.assign(as + 1, {}); dist.assign(as + 1, 0);
        av.assign(as + 1, -1); bv.assign(bs + 1, -1);
    }

    void addsol(int s, int e){ adj[s].push_back(e); } // O(1)
    int ret(){ // O(m sqrt n)
        int result = 0;
        while(bfs()){
            for(int i = 0;i <= as;i++){
                if(av[i] != -1) continue;
                result += dfs(i);
            }
        }
        return result;
    }

    template <class F>
    void it_all(const F& f) const{
        for(int i = 0;i <= as;i++) f(i, av[i]);
    }

    template <class F>
    void it_matched(const F& f) const{
        for(int i = 0;i <= as;i++) if(av[i] != -1) f(i, av[i]);
    }
};

//BCC
class _bcc{ // 1-based index
public:
    vector <vector <ll>> adj;
    vector <ll> mn, seq, num;
    ll n, cnt;
    stack <pll> st;

    _bcc(ll n) : n(n) {
        adj.resize(n + 1); mn.resize(n + 1);
        seq.resize(n + 1); num.resize(n + 1);
    }

    void add(ll s, ll e){
        adj[s].push_back(e);
        adj[e].push_back(s);
    }

    void clear(){
        seq.clear(); mn.clear(); cnt = 0;
        seq.resize(n + 1); mn.resize(n + 1);
    }

    void dfs_point(vector <ll>& arr, ll cur, ll pre = -1){
        bool flag = 0; ll now = 0;
        seq[cur] = mn[cur] = ++cnt;

        for(auto& nxt : adj[cur]){
            if(nxt == pre) continue;
            
            if(seq[nxt]) mn[cur] = min(mn[cur], seq[nxt]);
            else{   
                dfs_point(arr, nxt, cur); now++;
                mn[cur] = min(mn[cur], mn[nxt]);
                if(pre != -1 && mn[nxt] >= seq[cur]) flag = 1, num[cur]++;
            }
        }

        if(pre == -1 && now >= 2) flag = 1, num[cur] = now;
        if(flag) arr.push_back(cur);
    }

    void dfs_line(vector <pll>& arr, ll cur, ll pre = -1){
        seq[cur] = mn[cur] = ++cnt;

        for(auto& nxt : adj[cur]){
            if(nxt == pre) continue;
            
            if(seq[nxt]) mn[cur] = min(mn[cur], seq[nxt]);
            else{   
                dfs_line(arr, nxt, cur);
                mn[cur] = min(mn[cur], mn[nxt]);
                ll l = min(cur, nxt), r = max(cur, nxt);
                if(mn[nxt] > seq[cur]) arr.push_back({l, r});
            }
        }
    }

    void dfs_ret(vector<vector <pll>>& arr, ll cur, ll pre = -1){
        seq[cur] = mn[cur] = ++cnt;

        for(auto& nxt : adj[cur]){
            if(nxt == pre) continue;
            
            if(seq[cur] > seq[nxt]) st.push({cur, nxt});
            if(seq[nxt]) mn[cur] = min(mn[cur], seq[nxt]);
            else{   
                dfs_ret(arr, nxt, cur); 
                mn[cur] = min(mn[cur], mn[nxt]);
                if(mn[nxt] >= seq[cur]){
                    vector <pll> now;
                    while(st.top() != pll(cur, nxt)){
                        now.push_back(st.top());
                        st.pop();
                    }
                    now.push_back({cur, nxt});
                    st.pop();
                    arr.push_back(now);
                }
            }
        }
    }

    // 단절점 반환
    vector <ll> point(){
        vector <ll> ret; clear();
        for(int i = 1;i <= n;i++){
            if(seq[i]) continue;
            dfs_point(ret, i);
        }

        return ret;
    }

    // 단절선 반환
    vector <pll> line(){
        vector <pll> ret; clear();
        for(int i = 1;i <= n;i++){
            if(seq[i]) continue;
            dfs_line(ret, i);
        }

        return ret;
    }

    // bcc 반환, 테스트 해봐야 함
    vector <vector <pll>> ret(){
        vector <vector <pll>> ret; clear();
        for(int i = 1;i <= n;i++){
            if(seq[i]) continue;
            dfs_ret(ret, i);
        }

        return ret;
    }
};

class _3cycle{ // 1-based index
private:
    vector <vector<int>> tmp, adj;
    vector <int> cnt; int n; bool built = 0;
public: 
    _3cycle(int n = 0){ clear(n); } // O(n)
    void clear(int n){ // O(n);
        this->n = n; adj.assign(n + 1, {}); built = 0;
        cnt.assign(n + 1, 0); tmp.assign(n + 1, {});
    }

    void addsol(int st, int en){ tmp[st].push_back(en); } // O(1)
    void add(int st, int en){ addsol(st, en); addsol(en, st); } // O(1)

    void init(){ // O(n + m)
        assert(!built); built = 1;
        for(int i = 1;i <= n;i++){
            for(auto& j : tmp[i]){
                if(tmp[i].size() < tmp[j].size()) continue;
                if(tmp[i].size() == tmp[j].size() && i <= j) continue;
                adj[i].push_back(j);
            }
        }
    }

    ll ret(ll mod = 0){ // O(m sqrt m)
        assert(built); ll c = 0;
        for(int i = 1;i <= n;i++){
            for(auto& j : adj[i]) cnt[j]++;
            for(auto& j : adj[i]){
                for(auto& k : adj[j]) if(cnt[k]){
                    c++;
                    if(mod) c %= mod;
                }
            }
            for(auto& j : adj[i]) cnt[j]--;
        }
        return c;
    } 
};

class _4cycle{ // 1-based index
private:
    vector <vector<int>> tmp, adj;
    vector <int> cnt; int n; bool built = 0;
    bool less(int a, int b){
        if(tmp[a].size() < tmp[b].size()) return 1;
        return tmp[a].size() == tmp[b].size() && a <= b;
    }
public:
    _4cycle(int n = 0){ clear(n); } // O(n)
    void clear(int n){ // O(n)
        this->n = n; adj.assign(n + 1, {}); built = 0;
        cnt.assign(n + 1, 0); tmp.assign(n + 1, {});
    }

    void addsol(int st, int en){ tmp[st].push_back(en); } // O(1)
    void add(int st, int en){ addsol(st, en); addsol(en, st); } // O(1)

    void init(){ // O(n + m)
        assert(!built); built = 1;
        for(int i = 1;i <= n;i++){
            for(auto& j : tmp[i]){
                if(tmp[i].size() < tmp[j].size()) continue;
                if(tmp[i].size() == tmp[j].size() && i <= j) continue;
                adj[i].push_back(j);
            }
        }
    }

    ll ret(ll mod = 0){ // O(m sqrt m)
        assert(built); ll c = 0;
        for(int i = 1;i <= n;i++){
            vector <int> use;
            for(auto& j : adj[i]){
                for(auto& k : adj[j]){
                    if(less(i, k)) continue;
                    if(!cnt[k]) use.push_back(k);
                    c += cnt[k]++;
                    if(mod) c %= mod;
                }
            }
            for(auto& j : use) cnt[j] = 0;
        }
        return c;
    } 
};

template <class T>
class _clist{
private:
    vector <T> to;
    vector <int> head, nxt, deg; int n;

    template <class G>
    struct view_base{
        G* g; int u, ei;
        struct it{
            G* g; int ei;
            using ref_t = conditional_t<is_const_v<G>, const T&, T&>;
            ref_t operator*() const{ return g->to[ei]; }
            it& operator++(){ ei = g->nxt[ei]; return *this; }
            bool operator!=(const it& ot) const{ return ei != ot.ei; }
        };
        it begin() const{ return {g, ei}; }
        it end() const{ return {g, -1}; }
        int size() const{ return g->deg[u]; }
        bool empty() const{ return g->deg[u] == 0; }

        auto front() const -> conditional_t<is_const_v<G>, const T&, T&>{
            return g->to[ei];
        }
    };

    struct view : view_base<_clist>{
        void clear() const{ this->g->clear_node(this->u); }
        void push_front(const T& v) const{ this->g->push_front(this->u, v); }
        void pop_front() const{ this->g->pop_front(this->u); }
    };

    void clear_node(int u){ head[u] = -1; deg[u] = 0; }
public:
    _clist(int n = 0){ clear(n); } // O(n)
    void clear(int n){ // O(n)
        this->n = n; deg.assign(n + 1, 0);
        head.assign(n + 1, -1); to.clear(); nxt.clear();
    }

    int size() const{ return n; } // O(1)
    void push_front(int x, const T& v){ // O(1)
        nxt.push_back(head[x]); to.push_back(v);
        head[x] = (int)to.size() - 1; deg[x]++;
    }

    void pop_front(int x){ // O(1)
        int h = head[x]; if(h == -1) return;
        head[x] = nxt[h]; deg[x]--;
    }

    view_base <const _clist> operator[](int u) const{ return {this, u, head[u]}; } // O(1)
    view operator[](int u){ return {this, u, head[u]}; } // O(1)
};

template <class C = ll, C CINF = INF>
class _hungarian{ // 1-based index
    vector <vector <C>> cost;
    vector <C> row, col;
    vector <int> match, pre; int n;
    void loop(int i){
        match[0] = i;
        int j0 = 0;
        vector <C> mn(n + 1, CINF);
        vector <bool> use(n + 1, 0);

        while(1){
            use[j0] = 1;
            int i0 = match[j0], j1 = 0;
            C delta = INF;
            for(int j = 1;j <= n;j++){
                if(use[j]) continue;
                C cur = cost[i0][j] - row[i0] - col[j];
                if(cur < mn[j]){
                    mn[j] = cur;
                    pre[j] = j0;
                }

                if(mn[j] < delta){
                    delta = mn[j];
                    j1 = j;
                }
            }

            for(int j = 0;j <= n;j++){
                if(use[j]){ row[match[j]] += delta; col[j] -= delta; }
                else mn[j] -= delta;
            }

            j0 = j1;
            if(!match[j0]) break;
        }

        while(1){
            int j1 = pre[j0];
            match[j0] = match[j1];
            j0 = j1;
            if(!j0) break;
        }
    }
public:
    _hungarian(int n = 0){ clear(n); }
    void clear(int n){
        this-> n = n; col.assign(n + 1, 0);
        row.assign(n + 1, 0); match.assign(n + 1, 0);
        pre.assign(n + 1, 0); cost.assign(n + 1, vector<C>(n + 1, 0));
    }

    void set(int i, int j, C v){ cost[i][j] = v; }
    C build(){
        for(int i = 1;i <= n;i++) loop(i);
        return -col[0];
    }
};