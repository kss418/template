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

//CCW
namespace _ccw{
    inline i128 cross(pll v1, pll v2, pll v3){ // O(1)
        i128 num = ((i128)v2.x - v1.x) * ((i128)v3.y - v1.y);
        num -= ((i128)v3.x - v1.x) * ((i128)v2.y - v1.y);
        return num;
    }

    inline i128 cross(ll x1, ll y1, ll x2, ll y2, ll x3, ll y3){ // O(1)
        return cross({x1, y1}, {x2, y2}, {x3, y3});
    }

    inline int ret(pll v1, pll v2, pll v3){ // O(1)
        i128 num = cross(v1, v2, v3);
        return (num > 0) - (num < 0);
    }

    inline int ret(ll x1, ll y1, ll x2, ll y2, ll x3, ll y3){ // O(1)
        return ret({x1, y1}, {x2, y2}, {x3, y3});
    }
}

//ANGLE SORT
namespace _angle_sort{
    using i128 = __int128_t;
    template <class T>
    concept point = requires(const T& p){
        requires integral<remove_cvref_t<decltype(p.x)>>;
        requires integral<remove_cvref_t<decltype(p.y)>>;
    };

    inline auto cmp(ll cx = 0, ll cy = 0, bool is_clock = false){ // O(1)
        return [=]<point A = pll, point B = pll>(const A& a, const B& b){
            i128 ax = (i128)a.x - cx, ay = (i128)a.y - cy;
            i128 bx = (i128)b.x - cx, by = (i128)b.y - cy;
            auto half = [=](i128 x, i128 y){
                return ((is_clock ? y < 0 : y > 0) || (y == 0 && x >= 0)) ? 0 : 1;
            };
            int ha = half(ax, ay), hb = half(bx, by);
            if(ha != hb) return ha < hb;
            i128 cross = ax * by - ay * bx;
            if(cross) return is_clock ? cross < 0 : cross > 0;

            i128 da = ax * ax + ay * ay;
            i128 db = bx * bx + by * by;
            return da < db;
        };
    }

    template <ranges::contiguous_range R>
    requires sortable<ranges::iterator_t<R>, decltype(cmp())>
    inline void sort(R&& arr, ll cx = 0, ll cy = 0, bool is_clock = false){ // O(n log n)
        ranges::sort(arr, cmp(cx, cy, is_clock));
    }

    template <point A = pll, point B = pll>
    inline bool is_same(const A& a, const B& b, ll cx = 0, ll cy = 0){ // O(1)
        i128 ax = (i128)a.x - cx, ay = (i128)a.y - cy;
        i128 bx = (i128)b.x - cx, by = (i128)b.y - cy;
        return ax * by - ay * bx == 0 && ax * bx + ay * by > 0;
    }
}

//LINE INTERSECTION
namespace _li{
    inline int ccw(const pll& v1, const pll& v2, const pll& v3){ return _ccw::ret(v1, v2, v3); } // O(1)

    // 점이 선분 위에 있는지 판정 (끝점과 점 선분 포함)
    inline bool on_segment(pll a, pll b, pll p){ // O(1)
        return !ccw(a, b, p) && min(a.x, b.x) <= p.x && p.x <= max(a.x, b.x)
            && min(a.y, b.y) <= p.y && p.y <= max(a.y, b.y);
    }
    inline bool on_segment(pair<pll, pll> a, pll p){ return on_segment(a.x, a.y, p); } // O(1)

    // 두 선분의 교차 판정 (끝점 접촉과 일직선 겹침 포함)
    inline bool intersect(pll av1, pll av2, pll bv1, pll bv2){ // O(1)
        int ac = ccw(av1, av2, bv1) * ccw(av1, av2, bv2);
        int bc = ccw(bv1, bv2, av1) * ccw(bv1, bv2, av2);
        if(!ac && !bc){
            if(av1 > av2) swap(av1, av2);
            if(bv1 > bv2) swap(bv1, bv2);
            return bv1 <= av2 && av1 <= bv2;
        }
        return ac <= 0 && bc <= 0;
    }
    inline bool intersect(pair<pll, pll> a, pair<pll, pll> b){ return intersect(a.x, a.y, b.x, b.y); } // O(1)

    // 두 선분 내부의 비스듬한 교차 판정 (끝점 접촉과 일직선 겹침 제외)
    inline bool proper_intersect(pll av1, pll av2, pll bv1, pll bv2){ // O(1)
        int ac = ccw(av1, av2, bv1) * ccw(av1, av2, bv2);
        int bc = ccw(bv1, bv2, av1) * ccw(bv1, bv2, av2);
        return ac < 0 && bc < 0;
    }
    inline bool proper_intersect(pair<pll, pll> a, pair<pll, pll> b){ return proper_intersect(a.x, a.y, b.x, b.y); } // O(1)
}

template <signed_integral T = i128>
class _frac{
private:
    T p, q;
public:
    _frac(T p = 0, T q = 1){ set(p, q); }
    void set(T p, T q){
        assert(q);
        if(q < 0){ p = -p; q = -q; }
        if(!p){ this->p = 0; this->q = 1; return; }
        T g = gcd(p < 0 ? -p : p, q);
        this->p = p / g; this->q = q / g;
    }

    T num() const{ return p; }
    T den() const{ return q; }
    ld to_ld() const{ return (ld)p / (ld)q; }
    T floor() const{ return p / q - (p % q < 0); }
    T ceil() const{ return p / q + (p % q > 0); }

    _frac operator-() const{ return _frac(-p, q); }
    _frac inv() const{ assert(p != 0); return _frac(q, p); }

    _frac& operator+=(const _frac& a){
        T g = gcd(q, a.q);
        T num = p * (a.q / g) + a.p * (q / g);
        T h = gcd(num < 0 ? -num : num, g);
        T den = (q / g) * (a.q / h);
        p = num / h; q = den;
        return *this;
    }

    _frac& operator-=(const _frac& a){
        T g = gcd(q, a.q);
        T num = p * (a.q / g) - a.p * (q / g);
        T h = gcd(num < 0 ? -num : num, g);
        T den = (q / g) * (a.q / h);
        p = num / h; q = den;
        return *this;
    }

    _frac& operator*=(const _frac& a){
        T g = gcd(p < 0 ? -p : p, a.q), h = gcd(a.p < 0 ? -a.p : a.p, q);
        T num = (p / g) * (a.p / h), den = (q / h) * (a.q / g);
        p = num; q = den;
        return *this;
    }

    _frac& operator/=(const _frac& a){
        assert(a.p != 0);
        T g = gcd(p < 0 ? -p : p, a.p < 0 ? -a.p : a.p), h = gcd(q, a.q);
        T num = (p / g) * (a.q / h), den = (q / h) * (a.p / g);
        if(den < 0){ num = -num; den = -den; }
        p = num; q = den;
        return *this;
    }

    friend bool operator==(const _frac& a, const _frac& b) = default;
    friend bool operator==(const _frac& a, T b){ return a.q == 1 && a.p == b; }
    friend auto operator<=>(const _frac& a, const _frac& b){ return a.p * b.q <=> b.p * a.q; }

    friend _frac operator+(_frac a, const _frac& b){ return a += b; }
    friend _frac operator-(_frac a, const _frac& b){ return a -= b; }
    friend _frac operator*(_frac a, const _frac& b){ return a *= b; }
    friend _frac operator/(_frac a, const _frac& b){ return a /= b; }
};

//CONVEX HULL
class _ch{
public:
    _ch() {}
    vector <ll> num; vector <pll> p; ll cnt = 0;

    class node{
    public:
        ll x, y, idx;
        node(pll a, ll idx) : node(a.x, a.y, idx) {};
        node(ll x, ll y, ll idx){
            this->x = x; this->y = y;
            this->idx = idx;
        }
        operator pll() const{ return { x, y }; }
    };
    vector <node> arr, st;

    ll dist(pll v1, pll v2){
        ll dx = v1.x - v2.x, dy = v1.y - v2.y;
        return dx * dx + dy * dy;
    }

    ll ccw(pll v1, pll v2, pll v3){ return _ccw::ret(v1, v2, v3); } // O(1)

    void add(ll a, ll b){ add({a, b}); }
    void add(pll a){ arr.push_back({a, cnt++}); }

    void build(){ // O(n log n)
        st.clear(); p.clear();
        auto v = arr;
        sort(all(v), [&](const node& a, const node& b){
            return tie(a.x, a.y, a.idx) < tie(b.x, b.y, b.idx);
        });
        v.erase(unique(all(v), [&](const node& a, const node& b){
            return a.x == b.x && a.y == b.y;
        }), v.end());
        if(v.empty()) return;
        for(const auto& i : v){
            while(st.size() >= 2 && ccw(st[st.size() - 2], st.back(), i) <= 0) st.pop_back();
            st.push_back(i);
        }
        ll sz = st.size();
        for(ll i = (ll)v.size() - 2;i >= 0;i--){
            while(st.size() > sz && ccw(st[st.size() - 2], st.back(), v[i]) <= 0) st.pop_back();
            st.push_back(v[i]);
        }
        if(st.size() > 1) st.pop_back();
        for(const auto& i : st) p.push_back(i);
    }

    const vector <pll>& ret() const{ return p; } // O(1)
    const vector <node>& idx() const{ return st; } // O(1)
};

template <u64 mod = 998244353>
class _mod_matrix{ // 0-based index, Require _mint
public:
    using mint = _mint<mod>;
    int n, m; vector <mint> arr;
    _mod_matrix() : n(0), m(0){}
    _mod_matrix(int n, int m) : n(n), m(m), arr(1LL * n * m){}

    mint& at(int i, int j){ return arr[1LL * i * m + j]; }
    const mint& at(int i, int j) const{ return arr[1LL * i * m + j]; }
    void add(int i, int j, ll v){ at(i, j) += v; }
    void set(int i, int j, ll v){ at(i, j) = v; }

    static _mod_matrix id(int n){
        _mod_matrix ret(n, n);
        for(int i = 0; i < n; i++) ret.at(i, i) = 1;
        return ret;
    }

    _mod_matrix operator*(const _mod_matrix& ot) const{
        assert(m == ot.n);
        _mod_matrix ret(n, ot.m);
        for(int i = 0; i < n; i++){
            for(int k = 0; k < m; k++){
                const mint v = at(i, k);
                if(!v.v) continue;
                for(int j = 0; j < ot.m; j++) ret.at(i, j) += v * ot.at(k, j);
            }
        }
        return ret;
    }

    _mod_matrix operator+(const _mod_matrix& ot) const{
        assert(n == ot.n && m == ot.m);
        _mod_matrix ret(n, m);
        for(int i = 0; i < n * m; i++) ret.arr[i] = arr[i] + ot.arr[i];
        return ret;
    }

    _mod_matrix pow(ll k) const{ // n*n matrix
        assert(n == m); assert(k >= 0);
        _mod_matrix base = *this, ret = id(n);
        while(k > 0){
            if(k & 1) ret = ret * base;
            base = base * base;
            k >>= 1LL;
        }
        return ret;
    }

    _mod_matrix transpose() const{
        _mod_matrix ret(m, n);
        for(int i = 0;i < n;i++) for(int j = 0;j < m;j++) ret.at(j, i) = at(i, j);
        return ret;
    }

    friend ostream& operator<<(ostream& out, const _mod_matrix& cur){
        for(int i = 0; i < cur.n; i++){
            for(int j = 0; j < cur.m; j++) out << cur.at(i, j) << " ";
            out << "\n";
        }
        return out;
    }
};

//AREA
class _area{
public:
    _area() {}

    ll ret(span<const pll> v){ // div by 2
        i128 num = 0;
        for(int i = 0;i < v.size();i++){
            auto [x1, y1] = v[i];
            auto [x2, y2] = v[(i + 1) % v.size()];
            num += (i128)x1 * y2 - (i128)x2 * y1;
        }

        return (ll)abs(num);
    }
};

//LINE
class _line{ 
public:
    _frac<> a, b; i128 x = 0; bool vert = false;
    _line() = default;
    _line(const pll& p1, const pll& p2){
        if(p1.x == p2.x){
            vert = true; x = p1.x;
            a = _frac<>(), b = _frac<>();
        }
        else{
            vert = false;
            ll dx = p2.x - p1.x, dy = p2.y - p1.y;
            a = _frac<>(dy, dx), b = _frac<>(p1.y) - a * _frac<>(p1.x);
        }
    }

    _line(const _frac<>& slope, const pll& p){
        vert = false; x = 0;
        a = slope; b = _frac<>(p.y) - a * _frac<>(p.x);
    }

    _frac<> f(const _frac<>& x) const{
        assert(!vert);
        return a * x + b;
    }

    bool intersect(const _line& ot, pair<_frac<>, _frac<>>& ret) const{
        if(vert && ot.vert) return false;
        if(vert){
            _frac<> p1 = _frac<>(x), p2 = ot.f(p1); ret = {p1, p2};
            return true;
        }
        if(ot.vert){
            _frac<> p1 = _frac<>(ot.x), p2 = f(p1); ret = {p1, p2};
            return true;
        }
        if(a == ot.a) return false;
        _frac<> p1 = (ot.b - b) / (a - ot.a), p2 = f(p1); ret = {p1, p2};
        return true;
    }

    bool same(const _line& ot) const{
        if(vert != ot.vert) return false;
        if(vert) return x == ot.x;
        return a == ot.a && b == ot.b;
    }
};

// 점 다각형 내부 판별
class _inter{ // Require line_intersection
public:
    static bool ret(span<const pll> p, pll now){
        ll cnt = 0, flag = 0, sz = p.size();
        if(sz < 3) return 0;
        pll nxt = {1e12, now.y + 1};

        for(int i = 0;i < sz;i++){
            cnt += _li::intersect({p[i], p[(i + 1) % sz]}, {now, nxt});
        }

        for(int i = 0;i < sz;i++){
            flag += _li::on_segment(p[i], p[(i + 1) % sz], now);
        }

        return (cnt % 2 | flag);
    }
}; 

//ROTATING CALIPUS
class _rc{
public:
    using node = _ch::node;
private:
    vector <node> p;
    pair <pll, pll> result{}; pll ids = {-1, -1};
    ll mx = 0; ld len = 0;

    i128 area(ll a, ll b, ll c) const{
        i128 ret = ((i128)p[b].x - p[a].x) * ((i128)p[c].y - p[a].y);
        ret -= ((i128)p[c].x - p[a].x) * ((i128)p[b].y - p[a].y);
        return ret < 0 ? -ret : ret;
    }

    void update(ll l, ll r){
        ll dx = p[l].x - p[r].x, dy = p[l].y - p[r].y;
        ll now = dx * dx + dy * dy;
        if(now <= mx) return;
        mx = now; result = {pll(p[l]), pll(p[r])}; ids = {p[l].idx, p[r].idx};
    }
public:
    void build(span<const node> arr){ p.assign(all(arr)); build(); } // O(n)
    void build(span<const pll> arr){ // O(n)
        p.clear(); p.reserve(arr.size());
        for(ll i = 0;i < arr.size();i++) p.push_back({arr[i], i});
        build();
    }

    void build(){ // O(n)
        vector <node> v;
        for(const auto& i : p){
            if(v.empty() || v.back().x != i.x || v.back().y != i.y) v.push_back(i);
        }
        if(v.size() > 1 && v.front().x == v.back().x && v.front().y == v.back().y) v.pop_back();
        p.swap(v);
        mx = 0; len = 0; result = {}; ids = {-1, -1};
        ll sz = p.size(); if(!sz) return;
        result = {pll(p[0]), pll(p[0])}; ids = {p[0].idx, p[0].idx};
        if(sz == 1) return;
        bool line = 1;
        for(ll i = 2;i < sz;i++) if(area(0, 1, i)){ line = 0; break; }
        if(line){
            ll l = 0, r = 0;
            for(ll i = 1;i < sz;i++){
                if(tie(p[i].x, p[i].y) < tie(p[l].x, p[l].y)) l = i;
                if(tie(p[i].x, p[i].y) > tie(p[r].x, p[r].y)) r = i;
            }
            update(l, r);
        }
        else{
            ll r = 1;
            for(ll l = 0;l < sz;l++){
                ll nxt = (l + 1) % sz;
                r = max(r, l + 1);
                while(r + 1 < l + sz && area(l, nxt, (r + 1) % sz) >= area(l, nxt, r % sz)){
                    update(l, r % sz); update(nxt, r % sz); r++;
                }
                update(l, r % sz); update(nxt, r % sz);
            }
        }
        len = sqrtl(mx);
    }

    const pair <pll, pll>& ret() const{ return result; } // O(1)
    const pll& idx() const{ return ids; } // O(1)
    ll dist2() const{ return mx; } // O(1)
    ld dist() const{ return len; } // O(1)
};

class _dot{
public:
    _dot() {} 
    // ret > 0 -> angle < 90, ret < 0 -> angle > 90, ret == 0 -> angle == 90
    ll ret(ll x1, ll y1, ll x2, ll y2, ll x3, ll y3){
        return ret({x1, y1}, {x2, y2}, {x3, y3});
    }
    ll ret(pll l, pll m, pll r){
        pll a = {m.x - l.x, m.y - l.y};
        pll b = {m.x - r.x, m.y - r.y};
        return a.x * b.x + a.y * b.y;
    }
};

template <size_t m>
class _gf2{ // 0-based index
public:
    enum class state{ inconsistent, unique, multiple };
    class result{
    public:
        state status;
        bitset <m> value; ll rk, fr;
        bool inconsistent() const{ return status == state::inconsistent; } // O(1)
        bool unique() const{ return status == state::unique; } // O(1)
        bool multiple() const{ return status == state::multiple; } // O(1)
        const bitset <m>& get_value() const{ return value; } // O(1)
        ll rank() const{ return rk; } // O(1)
        ll free_count() const{ return fr; } // O(1)
    };

    vector <bitset <m + 1>> bs;
private:
    result ans{};
public:
    _gf2(ll n = 0) : bs(n){}
    void set_a(ll cy, ll cx, bool v){ bs[cy][cx] = v; }
    void set_b(ll cy, bool v){ bs[cy][m] = v; }
    void clear(){ for(auto& i : bs) i.reset(); }

    void build(){
        ll n = bs.size();
        auto arr = bs; vector <ll> pos(m, -1);
        ll cy = 0;
        for(ll cx = 0;cx < m && cy < n;cx++){
            ll now = -1;
            for(ll i = cy;i < n;i++){
                if(!arr[i][cx]) continue;
                now = i; break;
            }
            if(now == -1) continue;

            swap(arr[now], arr[cy]);
            pos[cx] = cy;
            for(ll i = 0;i < n;i++){
                if(i == cy || !arr[i][cx]) continue;
                arr[i] ^= arr[cy];
            }
            cy++;
        }

        ans = {cy == m ? state::unique : state::multiple, {}, cy, (ll)m - cy};
        for(ll i = cy;i < n;i++){
            if(!arr[i][m]) continue;
            ans.status = state::inconsistent; return;
        }
        for(ll i = 0;i < m;i++) if(pos[i] != -1) ans.value[i] = arr[pos[i]][m];
    }

    const result& ret() const{ return ans; } // O(1)
};

template <size_t N = 63>
class _xor_basis{
public:
    array<u64, N> basis{}; int rk = 0;
    _xor_basis(){ clear(); } // O(max_bit)
    void clear(){ basis.fill(0); rk = 0; } // O(max_bit)

    bool insert(u64 x){ // O(max_bit)
        for(int i = (int)N - 1; i >= 0; i--){
            if(((x >> i) & 1ull) == 0) continue;
            if(!basis[i]){
                basis[i] = x; rk++;
                return 1;
            }
            x ^= basis[i];
        }
        return 0;
    }

    bool can_make(u64 x) const{ // O(max_bit)
        for(int i = (int)N - 1; i >= 0; i--){
            if(((x >> i) & 1ull) == 0) continue;
            if(!basis[i]) return 0;
            x ^= basis[i];
        }
        return 1;
    }

    u64 max_xor(u64 x = 0) const{ // O(max_bit)
        u64 ret = x;
        for(int i = (int)N - 1; i >= 0; i--) ret = max(ret, ret ^ basis[i]);
        return ret;
    }

    u64 min_xor(u64 x = 0) const{ // O(max_bit)
        u64 ret = x;
        for(int i = (int)N - 1; i >= 0; i--) ret = min(ret, ret ^ basis[i]);
        return ret;
    }

    int rank() const{ return rk; } // O(1)
};
