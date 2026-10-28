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
template<class T>
concept has_path_query = requires(T& tree){ tree.query(0,0); };
static_assert(!has_path_query<graphs::_lca>);
static_assert(has_path_query<graphs::_lca_path<>>);
static_assert(is_base_of_v<graphs::_lca,graphs::_lca_path<>>);

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

class mst_box_policy{
public:
    class cost{
    public:
        long long v;
        cost(long long v):v(v){}
        bool operator<(const cost& other) const{return v<other.v;}
    };
    static cost zero(){return {0};}
    static cost add(const cost& a,const cost& b){return {a.v+b.v};}
};
static_assert(!is_default_constructible_v<mst_box_policy::cost>);

void check_mst_api(bool base){
    graphs::_mst<> tree(base+3,base);
    CHECK(tree.add(base,base+1,5)==0);CHECK(tree.add(base+1,base+2,1)==1);
    CHECK(tree.add(base+2,base+3,2)==2);CHECK(tree.add(base,base+3,9)==3);
    auto chosen=[&](){vector<int> ids;as_const(tree).it_edge([&](int id,int,int,const auto&){ids.push_back(id);});return ids;};
    CHECK(tree.build(2));CHECK(as_const(tree).ret()==3);CHECK(chosen()==vector<int>({1,2}));
    CHECK(tree.build());CHECK(as_const(tree).ret()==8);CHECK(chosen()==vector<int>({1,2,0}));
    CHECK(tree.build());CHECK(chosen()==vector<int>({1,2,0}));
    graphs::_lca paths(base+3,base);
    as_const(tree).it_edge([&](int,int a,int b,const auto&){paths.add(a,b);});paths.build();
    CHECK(paths.dist(base,base+3)==3);
    CHECK(tree.add(base,base+3,0)==4);CHECK(tree.build());CHECK(tree.ret()==3);CHECK(chosen()==vector<int>({4,1,2}));
    CHECK(tree.build(4));CHECK(tree.ret()==0);CHECK(chosen().empty());
    tree.clear(base+1,base);CHECK(tree.add(base,base+1,1)==0);CHECK(tree.add(base,base+1,1)==1);
    CHECK(tree.add(base,base,-100)==2);CHECK(tree.build());CHECK(tree.ret()==1);CHECK(chosen()==vector<int>({0}));
    tree.clear(base+2,base);CHECK(tree.add(base,base+1,-4)==0);CHECK(!tree.build());
    CHECK(tree.ret()==-4);CHECK(chosen()==vector<int>({0}));CHECK(tree.build(2));CHECK(tree.ret()==-4);
    tree.clear(0,1);CHECK(tree.build());CHECK(tree.ret()==0);CHECK(chosen().empty());
    tree.clear(0,0);CHECK(tree.build());CHECK(tree.ret()==0);CHECK(chosen().empty());
    graphs::_mst<mst_box_policy> boxed(base+2,base);
    CHECK(boxed.add(base,base+1,{3})==0);CHECK(boxed.add(base+1,base+2,{-2})==1);
    CHECK(boxed.build());CHECK(as_const(boxed).ret().v==1);
    int calls=0;as_const(boxed).it_edge([&](int,int,int,const mst_box_policy::cost&){++calls;});CHECK(calls==2);
}

void check_mst_oracle(bool base){
    mt19937 rng(625+base);
    for(int trial=0;trial<45;++trial){
        int count=1+rng()%5,n=base+count-1,m=rng()%9;
        graphs::_mst<> tree(n,base);vector<tuple<int,int,long long>> edges;
        auto add=[&](int a,int b,long long c){CHECK(tree.add(base+a,base+b,c)==(int)edges.size());edges.push_back({a,b,c});};
        for(int i=0;i<m;++i)add(rng()%count,rng()%count,(int)(rng()%11)-5);
        CHECK(tree.build(count));add(rng()%count,rng()%count,(int)(rng()%11)-5);
        auto root=[](vector<int>& p,int v){while(p[v]!=v)v=p[v];return v;};
        vector<int> complete(count);iota(all(complete),0);int minimum=count;
        for(auto[a,b,c]:edges){a=root(complete,a);b=root(complete,b);if(a!=b){complete[a]=b;--minimum;}}
        for(int target=1;target<=count;++target){
            int needed=count-max(target,minimum);long long best=LLONG_MAX;
            for(unsigned mask=0;mask<(1u<<edges.size());++mask){
                if(popcount(mask)!=needed)continue;
                vector<int> p(count);iota(all(p),0);long long sum=0;bool valid=true;
                for(size_t id=0;id<edges.size();++id)if(mask&(1u<<id)){
                    auto[a,b,c]=edges[id];a=root(p,a);b=root(p,b);
                    if(a==b){valid=false;break;}p[a]=b;sum+=c;
                }
                if(valid)best=min(best,sum);
            }
            CHECK(tree.build(target)==(minimum<=target));CHECK(as_const(tree).ret()==best);
            vector<int> ids,p(count);iota(all(p),0);long long sum=0;
            as_const(tree).it_edge([&](int id,int a,int b,const auto& c){
                CHECK(id>=0&&id<(int)edges.size());auto[oa,ob,oc]=edges[id];
                CHECK(a==base+oa);CHECK(b==base+ob);CHECK(c==oc);
                int ra=root(p,a-base),rb=root(p,b-base);CHECK(ra!=rb);p[ra]=rb;sum+=c;ids.push_back(id);
            });
            CHECK((int)ids.size()==needed);CHECK(sum==best);
            CHECK(tree.build(target)==(minimum<=target));vector<int> repeated;
            as_const(tree).it_edge([&](int id,int,int,const auto&){repeated.push_back(id);});CHECK(repeated==ids);
        }
    }
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
        as_const(plain).it<false>([&](int v){forward.push_back(v);});
        plain.it([&](int v){backward.push_back(v);});
        tree.template it<false>([&](int v){extended.push_back(v);});
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
        vector<vector<int>> expected(k),reverse(k);vector<int> indegree(k,0),outdegree(k,0),mark(k,-1);
        for(int cid=0;cid<k;cid++){
            vector<int> members;g.it_group(cid,[&](int v){members.push_back(v);});
            CHECK((int)members.size()==g.group_size(cid));CHECK(!members.empty());CHECK(g.root(cid)==members[0]);
            for(int a:members){
                CHECK(g.component(a)==cid);
                for(int b:input[a]){
                    int dest=g.component(b);
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
        for(int repeat=0;repeat<3;repeat++){
            CHECK(&g.ind()==stored_ind);CHECK(&g.outd()==stored_out);CHECK(&g.dag()==stored_dag);
            CHECK(g.ind()==indegree);CHECK(g.outd()==outdegree);CHECK(g.dag()==expected);
            vector<pair<int,int>> edges,expected_edges;
            g.it_dag([&](int a,int b){edges.push_back({a,b});});
            for(int a=0;a<k;a++)for(int b:expected[a])expected_edges.push_back({a,b});
            CHECK(edges==expected_edges);
            auto check_top=[&](bool rev){
                const auto& adj=rev?reverse:expected;auto degree=rev?outdegree:indegree;
                deque<int> q;for(int v=0;v<k;v++)if(!degree[v])q.push_back(v);
                vector<pair<int,int>> reference,actual;
                while(!q.empty()){
                    int cur=q.front();q.pop_front();reference.push_back({cur,-1});
                    for(int nxt:adj[cur]){reference.push_back({cur,nxt});if(!--degree[nxt])q.push_back(nxt);}
                }
                g.it_top([&](int a,int b){actual.push_back({a,b});},[&](int v){actual.push_back({v,-1});},rev);
                CHECK(actual==reference);
                int done=0;for(auto [a,b]:actual)if(b==-1)done++;CHECK(done==k);
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
    int calls=0;graph.it_dag([&](int,int){calls++;});CHECK(calls==0);
    graph.it_top([&](int,int){calls++;},[&](int){calls++;});CHECK(calls==(!base));
    graph.clear(1,!base);graph.build();CHECK(graph.comp_cnt()==2-int(!base));
    graph.addsol(!base,1);graph.build();CHECK(graph.component(!base)>=0);
}

int main(){
    try {
        for(bool base : {false,true}){ check_mst_api(base); check_mst_oracle(base); check_bcc(base); check_cycles(base); check_lca_order(base); check_lca_numeric(base); }
        for(bool base : {false,true}) for(int mode=0;mode<4;mode++) for(unsigned seed=1;seed<=8;seed++){
            check_lca_paths<lca_text>(base,mode,seed);
            check_lca_paths<lca_matrix>(base,mode,seed);
        }
        for(bool base:{false,true})for(unsigned seed=1;seed<=24;seed++)check_scc_dag(base,seed);
        cout << "PASS " << checks << " graph/LCA/SCC checks\n";
        return 0;
    } catch(const exception& e){ cerr << "FAIL " << e.what() << '\n'; return 1; }
}
