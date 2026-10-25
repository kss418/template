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

//Binary Search
class _bs { // integer
public:
    template <class F>
    static ll max_right(ll l, ll r, const F& f) {
        ll lo = l, hi = r, mid = -1;
        while (hi > lo) {
            mid = (hi + lo + 1) >> 1ll;
            if (f(mid)) lo = mid;
            else hi = mid - 1;
        }
        return lo;
    }

    template <class F>
    static ll min_left(ll l, ll r, const F& f) {
        ll lo = l, hi = r, mid = -1;
        while (hi > lo) {
            mid = (hi + lo) >> 1ll;
            if (f(mid)) hi = mid;
            else lo = mid + 1;
        }
        return lo;
    }
};

class _fbs { // real
public:
    template <class F>
    static ld max_right(ld l, ld r, const F& f, int iter = 80) {
        ld lo = l, hi = r;
        for(int i = 0; i < iter; i++){
            ld mid = (lo + hi) / 2;
            if (f(mid)) lo = mid;
            else hi = mid;
        }
        return lo;
    }

    template <class F>
    static ld min_left(ld l, ld r, const F& f, int iter = 80) {
        ld lo = l, hi = r;
        for(int i = 0; i < iter; i++){
            ld mid = (lo + hi) / 2;
            if (f(mid)) hi = mid;
            else lo = mid;
        }
        return lo;
    }
};

//Ternary Search
class _ts { // integer
public:
    template <class F>
    static ll max(ll l, ll r, const F& f) {
        while (r - l > 3) {
            ll st = (2 * l + r) / 3, en = (l + 2 * r) / 3;
            if(f(st) < f(en)) l = st;
            else r = en;
        }

        ll result = MINF;
        for(ll cur = l; cur <= r;cur++) result = std::max(result, f(cur));
        return result;
    }

    template <class F>
    static ll min(ll l, ll r, const F& f) {
        while (r - l > 3) {
            ll st = (2 * l + r) / 3, en = (l + 2 * r) / 3;
            if(f(st) > f(en)) l = st;
            else r = en;
        }

        ll result = INF;
        for(ll cur = l; cur <= r;cur++) result = std::min(result, f(cur));
        return result;
    }
};

class _fts { // real
public:
    template <class F>
    static ld max(ld l, ld r, const F& f, int iter = 120) {
        for(int i = 0; i < iter; i++){
            ld st = (2 * l + r) / 3, en = (l + 2 * r) / 3;
            if(f(st) < f(en)) l = st;
            else r = en;
        }
        return std::max(f(l), f(r));
    }

    template <class F>
    static ld min(ld l, ld r, const F& f, int iter = 120) {
        for(int i = 0; i < iter; i++){
            ld st = (2 * l + r) / 3, en = (l + 2 * r) / 3;
            if(f(st) > f(en)) l = st;
            else r = en;
        }
        return std::min(f(l), f(r));
    }
};

class _lcs {
public:
    string a, b; vector<vector<ll>> dp;
    ll n, m;

    _lcs(string a, string b) {
        this->a = a; this->b = b;
        this->n = a.size(), this->m = b.size();
        dp.resize(n + 1, vector<ll>(m + 1, 0));
    }

    void build() {
        n = a.size(); m = b.size(); dp.assign(n + 1, vector<ll>(m + 1, 0));
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                if (a[i] == b[j]) {
                    dp[i + 1][j + 1] = dp[i][j] + 1;
                }
                else {
                    dp[i + 1][j + 1] = max(dp[i][j + 1], dp[i + 1][j]);
                }
            }
        }
    }

    ll ret() { // lcs 길이 반환
        return dp[n][m];
    }

    string track() { // lcs string 반환
        string ret;
        ll cy = n, cx = m;
        while (cy && cx) {
            if (a[cy - 1] == b[cx - 1]) {
                ret.push_back(a[cy - 1]); cy--; cx--;
            }
            else if (dp[cy - 1][cx] > dp[cy][cx - 1]) cy--;
            else cx--;
        }

        reverse(ret.begin(), ret.end());
        return ret;
    }
};

//MO's
struct mos_policy{
    struct node{
        ll v;
        node(ll v = 0) : v(v){}
    };
    struct ret{
        ll v;
        ret(ll v = 0) : v(v){}
    };
    static void push_front(int idx, const node& v, ret& cur){

    }
    static void push_back(int idx, const node& v, ret& cur){
        push_front(idx, v, cur);
    }
    static void pop_front(int idx, const node& v, ret& cur){

    }
    static void pop_back(int idx, const node& v, ret& cur){
        pop_front(idx, v, cur);
    }
};

template <class policy = mos_policy>
class _mos{
    bool base;
public:
    using node = typename policy::node;
    using ret  = typename policy::ret;
private:
    int n, b, cl = 0, cr = -1; ret cur = ret_id();
    static void push_front(int idx, const node& v, ret& cur){ policy::push_front(idx, v, cur); }
    static void push_back(int idx, const node& v, ret& cur){ policy::push_back(idx, v, cur); }
    static void pop_front(int idx, const node& v, ret& cur){ policy::pop_front(idx, v, cur); }
    static void pop_back(int idx, const node& v, ret& cur){ policy::pop_back(idx, v, cur); }
    static node node_id(){ return node(); }
    static ret ret_id(){ return ret();  }
    struct query{ int l, r, idx; };
    struct cmp {
        int b;
        bool operator()(const query& a, const query& c) const {
            int ab = a.l / b, cb = c.l / b;
            if(ab != cb) return ab < cb;
            if(ab & 1) return a.r > c.r;
            return a.r < c.r;
        }
    };
    void apply(int nl, int nr){
        while(cl > nl){ --cl; push_front(cl, arr[cl], cur); }
        while(cr < nr){ ++cr; push_back(cr, arr[cr], cur); }
        while(cl < nl){ pop_front(cl, arr[cl], cur); ++cl; }
        while(cr > nr){ pop_back(cr, arr[cr], cur); --cr; }
    }
public:
    vector <ret> result; vector <node> arr; vector <query> vq;
    _mos(int n = 0, int b = 0, bool base = 1){ clear(n, b, base); } // O(n)
    void clear(int n, int b = 0, bool base = 1){ // O(n)
        this->n = n; this->base = base; this->b = b;
        if(!b) this->b = sqrt(n + 1); vq.clear();
        arr.assign(n + 1, node_id()); result.clear();
        cl = base; cr = base - 1; cur = ret_id();
    }

    const vector<ret>& get_result() const{ return result; } // O(1)
    void add(int l, int r){ assert(base <= l && l <= r && r <= n); vq.push_back({l, r, (int)vq.size()}); } // O(1)
    void set(int idx, const node& v){ if(idx >= base && idx <= n) arr[idx] = v; } // O(1)
    void build(){ // O(q log q + (n^2 / b + qb) * update)
        if constexpr (requires{ policy::reset(); }) policy::reset();
        cl = base; cr = base - 1; cur = ret_id();
        if (vq.empty()) { result.clear(); return; } 
        sort(all(vq), cmp{b}); result.assign(vq.size(), ret_id());
        for(const auto& cq : vq){
            apply(cq.l, cq.r);
            result[cq.idx] = cur;
        }
    }
};

class _cht{
public:
    class line{
    public:
        ll a, b; line(){}
        line(ll a, ll b) : a(a), b(b){}
        ll f(ll x) const { return a * x + b; }
    };
    line id_line() const{ return is_min ? line(0, INF) : line(0, MINF);}

    vector<line> v;
    bool is_min = 1; ll idx = 0;
    _cht(bool is_min = 1) : is_min(is_min) {}
    void clear(){ v.clear(); idx = 0; }

    bool chk(const line& a, const line& b, const line& c) const{
        i128 x1 = (i128)(a.b - b.b) * (c.a - b.a), x2 = (i128)(b.b - c.b) * (b.a - a.a);
        return is_min ? x1 <= x2 : x1 >= x2;
    }

    bool better(const line& a, const line& b, ll x) const{
        i128 y1 = (i128)a.a * x + a.b, y2 = (i128)b.a * x + b.b;
        if(y1 == y2){
            // tie break
        }
        return is_min ? y1 <= y2 : y1 >= y2; 
    }

    // 기울기가 같은 두 직선을 비교
    bool better_same(const line& a, const line& b) const{
        if(a.b == b.b){
            // tie break
        }
        return is_min ? a.b <= b.b : a.b >= b.b;
    }

    // 기울기 단조 증가 순으로 넣을 것
    void add(line cur){
        if (!v.empty() && v.back().a == cur.a){
            if (!better_same(cur, v.back())) return;
            v.pop_back();
        }

        while (v.size() >= 2 && chk(v[v.size() - 2], v.back(), cur)) v.pop_back();
        v.push_back(cur);
        if (idx >= (ll)v.size()) idx = (ll)v.size() - 1;
    }

    // 단조 증가 순으로 넣을 것
    line ls(ll x){
        if(v.empty()) return id_line();
        while (idx + 1 < (ll)v.size() && better(v[idx + 1], v[idx], x)) idx++;
        return v[idx];
    }

    line bs(ll x) const{
        if(v.empty()) return id_line();
        ll lo = 0, hi = (ll)v.size() - 1;
        while (lo < hi){
            ll mid = (lo + hi) >> 1ll;
            if (better(v[mid + 1], v[mid], x)) lo = mid + 1;
            else hi = mid;
        }
        return v[lo];
    }
};

class _pbs{
public:
    ll lo, hi;
    vector <ll> l, r;
    vector <vector <ll>> arr;

    _pbs(){}
    _pbs(ll lo, ll hi) : lo(lo), hi(hi){}

    class query{ 
    public:
        ll v;
    }; vector <query> q;
    void add(query a){ q.push_back(a); }

    void clear(){
        for(ll i = lo;i <= hi;i++) arr[i].clear();
    }

    bool f(ll cur, ll idx){ // 결정 함수
        query& now = q[idx];

        return 0;
    }

    void build(){
        arr.resize(hi + 1);
        l.assign(q.size(), lo); r.assign(q.size(), hi);

        while(1){
            clear(); bool flag = 0;
            for(int i = 0;i < q.size();i++){
                if(l[i] >= r[i]) continue;
                ll mid = (l[i] + r[i] + 1) >> 1ll;
                arr[mid].push_back(i); flag = 1;
            }
            if(!flag) break;

            ll num = 0;
            for(ll mid = lo;mid <= hi;mid++){
                for(auto& idx : arr[mid]){
                    if(f(mid, idx)) l[idx] = mid;
                    else r[idx] = mid - 1;
                }
            }
        }
    }
};
