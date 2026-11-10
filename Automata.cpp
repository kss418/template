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

namespace alphabet {
    class lowercase{
    public:
        static constexpr int size = 26;
        static constexpr int encode(unsigned char c){
            return c - 'a';
        }
        static constexpr unsigned char decode(int i){
            return 'a' + i;
        }
    };

    class uppercase{
    public:
        static constexpr int size = 26;
        static constexpr int encode(unsigned char c){
            return c - 'A';
        }
        static constexpr unsigned char decode(int i){
            return 'A' + i;
        }
    };

    class digits{
    public:
        static constexpr int size = 10;
        static constexpr int encode(unsigned char c){
            return c - '0';
        }
        static constexpr unsigned char decode(int i){
            return '0' + i;
        }
    };

    class alnum{
    public:
        static constexpr int size = 62;
        static constexpr int encode(unsigned char c){
            if('0' <= c && c <= '9') return c - '0';
            if('A' <= c && c <= 'Z') return 10 + c - 'A';
            return 36 + c - 'a';
        }
        static constexpr unsigned char decode(int i){
            if(i < 10) return '0' + i;
            if(i < 36) return 'A' + i - 10;
            return 'a' + i - 36;
        }
    };

    class bytes{
    public:
        static constexpr int size = 256;
        static constexpr int encode(unsigned char c){ return c; }
        static constexpr unsigned char decode(int i){
            return static_cast<unsigned char>(i);
        }
    };
}

template <class Alphabet = alphabet::lowercase>
class _trie { // 0-based index
private:
    vector <array<int, Alphabet::size>> adj;
    vector <int> cnt, pass;
    int new_node(){
        adj.push_back(array<int, Alphabet::size>{}); cnt.push_back(0); pass.push_back(0);
        return (int)adj.size() - 1;
    }
    vector <int> tf(const string& s) const{
        vector <int> ret; ret.reserve(s.size());
        for(unsigned char c : s) ret.push_back(Alphabet::encode(c));
        return ret;
    }
public:
    class state{
    private:
        friend class _trie;
        const _trie* owner;
        int idx;
        state(const _trie* owner, int idx) : owner(owner), idx(idx){}
    public:
        int id() const{ return idx; } // O(1)
        int count() const{ return idx == -1 ? 0 : owner->cnt[idx]; } // O(1)
        int prefix_count() const{ return idx == -1 ? 0 : owner->pass[idx]; } // O(1)
        bool end() const{ return count() > 0; } // O(1)
        bool valid() const{ return id() != -1; } // O(1)
        bool has_child() const{ return prefix_count() > count(); } // O(1)

        state go(int c) const{ // O(1)
            if(idx < 0 || idx >= (int)owner->adj.size()) return {owner, -1};
            int nxt = owner->adj[idx][c];
            return {owner, nxt ? nxt : -1};
        }

        template<class F>
        void it_child(const F& f) const{ // O(Alphabet::size)
            if(idx < 0 || idx >= (int)owner->adj.size()) return;
            for(int c = 0;c < Alphabet::size;c++){
                int nxt = owner->adj[idx][c];
                if(nxt && owner->pass[nxt]) f(c, state(owner, nxt));
            }
        }

        template<class F>
        void it_subtree(const F& f) const{ // O(k * Alphabet::size)
            if(idx < 0 || idx >= (int)owner->adj.size() || !owner->pass[idx]) return;
            f(*this);
            it_child([&](int, state nxt){ nxt.it_subtree(f); });
        }
    };

    _trie(){ clear(); } // O(1)
    void clear(){ adj.clear(); cnt.clear(); pass.clear(); new_node(); } // O(1)

    state get_state(const string& s) const{ auto v = tf(s); return get_state(v); } // O(|s|)
    state get_state(span<const int> v) const{ // O(|v|)
        int cur = 0;
        for(auto& c : v){
            if (!adj[cur][c]) return {this, -1};
            cur = adj[cur][c];
        }
        return {this, cur};
    }

    int size() const{ return pass.empty() ? 0 : pass[0]; } // O(1)
    state insert(const string& s){ auto v = tf(s); return insert(v); } // O(|s|)
    state insert(span<const int> v){ // O(|v|)
        int cur = 0; pass[cur]++;
        for(auto &c : v){
            if(!adj[cur][c]) adj[cur][c] = new_node();
            cur = adj[cur][c]; pass[cur]++;
        }
        cnt[cur]++; return {this, cur};
    }

    bool erase(const string& s){ auto v = tf(s); return erase(v); } // O(|s|)
    bool erase(span<const int> v){ // O(|v|)
        int state = get_state(v).id();
        if(state == -1 || !cnt[state]) return 0; 
        int cur = 0; cnt[state]--; pass[cur]--;
        for(auto &c : v){ cur = adj[cur][c]; pass[cur]--; }
        return 1;
    }

    int rank(const string& s) const{ auto v = tf(s); return rank(v); } // O(|s| * Alphabet::size)
    int rank(span<const int> v) const{ // O(|v| * Alphabet::size)
        int ret = 0, cur = 0;
        for(auto& c : v){
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

    vector <int> kth(int k) const{ // O(max_len * Alphabet::size)
        if(k <= 0 || k > pass[0]) return {};
        vector <int> ret; int cur = 0;
        while(1){
            if(k <= cnt[cur]) break;
            k -= cnt[cur];
            for(int c = 0;c < Alphabet::size;c++){
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
};

class _xor_trie { // 0-based index
private:
    vector <array<int, 2>> adj;
    vector <int> pass; int m; u64 tag;
    int new_node(){
        adj.push_back({0, 0}); pass.push_back(0);
        return (int)adj.size() - 1;
    }

    u64 arg(u64 x, bool is_max) const{
        if(!size()) return 0; int cur = 0; u64 ret = 0;
        x ^= tag;
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
        return ret ^ tag;
    }
public:
    class state{
    private:
        friend class _xor_trie;
        const _xor_trie* owner;
        int idx, bit;
        state(const _xor_trie* owner, int idx, int bit) : owner(owner), idx(idx), bit(bit){}
    public:
        int id() const{ return idx; } // O(1)
        int count() const{ return idx == -1 || bit >= 0 ? 0 : owner->pass[idx]; } // O(1)
        int prefix_count() const{ return idx == -1 ? 0 : owner->pass[idx]; } // O(1)
        bool end() const{ return count() > 0; } // O(1)
        bool valid() const{ return id() != -1; } // O(1)
        bool has_child() const{ return prefix_count() > count(); } // O(1)

        state go(bool c) const{ // O(1)
            if(idx < 0 || idx >= (int)owner->adj.size() || bit < 0) return {owner, -1, -1};
            bool flip = (owner->tag >> bit) & 1ull;
            int nxt = owner->adj[idx][c ^ flip];
            return {owner, nxt ? nxt : -1, bit - 1};
        }

        template<class F>
        void it_child(const F& f) const{ // O(1)
            if(idx < 0 || idx >= (int)owner->adj.size() || bit < 0) return;
            bool flip = (owner->tag >> bit) & 1ull;
            for(int c = 0;c < 2;c++){
                int nxt = owner->adj[idx][c ^ flip];
                if(nxt && owner->pass[nxt]) f(c, state(owner, nxt, bit - 1));
            }
        }

        template<class F>
        void it_subtree(const F& f) const{ // O(k)
            if(idx < 0 || idx >= (int)owner->adj.size() || !owner->pass[idx]) return;
            f(*this);
            it_child([&](int, state nxt){ nxt.it_subtree(f); });
        }
    };

    _xor_trie(int max_bit = 63){ clear(max_bit); } // O(1)
    void clear(int max_bit){ // O(1)
        this->m = max_bit; tag = 0;
        adj.clear(); pass.clear(); new_node();
    }

    void xor_all(u64 mask){ tag ^= mask; } // O(1)

    state root() const{ return {this, 0, m}; } // O(1)
    state get_state(u64 x) const{ // O(max_bit)
        x ^= tag;
        int cur = 0;
        for(int i = m;i >= 0;i--){
            bool bit = (x >> i) & 1ull; int nxt = adj[cur][bit];
            if(!nxt) return {this, -1, -1}; cur = nxt;
        }
        return {this, cur, -1};
    }

    int size() const { return pass.empty() ? 0 : pass[0]; } // O(1)
    state insert(u64 x){ // O(max_bit)
        x ^= tag;
        int cur = 0; pass[cur]++;
        for(int i = m;i >= 0;i--){
            bool bit = (x >> i) & 1ull;
            if(!adj[cur][bit]) adj[cur][bit] = new_node();
            cur = adj[cur][bit]; pass[cur]++;
        }
        return {this, cur, -1};
    }

    bool erase(u64 x){ // O(max_bit)
        int state = get_state(x).id();
        if(state == -1 || !pass[state]) return 0;
        x ^= tag;
        int cur = 0; pass[cur]--;
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

    u64 kth(u64 x, int k) const{ return x ^ argkth(x, k); } // O(max_bit + 1)
    u64 argkth(u64 x, int k) const{ // O(max_bit + 1)
        x ^= tag;
        int cur = 0; u64 ret = 0;
        for(int i = m;i >= 0;i--){
            bool bit = (x >> i) & 1ull;
            int nxt = adj[cur][bit], num = nxt ? pass[nxt] : 0;
            if(k > num){ k -= num; bit = !bit; }
            cur = adj[cur][bit]; ret |= (u64)bit << i;
        }
        return ret ^ tag;
    }

    int greater(u64 x, u64 k) const{ return k == numeric_limits<u64>::max() ? 0 : size() - less(x, k + 1); } // O(max_bit)
    int equal(u64 x, u64 k) const{ return k == numeric_limits<u64>::max() ? size() - less(x, k) : less(x, k + 1) - less(x, k); } // O(max_bit)
    int less(u64 x, u64 k) const{ // O(max_bit)
        x ^= tag;
        if(m < 63){
            u64 high_x = x >> (m + 1), high_k = k >> (m + 1);
            if(high_x != high_k) return high_x < high_k ? size() : 0;
        }
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
};

template <class Alphabet = alphabet::lowercase>
class _ac{
private:
    vector <array<int, Alphabet::size>> adj;
    vector<int> cnt, terminal_count, f, dep, dict;
    vector <bool> end; bool built;
    vector <int> tf(const string& s) const{
        vector <int> ret; ret.reserve(s.size());
        for(unsigned char c : s) ret.push_back(Alphabet::encode(c));
        return ret;
    }

    int new_node(){
        adj.push_back(array<int, Alphabet::size>{}); f.push_back(0); cnt.push_back(0); terminal_count.push_back(0);
        end.push_back(0); dict.push_back(0); dep.push_back(0);
        return (int)adj.size() - 1;
    }
public:
    class state{
    private:
        friend class _ac;
        const _ac* owner;
        int idx;
        state(const _ac* owner, int idx) : owner(owner), idx(idx){}
    public:
        int id() const{ return idx; } // O(1)
        ll terminal_count() const{ return owner->terminal_count[idx]; } // O(1)
        ll match_count() const{ return owner->cnt[idx]; } // O(1)
        state go(int c) const{ return {owner, owner->adj[idx][c]}; } // O(1)
        int max_match_len() const{ // O(1)
            return owner->end[idx] ? owner->dep[idx] : (owner->dict[idx] ? owner->dep[owner->dict[idx]] : 0);
        }

        template<class F>
        void it_match_lens(const F& f) const{ it_match_lens(0x3f3f3f3f, f); } // O(matches)

        template<class F>
        void it_match_lens(int k, const F& f) const{ // O(min(k, matches))
            if(k <= 0) return; int cnt = 0;
            if(owner->end[idx]){
                f(owner->dep[idx]); if(++cnt >= k) return;
            }
            for(int cur = owner->dict[idx];cur > 0 && cnt < k;cur = owner->dict[cur]) f(owner->dep[cur]), ++cnt;
        }
    };

    _ac(){ clear(); } // O(1)
    void clear(){ // O(1)
        built = 0; adj.clear(); end.clear(); dep.clear();
        f.clear(); cnt.clear(); terminal_count.clear(); dict.clear(); new_node();
    }
    
    void reserve(int max_node){ // O(1)
        adj.reserve(max_node); f.reserve(max_node); dep.reserve(max_node);
        cnt.reserve(max_node); terminal_count.reserve(max_node); end.reserve(max_node); dict.reserve(max_node);
    }

    state root() const{ return {this, 0}; } // O(1)
    const vector<int>& depth() const{ return dep; }
    const vector<bool>& terminal() const{ return end; }
    int size() const{ return (int)adj.size(); } // O(1)
    void insert(const string& s){ auto v = tf(s); insert(v); } // O(|s|)
    void insert(span<const int> v){ // O(|v|)
        int cur = 0;
        for(auto &c : v){
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
    
    void build(){ // O(max_node * Alphabet::size)
        cnt = terminal_count; fill(all(f), 0); fill(all(dict), 0);
        for(int cur = 0; cur < (int)adj.size(); ++cur) for(int c = 0; c < Alphabet::size; ++c){
            int nxt = adj[cur][c];
            if(nxt && dep[nxt] != dep[cur] + 1) adj[cur][c] = 0;
        }
        built = 1; deque <int> q;
        for(int c = 0;c < Alphabet::size;c++){
            int nxt = adj[0][c];
            if(!nxt) continue;
            f[nxt] = 0; dict[nxt] = 0; q.push_back(nxt);
        }
        
        while(!q.empty()){
            int cur = q.front(); q.pop_front();
            cnt[cur] += cnt[f[cur]];
            dict[cur] = end[f[cur]] ? f[cur] : dict[f[cur]];
            for(int c = 0;c < Alphabet::size;c++){
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
        it_state(v, [&](const state& st){ ret += st.match_count(); });
        return ret;
    }

    template <class F> 
    void it_state(const string& s, const F& f) const{ auto v = tf(s); it_state(v, f); } // O(|s|)
    template <class F> 
    void it_state(span<const int> v, const F& f) const{ // O(|v|)
        auto cur = root();
        for(auto& c : v){
            cur = cur.go(c); f(cur);
        }
    }
};

template <class T = ll, class Alphabet = alphabet::bytes>
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
    class state{
    private:
        friend class _sparse_ac;
        const _sparse_ac* owner;
        int idx;
        state(const _sparse_ac* owner, int idx) : owner(owner), idx(idx){}
    public:
        int id() const{ return idx; } // O(1)
        ll terminal_count() const{ return owner->terminal_count[idx]; } // O(1)
        ll match_count() const{ return owner->cnt[idx]; } // O(1)
        state go(const T& c) const{
            int cur = idx, nxt = owner->child(cur, c);
            while(cur && !nxt) cur = owner->f[cur], nxt = owner->child(cur, c);
            return {owner, nxt};
        }
        int max_match_len() const{ // O(1)
            return owner->end[idx] ? owner->dep[idx] : (owner->dict[idx] ? owner->dep[owner->dict[idx]] : 0);
        }

        template<class F>
        void it_match_lens(const F& f) const{ it_match_lens(0x3f3f3f3f, f); } // O(matches)

        template<class F>
        void it_match_lens(int k, const F& f) const{ // O(min(k, matches))
            if(k <= 0) return; int cnt = 0;
            if(owner->end[idx]){
                f(owner->dep[idx]); if(++cnt >= k) return;
            }
            for(int cur = owner->dict[idx];cur > 0 && cnt < k;cur = owner->dict[cur]) f(owner->dep[cur]), ++cnt;
        }
    };

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
        for(unsigned char c : s) ret.push_back(static_cast<T>(Alphabet::encode(c)));
        return ret;
    }
    
    void reserve(int max_node, int max_edge = -1){ // O(1)
        f.reserve(max_node); dep.reserve(max_node); head.reserve(max_node);
        end.reserve(max_node); dict.reserve(max_node); cnt.reserve(max_node); terminal_count.reserve(max_node);
        if(max_edge == -1) return;
        to.reserve(max_edge + 1); next.reserve(max_edge + 1); ch.reserve(max_edge + 1);
    }

    state root() const{ return {this, 0}; } // O(1)
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
        it_state(v, [&](const state& st){ ret += st.match_count(); });
        return ret;
    }

    template <class F>
    void it_state(const string& s, const F& f) const{ auto v = tf(s); it_state(v, f); } // O(|s|)
    template <class F>
    void it_state(span<const T> v, const F& f) const{ // O(|v|)
        auto cur = root();
        for(auto& c : v){
            cur = cur.go(c); f(cur);
        }
    }
};
