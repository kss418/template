#include <bits/stdc++.h>
namespace range_templates {
#include "../Segtree.cpp"
}
namespace numeric_templates {
#include "../Number_Theory.cpp"
}
namespace hash_templates {
using numeric_templates::_mint;
#include "../Hash.cpp"
}
using namespace std;
static int checks = 0;
#define CHECK(c) do { ++checks; if (!(c)) throw runtime_error(string("line ") + to_string(__LINE__) + ": " + #c); } while (0)

struct min_policy {
    struct node { long long v; node(long long v) : v(v){} };
    static node id(){ return {1'000'000}; }
    static node leaf(){ return {7}; }
    static node op(const node& a, const node& b){ return {min(a.v,b.v)}; }
};
struct product_policy {
    struct node { long long v; node(long long v) : v(v){} };
    static node id(){ return {1}; }
    static node leaf(){ return {0}; }
    static node op(const node& a, const node& b){ return {a.v*b.v%101}; }
    static node inv(const node& a){
        long long result = 1;
        for(int i = 0; i < 99; ++i) result = result*a.v%101;
        return {result};
    }
};
struct affine_policy {
    struct node { long long v; int sz; node(long long v,int sz):v(v),sz(sz){} };
    struct lazy {
        long long a,b; lazy(long long a,long long b):a(a),b(b){}
        bool operator==(const lazy&) const = default;
    };
    static node id(){ return {0,0}; }
    static node leaf(){ return {2,1}; }
    static lazy lz_id(){ return {1,0}; }
    static node op(const node& a,const node& b){ return {a.v+b.v,a.sz+b.sz}; }
    static void apply(node& value,const lazy& change){ value.v = value.v*change.a+value.sz*change.b; }
    static void compose(lazy& old,const lazy& next){ old = {old.a*next.a,old.b*next.a+next.b}; }
};

struct assign_policy {
    using node = range_templates::action::node;
    struct lazy {
        bool has; long long value;
        lazy(bool has,long long value):has(has),value(value){}
        bool operator==(const lazy& other) const {
            return has == other.has && (!has || value == other.value);
        }
    };
    static node id(){ return {0,0}; }
    static node leaf(){ return {0,1}; }
    static lazy lz_id(){ return {false,0}; }
    static node op(const node& a,const node& b){ return {a.v+b.v,a.sz+b.sz}; }
    static void apply(node& value,const lazy& change){ if(change.has)value.v=change.value*value.sz; }
    static void compose(lazy& old,const lazy& next){ if(next.has)old=next; }
};

void check_lazy_equality(){
    using tag = range_templates::action::lazy;
    CHECK(tag{0} == range_templates::action::lz_id());
    CHECK(!(tag{3} == range_templates::action::lz_id()));
    CHECK((assign_policy::lazy{false,123} == assign_policy::lz_id()));
    CHECK((!(assign_policy::lazy{true,0} == assign_policy::lz_id())));
    for(bool base : {false,true}){
        int n=base+6;
        range_templates::_prop<> add(n,base); add.update(base,n,{7}); add.update(base,n,{-7});
        CHECK(add.query(base,n).v == 0); CHECK(add.query(base,n).sz == 7);
        range_templates::_prop<assign_policy> set(n,base);
        set.update(base,n,{false,123}); CHECK(set.query(base,n).v == 0);
        set.update(base,n,{true,5}); CHECK(set.query(base,n).v == 35);
        set.update(base+1,n-1,{true,0}); CHECK(set.query(base,n).v == 10);
        set.update(base,n,{false,999}); CHECK(set.query(base,n).v == 10);
        set.update(n,{true,3}); CHECK(set.query(base,n).v == 8);
        for(int i=base;i<=n;++i){ CHECK(set.query(i).v == (i==base?5:i==n?3:0)); CHECK(set.query(i).sz == 1); }
        set.clear(n,base);set.update(base,n,{true,0});CHECK(set.query(base,n).v == 0);CHECK(set.query(base,n).sz == 7);
    }
}

static_assert(!is_default_constructible_v<range_templates::monoid::node>);
static_assert(!is_default_constructible_v<range_templates::group::node>);
static_assert(!is_default_constructible_v<range_templates::action::node>);
static_assert(!is_default_constructible_v<range_templates::action::lazy>);

void check_default_lazy(){
    for(bool base : {false,true}) for(int count = 0; count <= 9; ++count){
        int n = count-1+base;
        range_templates::_prop<> tree(n,base);
        CHECK(tree.query(base,n).v == 0); CHECK(tree.query(base,n).sz == count);
        tree.update(base,n,{5});
        CHECK(tree.query(base,n).v == 5*count); CHECK(tree.query(base,n).sz == count);
        CHECK(tree.query(n+1,n).sz == 0);
        tree.clear(n,base); CHECK(tree.query(base,n).v == 0); CHECK(tree.query(base,n).sz == count);
        if(!count) continue;
        for(int i = base; i <= n; ++i){ CHECK(tree.query(i).sz == 1); CHECK(tree.query(i).v == 0); }
        vector<range_templates::action::node> input(n+1,{-3,1}); tree.make(input);
        CHECK(tree.query(base,n).v == -3*count); CHECK(tree.query(base,n).sz == count);
        tree.make(vector<range_templates::action::node>{}); CHECK(tree.query(base,n).sz == 0);
        tree.make(input); tree.update(base,n,{4}); CHECK(tree.query(base,n).v == count);
    }
}
void check_factory_construction(){
    for(bool base : {false,true}){
        const int count = 4, n = base+count-1;
        vector<range_templates::action::node> input(n+1,range_templates::action::leaf());
        range_templates::_prop<> tree(n,base); tree.make(input); tree.update(base,n,{5});
        CHECK(tree.query(base,n).v == 20); CHECK(tree.query(base,n).sz == count);
        input.assign(n+1,{2}); tree.make(input); tree.update(base,n,{3});
        CHECK(tree.query(base,n).v == 20); CHECK(tree.query(base,n).sz == count);
        tree.update(base,n,range_templates::action::lz_id()); CHECK(tree.query(base,n).v == 20);
        input[base] = {7,0}; tree.make(input); tree.update(base,n,{3});
        CHECK(tree.query(base,n).v == 22); CHECK(tree.query(base,n).sz == count-1);
        CHECK(tree.query(n+1,n).v == 0); CHECK(tree.query(n+1,n).sz == 0);
        tree.clear(n,base); tree.update(base,n,{2}); CHECK(tree.query(base,n).v == 8); CHECK(tree.query(base,n).sz == count);
        range_templates::_seg<> plain(n,base); plain.make(vector<range_templates::monoid::node>(n+1,range_templates::monoid::leaf()));
        plain.set(n,{5}); CHECK(plain.query(base,n).v == 5);
        range_templates::_fw<> fenwick(n,base); fenwick.make(vector<range_templates::group::node>(n+1,range_templates::group::leaf()));
        fenwick.update(base,{2}); fenwick.update(n,{3}); CHECK(fenwick.query(base,n).v == 5);
    }
}
void check_explicit_id(){
    for(bool base : {false,true}){
        int n = base+4;
        range_templates::_seg<min_policy> tree(n,base);
        CHECK(tree.query(base,n).v == 7); CHECK(tree.query(n+1,n).v == 1'000'000);
        tree.set(base,{3}); CHECK(tree.query(base,n).v == 3); CHECK(tree.query(base+1,n).v == 7);
        CHECK(tree.max_right(base,[](auto value){return value.v >= 5;}) == base-1);
        CHECK(tree.min_left(n,[](auto value){return value.v >= 5;}) == base+1);
        vector<min_policy::node> values(n+1,{12}); tree.make(values);
        CHECK(tree.query(base,n).v == 12); tree.clear(n,base); CHECK(tree.query(base,n).v == 7);
        range_templates::_pst<min_policy> pst(n,base);
        CHECK(pst.query(0,base,n).v == 1'000'000);
        int root = pst.set(0,n,{9}); CHECK(pst.query(root,base,n).v == 9); CHECK(pst.query(root,base,n-1).v == 1'000'000);
        range_templates::_fw<product_policy> fw(n,base);
        CHECK(fw.query(base,n).v == 1); fw.update(base,{2}); fw.update(n,{3});
        CHECK(fw.query(base,n).v == 6); CHECK(fw.query(base+1,n).v == 3);
        range_templates::_fw2d<product_policy> grid(n,n,base);
        CHECK(grid.query(base,base,n,n).v == 1); grid.update(base,base,{2}); grid.update(n,n,{3});
        CHECK(grid.query(base,base,n,n).v == 6); CHECK(grid.query(base+1,base+1,n,n).v == 3);
    }
}
void check_affine(){
    mt19937 rng(415);
    for(bool base : {false,true}){
        int n = base+6; vector<long long> naive(n+1,2);
        range_templates::_prop<affine_policy> tree(n,base);
        CHECK(tree.query(base,n).v == 14); CHECK(tree.query(base,n).sz == 7);
        for(int round = 0; round < 150; ++round){
            int l = base+rng()%7, r = base+rng()%7; if(l>r)swap(l,r);
            if(round%3){
                long long a = rng()%2, b = (int)(rng()%9)-4;
                tree.update(l,r,{a,b}); for(int i=l;i<=r;++i)naive[i]=a*naive[i]+b;
            } else {
                long long want=0;for(int i=l;i<=r;++i)want+=naive[i];
                CHECK(tree.query(l,r).v == want); CHECK(tree.query(l,r).sz == r-l+1);
            }
        }
        range_templates::_hld_path<range_templates::_prop<>> hld(n,0,base);
        for(int i=base;i<n;++i)hld.add(i,i+1);hld.build();hld.update(base,n,{3});
        CHECK(hld.query(base,n).v == 21); CHECK(hld.query(base,n).sz == 7);
        hld.build(); CHECK(hld.query(base,n).v == 21); CHECK(hld.query(base,n).sz == 7);
    }
}

void check_hash_defaults(){
    hash_templates::_hash prefix;
    hash_templates::_deque_hash window(128);
    for(int value : {int('a'),int('b'),int('b'),int('a')}){prefix.push_back(value);window.push_back(value);}
    CHECK(!prefix.same(0,1,2,3)); CHECK(!window.same(0,1,2,3));
    CHECK(prefix.ret(0,1) == window.ret(0,1));
    hash_templates::_hash other;other.push_back('a');other.push_back('b');
    CHECK(prefix.ret(0,1) == other.ret(0,1));
    prefix.clear(); window.clear(128);
    for(int value : {1,0,1}){prefix.push_back(value);window.push_back(value);}
    CHECK(prefix.ret(0,0) == prefix.ret(0,1));
    CHECK(!prefix.same(0,0,0,1)); CHECK(!window.same(0,0,0,1));
    CHECK(prefix.same(0,0,2,2)); CHECK(window.same(0,0,2,2));
    CHECK(prefix.ret(0,-1) == 0); CHECK(window.ret(0,-1) == 0);
    CHECK(prefix.same(0,-1,2,1)); CHECK(window.same(0,-1,2,1));
}
long long naive_hash(const vector<int>& values,int l,int r,int key,int mod){
    long long result = 0, power = 1;
    for(int i=l;i<=r;++i){long long value=(values[i]%(long long)mod+mod)%mod;result=(result+value*power)%mod;power=power*key%mod;}
    return result;
}
void check_hash_operations(){
    const pair<int,int> key{37,41}, mod{1'000'000'007,1'000'000'009};
    hash_templates::_hash prefix(key,mod);hash_templates::_deque_hash window(256,key,mod);
    vector<int> values;mt19937 rng(809);
    for(int round=0;round<160;++round){
        int op=rng()%4;
        if(values.empty() || op<2){
            int value=(int)(rng()%301)-150;
            if(op==0){window.push_front(value);values.insert(values.begin(),value);}
            else {window.push_back(value);values.push_back(value);}
        } else if(op==2){window.pop_front();values.erase(values.begin());}
        else {window.pop_back();values.pop_back();}
        prefix.reset();for(int value:values)prefix.push_back(value);
        CHECK(window.size() == (int)values.size()); CHECK(prefix.size() == window.size());
        for(int trial=0;trial<5 && !values.empty();++trial){
            int l=rng()%values.size(),r=rng()%values.size();if(l>r)swap(l,r);
            long long expected=(naive_hash(values,l,r,key.first,mod.first)<<32)+naive_hash(values,l,r,key.second,mod.second);
            CHECK(prefix.ret(l,r)==expected); CHECK(window.ret(l,r)==expected);
            CHECK(window.same(l,r,l,r));
        }
    }
}
namespace bidirectional_checks {
using namespace range_templates;
struct text_monoid {
    using node=string;
    static node id(){return "";}
    static node leaf(){return "";}
    static node op(const node&a,const node&b){return a+b;}
};
struct matrix_monoid {
    struct node {
        long long a,b,c,d;
        node(long long a,long long b,long long c,long long d):a(a),b(b),c(c),d(d){}
        bool operator==(const node&)const=default;
    };
    static node id(){return {1,0,0,1};}
    static node leaf(){return id();}
    static node op(const node&l,const node&r){
        return {l.a*r.a+l.b*r.c,l.a*r.b+l.b*r.d,l.c*r.a+l.d*r.c,l.c*r.b+l.d*r.d};
    }
};
struct text_action {
    struct node {
        string text;int sz;
        node(string text,int sz):text(move(text)),sz(sz){}
    };
    struct lazy {
        bool has;char value;
        lazy(bool has,char value):has(has),value(value){}
        bool operator==(const lazy&)const=default;
    };
    static node id(){return {"",0};}
    static node leaf(){return {"x",1};}
    static lazy lz_id(){return {false,0};}
    static node op(const node&l,const node&r){return {l.text+r.text,l.sz+r.sz};}
    static void apply(node&value,const lazy&tag){if(tag.has)value.text=string(value.sz,tag.value);}
    static void compose(lazy&old,const lazy&next){if(next.has)old=next;}
};
struct signed_action : bidirectional_action<action> {
    static void apply(node&value,const lazy&tag){
        action::apply(value.forward,tag);action::apply(value.backward,{-tag.v});
    }
};
static_assert(!is_default_constructible_v<bidirectional<monoid>::node>);

void check_monoids(bool base){
    using P=bidirectional<text_monoid>;
    int n=base+2;vector<P::node> input(n+1,P::id());
    input[base]=P::lift("ab");input[base+1]=P::lift("cd");input[n]=P::lift("E");
    _seg<P> tree(n,base);tree.make(input);
    CHECK(tree.query(base,n).forward=="abcdE");CHECK(tree.query(base,n).backward=="Ecdab");
    CHECK(tree.query(base,base+1).forward=="abcd");CHECK(tree.query(base,base+1).backward=="cdab");
    CHECK(tree.max_right(base,[](const auto&v){return v.forward.size()<=2;})==base);
    CHECK(tree.query(n+1,n).forward.empty());CHECK(tree.query(n+1,n).backward.empty());
    tree.set(base+1,{"F","R"});
    CHECK(tree.query(base,n).forward=="abFE");CHECK(tree.query(base,n).backward=="ERab");
    CHECK(tree.min_left(n,[](const auto&v){return v.forward.size()<=2;})==base+1);
    tree.make(vector<P::node>{});CHECK(tree.query(base,n).forward.empty());
    tree.make(input);CHECK(tree.query(base,n).backward=="Ecdab");
    using M=bidirectional<matrix_monoid>;
    _seg<M> matrix(base+1,base);
    matrix.set(base,M::lift({1,1,0,1}));matrix.set(base+1,M::lift({1,0,1,1}));
    auto result=matrix.query(base,base+1);
    CHECK((result.forward==matrix_monoid::node{2,1,1,1}));
    CHECK((result.backward==matrix_monoid::node{1,1,1,2}));
    CHECK(matrix.query(base+2,base+1).forward==matrix_monoid::id());
    using S=bidirectional<monoid>;
    _seg<S> costs(base+1,base);costs.set(base,{{3},{-3}});costs.set(base+1,{{5},{-5}});
    CHECK(costs.query(base,base+1).forward.v==8);CHECK(costs.query(base,base+1).backward.v==-8);
    _seg<S> empty(base-1,base);CHECK(empty.query(base,base).forward.v==0);
}
void check_actions(bool base){
    using A=bidirectional_action<text_action>;
    int n=base+2;vector<A::node> input(n+1,A::id());
    input[base]=A::lift({"ab",1});input[base+1]=A::lift({"cd",1});input[n]=A::lift({"E",1});
    _prop<A> tree(n,base);tree.make(input);
    CHECK(tree.query(base,n).forward.text=="abcdE");CHECK(tree.query(base,n).backward.text=="Ecdab");
    tree.update(base,base+1,{true,'X'});
    CHECK(tree.query(base,n).forward.text=="XXE");CHECK(tree.query(base,n).backward.text=="EXX");
    tree.update(base+1,n,{true,'Y'});
    CHECK(tree.query(base,n).forward.text=="XYY");CHECK(tree.query(base,n).backward.text=="YYX");
    tree.update(base,n,A::lz_id());CHECK(tree.query(base,n).forward.text=="XYY");
    CHECK(tree.query(base,n).forward.sz==3);CHECK(tree.query(base,n).backward.sz==3);
    CHECK(tree.query(n+1,n).forward.sz==0);CHECK(tree.query(n+1,n).backward.sz==0);
    tree.clear(n,base);CHECK(tree.query(base,n).forward.text=="xxx");
    tree.make(vector<A::node>{});CHECK(tree.query(base,n).backward.text.empty());
    using Add=bidirectional_action<action>;
    vector<Add::node> costs{Add::id(),{{3,1},{-3,1}},{{5,1},{-5,1}}};
    _prop<Add> uniform{span<const Add::node>(costs)};uniform.update(1,2,{2});
    CHECK(uniform.query(1,2).forward.v==12);CHECK(uniform.query(1,2).backward.v==-4);
    _prop<signed_action> directed{span<const signed_action::node>(costs)};directed.update(1,2,{2});
    CHECK(directed.query(1,2).forward.v==12);CHECK(directed.query(1,2).backward.v==-12);
    directed.update(1,2,{-2});CHECK(directed.query(1,2).forward.v==8);CHECK(directed.query(1,2).backward.v==-8);
    _prop<Add> zero(n,base);zero.update(base,n,{3});
    CHECK(zero.query(base,n).forward.v==9);CHECK(zero.query(base,n).backward.v==9);
    CHECK(zero.query(base,n).forward.sz==3);CHECK(zero.query(base,n).backward.sz==3);
}

vector<int> path_vertices(const vector<vector<int>>& adj,int st,int en){
    vector<int> parent(adj.size(),-1);queue<int> q;q.push(st);parent[st]=st;
    while(!q.empty()){
        int cur=q.front();q.pop();
        for(int nxt:adj[cur])if(parent[nxt]==-1){parent[nxt]=cur;q.push(nxt);}
    }
    if(parent[en]==-1)return {};
    vector<int> path;for(int cur=en;;cur=parent[cur]){path.push_back(cur);if(cur==st)break;}
    reverse(path.begin(),path.end());return path;
}
vector<vector<int>> sample_tree(){
    vector<vector<int>> adj(7);
    for(auto [a,b]:vector<pair<int,int>>{{0,1},{0,2},{1,3},{1,4},{2,5},{5,6}}){adj[a].push_back(b);adj[b].push_back(a);}
    return adj;
}
template<class H>
void add_edges(H& h,const vector<vector<int>>& adj,int base){
    for(int a=0;a<(int)adj.size();++a)for(int b:adj[a])if(a<b)h.add(a+base,b+base);
}
template<class H>
vector<int> storage_vertices(const H& h,int count,int base){
    vector<int> at(count,-1);
    for(int v=0;v<count;++v){
        int segments=0;
        h.it_path(v+base,v+base,[&](int l,int r,bool rev){CHECK(l==r);CHECK(!rev);CHECK(l>=0&&l<count);CHECK(at[l]==-1);at[l]=v;++segments;});
        CHECK(segments==1);
    }
    return at;
}
void check_hld_order(bool base){
    using P=bidirectional<text_monoid>;
    const int count=7,n=base+count-1;auto adj=sample_tree();
    _hld_path<_seg<P>> h(n,0,base);add_edges(h,adj,base);h.build();
    vector<string> values{"A","bc","D","ef","G","hi","J"};
    for(int v=0;v<count;++v)h.set(v+base,P::lift(values[v]));
    auto at=storage_vertices(h,count,base);
    auto verify=[&]{
        for(int st=0;st<count;++st)for(int en=0;en<count;++en){
            auto path=path_vertices(adj,st,en);string forward,backward;
            for(int v:path)forward+=values[v];for(auto it=path.rbegin();it!=path.rend();++it)backward+=values[*it];
            auto result=h.query(st+base,en+base);
            CHECK(result.forward==forward);CHECK(result.backward==backward);
            CHECK(h.query(en+base,st+base).forward==backward);
            vector<int> visited;
            h.it_path(st+base,en+base,[&](int l,int r,bool rev){
                CHECK(l<=r);CHECK(l>=0&&r<count);
                if(rev){for(int i=r;i>=l;--i)visited.push_back(at[i]);}
                else{for(int i=l;i<=r;++i)visited.push_back(at[i]);}
            });
            CHECK(visited==path);
        }
    };
    verify();values[3]="XYZ";h.set(base+3,P::lift(values[3]));verify();h.build();at=storage_vertices(h,count,base);verify();
    _hld_path<_seg<>> sum(n,0,base);add_edges(sum,adj,base);sum.build();for(int v=0;v<count;++v)sum.set(v+base,{v+1});
    for(int st=0;st<count;++st)for(int en=0;en<count;++en){long long expected=0;for(int v:path_vertices(adj,st,en))expected+=v+1;CHECK(sum.query(st+base,en+base).v==expected);}
    using M=bidirectional<matrix_monoid>;_hld_path<_seg<M>> matrix(n,0,base);add_edges(matrix,adj,base);matrix.build();
    vector<matrix_monoid::node> matrices(count,matrix_monoid::id());
    for(int v=0;v<count;++v){matrices[v]=v%2?matrix_monoid::node{1,0,1,1}:matrix_monoid::node{1,1,0,1};matrix.set(v+base,M::lift(matrices[v]));}
    for(int st=0;st<count;++st)for(int en=0;en<count;++en){
        auto path=path_vertices(adj,st,en);auto forward=matrix_monoid::id(),backward=matrix_monoid::id();
        for(int v:path)forward=matrix_monoid::op(forward,matrices[v]);
        for(auto it=path.rbegin();it!=path.rend();++it)backward=matrix_monoid::op(backward,matrices[*it]);
        CHECK(matrix.query(st+base,en+base).forward==forward);CHECK(matrix.query(st+base,en+base).backward==backward);
    }
}
void check_hld_edges(bool base){
    using P=bidirectional<text_monoid>;const int count=7,n=base+count-1;auto adj=sample_tree();
    _hld_path<_seg<P>> vertex(n,0,base),edge(n,1,base);add_edges(vertex,adj,base);add_edges(edge,adj,base);vertex.build();edge.build();
    auto at=storage_vertices(vertex,count,base);vector<int> parent(count,-1);
    for(int v=1;v<count;++v){auto path=path_vertices(adj,0,v);parent[v]=path[path.size()-2];edge.set(v+base,{"d"+to_string(v),"u"+to_string(v)});}
    edge.set(base,{"ROOT-F","ROOT-B"});
    for(int st=0;st<count;++st)for(int en=0;en<count;++en){
        auto path=path_vertices(adj,st,en);string forward,backward;vector<pair<int,int>> expected,visited;
        for(int i=1;i<(int)path.size();++i){int a=path[i-1],b=path[i];forward+=parent[b]==a?"d"+to_string(b):"u"+to_string(a);expected.push_back({a,b});}
        for(int i=(int)path.size()-1;i>0;--i){int a=path[i],b=path[i-1];backward+=parent[b]==a?"d"+to_string(b):"u"+to_string(a);}
        auto result=edge.query(st+base,en+base);CHECK(result.forward==forward);CHECK(result.backward==backward);
        CHECK(edge.query(en+base,st+base).forward==backward);
        edge.it_path(st+base,en+base,[&](int l,int r,bool rev){
            CHECK(l<=r);CHECK(l>=0&&r<count);
            if(rev){for(int i=r;i>=l;--i){int v=at[i];CHECK(v!=0);visited.push_back({v,parent[v]});}}
            else{for(int i=l;i<=r;++i){int v=at[i];CHECK(v!=0);visited.push_back({parent[v],v});}}
        });CHECK(visited==expected);
    }
    edge.build();CHECK(edge.query(base+3,base+6).forward=="u3u1d2d5d6");
}
void check_hld_lazy(bool base){
    using P=bidirectional_action<text_action>;const int count=7,n=base+count-1;auto adj=sample_tree();
    _hld_path<_prop<P>> h(n,0,base);add_edges(h,adj,base);h.build();vector<char> values(count,'x');mt19937 rng(927);
    for(int round=0;round<30;++round){
        int st=rng()%count,en=rng()%count;char c='A'+rng()%4;h.update(st+base,en+base,{true,c});
        for(int v:path_vertices(adj,st,en))values[v]=c;
        if(round%7==0)h.build();
        for(int a=0;a<count;++a)for(int b=0;b<count;++b){
            auto path=path_vertices(adj,a,b);string forward,backward;
            for(int v:path)forward+=values[v];for(auto it=path.rbegin();it!=path.rend();++it)backward+=values[*it];
            auto result=h.query(a+base,b+base);CHECK(result.forward.text==forward);CHECK(result.backward.text==backward);
            CHECK(result.forward.sz==(int)path.size());CHECK(result.backward.sz==(int)path.size());
        }
    }
    _hld_path<_prop<signed_action>> edge(n,1,base);add_edges(edge,adj,base);edge.build();vector<long long> weights(count,0);vector<int> parent(count,-1);
    for(int v=1;v<count;++v){auto path=path_vertices(adj,0,v);parent[v]=path[path.size()-2];}
    for(int round=0;round<25;++round){
        int st=rng()%count,en=rng()%count;long long delta=(int)(rng()%9)-4;auto path=path_vertices(adj,st,en);
        if(round%2){
            edge.it_path(st+base,en+base,[&](int l,int r,bool rev){edge.seg.update(l,r,{rev?-delta:delta});});
            for(int i=1;i<(int)path.size();++i){int a=path[i-1],b=path[i];if(parent[b]==a)weights[b]+=delta;else weights[a]-=delta;}
        }else{
            edge.update(st+base,en+base,{delta});
            for(int i=1;i<(int)path.size();++i){int a=path[i-1],b=path[i];weights[parent[b]==a?b:a]+=delta;}
        }
        if(round%6==0)edge.build();
        for(int a=0;a<count;++a)for(int b=0;b<count;++b){
            auto p=path_vertices(adj,a,b);long long expected=0;
            for(int i=1;i<(int)p.size();++i){int u=p[i-1],v=p[i];expected+=parent[v]==u?weights[v]:-weights[u];}
            auto result=edge.query(a+base,b+base);CHECK(result.forward.v==expected);CHECK(result.backward.v==-expected);CHECK(result.forward.sz==(int)p.size()-1);
        }
    }
}
void check_hld_forest(bool base){
    using P=bidirectional<text_monoid>;int count=8,n=base+count-1;vector<vector<int>> adj(count);
    auto add=[&](int a,int b){adj[a].push_back(b);adj[b].push_back(a);};add(0,1);add(1,2);add(3,4);add(4,5);
    _hld_path<_seg<P>> h(n,0,base);add_edges(h,adj,base);h.build();vector<string> values;
    for(int v=0;v<count;++v){values.push_back("["+to_string(v)+"]");h.set(v+base,P::lift(values[v]));}
    auto verify=[&]{for(int a=0;a<count;++a)for(int b=0;b<count;++b){auto path=path_vertices(adj,a,b);if(path.empty())continue;
        string expected;for(int v:path)expected+=values[v];CHECK(h.query(a+base,b+base).forward==expected);}};
    verify();for(auto [a,b]:vector<pair<int,int>>{{2,3},{0,6},{6,7}}){add(a,b);h.add(a+base,b+base);}h.build();verify();h.build();verify();
    _hld_path<_seg<P>> empty(0,0,base);empty.build();
}
template<class H>
concept owns_hld_backend = requires(H& h){ h.seg; };
static_assert(!owns_hld_backend<_hld>);
static_assert(owns_hld_backend<_hld_path<_seg<>>>);
static_assert(is_base_of_v<_hld,_hld_path<_seg<>>>);

class point_backend {
public:
    using node = monoid::node;
    vector<node> values;
    node id() const{ return monoid::id(); }
    node op(const node& a,const node& b) const{ return monoid::op(a,b); }
    void clear(int n){ values.assign(n+1,id()); }
    void set(int i,const node& v){ values[i]=v; }
    node query(int l,int r) const{
        node result=id();
        for(int i=l;i<=r;i++) result=op(result,values[i]);
        return result;
    }
};

void check_hld_parts(bool base,int mode,unsigned seed){
    mt19937 rng(seed);int count=1+rng()%20,n=base+count-1;
    _hld plain(n,mode,base);_hld_path<_seg<>> wrapped(n,mode,base);
    vector<vector<int>> adj(count);
    auto add=[&](int a,int b){
        if(seed%2){plain.addsol(a+base,b+base);wrapped.addsol(a+base,b+base);}
        else{plain.add(a+base,b+base);wrapped.add(a+base,b+base);}
        adj[a].push_back(b);adj[b].push_back(a);
    };
    for(int v=1;v<count;v++)if(rng()%4)add(rng()%v,v);
    plain.build();wrapped.build();
    for(int v=0;v<count;v++)wrapped.set(v+base,{v+1});
    auto verify=[&]{
        vector<int> parent(count,-1),depth(count,-1),comp(count,-1);
        for(int root=0;root<count;root++)if(depth[root]==-1){
            queue<int> q;q.push(root);depth[root]=0;comp[root]=root;
            while(!q.empty()){
                int cur=q.front();q.pop();
                for(int v:adj[cur])if(depth[v]==-1){parent[v]=cur;depth[v]=depth[cur]+1;comp[v]=root;q.push(v);}
            }
        }
        const auto& view=plain;
        vector<int> seen(count,0);
        for(int v=0;v<count;v++){
            int i=view.index(v+base);CHECK(i>=0&&i<count);CHECK(!seen[i]);seen[i]=1;
            CHECK(view.vertex(i)==v+base);CHECK(wrapped.index(v+base)==i);CHECK(wrapped.vertex(i)==v+base);
            CHECK(view.parent(v+base)==(parent[v]==-1?-1:parent[v]+base));CHECK(view.depth(v+base)==depth[v]);
            CHECK(wrapped.parent(v+base)==view.parent(v+base));CHECK(wrapped.depth(v+base)==depth[v]);
            auto [l,r]=view.subtree(v+base);CHECK(l==i+mode);CHECK(r<count);
            CHECK(wrapped.subtree(v+base)==make_pair(l,r));
            vector<int> expected,actual;
            for(int w=0;w<count;w++){
                int cur=w;while(cur!=-1&&cur!=v)cur=parent[cur];
                if(cur==v&&(!mode||w!=v))expected.push_back(w);
            }
            for(int k=l;k<=r;k++)actual.push_back(view.vertex(k)-base);
            sort(actual.begin(),actual.end());CHECK(actual==expected);
            long long total=0;for(int w:expected)total+=w+1;
            CHECK(wrapped.query_sub(v+base).v==total);
        }
        for(int st=0;st<count;st++)for(int en=0;en<count;en++)if(comp[st]==comp[en]){
            auto path=path_vertices(adj,st,en);vector<int> expected;
            if(!mode)expected=path;
            else for(int i=1;i<(int)path.size();i++)expected.push_back(parent[path[i]]==path[i-1]?path[i]:path[i-1]);
            vector<int> actual;vector<tuple<int,int,bool>> intervals,other;
            view.it_path(st+base,en+base,[&](int l,int r,bool rev){
                CHECK(l>=0&&l<=r&&r<count);intervals.push_back({l,r,rev});
                if(rev)for(int i=r;i>=l;i--)actual.push_back(view.vertex(i)-base);
                else for(int i=l;i<=r;i++)actual.push_back(view.vertex(i)-base);
            });
            wrapped.it_path(st+base,en+base,[&](int l,int r,bool rev){other.push_back({l,r,rev});});
            CHECK(actual==expected);CHECK(intervals==other);
            long long total=0;for(int v:expected)total+=v+1;
            CHECK(wrapped.query(st+base,en+base).v==total);
        }
    };
    verify();plain.build();wrapped.build();verify();
    vector<int> roots;for(int v=1;v<count;v++)if(plain.parent(v+base)==-1)roots.push_back(v);
    for(int root:roots)add(0,root);
    plain.build();wrapped.build();verify();
    plain.clear(0,mode,base);wrapped.clear(0,mode,base);plain.build();wrapped.build();
    if(!base){CHECK(plain.index(0)==0);CHECK(plain.vertex(0)==0);CHECK(wrapped.query(0,0).v==0);}
    plain.clear(1,mode,!base);wrapped.clear(1,mode,!base);plain.build();wrapped.build();
    CHECK(plain.vertex(plain.index(!base))==!base);
}

void check_hld_fallback(bool base){
    _hld_path<point_backend> h(base+3,0,base);
    h.add(base,base+1);h.add(base+2,base+3);h.build();
    for(int i=0;i<4;i++)h.set(base+i,{i+1});
    h.add(base+1,base+3);h.build();CHECK(h.query(base,base+2).v==10);
    h.build();for(int i=0;i<4;i++)CHECK(h.query(base+i).v==i+1);
    h.clear(base+1,1,base);h.add(base,base+1);h.build();h.set(base+1,{7});
    CHECK(h.query(base,base+1).v==7);CHECK(h.query_sub(base+1).v==0);
}

void check_hld_subtree(bool base,int mode){
    auto adj=sample_tree();_hld_path<_prop<>> h(base+6,mode,base);
    add_edges(h,adj,base);h.build();h.update_sub(base+1,{5});
    vector<long long> values(7,0);
    values[3]=values[4]=5;if(!mode)values[1]=5;
    for(int v=0;v<7;v++)CHECK(h.query(v+base).v==values[v]);
    CHECK(h.query_sub(base+1).v==(!mode?15:10));
    h.build();h.update_sub(base+3,{2});if(!mode)values[3]+=2;
    for(int v=0;v<7;v++)CHECK(h.query(v+base).v==values[v]);
    long long expected=0;for(int v:path_vertices(adj,3,6))expected+=values[v];
    CHECK(h.query(base+3,base+6).v==expected);
}

void check_hld_direction(){
    for(bool base:{false,true}){
        check_hld_order(base);check_hld_edges(base);check_hld_lazy(base);check_hld_forest(base);check_hld_fallback(base);
        for(int mode=0;mode<2;mode++)check_hld_subtree(base,mode);
        for(int mode=0;mode<2;mode++)for(unsigned seed=1;seed<=16;seed++)check_hld_parts(base,mode,seed);
    }
}

void run(){for(bool base:{false,true}){check_monoids(base);check_actions(base);}}
}

int main(){
    int failures = 0;
    for(auto [name,test]:vector<pair<string,void(*)()>>{{"hld_direction",bidirectional_checks::check_hld_direction},{"bidirectional",bidirectional_checks::run},{"lazy_equality",check_lazy_equality},{"factory_construction",check_factory_construction},{"default_lazy",check_default_lazy},{"explicit_id",check_explicit_id},
        {"affine",check_affine},{"hash_defaults",check_hash_defaults},{"hash_operations",check_hash_operations}}){
        try{test();cout<<"PASS "<<name<<'\n';}catch(const exception& e){++failures;cerr<<"FAIL "<<name<<": "<<e.what()<<'\n';}
    }
    cout<<checks<<" identity/initial-leaf/hash checks, "<<failures<<" failed groups\n";
    return failures != 0;
}
