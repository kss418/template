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

// Default construction deliberately supplies neither the identity nor a leaf.
struct min_policy {
    struct node { long long v; node(long long v = -999) : v(v){} };
    static node id(){ return {1'000'000}; }
    static node leaf(){ return {7}; }
    static node op(const node& a, const node& b){ return {min(a.v,b.v)}; }
};
struct product_policy {
    struct node { long long v; node(long long v = 0) : v(v){} };
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
    struct node { long long v; int sz; node(long long v = -99,int sz = -99):v(v),sz(sz){} };
    struct lazy { long long a,b; lazy(long long a = 1,long long b = 0):a(a),b(b){} };
    static node id(){ return {0,0}; }
    static node leaf(){ return {2,1}; }
    static node op(const node& a,const node& b){ return {a.v+b.v,a.sz+b.sz}; }
    static bool is_lz_id(const lazy& value){ return value.a == 1 && value.b == 0; }
    static void apply(node& value,const lazy& change){ value.v = value.v*change.a+value.sz*change.b; }
    static void compose(lazy& old,const lazy& next){ old = {old.a*next.a,old.b*next.a+next.b}; }
};

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
void check_leaf_construction(){
    for(bool base : {false,true}){
        const int count = 4, n = base+count-1;
        vector<range_templates::action::node> input(n+1);
        range_templates::_prop<> tree(n,base); tree.make(input); tree.update(base,n,{5});
        CHECK(tree.query(base,n).v == 20); CHECK(tree.query(base,n).sz == count);
        input.assign(n+1,{2}); tree.make(input); tree.update(base,n,{3});
        CHECK(tree.query(base,n).v == 20); CHECK(tree.query(base,n).sz == count);
        tree.update(base,n,range_templates::action::lazy{}); CHECK(tree.query(base,n).v == 20);
        input[base] = {7,0}; tree.make(input); tree.update(base,n,{3});
        CHECK(tree.query(base,n).v == 22); CHECK(tree.query(base,n).sz == count-1);
        CHECK(tree.query(n+1,n).v == 0); CHECK(tree.query(n+1,n).sz == 0);
        tree.clear(n,base); tree.update(base,n,{2}); CHECK(tree.query(base,n).v == 8); CHECK(tree.query(base,n).sz == count);
        range_templates::_seg<> plain(n,base); plain.make(vector<range_templates::monoid::node>(n+1));
        plain.set(n,{5}); CHECK(plain.query(base,n).v == 5);
        range_templates::_fw<> fenwick(n,base); fenwick.make(vector<range_templates::group::node>(n+1));
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
        range_templates::_hld<range_templates::_prop<>> hld(n,0,base);
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
int main(){
    int failures = 0;
    for(auto [name,test]:vector<pair<string,void(*)()>>{{"leaf_construction",check_leaf_construction},{"default_lazy",check_default_lazy},{"explicit_id",check_explicit_id},
        {"affine",check_affine},{"hash_defaults",check_hash_defaults},{"hash_operations",check_hash_operations}}){
        try{test();cout<<"PASS "<<name<<'\n';}catch(const exception& e){++failures;cerr<<"FAIL "<<name<<": "<<e.what()<<'\n';}
    }
    cout<<checks<<" identity/initial-leaf/hash checks, "<<failures<<" failed groups\n";
    return failures != 0;
}
