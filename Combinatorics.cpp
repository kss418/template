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

class _comb{
public:
    vector <_dynamic_mint> fac, inv; int n; ll m; // m == prime
    _comb(int n, ll m = 998244353) : n(n), m(m), fac(n + 1), inv(n + 1){ 
        assert(n < m);
        fac[0] = _dynamic_mint(1, m);
        for(int i = 1;i <= n;i++) fac[i] = fac[i - 1] * i;
        inv[n] = fac[n].pow(m - 2);
        for(int i = n - 1;i >= 0;i--) inv[i] = inv[i + 1] * (i + 1);
    }

    ll ret(ll n, ll r){
        if(n < 0 || r < 0 || n < r) return 0;
        return (fac[n] * inv[r] * inv[n - r]).v;
    }
};

class _lucas{ // mod == prime
public:
    int mod; vector <_dynamic_mint> fac, inv;
    _lucas(int mod) : mod(mod), fac(mod), inv(mod){ // O(mod)
        fac[0] = _dynamic_mint(1, mod);
        for(int i = 1;i < mod;i++) fac[i] = fac[i - 1] * i;
        inv[mod - 1] = fac[mod - 1].pow(mod - 2);
        for(int i = mod - 2;i >= 0;i--) inv[i] = inv[i + 1] * (i + 1);
    }

    _dynamic_mint cal(ll n, ll r){ // O(1)
        if(n < r) return _dynamic_mint(0, mod);
        return fac[n] * inv[r] * inv[n - r];
    }

    int ret(ll n, ll r){ // O(log n)
        if(n < r || n < 0 || r < 0) return 0;
        if(!n || !r || n == r) return 1;
        return (cal(n % mod, r % mod) * ret(n / mod, r / mod)).v;
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
        for(auto& i : l) crt.add(i.ret(n, r), i.mod);
        crt.build();
        return crt.ret();
    }

    void clear(){ l.clear(); c.clear(); crt.clear(); }
};