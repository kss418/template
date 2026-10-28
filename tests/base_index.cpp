#include <bits/stdc++.h>
namespace range_templates {
#include "../Segtree.cpp"
}
namespace graphs {
#include "../Graph.cpp"
}
namespace sets {
#include "../DSU.cpp"
}
namespace flows {
#include "../Flow.cpp"
}
namespace paths {
#include "../Shortest_Path.cpp"
}
namespace offline {
#include "../Others.cpp"
}
using namespace std;
static int checks = 0;
#define CHECK(c) do { ++checks; if (!(c)) throw runtime_error(string("line ") + to_string(__LINE__) + ": " + #c); } while (0)
class uf_leaf_policy{
public:
    class node{
    public:
        long long v;
        node(long long v):v(v){}
    };
    static node leaf(){ return {7}; }
    static void merge(node& a, node& b){ a.v += b.v; }
};
static_assert(!is_default_constructible_v<uf_leaf_policy::node>);
class sum_mo {
public:
    class node {
    public:
        long long v;
        node(long long v):v(v){}
    };
    using ret=node;
    static node leaf(){return {0};}
    static void reset(){}
    static ret id(){return {0};}
    static void push_front(int,const node& v,ret& cur){cur.v+=v.v;}
    static void push_back(int i,const node& v,ret& cur){push_front(i,v,cur);}
    static void pop_front(int,const node& v,ret& cur){cur.v-=v.v;}
    static void pop_back(int i,const node& v,ret& cur){pop_front(i,v,cur);}
};
class lift_sum {
public:
    class node {
    public:
        long long v;
        node(long long v):v(v){}
    };
    using cost=long long;
    static node id(){return {0};}
    static node lift(cost v){return {v};}
    static node merge(const node&a,const node&b){return {a.v+b.v};}
};
class product_mo{
public:
    class node{
    public:
        long long v;
        node(long long v):v(v){}
    };
    class ret{
    public:
        long long v;int size;
        ret(long long v,int size):v(v),size(size){}
    };
    static node leaf(){return {2};}
    static void reset(){}
    static ret id(){return {1,0};}
    static void push_front(int,const node& v,ret& cur){cur.v=cur.v*v.v%101;++cur.size;}
    static void push_back(int i,const node& v,ret& cur){push_front(i,v,cur);}
    static void pop_front(int,const node& v,ret& cur){
        long long inverse=1;
        for(int i=0;i<99;++i)inverse=inverse*v.v%101;
        cur.v=cur.v*inverse%101;--cur.size;
    }
    static void pop_back(int i,const node& v,ret& cur){pop_front(i,v,cur);}
};
static_assert(!is_default_constructible_v<offline::mos_policy::node>);
static_assert(!is_default_constructible_v<offline::mos_policy::ret>);
static_assert(!is_default_constructible_v<product_mo::node>);
static_assert(!is_default_constructible_v<product_mo::ret>);
void check_mos_factories(bool base){
    int n=base+8;offline::_mos<product_mo> mo(n,2,base);
    vector<long long> values(n+1,2);vector<pair<int,int>> queries;
    for(int i=base;i<=n;++i)CHECK(mo.arr[i].v==2);
    mo.build();CHECK(mo.get_result().empty());
    mt19937 rng(714+base);
    for(int i=0;i<100;++i){int l=base+rng()%9,r=base+rng()%9;if(l>r)swap(l,r);mo.add(l,r);queries.push_back({l,r});}
    auto verify=[&](){
        mo.build();CHECK(mo.get_result().size()==queries.size());
        for(size_t i=0;i<queries.size();++i){
            auto[l,r]=queries[i];long long value=1;
            for(int j=l;j<=r;++j)value=value*values[j]%101;
            CHECK(mo.get_result()[i].v==value);CHECK(mo.get_result()[i].size==r-l+1);
        }
    };
    verify();verify();
    for(int i=base;i<=n;i+=2){values[i]=3+i;mo.set(i,{values[i]});}
    mo.add(base,n);queries.push_back({base,n});verify();verify();
    mo.clear(n,0,!base);CHECK(mo.get_result().empty());CHECK(mo.vq.empty());
    for(int i=!base;i<=n;++i)CHECK(mo.arr[i].v==2);
    mo.add(!base,n);mo.build();long long expected=1;
    for(int i=!base;i<=n;++i)expected=expected*2%101;
    CHECK(mo.get_result()[0].v==expected);CHECK(mo.get_result()[0].size==n-int(!base)+1);
    mo.clear(0,0,1);mo.build();mo.build();CHECK(mo.get_result().empty());
    mo.clear(0,0,0);mo.add(0,0);mo.build();CHECK(mo.get_result()[0].v==2);CHECK(mo.get_result()[0].size==1);
    offline::_mos<> defaults;defaults.build();CHECK(defaults.get_result().empty());
}
void check_ranges(bool base){
    const int n=4, first=base, count=n-first+1;
    long long total=0;
    vector<range_templates::monoid::node> a(n+1,range_templates::monoid::leaf());
    vector<range_templates::action::node> lazy_a(n+1,range_templates::action::leaf());
    vector<range_templates::group::node> group_a(n+1,range_templates::group::leaf());
    vector<range_templates::mt_policy::node> sorted_a(n+1);
    for(int i=0;i<=n;++i){a[i]={i+1};lazy_a[i]={i+1,1};group_a[i]={i+1};sorted_a[i]={i+1};if(i>=first)total+=i+1;}
    range_templates::_seg<> seg(span<const range_templates::monoid::node>(a),base);
    range_templates::_prop<> prop(span<const range_templates::action::node>(lazy_a),base);
    range_templates::_fw<> fw(span<const range_templates::group::node>(group_a),base);
    range_templates::_mt<> mt(span<const range_templates::mt_policy::node>(sorted_a),base);
    CHECK(seg.query(-5,n+5).v==total);CHECK(prop.query(-5,n+5).v==total);CHECK(fw.query(-5,n+5).v==total);
    CHECK(mt.geq(-5,n+5,{0})==count);CHECK(fw.pre(first-1).v==0);
    auto zero=[](const auto&v){return v.v<=0;};
    CHECK(seg.max_right(first,zero)==first-1);CHECK(seg.max_right(n,zero)==n-1);
    CHECK(seg.min_left(first,zero)==first+1);CHECK(seg.min_left(n,zero)==n+1);
    CHECK(prop.max_right(n,zero)==n-1);CHECK(prop.min_left(first,zero)==first+1);
    prop.update(first,n,{2});CHECK(prop.query(first,n).v==total+2*count);
    range_templates::_pst_diff<> pst(n,base);int root=0;
    for(int i=first;i<=n;++i)root=pst.set(root,i,{i+1});
    CHECK(pst.query(root,-5,n+5).v==total);CHECK(pst.query(0,first,n).v==0);
    CHECK(pst.diff_query(0,root,first,n).v==total);
    CHECK(pst.diff_max_right(0,root,n,zero)==n-1);CHECK(pst.diff_min_left(0,root,first,zero)==first+1);
    range_templates::_fw2d<> fw2(n,n,base);for(int i=first;i<=n;++i)for(int j=first;j<=n;++j)fw2.update(i,j,{1});
    CHECK(fw2.query(-5,-5,n+5,n+5).v==count*count);
    CHECK(fw2.pre(first-1,n).v==0);
    offline::_mos<sum_mo> mo(n,0,base);for(int i=0;i<=n;++i)mo.set(i,{i+1});mo.add(first,n);mo.add(n,n);mo.build();
    CHECK(mo.get_result()[0].v==total);CHECK(mo.get_result()[1].v==n+1);
    seg.clear(n,!base);seg.set(!base,{7});CHECK(seg.query(-5,n+5).v==7);
    pst.clear(n,!base);CHECK(pst.node_count()==0);
}
void check_sets(bool base){
    int n=4, first=base, count=n-first+1;
    sets::_uf uf(n,base);CHECK(uf.count()==count);CHECK(uf.merge(first,n));CHECK(uf.count()==count-1);CHECK(uf.same(first,n));
    uf.clear(n,!base);CHECK(uf.count()==n-int(!base)+1);
    sets::_uf_value<uf_leaf_policy> meta(n,base);
    for(int i=first;i<=n;++i)CHECK(meta.get(i).v==7);
    CHECK(meta.merge(first,first+1));CHECK(meta.get(first+1).v==14);
    CHECK(meta.merge(n,first+1));CHECK(meta.get(n).v==21);CHECK(meta.size(n)==3);
    CHECK(!meta.merge(first,n));CHECK(meta.get(first).v==21);
    meta.set(n,{25});CHECK(meta.get(first+1).v==25);
    CHECK(meta.merge(first+2,n));CHECK(meta.get(first+2).v==32);
    meta.clear(n,base);CHECK(meta.count()==count);
    for(int i=first;i<=n;++i){CHECK(meta.get(i).v==7);CHECK(meta.size(i)==1);}
    meta.clear(0,1);CHECK(meta.count()==0);
    meta.clear(0,0);CHECK(meta.count()==1);CHECK(meta.get(0).v==7);
    CHECK(!meta.merge(0,0));CHECK(meta.get(0).v==7);
    sets::_wuf w(n,base);w.merge(first,n,9);CHECK(w.diff(first,n).status==sets::_wuf::state::known);CHECK(w.diff(first,n).value==9);
    w.clear(n,!base);CHECK(!w.same(!base,n));
    sets::_ufset intervals(n,base);CHECK(intervals.count()==count);CHECK(intervals.prv(first)==first-1);CHECK(intervals.nxt(n)==n+1);
    intervals.merge(first,first+1);CHECK(intervals.same(first,first+1));CHECK(intervals.prv(first+1)==first-1);CHECK(intervals.nxt(first)==first+2);
}
void check_uf_split(bool base){
    sets::_uf basic;sets::_uf_value<> empty;
    CHECK(basic.count()==0);CHECK(empty.count()==0);
    empty.clear(0,0);CHECK(empty.count()==1);CHECK(!empty.merge(0,0));
    const int n=base+8;
    sets::_uf plain(n,base);sets::_uf_value<uf_leaf_policy> values(n,base);
    vector<int> component(n+1);iota(component.begin(),component.end(),0);
    vector<long long> weight(n+1,7);mt19937 rng(813+base);
    for(int step=0;step<200;++step){
        int a=base+rng()%9,b=base+rng()%9;
        if(step%4==0){
            long long v=(int)(rng()%101)-50;values.set(a,{v});
            int group=component[a];bool first=true;
            for(int i=base;i<=n;++i)if(component[i]==group){weight[i]=first?v:0;first=false;}
        }else{
            int from=component[b],to=component[a];bool joined=from!=to;
            CHECK(plain.merge(a,b)==joined);CHECK(values.merge(a,b)==joined);
            if(joined)for(int i=base;i<=n;++i)if(component[i]==from)component[i]=to;
        }
        set<int> groups;
        for(int i=base;i<=n;++i){
            groups.insert(component[i]);int size=0;long long sum=0;
            for(int j=base;j<=n;++j)if(component[i]==component[j]){++size;sum+=weight[j];}
            CHECK(plain.find(i)==values.find(i));CHECK(plain.size(i)==size);CHECK(values.size(i)==size);
            CHECK(values.get(i).v==sum);CHECK(plain.root(i)==values.root(i));
            CHECK(plain.same(i,a)==(component[i]==component[a]));
        }
        CHECK(plain.count()==(int)groups.size());CHECK(values.count()==plain.count());
        if(step==99){
            plain.clear(n,base);values.clear(n,base);iota(component.begin(),component.end(),0);fill(weight.begin(),weight.end(),7);
        }
    }
    plain.clear(0,1);values.clear(0,1);CHECK(plain.count()==0);CHECK(values.count()==0);
    plain.clear(0,0);values.clear(0,0);CHECK(plain.size(0)==1);CHECK(values.get(0).v==7);
}
void check_weighted_uf(bool base){
    using state=sets::_wuf::state;
    const int first=base,n=base+7;
    sets::_wuf w(n,base);
    vector<vector<pair<int,long long>>> edges(n+1);
    auto add=[&](int a,int b,long long c){w.merge(a,b,c);edges[a].push_back({b,c});edges[b].push_back({a,-c});};
    auto verify=[&](){
        vector<int> component(n+1,-1);vector<long long> potential(n+1);vector<bool> bad(n+1);
        for(int root=first;root<=n;++root)if(component[root]==-1){
            queue<int> q;q.push(root);component[root]=root;
            while(!q.empty()){
                int cur=q.front();q.pop();
                for(auto[next,c]:edges[cur]){
                    if(component[next]==-1){component[next]=root;potential[next]=potential[cur]+c;q.push(next);}
                    else if(potential[next]!=potential[cur]+c)bad[root]=true;
                }
            }
        }
        for(int a=first;a<=n;++a)for(int b=first;b<=n;++b){
            auto result=w.diff(a,b);bool same=component[a]==component[b];
            CHECK(w.same(a,b)==same);
            if(!same)CHECK(result.status==state::disconnected);
            else if(bad[component[a]])CHECK(result.status==state::inconsistent);
            else{CHECK(result.status==state::known);CHECK(result.value==potential[b]-potential[a]);}
        }
    };
    verify();
    add(first,first+1,7);add(first+1,first+2,-3);add(first+3,first+1,9);verify();
    add(first,first+2,4);verify();add(first,first+2,99);verify();
    add(first+4,first+5,2);add(first+4,first+1,6);verify();
    w.clear(n,base);for(auto& e:edges)e.clear();
    add(first,first+1,0);add(first,first+1,1);
    add(first+2,first+3,-2);add(first+3,first+4,0);add(first+2,first,5);verify();
    mt19937 rng(942+base);
    for(int trial=0;trial<30;++trial){
        w.clear(n,base);for(auto& e:edges)e.clear();
        vector<long long> hidden(n+1);for(auto& v:hidden)v=(int)(rng()%31)-15;
        for(int step=0;step<20;++step){
            int a=first+rng()%8,b=first+rng()%8;
            long long c=hidden[b]-hidden[a];if(step%7==6)++c;
            add(a,b,c);verify();
        }
    }
    w.clear(0,1);w.clear(0,0);CHECK(w.diff(0,0).status==state::known);CHECK(w.diff(0,0).value==0);
    w.merge(0,0,-1);CHECK(w.diff(0,0).status==state::inconsistent);
    w.clear(0,0);CHECK(w.diff(0,0).status==state::known);
    const int last=100000;sets::_wuf chain(last,base);
    for(int i=first;i<last;++i)chain.merge(i,i+1,1);
    CHECK(chain.diff(first,last).status==state::known);CHECK(chain.diff(first,last).value==last-first);
    CHECK(chain.diff(last,first).value==first-last);
}
void check_graphs(bool base){
    int n=4, first=base, count=n-first+1;
    graphs::_lca lca(n,base);graphs::_lca_path<lift_sum> weighted(n,base);
    range_templates::_hld_path<range_templates::_seg<>> hld(n,0,base);
    graphs::_vtree vt(n,base);graphs::_mst<> mst(n,base);
    for(int i=first;i<n;++i){lca.add(i,i+1);weighted.add(i,i+1,1);hld.add(i,i+1);vt.add(i,i+1);mst.add(i,i+1,1);}
    lca.build();weighted.build();hld.build();vt.build();
    CHECK(lca.depth(first)==0);CHECK(lca.ancestor(n,n-first)==first);CHECK(lca.ret(first,n)==first);CHECK(lca.dist(first,n)==n-first);
    CHECK(weighted.query(first,n).v==n-first);CHECK(vt.dist_tree(first,n)==n-first);CHECK(vt.make({first,n}).front()==first);
    for(int i=first;i<=n;++i)hld.set(i,{1});CHECK(hld.query(first,n).v==count);CHECK(hld.query_sub(first).v==count);
    range_templates::_hld_path<range_templates::_seg<>> edge_hld(n,1,base);
    for(int i=first;i<n;++i)edge_hld.add(i,i+1);
    edge_hld.build();edge_hld.set(first,{99});for(int i=first+1;i<=n;++i)edge_hld.set(i,{2});
    CHECK(edge_hld.query(first,n).v==2*(count-1));CHECK(edge_hld.query_sub(first).v==2*(count-1));
    CHECK(mst.build());CHECK(mst.ret()==count-1);
    graphs::_mst<> disconnected(n,base);for(int i=first+1;i<n;++i)disconnected.add(i,i+1,1);CHECK(!disconnected.build());
    graphs::_st lift(n,32,base);for(int i=first;i<=n;++i)lift.add(i,min(i+1,n));lift.build();CHECK(lift.ret(first,n-first).first==n);
    graphs::_scc scc(n,base);scc.build();CHECK(scc.comp_cnt()==count);CHECK(scc.component(first)>=0);
    scc.clear(n,!base);scc.build();CHECK(scc.comp_cnt()==n-int(!base)+1);
    graphs::_2sat sat(n,base);CHECK(sat.neg(first)!=sat.pos(n));sat.add_true(sat.pos(first));sat.add_false(sat.pos(n));CHECK(sat.build());
    graphs::_2sat impossible(n,base);impossible.add_eq(impossible.pos(first),impossible.pos(n));impossible.add_true(impossible.pos(first));impossible.add_false(impossible.pos(n));CHECK(!impossible.build());
    graphs::_bm match(n,n,base);for(int i=first;i<=n;++i)match.addsol(i,i);CHECK(match.build()==count);
    graphs::_clist<int> list(n,base);CHECK(list.size()==count);list[first].push_front(7);CHECK(list[first].front()==7);
    graphs::_bcc bcc(n,base);for(int i=first+1;i<=n;++i)bcc.add(first,i);bcc.build();CHECK(bcc.point()==vector<long long>{first});CHECK((int)bcc.line().size()==count-1);
    int last=first+3;
    graphs::_3cycle triangle(last,base);triangle.add(first,first+1);triangle.add(first+1,first+2);triangle.add(first+2,first);triangle.add(first,first+3);triangle.build();CHECK(triangle.ret()==1);
    graphs::_4cycle square(last,base);for(int i=first;i<=last;++i)square.add(i,i==last?first:i+1);square.build();CHECK(square.ret()==1);
    graphs::_hungarian<> assignment(n,base);long long expected=0;
    for(int i=first;i<=n;++i){expected+=i+1;for(int j=first;j<=n;++j)assignment.set(i,j,i==j?i+1:1000);}
    CHECK(assignment.build()==expected);
}
void check_paths_and_flow(bool base){
    int n=4, first=base, count=n-first+1;
    paths::_dij<> dij(n,base);paths::_floyd<> floyd(n,base);paths::_spfa<> spfa(n,base);
    flows::_flow<> flow(n,base);flows::_mcmf<> mincost(n,1,base);
    for(int i=first;i<n;++i){dij.addsol(i,i+1,1);floyd.addsol(i,i+1,1);spfa.addsol(i,i+1,1);flow.addsol(i,i+1,3);mincost.addsol(i,i+1,2,1);}
    dij.build(first);floyd.build();CHECK(spfa.build(first));
    CHECK(dij.ret(n)==count-1);CHECK(floyd.ret(first,n)==count-1);CHECK(floyd.ret(first,first)==0);CHECK(spfa.ret(n)==count-1);
    CHECK(flow.build(first,n)==3);flow.reset();CHECK(flow.build(first,n)==3);
    CHECK((mincost.build(first,n)==pair<long long,long long>{2,2*(count-1)}));
    paths::_spfa<> parallel(n,base);for(long long w:{5,4,3,2,1})parallel.addsol(first,n,w);CHECK(parallel.build(first));CHECK(parallel.ret(n)==1);
    paths::_spfa<> negative(n,base);negative.addsol(first,n,-1);negative.addsol(n,first,0);CHECK(!negative.build(first));CHECK(negative.is_cycle(first));
    paths::_floyd<> via(n,base);via.addsol(n,first,1);via.addsol(first,first+1,1);via.build();CHECK(via.ret(n,first+1)==2);
    flows::_flow<> self(n,base);auto handle=self.addsol(first,first,5);self.set_flow(handle,2);CHECK(self.get_flow(handle)==2);
}
void check_defaults(){
    CHECK(sets::_uf(0).count()==0);CHECK(sets::_ufset(0).count()==0);CHECK(graphs::_clist<int>(0).size()==0);
    graphs::_scc scc;scc.build();CHECK(scc.comp_cnt()==0);
    graphs::_2sat sat;CHECK(sat.build());graphs::_bm bm;CHECK(bm.build()==0);
    graphs::_hungarian<> assignment;CHECK(assignment.build()==0);
    graphs::_lca lca;lca.build();graphs::_vtree vt;vt.build();CHECK(vt.make({}).empty());
    range_templates::_hld_path<range_templates::_seg<>> hld;hld.build();
    range_templates::_mt<> single(0,0);single.update(0,{3});CHECK(single.eq(0,0,{3})==1);
    vector<range_templates::monoid::node> empty;
    range_templates::_seg<> seg(span<const range_templates::monoid::node>(empty),0);CHECK(seg.query(0,0).v==0);
    range_templates::_seg<> default_seg(4);default_seg.set(0,{99});default_seg.set(1,{3});CHECK(default_seg.query(0,4).v==3);
    CHECK(sets::_uf(4).count()==4);CHECK(graphs::_clist<int>(4).size()==4);
}
void check_oracles(bool base){
    mt19937 rng(418+base);
    int first=base,n=7;
    vector<long long> values(n+1),lazy_values(n+1);
    vector<range_templates::monoid::node> a(n+1,range_templates::monoid::leaf());
    vector<range_templates::group::node> ga(n+1,range_templates::group::leaf());
    vector<range_templates::action::node> la(n+1,range_templates::action::leaf());
    for(int i=first;i<=n;++i){values[i]=lazy_values[i]=i+1;a[i]={i+1};ga[i]={i+1};la[i]={i+1,1};}
    range_templates::_seg<> seg(span<const range_templates::monoid::node>(a),base);
    range_templates::_prop<> prop(span<const range_templates::action::node>(la),base);
    range_templates::_fw<> fw(span<const range_templates::group::node>(ga),base);
    range_templates::_pst<> pst(n,base);int root=0;
    for(int i=first;i<=n;++i)root=pst.set(root,i,{values[i]});
    for(int trial=0;trial<200;++trial){
        int pos=first+rng()%(n-first+1),old_root=root;long long old=values[pos];
        values[pos]=rng()%20;seg.set(pos,{values[pos]});fw.set(pos,{values[pos]});root=pst.set(root,pos,{values[pos]});
        CHECK(pst.get(old_root,pos).v==old);
        int l=first+rng()%(n-first+1),r=first+rng()%(n-first+1);if(l>r)swap(l,r);
        long long sum=accumulate(values.begin()+l,values.begin()+r+1,0LL);
        CHECK(seg.query(l,r).v==sum);CHECK(fw.query(l,r).v==sum);CHECK(pst.query(root,l,r).v==sum);
        long long cap=rng()%50,acc=0;auto pred=[cap](const auto&v){return v.v<=cap;};
        int right=l-1;while(right<n && acc+values[right+1]<=cap)acc+=values[++right];
        CHECK(seg.max_right(l,pred)==right);CHECK(pst.max_right(root,l,pred)==right);
        int left=r+1;acc=0;while(left>first && acc+values[left-1]<=cap)acc+=values[--left];
        CHECK(seg.min_left(r,pred)==left);CHECK(pst.min_left(root,r,pred)==left);
        long long delta=rng()%4;prop.update(l,r,{delta});for(int i=l;i<=r;++i)lazy_values[i]+=delta;
        CHECK(prop.query(l,r).v==accumulate(lazy_values.begin()+l,lazy_values.begin()+r+1,0LL));
        right=l-1;acc=0;while(right<n && acc+lazy_values[right+1]<=cap)acc+=lazy_values[++right];
        left=r+1;acc=0;while(left>first && acc+lazy_values[left-1]<=cap)acc+=lazy_values[--left];
        CHECK(prop.max_right(l,pred)==right);CHECK(prop.min_left(r,pred)==left);
    }
    for(int trial=0;trial<40;++trial){
        bool edge[5][5]{};graphs::_3cycle triangles(first+4,base);graphs::_4cycle squares(first+4,base);
        for(int i=0;i<5;++i)for(int j=i+1;j<5;++j)if(rng()%2){edge[i][j]=edge[j][i]=1;triangles.add(first+i,first+j);squares.add(first+i,first+j);}
        long long tri=0,square=0;
        for(int a=0;a<5;++a)for(int b=a+1;b<5;++b)for(int c=b+1;c<5;++c){
            tri+=edge[a][b]&&edge[b][c]&&edge[c][a];
            for(int d=c+1;d<5;++d){
                square+=edge[a][b]&&edge[b][c]&&edge[c][d]&&edge[d][a];
                square+=edge[a][b]&&edge[b][d]&&edge[d][c]&&edge[c][a];
                square+=edge[a][c]&&edge[c][b]&&edge[b][d]&&edge[d][a];
            }
        }
        triangles.build();squares.build();CHECK(triangles.ret()==tri);CHECK(squares.ret()==square);
    }
    for(int trial=0;trial<80;++trial){
        graphs::_2sat solver(first+3,base);vector<pair<int,int>> clauses;
        for(int i=0,sz=rng()%15;i<sz;++i){
            int a=rng()%8,b=rng()%8;clauses.push_back({a,b});
            auto literal=[&](int v){int result=solver.pos(first+v/2);return v%2?solver.op(result):result;};
            solver.add_or(literal(a),literal(b));
        }
        auto value=[](int mask,int literal){return bool((mask>>(literal/2))&1) != bool(literal%2);};
        bool possible=0;
        for(int mask=0;mask<16;++mask){bool valid=1;for(auto[a,b]:clauses)valid&=value(mask,a)||value(mask,b);possible|=valid;}
        CHECK(solver.build()==possible);
        if(possible){int assignment=0;solver.it([&](int i,int v){if(i<=first+3 && v)assignment|=1<<(i-first);});for(auto[a,b]:clauses)CHECK(value(assignment,a)||value(assignment,b));}
    }
}
int main(){
    try {for(bool base:{false,true}){check_ranges(base);check_mos_factories(base);check_sets(base);check_uf_split(base);check_weighted_uf(base);check_graphs(base);check_paths_and_flow(base);check_oracles(base);}check_defaults();cout<<"PASS "<<checks<<" base-index checks\n";return 0;}
    catch(const exception& e){cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
