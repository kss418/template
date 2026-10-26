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
namespace graphs {
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
    automata::_xor_trie small(2); small.insert(7);
    CHECK(small.less(0,8) == 1); CHECK(small.greater(0,7) == 0); CHECK(small.eqaul(0,7) == 1);
    const uint64_t top = numeric_limits<uint64_t>::max();
    for(int bit : {0,1,2,5,63}){
        uint64_t mask = bit == 63 ? top : ((1ull << (bit + 1)) - 1);
        automata::_xor_trie trie(bit);
        vector<uint64_t> values = {0,1,mask,mask,mask/2};
        for(auto value : values) trie.insert(value);
        vector<uint64_t> limits = {0,1,2,7,8,mask,top-1,top};
        if(bit < 63) limits.push_back(mask+1);
        for(uint64_t input : {0ull,1ull,7ull,8ull,1ull<<32,1ull<<63,~0ull}){
            for(uint64_t limit : limits){
                int less = 0, equal = 0, greater = 0;
                for(auto value : values){
                    auto actual = input ^ value;
                    less += actual < limit; equal += actual == limit; greater += actual > limit;
                }
                CHECK(trie.less(input,limit) == less);
                CHECK(trie.eqaul(input,limit) == equal);
                CHECK(trie.greater(input,limit) == greater);
            }
        }
        for(auto value : values) CHECK(trie.erase(value));
        CHECK(trie.less(top,top) == 0); CHECK(trie.eqaul(top,top) == 0); CHECK(trie.greater(top,0) == 0);
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
        graphs::_bcc parallel(base+1, base); parallel.add(base,base+1); parallel.add(base,base+1);
        CHECK(parallel.line().empty()); CHECK(parallel.point().empty());
        auto blocks = parallel.ret(); CHECK(blocks.size() == 1); CHECK(blocks[0].size() == 2);
        parallel.add(base+1,base); CHECK(parallel.line().empty());
        blocks = parallel.ret(); CHECK(blocks.size() == 1); CHECK(blocks[0].size() == 3);
        graphs::_bcc joined(base+4, base);
        vector<Edge> fixture = {{0,1},{0,1},{1,2},{2,3},{3,1},{3,4}};
        for(auto [a,b] : fixture) joined.add(a+base,b+base);
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

int main(){
    int failures = 0;
    for(auto [name, test] : vector<pair<string,void(*)()>>{
        {"manacher",check_manacher},{"pollard_one",check_pollard_one},{"line_same",check_line_same},
        {"xor_thresholds",check_xor_thresholds},{"bcc_parallel",check_bcc_parallel}}){
        try { test(); cout << "PASS " << name << '\n'; }
        catch(const exception& e){ ++failures; cerr << "FAIL " << name << ": " << e.what() << '\n'; }
    }
    cout << checks << " bug-regression checks, " << failures << " failed groups\n";
    return failures != 0;
}
