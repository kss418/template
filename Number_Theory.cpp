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

template <u64 mod> requires (mod > 0)
class _mint{
private:
    using mint = _mint;
    using u128 = __uint128_t;
public:
    u64 v;
    template <integral T = int>
    constexpr _mint(T v = 0) : v(norm((i128)v)){}
    static constexpr u64 get_mod(){ return mod; }
    static constexpr u64 norm(i128 x){ x %= mod; return (u64)(x < 0 ? x + mod : x); }
    static constexpr tuple <u64, i128, i128> gcd(u64 a, u64 b){
        if(!b) return {a, 1, 0};
        auto [g, x, y] = gcd(b, a % b);
        return {g, y, x - (a / b) * y};
    }

    constexpr mint inv() const{
        assert(v);
        auto [g, x, y] = gcd(v, mod);
        assert(g == 1);
        return mint(x);
    }

    constexpr mint pow(u64 e) const{
        mint a = *this, ret(1);
        while(e){
            if(e & 1) ret *= a;
            a *= a; e >>= 1;
        }
        return ret;
    }

    constexpr mint& operator+=(const mint& a){
        u64 b = a.v;
        if(v >= mod - b) v -= mod - b;
        else v += b;
        return *this;
    }

    constexpr mint& operator-=(const mint& a){
        u64 b = a.v;
        if(v < b) v += mod - b;
        else v -= b;
        return *this;
    }

    constexpr mint& operator*=(const mint& a){ v = (u64)((u128)v * a.v % mod); return *this; }
    constexpr mint& operator/=(const mint& a){ return *this *= a.inv(); }
    friend constexpr mint operator+(mint a, const mint& b){ return a += b; }
    friend constexpr mint operator-(mint a, const mint& b){ return a -= b; }
    friend constexpr mint operator*(mint a, const mint& b){ return a *= b; }
    friend constexpr mint operator/(mint a, const mint& b){ return a /= b; }
    friend constexpr bool operator==(const mint& a, const mint& b){ return a.v == b.v; }
    constexpr mint operator+() const{ return *this; }
    constexpr mint operator-() const{ auto ret = *this; ret.v = v ? mod - v : 0; return ret; }
    friend ostream& operator<<(ostream& out, const mint& a){ return out << a.v; }
    friend istream& operator>>(istream& in, mint& a){ ll v; if(in >> v) a = v; return in; }
};

class _dynamic_mint{
private:
    using mint = _dynamic_mint;
    using u128 = __uint128_t;
    template <class T> static constexpr bool arg = integral<T> || same_as<T, mint>;
    template <class A, class B> static constexpr bool args = arg<A> && arg<B> && (same_as<A, mint> || same_as<B, mint>);
    static constexpr u64 val(i128 a, u64 mod){ return norm(a, mod); }
    static constexpr u64 val(const mint& a, u64 mod){ assert(a.mod == mod); return a.v; }
    template <class A, class B>
    static constexpr mint cast(const A& a, const B& b){
        if constexpr(same_as<A, mint>) return a;
        else return mint((i128)a, b.mod);
    }
public:
    u64 mod = 1, v = 0;
    constexpr _dynamic_mint() = default;
    template <integral T>
    constexpr _dynamic_mint(T v, u64 mod) : mod(mod), v(norm((i128)v, mod)){}
    constexpr u64 get_mod() const{ return mod; }
    static constexpr u64 norm(i128 x, u64 m){ x %= m; return (u64)(x < 0 ? x + m : x); }
    static constexpr tuple <u64, i128, i128> gcd(u64 a, u64 b){
        if(!b) return {a, 1, 0};
        auto [g, x, y] = gcd(b, a % b);
        return {g, y, x - (a / b) * y};
    }

    constexpr mint inv() const{
        assert(v);
        auto [g, x, y] = gcd(v, mod);
        assert(g == 1);
        return mint(x, mod);
    }

    constexpr mint pow(u64 e) const{
        mint a = *this, ret(1, mod);
        while(e){
            if(e & 1) ret *= a;
            a *= a; e >>= 1;
        }
        return ret;
    }

    template <class T> requires arg<T>
    constexpr mint& operator+=(const T& a){
        u64 b = val(a, mod);
        if(v >= mod - b) v -= mod - b;
        else v += b;
        return *this;
    }

    template <class T> requires arg<T>
    constexpr mint& operator-=(const T& a){
        u64 b = val(a, mod);
        if(v < b) v += mod - b;
        else v -= b;
        return *this;
    }

    template <class T> requires arg<T>
    constexpr mint& operator*=(const T& a){ v = (u64)((u128)v * val(a, mod) % mod); return *this; }
    template <class T> requires arg<T>
    constexpr mint& operator/=(const T& a){ return *this *= mint(val(a, mod), mod).inv(); }

    template <class A, class B> requires args<A, B>
    friend constexpr mint operator+(const A& a, const B& b){ return cast(a, b) += b; }
    template <class A, class B> requires args<A, B>
    friend constexpr mint operator-(const A& a, const B& b){ return cast(a, b) -= b; }
    template <class A, class B> requires args<A, B>
    friend constexpr mint operator*(const A& a, const B& b){ return cast(a, b) *= b; }
    template <class A, class B> requires args<A, B>
    friend constexpr mint operator/(const A& a, const B& b){ return cast(a, b) /= b; }
    template <class A, class B> requires args<A, B>
    friend constexpr bool operator==(const A& a, const B& b){
        auto l = cast(a, b), r = cast(b, a);
        return l.mod == r.mod && l.v == r.v;
    }

    template <integral T>
    constexpr mint& operator=(T a){ v = norm((i128)a, mod); return *this; }
    constexpr mint operator+() const{ return *this; }
    constexpr mint operator-() const{ auto ret = *this; ret.v = v ? mod - v : 0; return ret; }
    friend ostream& operator<<(ostream& out, const mint& a){ return out << a.v; }
    friend istream& operator>>(istream& in, mint& a){ ll v; if(in >> v) a = v; return in; }
};

template <size_t n>
class _es{
public:
    static constexpr size_t sz = (n >> 1) + 1;
private:
    bitset <sz> bs;
    vector <int> prime;
public:
    _es(){ build(); }

    void build(){ // O(n log log n)
        prime.clear(); bs.reset();
        if(n >= 2) prime.push_back(2);
        bs.set(0);
        for(int i = 3;i <= n;i += 2){
            if(bs[i >> 1]) continue;
            prime.push_back(i);
            for(ll j = 1ll * i * i;j <= (ll)n;j += 2ll * i) bs.set(j >> 1);
        }
    }

    const vector<int>& ret() const{ return prime; } // O(1)
    int count() const{ return prime.size(); } // O(1)
    int count(int l, int r) const{ // O(log pi(n))
        if(l > r) return 0;
        return upper_bound(all(prime), r) - lower_bound(all(prime), l);
    }
    template <class F>
    void it_prime(const F& f) const{ for(auto& p : prime) f(p); } // O(pi(n))
    bool is_prime(int x) const{ // O(1)
        if(x == 2) return 1;
        if(x < 2 || x % 2 == 0) return 0;
        return !bs[x >> 1];
    }
};

template <size_t n>
class _es_segment : public _es<n>{
private:
    ll l = 0, r = -1;
    vector<bool> bs;
    vector<ll> prime;
public:
    _es_segment() = default;
    _es_segment(ll l, ll r){ build(l, r); }

    void build(){ build(l, r); } // O(w log log r + pi(sqrt r))
    void build(ll l, ll r){ // O(w log log r + pi(sqrt r))
        this->l = l; this->r = r; prime.clear();
        if(l > r){ bs.clear(); return; }
        bs.assign(r - l + 1, false);
        ll lo = max(l, 2ll);
        for(int p : _es<n>::ret()){
            ll square = 1ll * p * p;
            if(square > r) break;
            ll first = max(square, ((lo + p - 1) / p) * p);
            for(ll v = first;v <= r;v += p) bs[v - l] = true;
        }
        for(ll v = lo;v <= r;v++) if(!bs[v - l]) prime.push_back(v);
    }

    const vector<ll>& ret() const{ return prime; } // O(1)
    ll count() const{ return prime.size(); } // O(1)
    ll count(ll l, ll r) const{ // O(log k)
        if(l > r) return 0;
        return upper_bound(all(prime), r) - lower_bound(all(prime), l);
    }
    template <class F>
    void it_prime(const F& f) const{ for(auto p : prime) f(p); } // O(k)
    bool is_prime(ll x) const{ return x >= 2 && !bs[x - l]; } // O(1)
};

namespace _gcd{
    constexpr tll build(ll a, ll b){ // O(log max(|a|, |b|))
        i128 g = a, r = b, x = 1, y = 0, nx = 0, ny = 1;
        while(r){
            i128 q = g / r, tmp = g % r;
            g = r; r = tmp;
            tmp = x - q * nx; x = nx; nx = tmp;
            tmp = y - q * ny; y = ny; ny = tmp;
        }
        if(g < 0){ g = -g; x = -x; y = -y; }
        return {(ll)g, (ll)x, (ll)y};
    }

    constexpr ll ret(ll a, ll b){ // O(log max(|a|, |b|))
        i128 x = a, y = b;
        while(y){ i128 tmp = x % y; x = y; y = tmp; }
        return (ll)(x < 0 ? -x : x);
    }

    constexpr ll inv(ll a, ll mod){ // O(log mod)
        auto [g, x, y] = build(a, mod);
        if(g != 1) return -1;
        x %= mod;
        return x < 0 ? x + mod : x;
    }

    constexpr optional<pll> solve(ll a, ll b, ll mod){ // O(log mod)
        auto [g, x, y] = build(a, mod);
        if(b % g) return nullopt;
        ll period = mod / g;
        i128 value = (i128)x * (b / g) % period;
        if(value < 0) value += period;
        return pll{(ll)value, period};
    }
}

class _crt{
public:
    enum class state{ known, inconsistent };
    class result{
    public:
        state status;
        ll value, mod;
        bool inconsistent() const{ return status == state::inconsistent; } // O(1)
        ll get_value() const{ return value; } // O(1)
        ll get_mod() const{ return mod; } // O(1)
    };

    vector <_dynamic_mint> arr;
private:
    result ans = {state::known, 0, 1};
    static tll gcd(ll a, ll b){
        if(!b) return {a, 1, 0};
        auto [g, x, y] = gcd(b, a % b);
        return {g, y, x - (a / b) * y};
    }

    static result merge(const result& a, const _dynamic_mint& b){
        auto [g, cx, cy] = gcd(a.mod, (ll)b.mod);
        ll diff = (ll)b.v - a.value;
        if(diff % g) return {state::inconsistent, 0, 0};
        i128 mod = (i128)a.mod / g * b.mod;
        ll m = b.mod / g, k = (ll)((i128)(diff / g) * cx % m);
        if(k < 0) k += m;
        ll v = (ll)((i128)a.value + (i128)a.mod * k);
        return {state::known, v, (ll)mod};
    }
public:
    void add(ll a, ll m){ arr.emplace_back(a, m); } // O(1) amortized
    void add(pll a){ arr.emplace_back(a.x, a.y); } // O(1) amortized
    void add(const _dynamic_mint& a){ arr.push_back(a); } // O(1) amortized
    void clear(){ arr.clear(); ans = {state::known, 0, 1}; } // O(n)

    void build(){ // O(n log M)
        ans = {state::known, 0, 1};
        for(const auto& i : arr){
            ans = merge(ans, i);
            if(ans.status != state::known) break;
        }
    }

    const result& ret() const{ return ans; } // O(1)
};

//이산 로그
class _dlog{
public:
    map<ll, ll> num; vector <ll> prime;
    _dlog(){};

    void factor(ll p){ // 소인수분해
        prime.clear();  
        for(ll i = 2;i <= p / i;i++){
            if(p % i) continue;
            prime.push_back(i);
            while(p % i == 0) p /= i;
        }
        if(p > 1) prime.push_back(p);
    }

    ll phi(ll p){ 
        factor(p); ll ret = p;
        for(auto& i : prime) ret /= i, ret *= i - 1;
        return ret;
    }

    ll sqrt(ll num){
        ll lo = 0, hi = num;
        while(hi > lo){
            ll mid = midpoint(hi, lo);
            if((i128)mid * mid < num) lo = mid;
            else hi = mid - 1;
        }
        return lo;
    }

    ll pow(ll a, ll b, ll p){
        ll ret = 1 % p; a %= p;
        if(a < 0) a += p;
        while(b){
            if(b & 1) ret = (i128)ret * a % p;
            a = (i128)a * a % p; b >>= 1;
        }      
        return ret;
    }
    
    ll ret(ll a, ll b, ll p) { // a^x = b (mod p), ret x
        b %= p;
        if(b < 0) b += p;
        ll mx = sqrt(phi(p)) + 1; num.clear();
        ll inv = pow(a, p - 2, p), cur = b;
        for(ll i = 0;i < mx;i++) {
            if(num.find(cur) == num.end()) num[cur] = i;
            cur = (i128)cur * inv % p;
        }
        ll step = pow(a, mx, p);
        cur = 1; for(ll i = 0;i < mx;i++){
            if(num.find(cur) != num.end()) return i * mx + num[cur];
            cur = (i128)cur * step % p;
        }
        return -1;
    }
};

//Pollard Rho
class _pr{
private:
    using u128 = __uint128_t;
    mt19937_64 rnd;
    static constexpr u64 small_primes[] = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

    static constexpr u64 mul_mod(u64 a, u64 b, u64 mod){ return (u128)a * b % mod; }
    static constexpr u64 pow_mod(u64 a, u64 b, u64 mod){
        u64 ret = 1; a %= mod;
        while(b){
            if(b & 1) ret = mul_mod(a, ret, mod);
            a = mul_mod(a, a, mod);
            b >>= 1ll;
        }
        return ret;
    }

    static constexpr u64 f(u64 x, u64 c, u64 mod){ return ((u128)x * x + c) % mod; }

    u64 pollard(u64 n){
        for(u64 p : small_primes) if(n % p == 0) return p;
        uniform_int_distribution<u64> rx(2, n - 2), rc(1, n - 1);
        auto diff = [](u64 a, u64 b){ return a > b ? a - b : b - a; };

        while(1){
            u64 b = rx(rnd), c = rc(rnd), g = 1;
            for(u64 len = 1;g == 1;len *= 2){
                u64 a = b;
                for(u64 i = 0;i < len && g == 1;){
                    u64 start = b, product = 1, cnt = min<u64>(128, len - i);
                    for(u64 j = 0;j < cnt;j++){
                        b = f(b, c, n);
                        product = mul_mod(product, diff(a, b), n);
                    }
                    g = gcd(product, n);
                    if(g == n){
                        do{
                            start = f(start, c, n);
                            g = gcd(diff(a, start), n);
                        }while(g == 1);
                    }
                    i += cnt;
                }
            }
            if(g != n) return g;
        }
    }

    void dnc(u64 n, vector<u64>& ret){
        if(n == 1) return;
        if(isprime(n)){
            ret.push_back(n);
            return;
        }

        u64 p = pollard(n);
        dnc(p, ret); dnc(n / p, ret);
    }
public:
    _pr() : _pr((u64)chrono::steady_clock::now().time_since_epoch().count()){}
    explicit _pr(u64 seed) : rnd(seed){}

    static constexpr bool isprime(u64 n){
        if(n < 2) return 0;
        for(u64 i : small_primes){
            if(n == i) return 1;
            if(n % i == 0) return 0;
        }

        u64 d = n - 1, s = 0;
        while((d & 1) == 0) d >>= 1ll, ++s;

        auto chk = [&](u64 a) -> bool{
            if(a % n == 0) return 0;
            u64 x = pow_mod(a, d, n);
            if(x == 1 || x == n - 1) return 0;
            for(u64 r = 1;r < s;r++){
                x = mul_mod(x, x, n);
                if(x == n - 1) return 0;
            }
            return 1;
        };

        for (u64 a : {2ULL, 325ULL, 9375ULL, 28178ULL, 450775ULL, 9780504ULL, 1795265022ULL}) {
            if (chk(a)) return 0;
        }
        return 1;
    }

    vector <u64> factorize(u64 n){ // 소인수분해 (n >= 1)
        vector<u64> ret;
        dnc(n, ret);
        sort(all(ret));
        return ret;
    }

    vector <pair<u64, int>> factorize_cnt(u64 n){
        auto f = factorize(n);
        vector<pair<u64, int>> ret;
        for(u64& p : f){
            if(ret.empty() || ret.back().x != p) ret.push_back({p, 1});
            else ret.back().y++;
        }
        return ret;
    }

    u64 phi(u64 n){ return n ? phi(factorize_cnt(n)) : 0; }
    static constexpr u64 phi(span<const pair<u64, int>> fc){ // O(sum of exponents)
        u64 ret = 1;
        for(auto [p, c] : fc){
            ret *= p - 1;
            for(int i = 1;i < c;i++) ret *= p;
        }
        return ret;
    }

    int mobius(u64 n){ return n <= 1 ? (int)n : mobius(factorize_cnt(n)); }
    static constexpr int mobius(span<const pair<u64, int>> fc){ // O(fc.size())
        for(auto [p, c] : fc) if(c >= 2) return 0;
        return fc.size() % 2 ? -1 : 1;
    }

    u64 divisor_num(u64 n){ return n ? divisor_num(factorize_cnt(n)) : 0; }
    static constexpr u64 divisor_num(span<const pair<u64, int>> fc){ // O(fc.size())
        u64 ret = 1;
        for(auto [p, c] : fc) ret *= c + 1;
        return ret;
    }

    u64 divisor_sum(u64 n){ return n ? divisor_sum(factorize_cnt(n)) : 0; }
    static constexpr u64 divisor_sum(span<const pair<u64, int>> fc){ // O(sum of exponents)
        u128 ret = 1;
        for(auto [p, c] : fc){
            u128 now = 1, cur = 1;
            for(int i = 0;i < c;i++){
                cur *= p;
                now += cur;
            }
            ret *= now;
        }
        return (u64)ret;
    }
};

class _ls{
private:
    vector <int> lp, prime, vphi, vmu; int n = 0, flag = 0;
public:
    static constexpr int with_phi = 1, with_mobius = 2;

    _ls() = default;
    _ls(int n, int flag = 0) : n(n), flag(flag){ build(); }

    void make(int n, int flag = 0){ this->n = n; this->flag = flag; } // O(1)

    void build(){ // O(n)
        bool do_phi = flag & with_phi, do_mu = flag & with_mobius;
        prime.clear(); vphi.clear(); vmu.clear();
        lp.assign(n + 1, 0);
        if(n >= 3) prime.reserve((int)(n / log((ld)n)) + 10);
        if(do_phi){ vphi.assign(n + 1, 0); if(n >= 1) vphi[1] = 1; }
        if(do_mu){ vmu.assign(n + 1, 0); if(n >= 1) vmu[1] = 1; }

        for(int i = 2;i <= n;i++){
            if(!lp[i]){
                lp[i] = i;
                prime.push_back(i);
                if(do_phi) vphi[i] = i - 1;
                if(do_mu) vmu[i] = -1;
            }

            int least = lp[i];
            for(int p : prime){
                ll x = 1ll * p * i;
                if(x > n) break;
                lp[x] = p;

                if(p == least){
                    if(do_phi) vphi[x] = vphi[i] * p;
                    if(do_mu) vmu[x] = 0;
                    break;
                }
                if(do_phi) vphi[x] = vphi[i] * (p - 1);
                if(do_mu) vmu[x] = -vmu[i];
            }
        }
    }

    const vector<int>& ret() const{ return prime; } // O(1)
    int count() const{ return prime.size(); } // O(1)
    int count(int l, int r) const{ // O(log pi(n))
        if(l > r) return 0;
        return upper_bound(all(prime), r) - lower_bound(all(prime), l);
    }
    template <class F>
    void it_prime(const F& f) const{ for(int p : prime) f(p); } // O(pi(n))
    bool is_prime(int x) const{ return x >= 2 && lp[x] == x; }

    vector <ll> factorize(int x){ // 소인수분해 (1 <= x <= n)
        vector <ll> ret;
        while(x != 1){
            ret.push_back(lp[x]);
            x /= lp[x];
        }
        return ret;
    }

    vector <pll> factorize_cnt(int x){ // p^c 꼴로 소인수분해 (1 <= x <= n)
        vector <pll> ret;
        while(x != 1){
            ll p = lp[x], c = 0;
            while(x % p == 0) x /= p, c++;
            ret.push_back({p, c});
        }
        return ret;
    }

    ll divisor_num(int x){ // 약수의 개수
        if(!x) return 0;
        ll ret = 1;
        while(x != 1){
            ll p = lp[x], c = 0;
            while(x % p == 0) x /= p, c++;
            ret *= c + 1;
        }
        return ret;
    }

    ll divisor_sum(int x){  // 약수의 합
        if(x == 0 || x == 1) return x;   
        ll ret = 1;
        while(x != 1){
            ll p = lp[x], sum = 1, cur = 1;
            while(x % p == 0){
                x /= p;
                cur *= p;
                sum += cur;
            }
            ret *= sum;
        }
        return ret;
    }

    vector <ll> divisor(int x){ // 약수 벡터 반환
        if(!x) return {};
        if(x == 1) return {1};
        auto fc = factorize_cnt(x);
        vector <ll> ret = {1};
        for(auto& [p, c] : fc){
            ll mul = 1, sz = ret.size();
            for(ll k = 1;k <= c;k++){
                mul *= p;
                for(int i = 0;i < sz;i++) ret.push_back(ret[i] * mul);
            }
        }

        sort(all(ret));
        return ret;
    }

    ll phi(int x) const { return vphi[x]; } // 오일러 피
    ll mobius(int x) const { return vmu[x]; } // 뫼비우스
};

// Prime Tower
class _pt{
public:
    using result = _crt::result;
    vector <ll> arr, mods; _crt crt;
    void add(ll v){ // O(1) 
        if(!arr.empty() && arr.back() <= 1) return;
        arr.push_back(v); 
    }

    void add_mod(ll p, ll e){ // O(e)
        ll mod = 1;
        for(int i = 1;i <= e;i++) mod *= p;
        mods.push_back(mod);
    }

    void add_mod(ll m){ // O(sqrt(m))
        for(ll i = 2;i * i <= m;i++){
            if(m % i) continue; ll c = 0;
            while(m % i == 0) m /= i, c++;
            add_mod(i, c);
        }
        if(m > 1) add_mod(m, 1);
    }

    i128 mul_cap(ll a, ll b, ll cap){
        if(!a || !b) return 0;
        if(a >= cap) return cap;
        i128 ret = (i128)a * b;
        return (ret >= (i128)cap) ? cap : ret; 
    }

    ll pow_cap(ll a, ll b, ll cap){
        i128 ret = 1;
        while(b){
            if(b & 1){
                ret = mul_cap(ret, a, cap);
                if(ret >= cap) return cap;
            }
            b >>= 1ll;
            if(!b) break;
            a = mul_cap(a, a, cap);
            if (a >= cap) a = cap;
        }
        return ret;
    }

    ll pow_mod(ll a, ll b, ll p){ return _dynamic_mint(a, p).pow(b).v; }
    ll cal_phi(ll x){
        ll ret = x;
        for(ll i = 2;i * i <= x;i++){
            if(x % i) continue;
            while(x % i == 0) x /= i;
            ret *= i - 1; ret /= i;
        }
        if(x > 1) ret *= x - 1, ret /= x;
        return ret;
    }

    ll cal_exact(int cur){
        if(cur + 1 == arr.size()) return arr[cur];
        return pow_cap(arr[cur], cal_exact(cur + 1), 100);
    }

    bool tower_le(int cur, ll num){
        if((int)arr.size() - cur > 4) return 0;
        ll now = arr.back();
        for(int i = (int)arr.size() - 2;i >= cur;i--){
            if(now > num) return 0;
            now = pow_cap(arr[i], now, num + 1);
        }
        return now <= num;
    }

    ll cal(int cur, ll mod){
        if(mod == 1) return 0;
        if(arr[cur] == 1) return 1;
        if(cur + 1 == arr.size()) return _dynamic_mint(arr[cur], mod).v;
        if(tower_le(cur + 1, 100)) return pow_mod(arr[cur], cal_exact(cur + 1), mod);
        ll phi = cal_phi(mod), e = cal(cur + 1, phi) + 100 * phi;
        return pow_mod(arr[cur], e, mod);
    }

    void clear(){ arr.clear(); mods.clear(); crt.clear(); } // O(n)
    void build(){
        crt.clear();
        for(ll mod : mods) crt.add(cal(0, mod), mod);
        crt.build();
    }

    const result& ret() const{ return crt.ret(); } // O(1)
};
