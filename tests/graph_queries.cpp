#include <bits/stdc++.h>
namespace graphs {
#include "../Graph.cpp"
}
using namespace std;
static int checks = 0;
#define CHECK(c) do { ++checks; if (!(c)) throw runtime_error(string("line ") + to_string(__LINE__) + ": " + #c); } while (0)
static_assert(is_same_v<decltype(declval<const graphs::_bcc&>().point()), const vector<long long>&>);
static_assert(is_same_v<decltype(declval<const graphs::_bcc&>().line()), const vector<pair<long long,long long>>&>);
static_assert(is_same_v<decltype(declval<const graphs::_bcc&>().ret()), const vector<vector<pair<long long,long long>>>&>);
static_assert(is_same_v<decltype(graphs::lca_policy::cost::v),long long>);
static_assert(!is_default_constructible_v<graphs::lca_policy::node>);

class lca_max : public graphs::lca_policy {
public:
    static constexpr bool use_node = 1;
};
void check_lca_numeric(bool base){
    graphs::_lca<lca_max> tree(base+3,base);
    tree.add(base,base+1,-5000000000000LL,-7000000000000LL);
    tree.add(base+1,base+2,3000000000000LL,9000000000000LL);
    tree.build();
    CHECK(tree.query(base,base+1).v == -5000000000000LL);
    CHECK(tree.query(base+1,base).v == -7000000000000LL);
    CHECK(tree.query(base,base+2).v == 3000000000000LL);
    CHECK(tree.query(base+2,base).v == 9000000000000LL);
    CHECK(tree.query(base,base).v == graphs::lca_policy::id().v);
    CHECK(tree.query(base,base+3).v == graphs::lca_policy::id().v);
    tree.build(); CHECK(tree.query(base+2,base).v == 9000000000000LL);
}

void check_bcc(bool base){
    int a = base, b = base+1, c = base+2;
    graphs::_bcc graph(c,base); graph.add(a,b); graph.add(b,c); graph.build();
    const auto& read = graph;
    CHECK(read.point() == vector<long long>{b});
    CHECK(read.line().size() == 2); CHECK(read.ret().size() == 2); CHECK(graph.num[b] == 1);
    auto points = read.point(), low = graph.mn, order = graph.seq, split = graph.num;
    auto bridges = read.line(); auto blocks = read.ret(); auto visits = graph.cnt;
    const auto* stored_points = &read.point(); const auto* stored_lines = &read.line(); const auto* stored_blocks = &read.ret();
    for(int i = 0; i < 10; ++i){
        CHECK(&read.point() == stored_points); CHECK(&read.line() == stored_lines); CHECK(&read.ret() == stored_blocks);
        CHECK(read.point() == points); CHECK(read.line() == bridges); CHECK(read.ret() == blocks);
        CHECK(graph.mn == low); CHECK(graph.seq == order); CHECK(graph.num == split); CHECK(graph.cnt == visits);
    }
    graph.build(); CHECK(read.point() == points); CHECK(read.line() == bridges); CHECK(read.ret() == blocks);
    graph.add(a,c); graph.build();
    CHECK(read.point().empty()); CHECK(read.line().empty()); CHECK(read.ret().size() == 1); CHECK(read.ret()[0].size() == 3);
    CHECK(graph.num[b] == 0); CHECK(graph.st.empty()); CHECK(points == vector<long long>{b});
    graph.clear(c,base); graph.add(a,b); graph.add(a,c); graph.build();
    CHECK(read.point() == vector<long long>{a}); CHECK(graph.num[a] == 2);
    graph.clear(); graph.build(); CHECK(read.point() == vector<long long>{a}); CHECK(read.line().size() == 2);
    graph.clear(base,base); graph.build(); CHECK(read.point().empty()); CHECK(read.line().empty()); CHECK(read.ret().empty());
    graphs::_bcc empty(0,base); empty.build(); CHECK(empty.point().empty()); CHECK(empty.line().empty()); CHECK(empty.ret().empty());
}

template<class Cycle>
void complete_graph(Cycle& graph, int count, int base){
    for(int i = 0; i < count; ++i) for(int j = i+1; j < count; ++j) graph.add(i+base,j+base);
}
void check_cycles(bool base){
    graphs::_3cycle triangle(base+4,base); graphs::_4cycle square(base+4,base);
    complete_graph(triangle,5,base); complete_graph(square,5,base);
    const auto& t = triangle; const auto& s = square;
    for(int repeat = 0; repeat < 2; ++repeat){
        CHECK(triangle.build() == 10); CHECK(square.build() == 15);
        for(int query = 0; query < 20; ++query){ CHECK(t.ret() == 10); CHECK(s.ret() == 15); }
        CHECK(triangle.build(3) == 1); CHECK(square.build(3) == 0); CHECK(t.ret() == 1); CHECK(s.ret() == 0);
        CHECK(triangle.build(7) == 3); CHECK(square.build(7) == 1); CHECK(t.ret() == 3); CHECK(s.ret() == 1);
        CHECK(triangle.build(1) == 0); CHECK(square.build(1) == 0); CHECK(t.ret() == 0); CHECK(s.ret() == 0);
    }
    triangle.clear(base+3,base); square.clear(base+3,base);
    for(int i = 0; i < 3; ++i){ triangle.add(base+i,base+i+1); square.add(base+i,base+i+1); }
    CHECK(triangle.build() == 0); CHECK(square.build() == 0);
    triangle.add(base,base+3); square.add(base,base+3);
    CHECK(triangle.build() == 0); CHECK(square.build() == 1); CHECK(t.ret() == 0); CHECK(s.ret() == 1);
    triangle.add(base,base+2); square.add(base,base+2);
    CHECK(triangle.build() == 2); CHECK(square.build() == 1);
    triangle.add(base+1,base+3); square.add(base+1,base+3);
    CHECK(triangle.build() == 4); CHECK(square.build() == 3);
    triangle.clear(0,base); square.clear(0,base);
    CHECK(triangle.build() == 0); CHECK(square.build() == 0); CHECK(t.ret() == 0); CHECK(s.ret() == 0);
}
class lca_text {
public:
    class node {
    public:
        string v;
        node(string v) : v(move(v)) {}
    };
    using cost = string;
    static constexpr bool use_node = 1;
    static node id(){ return {""}; }
    static node lift(const cost& v){ return {v}; }
    static node merge(const node& a, const node& b){ return {a.v+b.v}; }
};
void check_lca_order(bool base){
    graphs::_lca<lca_text> tree(base+5,base);
    tree.add(base,base+1,"a"); tree.add(base+1,base+2,"b");
    tree.add(base+1,base+3,"c"); tree.add(base+3,base+4,"d");
    tree.build();
    CHECK(tree.query(base,base+4).v == "acd");
    CHECK(tree.query(base+4,base).v == "dca");
    CHECK(tree.query(base+2,base+4).v == "bcd");
    tree.clear(base+4,base);
    tree.add(base+1,base+2,"a","A"); tree.add(base+2,base+3,"b","B");
    tree.add(base+3,base+4,"c","C"); tree.build();
    CHECK(tree.query(base+1,base+4).v == "abc");
    CHECK(tree.query(base+4,base+1).v == "CBA");
    tree.add(base,base+3,"d","D"); tree.build();
    CHECK(tree.parent(base+2) == base+3);
    CHECK(tree.query(base,base+1).v == "dBA");
    CHECK(tree.query(base+1,base+4).v == "abc");
    CHECK(tree.query(base+4,base+1).v == "CBA");
    tree.build(); CHECK(tree.query(base+1,base).v == "abD");
}
class lca_matrix {
public:
    using cost = array<long long,4>;
    class node {
    public:
        cost v;
        explicit node(cost v) : v(v) {}
    };
    static constexpr bool use_node = 1;
    static node id(){ return node({1,0,0,1}); }
    static node lift(const cost& v){ return node(v); }
    static node merge(const node& a, const node& b){
        cost v{};
        for(int i=0;i<2;i++) for(int j=0;j<2;j++) for(int k=0;k<2;k++)
            v[2*i+j]=(v[2*i+j]+a.v[2*i+k]*b.v[2*k+j])%1000000007;
        return node(v);
    }
};
static_assert(!is_default_constructible_v<lca_matrix::node>);
template<class P>
void check_lca_paths(bool base, int mode, unsigned seed){
    using cost = typename P::cost;
    mt19937 rng(seed);
    int count=3+rng()%23, n=base+count-1;
    graphs::_lca<P> tree(n,base);
    vector<vector<pair<int,cost>>> adj(n+1);
    auto value=[&]() -> cost {
        if constexpr(is_same_v<cost,string>) return "["+to_string(rng()%1000)+"]";
        else return {1+(int)(rng()%9),1+(int)(rng()%9),1+(int)(rng()%9),1+(int)(rng()%9)};
    };
    auto add=[&](int a,int b){
        cost ab=value(), ba=mode%2 ? value() : ab;
        if(mode>=2){
            if(mode%2) tree.addsol(a,b,ab,ba);
            else tree.addsol(a,b,ab);
        } else if(mode%2){
            if(rng()%2) tree.add(a,b,ab,ba); else tree.add(b,a,ba,ab);
        } else {
            if(rng()%2) tree.add(a,b,ab); else tree.add(b,a,ab);
        }
        adj[a].push_back({b,ab}); adj[b].push_back({a,ba});
    };
    for(int v=base+1;v<=n;v++) if(rng()%4) add(base+rng()%(v-base),v);
    auto verify=[&](){
        vector<int> par(n+1,-1), dep(n+1,-1), comp(n+1,-1);
        for(int root=base;root<=n;root++) if(dep[root]==-1){
            queue<int> q; q.push(root); dep[root]=0; comp[root]=root;
            while(!q.empty()){
                int v=q.front(); q.pop();
                for(auto [w,c]:adj[v]) if(dep[w]==-1){
                    par[w]=v; dep[w]=dep[v]+1; comp[w]=root; q.push(w);
                }
            }
        }
        for(int a=base;a<=n;a++){
            CHECK(tree.parent(a)==par[a]); CHECK(tree.depth(a)==dep[a]);
            CHECK(tree.ancestor(a,0)==a); CHECK(tree.ancestor(a,dep[a]+1)==-1);
            for(int b=base;b<=n;b++){
                CHECK(tree.same_comp(a,b)==(comp[a]==comp[b]));
                if(comp[a]!=comp[b]){
                    CHECK(tree.ret(a,b)==-1); CHECK(tree.dist(a,b)==-1);
                    CHECK(tree.jump(a,b,0)==-1);
                    CHECK(tree.query(a,b).v==P::id().v); continue;
                }
                int u=a,v=b;
                while(dep[u]>dep[v]) u=par[u];
                while(dep[v]>dep[u]) v=par[v];
                while(u!=v){ u=par[u]; v=par[v]; }
                CHECK(tree.ret(a,b)==u);
                vector<int> prev(n+1,-1); queue<int> q; q.push(a); prev[a]=a;
                while(!q.empty()){
                    int cur=q.front();q.pop();
                    for(auto [w,c]:adj[cur]) if(prev[w]==-1){ prev[w]=cur;q.push(w); }
                }
                vector<int> path{b};
                while(path.back()!=a) path.push_back(prev[path.back()]);
                reverse(path.begin(),path.end());
                auto expected=P::id();
                for(int i=1;i<(int)path.size();i++)
                    for(auto [w,c]:adj[path[i-1]]) if(w==path[i]) expected=P::merge(expected,P::lift(c));
                CHECK(tree.query(a,b).v==expected.v);
                CHECK(tree.dist(a,b)==(int)path.size()-1);
                for(int k=0;k<(int)path.size();k++) CHECK(tree.jump(a,b,k)==path[k]);
                CHECK(tree.jump(a,b,-1)==-1); CHECK(tree.jump(a,b,path.size())==-1);
            }
        }
    };
    tree.build(); verify(); tree.build(); verify();
    for(int root=base+1;root<=n;root++) if(tree.parent(root)==-1) add(base,root);
    tree.build(); verify();
    tree.clear(base,base); tree.build();
    CHECK(tree.ret(base,base)==base); CHECK(tree.dist(base,base)==0);
    CHECK(tree.query(base,base).v==P::id().v);
    tree.clear(0,base); tree.build(); tree.build();
    if(!base) CHECK(tree.query(0,0).v==P::id().v);
}
int main(){
    try {
        for(bool base : {false,true}){ check_bcc(base); check_cycles(base); check_lca_order(base); check_lca_numeric(base); }
        for(bool base : {false,true}) for(int mode=0;mode<4;mode++) for(unsigned seed=1;seed<=8;seed++){
            check_lca_paths<lca_text>(base,mode,seed);
            check_lca_paths<lca_matrix>(base,mode,seed);
        }
        cout << "PASS " << checks << " graph-result/directional-LCA checks\n";
        return 0;
    } catch(const exception& e){ cerr << "FAIL " << e.what() << '\n'; return 1; }
}
