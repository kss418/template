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

class _uf{
private:
    bool base;
    vector<int> p; int n, cc;
protected:
    bool merge_internal(int& a, int& b){ // O(alpha(n)) amortized
        a = find(a); b = find(b);
        if(a == b) return false;
        if(p[a] > p[b]) swap(a, b);
        p[a] += p[b]; p[b] = a; cc--;
        return true;
    }
public:
    _uf(int n = 0, bool base = 1){ clear(n, base); } // O(n)
    void clear(int n, bool base = 1){ // O(n)
        this->n = n; this->base = base; cc = n - base + 1;
        p.assign(n + 1, -1);
    }

    int find(int x){ // O(alpha(n)) amortized
        if(p[x] < 0) return x;
        return p[x] = find(p[x]);
    }

    bool merge(int a, int b){ return merge_internal(a, b); } // O(alpha(n)) amortized
    int count() const{ return cc; } // O(1)
    int size(int x){ return -p[find(x)]; } // O(alpha(n)) amortized
    bool root(int x) const{ return p[x] < 0; } // O(1)
    bool same(int a, int b){ return find(a) == find(b); } // O(alpha(n)) amortized

    template<class F>
    void it_root(const F& f) const{ // O(n)
        for(int i = base;i <= n;i++) if(root(i)) f(i);
    }

    vector<vector<int>> groups(){ // O(n * alpha(n))
        vector<int> id(n + 1, -1);
        vector<vector<int>> ret; ret.reserve(cc);
        it_root([&](int r){
            id[r] = (int)ret.size(); ret.emplace_back();
            ret.back().reserve(-p[r]);
        });
        for(int i = base;i <= n;i++) ret[id[find(i)]].push_back(i);
        return ret;
    }
};

class uf_policy{
public:
    class node{};
    static node leaf(){ return {}; }
    static void merge(node& a, node& b){}
};

template <class policy = uf_policy>
class _uf_value : public _uf{
public:
    using node = typename policy::node;
private:
    bool base; vector<node> arr;
    node leaf() const{ return policy::leaf(); }
public:
    _uf_value(int n = 0, bool base = 1) : _uf(n, base), base(base), arr(n + 1, leaf()){} // O(n)
    void clear(int n, bool base = 1){ // O(n)
        _uf::clear(n, base); this->base = base;
        arr.assign(n + 1, leaf());
    }

    void make(span<const node> v){ // O(n)
        vector<node> tmp(v.size(), leaf());
        for(int i = base;i < (int)v.size();i++) tmp[i] = v[i];
        _uf::clear(v.empty() ? base - 1 : (int)v.size() - 1, base);
        arr.swap(tmp);
    }

    bool merge(int a, int b){ // O(alpha(n)) amortized + policy::merge
        if(!merge_internal(a, b)) return false;
        policy::merge(arr[a], arr[b]);
        return true;
    }

    node& get(int x){ return arr[find(x)]; } // O(alpha(n)) amortized
    void set(int x, const node& v){ arr[find(x)] = v; } // O(alpha(n)) amortized

    template<class F>
    void it_component(const F& f){ // O(n)
        it_root([&](int root){ f(root, arr[root]); });
    }
};

// WEIGHTED UNION FIND
class _wuf{
private:
    bool base;
    vector<int> p;
    vector<ll> d;
    vector<char> dis; int n, cc;
public:
    enum class state{ disconnected, inconsistent, known };
    class result{
    public:
        state status;
        ll value;
        bool disconnected() const{ return status == state::disconnected; } // O(1)
        bool inconsistent() const{ return status == state::inconsistent; } // O(1)
        ll get_value() const{ return value; } // O(1)
    };

    _wuf(int n = 0, bool base = 1){ clear(n, base); } // O(n)
    void clear(int n, bool base = 1){ // O(n)
        this->n = n; this->base = base; cc = n - base + 1;
        p.assign(n + 1, -1); d.assign(n + 1, 0); dis.assign(n + 1, 0);
    }

    pair<int, ll> find(int num){ // O(alpha(n)) amortized
        if(p[num] < 0) return {num, 0ll};
        auto [pa, diff] = find(p[num]);
        p[num] = pa; d[num] += diff;
        return {p[num], d[num]};
    }

    void merge(int a, int b, ll c){ // O(alpha(n)) amortized
        auto [pa, da] = find(a);
        auto [pb, db] = find(b);
        if(pa == pb){ dis[pb] |= ((i128)db - da != c); return; }
        i128 delta = (i128)db - da - c;
        if(p[pa] < p[pb]){ swap(pa, pb); delta = -delta; }
        p[pb] += p[pa]; p[pa] = pb; d[pa] = (ll)delta;
        dis[pb] |= dis[pa]; cc--;
    }

    result diff(int a, int b){ // O(alpha(n)) amortized
        auto [pa, da] = find(a);
        auto [pb, db] = find(b);
        if(pa != pb) return {state::disconnected, 0};
        if(dis[pa]) return {state::inconsistent, 0};
        return {state::known, db - da};
    }

    bool same(int a, int b){ return find(a).x == find(b).x; } // O(alpha(n)) amortized
    int count() const{ return cc; } // O(1)
    int size(int x){ return -p[find(x).x]; } // O(alpha(n)) amortized
    bool root(int x) const{ return p[x] < 0; } // O(1)

    template<class F>
    void it_root(const F& f) const{ // O(n)
        for(int i = base;i <= n;i++) if(root(i)) f(i);
    }

    vector<vector<int>> groups(){ // O(n * alpha(n))
        vector<int> id(n + 1, -1);
        vector<vector<int>> ret; ret.reserve(cc);
        it_root([&](int r){
            id[r] = (int)ret.size(); ret.emplace_back();
            ret.back().reserve(-p[r]);
        });
        for(int i = base;i <= n;i++) ret[id[find(i).x]].push_back(i);
        return ret;
    }
};

class _ufset{
private:
    bool base;
    vector <int> p, l, r; int n, cc;
    int find_internal(int num){ // O(alpha(n)) amortized
        if(p[num] < 0) return num;
        return p[num] = find_internal(p[num]);
    }

    bool merge(int a, int b){ // O(alpha(n)) amortized
        a = find_internal(a + 1); b = find_internal(b + 1);
        if (a == b) return false;
        if (p[a] > p[b]) swap(a, b);
        p[a] += p[b]; p[b] = a; cc--;
        l[a] = min(l[a], l[b]);
        r[a] = max(r[a], r[b]);
        return true;
    }
public:
    _ufset(int n = 0, bool base = 1){ clear(n, base); } // O(n)
    void clear(int n, bool base = 1){ // O(n)
        this->n = n; this->base = base; cc = n - base + 1;
        p.assign(n + 3, -1);
        l.resize(n + 3); r.resize(n + 3);
        iota(all(l), 0); iota(all(r), 0);
    }

    int find(int num){ // O(alpha(n)) amortized
        return find_internal(num + 1) - 1;
    }

    bool merge_prv(int x){ // O(alpha(n)) amortized
        int v = prv(x);
        if(v < base) return false;
        return merge(x, v);
    }

    bool merge_nxt(int x){ // O(alpha(n)) amortized
        int v = nxt(x);
        if(v > n) return false;
        return merge(x, v);
    }

    int count() const{ return cc; } // O(1)
    int size(int x){ return -p[find_internal(x + 1)]; } // O(alpha(n)) amortized
    bool root(int x) const{ return p[x + 1] < 0; } // O(1)
    bool same(int a, int b) { return find(a) == find(b); } // O(alpha(n)) amortized
    int left(int x){ return l[find_internal(x + 1)] - 1; } // O(alpha(n)) amortized
    int right(int x){ return r[find_internal(x + 1)] - 1; } // O(alpha(n)) amortized
    int prv(int x){ return find(left(x) - 1); } // O(alpha(n)) amortized
    int nxt(int x){ return find(right(x) + 1); } // O(alpha(n)) amortized

    template<class F>
    void it_root(const F& f) const{ // O(n)
        for(int i = base;i <= n;i++) if(root(i)) f(i);
    }

    vector<vector<int>> groups(){ // O(n * alpha(n))
        vector<int> id(n + 1, -1);
        vector<vector<int>> ret; ret.reserve(cc);
        it_root([&](int root){
            id[root] = (int)ret.size(); ret.emplace_back();
            ret.back().reserve(-p[root + 1]);
        });
        for(int i = base;i <= n;i++) ret[id[find(i)]].push_back(i);
        return ret;
    }
};
