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
class _lca {
protected:
    bool base;
    int n, lg;
    vector<vector<int>> p, adj;
    vector<int> d, comp, ord;
private:
    void build_comp(int root, int cid){
        stack<int> st; st.push(root); d[root] = 0; comp[root] = cid;
        while(!st.empty()){
            int cur = st.top(); st.pop(); ord.push_back(cur);
            for(int nxt : adj[cur]){
                if(d[nxt] != -1) continue;
                p[0][nxt] = cur; d[nxt] = d[cur] + 1; comp[nxt] = cid;
                st.push(nxt);
            }
        }
    }

    int shift(int cur, int diff) const{
        for(int i = lg; i >= 0; i--) if(diff & (1ll << i)) cur = p[i][cur];
        return cur;
    }
public:
    _lca(int n = 0, bool base = 1){ clear(n, base); } // O(n log n)
    void clear(int n, bool base = 1){ // O(n log n)
        this->n = n; this->base = base; lg = 1;
        while((1ll << lg) <= n) lg++;
        p.assign(lg + 1, vector<int>(n + 1, -1));
        adj.assign(n + 1, {}); d.assign(n + 1, -1); comp.assign(n + 1, 0);
        ord.clear(); ord.reserve(n + 1);
    }

    void add(int a, int b){ addsol(a, b); addsol(b, a); } // O(1)
    void addsol(int a, int b){ adj[a].push_back(b); } // O(1)
    void build(){ // O(n log n)
        for(auto& row : p) fill(all(row), -1);
        fill(all(d), -1); fill(all(comp), 0); ord.clear();
        int cid = 0;
        for(int i = base; i <= n; i++) if(d[i] == -1) build_comp(i, ++cid);
        for(int i = 1; i <= lg; i++) for(int v = base; v <= n; v++){
            int mid = p[i - 1][v];
            if(mid != -1) p[i][v] = p[i - 1][mid];
        }
    }

    int component(int v) const{ return comp[v]; } // O(1)
    bool same_comp(int a, int b) const{ return comp[a] == comp[b]; } // O(1)
    int depth(int v) const{ return d[v]; } // O(1)
    int parent(int v) const{ return p[0][v]; } // O(1)
    int ancestor(int v, int k) const{ // O(log n)
        if(k < 0 || k > d[v]) return -1;
        return shift(v, k);
    }

    template<bool REV = true, class F>
    void it(const F& f) const{ // O(n)
        if constexpr (!REV) for(int v : ord) f(v);
        else for(auto iter = ord.rbegin(); iter != ord.rend(); ++iter) f(*iter);
    }

    int ret(int a, int b) const{ // O(log n)
        if(!same_comp(a, b)) return -1;
        if(d[a] < d[b]) swap(a, b);
        a = shift(a, d[a] - d[b]);
        if(a == b) return a;
        for(int i = lg; i >= 0; i--){
            if(p[i][a] == p[i][b]) continue;
            a = p[i][a]; b = p[i][b];
        }
        return p[0][a];
    }

    int dist(int a, int b) const{ // O(log n)
        int l = ret(a, b);
        return l == -1 ? -1 : d[a] + d[b] - 2 * d[l];
    }

    int jump(int a, int b, int k) const{ // O(log n)
        int l = ret(a, b);
        if(l == -1) return -1;
        int da = d[a] - d[l], db = d[b] - d[l];
        if(k < 0 || k > da + db) return -1;
        return k <= da ? shift(a, k) : shift(b, da + db - k);
    }
};

class lca_policy {
public:
    class node {
    public:
        ll v;
        node(ll v) : v(v){}
    };
    class cost {
    public:
        ll v;
        cost(ll v = 0) : v(v){}
    };
    static node id(){ return {MINF}; }
    static node lift(const cost& c){ return {c.v}; }
    static node merge(const node& a, const node& b){ return {max(a.v, b.v)}; }
};

template<class policy = lca_policy>
class _lca_path : public _lca {
private:
    using node = typename policy::node;
    using cost = typename policy::cost;
    class path {
    public:
        node up, down;
    };
    vector<vector<pair<cost,cost>>> weight;
    vector<vector<path>> arr;

    static path merge(const path& a, const path& b){
        return {policy::merge(a.up, b.up), policy::merge(b.down, a.down)};
    }
public:
    _lca_path(int n = 0, bool base = 1) : _lca(n, base), weight(n + 1){} // O(n log n)
    void clear(int n, bool base = 1){ // O(n log n)
        _lca::clear(n, base); weight.assign(n + 1, {}); arr.clear();
    }

    void add(int a, int b, cost co = cost()){ add(a, b, co, co); } // O(1)
    void add(int a, int b, cost ab, cost ba){ addsol(a, b, ab, ba); addsol(b, a, ba, ab); } // O(1)
    void addsol(int a, int b, cost co = cost()){ addsol(a, b, co, co); } // O(1)
    void addsol(int a, int b, cost ab, cost ba){ // O(1)
        _lca::addsol(a, b); weight[a].push_back({ab, ba});
    }

    void build(){ // O(n log n)
        _lca::build();
        arr.assign(lg + 1, vector<path>(n + 1, {policy::id(), policy::id()}));
        vector<bool> used(n + 1, 0);
        for(int a = base; a <= n; a++) for(int i = 0; i < (int)adj[a].size(); i++){
            int b = adj[a][i];
            if(p[0][b] != a || used[b]) continue;
            used[b] = 1;
            const auto& [ab, ba] = weight[a][i];
            arr[0][b] = {policy::lift(ba), policy::lift(ab)};
        }
        for(int i = 1; i <= lg; i++) for(int v = base; v <= n; v++){
            int mid = p[i - 1][v];
            if(mid != -1) arr[i][v] = merge(arr[i - 1][v], arr[i - 1][mid]);
        }
    }

    node query(int a, int b) const{ // O(log n)
        node left = policy::id(), right = policy::id();
        int l = ret(a, b);
        if(l == -1) return left;
        int da = d[a] - d[l], db = d[b] - d[l];
        for(int i = lg; i >= 0; i--){
            if(da & (1ll << i)){
                left = policy::merge(left, arr[i][a].up); a = p[i][a];
            }
            if(db & (1ll << i)){
                right = policy::merge(arr[i][b].down, right); b = p[i][b];
            }
        }
        return policy::merge(left, right);
    }
};

// Virtual Tree
class _vtree {
    bool base;
private:
    vector <vector<int>> lift;
    vector <ll> dist;
    vector <int> in, out, dep, used;
    int n, lg, seq; bool built = 0;

    bool is_ancestor(int a, int b) const{
        return in[a] <= in[b] && out[b] <= out[a];
    }

    void sort_node(vector <int>& node) const{
        sort(all(node), [&](int a, int b){
            return in[a] < in[b];
        });
    }

    void clear_vt(){
        for(auto& v : used) vt[v].clear();
        used.clear();
    }

    int lca(int a, int b) const{
        if(is_ancestor(a, b)) return a;
        if(is_ancestor(b, a)) return b;
        for(int i = lg;i >= 0;i--){
            if(!is_ancestor(lift[i][a], b)) a = lift[i][a];
        }
        return lift[0][a];
    }

    void dfs(int cur, int pre){
        in[cur] = ++seq;
        lift[0][cur] = pre;
        for(int i = 1;i <= lg;i++){
            lift[i][cur] = lift[i - 1][lift[i - 1][cur]];
        }

        for(auto& [nxt, co] : adj[cur]){
            if(nxt == pre) continue;
            dep[nxt] = dep[cur] + 1;
            dist[nxt] = dist[cur] + co;
            dfs(nxt, cur);
        }
        out[cur] = seq;
    }
public:
    vector <vector<pair<int, ll>>> adj, vt;
    _vtree(int n = 0, bool base = 1){ clear(n, base); } // O(1)
    void clear(int n, bool base = 1){ // O(n)
        this->n = n; this->base = base; seq = 0; built = 0; lg = 1;
        while((1ll << lg) <= n) lg++;
        adj.assign(n + 1, {}); vt.assign(n + 1, {});
        lift.assign(lg + 1, vector<int>(n + 1, 0));
        in.assign(n + 1, 0); out.assign(n + 1, 0);
        dep.assign(n + 1, 0); dist.assign(n + 1, 0);
        used.clear();
    }

    void add(int a, int b, ll c = 1){
        adj[a].push_back({b, c});
        adj[b].push_back({a, c});
        built = 0;
    } // O(1)

    void build(int root = -1){ // O(n log n)
        seq = 0; built = 1; clear_vt();
        for(auto& row : lift) fill(all(row), 0);
        fill(all(in), 0); fill(all(out), 0);
        fill(all(dep), 0); fill(all(dist), 0);
        if(n < base) return;
        if(root == -1) root = base;
        assert(base <= root && root <= n);
        dfs(root, root);
    }

    int depth(int v) const { return dep[v]; } // O(1)
    int parent(int v) const { return lift[0][v]; } // O(1)
    ll dist_root(int v) const { return dist[v]; } // O(1)
    int ret(int a, int b) const { return lca(a, b); } // O(log n)

    ll dist_tree(int a, int b) const{ // O(log n)
        int c = lca(a, b);
        return dist[a] + dist[b] - 2 * dist[c];
    }

    vector <int> make(vector <int> node){ // O(k log k)
        clear_vt();
        if(node.empty()) return {};
        sort_node(node);

        int sz = node.size();
        for(int i = 0;i < sz - 1;i++) node.push_back(lca(node[i], node[i + 1]));

        sort_node(node);
        node.erase(unique(all(node)), node.end());
        used = node;

        vector<int> st = {node[0]};
        for(int i = 1;i < node.size();i++){
            int v = node[i];
            while(!st.empty() && !is_ancestor(st.back(), v)){
                st.pop_back();
            }

            vt[st.back()].push_back({v, dist[v] - dist[st.back()]});
            st.push_back(v);
        }
        return node;
    }
};

// Sparse Table
class _st {
    bool base;
private:
    vector <vector<ll>> sum;
    vector <vector<int>> lift; ll m; int n, lg; 
public:
    _st(int n = 0, ll max_step = 0, bool base = 1){ clear(n, max_step, base); }; // O(n log m)
    void clear(int n, ll m = 60, bool base = 1){ // O(n log m)
        lg = 1; this->n = n; this->base = base; this->m = m;
        while((1ll << lg) <= m) lg++;
        lift.assign(lg + 1, vector<int>(n + 2, n + 1));
        sum.assign(lg + 1, vector<ll>(n + 2, 1));
    }

    void add(int st, int en, ll co = 1) { assert(base <= st && st <= n && base <= en && en <= n + 1); lift[0][st] = en; sum[0][st] = co; } // O(1)
    void build(){ // O(n log m)
        for(int i = 1;i <= lg;i++){
            for(int j = base;j <= n;j++){
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
    bool base;
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
        uf.clear(n); result = zero(); built = 1;
        sort(all(adj)); int cnt = 0;
        for(auto& i : adj){
            auto& [st, en, c] = i;
            if(cnt >= n - base - num) return 1;
            if(!uf.merge(st, en)) continue;
            result = add(result, c); cnt++;
        }
        return cnt >= n - base - num;
    }
public:
    _mst(int n = 0, bool base = 1){ clear(n, base); } // O(n)
    void clear(int n, bool base = 1){ // O(n)
        this->n = n; this->base = base; result = zero();
        uf.clear(n); built = 0; adj.clear();
    }

    void add(int st, int en, cost c){ // O(1)
        assert(base <= st && st <= n && base <= en && en <= n);
        adj.push_back({st, en, c}); 
    }

    bool build(int num = 0){ return cal(num);} // O(m log m)
    cost ret(){ // O(1)

        return result; 
    }
};

//SCC
class _scc{
protected:
    vector <vector<int>> adj, rev, group;
    vector <int> comp, ord; vector <bool> v; int n, cc; bool base;
private:
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
    _scc(int max_index = 0, bool base = 1){ clear(max_index, base); } // O(n)
    void clear(int max_index, bool base = 1){ // O(n)
        this->n = max_index; this->base = base;
        cc = 0; adj.assign(n + 1, {}); rev.assign(n + 1, {});
        comp.assign(n + 1, -1); group.clear(); ord.clear();
        v.assign(n + 1, false); ord.reserve(n + 1);
    }

    void addsol(int s, int e){ adj[s].push_back(e); rev[e].push_back(s); } // O(1)
    void build(){ // O(n + m)
        cc = 0; ord.clear(); group.clear(); fill(all(v), false); fill(all(comp), -1);
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

};

class _scc_dag : public _scc {
private:
    array<vector<vector<int>>, 2> edges;
    array<vector<int>, 2> degree, order;
public:
    _scc_dag(int max_index = 0, bool base = 1) : _scc(max_index, base){} // O(n)
    void clear(int max_index, bool base = 1){ // O(n + m)
        _scc::clear(max_index, base);
        for(auto& g : edges) g.clear();
        for(auto& d : degree) d.clear();
        for(auto& v : order) v.clear();
    }

    void build(){ // O(n + m)
        _scc::build();
        for(auto& g : edges) g.assign(cc, {});
        for(auto& d : degree) d.assign(cc, 0);
        vector<int> mark(cc, -1);
        for(int i = 0; i < cc; i++) for(int cur : group[i]) for(int nxt : adj[cur]){
            int j = comp[nxt];
            if(i == j || mark[j] == i) continue;
            mark[j] = i;
            edges[0][i].push_back(j); edges[1][j].push_back(i);
            degree[0][j]++; degree[1][i]++;
        }
        for(int dir = 0; dir < 2; dir++){
            auto pending = degree[dir];
            auto& top = order[dir]; top.clear(); top.reserve(cc);
            for(int i = 0; i < cc; i++) if(!pending[i]) top.push_back(i);
            for(size_t i = 0; i < top.size(); i++)
                for(int nxt : edges[dir][top[i]]) if(!--pending[nxt]) top.push_back(nxt);
        }
    }

    const vector<vector<int>>& dag() const{ return edges[0]; } // O(1)
    const vector<int>& ind() const{ return degree[0]; } // O(1)
    const vector<int>& outd() const{ return degree[1]; } // O(1)

    template<class F>
    void it_dag(const F& f) const{ // O(k + e)
        for(int i = 0; i < cc; i++) for(int j : edges[0][i]) f(i, j);
    }

    template<bool REV = true, class F, class G>
    void it_top(const F& merge, const G& done) const{ // O(k + e)
        for(int cur : order[REV]){
            done(cur);
            for(int nxt : edges[REV][cur]) merge(cur, nxt);
        }
    }
};

//2SAT
class _2sat{
private:
    int n = 0, vars = 0; bool base = 1;
public:
    _scc scc; 
    _2sat(int max_index = 0, bool base = 1){ clear(max_index, base); } // O(n)
    void clear(int max_index, bool base = 1){ // O(n)
        this->n = max_index; this->base = base;
        vars = n - base + 1;
        scc.clear(n + vars, base);
    }

    int pos(int x) const{ return x; } // O(1)
    int neg(int x) const{ return x + vars; } // O(1)
    int op(int x) const{ return x <= n ? x + vars : x - vars; } // O(1)

    void add(int a, int b){ scc.addsol(a, b); } // O(1)
    void add_imp(int a, int b){ add_or(op(a), b); } // O(1)
    void add_or(int a, int b){ add(op(a), b); add(op(b), a); } // O(1)
    void add_true(int x){ add(op(x), x); } // O(1)
    void add_false(int x){ add_true(op(x)); } // O(1)
    void add_eq(int a, int b){ add_imp(a, b); add_imp(b, a); } // O(1)
    void add_xor(int a, int b){ add_or(a, b); add_or(op(a), op(b)); } // O(1)
    void add_most1(int a, int b){ add_or(op(a), op(b)); } // O(1)
    
    void add_most1(int a, int b, int c){ add_most1(a, b); add_most1(b, c); add_most1(a, c); } // O(1)
    void add_least2(int a, int b, int c){ add_or(a, b); add_or(a, c); add_or(b, c); } // O(1)

    bool build(){ // O(n + m)
        scc.build(); 
        for(int i = base;i <= n;i++){
            if(scc.same(pos(i), neg(i))) return false;
        }
        return true;
    }

    template<class F>
    void it(const F& f) const{ // O(n)
        for(int i = base; i <= n + vars; i++){
            int x = (scc.component(i) > scc.component(op(i))) ? 1 : 0;
            f(i, x);
        }
    }
};


//이분 매칭
class _bm {
    bool base;
private:
    vector<vector<int>> adj; int as, bs, result;
    vector<int> av, bv, dist;
    static constexpr int INF = 0x3f3f3f3f;
    bool bfs(){
        queue <int> q;
        for(int i = base;i <= as;i++){
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
    _bm(int a = 0, int b = 0, bool base = 1){ clear(a, b, base); } // O(n)
    void clear(int a, int b, bool base = 1){ // O(n)
        as = a; bs = b; result = 0; this->base = base;
        adj.assign(as + 1, {}); dist.assign(as + 1, 0);
        av.assign(as + 1, -1); bv.assign(bs + 1, -1);
    }

    void addsol(int s, int e){ adj[s].push_back(e); } // O(1)
    int build(){ // O(m sqrt n)
        result = 0; fill(all(av), -1); fill(all(bv), -1); fill(all(dist), 0);
        while(bfs()){
            for(int i = base;i <= as;i++){
                if(av[i] != -1) continue;
                result += dfs(i);
            }
        }
        return result;
    }

    int ret() const{ return result; } // O(1)

    template <class F>
    void it_all(const F& f) const{
        for(int i = base;i <= as;i++) f(i, av[i]);
    }

    template <class F>
    void it_matched(const F& f) const{
        for(int i = base;i <= as;i++) if(av[i] != -1) f(i, av[i]);
    }
};

//BCC
class _bcc{
    bool base;
    vector <ll> points;
    vector <pll> lines;
    vector <vector <pll>> blocks;

    void dfs(ll cur, ll pre = -1){
        seq[cur] = mn[cur] = ++cnt;
        ll children = 0; bool cut = false, skipped_parent = false;
        for(auto nxt : adj[cur]){
            if(nxt == pre && !skipped_parent){ skipped_parent = true; continue; }
            if(seq[nxt]){
                mn[cur] = min(mn[cur], seq[nxt]);
                if(seq[nxt] < seq[cur]) st.push({cur, nxt});
                continue;
            }

            st.push({cur, nxt}); dfs(nxt, cur); children++;
            mn[cur] = min(mn[cur], mn[nxt]);
            if(mn[nxt] > seq[cur]) lines.push_back({min(cur, nxt), max(cur, nxt)});
            if(mn[nxt] >= seq[cur]){
                if(pre != -1){ cut = true; num[cur]++; }
                vector <pll> block;
                while(true){
                    auto edge = st.top(); st.pop(); block.push_back(edge);
                    if(edge == pll(cur, nxt)) break;
                }
                blocks.push_back(move(block));
            }
        }
        if(pre == -1 && children >= 2){ cut = true; num[cur] = children; }
        if(cut) points.push_back(cur);
    }
public:
    vector <vector <ll>> adj;
    vector <ll> mn, seq, num;
    ll n, cnt;
    stack <pll> st;

    _bcc(ll n = 0, bool base = 1){ clear(n, base); }
    void add(ll s, ll e){
        adj[s].push_back(e); adj[e].push_back(s);
    }

    void clear(ll n, bool base = 1){
        this->n = n; this->base = base;
        adj.assign(n + 1, {}); clear();
    }
    void clear(){
        seq.assign(n + 1, 0); mn.assign(n + 1, 0); num.assign(n + 1, 0); cnt = 0;
        while(!st.empty()) st.pop();
        points.clear(); lines.clear(); blocks.clear();
    }

    void build(){ // O(n + m)
        clear();
        for(int i = base;i <= n;i++) if(!seq[i]) dfs(i);
    }
    const vector<ll>& point() const{ return points; } // O(1)
    const vector<pll>& line() const{ return lines; } // O(1)
    const vector<vector<pll>>& ret() const{ return blocks; } // O(1)
};

class _3cycle{
    bool base;
private:
    vector <vector<int>> tmp, adj;
    vector <int> cnt; int n; ll result = 0;
public: 
    _3cycle(int n = 0, bool base = 1){ clear(n, base); } // O(n)
    void clear(int n, bool base = 1){ // O(n);
        this->n = n; this->base = base; adj.assign(n + 1, {}); result = 0;
        cnt.assign(n + 1, 0); tmp.assign(n + 1, {});
    }

    void addsol(int st, int en){ tmp[st].push_back(en); } // O(1)
    void add(int st, int en){ addsol(st, en); addsol(en, st); } // O(1)

    ll build(ll mod = 0){ // O(n + m sqrt m)
        adj.assign(n + 1, {}); fill(all(cnt), 0); result = 0;
        for(int i = base;i <= n;i++){
            for(auto& j : tmp[i]){
                if(tmp[i].size() < tmp[j].size()) continue;
                if(tmp[i].size() == tmp[j].size() && i <= j) continue;
                adj[i].push_back(j);
            }
        }

        ll c = 0;
        for(int i = base;i <= n;i++){
            for(auto& j : adj[i]) cnt[j]++;
            for(auto& j : adj[i]){
                for(auto& k : adj[j]) if(cnt[k]){
                    c++;
                    if(mod) c %= mod;
                }
            }
            for(auto& j : adj[i]) cnt[j]--;
        }
        return result = c;
    }
    ll ret() const{ return result; } // O(1)
};

class _4cycle{
    bool base;
private:
    vector <vector<int>> tmp, adj;
    vector <int> cnt; int n; ll result = 0;
    bool less(int a, int b){
        if(tmp[a].size() < tmp[b].size()) return 1;
        return tmp[a].size() == tmp[b].size() && a <= b;
    }
public:
    _4cycle(int n = 0, bool base = 1){ clear(n, base); } // O(n)
    void clear(int n, bool base = 1){ // O(n)
        this->n = n; this->base = base; adj.assign(n + 1, {}); result = 0;
        cnt.assign(n + 1, 0); tmp.assign(n + 1, {});
    }

    void addsol(int st, int en){ tmp[st].push_back(en); } // O(1)
    void add(int st, int en){ addsol(st, en); addsol(en, st); } // O(1)

    ll build(ll mod = 0){ // O(n + m sqrt m)
        adj.assign(n + 1, {}); fill(all(cnt), 0); result = 0;
        for(int i = base;i <= n;i++){
            for(auto& j : tmp[i]){
                if(tmp[i].size() < tmp[j].size()) continue;
                if(tmp[i].size() == tmp[j].size() && i <= j) continue;
                adj[i].push_back(j);
            }
        }

        ll c = 0;
        for(int i = base;i <= n;i++){
            vector <int> use;
            for(auto& j : adj[i]){
                for(auto& k : tmp[j]){
                    if(less(i, k)) continue;
                    if(!cnt[k]) use.push_back(k);
                    c += cnt[k]++;
                    if(mod) c %= mod;
                }
            }
            for(auto& j : use) cnt[j] = 0;
        }
        return result = c;
    }
    ll ret() const{ return result; } // O(1)
};

template <class T>
class _clist{
    bool base;
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
    _clist(int n = 0, bool base = 1){ clear(n, base); } // O(n)
    void clear(int n, bool base = 1){ // O(n)
        this->n = n; this->base = base; deg.assign(n + 1, 0);
        head.assign(n + 1, -1); to.clear(); nxt.clear();
    }

    int size() const{ return n - base + 1; } // O(1)
    void push_front(int x, const T& v){
        assert(base <= x && x <= n); // O(1)
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
class _hungarian{
    bool base;
    vector <vector <C>> cost;
    vector <C> row, col;
    vector <int> match, pre; int n, sz;
    void loop(int i){
        match[0] = i;
        int j0 = 0;
        vector <C> mn(sz + 1, CINF);
        vector <bool> use(sz + 1, 0);

        while(1){
            use[j0] = 1;
            int i0 = match[j0], j1 = 0;
            C delta = CINF;
            for(int j = 1;j <= sz;j++){
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

            for(int j = 0;j <= sz;j++){
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
    _hungarian(int n = 0, bool base = 1){ clear(n, base); }
    void clear(int n, bool base = 1){
        this->n = n; this->base = base; sz = n - base + 1; col.assign(sz + 1, 0);
        row.assign(sz + 1, 0); match.assign(sz + 1, 0);
        pre.assign(sz + 1, 0); cost.assign(sz + 1, vector<C>(sz + 1, 0));
    }

    void set(int i, int j, C v){
        assert(base <= i && i <= n && base <= j && j <= n);
        cost[i - base + 1][j - base + 1] = v;
    }
    C build(){
        fill(all(row), 0); fill(all(col), 0); fill(all(match), 0); fill(all(pre), 0);
        for(int i = 1;i <= sz;i++) loop(i);
        return -col[0];
    }
};
