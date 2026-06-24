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

struct monoid{
    struct node {
        ll v;
        node(ll v = 0) : v(v){}
    };
    static node op(const node& l, const node& r){
        return {
            l.v + r.v
        };
    }
};

template <class policy = monoid>
class _seg {
public:
    using node = typename policy::node;
    node op(const node& l, const node& r) const{ return policy::op(l, r); }
    node id() const{ return node(); }
private:
    int n, sz; vector <node> seg;
    node join(const node& a, const node& b, bool right) const{ return right ? op(a, b) : op(b, a); }
    template<class F>
    int bisect(int p, bool right, const F& f) {
        assert(0 <= p && p <= n); assert(f(id()));
        if(right) { if (p == n) return n; }
        else { if (p == 0) return 0; }
        int i = right ? (p + sz) : (p + 1 + sz); node v = id();
        while(1){
            if(!right) --i;
            if(right) while ((i & 1) == 0) i >>= 1;
            if(!right) while (i > 1 && (i & 1)) i >>= 1;
            node tmp1 = join(v, seg[i], right);
            if (!f(tmp1)) {
                while (i < sz) {
                    i = (i << 1) | (!right);         
                    node tmp2 = join(v, seg[i], right);
                    if(f(tmp2)){ v = tmp2; i += right ? 1 : -1; }
                }
                return right ? (i - sz - 1) : (i + 1 - sz);
            }
            v = tmp1;
            if(right) ++i; if((i & -i) == i) break;
        }
        return right ? n : 0;
    }
public:
    _seg(int n = 0){ clear(n); } // O(n)
    _seg(span<const node> arr){ build(arr); } // O(n)
    void clear(int n){ // O(n)
        this->n = n;
        sz = 1; while(sz < n + 1) sz <<= 1;
        seg.assign(2 * sz, id());
    }

    void build(span<const node> arr){ // O(n)
        if(arr.empty()){ clear(0); return; }
        clear((int)arr.size() - 1);
        for(int i = 0;i < (int)arr.size();i++) seg[i + sz] = arr[i];
        for(int i = sz - 1;i >= 1;i--) seg[i] = op(seg[i << 1], seg[i << 1 | 1]);
    }

    node query(int st, int en){ // O(log n)
        st = max(0, st); en = min(n, en);
        if(st > en) return id();
        node l = id(), r = id();
        st += sz; en += sz;
        while(st <= en){
            if(st & 1) l = op(l, seg[st++]);
            if(!(en & 1)) r = op(seg[en--], r);
            st >>= 1; en >>= 1; 
        }
        return op(l, r);
    }

    void set(int idx, const node& v){ // O(log n)
        if(idx < 0 || idx > n) return;
        int p = idx + sz; seg[p] = v;
        for(p >>= 1; p; p >>= 1) seg[p] = op(seg[p << 1], seg[p << 1 | 1]);
    }

    template<class F>
    int max_right(int l, const F& f){ return bisect(l, 1, f); } // O(log n)

    template<class F>
    int min_left(int r, const F& f){  return bisect(r, 0, f); } // O(log n)
};

struct action{
    struct node {
        ll v; int sz;
        node(ll v = 0, int sz = 0) : v(v), sz(sz){}
    };
    struct lazy{
        ll v;
        lazy(ll v = 0) : v(v){}
    };
    static node op(const node& l, const node& r){
        return node{
            l.v + r.v,
            l.sz + r.sz
        };
    }
    static bool is_lz_id(const lazy& now){ return now.v == 0; }
    static void apply(node& seg, const lazy& lz){ seg.v += lz.v * seg.sz; }
    static void compose(lazy& s, const lazy& p){ s.v += p.v; }
};

template <class policy = action>
class _prop { 
public:
    using node = typename policy::node;
    using lazy = typename policy::lazy;
    node op(const node& l, const node& r) const{ return policy::op(l, r); }
    node id() const{ return node(); }
private:
    vector <node> seg; vector <lazy> lz; int sz, h, n;
    lazy lz_id() const{ return lazy(); }
    bool is_lz_id(const lazy& now) const{ return policy::is_lz_id(now); }
    void prop(node& seg, const lazy& lz) const{ policy::apply(seg, lz); }
    void comp(lazy& s, const lazy& p) const{ policy::compose(s, p); }
    node join(const node& a, const node& b, bool right) const{ return right ? op(a, b) : op(b, a); }

    void push(int idx){
        if(is_lz_id(lz[idx])) return;
        apply(idx << 1, lz[idx]);
        apply(idx << 1 | 1, lz[idx]);
        lz[idx] = lz_id();
    }

    void propagate(int idx){
        for(int i = h;i > 0;i--){
            int cur = idx >> i;
            push(cur);
        }
    }

    void apply(int idx, const lazy& now){
        prop(seg[idx], now);
        if(idx < sz) comp(lz[idx], now); 
    }

    void rebuild(int idx){
        while(idx > 1){
            idx >>= 1;
            seg[idx] = op(seg[idx << 1], seg[idx << 1 | 1]);
            if(is_lz_id(lz[idx])) continue;
            prop(seg[idx], lz[idx]);
        }
    }

    template<class F>
    int bisect(int p, bool right, const F& f) {
        assert(0 <= p && p <= n); assert(f(id()));
        if(right) { if (p == n) return n; }
        else { if (p == 0) return 0; }
        int i = right ? (p + sz) : (p + 1 + sz);
        if(right) propagate(i); else propagate(i - 1);
        node v = id();
        while(1){
            if(!right) --i;
            if(right) while ((i & 1) == 0) i >>= 1;
            if(!right) while (i > 1 && (i & 1)) i >>= 1;
            node tmp1 = join(v, seg[i], right);
            if (!f(tmp1)) {
                while (i < sz) {
                    push(i);
                    i = (i << 1) | (!right);
                    node tmp2 = join(v, seg[i], right);
                    if(f(tmp2)){ v = tmp2; i += right ? 1 : -1; }
                }
                return right ? (i - sz - 1) : (i + 1 - sz);
            }
            v = tmp1;
            if(right) ++i; if((i & -i) == i) break;
        }
        return right ? n : 0;
    }
public:
    _prop(int n = 0){ clear(n); } // O(n)
    _prop(span<const node> arr){ build(arr); } // O(n)
    void clear(int n){ // O(n)
        this->n = n;
        sz = 1; while(sz < n + 1) sz <<= 1;
        h = __lg(sz);
        seg.assign(2 * sz, id());
        lz.assign(2 * sz, lz_id());
    }

    void build(span<const node> arr){ // O(n)
        if(arr.empty()){ clear(0); return; }
        clear((int)arr.size() - 1);
        for(int i = 0;i < (int)arr.size();i++) seg[i + sz] = arr[i];
        for(int i = sz - 1;i >= 1;i--) seg[i] = op(seg[i << 1], seg[i << 1 | 1]);
    }

    node query(int idx){ // O(log n)
        idx += sz;
        propagate(idx);
        return seg[idx];
    }
    node query(int st, int en) { // O(log n)
        st = max(0, st); en = min(n, en);
        if(st > en) return id();

        int l = (int)st + sz, r = (int)en + sz;
        propagate(l); propagate(r);

        node nl = id(), nr = id();
        while(l <= r){
            if(l & 1) nl = op(nl, seg[l++]);
            if(!(r & 1)) nr = op(seg[r--], nr);
            l >>= 1; r >>= 1;
        }

        return op(nl, nr);
    }

    void update(int idx, const lazy& lz){ update(idx, idx, lz); } // O(log n)
    void update(int st, int en, const lazy& lz){ // O(log n)
        st = max(0, st); en = min(n, en);
        if(st > en) return;

        int l = (int)st + sz, r = (int)en + sz;
        int tl = l, tr = r;
        propagate(l); propagate(r);

        while(l <= r){
            if(l & 1) apply(l++, lz);
            if(!(r & 1)) apply(r--, lz);
            l >>= 1; r >>= 1;
        }
        
        rebuild(tl); rebuild(tr);
    }

    template<class F>
    int max_right(int l, const F& f){ return bisect(l, 1, f); } // O(log n)

    template<class F>
    int min_left(int r, const F& f){  return bisect(r, 0, f); } // O(log n)
};

template <class P>
concept is_action = requires(P s, int l, int r, const typename P::lazy& z){ s.update(l, r, z); };

template <class policy>
class _hld { // 1-based index, m=0 vertex, m=1 edge(child-index)
public:
    using node = typename policy::node;
    policy seg;
private:
    vector <vector <int>> adj, tmp;
    vector <int> sz, top, d, in, out, p;
    int n, m, cnt = 0; bool built = 0;
    void build_tree(int cur, int pre = -1){
        for(auto& nxt : tmp[cur]){
            if(pre == nxt) continue;
            adj[cur].push_back(nxt);
            build_tree(nxt, cur);
        }
        tmp[cur].clear();
    }

    void dfs1(int cur){
        sz[cur] = 1;
        for(auto& nxt : adj[cur]){
            d[nxt] = d[cur] + 1; p[nxt] = cur;
            dfs1(nxt); sz[cur] += sz[nxt];
            if(sz[nxt] > sz[adj[cur][0]]) swap(nxt, adj[cur][0]);
        }
    }

    void dfs2(int cur){
        in[cur] = ++cnt;
        for(auto& nxt : adj[cur]){
            top[nxt] = (nxt == adj[cur][0] ? top[cur] : nxt);
            dfs2(nxt);
        }
        out[cur] = cnt;
    }

    vector<pair<int,int>> get_path(int st, int en){
        vector<pair<int,int>> ret;
        while(top[st] != top[en]){
            if(d[top[st]] < d[top[en]]) swap(st, en);
            int cur = top[st], l = in[cur], r = in[st];
            if(m == 1 && p[cur] == 0) l++; 
            if(l <= r) ret.push_back({l, r});
            st = p[cur];
        }
        if(d[st] > d[en]) swap(st, en);
        int l = in[st] + m, r = in[en];
        if(l <= r) ret.push_back({l, r});
        return ret;
    }
public:
    _hld(int n = 0, int m = 0){ clear(n, m); }
    void clear(int n, int m = 0){
        this->n = n; this->m = m; cnt = 0;
        adj.assign(n + 1, {}); tmp.assign(n + 1, {}); 
        top.assign(n + 1, 0); d.assign(n + 1, 0); 
        in.assign(n + 1, 0); out.assign(n + 1, 0);
        p.assign(n + 1, 0); sz.assign(n + 1, 0);
        seg.clear(n); built = 0;
    }

    void addsol(int s, int e){ tmp[s].push_back(e); }
    void add(int s, int e){ addsol(s, e); addsol(e, s); }
    void init(){
        assert(!built); built = 1;
        for(int i = 1;i <= n;i++){
            if(sz[i]) continue; top[i] = i;
            build_tree(i); dfs1(i); dfs2(i);
        }
        tmp.clear(); tmp.shrink_to_fit();
    }

    node query(int x){
        assert(built);
        return seg.query(in[x], in[x]);
    }

    node query(int st, int en) { 
        assert(built); node ret = seg.id();
        auto path = get_path(st, en);
        for(auto &[l, r] : path) ret = seg.op(ret, seg.query(l, r));
        return ret;
    }

    node query_sub(int x){ assert(built); return seg.query(in[x] + m, out[x]); }
    void set(int x, const node& v)
        requires requires(policy s, int i, const node& nv){ s.set(i, nv); }
    { assert(built); seg.set(in[x], v); }

    void update(int x, const node& v)
        requires requires(policy s, int i, const node& nv){ s.update(i, nv); }
    { assert(built); seg.update(in[x], v); }

    template<class P = policy>
    void update(int st, int en, const typename P::lazy& lz)
        requires is_action<P>
    {
        auto path = get_path(st, en); assert(built);
        for(auto &[l, r] : path) seg.update(l, r, lz);
    }

    template<class P = policy>
    void update_sub(int x, const typename P::lazy& lz)
        requires is_action<P>
    { assert(built); seg.update(in[x] + m, out[x], lz); }
}; 

struct mt_policy {
    struct node {
        ll v;
        node(ll v = 0) : v(v){}
        bool operator < (const node& ot) const{ return v < ot.v; }
    };
};

template <class policy = mt_policy>
class _mt{ // 0-based index
public:
    using node = typename policy::node;
private:
    vector <vector <node>> seg;
    vector <node> arr; int n, sz = 1;
    static int count_node(const vector<node>& cur, const node& v, bool is_greater, bool is_less, bool is_eq){
        int ret = 0;
        auto lb = lower_bound(cur.begin(), cur.end(), v);
        auto ub = upper_bound(cur.begin(), cur.end(), v);
        if(is_eq) ret += (int)(ub - lb);                  
        if(is_greater) ret += (int)(cur.end() - ub);
        if(is_less) ret += (int)(lb - cur.begin());
        return ret; 
    }

    int query(int st, int en, const node& v, bool is_greater, bool is_less, bool is_eq) const{
        st = max(st, 0); en = min(en, n);
        if(n <= 0 || st > en) return 0;
        int l = sz + st, r = sz + en, ret = 0;
        while(l <= r){
            if(l & 1) ret += count_node(seg[l++], v, is_greater, is_less, is_eq);
            if(!(r & 1)) ret += count_node(seg[r--], v, is_greater, is_less, is_eq);
            l >>= 1; r >>= 1;
        }
        return ret;
    }
public:
    _mt(int n = 0){ clear(n); } // O(n)
    _mt(span<const node> arr){ build(arr); } // O(n log n)
    void clear(int n){ // O(n)
        this->n = n; sz = 1; while(sz < n + 1) sz <<= 1; 
        arr.assign(n + 1, node()); seg.assign(2 * sz, {});
    }

    void build(span<const node> arr){ // O(n log n)
        clear((int)arr.size() - 1); this->arr = arr; 
        for(int i = 0; i <= n; i++) seg[sz + i].assign(1, arr[i]);
        for(int i = sz - 1; i >= 1; i--){
            auto &l = seg[i << 1], &r = seg[i << 1 | 1], &cur = seg[i];
            cur.resize(l.size() + r.size());
            merge(all(l), all(r), cur.begin());
        }
    }

    void update(int idx, const node& v){ // O(n)
        if(idx < 0 || idx > n) return; arr[idx] = v;
        int p = sz + idx; seg[p].assign(1, v); 
        for(int i = p >> 1;i >= 1;i >>= 1){
            auto &l = seg[i << 1], &r = seg[i << 1 | 1], &cur = seg[i];
            cur.resize(l.size() + r.size());
            merge(all(l), all(r), cur.begin());
        }
    }

    int greater(int st, int en, const node& v) const{ return query(st, en, v, 1, 0, 0); } // O(log^2 n)
    int less(int st, int en, const node& v) const{ return query(st, en, v, 0, 1, 0); } // O(log^2 n)
    int geq(int st, int en, const node& v) const{ return query(st, en, v, 1, 0, 1); } // O(log^2 n)
    int leq(int st, int en, const node& v) const{ return query(st, en, v, 0, 1, 1); } // O(log^2 n)
    int eq(int st, int en, const node& v) const{ return query(st, en, v, 0, 0, 1); } // O(log^2 n)
};

template <class policy>
concept has_inv = requires(const typename policy::node& a){
    { policy::inv(a) } -> std::same_as<typename policy::node>;
};

struct group{
    struct node{
        ll v;
        node(ll v = 0) : v(v){}
    };
    static node op(const node& l, const node& r){
        return {l.v + r.v};
    }
    static node inv(const node& a){
        return {-a.v};
    }
};

template <class policy = group>
class _pst{
public:
    using node = typename policy::node;
    node op(const node& l, const node& r) const{ return policy::op(l, r); }
    node id() const{ return node(); }
    node inv(const node& a) const requires has_inv<policy>{ return policy::inv(a); }
private:
    vector <int> lv, rv, root; vector <node> seg;
    int n, q, lg, nc, vc;
    int clone(int idx){
        ++nc; seg[nc] = seg[idx];
        lv[nc] = lv[idx]; rv[nc] = rv[idx];
        return nc;
    }

    int set(int idx, int s, int e, int pos, const node& v){
        int cur = clone(idx);
        if(s == e){ seg[cur] = v; return cur; }
        int m = (s + e) >> 1;
        if(pos <= m) lv[cur] = set(lv[cur], s, m, pos, v);
        else rv[cur] = set(rv[cur], m + 1, e, pos, v);
        seg[cur] = op(seg[lv[cur]], seg[rv[cur]]);
        return cur;
    }

    node query(int idx, int s, int e, int l, int r) const{
        if(!idx || r < s || e < l) return id();
        if(l <= s && r >= e) return seg[idx];
        int m = (s + e) >> 1;
        node ln = query(lv[idx], s, m, l, r);
        node rn = query(rv[idx], m + 1, e, l, r);
        return op(ln, rn);
    }

    node diff_seg(int idx1, int idx2) const requires has_inv<policy>
    { return op(seg[idx2], inv(seg[idx1])); }

    template<class F>
    int bisect(int idx, int s, int e, int p, bool right, const F& f, node& v) const{
        if(right && e < p) return p - 1;
        if(!right && p < s) return p + 1;
        if(!idx) return right ? e : s;
        if((right && p <= s) || (!right && e <= p)){
            node nxt = right ? op(v, seg[idx]) : op(seg[idx], v);
            if(f(nxt)){ v = nxt; return right ? e : s; }
            if(s == e) return right ? s - 1 : s + 1;
        }
        int m = (s + e) >> 1;
        if(right){
            int ret = bisect(lv[idx], s, m, p, right, f, v);
            if(ret < m) return ret;
            return bisect(rv[idx], m + 1, e, p, right, f, v);
        }
        int ret = bisect(rv[idx], m + 1, e, p, right, f, v);
        if(ret > m + 1) return ret;
        return bisect(lv[idx], s, m, p, right, f, v);
    }

    template<class F>
    int bisect_diff(int idx1, int idx2, int s, int e, int p, bool right, const F& f, node& v) const requires has_inv<policy>{
        if(right && e < p) return p - 1;
        if(!right && p < s) return p + 1;
        if(!idx1 && !idx2) return right ? e : s;
        if((right && p <= s) || (!right && e <= p)){
            node nxt = right ? op(v, diff_seg(idx1, idx2)) : op(diff_seg(idx1, idx2), v);
            if(f(nxt)){ v = nxt; return right ? e : s; }
            if(s == e) return right ? s - 1 : s + 1;
        }
        int m = (s + e) >> 1;
        if(right){
            int ret = bisect_diff(lv[idx1], lv[idx2], s, m, p, right, f, v);
            if(ret < m) return ret;
            return bisect_diff(rv[idx1], rv[idx2], m + 1, e, p, right, f, v);
        }
        int ret = bisect_diff(rv[idx1], rv[idx2], m + 1, e, p, right, f, v);
        if(ret > m + 1) return ret;
        return bisect_diff(lv[idx1], lv[idx2], s, m, p, right, f, v);
    }
public:
    _pst(int n = 0, int q = 0){ clear(n, q); } // q -> update size
    void clear(int n, int q){ // O(q log n)
        this->n = n; this->q = q;
        lg = __lg(n + 1) + 2; nc = vc = 0;
        int mx = (q + 1) * lg + 5;
        lv.assign(mx, 0); rv.assign(mx, 0);
        root.assign(q + 1, 0); seg.assign(mx, id());
    }

    int version() const{ return vc; }
    int set(int idx, const node& v){ return set(vc, idx, v); } // O(log n)
    int set(int ver, int idx, const node& v){ // O(log n)
        assert(0 <= ver && ver <= vc);
        assert(vc + 1 <= q); assert(0 <= idx && idx <= n);
        root[++vc] = set(root[ver], 0, n, idx, v);
        return vc;
    }

    node query(int ver, int idx) const{ return query(ver, idx, idx); } // O(log n)
    node query(int ver, int l, int r) const{ // O(log n)
        assert(0 <= ver && ver <= vc);
        l = max(0, l); r = min(n, r);
        if(l > r) return id();
        return query(root[ver], 0, n, l, r);
    }

    node diff_query(int ver1, int ver2, int idx) const requires has_inv<policy>
    { return diff_query(ver1, ver2, idx, idx); } // O(log n)

    node diff_query(int ver1, int ver2, int l, int r) const requires has_inv<policy>{ // O(log n)
        ver1--; assert(0 <= ver1 && ver1 <= ver2 && ver2 <= vc);
        l = max(0, l); r = min(n, r);
        if(l > r) return id();
        return op(query(root[ver2], 0, n, l, r), inv(query(root[ver1], 0, n, l, r)));
    }

    template<class F>
    int max_right(int ver, int l, const F& f) const{
        assert(0 <= ver && ver <= vc); assert(0 <= l && l <= n);
        assert(f(id())); node v = id();
        return bisect(root[ver], 0, n, l, 1, f, v);
    }

    template<class F>
    int min_left(int ver, int r, const F& f) const{
        assert(0 <= ver && ver <= vc); assert(0 <= r && r <= n);
        assert(f(id())); node v = id();
        return bisect(root[ver], 0, n, r, 0, f, v);
    }

    template<class F>
    int diff_max_right(int ver1, int ver2, int l, const F& f) const requires has_inv<policy>{
        ver1--; assert(0 <= ver1 && ver1 <= ver2 && ver2 <= vc);
        assert(0 <= l && l <= n); assert(f(id())); node v = id();
        return bisect_diff(root[ver1], root[ver2], 0, n, l, 1, f, v);
    }

    template<class F>
    int diff_min_left(int ver1, int ver2, int r, const F& f) const requires has_inv<policy>{
        ver1--; assert(0 <= ver1 && ver1 <= ver2 && ver2 <= vc);
        assert(0 <= r && r <= n); assert(f(id())); node v = id();
        return bisect_diff(root[ver1], root[ver2], 0, n, r, 0, f, v);
    }
};

template <class policy = group>
class _fw{ // 0-based index
private:
    using node = typename policy::node;
    vector <node> bit; int n;
    node op(const node& l, const node& r) const{ return policy::op(l, r); }
    node id() const{ return node(); }
    node inv(const node& a) const{ return policy::inv(a); }
public:
    _fw(int n = 0){ clear(n); } // O(n)
    _fw(span<const node> arr){ build(arr); } // O(n)
    void clear(int n){ // O(n)
        this->n = n;
        bit.assign(n + 2, id());
    }

    void build(span<const node> arr){ // O(n)
        if(arr.empty()){ clear(0); return; }
        clear((int)arr.size() - 1);
        for(int i = 0;i <= n;i++) bit[i + 1] = arr[i];
        for(int i = 1;i <= n + 1;i++){
            int j = i + (i & -i);
            if(j <= n + 1) bit[j] = op(bit[j], bit[i]);
        }
    }

    // range query -> need inv
    node query(int idx) requires has_inv<policy> { return query(idx, idx); } // O(log n)
    node query(int l, int r) requires has_inv<policy> { // O(log n)
        l = max(l, 0); r = min(r, n);
        if(l > r) return id();
        return op(pre(r), inv(pre(l - 1))); 
    }

    // return 0 ~ idx
    node pre(int idx){ // O(log n)
        node ret = id(); idx = min(idx, n);
        for(int i = idx + 1;i > 0;i -= i & -i) ret = op(ret, bit[i]);
        return ret;
    }

    void set(int idx, const node& v) requires has_inv<policy> { // O(log n)
        if(idx < 0 || idx > n) return;
        node cur = query(idx, idx), d = op(v, inv(cur));         
        update(idx, d);                
    }

    void update(int idx, const node& v){ // O(log n)
        if(idx < 0 || idx > n) return;
        for(int i = idx + 1;i <= n + 1;i += i & -i) bit[i] = op(bit[i], v);
    }
};

template <class policy = group>
class _fw2d{
private:
    using node = typename policy::node;
    vector<vector<node>> bit; int n, m;
    node op(const node& l, const node& r) const { return policy::op(l, r); }
    node id() const { return node(); }
    node inv(const node& a) const { return policy::inv(a); }
public:
    _fw2d(int n = 0, int m = 0){ clear(n, m); } // O(nm)
    void clear(int n, int m){ // O(nm)
        this->n = n; this->m = m;
        bit.assign(n + 2, vector<node>(m + 2, id()));
    }

    // return (0,0) ~ (y,x)
    node pre(int y, int x){ // O(log n log m)
        if(x < 0 || y < 0) return id();
        node ret = id(); x = min(x, m); y = min(y, n);
        for(int i = y + 1; i > 0; i -= i & -i){
            for(int j = x + 1; j > 0; j -= j & -j){
                ret = op(ret, bit[i][j]);
            }
        }
        return ret;
    }

    // range query -> need inv
    node query(int y, int x) requires has_inv<policy> { return query(y, x, y, x); } // O(log n log m)
    node query(int y1, int x1, int y2, int x2) requires has_inv<policy> { // O(log n log m)
        x1 = max(x1, 0); y1 = max(y1, 0);
        x2 = min(x2, m); y2 = min(y2, n);
        if(x1 > x2 || y1 > y2) return id();
        node a = pre(y2, x2), b = pre(y2, x1 - 1);
        node c = pre(y1 - 1, x2), d = pre(y1 - 1, x1 - 1);
        node ab = op(a, inv(b)), cd = op(inv(c), d);
        return op(ab, cd);
    }

    void update(int y, int x, const node& v){ // O(log n log m)
        if(x < 0 || y < 0 || x > m || y > n) return;
        for(int i = y + 1; i <= n + 1; i += i & -i){
            for(int j = x + 1; j <= m + 1; j += j & -j){
                bit[i][j] = op(bit[i][j], v);
            }
        }
    }

    void set(int y, int x, const node& v) requires has_inv<policy> { // O(log n log m)
        if(x < 0 || y < 0 || x > m || y > n) return;
        node cur = query(y, x), d = op(v, inv(cur));
        update(y, x, d);
    }
};
