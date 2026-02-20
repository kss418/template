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
class _ccw{
public:
    _ccw() {}

    ll ret(ll x1, ll y1, ll x2, ll y2, ll x3, ll y3){
        return ret({x1, y1}, {x2, y2}, {x3, y3});
    }
    ll ret(pll v1, pll v2, pll v3){
        ll num = (v2.x - v1.x) * (v3.y - v1.y);
        num -= (v3.x - v1.x) * (v2.y - v1.y);

        if(num > 0) return 1; 
        else if(num < 0) return -1; 
        return 0;
    }
};

//LINE INTERSECTION
class _li{
public:
    static int ccw(const pll& v1, const pll& v2, const pll& v3){ // O(1)
        i128 x1 = (i128)v2.x - v1.x, y1 = (i128)v2.y - v1.y;
        i128 x2 = (i128)v3.x - v1.x, y2 = (i128)v3.y - v1.y;
        i128 cross = x1 * y2 - x2 * y1;
        if(cross > 0) return 1; 
        else if(cross < 0) return -1; 
        return 0;
    }

    static bool intersect(pair<pll, pll> a, pair<pll, pll> b){ // O(1)
        return intersect(a.x, a.y, b.x, b.y); 
    } 
    static bool intersect(pll av1, pll av2, pll bv1, pll bv2){ // O(1)
        int ac = ccw(av1, av2, bv1) * ccw(av1, av2, bv2);
        int bc = ccw(bv1, bv2, av1) * ccw(bv1, bv2, av2);
        if(!ac && !bc){
            if(av1 > av2) swap(av1, av2);
            if(bv1 > bv2) swap(bv1, bv2);
            return bv1 <= av2 && av1 <= bv2;
        }
        return ac <= 0 && bc <= 0;
    }
};

class _frac{
public:
    i128 p, q;
    _frac(i128 p = 0, i128 q = 1){ set(p, q); }
    void set(i128 p, i128 q){
        assert(q);
        if(q < 0){ p = -p; q = -q; }
        if(!p){ this->p = 0; this->q = 1; return; }
        i128 g = gcd(p < 0 ? -p : p, q);
        this->p = p / g; this->q = q / g;
    }

    ld to_ld() const{ return (ld)p / (ld)q; }
    i128 floor() const{
        if(p >= 0) return p / q;
        return -((-p + q - 1) / q);
    }
    i128 ceil() const{
        if(p >= 0) return (p + q - 1) / q;
        return -((-p) / q);
    }

    _frac operator-() const{ return _frac(-p, q); }
    _frac inv() const{ assert(p != 0); return _frac(q, p); }

    friend bool operator==(const _frac& a, i128 b){
        assert(a.q); i128 p = a.p, q = a.q;
        if(q < 0){ p = -p; q = -q; }      
        if(p % q) return false;     
        return (p / q) == b;
    }
    friend bool operator==(i128 a, const _frac& b){ return b == a; }
    friend bool operator!=(const _frac& a, i128 b){ return !(a == b); }
    friend bool operator!=(i128 a, const _frac& b){ return !(a == b); }

    friend bool operator==(const _frac& a, const _frac& b){ return a.p == b.p && a.q == b.q; }
    friend bool operator!=(const _frac& a, const _frac& b){ return !(a == b); }

    friend bool operator<(const _frac& a, const _frac& b){ return a.p * b.q < b.p * a.q; }
    friend bool operator>(const _frac& a, const _frac& b){ return b < a; }
    friend bool operator<=(const _frac& a, const _frac& b){ return !(b < a); }
    friend bool operator>=(const _frac& a, const _frac& b){ return !(a < b); }

    friend _frac operator+(const _frac& a, const _frac& b){ return _frac(a.p * b.q + b.p * a.q, a.q * b.q); }
    friend _frac operator-(const _frac& a, const _frac& b){ return _frac(a.p * b.q - b.p * a.q, a.q * b.q); }
    friend _frac operator*(const _frac& a, const _frac& b){ return _frac(a.p * b.p, a.q * b.q); }
    friend _frac operator/(const _frac& a, const _frac& b){ assert(b.p != 0); return _frac(a.p * b.q, a.q * b.p); }
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
        operator pll(){ return { x, y }; }
    };
    vector <node> arr, st;

    ll dist(pll v1, pll v2){
        ll dx = v1.x - v2.x, dy = v1.y - v2.y;
        return dx * dx + dy * dy;
    }

    ll ccw(pll v1, pll v2, pll v3){
        ll num = (v2.x - v1.x) * (v3.y - v1.y);
        num -= (v3.x - v1.x) * (v2.y - v1.y);

        if(num > 0) return 1;
        else if(num < 0) return -1;
        return 0;
    }
    
    void add(ll a, ll b) { add({a, b}); }
    void add(pll a){ arr.push_back({a, cnt++}); }

    void init(){    
        swap(arr[0], *min_element(all(arr), [&](pll a, pll b){
            if(a.x != b.x) return a.x < b.x;
            return a.y < b.y;
        }));

        sort(arr.begin() + 1, arr.end(), [&](pll a, pll b){
            ll dir = ccw(arr[0], {a.x, a.y}, {b.x, b.y});
            if(dir) return dir > 0;
            return dist(arr[0], {a.x, a.y}) < dist(arr[0], {b.x, b.y});
        });

        for(auto& i : arr){
            while(st.size() >= 2 && ccw(st[st.size() - 2], st.back(), i) <= 0) st.pop_back();
            st.push_back(i);
        }
        for(auto& i : st) p.push_back(i);
    }

    vector <pll> ret(){ return p; } // 좌표 반환
    vector <node> idx(){ return st; } // 좌표 + 인덱스 반환
};

class _mod_matrix{ // 0-based index, Require _mint
public:
    int n, m; ll mod; vector<_mint> arr;
    _mod_matrix() : n(0), m(0), mod(1){}
    _mod_matrix(int n, int m, ll mod = 998244353)
        : n(n), m(m), mod(mod), arr(1LL * n * m, _mint(0, mod)) {}

    _mint& at(int i, int j){ return arr[1LL * i * m + j]; }
    const _mint& at(int i, int j) const{ return arr[1LL * i * m + j]; }
    void add(int i, int j, ll v){ at(i, j) += v; }
    void set(int i, int j, ll v){ at(i, j) = _mint(v, mod); }

    static _mod_matrix id(int n, ll mod){
        _mod_matrix ret(n, n, mod);
        for(int i = 0; i < n; i++) ret.at(i, i) = _mint(1, mod);
        return ret;
    }

    _mod_matrix operator*(const _mod_matrix& ot) const{
        assert(m == ot.n);
        assert(mod == ot.mod);
        _mod_matrix ret(n, ot.m, mod);
        for(int i = 0; i < n; i++){
            for(int k = 0; k < m; k++){
                const _mint v = at(i, k);
                if(!v.v) continue;
                for(int j = 0; j < ot.m; j++) ret.at(i, j) += v * ot.at(k, j);
            }
        }
        return ret;
    }

    _mod_matrix operator+(const _mod_matrix& ot) const{
        assert(n == ot.n && m == ot.m);
        _mod_matrix ret(n, m, mod);
        for(int i = 0; i < n * m; i++) ret.arr[i] = arr[i] + ot.arr[i];
        return ret;
    }

    _mod_matrix pow(ll k) const{ // n*n matrix
        assert(n == m); assert(k >= 0);
        _mod_matrix base = *this, ret = id(n, mod);
        while(k > 0){
            if(k & 1) ret = ret * base;
            base = base * base;
            k >>= 1LL;
        }
        return ret;
    }

    _mod_matrix transpose() const{
        _mod_matrix ret(m, n, mod);
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
        ll num = 0;
        for(int i = 0;i < v.size();i++){
            auto [x1, y1] = v[i];
            auto [x2, y2] = v[(i + 1) % v.size()];
            num += x1 * y2 - x2 * y1;
        }

        return abs(num);
    }
};

//LINE
class _line{ 
public:
    _frac a, b; i128 x = 0; bool vert = false;
    _line() = default;
    _line(const pll& p1, const pll& p2){
        if(p1.x == p2.x){
            vert = true; x = p1.x;
            a = _frac(), b = _frac();
        }
        else{
            vert = false;
            ll dx = p2.x - p1.x, dy = p2.y - p1.y;
            a = _frac(dy, dx), b = _frac(p1.y) - a * _frac(p1.x);
        }
    }

    _line(const _frac& slope, const pll& p){
        vert = false; x = 0;
        a = slope; b = _frac(p.y) - a * _frac(p.x);
    }

    _frac f(const _frac& x) const{
        assert(!vert);
        return a * x + b;
    }

    bool intersect(const _line& ot, pair<_frac, _frac>& ret) const{
        if(vert && ot.vert) return false;
        if(vert){
            _frac p1 = _frac(x), p2 = ot.f(p1); ret = {p1, p2};
            return true;
        }
        if(ot.vert){
            _frac p1 = _frac(ot.x), p2 = f(p1); ret = {p1, p2};
            return true;
        }
        if(a == ot.a) return false;
        _frac p1 = (ot.b - b) / (a - ot.a), p2 = f(p1); ret = {p1, p2};
        return true;
    }

    bool same(const _line& ot) const{
        if(vert && ot.vert) return x == ot.x;
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
            flag += _li::intersect({p[i], p[(i + 1) % sz]}, {now, now});
        }

        return (cnt % 2 | flag);
    }
}; 

//ROTATING CALIPUS
class _rc{
public:
    _rc() {}
    vector <ll> num; vector <pll> p; ll cnt = 0;

    class node{
    public:
        ll x, y, idx;
        node(pll a, ll idx) : node(a.x, a.y, idx) {};
        node(ll x, ll y, ll idx){
            this->x = x; this->y = y;
            this->idx = idx;
        }
        operator pll(){ return { x, y }; }
        operator pld(){ return { x, y }; }
    };
    vector <node> arr, st;

    ll ccw(pll v1, pll v2, pll v3){
        ll num = (v2.x - v1.x) * (v3.y - v1.y);
        num -= (v3.x - v1.x) * (v2.y - v1.y);

        if(num > 0) return 1;
        else if(num < 0) return -1;
        return 0;
    }
    
    void add(ll a, ll b) { add({a, b}); }
    void add(pll a){ arr.push_back({a, cnt++}); }

    void init_ch(){    
        swap(arr[0], *min_element(all(arr), [&](pll a, pll b){
            if(a.x != b.x) return a.x < b.x;
            return a.y < b.y;
        }));

        sort(arr.begin() + 1, arr.end(), [&](pll a, pll b){
            ll dir = ccw(arr[0], {a.x, a.y}, {b.x, b.y});
            if(dir) return dir > 0;
            return d(arr[0], {a.x, a.y}) < d(arr[0], {b.x, b.y});
        });

        for(auto& i : arr){
            while(st.size() >= 2 && ccw(st[st.size() - 2], st.back(), i) <= 0) st.pop_back();
            st.push_back(i);
        }
        for(auto& i : st) p.push_back(i);
    }

    ll area(ll a, ll b, ll c){ // div by 2
        ll num = 0;
        vector <pll> v = {p[a], p[b], p[c]};
        for(int i = 0;i < v.size();i++){
            auto [x1, y1] = v[i];
            auto [x2, y2] = v[(i + 1) % v.size()];
            num += x1 * y2 - x2 * y1;
        }
        return abs(num);
    }

    ld d(pld a, pld b) { // chk overflow
        ld dx = a.x - b.x, dy = a.y - b.y;
        return dx * dx + dy * dy;
    }

    ll mx = 0; pair <pll, pll> result;
    void get(ll l, ll r){
        ll sz = p.size();
        for(int i = 0;i <= 1;i++){
            ll nxt = (r + i) % sz;
            ll now = d(p[l], p[nxt]);
            if(mx > now) continue;
            mx = now; result = { p[l], p[nxt] };
        }
    }

    void init_rc(){
        ll sz = p.size();
        if(sz == 2) get(0, 1);
        else{
            ll l = 0, r = 1; ld now = 0;
            while(area(sz - 1, l, r) < area(sz - 1, l, r + 1)) r++;
            while(r < 2 * sz && l < sz){
                get(l, r);
                if(area(l, l + 1, r) < area(l, l + 1, r + 1)) r++;
                else l++;
            }
        }
    }

    void init() { init_ch(); init_rc(); }
    pair <pll, pll> ret(){ return result; }
    ld dist(){ return sqrtl(mx); }
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

template <size_t n, size_t m>
class _gf2{ // 0-based index
public:
    vector <bitset <m + 1>> bs;
    _gf2() : bs(n){}
    void set_a(ll cy, ll cx, bool v){ bs[cy][cx] = v; }
    void set_b(ll cy, bool v){ bs[cy][m] = v; }

    // 해가 존재하면 1, 아니면 0 반환
    // 해는 result에 담김
    bool ret(bitset <m>& result){
        vector <ll> pos(m, -1);
        ll cy = 0;
        for(ll cx = 0;cx < m && cy < n; cx++){
            ll now = -1;
            for(int i = cy;i < n;i++){
                if(!bs[i][cx]) continue;
                now = i; break;
            }
            if(now == -1) continue;

            swap(bs[now], bs[cy]);
            pos[cx] = cy;

            for(int i = 0;i < n;i++){
                if(i == cy || !bs[i][cx]) continue;
                bs[i] ^= bs[cy];
            }

            cy++;
        }

        for(int i = 0;i < n;i++){
            bool flag = 1;
            for(int j = 0;j < m;j++){
                if(!bs[i][j]) continue;
                flag = 0; break;
            }

            if(flag && bs[i][m]) return 0;
        }

        result.reset();
        for(int i = 0;i < m;i++) result[i] = (pos[i] != -1 ? bs[pos[i]][m] : 0);
        return 1;
    }

    void clear(){ for(int i = 0;i < n;i++) bs[i].reset(); }
};
