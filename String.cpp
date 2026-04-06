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

//KMP
template <class T = ll>
class _kmp{ // 0-based index
private:
    int m;
    vector <T> pat; vector <int> arr;
    void build(){
        m = (int)pat.size(); int nxt = 0; arr.assign(m, 0);
        for(int cur = 1;cur < m;cur++){
            while(nxt && !build_chk(cur, nxt)) nxt = arr[nxt - 1];
            if(build_chk(cur, nxt)) arr[cur] = ++nxt;
        }
    }

    bool build_chk(int cur, int nxt) const{
        return pat[cur] == pat[nxt];
    }

    bool match_chk(span<const T> v, int idx, int state) const{
        return v[idx] == pat[state];
    }

    static vector <T> tf(const string& s){
        vector <T> ret; ret.reserve(s.size());
        for(unsigned char c : s) ret.push_back(T(c));
        return ret;
    }
public:
    _kmp() : m(0){}
    _kmp(const string& s){ set(s); } // O(|s|)
    _kmp(span<const T> v){ set(v); } // O(|v|)
    void clear(){ pat.clear(); arr.clear(); m = 0; } // O(1)
    void set(const string& s){ pat = tf(s); build(); } // O(|s|)
    void set(span<const T> v){ pat.assign(all(v)); build(); } // O(|v|)

    const vector<int>& fail() const{ return arr; } // O(1)
    template <class F>
    void it_match(const string& s, const F& f) const{ it_match(tf(s), f);} // O(|s| + m)
    template <class F>
    void it_match(span<const T> v, const F& f) const{ // O(|v| + m)
        int n = (int)v.size();
        if(!m) return; int state = 0;
        for(int i = 0;i < n;i++){
            state = go(state, v, i);
            if(state == m) f(i - m + 1);
        }
    }

    int go(int state, span<const T> v, int idx) const{ // amortized O(1)
        if(!m) return 0;
        if(state == m) state = arr[m - 1];
        while(state && !match_chk(v, idx, state)) state = arr[state - 1];
        if(match_chk(v, idx, state)) state++;
        return state;
    }

    template <class F>
    void it_border(const F& f) const{ // O(m)
        if(!m) return;
        for(int x = arr[m - 1];x > 0;x = arr[x - 1]) f(x);
    }
};

//SUFFIX-ARRAY
template <class T = ll>
class _sa{
private:
    vector <T> arr; vector <int> sa, pos, lcp, cnt, tmp_sa; int n, d; 
    void build(){ build_sa(); build_lcp(); }
    template <class F>
    void counting_sort(int sz, const F& f){
        cnt.assign(sz, 0);
        for(int i = 0;i < n;i++) cnt[f(sa[i])]++;
        for(int i = 1;i < sz;i++) cnt[i] += cnt[i - 1];
        for(int i = n - 1;i >= 0;i--) tmp_sa[--cnt[f(sa[i])]] = sa[i];
        sa.swap(tmp_sa);
    }

    void build_sa(){
        vector <int> tmp(n); vector <pair<T, int>> num; 
        for(int i = 0;i < n;i++) num.push_back({arr[i], i});
        sort(all(num)); pos[num[0].y] = 0;
        tmp_sa.resize(n); int r = 0;
        for(int i = 1; i < n; i++){
            if(num[i].x != num[i - 1].x) r++;
            pos[num[i].y] = r;
        }
        for(d = 1; ;d <<= 1){
            int sz = r + 1;
            counting_sort(sz + 1, [&](int i){ return i + d < n ? pos[i + d] + 1 : 0; });
            counting_sort(sz + 1, [&](int i){ return pos[i] + 1; });
            tmp[sa[0]] = 0; int nr = 0;
            for(int i = 1;i < n;i++){
                int pre = sa[i - 1], cur = sa[i];
                int p1 = pos[pre], c1 = pos[cur];
                int p2 = pre + d < n ? pos[pre + d] : -1;
                int c2 = cur + d < n ? pos[cur + d] : -1;
                if(p1 != c1 || p2 != c2) nr++; tmp[cur] = nr;
            }
            for(int i = 0;i < n;i++) pos[i] = tmp[i]; r = nr;
            if(r == n - 1) break;
        }
    }

    void build_lcp(){
        int k = 0;
        for(int cur = 0;cur < n;cur++){
            k = max(k - 1, 0);
            if(pos[cur] == n - 1) continue;
            int nxt = sa[pos[cur] + 1];
            while(cur + k < n && nxt + k < n && arr[cur + k] == arr[nxt + k]) k++;
            lcp[pos[cur]] = k;
        }
    }
public:
    _sa(){ clear(0); } // O(1)
    _sa(const string& s){ set(s); } // O(n log n)
    _sa(span<const T> v) { set(v); } // O(n log n)
    void set(const string& s){ clear(s.size()); for(auto& i : s) arr.push_back((T)i); build(); } // O(n log n)
    void set(span<const T> v){ clear(v.size()); this->arr.assign(all(v)); build(); } // O(n log n)

    void clear(int n){ // O(n)
        this->n = n; d = 1; sa.assign(n, 0); arr.clear(); arr.reserve(n);
        iota(all(sa), 0); lcp.assign(n, 0); pos.assign(n, 0);
    }

    const vector<int>& get_sa() const{ return sa; } // O(1)
    const vector<int>& get_pos() const{ return pos; } // O(1)
    const vector<int>& get_lcp() const{ return lcp; } // O(1)
}; 

//MANACHER
template <class T = ll>
class _mnc { // 0-based index
private:
    vector <T> arr; vector <int> rad;
    int n, m = 0; ll even = 0, odd = 0;
    void build(){
        this->n = arr.size(); rad.assign(n, 0);
        int r = -1, c = -1; even = odd = m = 0;
        for(int i = 0;i < n;i++){
            if(i <= r) rad[i] = min(r - i, rad[2 * c - i]);
            while(i + rad[i] + 1 < n && i - rad[i] - 1 >= 0){
                if(arr[i + rad[i] + 1] == arr[i - rad[i] - 1]) rad[i]++;
                else break;
            }
            if(i + rad[i] > r) c = i, r = i + rad[i];
            (arr[i] == inf() ? even : odd) += rad[i] / 2 + (arr[i] != inf());
            m = max(m, rad[i]);
        }
    }

    static constexpr T inf() {
        if constexpr (is_same_v<T, int>) return 0x3f3f3f3f;       
        else if constexpr (is_same_v<T, char>) return (char)255;
        else return INF; 
    }

    template <class It, class F>
    void build(It begin, It end, const F& f){
        size_t len = (size_t)distance(begin, end);
        arr.clear(); arr.reserve(2 * len + 1);
        const T sep = inf();
        for(auto it = begin;it != end;++it){
            arr.push_back(sep); arr.push_back(f(*it));
        }
        arr.push_back(sep); build();
    }
public:
    _mnc(){} // O(1)
    _mnc(const string& s){ set(s); } // O(n)
    _mnc(span<const T> v){ set(v); } // O(n)
    void set(const string& s){ build(all(s), [&](char c){ return (unsigned char)c; }); } // O(n)
    void set(span<const T> v){ build(all(v), [&](const T& x){ return x; }); } // O(n)

    const vector<int>& get_rad() const{ return rad; } // O(1)
    ll cnt() const{ return odd + even; } // O(1)
    ll cnt_odd() const{ return odd; } // O(1)
    ll cnt_even() const{ return even; } // O(1)
    int max_len() const{ return m; } // O(1)
    bool is_pal(int l, int r) const{ // O(1)
        int m = l + r;
        return rad[m] >= r - l;
    }

    pair<int, int> range_odd(int idx) const{ // O(1)
        int r = rad[2 * idx + 1] / 2;
        return { idx - r, idx + r };
    }
    
    // center == (idx - 1, idx)
    pair<int, int> range_even(int idx) const{ // O(1)
        int r = rad[2 * idx] / 2;
        return { idx - r, idx + r - 1 };
    }
};

//z
template <class T = ll>
class _z { // 0-based index
private:
    vector<T> arr; vector<int> z; int n = 0;
    void build(){
        n = (int)arr.size(); z.assign(n, 0);
        if(!n) return;
        z[0] = n; int l = 0, r = 0;
        for(int i = 1; i < n; i++){
            if(i <= r) z[i] = min(r - i + 1, z[i - l]);
            while(i + z[i] < n && arr[z[i]] == arr[i + z[i]]) z[i]++;
            if(i + z[i] - 1 > r){ l = i; r = i + z[i] - 1; }
        }
    }

    static vector<T> tf(const string& s){
        vector<T> ret; ret.reserve(s.size());
        for(unsigned char c : s) ret.push_back((T)c);
        return ret;
    }
public:
    _z(){}
    _z(const string& s){ set(s); } // O(|s|)
    _z(span<const T> v){ set(v); } // O(|v|)
    void clear(){ arr.clear(); z.clear(); n = 0; } // O(1)
    void set(const string& s){ arr = tf(s); build(); } // O(|s|)
    void set(span<const T> v){ arr.assign(all(v)); build(); } // O(|v|)
    const vector<int>& result() const{ return z; } // O(1)
};
