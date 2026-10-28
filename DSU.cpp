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
    bool base;
private:
    vector<int> p, sz; int n, cc;
protected:
    bool merge_internal(int& a, int& b){ // O(1)
        a = find(a); b = find(b);
        if(a == b) return false;
        if(sz[a] < sz[b]) swap(a, b);
        p[b] = a; sz[a] += sz[b]; cc--;
        return true;
    }
public:
    _uf(int n = 0, bool base = 1){ clear(n, base); } // O(n)
    void clear(int n, bool base = 1){ // O(n)
        this->n = n; this->base = base; cc = n - base + 1;
        p.assign(n + 1, -1); sz.assign(n + 1, 1);
    }

    int find(int x){ // O(1)
        if(p[x] == -1) return x;
        return p[x] = find(p[x]);
    }

    bool merge(int a, int b){ return merge_internal(a, b); } // O(1)
    int count() const{ return cc; } // O(1)
    int size(int x){ return sz[find(x)]; } // O(1)
    bool root(int x){ return find(x) == x; } // O(1)
    bool same(int a, int b){ return find(a) == find(b); } // O(1)
};

class uf_policy{
public:
    class node{};
    static node leaf(){ return {}; }
    static void merge(node& a, node& b){}
};

template <class policy = uf_policy>
class _uf_value : public _uf{
private:
    using node = typename policy::node;
    vector<node> arr;
    node leaf() const{ return policy::leaf(); }
public:
    _uf_value(int n = 0, bool base = 1) : _uf(n, base), arr(n + 1, leaf()){} // O(n)
    void clear(int n, bool base = 1){ // O(n)
        _uf::clear(n, base);
        arr.assign(n + 1, leaf());
    }

    bool merge(int a, int b){ // O(1) + policy::merge
        if(!merge_internal(a, b)) return false;
        policy::merge(arr[a], arr[b]);
        return true;
    }

    node& get(int x){ return arr[find(x)]; } // O(1)
    void set(int x, const node& v){ arr[find(x)] = v; } // O(1)
};

// WEIGHTED UNION FIND
class _wuf{
    bool base;
private:
    vector<ll> sz;
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

    vector<ll> p, d, dis; ll n;
    _wuf(ll n = 0, bool base = 1){ clear(n, base); } // O(n)
    void clear(ll n, bool base = 1){ // O(n)
        this->n = n; this->base = base; dis.assign(n + 1, 0);
        p.assign(n + 1, -1); d.assign(n + 1, 0); sz.assign(n + 1, 1);
    }

    pll find(ll num){ // O(alpha(n))
        if(p[num] == -1) return {num, 0ll};
        auto [pa, diff] = find(p[num]);
        p[num] = pa; d[num] += diff;
        return {p[num], d[num]};
    }

    void merge(ll a, ll b, ll c){ // O(alpha(n))
        auto [pa, da] = find(a);
        auto [pb, db] = find(b);
        if(pa == pb){ dis[pb] |= (db - da != c); return; }
        ll delta = db - da - c;
        if(sz[pa] > sz[pb]){ swap(pa, pb); delta = -delta; }
        p[pa] = pb; d[pa] = delta;
        sz[pb] += sz[pa]; dis[pb] |= dis[pa];
    }

    result diff(ll a, ll b){ // O(alpha(n))
        auto [pa, da] = find(a);
        auto [pb, db] = find(b);
        if(pa != pb) return {state::disconnected, 0};
        if(dis[pa]) return {state::inconsistent, 0};
        return {state::known, db - da};
    }

    bool same(ll a, ll b){ return find(a).x == find(b).x; } // O(alpha(n))
};

class _ufset{
    bool base;
private:
    vector <int> p, sz, l, r; int n, cc;
    int find_internal(int num){
        if(p[num] == -1) return num;
        return p[num] = find_internal(p[num]);
    }
public:
    _ufset(int n = 0, bool base = 1){ clear(n, base); } // O(n)
    void clear(int n, bool base = 1){ // O(n)
        this->n = n; this->base = base; cc = n - base + 1;
        p.assign(n + 3, -1); sz.assign(n + 3, 1);
        l.resize(n + 3); r.resize(n + 3);
        iota(all(l), 0); iota(all(r), 0);
    }

    int find(int num){ // O(1)
        assert(base - 1 <= num && num <= n + 1);
        return find_internal(num + 1) - 1;
    }

    bool merge(int a, int b){ // O(1)
        assert(base <= a && a <= n && base <= b && b <= n);
        a = find_internal(a + 1); b = find_internal(b + 1);
        if (a == b) return false; 
        if (sz[a] < sz[b]) swap(a, b);
        p[b] = a; sz[a] += sz[b]; cc--;
        l[a] = min(l[a], l[b]);
        r[a] = max(r[a], r[b]);
        return true;
    }

    int count() const{ return cc; } // O(1)
    int size(int x){ return sz[find_internal(x + 1)]; } // O(1)
    bool root(int x){ return find(x) == x; } // O(1)
    bool same(int a, int b) { return find(a) == find(b); } // O(1)
    int prv(int x){ return find_internal(l[find_internal(x + 1)] - 1) - 1; } // O(1)
    int nxt(int x){ return find_internal(r[find_internal(x + 1)] + 1) - 1; } // O(1)
};
