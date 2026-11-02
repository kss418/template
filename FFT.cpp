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

template <size_t sz>
class _fft{
private:
    using cpd = complex <double>;
    static constexpr size_t n = sz ? bit_ceil(sz) : 0;
    static void transform(vector <cpd>& v, bool inv){ // O(n log n)
        if(n < 2) return;
        for(ll i = 1, j = 0;i < n;i++){
            ll bit = n >> 1;
            for(;j & bit;bit >>= 1) j ^= bit;
            j ^= bit;
            if(i < j) swap(v[i], v[j]);
        }
        const double PI = acos(-1.0);
        for(ll k = 1;k < n;k <<= 1){
            double a = PI / k * (inv ? 1 : -1);
            cpd w(cos(a), sin(a));
            for(ll i = 0;i < n;i += 2 * k){
                cpd wp(1, 0);
                for(ll j = 0;j < k;j++){
                    cpd l = v[i + j], r = v[i + j + k] * wp;
                    v[i + j] = l + r;
                    v[i + j + k] = l - r;
                    wp *= w;
                }
            }
        }
        if(inv) for(auto& i : v) i /= (double)n;
    }
public:
    class node{
        friend class _fft;
    private:
        vector <cpd> v; ll len;
        node(ll len) : v(n), len(len){}
    public:
        node& operator+=(const node& a){ // O(n)
            for(ll i = 0;i < n;i++) v[i] += a.v[i];
            len = max(len, a.len);
            return *this;
        }

        node& operator*=(const node& a){ // O(n)
            for(ll i = 0;i < n;i++) v[i] *= a.v[i];
            len = len && a.len ? len + a.len - 1 : 0;
            return *this;
        }

        friend node operator+(node a, const node& b){ a += b; return a; } // O(n)
        friend node operator*(node a, const node& b){ a *= b; return a; } // O(n)
        friend ostream& operator<<(ostream& out, const node& a){ // O(n log n)
            auto ret = _fft{}.inverse(a);
            if(ret.empty()) return out << 0;
            for(ll i = (ll)ret.size() - 1;i >= 0;i--) out << ret[i];
            return out;
        }
    };

    template <integral T>
    node make(const vector <T>& arr) const{ // O(n log n)
        node ret(arr.size());
        for(ll i = 0;i < (ll)arr.size();i++) ret.v[i] = (double)arr[i];
        transform(ret.v, 0);
        return ret;
    }

    template <integral T = ll>
    vector <T> inverse(const node& a) const{ // O(n log n)
        if(!a.len) return {};
        auto v = a.v; transform(v, 1);
        vector <T> ret(a.len);
        for(ll i = 0;i < a.len;i++) ret[i] = (T)round(v[i].real());
        return ret;
    }

    node carry(const node& a, ll radix = 10) const{ // O(n log n)
        auto ret = inverse(a);
        if(ret.empty()) return a;
        i128 c = 0;
        for(auto& i : ret){
            c += i;
            i = (ll)(c % radix);
            c /= radix;
        }
        while(c){
            ret.push_back((ll)(c % radix));
            c /= radix;
        }
        while(ret.size() > 1 && !ret.back()) ret.pop_back();
        return make(ret);
    }
};
