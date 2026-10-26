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

class _hash{ // 0-based index
private:
    using pi = pair<int, int>;
    class _core {
    public:
        vector <int> pw, pre, ipw; 
        _mint key, inv; int mod; 
        _core(){};
        _core(int key, int mod) : mod(mod), key(key, mod){ // O(n)
            assert(mod > 1); assert(this->key.v > 0);
            pre.clear(); pw.clear(); ipw.clear();
            pw.push_back(1); ipw.push_back(1); inv = this->key.inv();
        }

        void ensure_pw(int k){ 
            while((int)pw.size() <= k){
                pw.push_back((_mint(pw.back(), mod) * key).v); 
                ipw.push_back((_mint(ipw.back(), mod) * inv).v); 
            }
        }

        int size() const{ return (int)pre.size(); }
        void push_back(int v){ // O(1)
            _mint now = _mint(v, mod); ensure_pw(size());
            if(pre.empty()) pre.push_back(now.v);
            else pre.push_back((_mint(pre.back(), mod) + now * pw[size()]).v);
        }

        void pop_back(){ // O(1)
            assert(!pre.empty());
            pre.pop_back();
        }

        ll ret(int l, int r) const{ // O(1)
            _mint ret = _mint(pre[r], mod);
            if(l > 0) ret -= _mint(pre[l - 1], mod);
            ret *= ipw[l];
            return ret.v;
        }
    }; array <_core, 2> arr; 
public:
    _hash(pi key = {911382323, 972663749}, pi mod = {1000000007, 1000000009}){ clear(key, mod); }
    void clear(pi key = {911382323, 972663749}, pi mod = {1000000007, 1000000009}){ // O(1)
        arr[0] = {key.x, mod.x}; arr[1] = {key.y, mod.y};
    } 

    void reset(){ arr[0].pre.clear(); arr[1].pre.clear(); }
    void push_back(int v){ for(auto& c : arr) c.push_back(v); } // O(1)
    void pop_back(){ for(auto& c : arr) c.pop_back(); } // O(1)
    int size() const{ return arr[0].size(); } // O(1)
    ll ret(int l, int r) const{ // O(1)
        if(l > r) return 0;
        return ((ll)arr[0].ret(l, r) << 32) + arr[1].ret(l, r);
    }

    bool same(int l1, int r1, int l2, int r2) const{ // O(1)
        if(r1 - l1 != r2 - l2) return false;
        if(l1 > r1) return true;
        for(const auto& c : arr) if(c.ret(l1, r1) != c.ret(l2, r2)) return 0;
        return 1;
    }
};

class _deque_hash{ // 0-based index
private:
    class _core{ 
    public:
        vector <int> bit, arr, pw, ipw;
        int mod, key, inv, n, s, e;
        _core() : mod(1), key(0), inv(0), n(0), s(0), e(-1){}
        _core(int n, int key, int mod) : mod(mod), key(key) { // mod == prime
            assert(mod > 1); assert(key > 0);
            inv = _mint(key, mod).inv().v;
            n = 2 * n + 5, s = n / 2; e = s - 1; this->n = n;
            bit.assign(n, 0); arr.assign(n, 0);
            pw.assign(n, 1); ipw.assign(n, 1);
            for(int i = 1;i < n;i++){
                pw[i] = (_mint(pw[i - 1], mod) * key).v;
                ipw[i] = (_mint(ipw[i - 1], mod) * inv).v;
            }
        }       

        void set(int idx, ll v){ // O(log n)
            _mint now(v, mod), diff = now - _mint(arr[idx], mod);
            arr[idx] = now.v; add(idx, (diff * _mint(pw[idx], mod)).v);
        }

        void clear(int idx){ // O(log n)
            _mint diff = _mint(0, mod) - _mint(arr[idx], mod); 
            arr[idx] = 0; add(idx, (diff * _mint(pw[idx], mod)).v);
        }

        void add(int idx, int v){ // O(log n)
            for(int i = idx;i < n;i = (i | (i + 1))){
                bit[i] += v; bit[i] %= mod;
            }
        }

        int sum(int idx) const{ // O(log n)
            int ret = 0;
            for(int i = idx;i >= 0;i = (i & (i + 1)) - 1) ret += bit[i], ret %= mod;
            return ret;
        }

        int size() const{ return e - s + 1; } // O(1)
        void push_back(int v){ set(++e, v); } // O(log n)
        void push_front(int v){ set(--s, v); } // O(log n)
        void pop_back(){ clear(e--); } // O(log n)
        void pop_front(){ clear(s++); } // O(log n)

        int ret(int l, int r) const{ // O(log n)
            l += s; r += s;
            int ret = sum(r);
            if(l) ret -= sum(l - 1);
            if(ret < 0) ret += mod;
            return (_mint(ipw[l], mod) * ret).v;
        }
    }; array <_core, 2> arr; int n;
public:
    using pi = pair<int, int>;
    _deque_hash(int n = 0, pi key = {911382323, 972663749}, pi mod = {1000000007, 1000000009}){ clear(n, key, mod); }
    void clear(int n, pi key = {911382323, 972663749}, pi mod = {1000000007, 1000000009}){ // O(n)
        arr[0] = {n, key.x, mod.x}; arr[1] = {n, key.y, mod.y};
    } 

    void push_back(int v){ for(auto& c : arr) c.push_back(v); } // O(log n)
    void push_front(int v){ for(auto& c : arr) c.push_front(v); } // O(log n)
    void pop_back(){ for(auto& c : arr) c.pop_back(); } // O(log n)
    void pop_front(){ for(auto& c : arr) c.pop_front(); } // O(log n)
    int size() const{ return arr[0].size(); } // O(1)
    ll ret(int l, int r) const{ // O(log n)
        if(l > r) return 0;
        return ((ll)arr[0].ret(l, r) << 32) + arr[1].ret(l, r);
    }

    bool same(int l1, int r1, int l2, int r2) const{ // O(log n)
        if(r1 - l1 != r2 - l2) return false;
        if(l1 > r1) return true;
        for(const auto& c : arr) if(c.ret(l1, r1) != c.ret(l2, r2)) return 0;
        return 1;
    }
};
