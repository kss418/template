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

template <int M>
class _trie { // 0-based index
private:
    vector <array<int, M>> adj;
    vector <int> cnt, pass;
    int new_node(){
        adj.push_back(array<int, M>{}); cnt.push_back(0); pass.push_back(0);
        return (int)adj.size() - 1;
    }
    vector <int> tf(const string& s) const{  
        vector <int> ret;
        for(auto& i : s){
            if('a' <= i && i <= 'z') ret.push_back(i - 'a');
            else if('A' <= i && i <= 'Z') ret.push_back(i - 'A');
            else ret.push_back(i - '0');
        }
        return ret;
    }

    int get_state(span<const int> v) const{
        int cur = 0;
        for(auto& c : v){
            assert(0 <= c && c < M);
            if (!adj[cur][c]) return -1;
            cur = adj[cur][c];
        }
        return cur;
    }
public:
    _trie(){ clear(); } // O(1)
    void clear(){ adj.clear(); cnt.clear(); pass.clear(); new_node(); } // O(1)
    void reserve(int max_node){ // O(1)
        adj.reserve(max_node); cnt.reserve(max_node);
        pass.reserve(max_node);
    }

    int size() const{ return pass.empty() ? 0 : pass[0]; } // O(1)
    void insert(const string& s){ auto v = tf(s); insert(v); } // O(|s|)
    void insert(span<const int> v){ // O(|v|)
        int cur = 0; pass[cur]++;
        for(auto &c : v){
            assert(0 <= c && c < M);
            if(!adj[cur][c]) adj[cur][c] = new_node();
            cur = adj[cur][c]; pass[cur]++;
        }
        cnt[cur]++;
    }

    bool erase(const string& s){ auto v = tf(s); return erase(v); } // O(|s|)
    bool erase(span<const int> v){ // O(|v|)
        int state = get_state(v);
        if(state == -1 || !cnt[state]) return 0; 
        int cur = 0; cnt[state]--; pass[cur]--;
        for(auto &c : v){ cur = adj[cur][c]; pass[cur]--; }
        return 1;
    }

    int count(const string& s) const{ auto v = tf(s); return count(v); } // O(|s|)
    int count(span<const int> v) const{ return count(get_state(v)); } // O(|v|)
    int count(int state) const{ return (state == -1 ? 0 : cnt[state]); } // O(1);

    int prefix_count(const string& prefix) const { auto v = tf(prefix); return prefix_count(v); } // O(|s|)
    int prefix_count(span<const int> v) const { return prefix_count(get_state(v)); } // O(|v|)
    int prefix_count(int state) const{ return (state == -1 ? 0 : pass[state]); } // O(1)

    int rank(const string& s) const{ auto v = tf(s); return rank(v); } // O(|s| * m)
    int rank(span<const int> v) const{ // O(|v| * m)
        int ret = 0, cur = 0;
        for(auto& c : v){
            assert(0 <= c && c < M);
            ret += cnt[cur];
            for(int x = 0;x < c;x++){
                int nxt = adj[cur][x];
                if(nxt) ret += pass[nxt];
            }
            int nxt = adj[cur][c];
            if(!nxt) return ret; cur = nxt;
        }
        return ret;
    }

    vector <int> kth(int k) const{ // O(max_len * m)
        if(k <= 0 || k > pass[0]) return {};
        vector <int> ret; int cur = 0;
        while(1){
            if(k <= cnt[cur]) break;
            k -= cnt[cur];
            for(int c = 0;c < M;c++){
                int nxt = adj[cur][c];
                if(!nxt) continue;
                if(pass[nxt] >= k){
                    ret.push_back(c);
                    cur = nxt; break;
                }
                else k -= pass[nxt];
            }
        }
        return ret;
    }

    int go(int state, int c) const{ // O(1)
        if(state < 0 || state >= (int)adj.size()) return -1;
        assert(0 <= c && c < M);
        int nxt = adj[state][c];
        return nxt ? nxt : -1;
    }
};

class _xor_trie { // 0-based index
private:
    vector <array<int, 2>> adj;
    vector <int> cnt, pass; int m;
    int new_node(){
        adj.push_back({0, 0}); cnt.push_back(0); pass.push_back(0);
        return (int)adj.size() - 1;
    }
    int get_state(u64 x) const{
        int cur = 0;
        for(int i = m;i >= 0;i--){
            bool bit = (x >> i) & 1ull; int nxt = adj[cur][bit];
            if(!nxt) return -1; cur = nxt;
        }
        return cur;
    }

    u64 arg(u64 x, bool is_max) const{
        if(!size()) return 0; int cur = 0; u64 ret = 0;
        for(int i = m;i >= 0;i--){
            bool bit = (x >> i) & 1ull, need = bit ^ is_max;
            int nxt = adj[cur][need];
            if(nxt && pass[nxt] > 0){
                ret |= (u64)(need) << i; cur = nxt;
            }
            else{
                ret |= (u64)(need ^ 1) << i;
                cur = adj[cur][need ^ 1];
            }
        }
        return ret;
    }
public:
    _xor_trie(int max_bit = 0){ clear(max_bit); } // O(1)
    void clear(int max_bit){ // O(1)
        this->m = max_bit;
        adj.clear(); cnt.clear(); pass.clear(); new_node();
    }

    void reserve(int max_node){ // O(1)
        adj.reserve(max_node); cnt.reserve(max_node);
        pass.reserve(max_node);
    }

    int size() const { return pass.empty() ? 0 : pass[0]; } // O(1)
    void insert(u64 x){ // O(max_bit)
        int cur = 0; pass[cur]++;
        for(int i = m;i >= 0;i--){
            bool bit = (x >> i) & 1ull;
            if(!adj[cur][bit]) adj[cur][bit] = new_node();
            cur = adj[cur][bit]; pass[cur]++;
        }
        cnt[cur]++;
    }

    bool erase(u64 x){ // O(max_bit)
        int state = get_state(x);
        if(state == -1 || !cnt[state]) return 0; 
        int cur = 0; cnt[state]--; pass[cur]--;
        for(int i = m;i >= 0;i--){
            bool bit = (x >> i) & 1ull;
            cur = adj[cur][bit]; pass[cur]--;
        }
        return 1;
    }

    u64 max(u64 x) const{ return x ^ argmax(x); } // O(max_bit)
    u64 argmax(u64 x) const{ return arg(x, 1); } // O(max_bit)
    u64 min(u64 x) const{ return x ^ argmin(x); } // O(max_bit)
    u64 argmin(u64 x) const{ return arg(x, 0); } // O(max_bit)

    int greater(u64 x, u64 k){ return size() - less(x, k + 1); } // O(max_bit)
    int eqaul(u64 x, u64 k){ return less(x, k + 1) - less(x, k); } // O(max_bit)
    int less(u64 x, u64 k){ // O(max_bit)
        int cur = 0, ret = 0;
        for(int i = m;i >= 0;i--){
            bool kb = (k >> i) & 1ull, xb = (x >> i) & 1ull;
            if(kb){   
                if(adj[cur][xb]) ret += pass[adj[cur][xb]];
                if(!adj[cur][xb ^ 1]) return ret;
            }
            else if(!adj[cur][xb]) return ret;
            cur = adj[cur][xb ^ kb];
        }
        return ret;
    }

    int count(u64 x) const{ return count_state(get_state(x)); } // O(max_bit)
    int count_state(int state) const{ return (state == -1 ? 0 : cnt[state]); } // O(1);
    int sub(u64 x) const{ return sub_state(get_state(x)); } // O(max_bit)
    int sub_state(int state) const{ return (state == -1 ? 0 : pass[state]); } // O(1)

    int go(int state, bool bit) const{ // O(1)
        if(state < 0 || state >= (int)adj.size()) return -1;
        int nxt = adj[state][bit];
        return nxt ? nxt : -1;
    }
};

template <int M = 26>
class _ac{
private:
    vector <array<int, M>> adj;
    vector<int> cnt, terminal_count, f, dep, dict;
    vector <bool> end; bool built;
    vector <int> tf(const string& s) const{
        vector <int> ret; ret.reserve(s.size());
        for(auto& i : s){
            if('a' <= i && i <= 'z') ret.push_back(i - 'a');
            else if('A' <= i && i <= 'Z') ret.push_back(i - 'A');
            else ret.push_back(i - '0');
        }
        return ret;
    }

    int new_node(){
        adj.push_back(array<int, M>{}); f.push_back(0); cnt.push_back(0); terminal_count.push_back(0);
        end.push_back(0); dict.push_back(0); dep.push_back(0);
        return (int)adj.size() - 1;
    }
public:
    _ac(){ clear(); } // O(1)
    void clear(){ // O(1)
        built = 0; adj.clear(); end.clear(); dep.clear();
        f.clear(); cnt.clear(); terminal_count.clear(); dict.clear(); new_node();
    }
    
    void reserve(int max_node){ // O(1)
        adj.reserve(max_node); f.reserve(max_node); dep.reserve(max_node);
        cnt.reserve(max_node); terminal_count.reserve(max_node); end.reserve(max_node); dict.reserve(max_node);
    }

    const vector<int>& depth() const{ return dep; }
    const vector<bool>& terminal() const{ return end; }
    int size() const{ return (int)adj.size(); } // O(1)
    void insert(const string& s){ auto v = tf(s); insert(v); } // O(|s|)
    void insert(span<const int> v){ // O(|v|)
        int cur = 0;
        for(auto &c : v){
            assert(0 <= c && c < M);
            int nxt = adj[cur][c];
            if(!nxt || dep[nxt] != dep[cur] + 1){
                nxt = new_node();
                adj[cur][c] = nxt;
                dep[nxt] = dep[cur] + 1;
            }
            cur = adj[cur][c];
        }
        end[cur] = 1; terminal_count[cur]++;
    }
    
    void build(){ // O(max_node * m)
        cnt = terminal_count; fill(all(f), 0); fill(all(dict), 0);
        for(int cur = 0; cur < (int)adj.size(); ++cur) for(int c = 0; c < M; ++c){
            int nxt = adj[cur][c];
            if(nxt && dep[nxt] != dep[cur] + 1) adj[cur][c] = 0;
        }
        built = 1; deque <int> q;
        for(int c = 0;c < M;c++){
            int nxt = adj[0][c];
            if(!nxt) continue;
            f[nxt] = 0; dict[nxt] = 0; q.push_back(nxt);
        }
        
        while(!q.empty()){
            int cur = q.front(); q.pop_front();
            cnt[cur] += cnt[f[cur]];
            dict[cur] = end[f[cur]] ? f[cur] : dict[f[cur]];
            for(int c = 0;c < M;c++){
                int nxt = adj[cur][c];
                if(!nxt) adj[cur][c] = adj[f[cur]][c];
                else{
                    f[nxt] = adj[f[cur]][c];
                    q.push_back(nxt);
                }
            }
        }
    }

    ll count(const string& s) const{ auto v = tf(s); return count(v); } // O(|s|)
    ll count(span<const int> v) const{ // O(|v|)
        ll ret = 0;
        it_state(v, [&](int st){ ret += cnt[st]; });
        return ret;
    }

    int go(int state, int c) const{ // O(1)
        assert(0 <= c && c < M);
        return adj[state][c];
    }

    template <class F> 
    void it_state(const string& s, const F& f) const{ auto v = tf(s); it_state(v, f); } // O(|s|)
    template <class F> 
    void it_state(span<const int> v, const F& f) const{ // O(|v|)
        int cur = 0;
        for(auto& c : v){
            assert(0 <= c && c < M);
            cur = go(cur, c), f(cur);
        }
    }

    int max_match_len(int state) const{ // O(1)

        return end[state] ? dep[state] : (dict[state] ? dep[dict[state]] : 0);
    }

    template <class F>
    void it_match_lens(int state, const F& f) const{ it_match_lens(state, 0x3f3f3f3f, f); } // O(min(k, matches))
    template <class F>
    void it_match_lens(int state, int k, const F& f) const{ // O(min(k, matches))
        if(k <= 0) return; int cnt = 0;
        if(end[state]){
            f(dep[state]); if(++cnt >= k) return;
        }
        for(int x = dict[state];x > 0 && cnt < k;x = dict[x]) f(dep[x]), ++cnt;
    }
};

template <class T = ll>
class _sparse_ac{
private:
    vector <ll> cnt, terminal_count;
    vector <int> f, dep, dict, head, to, next;
    vector <T> ch; vector <bool> end; bool built;
    
    int new_node(){
        f.push_back(0); cnt.push_back(0); terminal_count.push_back(0); head.push_back(0);
        end.push_back(0); dict.push_back(0); dep.push_back(0);
        return (int)head.size() - 1;
    }

    void add_edge(int cur, const T& c, int nxt){
        to.push_back(nxt); ch.push_back(c);
        next.push_back(head[cur]);
        head[cur] = (int)to.size() - 1;
    }

    int child(int state, const T& c) const{
        for(int e = head[state];e > 0;e = next[e]) if(ch[e] == c) return to[e];
        return 0;
    }

    int set_child(int state, const T& c){
        int nxt = child(state, c);
        if(nxt) return nxt;
        int ret = new_node(); add_edge(state, c, ret);
        return ret;
    }
public:
    _sparse_ac(){ clear(); } // O(1)
    void clear(){ // O(1)
        built = 0; end.clear(); dep.clear();
        f.clear(); dict.clear(); cnt.clear(); terminal_count.clear();
        to.clear(); next.clear(); ch.clear(); head.clear();
        to.push_back(0); next.push_back(0); ch.push_back(T{});
        new_node();
    }

    vector <T> tf(const string& s) const{
        vector <T> ret; ret.reserve(s.size());
        for(unsigned char c : s) ret.push_back((T)c);
        return ret;
    }
    
    void reserve(int max_node, int max_edge = -1){ // O(1)
        f.reserve(max_node); dep.reserve(max_node); head.reserve(max_node);
        end.reserve(max_node); dict.reserve(max_node); cnt.reserve(max_node); terminal_count.reserve(max_node);
        if(max_edge == -1) return;
        to.reserve(max_edge + 1); next.reserve(max_edge + 1); ch.reserve(max_edge + 1);
    }

    const vector<int>& depth() const{ return dep; } // O(1)
    const vector<bool>& terminal() const{ return end; } // O(1)
    int size() const{ return (int)head.size(); } // O(1)
    void insert(const string& s){ auto v = tf(s); insert(v); } // O(|s|)
    void insert(span<const T> v){ // O(|v|)
        int cur = 0;
        for(auto &c : v){
            int nxt = child(cur, c);
            if(!nxt){
                nxt = set_child(cur, c);
                dep[nxt] = dep[cur] + 1;
            }
            cur = nxt;
        }
        end[cur] = 1; terminal_count[cur]++;
    }
    
    void build(){ 
        cnt = terminal_count; fill(all(f), 0); fill(all(dict), 0);
        built = 1; deque <int> q;
        for(int e = head[0]; e > 0; e = next[e]){
            int nxt = to[e]; f[nxt] = dict[nxt] = 0;
            q.push_back(nxt);
        }
        
        while(!q.empty()){
            int cur = q.front(); q.pop_front();
            cnt[cur] += cnt[f[cur]];
            dict[cur] = end[f[cur]] ? f[cur] : dict[f[cur]];
            for(int e = head[cur];e > 0;e = next[e]){
                auto c = ch[e]; int p = f[cur], u = child(p, c);
                while(p && !u) p = f[p], u = child(p, c);
                int nxt = to[e]; f[nxt] = u; q.push_back(nxt);
            }
        }
    }

    ll count(const string& s) const{ auto v = tf(s); return count(v); } // O(|s|)
    ll count(span<const T> v) const{ // O(|v|)
        ll ret = 0;
        it_state(v, [&](int st){ ret += cnt[st]; });
        return ret;
    }

    int go(int state, const T& c) const{

        int nxt = child(state, c);
        while(state && !nxt) state = f[state], nxt = child(state, c);
        return nxt;
    }

    template <class F>
    void it_state(const string& s, const F& f) const{ auto v = tf(s); it_state(v, f); } // O(|s|)
    template <class F>
    void it_state(span<const T> v, const F& f) const{ // O(|v|)
        int cur = 0;
        for(auto& c : v){
            cur = go(cur, c), f(cur);
        }
    }

    int max_match_len(int state) const{ // O(1)

        return end[state] ? dep[state] : (dict[state] ? dep[dict[state]] : 0);
    }

    template <class F>
    void it_match_lens(int state, const F& f) const{ it_match_lens(state, 0x3f3f3f3f, f); } // O(min(k, matches))
    template <class F>
    void it_match_lens(int state, int k, const F& f) const{ // O(min(k, matches))
        if(k <= 0) return; int cnt = 0;
        if(end[state]){
            f(dep[state]); if(++cnt >= k) return;
        }
        for(int x = dict[state];x > 0 && cnt < k;x = dict[x]) f(dep[x]), ++cnt;
    }
};
