#include <bits/stdc++.h>
namespace numeric_templates {
#include "../Number_Theory.cpp"
}
namespace geometry {
using numeric_templates::_mint;
#include "../Geometry.cpp"
}
namespace automata {
#include "../Automata.cpp"
}
namespace strings {
#include "../String.cpp"
}
namespace graph_sets {
#include "../DSU.cpp"
}
namespace graphs {
using graph_sets::_uf;
#include "../Graph.cpp"
}
using namespace std;
static int checks = 0;
#define CHECK(c) do { ++checks; if (!(c)) throw runtime_error(string("line ") + to_string(__LINE__) + ": " + #c); } while (0)

void check_manacher(){
    strings::_mnc<> ab(string("ab")), aba(string("aba"));
    CHECK(!ab.is_pal(0, 1)); CHECK(aba.is_pal(0, 2));
    for(int len = 0; len <= 8; ++len) for(int mask = 0; mask < (1 << len); ++mask){
        string text(len, 'a');
        for(int i = 0; i < len; ++i) text[i] += (mask >> i) & 1;
        strings::_mnc<> m(text);
        for(int l = 0; l < len; ++l) for(int r = l; r < len; ++r){
            string part = text.substr(l, r - l + 1), reverse_part = part;
            reverse(reverse_part.begin(), reverse_part.end());
            CHECK(m.is_pal(l, r) == (part == reverse_part));
        }
    }
}

void check_pollard_one(){
    numeric_templates::_pr p;
    CHECK(p.factorize_cnt(1).empty());
    CHECK(p.phi(1) == 1); CHECK(p.divisor_num(1) == 1); CHECK(p.divisor_sum(1) == 1);
    for(uint64_t n = 1; n <= 80; ++n){
        uint64_t phi = 0, count = 0, sum = 0;
        for(uint64_t d = 1; d <= n; ++d){
            phi += gcd(n, d) == 1;
            if(n % d == 0){ ++count; sum += d; }
        }
        CHECK(p.phi(n) == phi); CHECK(p.divisor_num(n) == count); CHECK(p.divisor_sum(n) == sum);
    }
}

void check_line_same(){
    using P = geometry::pll;
    geometry::_line v(P{0,0}, P{0,1}), h(P{0,0}, P{1,0});
    CHECK(!v.same(h)); CHECK(!h.same(v));
    vector<pair<P,P>> ends = {
        {{0,0},{0,1}}, {{0,3},{0,-2}}, {{1,0},{1,1}},
        {{0,0},{1,0}}, {{3,0},{-2,0}}, {{0,1},{1,1}},
        {{0,0},{1,1}}, {{2,2},{-1,-1}}, {{0,1},{1,2}}, {{0,0},{1,-1}}
    };
    auto cross = [](P a, P b, P c){
        return (b.first-a.first)*(c.second-a.second) - (b.second-a.second)*(c.first-a.first);
    };
    for(auto [a,b] : ends) for(auto [c,d] : ends){
        geometry::_line first(a,b), second(c,d);
        CHECK(first.same(second) == (cross(a,b,c) == 0 && cross(a,b,d) == 0));
    }
}

void check_xor_thresholds(){
    automata::_xor_trie defaults;
    defaults.insert(2);CHECK(defaults.count(0)==0);CHECK(defaults.count(2)==1);
    const uint64_t full=numeric_limits<uint64_t>::max(),high=1ull<<63;
    vector<uint64_t> default_values={2,0,1,high,high|5,full,full};
    for(size_t i=1;i<default_values.size();++i)defaults.insert(default_values[i]);
    CHECK(defaults.count(full)==2);CHECK(defaults.count(high)==1);CHECK(defaults.count(0)==1);
    const auto& default_view=defaults;
    for(uint64_t input:{0ull,1ull,2ull,1ull<<63,~0ull}){
        uint64_t best_min=full,best_max=0;
        for(auto value:default_values){best_min=min(best_min,input^value);best_max=max(best_max,input^value);}
        CHECK(default_view.min(input)==best_min);CHECK(default_view.max(input)==best_max);
        CHECK((input^default_view.argmin(input))==best_min);CHECK((input^default_view.argmax(input))==best_max);
    }
    for(auto value:default_values)CHECK(defaults.erase(value));
    CHECK(defaults.size()==0);
    automata::_xor_trie small(2); small.insert(7);
    CHECK(small.less(0,8) == 1); CHECK(small.greater(0,7) == 0); CHECK(small.equal(0,7) == 1);
    const uint64_t top = numeric_limits<uint64_t>::max();
    for(int bit : {0,1,2,5,63}){
        uint64_t mask = bit == 63 ? top : ((1ull << (bit + 1)) - 1);
        automata::_xor_trie trie(bit);
        vector<uint64_t> values = {0,1,mask,mask,mask/2};
        for(auto value : values) trie.insert(value);
        const auto& view=trie;
        vector<uint64_t> limits = {0,1,2,7,8,mask,top-1,top};
        if(bit < 63) limits.push_back(mask+1);
        for(uint64_t input : {0ull,1ull,7ull,8ull,1ull<<32,1ull<<63,~0ull}){
            for(uint64_t limit : limits){
                int less = 0, equal = 0, greater = 0;
                for(auto value : values){
                    auto actual = input ^ value;
                    less += actual < limit; equal += actual == limit; greater += actual > limit;
                }
                CHECK(view.less(input,limit) == less);
                CHECK(view.equal(input,limit) == equal);
                CHECK(view.greater(input,limit) == greater);
            }
        }
        for(auto value : values) CHECK(trie.erase(value));
        CHECK(view.less(top,top) == 0); CHECK(view.equal(top,top) == 0); CHECK(view.greater(top,0) == 0);
    }
}

using Edge = pair<int,int>;
int components(int count, const vector<Edge>& edges, int omit_edge = -1, int omit_vertex = -1){
    vector<vector<int>> adj(count);
    for(int i = 0; i < (int)edges.size(); ++i){
        auto [a,b] = edges[i];
        if(i == omit_edge || a == omit_vertex || b == omit_vertex) continue;
        adj[a].push_back(b); adj[b].push_back(a);
    }
    vector<bool> seen(count); int result = 0;
    for(int root = 0; root < count; ++root){
        if(root == omit_vertex || seen[root]) continue;
        ++result; vector<int> stack{root}; seen[root] = true;
        while(!stack.empty()){
            int cur = stack.back(); stack.pop_back();
            for(int next : adj[cur]) if(!seen[next]){ seen[next] = true; stack.push_back(next); }
        }
    }
    return result;
}
Edge canonical(int a, int b){ return {min(a,b), max(a,b)}; }

void check_bcc_parallel(){
    for(bool base : {false,true}){
        graphs::_bcc parallel(base+1, base); parallel.add(base,base+1); parallel.add(base,base+1); parallel.build();
        CHECK(parallel.line().empty()); CHECK(parallel.point().empty());
        auto blocks = parallel.ret(); CHECK(blocks.size() == 1); CHECK(blocks[0].size() == 2);
        parallel.add(base+1,base); parallel.build(); CHECK(parallel.line().empty());
        blocks = parallel.ret(); CHECK(blocks.size() == 1); CHECK(blocks[0].size() == 3);
        graphs::_bcc joined(base+4, base);
        vector<Edge> fixture = {{0,1},{0,1},{1,2},{2,3},{3,1},{3,4}};
        for(auto [a,b] : fixture) joined.add(a+base,b+base);
        joined.build();
        auto groups = joined.ret(); vector<vector<Edge>> actual;
        for(const auto& group : groups){
            vector<Edge> normalized;
            for(auto [a,b] : group) normalized.push_back(canonical(a-base,b-base));
            sort(normalized.begin(),normalized.end()); actual.push_back(normalized);
        }
        vector<vector<Edge>> expected = {{{0,1},{0,1}},{{1,2},{1,3},{2,3}},{{3,4}}};
        sort(actual.begin(),actual.end()); sort(expected.begin(),expected.end()); CHECK(actual == expected);
        mt19937 rng(643);
        for(int round = 0; round < 140; ++round){
            int count = 2 + rng()%6; vector<Edge> edges;
            graphs::_bcc graph(count-1+base, base);
            for(int i = 0, len = rng()%16; i < len; ++i){
                int a = rng()%count, b = rng()%count; if(a == b) continue;
                edges.push_back({a,b}); graph.add(a+base,b+base);
            }
            graph.build();
            int before = components(count,edges);
            vector<Edge> want_bridges, got_bridges;
            for(int i = 0; i < (int)edges.size(); ++i) if(components(count,edges,i) > before)
                want_bridges.push_back(canonical(edges[i].first,edges[i].second));
            for(auto [a,b] : graph.line()) got_bridges.push_back(canonical(a-base,b-base));
            sort(want_bridges.begin(),want_bridges.end()); sort(got_bridges.begin(),got_bridges.end());
            CHECK(got_bridges == want_bridges);
            vector<int> want_points, got_points;
            for(int v = 0; v < count; ++v) if(components(count,edges,-1,v) > before) want_points.push_back(v);
            for(auto v : graph.point()) got_points.push_back(v-base);
            sort(got_points.begin(),got_points.end()); CHECK(got_points == want_points);
            vector<Edge> all_edges, normalized_edges;
            for(auto [a,b] : edges) normalized_edges.push_back(canonical(a,b));
            for(auto group : graph.ret()) for(auto [a,b] : group) all_edges.push_back(canonical(a-base,b-base));
            sort(all_edges.begin(),all_edges.end()); sort(normalized_edges.begin(),normalized_edges.end());
            CHECK(all_edges == normalized_edges);
        }
    }
}

class reverse_alphabet {
public:
    static constexpr int size=26;
    static int encode(unsigned char c){return 25-(c-'a');}
    static unsigned char decode(int i){return 'z'-i;}
};
class folded_alphabet : public automata::alphabet::lowercase {
public:
    static int encode(unsigned char c){
        if('A'<=c&&c<='Z')c=c-'A'+'a';
        return automata::alphabet::lowercase::encode(c);
    }
};
class span_only_alphabet {
public:
    static constexpr int size=3;
    static int encode(unsigned char)=delete;
};
class sparse_span_only_alphabet {
public:
    static int encode(unsigned char)=delete;
};
template<class A>
vector<int> encoded(const string& s){
    vector<int> result;for(unsigned char c:s)result.push_back(A::encode(c));return result;
}
template<class A>
void check_alphabet_trie(){
    vector<int> seen(256,0);
    for(int i=0;i<A::size;i++){
        unsigned char c=A::decode(i);CHECK(!seen[c]);seen[c]=1;CHECK(A::encode(c)==i);
    }
    automata::_trie<A> tree;tree.reserve(1024);
    vector<string> words{"",""};
    for(int i=0;i<A::size;i++)words.push_back(string(1,char(A::decode(i))));
    mt19937 rng(431+A::size);
    for(int i=0;i<35;i++){
        string s;for(int j=0,len=rng()%5;j<len;j++)s+=char(A::decode(rng()%A::size));words.push_back(s);
    }
    multiset<vector<int>> expected;
    for(const string& s:words){tree.insert(s);expected.insert(encoded<A>(s));}
    auto verify=[&]{
        CHECK(tree.size()==(int)expected.size());int k=1;
        for(const auto& word:expected){CHECK(tree.kth(k)==word);k++;}
        for(const string& s:words){
            auto word=encoded<A>(s);
            CHECK(tree.count(s)==(int)expected.count(word));CHECK(tree.count(span<const int>(word))==tree.count(s));
            CHECK(tree.rank(s)==distance(expected.begin(),expected.lower_bound(word)));
            CHECK(tree.rank(span<const int>(word))==tree.rank(s));
            int prefixes=0;for(const auto& candidate:expected)if(candidate.size()>=word.size()&&equal(word.begin(),word.end(),candidate.begin()))prefixes++;
            CHECK(tree.prefix_count(s)==prefixes);CHECK(tree.prefix_count(span<const int>(word))==prefixes);
            int state=0;for(int c:word)state=tree.go(state,c);
            CHECK(tree.count(state)==tree.count(s));CHECK(tree.prefix_count(state)==prefixes);
        }
    };
    verify();
    for(int i=0;i<(int)words.size();i+=3){
        auto word=encoded<A>(words[i]);auto it=expected.find(word);CHECK(it!=expected.end());
        CHECK(tree.erase(words[i]));expected.erase(it);
    }
    verify();
    string missing(8,char(A::decode(0)));CHECK(!tree.erase(missing));
    auto raw=encoded<A>(missing);tree.insert(span<const int>(raw));CHECK(tree.count(missing)==1);
    CHECK(tree.erase(span<const int>(raw)));CHECK(tree.count(missing)==0);
    tree.clear();CHECK(tree.size()==0);CHECK(tree.prefix_count(string())==0);
}

template<class A>
void check_alphabet_ac(){
    automata::_ac<A> dense,dense_span;
    automata::_sparse_ac<int,A> sparse,sparse_span;
    vector<string> patterns;string text;
    for(int i=0;i<A::size;i++){
        string s(1,char(A::decode(i)));patterns.push_back(s);text+=s;
    }
    mt19937 rng(704+A::size);
    for(int i=0;i<20;i++){
        string s;for(int j=0,len=1+rng()%4;j<len;j++)s+=char(A::decode(rng()%A::size));patterns.push_back(s);
    }
    patterns.push_back(patterns[0]);
    for(int i=0;i<100;i++)text+=char(A::decode(rng()%A::size));
    auto insert=[&](const string& s){
        auto codes=encoded<A>(s);dense.insert(s);sparse.insert(s);
        dense_span.insert(span<const int>(codes));sparse_span.insert(span<const int>(codes));
    };
    for(const string& s:patterns)insert(s);
    auto verify=[&]{
        dense.build();sparse.build();dense_span.build();sparse_span.build();
        long long expected=0;for(const string& s:patterns)for(size_t i=0;i+s.size()<=text.size();i++)expected+=text.compare(i,s.size(),s)==0;
        auto codes=encoded<A>(text);
        CHECK(dense.count(text)==expected);CHECK(sparse.count(text)==expected);
        CHECK(dense.count(span<const int>(codes))==expected);CHECK(sparse.count(span<const int>(codes))==expected);
        CHECK(dense_span.count(text)==expected);CHECK(sparse_span.count(text)==expected);
        vector<int> states,other;
        dense.it_state(text,[&](int s){states.push_back(s);});dense.it_state(span<const int>(codes),[&](int s){other.push_back(s);});CHECK(states==other);
        int cur=0;for(int i=0;i<(int)codes.size();i++){cur=dense.go(cur,codes[i]);CHECK(cur==states[i]);}
        for(int i=0;i<(int)text.size();i++){
            set<int,greater<int>> lengths;
            for(const string& s:patterns)if(s.size()<=size_t(i+1)&&text.compare(i+1-s.size(),s.size(),s)==0)lengths.insert(s.size());
            vector<int> actual;dense.it_match_lens(states[i],[&](int len){actual.push_back(len);});
            CHECK(actual==vector<int>(lengths.begin(),lengths.end()));CHECK(dense.max_match_len(states[i])==*lengths.begin());
        }
    };
    verify();verify();patterns.push_back(text.substr(0,3));insert(patterns.back());verify();
    dense.clear();sparse.clear();dense.build();sparse.build();CHECK(dense.count(text)==0);CHECK(sparse.count(text)==0);
}

void check_alphabets(){
    using namespace automata;
    static_assert(alphabet::lowercase::encode('a')==0&&alphabet::lowercase::encode('z')==25);
    static_assert(alphabet::uppercase::encode('A')==0&&alphabet::uppercase::encode('Z')==25);
    static_assert(alphabet::digits::encode('0')==0&&alphabet::digits::encode('9')==9);
    static_assert(alphabet::alnum::encode('A')==10&&alphabet::alnum::encode('a')==36);
    static_assert(alphabet::bytes::encode(0)==0&&alphabet::bytes::encode(255)==255);
    check_alphabet_trie<alphabet::lowercase>();check_alphabet_trie<alphabet::uppercase>();check_alphabet_trie<alphabet::digits>();
    check_alphabet_trie<alphabet::alnum>();check_alphabet_trie<alphabet::bytes>();check_alphabet_trie<reverse_alphabet>();
    check_alphabet_ac<alphabet::lowercase>();check_alphabet_ac<alphabet::uppercase>();check_alphabet_ac<alphabet::digits>();
    check_alphabet_ac<alphabet::alnum>();check_alphabet_ac<alphabet::bytes>();check_alphabet_ac<reverse_alphabet>();
    _trie<alphabet::alnum> mixed;mixed.insert("a");CHECK(mixed.count("A")==0);CHECK(mixed.count("0")==0);
    _trie<folded_alphabet> folded;folded.insert("Ab");CHECK(folded.count("aB")==1);
    _trie<span_only_alphabet> numeric;vector<int> word{2,0,1};numeric.insert(span<const int>(word));
    CHECK(numeric.count(span<const int>(word))==1);CHECK(numeric.kth(1)==word);
    _ac<span_only_alphabet> d;_sparse_ac<int,span_only_alphabet> s;
    d.insert(span<const int>(word));s.insert(span<const int>(word));d.build();s.build();
    CHECK(d.count(span<const int>(word))==1);CHECK(s.count(span<const int>(word))==1);
    vector<int> arbitrary{-200,1000,-7};_sparse_ac<int,sparse_span_only_alphabet> arbitrary_sparse;
    arbitrary_sparse.insert(span<const int>(arbitrary));arbitrary_sparse.build();CHECK(arbitrary_sparse.count(span<const int>(arbitrary))==1);
    _sparse_ac<> raw;raw.insert("a");raw.build();CHECK(raw.count("aA0")==1);
    _trie<> legacy;legacy.insert("ab");CHECK(legacy.count("ab")==1);
    _ac<> legacy_ac;legacy_ac.insert("ab");legacy_ac.build();CHECK(legacy_ac.count("abab")==2);
}

int main(){
    int failures = 0;
    for(auto [name, test] : vector<pair<string,void(*)()>>{
        {"manacher",check_manacher},{"pollard_one",check_pollard_one},{"line_same",check_line_same},
        {"xor_thresholds",check_xor_thresholds},{"bcc_parallel",check_bcc_parallel},{"alphabet_policies",check_alphabets}}){
        try { test(); cout << "PASS " << name << '\n'; }
        catch(const exception& e){ ++failures; cerr << "FAIL " << name << ": " << e.what() << '\n'; }
    }
    cout << checks << " bug-regression checks, " << failures << " failed groups\n";
    return failures != 0;
}
