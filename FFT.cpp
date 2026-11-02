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

class _fft{
private:
    using cpd = complex<double>;
    size_t n = 0;
    static void transform(vector<cpd>& v, bool inv){ // O(n log n)
        size_t n = v.size();
        if(n < 2) return;
        for(size_t i = 1, j = 0; i < n; i++){
            size_t bit = n >> 1;
            for(; j & bit; bit >>= 1) j ^= bit;
            j ^= bit;
            if(i < j) swap(v[i], v[j]);
        }
        const double pi = acos(-1.0);
        for(size_t k = 1; k < n; k <<= 1){
            double angle = pi / k * (inv ? 1 : -1);
            cpd w(cos(angle), sin(angle));
            for(size_t i = 0; i < n; i += 2 * k){
                cpd wp(1, 0);
                for(size_t j = 0; j < k; j++){
                    cpd left = v[i + j], right = v[i + j + k] * wp;
                    v[i + j] = left + right;
                    v[i + j + k] = left - right;
                    wp *= w;
                }
            }
        }
        if(inv) for(auto& value : v) value /= (double)n;
    }
public:
    class node{
        friend class _fft;
    private:
        vector<cpd> v;
        size_t len;
        node(size_t n, size_t len) : v(n), len(len){}
    public:
        node& operator+=(const node& other){ // O(n)
            for(size_t i = 0; i < v.size(); i++) v[i] += other.v[i];
            len = max(len, other.len);
            return *this;
        }

        node& operator*=(const node& other){ // O(n)
            for(size_t i = 0; i < v.size(); i++) v[i] *= other.v[i];
            len = len && other.len ? len + other.len - 1 : 0;
            return *this;
        }

        friend node operator+(node a, const node& b){ // O(n)
            a += b;
            return a;
        }

        friend node operator*(node a, const node& b){ // O(n)
            a *= b;
            return a;
        }
    };

    _fft(size_t size = 0){ clear(size); } // O(1)
    void clear(size_t size = 0){ n = size ? bit_ceil(size) : 0; } // O(1)

    template<integral T>
    node make(const vector<T>& arr) const{ // O(n log n)
        node ret(n, arr.size());
        for(size_t i = 0; i < arr.size(); i++) ret.v[i] = (double)arr[i];
        transform(ret.v, 0);
        return ret;
    }

    template<integral T = ll>
    vector<T> inverse(const node& value) const{ // O(n log n)
        if(!value.len) return {};
        auto v = value.v; transform(v, 1);
        vector<T> ret(value.len);
        for(size_t i = 0; i < value.len; i++) ret[i] = (T)round(v[i].real());
        return ret;
    }
};
