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

template <u64 mod = 998244353>
class _comb{ // mod == prime
public:
    using mint = _mint<mod>;
private:
    vector <mint> fac, ifac; int n;
public:
    _comb(int n = 0){ build(n); } // O(n + log mod)

    void build(int n){ // O(n + log mod)
        assert(n >= 0 && (u64)n < mod);
        this->n = n; fac.resize(n + 1); ifac.resize(n + 1);
        fac[0] = 1;
        for(int i = 1;i <= n;i++) fac[i] = fac[i - 1] * i;
        ifac[n] = fac[n].pow(mod - 2);
        for(int i = n - 1;i >= 0;i--) ifac[i] = ifac[i + 1] * (i + 1);
    }

    u64 comb(int n, int r) const{ // O(1)
        if(n < 0 || r < 0 || n < r) return 0;
        return (fac[n] * ifac[r] * ifac[n - r]).v;
    }

    u64 perm(int n, int r) const{ // O(1)
        if(n < 0 || r < 0 || n < r) return 0;
        return (fac[n] * ifac[n - r]).v;
    }

    u64 multicomb(int n, int r) const{ // O(1)
        if(n < 0 || r < 0) return 0;
        if(!n) return !r;
        ll total = (ll)n + r - 1;
        assert(total <= this->n);
        return comb((int)total, r);
    }

    u64 factorial(int n) const{ return fac[n].v; } // O(1)
    u64 inv_factorial(int n) const{ return ifac[n].v; } // O(1)
};

class _lucas{ // mod == prime
private:
    int mod = 0; vector <int> fac, ifac;

    int cal(int n, int r) const{ // O(1)
        if(n < r) return 0;
        return (ll)fac[n] * ifac[r] % mod * ifac[n - r] % mod;
    }
public:
    _lucas() = default;
    _lucas(int mod){ build(mod); } // O(mod)

    void build(int mod){ // O(mod)
        assert(mod >= 2);
        this->mod = mod; fac.resize(mod); ifac.resize(mod);
        fac[0] = 1;
        for(int i = 1;i < mod;i++) fac[i] = (ll)fac[i - 1] * i % mod;
        ifac[mod - 1] = _dynamic_mint(fac[mod - 1], mod).pow(mod - 2).v;
        for(int i = mod - 2;i >= 0;i--) ifac[i] = (ll)ifac[i + 1] * (i + 1) % mod;
    }

    int get_mod() const{ return mod; } // O(1)

    int comb(ll n, ll r) const{ // O(log_mod(n))
        if(n < r || n < 0 || r < 0) return 0;
        if(!n || !r || n == r) return 1;
        return (ll)cal(n % mod, r % mod) * comb(n / mod, r / mod) % mod;
    }

    int perm(ll n, ll r) const{ // O(1)
        if(n < 0 || r < 0 || n < r || r >= mod) return 0;
        int rem = n % mod;
        if(rem < r) return 0;
        return (ll)fac[rem] * ifac[rem - r] % mod;
    }

    int multicomb(ll n, ll r) const{ // O(log_mod(n + r))
        if(n < 0 || r < 0) return 0;
        if(!n) return !r;
        i128 total = (i128)n + r - 1;
        assert(total <= LLONG_MAX);
        return comb((ll)total, r);
    }
};

// comb mod lcm(p1^e1, p2^e2, ...)
class _comb_crt{ // Require dynamic_mint, crt
public:
    using result = _crt::result;
private:
    class prime_power{
    private:
        vector <ll> pre; ll p, e, mod = 1;

        ll mul(ll a, ll b) const{ return (i128)a * b % mod; } // O(1)

        ll vp(ll n) const{ // O(log_p(n))
            ll ret = 0;
            while(n){ n /= p; ret += n; }
            return ret;
        }

        ll f(ll n) const{ // O(log_p(n))
            if(!n) return 1;
            ll div = n / mod, rem = n % mod, ret = pre[rem];
            if(div & 1) ret = mul(ret, pre[mod]);
            return mul(ret, f(n / p));
        }

        ll cal(ll numerator, ll denominator, ll power) const{ // O(log mod + log power)
            ll inv = _dynamic_mint(denominator, mod).inv().v;
            ll factor = _dynamic_mint(p, mod).pow(power).v;
            return mul(mul(numerator, inv), factor);
        }
    public:
        prime_power(ll p, ll e) : p(p), e(e){ // O(p^e)
            for(ll i = 0;i < e;i++) mod *= p;
            pre.assign(mod + 1, 1);
            for(ll i = 1;i <= mod;i++){
                pre[i] = pre[i - 1];
                if(i % p) pre[i] = mul(pre[i], i);
            }
        }

        ll get_mod() const{ return mod; } // O(1)

        ll comb(ll n, ll r) const{ // O(log_p(n) + log mod)
            if(n < 0 || r < 0 || r > n) return 0;
            ll now = vp(n) - vp(r) - vp(n - r);
            if(now >= e) return 0;
            return cal(f(n), mul(f(r), f(n - r)), now);
        }

        ll perm(ll n, ll r) const{ // O(log_p(n) + log mod)
            if(n < 0 || r < 0 || r > n) return 0;
            ll now = vp(n) - vp(n - r);
            if(now >= e) return 0;
            return cal(f(n), f(n - r), now);
        }
    };

    vector <pll> factors;
    vector <prime_power> c; vector <_lucas> l;

    template<class F>
    result query(const F& f) const{ // O(k * (log n + log M))
        _crt crt;
        for(const auto& i : c) crt.add(f(i), i.get_mod());
        for(const auto& i : l) crt.add(f(i), i.get_mod());
        crt.build(); return crt.ret();
    }
public:
    void add(ll p, ll e){ // O(1) amortized
        if(e <= 0) return;
        factors.emplace_back(p, e);
    }

    void build(){ // O(sum(p^e))
        c.clear(); l.clear();
        for(const auto& [p, e] : factors){
            if(e == 1) l.emplace_back(p);
            else c.emplace_back(p, e);
        }
    }

    result comb(ll n, ll r) const{ // O(k * (log n + log M))
        return query([&](const auto& table){ return table.comb(n, r); });
    }

    result perm(ll n, ll r) const{ // O(k * (log n + log M))
        return query([&](const auto& table){ return table.perm(n, r); });
    }

    result multicomb(ll n, ll r) const{ // O(k * (log(n + r) + log M))
        if(n < 0 || r < 0) return comb(-1, 0);
        if(!n) return comb(0, r ? 1 : 0);
        i128 total = (i128)n + r - 1;
        assert(total <= LLONG_MAX);
        return comb((ll)total, r);
    }

    void clear(){ factors.clear(); c.clear(); l.clear(); } // O(sum(p^e))
};