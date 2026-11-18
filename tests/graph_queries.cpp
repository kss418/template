#include <bits/stdc++.h>
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
static_assert(is_same_v<decltype(declval<const graphs::_bcc&>().point()), const vector<long long>&>);
static_assert(is_same_v<decltype(declval<const graphs::_bcc&>().line()), const vector<pair<long long,long long>>&>);
static_assert(is_same_v<decltype(declval<const graphs::_bcc&>().ret()), const vector<vector<pair<long long,long long>>>&>);
static_assert(is_same_v<decltype(graphs::lca_policy::cost::v),long long>);
static_assert(!is_default_constructible_v<graphs::lca_policy::node>);
template<class T>
concept has_path_query = requires(T& tree){ tree.query(0,0); };
static_assert(!has_path_query<graphs::_lca>);
static_assert(has_path_query<graphs::_lca_path<>>);
static_assert(is_base_of_v<graphs::_lca,graphs::_lca_path<>>);
template<class T>
concept has_public_vtree_storage=requires(T& t){t.adj;} || requires(T& t){t.vt;};
static_assert(!has_public_vtree_storage<graphs::_vtree>);
static_assert(is_base_of_v<graphs::_lca,graphs::_vtree>);
static_assert(is_same_v<decltype(declval<const graphs::_vtree&>().nodes()),const vector<int>&>);

void check_vtree(bool base,unsigned seed){
    mt19937 rng(seed);
    int count=1+rng()%12,n=base+count-1;
    graphs::_vtree tree(n,base);
    vector<vector<pair<int,long long>>> adj(n+1);
    vector<pair<int,int>> skipped;
    auto add=[&](int a,int b){
        long long c=(int)(rng()%31)-15;
        tree.add(a,b,c); adj[a].push_back({b,c}); adj[b].push_back({a,c});
    };
    for(int v=base+1;v<=n;v++){
        int a=base+rng()%(v-base);
        if(seed%2 || rng()%3) add(a,v); else skipped.push_back({a,v});
    }
    auto verify=[&](int root){
        tree.build(root);
        CHECK(tree.nodes().empty()); CHECK(tree.root()==-1);
        int old_edges=0; tree.it_edge([&](int,int,long long){old_edges++;}); CHECK(old_edges==0);
        vector<int> p(n+1,-1),dep(n+1,-1),comp(n+1,-1),pos(n+1,-1);
        vector<long long> dist(n+1);
        auto visit=[&](int r){
            queue<int> q; q.push(r); dep[r]=0; comp[r]=r;
            while(!q.empty()){
                int v=q.front(); q.pop();
                for(auto [w,c]:adj[v]) if(dep[w]==-1){
                    p[w]=v; dep[w]=dep[v]+1; comp[w]=r; dist[w]=dist[v]+c; q.push(w);
                }
            }
        };
        if(root!=-1) visit(root);
        for(int v=base;v<=n;v++) if(dep[v]==-1) visit(v);
        int index=base; tree.it([&](int v){pos[v]=index++;},false);
        auto ancestor=[&](int a,int b){for(int v=b;v!=-1;v=p[v]) if(v==a) return true; return false;};
        auto lca=[&](int a,int b){
            if(comp[a]!=comp[b]) return -1;
            while(dep[a]>dep[b]) a=p[a];
            while(dep[b]>dep[a]) b=p[b];
            while(a!=b){a=p[a]; b=p[b];}
            return a;
        };
        for(int a=base;a<=n;a++){
            CHECK(tree.parent(a)==p[a]); CHECK(tree.depth(a)==dep[a]); CHECK(tree.dist_root(a)==dist[a]);
            for(int b=base;b<=n;b++){
                int c=lca(a,b); CHECK(tree.ret(a,b)==c);
                CHECK(tree.dist_tree(a,b)==(c==-1 ? -1 : dist[a]+dist[b]-2*dist[c]));
            }
        }
        for(int repeat=0;repeat<18;repeat++){
            int anchor=base+rng()%count;
            vector<int> pool,keys;
            for(int v=base;v<=n;v++) if(comp[v]==comp[anchor]) pool.push_back(v);
            if(repeat==1) keys=pool;
            else if(repeat==2) keys={anchor,anchor,anchor};
            else if(repeat){int k=rng()%(2*pool.size()+1); while(k--) keys.push_back(pool[rng()%pool.size()]);}
            set<int> expected(all(keys));
            for(int a:keys) for(int b:keys) expected.insert(lca(a,b));
            auto nodes=tree.make(keys); set<int> actual(all(nodes));
            CHECK(actual==expected); CHECK(actual.size()==nodes.size()); CHECK(as_const(tree).nodes()==nodes);
            CHECK(tree.root()==(nodes.empty() ? -1 : nodes.front()));
            for(int i=1;i<(int)nodes.size();i++) CHECK(pos[nodes[i-1]]<pos[nodes[i]]);
            map<pair<int,int>,long long> child_edges,all_edges;
            vector<int> indegree(n+1);
            for(int a=base;a<=n;a++) as_const(tree).it_child(a,[&](int b,long long c){
                CHECK(actual.contains(a)); CHECK(actual.contains(b)); CHECK(a!=b); CHECK(ancestor(a,b));
                CHECK(c==dist[b]-dist[a]); CHECK(child_edges.emplace(pair{a,b},c).second); indegree[b]++;
                for(int mid:actual) if(mid!=a && mid!=b) CHECK(!(ancestor(a,mid)&&ancestor(mid,b)));
            });
            as_const(tree).it_edge([&](int a,int b,long long c){CHECK(all_edges.emplace(pair{a,b},c).second);});
            CHECK(child_edges==all_edges); CHECK(all_edges.size()==(nodes.empty() ? 0 : nodes.size()-1));
            for(int v:nodes){CHECK(indegree[v]==(v==tree.root() ? 0 : 1)); CHECK(ancestor(tree.root(),v));}
            if(repeat==1){auto copy=nodes; CHECK(tree.make(tree.nodes())==copy); CHECK(tree.nodes()==copy);}
        }
    };
    verify(-1);
    for(int root=base;root<=n;root++) verify(root);
    verify(n); verify(-1);
    for(auto [a,b]:skipped) add(a,b);
    verify(n); verify(n);
    tree.clear(base,base); tree.build(base);
    CHECK(tree.parent(base)==-1); CHECK(tree.dist_root(base)==0);
    CHECK(tree.make(array{int(base),int(base)})==vector<int>{base}); CHECK(tree.root()==base);
    CHECK(tree.nodes()==vector<int>{base});
    int edges=0; tree.it_child(base,[&](int,long long){edges++;}); tree.it_edge([&](int,int,long long){edges++;}); CHECK(edges==0);
    CHECK(tree.make({}).empty()); CHECK(tree.nodes().empty()); CHECK(tree.root()==-1);
    tree.clear(0,1); tree.build(); CHECK(tree.make({}).empty()); CHECK(tree.root()==-1);
}

void check_vtree_wide(bool base){
    constexpr long long big=6000000000000000000LL;
    graphs::_vtree tree(base+2,base);
    tree.add(base,base+1,big); tree.add(base+1,base+2,1); tree.build();
    CHECK(tree.dist_tree(base+1,base+2)==1); CHECK(tree.dist_tree(base+2,base+1)==1);
    CHECK(tree.dist_root(base+2)==big+1);
    CHECK((tree.make(array{base+1,base+2})==vector<int>{base+1,base+2}));
    tree.it_edge([&](int a,int b,long long c){CHECK(a==base+1); CHECK(b==base+2); CHECK(c==1);});
    tree.clear(base+2,base); tree.add(base,base+1,big); tree.add(base+1,base+2,big); tree.build();
    CHECK(tree.dist_tree(base+1,base+2)==big);
    tree.make(array{base+1,base+2}); tree.it_edge([&](int,int,long long c){CHECK(c==big);});
    tree.build(base+2); CHECK(tree.parent(base+2)==-1); CHECK(tree.dist_tree(base,base+1)==big);
    tree.clear(base+2,base); tree.add(base,base+1,LLONG_MIN); tree.add(base+1,base+2,LLONG_MAX); tree.build();
    CHECK(tree.dist_root(base+2)==-1); CHECK(tree.dist_tree(base+1,base+2)==LLONG_MAX);
    CHECK(tree.dist_tree(base,base+1)==LLONG_MIN); CHECK(tree.dist_tree(base,base+2)==-1);
    tree.make(array{int(base),base+2}); tree.it_edge([&](int,int,long long c){CHECK(c==-1);});
}

void check_vtree_span(bool base){
    graphs::_vtree tree(base+3,base);
    tree.add(base,base+1); tree.add(base+1,base+2); tree.add(base+1,base+3); tree.build();
    const array<int,5> keys{-1,base+2,base+3,base+2,-1};
    const set<int> expected{base+1,base+2,base+3};
    auto result=tree.make(span<const int>(keys).subspan(1,3));
    CHECK(set<int>(all(result))==expected); CHECK(keys[1]==base+2 && keys[2]==base+3);
    CHECK(tree.make(tree.nodes())==result);
    auto subset=span<const int>(tree.nodes()).subspan(1);
    result=tree.make(subset); CHECK(set<int>(all(result))==expected);
    const int raw[]{base+2,base+3};
    result=tree.make(raw); CHECK(set<int>(all(result))==expected);
    result=tree.make(span<const int,2>(raw)); CHECK(set<int>(all(result))==expected);
    vector<int> input{base+2,base+3,base+2},copy=input;
    result=tree.make(input); CHECK(input==copy); CHECK(set<int>(all(result))==expected);
    CHECK(tree.make(span<const int>{}).empty()); CHECK(tree.root()==-1); CHECK(tree.nodes().empty());
}

void check_lca_numeric(bool base){
    graphs::_lca_path<> tree(base+3,base);
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
    CHECK(as_const(tree).ret(base,base+2) == base);
    CHECK(as_const(tree).query(base+2,base).v == 9000000000000LL);
    tree.clear(base+1,base); tree.add(base,base+1); tree.build();
    CHECK(tree.query(base,base+1).v == 0);
    tree.clear(base+1,base);
    tree.addsol(base,base+1,2,3); tree.addsol(base,base+1,5,7); tree.build();
    CHECK(tree.query(base,base+1).v == 2);
    CHECK(tree.query(base+1,base).v == 3);
    graphs::_lca_path<> empty; empty.build(); empty.build();
}

void check_mst_api(bool base){
    graphs::_mst<> tree(base+3,base);
    CHECK(tree.add(base,base+1,5)==0);CHECK(tree.add(base+1,base+2,1)==1);
    CHECK(tree.add(base+2,base+3,2)==2);CHECK(tree.add(base,base+3,9)==3);
    const graphs::_mst<>::edge& original=as_const(tree).get_edge(3);
    CHECK(original.s==base && original.e==base+3 && original.c==9);
    auto chosen=[&](){vector<int> ids;as_const(tree).it_edge([&](int id,int,int,const auto&){ids.push_back(id);});return ids;};
    CHECK(tree.build(2));CHECK(as_const(tree).ret()==3);CHECK(chosen()==vector<int>({1,2}));
    CHECK(tree.build());CHECK(as_const(tree).ret()==8);CHECK(chosen()==vector<int>({1,2,0}));
    CHECK(tree.build());CHECK(chosen()==vector<int>({1,2,0}));
    CHECK(tree.build(1,true));CHECK(tree.ret()==16);CHECK(chosen()==vector<int>({3,0,2}));
    CHECK(tree.build(2,true));CHECK(tree.ret()==14);CHECK(chosen()==vector<int>({3,0}));
    CHECK(tree.build());CHECK(tree.ret()==8);CHECK(chosen()==vector<int>({1,2,0}));
    graphs::_lca paths(base+3,base);
    as_const(tree).it_edge([&](int id,int,int,const auto&){
        const auto& e=tree.get_edge(id); paths.add(e.s,e.e);
    });paths.build();
    CHECK(paths.dist(base,base+3)==3);
    CHECK(tree.add(base,base+3,0)==4);CHECK(tree.build());CHECK(tree.ret()==3);CHECK(chosen()==vector<int>({4,1,2}));
    CHECK(tree.build(4));CHECK(tree.ret()==0);CHECK(chosen().empty());
    tree.clear(base+1,base);CHECK(tree.add(base,base+1,1)==0);CHECK(tree.add(base,base+1,1)==1);
    CHECK(tree.add(base,base,-100)==2);CHECK(tree.build());CHECK(tree.ret()==1);CHECK(chosen()==vector<int>({0}));
    CHECK(tree.build(1,true));CHECK(tree.ret()==1);CHECK(chosen()==vector<int>({0}));
    tree.clear(base+2,base);CHECK(tree.add(base,base+1,-4)==0);CHECK(!tree.build());
    CHECK(tree.ret()==-4);CHECK(chosen()==vector<int>({0}));CHECK(tree.build(2));CHECK(tree.ret()==-4);
    tree.clear(0,1);CHECK(tree.build());CHECK(tree.ret()==0);CHECK(chosen().empty());
    tree.clear(0,0);CHECK(tree.build());CHECK(tree.ret()==0);CHECK(chosen().empty());
}

class mst_default_cost{
public:
    long long v;
    mst_default_cost() : v(0){}
    explicit mst_default_cost(long long v) : v(v){}
    bool operator<(const mst_default_cost& other) const{ return v < other.v; }
    void operator+=(const mst_default_cost& other){ v += other.v; }
};

template<class T>
concept mst_cost_supported=requires{typename graphs::_mst<T>;};
static_assert(mst_cost_supported<long long>);
static_assert(mst_cost_supported<__int128_t>);
static_assert(mst_cost_supported<long double>);
static_assert(mst_cost_supported<mst_default_cost>);
static_assert(!mst_cost_supported<vector<int>>);
static_assert(!mst_cost_supported<complex<double>>);
static_assert(!mst_cost_supported<valarray<int>>);
static_assert(is_same_v<decltype(declval<const graphs::_mst<>&>().get_edge(0)),const graphs::_mst<>::edge&>);

void check_mst_default_cost(bool base){
    graphs::_mst<mst_default_cost> tree(base+2,base);
    tree.add(base,base+1,mst_default_cost(3)); tree.add(base+1,base+2,mst_default_cost(-2));
    tree.add(base,base+2,mst_default_cost(10));
    CHECK(tree.build()); CHECK(tree.ret().v==1);
    CHECK(tree.build(1,true)); CHECK(tree.ret().v==13);
    CHECK(tree.build(2)); CHECK(tree.ret().v==-2);
    CHECK(tree.build(3,true)); CHECK(tree.ret().v==0);
    CHECK(tree.build()); CHECK(tree.ret().v==1);
    tree.clear(base+2,base); CHECK(!tree.build()); CHECK(tree.ret().v==0);
    CHECK(tree.add(base,base+1,mst_default_cost(4))==0);
    CHECK(!tree.build()); CHECK(tree.ret().v==4);
    tree.clear(0,1); CHECK(tree.build()); CHECK(tree.ret().v==0);
    int count=0; tree.it_edge([&](int,int,int,const mst_default_cost&){ count++; }); CHECK(count==0);
}

void check_mst_numeric(bool base){
    using i128=__int128_t;
    graphs::_mst<i128> tree(base+3,base);
    tree.add(base,base+1,LLONG_MIN); tree.add(base+1,base+2,-1); tree.add(base+2,base+3,LLONG_MAX);
    CHECK(tree.build()); CHECK(tree.ret()==-2);
    CHECK(tree.build(1,true)); CHECK(tree.ret()==-2);
    tree.clear(base+3,base);
    tree.add(base,base+1,LLONG_MAX); tree.add(base+1,base+2,1); tree.add(base+2,base+3,LLONG_MIN);
    CHECK(tree.build(1,true)); CHECK(tree.ret()==0); CHECK(tree.build()); CHECK(tree.ret()==0);
    graphs::_mst<i128> wide(base+2,base);
    i128 big=(i128)1<<100;
    wide.add(base,base+1,big+3); wide.add(base+1,base+2,big+5); wide.add(base,base+2,big+10);
    CHECK(wide.build()); CHECK(wide.ret()==2*big+8);
    CHECK(wide.build(1,true)); CHECK(wide.ret()==2*big+15);
    int count=0;
    wide.it_edge([&](int,int,int,const i128& cost){CHECK(cost>=big); count++;}); CHECK(count==2);
    graphs::_mst<long double> real(base+2,base);
    real.add(base,base+1,1.25L); real.add(base+1,base+2,2.5L); real.add(base,base+2,4.125L);
    CHECK(real.build()); CHECK(real.ret()==3.75L);
    CHECK(real.build(1,true)); CHECK(real.ret()==6.625L);
    CHECK(real.build(2,true)); CHECK(real.ret()==4.125L);
    real.clear(base+2,base);
    real.add(base,base+1,1); real.add(base,base+2,1); real.add(base+1,base+2,sqrtl(2));
    CHECK(real.build()); CHECK(fabsl(real.ret()-2)<1e-15L);
    CHECK(real.build(1,true)); CHECK(fabsl(real.ret()-(1+sqrtl(2)))<1e-15L);
}

template<class T=long long>
void check_mst_oracle(bool base,bool is_max=false){
    mt19937 rng(625+base);
    for(int trial=0;trial<45;++trial){
        int count=1+rng()%5,n=base+count-1,m=rng()%9;
        graphs::_mst<T> tree(n,base);vector<tuple<int,int,long long>> edges;
        auto add=[&](int a,int b,long long c){CHECK(tree.add(base+a,base+b,c)==(int)edges.size());edges.push_back({a,b,c});};
        for(int i=0;i<m;++i)add(rng()%count,rng()%count,(int)(rng()%11)-5);
        CHECK(tree.build(count,is_max));add(rng()%count,rng()%count,(int)(rng()%11)-5);
        auto root=[](vector<int>& p,int v){while(p[v]!=v)v=p[v];return v;};
        vector<int> complete(count);iota(all(complete),0);int minimum=count;
        for(auto[a,b,c]:edges){a=root(complete,a);b=root(complete,b);if(a!=b){complete[a]=b;--minimum;}}
        for(int target=1;target<=count;++target){
            int needed=count-max(target,minimum);long long best=is_max?LLONG_MIN:LLONG_MAX;
            for(unsigned mask=0;mask<(1u<<edges.size());++mask){
                if(popcount(mask)!=needed)continue;
                vector<int> p(count);iota(all(p),0);long long sum=0;bool valid=true;
                for(size_t id=0;id<edges.size();++id)if(mask&(1u<<id)){
                    auto[a,b,c]=edges[id];a=root(p,a);b=root(p,b);
                    if(a==b){valid=false;break;}p[a]=b;sum+=c;
                }
                if(valid)best=is_max?max(best,sum):min(best,sum);
            }
            CHECK(tree.build(target,is_max)==(minimum<=target));CHECK(as_const(tree).ret()==best);
            vector<int> ids,p(count);iota(all(p),0);long long sum=0;
            as_const(tree).it_edge([&](int id,int a,int b,const auto& c){
                CHECK(id>=0&&id<(int)edges.size());auto[oa,ob,oc]=edges[id];
                CHECK(a==base+oa);CHECK(b==base+ob);CHECK(c==oc);
                int ra=root(p,a-base),rb=root(p,b-base);CHECK(ra!=rb);p[ra]=rb;sum+=(long long)c;ids.push_back(id);
            });
            CHECK((int)ids.size()==needed);CHECK(sum==best);
            CHECK(tree.build(target,is_max)==(minimum<=target));vector<int> repeated;
            as_const(tree).it_edge([&](int id,int,int,const auto&){repeated.push_back(id);});CHECK(repeated==ids);
        }
    }
}

void check_bcc(bool base){
    int a = base, b = base+1, c = base+2;
    graphs::_bcc graph(c,base); graph.add(a,b); graph.add(b,c); graph.build();
    const auto& read = graph;
    CHECK(read.point() == vector<long long>{b});
    CHECK(read.line().size() == 2); CHECK(read.ret().size() == 2); CHECK(read.split_count(b) == 2);
    auto points = read.point();
    auto bridges = read.line(); auto blocks = read.ret();
    const auto* stored_points = &read.point(); const auto* stored_lines = &read.line(); const auto* stored_blocks = &read.ret();
    for(int i = 0; i < 10; ++i){
        CHECK(&read.point() == stored_points); CHECK(&read.line() == stored_lines); CHECK(&read.ret() == stored_blocks);
        CHECK(read.point() == points); CHECK(read.line() == bridges); CHECK(read.ret() == blocks);
        CHECK(read.split_count(b) == 2); CHECK(read.is_cut(b));
    }
    graph.build(); CHECK(read.point() == points); CHECK(read.line() == bridges); CHECK(read.ret() == blocks);
    graph.add(a,c); graph.build();
    CHECK(read.point().empty()); CHECK(read.line().empty()); CHECK(read.ret().size() == 1); CHECK(read.ret()[0].size() == 3);
    CHECK(read.split_count(b) == 1); CHECK(!read.is_cut(b)); CHECK(points == vector<long long>{b});
    graph.clear(c,base); graph.add(a,b); graph.add(a,c); graph.build();
    CHECK(read.point() == vector<long long>{a}); CHECK(read.split_count(a) == 2);
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
        triangle.build(); CHECK(triangle.ret() == 10); CHECK(square.build() == 15);
        for(int query = 0; query < 20; ++query){ CHECK(t.ret() == 10); CHECK(s.ret() == 15); }
        triangle.build(3); CHECK(triangle.ret() == 1); CHECK(square.build(3) == 0); CHECK(t.ret() == 1); CHECK(s.ret() == 0);
        triangle.build(7); CHECK(triangle.ret() == 3); CHECK(square.build(7) == 1); CHECK(t.ret() == 3); CHECK(s.ret() == 1);
        triangle.build(1); CHECK(triangle.ret() == 0); CHECK(square.build(1) == 0); CHECK(t.ret() == 0); CHECK(s.ret() == 0);
    }
    triangle.clear(base+3,base); square.clear(base+3,base);
    for(int i = 0; i < 3; ++i){ triangle.add(base+i,base+i+1); square.add(base+i,base+i+1); }
    triangle.build(); CHECK(triangle.ret() == 0); CHECK(square.build() == 0);
    triangle.add(base,base+3); square.add(base,base+3);
    triangle.build(); CHECK(triangle.ret() == 0); CHECK(square.build() == 1); CHECK(t.ret() == 0); CHECK(s.ret() == 1);
    triangle.add(base,base+2); square.add(base,base+2);
    triangle.build(); CHECK(triangle.ret() == 2); CHECK(square.build() == 1);
    triangle.add(base+1,base+3); square.add(base+1,base+3);
    triangle.build(); CHECK(triangle.ret() == 4); CHECK(square.build() == 3);
    triangle.clear(0,base); square.clear(0,base);
    triangle.build(); CHECK(triangle.ret() == 0); CHECK(square.build() == 0); CHECK(t.ret() == 0); CHECK(s.ret() == 0);
}
class lca_text {
public:
    class node {
    public:
        string v;
        node(string v) : v(move(v)) {}
    };
    using cost = string;
    static node id(){ return {""}; }
    static node lift(const cost& v){ return {v}; }
    static node merge(const node& a, const node& b){ return {a.v+b.v}; }
};
void check_lca_order(bool base){
    graphs::_lca_path<lca_text> tree(base+5,base);
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
    static node id(){ return node({1,0,0,1}); }
    static node lift(const cost& v){ return node(v); }
    static node merge(const node& a, const node& b){
        cost v{};
        for(int i=0;i<2;i++) for(int j=0;j<2;j++) for(int k=0;k<2;k++)
            v[2*i+j]=(v[2*i+j]+a.v[2*i+k]*b.v[2*k+j])%1000000007;
        return node(v);
    }
};

void check_lca_roots(bool base){
    mt19937 rng(7123+base);
    for(int trial=0;trial<8;trial++){
        int n=base+8;
        graphs::_lca plain(n,base);
        graphs::_lca_path<lca_text> tree(n,base);
        vector<vector<pair<int,string>>> adj(n+1);
        auto add=[&](int a,int b){
            string ab="["+to_string(a)+">"+to_string(b)+"]";
            string ba="["+to_string(b)+">"+to_string(a)+"]";
            plain.add(a,b); tree.add(a,b,ab,ba);
            adj[a].push_back({b,ab}); adj[b].push_back({a,ba});
        };
        vector<int> labels(9); iota(all(labels),int(base)); shuffle(all(labels),rng);
        vector<pair<int,int>> skipped;
        for(int i=1;i<9;i++){
            int a=labels[rng()%i],b=labels[i];
            if(trial==0 || rng()%3) add(a,b); else skipped.push_back({a,b});
        }
        auto verify=[&](int root){
            plain.build(root); tree.build(root);
            vector<int> par(n+1,-1),dep(n+1,-1),comp(n+1,-1);
            auto visit=[&](int r){
                queue<int> q; q.push(r); dep[r]=0; comp[r]=r;
                while(!q.empty()){
                    int v=q.front(); q.pop();
                    for(auto [w,c]:adj[v]) if(dep[w]==-1){
                        par[w]=v; dep[w]=dep[v]+1; comp[w]=r; q.push(w);
                    }
                }
            };
            if(root!=-1) visit(root);
            for(int v=base;v<=n;v++) if(dep[v]==-1) visit(v);
            auto lca=[](int a,int b,const vector<int>& p,const vector<int>& d){
                if(d[a]==-1 || d[b]==-1) return -1;
                while(d[a]>d[b]) a=p[a];
                while(d[b]>d[a]) b=p[b];
                while(a!=b){ a=p[a]; b=p[b]; }
                return a;
            };
            vector<int> ord,extended,pos(n+1,-1);
            plain.it([&](int v){ord.push_back(v);},false);
            tree.it([&](int v){extended.push_back(v);},false);
            CHECK(ord==extended); CHECK(ord.size()==9);
            for(int i=0;i<9;i++){ CHECK(pos[ord[i]]==-1); pos[ord[i]]=base+i; }
            for(int a=base;a<=n;a++){
                CHECK(plain.parent(a)==par[a]); CHECK(tree.parent(a)==par[a]);
                CHECK(plain.depth(a)==dep[a]); CHECK(tree.depth(a)==dep[a]);
                int expected_size=0;
                auto [lo,hi]=as_const(plain).subtree_range(a);
                CHECK(lo==pos[a]); CHECK(base<=lo && lo<=hi && hi<=n);
                CHECK(tree.subtree_range(a)==plain.subtree_range(a));
                for(int b=base;b<=n;b++){
                    bool ancestor=false;
                    for(int v=b;v!=-1;v=par[v]) if(v==a) ancestor=true;
                    expected_size+=ancestor;
                    CHECK(plain.is_ancestor(a,b)==ancestor);
                    CHECK(tree.is_ancestor(a,b)==ancestor);
                    CHECK((lo<=pos[b] && pos[b]<=hi)==ancestor);
                    int want=comp[a]==comp[b] ? lca(a,b,par,dep) : -1;
                    CHECK(plain.ret(a,b)==want); CHECK(tree.ret(a,b)==want);
                    vector<int> prev(n+1,-1); vector<string> path(n+1);
                    queue<int> q; q.push(a); prev[a]=a;
                    while(!q.empty()){
                        int v=q.front(); q.pop();
                        for(auto [w,c]:adj[v]) if(prev[w]==-1){
                            prev[w]=v; path[w]=path[v]+c; q.push(w);
                        }
                    }
                    CHECK(tree.query(a,b).v==path[b]);
                    CHECK(plain.dist(a,b)==(want==-1 ? -1 : dep[a]+dep[b]-2*dep[want]));
                }
                CHECK(plain.subtree_size(a)==expected_size);
                CHECK(tree.subtree_size(a)==expected_size);
                CHECK(hi-lo+1==expected_size);
                int v=a;
                for(int k=0;k<=dep[a]+1;k++){
                    CHECK(plain.ancestor(a,k)==v); CHECK(tree.ancestor(a,k)==v);
                    if(v!=-1) v=par[v];
                }
            }
            for(int r=base;r<=n;r++){
                vector<int> rp(n+1,-1),rd(n+1,-1);
                queue<int> q; q.push(r); rd[r]=0;
                while(!q.empty()){
                    int v=q.front(); q.pop();
                    for(auto [w,c]:adj[v]) if(rd[w]==-1){rp[w]=v; rd[w]=rd[v]+1; q.push(w);}
                }
                for(int a=base;a<=n;a++) for(int b=base;b<=n;b++){
                    int want=lca(a,b,rp,rd);
                    CHECK(as_const(plain).ret(a,b,r)==want);
                    CHECK(as_const(tree).ret(a,b,r)==want);
                }
            }
            vector<int> after;
            plain.it([&](int v){after.push_back(v);},false); CHECK(after==ord);
            for(bool rev:{false,true}){
                vector<int> actual,expected=ord;
                if(rev) reverse(all(expected));
                plain.it([&](int v){actual.push_back(v);},rev); CHECK(actual==expected);
            }
            for(int v=base;v<=n;v++){CHECK(plain.parent(v)==par[v]); CHECK(plain.depth(v)==dep[v]);}
        };
        verify(-1);
        for(int root=base;root<=n;root++) verify(root);
        verify(n); verify(-1);
        for(auto [a,b]:skipped) add(a,b);
        verify(labels.back()); verify(labels.back());
    }
    graphs::_lca plain(base+2,base);
    graphs::_lca_path<lca_text> tree(base+2,base);
    plain.addsol(base+2,base+1); plain.addsol(base+1,base);
    tree.addsol(base+2,base+1,"a","A"); tree.addsol(base+1,base,"b","B");
    for(int repeat=0;repeat<2;repeat++){
        plain.build(base+2); tree.build(base+2);
        CHECK(plain.ret(base,base+2)==base+2); CHECK(plain.dist(base,base+2)==2);
        CHECK(plain.subtree_size(base+2)==3); CHECK(plain.subtree_size(base+1)==2);
        CHECK((plain.subtree_range(base+2)==pair<int,int>{base,base+2}));
        CHECK(plain.ret(base+1,base+2,base)==base+1);
        CHECK(tree.query(base+2,base).v=="ab"); CHECK(tree.query(base,base+2).v=="BA");
    }
    plain.clear(base,base); tree.clear(base,base); plain.build(base); tree.build(base);
    CHECK((plain.subtree_range(base)==pair<int,int>{base,base}));
    CHECK(tree.subtree_size(base)==1); CHECK(plain.is_ancestor(base,base));
    CHECK(tree.ret(base,base,base)==base); CHECK(tree.query(base,base).v.empty());
    plain.clear(0,1); tree.clear(0,1);
    for(int repeat=0;repeat<2;repeat++){
        plain.build(); tree.build();
        int count=0; plain.it([&](int){count++;}); tree.it([&](int){count++;}); CHECK(count==0);
    }
}

template<bool edge>
void check_lca_path_extensions(bool base,unsigned seed){
    mt19937 rng(seed);
    int count=4+rng()%7,n=base+count-1;
    graphs::_lca_path<lca_text,edge> tree(n,base);
    vector<vector<pair<int,string>>> adj(n+1);
    vector<string> values(n+1);
    vector<int> labels(count); iota(all(labels),int(base)); shuffle(all(labels),rng);
    auto add=[&](int a,int b){
        string ab=rng()%3 ? "["+to_string(a)+">"+to_string(b)+"]" : "";
        string ba=rng()%3 ? "["+to_string(b)+">"+to_string(a)+"]" : "";
        if constexpr(edge) tree.add(a,b,ab,ba); else tree.add(a,b);
        adj[a].push_back({b,ab}); adj[b].push_back({a,ba});
    };
    vector<pair<int,int>> skipped;
    for(int i=1;i<count;i++){
        int a=labels[rng()%i],b=labels[i];
        if(rng()%3) add(a,b); else skipped.push_back({a,b});
    }
    if constexpr(!edge){
        for(int v=0;v<=n;v++) values[v]=rng()%3 ? "{"+to_string(v)+"}" : "";
        tree.make(values);
    }
    auto aggregate=[&](const vector<int>& path){
        string ret;
        for(int i=0;i<(int)path.size();i++){
            if constexpr(!edge) ret+=values[path[i]];
            else if(i) for(auto [w,c]:adj[path[i-1]]) if(w==path[i]) ret+=c;
        }
        return ret;
    };
    auto verify=[&](int root){
        tree.build(root);
        vector<int> par(n+1,-1),dep(n+1,-1);
        auto visit=[&](int r){
            queue<int> q; q.push(r); dep[r]=0;
            while(!q.empty()){
                int v=q.front(); q.pop();
                for(auto [w,c]:adj[v]) if(dep[w]==-1){par[w]=v; dep[w]=dep[v]+1; q.push(w);}
            }
        };
        if(root!=-1) visit(root);
        for(int v=base;v<=n;v++) if(dep[v]==-1) visit(v);
        for(int a=base;a<=n;a++){
            vector<int> up{a};
            for(int k=0;k<=dep[a];k++){
                CHECK(as_const(tree).query_up(a,k).v==aggregate(up));
                vector<int> down(up.rbegin(),up.rend());
                CHECK(as_const(tree).query_down(a,k).v==aggregate(down));
                if(k<dep[a]) up.push_back(par[up.back()]);
            }
            vector<int> prev(n+1,-1); queue<int> q; q.push(a); prev[a]=a;
            while(!q.empty()){
                int v=q.front(); q.pop();
                for(auto [w,c]:adj[v]) if(prev[w]==-1){prev[w]=v; q.push(w);}
            }
            for(int b=base;b<=n;b++){
                if(prev[b]==-1){
                    CHECK(tree.query(a,b).v.empty());
                    CHECK(tree.bisect(a,b,[](const auto&){return true;})==-1);
                    continue;
                }
                vector<int> path{b};
                while(path.back()!=a) path.push_back(prev[path.back()]);
                reverse(all(path));
                string full=aggregate(path);
                CHECK(as_const(tree).query(a,b).v==full);
                CHECK(tree.bisect(a,b,[](const auto&){return true;})==b);
                for(size_t limit=0;limit<=full.size()+1;limit++){
                    int expected=-1,calls=0;
                    vector<int> prefix;
                    for(int v:path){
                        prefix.push_back(v);
                        if(aggregate(prefix).size()>limit) break;
                        expected=v;
                    }
                    int actual=as_const(tree).bisect(a,b,[&](const lca_text::node& v){
                        calls++;
                        return full.starts_with(v.v) && v.v.size()<=limit;
                    });
                    CHECK(actual==expected);
                    CHECK(calls<=4*(int)bit_width((unsigned)count)+1);
                }
            }
        }
    };
    verify(-1);
    for(int root:labels) verify(root);
    verify(labels.back());
    if constexpr(!edge){
        values[labels.front()]="updated"; tree.set(labels.front(),values[labels.front()]);
        verify(labels.back());
        vector<string> input(n/2+1,"short");
        tree.make(input); fill(all(values),"");
        for(int v=base;v<(int)input.size();v++) values[v]=input[v];
        verify(labels.front());
        input.assign(n+4,"long"); tree.make(input);
        for(int v=base;v<=n;v++) values[v]=input[v];
        verify(labels.back());
    }
    for(auto [a,b]:skipped) add(a,b);
    verify(labels.front()); verify(labels.back());
    tree.clear(base,base); tree.build(base);
    CHECK(tree.query(base,base).v.empty());
    CHECK(tree.query_up(base,0).v.empty()); CHECK(tree.query_down(base,0).v.empty());
    CHECK(tree.bisect(base,base,[](const auto&){return true;})==base);
    if constexpr(!edge){
        tree.set(base,"Z"); tree.build(); CHECK(tree.query(base,base).v=="Z");
        CHECK(tree.bisect(base,base,[](const auto& v){return v.v.empty();})==-1);
        tree.make(vector<string>{}); tree.build(); CHECK(tree.query(base,base).v.empty());
    }
    tree.clear(0,1); tree.build(); tree.build();
    int seen=0; tree.it([&](int){seen++;}); CHECK(seen==0);
}

void check_lca_path_boundaries(bool base){
    graphs::_lca_path<lca_text> edges(base+65,base);
    graphs::_lca_path<lca_text,false> vertices(base+65,base);
    for(int v=base;v<base+65;v++){
        edges.add(v,v+1,"a","b"); vertices.add(v,v+1);
    }
    for(int v=base;v<=base+65;v++) vertices.set(v,"c");
    for(int root:{int(base),base+32,base+65}){
        edges.build(root); vertices.build(root);
        for(int start:{int(base),base+17,base+32,base+65}) for(int end:{int(base),base+1,base+33,base+65}){
            int len=abs(start-end),dir=start<=end ? 1 : -1;
            for(int budget=0;budget<=len+2;budget++){
                auto f=[&](const lca_text::node& v){return (int)v.v.size()<=budget;};
                CHECK(edges.bisect(start,end,f)==start+dir*min(len,budget));
                CHECK(vertices.bisect(start,end,f)==(budget==0 ? -1 : start+dir*min(len,budget-1)));
            }
        }
    }
    graphs::_lca_path<lca_matrix,false> matrices(base+2,base);
    lca_matrix::cost a{1,2,3,4},b{0,1,1,0},c{2,0,0,3};
    matrices.add(base,base+1); matrices.add(base+1,base+2);
    matrices.set(base,a); matrices.set(base+1,b); matrices.set(base+2,c);
    for(int root=base;root<=base+2;root++){
        matrices.build(root);
        CHECK(matrices.query(base,base+2).v==lca_matrix::merge(lca_matrix::merge(lca_matrix::lift(a),lca_matrix::lift(b)),lca_matrix::lift(c)).v);
        CHECK(matrices.query(base+2,base).v==lca_matrix::merge(lca_matrix::merge(lca_matrix::lift(c),lca_matrix::lift(b)),lca_matrix::lift(a)).v);
    }
}

static_assert(!is_default_constructible_v<lca_matrix::node>);
template<class P>
void check_lca_paths(bool base, int mode, unsigned seed){
    using cost = typename P::cost;
    mt19937 rng(seed);
    int count=3+rng()%23, n=base+count-1;
    graphs::_lca_path<P> tree(n,base);
    graphs::_lca plain(n,base);
    vector<vector<pair<int,cost>>> adj(n+1);
    auto value=[&]() -> cost {
        if constexpr(is_same_v<cost,string>) return "["+to_string(rng()%1000)+"]";
        else return {1+(int)(rng()%9),1+(int)(rng()%9),1+(int)(rng()%9),1+(int)(rng()%9)};
    };
    auto add=[&](int a,int b){
        cost ab=value(), ba=mode%2 ? value() : ab;
        if(mode>=2) plain.addsol(a,b); else plain.add(a,b);
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
        plain.build();
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
        vector<int> forward, backward, extended;
        as_const(plain).it([&](int v){forward.push_back(v);},false);
        plain.it([&](int v){backward.push_back(v);});
        tree.it([&](int v){extended.push_back(v);},false);
        CHECK(forward==extended);
        CHECK(vector<int>(forward.rbegin(),forward.rend())==backward);
        CHECK((int)forward.size()==count);
        vector<int> pos(n+1,-1);
        for(int i=0;i<count;i++){ CHECK(pos[forward[i]]==-1); pos[forward[i]]=i; }
        for(int a=base;a<=n;a++){
            CHECK(tree.parent(a)==par[a]); CHECK(tree.depth(a)==dep[a]);
            CHECK(as_const(plain).parent(a)==par[a]); CHECK(plain.depth(a)==dep[a]);
            CHECK(tree.component(a)==plain.component(a));
            if(par[a]!=-1) CHECK(pos[par[a]]<pos[a]);
            CHECK(tree.ancestor(a,0)==a); CHECK(tree.ancestor(a,dep[a]+1)==-1);
            for(int b=base;b<=n;b++){
                CHECK(tree.same_comp(a,b)==(comp[a]==comp[b]));
                CHECK(plain.ret(a,b)==tree.ret(a,b)); CHECK(plain.dist(a,b)==tree.dist(a,b));
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
                for(int k=0;k<(int)path.size();k++) CHECK(as_const(plain).jump(a,b,k)==path[k]);
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
    tree.clear(2,!base); tree.add(!base,2,value()); tree.build();
    CHECK(tree.ret(!base,2)==!base); CHECK(tree.dist(!base,2)==1);
}
template<class T>
concept has_scc_dag = requires(const T& g){ g.ind(); g.outd(); g.it_dag([](int,int){}); };
static_assert(!has_scc_dag<graphs::_scc>);
static_assert(has_scc_dag<graphs::_scc_dag>);
static_assert(is_base_of_v<graphs::_scc,graphs::_scc_dag>);
static_assert(is_same_v<decltype(declval<const graphs::_scc_dag&>().ind()),const vector<int>&>);
static_assert(is_same_v<decltype(declval<const graphs::_scc_dag&>().outd()),const vector<int>&>);
static_assert(is_same_v<decltype(declval<const graphs::_scc_dag&>().dag()),const vector<vector<int>>&>);
static_assert(is_same_v<decltype(declval<const graphs::_scc&>().get_group(0)),const vector<int>&>);
static_assert(is_same_v<decltype(declval<const graphs::_scc&>().groups()),const vector<vector<int>>&>);

void check_scc_dag(bool base,unsigned seed){
    mt19937 rng(seed);int count=1+rng()%12,n=base+count-1;
    graphs::_scc plain(n,base);graphs::_scc_dag graph(n,base);
    vector<vector<int>> input(n+1);
    auto add=[&](int a,int b){plain.addsol(a,b);graph.addsol(a,b);input[a].push_back(b);};
    for(int a=base;a<=n;a++)for(int b=base;b<=n;b++)if(rng()%7==0){
        add(a,b);if(rng()%2)add(a,b);
    }
    auto verify=[&]{
        plain.build();graph.build();const auto& g=graph;int k=g.comp_cnt();
        CHECK(k==plain.comp_cnt());
        vector<vector<bool>> reach(n+1,vector<bool>(n+1,0));
        for(int a=base;a<=n;a++){reach[a][a]=1;for(int b:input[a])reach[a][b]=1;}
        for(int m=base;m<=n;m++)for(int a=base;a<=n;a++)for(int b=base;b<=n;b++)reach[a][b]=reach[a][b]||(reach[a][m]&&reach[m][b]);
        vector<int> seen(n+1,0),ids,all_vertices;
        g.it_cid([&](int cid){ids.push_back(cid);});
        CHECK((int)ids.size()==k);for(int i=0;i<k;i++)CHECK(ids[i]==i);
        g.it_all([&](int v){all_vertices.push_back(v);seen[v]++;});
        CHECK((int)all_vertices.size()==count);
        CHECK((int)g.groups().size()==k); CHECK(g.groups()==plain.groups());
        for(bool rev:{false,true}){
            vector<int> actual_ids,expected_ids(k),actual_vertices,expected_vertices;
            iota(all(expected_ids),0); if(rev) std::reverse(all(expected_ids));
            g.it_cid([&](int cid){actual_ids.push_back(cid);},rev); CHECK(actual_ids==expected_ids);
            for(int cid:expected_ids) for(int v:g.get_group(cid)) expected_vertices.push_back(v);
            g.it_all([&](int v){actual_vertices.push_back(v);},rev); CHECK(actual_vertices==expected_vertices);
        }
        vector<vector<int>> expected(k),reverse(k);vector<int> indegree(k,0),outdegree(k,0),mark(k,-1);
        for(int cid=0;cid<k;cid++){
            vector<int> members;g.it_group(cid,[&](int v){members.push_back(v);});
            CHECK((int)members.size()==g.group_size(cid));CHECK(!members.empty());CHECK(g.root(cid)==members[0]);
            CHECK(g.get_group(cid)==members); CHECK(&g.get_group(cid)==&g.groups()[cid]);
            for(int a:members){
                CHECK(g.component(a)==cid);
                for(int b:input[a]){
                    int dest=g.component(b);
                    if(dest!=cid) CHECK(cid<dest);
                    if(dest==cid||mark[dest]==cid)continue;
                    mark[dest]=cid;expected[cid].push_back(dest);reverse[dest].push_back(cid);indegree[dest]++;outdegree[cid]++;
                }
            }
        }
        for(int a=base;a<=n;a++){
            CHECK(seen[a]==1);CHECK(g.component(a)==plain.component(a));
            for(int b=base;b<=n;b++)CHECK(g.same(a,b)==(reach[a][b]&&reach[b][a]));
        }
        const auto* stored_ind=&g.ind();const auto* stored_out=&g.outd();const auto* stored_dag=&g.dag();
        const auto* stored_reverse=&g.dag(true); const auto* stored_groups=&g.groups();
        for(int repeat=0;repeat<3;repeat++){
            CHECK(&g.ind()==stored_ind);CHECK(&g.outd()==stored_out);CHECK(&g.dag()==stored_dag);
            CHECK(g.ind()==indegree);CHECK(g.outd()==outdegree);CHECK(g.dag()==expected);
            CHECK(&g.dag(false)==stored_dag); CHECK(&g.dag(true)==stored_reverse); CHECK(g.dag(true)==reverse);
            CHECK(&g.groups()==stored_groups);
            vector<pair<int,int>> edges,expected_edges;
            g.it_dag([&](int a,int b){edges.push_back({a,b});});
            for(int a=0;a<k;a++)for(int b:expected[a])expected_edges.push_back({a,b});
            CHECK(edges==expected_edges);
            for(bool rev:{false,true}){
                vector<pair<int,int>> actual,reference;
                const auto& adj=rev ? reverse : expected;
                for(int a=0;a<k;a++) for(int b:adj[a]) reference.push_back({a,b});
                g.it_dag([&](int a,int b){actual.push_back({a,b});},rev); CHECK(actual==reference);
            }
            auto check_top=[&](bool rev){
                const auto& adj=rev?reverse:expected;
                vector<pair<int,int>> reference,actual;
                for(int i=0;i<k;i++){
                    int cur=rev ? k-1-i : i; reference.push_back({cur,-1});
                    for(int nxt:adj[cur]) reference.push_back({cur,nxt});
                }
                g.it_top([&](int a,int b){actual.push_back({a,b});},[&](int v){actual.push_back({v,-1});},rev);
                CHECK(actual==reference);
                int done=0;for(auto [a,b]:actual)if(b==-1)done++;CHECK(done==k);
                vector<vector<bool>> reached(k,vector<bool>(k,false));
                g.it_top([&](int a,int b){for(int v=0;v<k;v++) reached[b][v]=reached[b][v]||reached[a][v];},[&](int a){reached[a][a]=true;},rev);
                for(int a=base;a<=n;a++) for(int b=base;b<=n;b++){
                    int from=g.component(a),to=g.component(b);
                    CHECK((rev ? reached[from][to] : reached[to][from])==reach[a][b]);
                }
                if(rev){
                    vector<pair<int,int>> defaults;
                    g.it_top([&](int a,int b){defaults.push_back({a,b});},[&](int v){defaults.push_back({v,-1});});
                    CHECK(defaults==actual);
                }
            };
            check_top(false);check_top(true);
        }
    };
    verify();verify();
    for(int v=base;v<n;v++)add(v,v+1);verify();
    add(n,base);verify();CHECK(graph.comp_cnt()==1);CHECK(graph.dag()==vector<vector<int>>(1));
    graph.clear(0,base);graph.build();graph.build();
    CHECK(graph.comp_cnt()==(!base));CHECK(graph.ind()==vector<int>(!base,0));CHECK(graph.outd()==vector<int>(!base,0));
    CHECK((int)graph.groups().size()==(!base)); CHECK(graph.dag(true)==graph.dag());
    int calls=0;graph.it_dag([&](int,int){calls++;});CHECK(calls==0);
    graph.it_top([&](int,int){calls++;},[&](int){calls++;});CHECK(calls==(!base));
    graph.clear(1,!base);graph.build();CHECK(graph.comp_cnt()==2-int(!base));
    graph.addsol(!base,1);graph.build();CHECK(graph.component(!base)>=0);
}

void check_clist_views(bool base){
    graphs::_clist<int> g(base+1,base);
    auto view=g[base],copy=view;
    auto read=as_const(g)[base];
    static_assert(is_same_v<decltype(view.front()),int&>);
    static_assert(is_same_v<decltype(read.front()),const int&>);
    auto verify=[&](const vector<int>& expected){
        auto check=[&](const auto& current){
            CHECK(current.size()==(int)expected.size());CHECK(current.empty()==expected.empty());
            vector<int> actual;for(int v:current)actual.push_back(v);
            CHECK(actual==expected);
            if(!expected.empty())CHECK(current.front()==expected.front());
        };
        check(view);check(copy);check(read);check(g[base]);
    };
    verify({});view.push_front(1);verify({1});copy.push_front(2);verify({2,1});
    g.push_front(base+1,99);verify({2,1});
    g.push_front(base,3);verify({3,2,1});view.front()=30;verify({30,2,1});
    view.pop_front();verify({2,1});g.pop_front(base);verify({1});
    copy.clear();verify({});g[base].push_front(4);verify({4});
    copy.pop_front();verify({});copy.pop_front();verify({});
    for(int i=0;i<64;i++)g.push_front(base+1,-i);
    view.push_front(5);verify({5});
    g.clear(base+1,base);verify({});view.push_front(6);verify({6});
}

void check_lift_defaults(){
    constexpr long long big=1LL<<60;
    auto verify=[&](graphs::_st& tree,int base){
        for(long long weight:{1LL,3LL}){
            for(int v=0;v<3;v++)tree.set(base+v,base+(v+1)%3,weight);
            tree.build();
            for(long long budget:{0LL,1LL,10LL,big}){
                pair<int,long long> expected{base+int((budget/weight)%3),(budget/weight)*weight};
                CHECK(tree.ret(base,budget)==expected);
            }
            tree.build();CHECK(tree.ret(base,big).second==(big/weight)*weight);
        }
    };
    graphs::_st constructed(3);verify(constructed,1);
    constructed.clear(3);verify(constructed,1);
    graphs::_st cleared;cleared.clear(3);verify(cleared,1);
    graphs::_st zero_based(2,big,0);verify(zero_based,0);
    cleared.clear(3,16);for(int v=1;v<=3;v++)cleared.set(v,v==3?1:v+1);cleared.build();
    CHECK((cleared.ret(1,10)==pair<int,long long>{2,10}));
}

void check_lift_api(bool base,unsigned seed){
    mt19937 rng(seed);
    int count=1+rng()%12,n=base+count-1,sink=n+1;
    graphs::_st tree(n,256,base);
    vector<int> nxt(n+2,sink); vector<long long> cost(n+2,1); cost[sink]=0;
    auto verify=[&](){
        tree.build();
        for(int start=base;start<=sink;start++){
            CHECK(tree.next(start)==nxt[start]);
            for(long long budget=0;budget<=256;budget++){
                int v=start; long long used=0;
                while(v!=sink && cost[v]<=budget-used){used+=cost[v]; v=nxt[v];}
                CHECK((as_const(tree).ret(start,budget)==pair<int,long long>{v,used}));
            }
            int v=start; long long used=0;
            for(long long k=0;k<=256;k++){
                CHECK((as_const(tree).jump(start,k)==pair<int,long long>{v,used}));
                used+=cost[v]; v=nxt[v];
            }
        }
    };
    verify();
    for(int round=0;round<3;round++){
        for(int v=base;v<=n;v++){
            nxt[v]=base+rng()%(count+1); cost[v]=1+rng()%25;
            tree.set(v,nxt[v],cost[v]);
        }
        verify(); verify();
    }
    tree.clear(0,256,1); tree.build(); tree.build();
    CHECK((tree.jump(1,256)==pair<int,long long>{1,0}));
    CHECK((tree.ret(1,LLONG_MAX)==pair<int,long long>{1,0}));
    tree.clear(base,256,base); tree.set(base,base); tree.build();
    CHECK((tree.jump(base,7)==pair<int,long long>{base,7}));
}

void check_lift_boundaries(bool base){
    const long long big=1LL<<62;
    for(long long maximum:{0LL,1LL,2LL,3LL,4LL,7LL,8LL,(1LL<<60),big-1,big,LLONG_MAX}){
        graphs::_st tree(base+2,maximum,base);
        for(int v=0;v<3;v++) tree.set(base+v,base+(v+1)%3);
        tree.build(); tree.build();
        vector<long long> steps{0,maximum};
        if(maximum) steps.push_back(maximum-1);
        for(int bit=0;bit<=62;bit++) if((1LL<<bit)<=maximum) steps.push_back(1LL<<bit);
        for(int start=0;start<3;start++) for(long long k:steps){
            pair<int,long long> expected{base+(start+k%3)%3,k};
            CHECK(tree.jump(base+start,k)==expected); CHECK(tree.ret(base+start,k)==expected);
        }
        const array<long long,3> cost{-3,0,3};
        for(int v=0;v<3;v++) tree.set(base+v,base+(v+1)%3,cost[v]);
        tree.build();
        for(int start=0;start<3;start++) for(long long k:steps){
            long long sum=0; for(int j=0;j<k%3;j++) sum+=cost[(start+j)%3];
            CHECK((tree.jump(base+start,k)==pair<int,long long>{base+(start+k%3)%3,sum}));
        }
    }
    graphs::_st terminal(base+1,LLONG_MAX,base);
    terminal.set(base,base+1,3); terminal.set(base+1,base+2,4); terminal.build();
    for(long long budget:{0LL,2LL,3LL,6LL,7LL,8LL,LLONG_MAX}){
        pair<int,long long> expected=budget<3 ? pair{int(base),0LL} : budget<7 ? pair{base+1,3LL} : pair{base+2,7LL};
        CHECK(terminal.ret(base,budget)==expected);
    }
    CHECK((terminal.jump(base,0)==pair<int,long long>{base,0}));
    CHECK((terminal.jump(base,1)==pair<int,long long>{base+1,3}));
    CHECK((terminal.jump(base,2)==pair<int,long long>{base+2,7}));
    CHECK((terminal.jump(base,LLONG_MAX)==pair<int,long long>{base+2,7}));
    terminal.clear(base,LLONG_MAX,base); terminal.set(base,base,LLONG_MAX); terminal.build();
    CHECK((terminal.ret(base,LLONG_MAX)==pair<int,long long>{base,LLONG_MAX}));
    CHECK((terminal.jump(base,1)==pair<int,long long>{base,LLONG_MAX}));
    terminal.set(base,base,LLONG_MIN); terminal.build();
    CHECK((terminal.jump(base,1)==pair<int,long long>{base,LLONG_MIN}));
}

void check_lift_u64(bool base){
    using u64=uint64_t;
    constexpr u64 high=1ULL<<63,maximum=UINT64_MAX;
    for(u64 bound:{high-1,high,high+1,maximum}){
        graphs::_st tree(base+2,bound,base);
        const array<long long,3> cost{-3,0,3};
        for(int v=0;v<3;v++) tree.set(base+v,base+(v+1)%3,cost[v]);
        tree.build(); tree.build();
        vector<u64> steps{0,bound,bound-1};
        for(u64 k:{high-1,high,high+1,maximum}) if(k<=bound) steps.push_back(k);
        for(int bit=0;bit<64;bit++) if((1ULL<<bit)<=bound) steps.push_back(1ULL<<bit);
        for(int start=0;start<3;start++) for(u64 k:steps){
            long long sum=0; for(u64 j=0;j<k%3;j++) sum+=cost[(start+j)%3];
            CHECK((tree.jump(base+start,k)==pair<int,long long>{base+(start+k%3)%3,sum}));
        }
        for(int v=0;v<3;v++) tree.set(base+v,base+(v+1)%3);
        tree.build();
        CHECK((tree.ret(base,LLONG_MAX)==pair<int,long long>{base+LLONG_MAX%3,LLONG_MAX}));
    }
    graphs::_st terminal(base,maximum,base);
    terminal.set(base,base+1,3); terminal.build();
    CHECK((terminal.jump(base,maximum)==pair<int,long long>{base+1,3}));
    terminal.clear(base,maximum,base); terminal.set(base,base,LLONG_MAX); terminal.build();
    CHECK((terminal.jump(base,1)==pair<int,long long>{base,LLONG_MAX}));
    terminal.set(base,base,LLONG_MIN); terminal.build();
    CHECK((terminal.jump(base,1)==pair<int,long long>{base,LLONG_MIN}));
    graphs::_st alternating(base+1,maximum,base);
    alternating.set(base,base+1,LLONG_MIN); alternating.set(base+1,base,LLONG_MAX); alternating.build();
    CHECK((alternating.jump(base,high)==pair<int,long long>{base,-(1LL<<62)}));
    CHECK((alternating.jump(base+1,maximum)==pair<int,long long>{base,0}));
    graphs::_st defaults(3);
    for(int round=0;round<2;round++){
        if(round) defaults.clear(3);
        for(int v=1;v<=3;v++) defaults.set(v,v==3 ? 1 : v+1,0);
        defaults.build();
        CHECK((defaults.jump(1,high)==pair<int,long long>{3,0}));
    }
}

template<class T>
concept sat_public_add=requires(T& s){s.add(0,1);};
template<class T>
concept sat_public_scc=requires(T& s){s.scc;} || requires(T& s){s.get_scc();};
static_assert(!sat_public_add<graphs::_2sat>);
static_assert(!sat_public_scc<graphs::_2sat>);
static_assert(is_same_v<decltype(declval<graphs::_2sat&>().build()),void>);
static_assert(is_same_v<decltype(declval<const graphs::_2sat&>().ret()),bool>);
static_assert(is_same_v<decltype(declval<const graphs::_2sat&>().value(0)),bool>);

bool sat_truth(int mask,int literal){ return bool((mask>>(literal%3))&1)!=(literal>=3); }
bool sat_condition(int kind,int mask,int a,int b,int c){
    bool av=sat_truth(mask,a),bv=sat_truth(mask,b),cv=sat_truth(mask,c);
    switch(kind){
        case 0:return av;
        case 1:return !av;
        case 2:return av||bv;
        case 3:return !av||bv;
        case 4:return av==bv;
        case 5:return av!=bv;
        case 6:return int(av)+bv<=1;
        case 7:return int(av)+bv+cv<=1;
        default:return int(av)+bv+cv>=2;
    }
}
void sat_add_condition(graphs::_2sat& s,int base,int kind,int a,int b,int c){
    auto literal=[&](int i){return i<3 ? s.pos(base+i) : s.neg(base+i-3);};
    a=literal(a); b=literal(b); c=literal(c);
    switch(kind){
        case 0:s.add_true(a);break;
        case 1:s.add_false(a);break;
        case 2:s.add_or(a,b);break;
        case 3:s.add_imp(a,b);break;
        case 4:s.add_eq(a,b);break;
        case 5:s.add_xor(a,b);break;
        case 6:s.add_most1(a,b);break;
        case 7:s.add_most1(a,b,c);break;
        default:s.add_least2(a,b,c);
    }
}
int sat_assignment(const graphs::_2sat& s,int base){
    int mask=0,next=base;
    s.it([&](int i,bool value){
        CHECK(i==next++); CHECK(i<=base+2); CHECK(value==s.value(i));
        if(value) mask|=1<<(i-base);
    });
    CHECK(next==base+3);
    for(int v=0;v<3;v++) CHECK(s.value(base+v)==bool((mask>>v)&1));
    next=base;
    s.it_literal([&](int i,bool value){CHECK(i==next++); CHECK(value==sat_truth(mask,i-base));});
    CHECK(next==base+6);
    return mask;
}
void check_2sat_api(bool base){
    for(int kind=0;kind<9;kind++) for(int a=0;a<6;a++) for(int b=0;b<(kind<2 ? 1 : 6);b++)
        for(int c=0;c<(kind<7 ? 1 : 6);c++) for(int mask=0;mask<8;mask++){
            graphs::_2sat s(base+2,base); sat_add_condition(s,base,kind,a,b,c);
            for(int v=0;v<3;v++) if(mask&(1<<v)) s.add_true(s.pos(base+v)); else s.add_false(s.pos(base+v));
            bool possible=sat_condition(kind,mask,a,b,c); s.build(); CHECK(s.ret()==possible);
            if(possible) CHECK(sat_assignment(s,base)==mask);
        }
    mt19937 rng(876+base);
    for(int trial=0;trial<200;trial++){
        graphs::_2sat s(base+2,base); vector<array<int,4>> constraints;
        int count=rng()%15;
        for(int i=0;i<count;i++){
            array<int,4> q{int(rng()%9),int(rng()%6),int(rng()%6),int(rng()%6)};
            constraints.push_back(q); sat_add_condition(s,base,q[0],q[1],q[2],q[3]);
        }
        bool possible=false;
        for(int mask=0;mask<8;mask++){
            bool valid=true; for(auto q:constraints) valid&=sat_condition(q[0],mask,q[1],q[2],q[3]);
            possible|=valid;
        }
        for(int repeat=0;repeat<2;repeat++){
            s.build(); CHECK(s.ret()==possible);
            if(possible){int mask=sat_assignment(s,base); for(auto q:constraints) CHECK(sat_condition(q[0],mask,q[1],q[2],q[3]));}
        }
        s.clear(base,base); s.add_true(s.pos(base)); s.build(); CHECK(s.ret()); CHECK(s.value(base));
        int vars=0,literals=0; s.it([&](int i,bool value){CHECK(i==base); CHECK(value); vars++;});
        s.it_literal([&](int i,bool value){CHECK(value==(i==base)); literals++;}); CHECK(vars==1); CHECK(literals==2);
        s.add_false(s.pos(base)); s.build(); CHECK(!s.ret());
        s.clear(!base,!base); s.add_false(s.pos(!base)); s.build(); CHECK(s.ret()); CHECK(!s.value(!base));
        s.clear(0,1); s.build(); CHECK(s.ret()); vars=0; s.it([&](int,bool){vars++;}); s.it_literal([&](int,bool){vars++;}); CHECK(vars==0);
    }
    graphs::_2sat contradiction(base+1,base);
    contradiction.add_imp(contradiction.pos(base),contradiction.pos(base+1));
    contradiction.add_true(contradiction.pos(base)); contradiction.add_false(contradiction.pos(base+1));
    contradiction.build(); CHECK(!contradiction.ret());
}

static_assert(is_same_v<decltype(declval<graphs::_bm&>().build()),void>);
static_assert(is_same_v<decltype(declval<graphs::_bm&>().augment()),void>);

int bm_oracle(const vector<vector<bool>>& edges,int right){
    vector<bool> possible(1<<right); possible[0]=true;
    for(const auto& row:edges){
        auto next=possible;
        for(int mask=0;mask<(1<<right);mask++) if(possible[mask])
            for(int v=0;v<right;v++) if(row[v] && !(mask&(1<<v))) next[mask|(1<<v)]=true;
        possible=next;
    }
    int result=0;
    for(int mask=0;mask<(1<<right);mask++) if(possible[mask]) result=max(result,popcount(unsigned(mask)));
    return result;
}
void verify_bm(const graphs::_bm& m,const vector<vector<bool>>& edges,int right,int base){
    int left=edges.size(),count=0,next=base;
    CHECK(m.ret()==bm_oracle(edges,right));
    vector<pair<int,int>> expected,actual;
    m.it_all([&](int a,int b){
        CHECK(a==next++); CHECK(m.match_left(a)==b);
        if(b==-1) return;
        CHECK(b>=base && b<base+right); CHECK(edges[a-base][b-base]);
        CHECK(m.match_right(b)==a); expected.emplace_back(a,b); count++;
    });
    CHECK(next==base+left); CHECK(count==m.ret());
    count=0;
    for(int b=base;b<base+right;b++){
        int a=m.match_right(b);
        if(a==-1) continue;
        CHECK(a>=base && a<base+left); CHECK(m.match_left(a)==b); count++;
    }
    CHECK(count==m.ret());
    m.it_matched([&](int a,int b){actual.emplace_back(a,b);}); CHECK(actual==expected);
}
void check_bm_api(bool base){
    for(int left=1;left<=3;left++) for(int right=1;right<=3;right++)
        for(int mask=0;mask<(1<<(left*right));mask++){
            graphs::_bm m(base+left-1,base+right-1,base);
            vector<vector<bool>> edges(left,vector<bool>(right));
            for(int a=0;a<left;a++) for(int b=0;b<right;b++) if(mask&(1<<(a*right+b))){
                edges[a][b]=true; m.addsol(base+a,base+b);
                if((a+b+mask)%3==0) m.addsol(base+a,base+b);
            }
            m.build(); verify_bm(m,edges,right,base);
            m.augment(); verify_bm(m,edges,right,base);
            m.build(); verify_bm(m,edges,right,base);
        }
    mt19937 rng(915+base);
    for(int trial=0;trial<120;trial++){
        int left=1+rng()%6,right=1+rng()%6;
        graphs::_bm m(base+left-1,base+right-1,base);
        vector<vector<bool>> edges(left,vector<bool>(right));
        for(int step=0;step<18;step++){
            int a=rng()%left,b=rng()%right; edges[a][b]=true; m.addsol(base+a,base+b);
            m.augment(); verify_bm(m,edges,right,base);
            if(step%5==0){m.build(); verify_bm(m,edges,right,base);}
        }
    }
    graphs::_bm stable(base+1,base+1,base);
    stable.addsol(base+1,base); stable.build();
    stable.addsol(base,base); stable.augment();
    CHECK(stable.ret()==1); CHECK(stable.match_left(base)==-1); CHECK(stable.match_left(base+1)==base);
    stable.addsol(base+1,base+1); stable.augment(); CHECK(stable.ret()==2);
    CHECK(stable.match_left(base)==base); CHECK(stable.match_left(base+1)==base+1);
    stable.clear(!base,!base,!base); stable.augment(); CHECK(stable.ret()==0);
    CHECK(stable.match_left(!base)==-1); CHECK(stable.match_right(!base)==-1);
    stable.addsol(!base,!base); stable.augment(); CHECK(stable.ret()==1);
    stable.build(); CHECK(stable.ret()==1);

    graphs::_bm shortest(base+3,base+3,base);
    for(auto [a,b]:{pair{0,0},{0,3},{1,1},{1,2},{2,2},{2,3},{3,1},{3,0}}) shortest.addsol(base+a,base+b);
    shortest.build(); CHECK(shortest.ret()==4);
    CHECK(shortest.match_left(base)==base+3); CHECK(shortest.match_left(base+3)==base);
    CHECK(shortest.match_left(base+1)==base+1); CHECK(shortest.match_left(base+2)==base+2);
}
void check_bm_empty(){
    for(auto [left,right]:{pair{0,0},{0,3},{3,0}}){
        graphs::_bm m(left,right);
        vector<vector<bool>> edges(left,vector<bool>(right));
        m.augment(); verify_bm(m,edges,right,1);
        m.build(); verify_bm(m,edges,right,1);
        m.clear(0,0,0); m.addsol(0,0); m.augment(); CHECK(m.ret()==1);
        CHECK(m.match_left(0)==0); CHECK(m.match_right(0)==0);
    }
    graphs::_bm defaults; defaults.build(); CHECK(defaults.ret()==0);
}

template<class T>
concept bcc_public_storage=requires(T& t){t.adj;} || requires(T& t){t.mn;} || requires(T& t){t.seq;}
    || requires(T& t){t.num;} || requires(T& t){t.n;} || requires(T& t){t.cnt;} || requires(T& t){t.st;};
static_assert(!bcc_public_storage<graphs::_bcc>);
static_assert(is_same_v<decltype(declval<graphs::_bcc&>().add(0,1)),int>);
static_assert(is_same_v<decltype(declval<const graphs::_bcc&>().get_edge(0)),const pair<long long,long long>&>);
static_assert(is_same_v<decltype(declval<const graphs::_bcc&>().get_block(0)),const vector<int>&>);
static_assert(is_same_v<decltype(declval<const graphs::_bcc&>().block_vertices(0)),const vector<long long>&>);
static_assert(is_same_v<decltype(declval<const graphs::_bcc&>().vertex_blocks(0)),const vector<int>&>);

int bcc_components(int n,const vector<pair<int,int>>& edges,int skip_vertex=-1,int skip_edge=-1){
    vector<vector<int>> adj(n);
    for(int i=0;i<(int)edges.size();i++) if(i!=skip_edge){
        auto[a,b]=edges[i]; if(a==skip_vertex || b==skip_vertex) continue;
        adj[a].push_back(b); adj[b].push_back(a);
    }
    vector<bool> seen(n); int count=0;
    for(int v=0;v<n;v++) if(v!=skip_vertex && !seen[v]){
        count++; queue<int> q; q.push(v); seen[v]=true;
        while(!q.empty()){int a=q.front(); q.pop(); for(int b:adj[a]) if(!seen[b]){seen[b]=true; q.push(b);}}
    }
    return count;
}
void verify_bcc_api(graphs::_bcc& graph,int n,const vector<pair<int,int>>& edges,int base){
    int m=edges.size(),before=bcc_components(n,edges);
    vector<int> parent(m); iota(parent.begin(),parent.end(),0);
    auto root=[&](int v){while(parent[v]!=v) v=parent[v]; return v;};
    for(int mask=1;mask<(1<<m);mask++){
        vector<int> degree(n); vector<pair<int,int>> cycle; int first=-1;
        for(int i=0;i<m;i++) if(mask&(1<<i)){
            auto[a,b]=edges[i]; degree[a]++; degree[b]++; cycle.push_back(edges[i]); first=i;
        }
        bool valid=popcount(unsigned(mask))>=2; int used=0;
        for(int d:degree){used+=d!=0; if(d && d!=2) valid=false;}
        if(!valid || bcc_components(n,cycle)!=n-used+1) continue;
        for(int i=0;i<m;i++) if(mask&(1<<i)) parent[root(i)]=root(first);
    }
    for(int repeat=0;repeat<2;repeat++){
        graph.build(); const auto& g=as_const(graph);
        vector<long long> cuts; vector<int> bridges,seen(m),component(m,-1);
        for(int v=0;v<n;v++){
            int split=bcc_components(n,edges,v)-before+1;
            CHECK(g.split_count(base+v)==split); CHECK(g.is_cut(base+v)==(split>1));
            if(split>1) cuts.push_back(base+v);
        }
        auto got_cuts=g.point(); sort(got_cuts.begin(),got_cuts.end()); CHECK(got_cuts==cuts);
        vector<pair<long long,long long>> bridge_pairs;
        for(int id=0;id<m;id++){
            auto[a,b]=edges[id]; CHECK((g.get_edge(id)==pair<long long,long long>{base+a,base+b}));
            if(bcc_components(n,edges,-1,id)>before){bridges.push_back(id); bridge_pairs.emplace_back(base+min(a,b),base+max(a,b));}
        }
        auto got_bridges=g.bridge_ids(); sort(got_bridges.begin(),got_bridges.end()); CHECK(got_bridges==bridges);
        auto got_pairs=g.line(); sort(got_pairs.begin(),got_pairs.end()); sort(bridge_pairs.begin(),bridge_pairs.end()); CHECK(got_pairs==bridge_pairs);
        CHECK(g.block_count()==(int)g.ret().size()); vector<vector<int>> memberships(n);
        for(int cid=0;cid<g.block_count();cid++){
            const auto& block=g.get_block(cid); CHECK(!block.empty());
            vector<pair<long long,long long>> pairs; set<long long> vertices;
            for(int id:block){
                CHECK(id>=0 && id<m); CHECK(!seen[id]++); component[id]=cid; CHECK(g.edge_block(id)==cid);
                auto[a,b]=edges[id]; pairs.emplace_back(base+min(a,b),base+max(a,b)); vertices.insert(base+a); vertices.insert(base+b);
            }
            auto stored=g.ret()[cid]; for(auto&[a,b]:stored) if(a>b) swap(a,b);
            sort(stored.begin(),stored.end()); sort(pairs.begin(),pairs.end()); CHECK(stored==pairs);
            const auto& values=g.block_vertices(cid);
            CHECK(set<long long>(values.begin(),values.end())==vertices); CHECK(values.size()==vertices.size());
            for(long long v:vertices) memberships[v-base].push_back(cid);
        }
        for(int id=0;id<m;id++) CHECK(seen[id]==1);
        for(int a=0;a<m;a++) for(int b=0;b<m;b++) CHECK((component[a]==component[b])==(root(a)==root(b)));
        for(int v=0;v<n;v++) CHECK(g.vertex_blocks(base+v)==memberships[v]);
    }
}
void check_bcc_api(bool base){
    for(int n=1;n<=4;n++){
        vector<pair<int,int>> all_edges;
        for(int a=0;a<n;a++) for(int b=a+1;b<n;b++) all_edges.emplace_back(a,b);
        for(int mask=0;mask<(1<<(int)all_edges.size());mask++){
            graphs::_bcc g(base+n-1,base); vector<pair<int,int>> edges;
            for(int id=0;id<(int)all_edges.size();id++) if(mask&(1<<id)){
                auto[a,b]=all_edges[id]; CHECK(g.add(base+a,base+b)==(int)edges.size()); edges.emplace_back(a,b);
            }
            verify_bcc_api(g,n,edges,base);
        }
    }
    mt19937 rng(427+base);
    for(int trial=0;trial<160;trial++){
        int n=2+rng()%5,m=rng()%9; graphs::_bcc g(base+n-1,base); vector<pair<int,int>> edges;
        for(int i=0;i<m;i++){
            int a=rng()%n,b=rng()%(n-1); if(b>=a) b++;
            CHECK(g.add(base+a,base+b)==i); edges.emplace_back(a,b);
        }
        verify_bcc_api(g,n,edges,base);
        int a=rng()%n,b=(a+1)%n; CHECK(g.add(base+a,base+b)==m); edges.emplace_back(a,b);
        verify_bcc_api(g,n,edges,base);
        g.clear(); verify_bcc_api(g,n,edges,base);
        g.clear(!base,!base); verify_bcc_api(g,1,{},!base);
        g.clear(!base+1,!base); CHECK(g.add(!base,!base+1)==0); verify_bcc_api(g,2,{{0,1}},!base);
        g.clear(0,1); g.build(); CHECK(g.block_count()==0); CHECK(g.ret().empty()); CHECK(g.bridge_ids().empty());
    }
    graphs::_bcc singleton(base,base); verify_bcc_api(singleton,1,{},base);
    CHECK(singleton.vertex_blocks(base).empty());
    graphs::_bcc empty; empty.build(); CHECK(empty.block_count()==0); CHECK(empty.point().empty());
}

template<class T>
concept triangle_addsol=requires(T& t){t.addsol(0,1);};
static_assert(!triangle_addsol<graphs::_3cycle>);
static_assert(is_same_v<decltype(declval<graphs::_3cycle&>().build()),void>);

void verify_triangles(graphs::_3cycle& g,const vector<vector<bool>>& edges,int base){
    int n=edges.size(); set<array<int,3>> expected;
    vector<int> expected_counts(n);
    for(int a=0;a<n;a++) for(int b=a+1;b<n;b++) for(int c=b+1;c<n;c++)
        if(edges[a][b] && edges[a][c] && edges[b][c]){
            expected.insert({base+a,base+b,base+c}); expected_counts[a]++; expected_counts[b]++; expected_counts[c]++;
        }
    for(long long mod:{0LL,1LL,2LL,7LL,LLONG_MAX}){
        g.build(mod); const auto& read=as_const(g);
        long long want=mod ? expected.size()%mod : expected.size(); CHECK(read.ret()==want);
        for(int repeat=0;repeat<2;repeat++){
            set<array<int,3>> actual; vector<int> counts(n);
            read.it_triangle([&](int a,int b,int c){
                CHECK(a>=base && a<base+n); CHECK(b>=base && b<base+n); CHECK(c>=base && c<base+n);
                CHECK(a!=b && b!=c && a!=c); CHECK(edges[a-base][b-base] && edges[a-base][c-base] && edges[b-base][c-base]);
                array<int,3> triangle{a,b,c}; sort(triangle.begin(),triangle.end()); CHECK(actual.insert(triangle).second);
                counts[a-base]++; counts[b-base]++; counts[c-base]++;
            });
            CHECK(actual==expected); CHECK(counts==expected_counts); CHECK(read.ret()==want);
        }
    }
}
void check_triangle_api(bool base){
    for(int n=1;n<=5;n++){
        vector<pair<int,int>> pairs; for(int a=0;a<n;a++) for(int b=a+1;b<n;b++) pairs.emplace_back(a,b);
        for(int mask=0;mask<(1<<(int)pairs.size());mask++){
            graphs::_3cycle g(base+n-1,base); vector<vector<bool>> edges(n,vector<bool>(n));
            for(int id=0;id<(int)pairs.size();id++) if(mask&(1<<id)){
                auto[a,b]=pairs[id]; edges[a][b]=edges[b][a]=true; g.add(base+a,base+b);
            }
            verify_triangles(g,edges,base);
        }
    }
    mt19937 rng(702+base);
    for(int trial=0;trial<80;trial++){
        int n=3+rng()%10; graphs::_3cycle g(base+n-1,base); vector<vector<bool>> edges(n,vector<bool>(n));
        vector<pair<int,int>> missing;
        for(int a=0;a<n;a++) for(int b=a+1;b<n;b++){
            if(rng()%2){edges[a][b]=edges[b][a]=true;g.add(base+a,base+b);} else missing.emplace_back(a,b);
        }
        verify_triangles(g,edges,base);
        if(!missing.empty()){
            auto[a,b]=missing.front(); edges[a][b]=edges[b][a]=true; g.add(base+a,base+b); verify_triangles(g,edges,base);
        }
        g.clear(!base,!base); verify_triangles(g,vector<vector<bool>>(1,vector<bool>(1)),!base);
        g.clear(0,1); verify_triangles(g,{},1);
    }
    graphs::_3cycle nested(base+3,base); complete_graph(nested,4,base); nested.build(3);
    int calls=0,total=0;
    as_const(nested).it_triangle([&](int,int,int){
        calls++; as_const(nested).it_triangle([&](int,int,int){total++;});
    });
    CHECK(calls==4); CHECK(total==16); CHECK(nested.ret()==1);
    graphs::_3cycle empty; verify_triangles(empty,{},1);
}

int main(){
    try {
        for(bool base:{false,true}) check_bm_api(base);
        check_bm_empty();
        for(bool base:{false,true}) check_triangle_api(base);
        for(bool base:{false,true}) check_bcc_api(base);
        for(bool base:{false,true}) check_2sat_api(base);
        check_lift_defaults();
        for(bool base:{false,true}){
            check_lift_boundaries(base);
            check_lift_u64(base);
            for(unsigned seed=1;seed<=12;seed++) check_lift_api(base,seed);
        }
        for(bool base:{false,true}){
            check_vtree_wide(base);
            check_vtree_span(base);
            for(unsigned seed=1;seed<=12;seed++) check_vtree(base,seed);
        }
        for(bool base : {false,true}){ check_clist_views(base); check_mst_api(base); check_mst_default_cost(base); check_mst_numeric(base); check_mst_oracle(base); check_mst_oracle(base,true); check_mst_oracle<graphs::i128>(base); check_mst_oracle<graphs::i128>(base,true); check_mst_oracle<long double>(base); check_mst_oracle<long double>(base,true); check_bcc(base); check_cycles(base); check_lca_order(base); check_lca_numeric(base); check_lca_roots(base); }
        for(bool base : {false,true}) for(int mode=0;mode<4;mode++) for(unsigned seed=1;seed<=8;seed++){
            check_lca_paths<lca_text>(base,mode,seed);
            check_lca_paths<lca_matrix>(base,mode,seed);
        }
        for(bool base:{false,true}){
            check_lca_path_boundaries(base);
            for(unsigned seed=1;seed<=6;seed++){
                check_lca_path_extensions<true>(base,seed);
                check_lca_path_extensions<false>(base,seed);
            }
        }
        for(bool base:{false,true})for(unsigned seed=1;seed<=24;seed++)check_scc_dag(base,seed);
        cout << "PASS " << checks << " graph/LCA/SCC checks\n";
        return 0;
    } catch(const exception& e){ cerr << "FAIL " << e.what() << '\n'; return 1; }
}
