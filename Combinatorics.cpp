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

// comb mod (p1^e1 + p2^e2...)
class _comb_crt{ // Require dynamic_mint, crt
public:
    using result = _crt::result;
    class _comb{ // O(p1^e1 + p2^e2 ...)
    public:
        vector <_dynamic_mint> pre; ll p, e, mod = 1;
        _comb(ll prime, ll exp) : p(prime), e(exp){
            for(int i = 1;i <= e;i++) mod *= p;
            pre.assign(mod + 1, _dynamic_mint(1, mod));
            for(ll i = 1;i <= mod;i++){
                pre[i] = pre[i - 1];
                if(i % p) pre[i] *= i;
            }
        }   
        
        ll vp(ll n) const{
            ll ret = 0;
            while(n){ n /= p; ret += n; }
            return ret;
        }

        _dynamic_mint f(ll n) const{ // p^e = p^k * f(n)
            if(!n) return _dynamic_mint(1, mod);
            ll div = n / mod, rem = n % mod; _dynamic_mint ret(1, mod);
            if(p % 2) ret = div & 1 ? ret *= _dynamic_mint(mod - 1, mod) : _dynamic_mint(1, mod);
            else ret *= pre[mod].pow(div);
            ret *= pre[rem]; ret *= f(n / p);
            return ret;
        }

        ll ret(ll n, ll r) const{ // O(log n + log p)
            if(r < 0 || r > n) return 0;
            ll now = vp(n) - vp(r) - vp(n - r);
            if(now >= e) return 0;

            _dynamic_mint div = f(r) * f(n - r), inv = div.inv();
            return (f(n) * inv * _dynamic_mint(p, mod).pow(now)).v;
        }
    }; 

    vector <_comb> c; vector <_lucas> l; _crt crt;
    void add(ll p, ll e){ // O(1) 
        if(e <= 0) return;
        if(e == 1) l.emplace_back(p);
        else c.emplace_back(p, e);
    }

    result ret(ll n, ll r){ // O(log n + log p)
        crt.clear();
        for(auto& i : c) crt.add(i.ret(n, r), i.mod);
        for(auto& i : l) crt.add(i.comb(n, r), i.get_mod());
        crt.build();
        return crt.ret();
    }

    void clear(){ l.clear(); c.clear(); crt.clear(); }
};