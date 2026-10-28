#define main base_main
#include "base_index.cpp"
#undef main
namespace automata_life {
#include "../Automata.cpp"
}
namespace strings_life {
#include "../String.cpp"
}
namespace numbers_life {
#include "../Number_Theory.cpp"
}
namespace geometry_life {
using numbers_life::_mint;
#include "../Geometry.cpp"
}
class external_mo {
public:
    using node=sum_mo::node;using ret=node;
    static node leaf(){return sum_mo::leaf();}
    static ret id(){return sum_mo::id();}
    static inline long long state=0;
    static void reset(){state=0;}
    static void push_front(int,const node&v,ret&cur){state+=v.v;cur.v=state;}
    static void push_back(int i,const node&v,ret&cur){push_front(i,v,cur);}
    static void pop_front(int,const node&v,ret&cur){state-=v.v;cur.v=state;}
    static void pop_back(int i,const node&v,ret&cur){pop_front(i,v,cur);}
};
long long naive_matches(const vector<string>&patterns,const string&text){
    long long total=0;for(const auto&p:patterns)for(size_t i=0;i+p.size()<=text.size();++i)total+=text.compare(i,p.size(),p)==0;return total;
}
void check_automata(){
    automata_life::_ac<> dense;automata_life::_sparse_ac<> sparse;vector<string>patterns;
    mt19937 rng(617);
    for(int round=0;round<35;++round){
        string p;for(int k=0,len=1+rng()%4;k<len;++k)p+=char('a'+rng()%3);patterns.push_back(p);dense.insert(p);sparse.insert(p);
        string text;for(int i=0;i<40;++i)text+=char('a'+rng()%3);
        for(int rebuild=0;rebuild<2;++rebuild){dense.build();sparse.build();CHECK(dense.count(text)==naive_matches(patterns,text));CHECK(sparse.count(text)==naive_matches(patterns,text));}
    }
}
void check_rebuild(bool base){
    int first=base,n=first+3;
    graphs::_lca l(n,base);l.add(first,first+1);l.add(first+2,n);l.build();CHECK(!l.same_comp(first,n));l.add(first+1,first+2);l.build();CHECK(l.dist(first,n)==3);l.build();CHECK(l.dist(first,n)==3);
    range_templates::_hld_path<range_templates::_seg<>> h(n,0,base);h.add(first,first+1);h.add(first+2,n);h.build();for(int i=first;i<=n;++i)h.set(i,{i-first+1});h.add(first+1,first+2);h.build();CHECK(h.query(first,n).v==10);h.build();for(int i=first;i<=n;++i)CHECK(h.query(i).v==i-first+1);
    range_templates::_hld_path<range_templates::_prop<>> lazy(n,0,base);lazy.add(first,first+1);lazy.add(first+2,n);lazy.build();vector<range_templates::action::node> init(n+1,range_templates::action::leaf());for(int i=0;i<4;++i)init[i]={i+1,1};lazy.seg.make(init);lazy.update(first,first+1,{2});lazy.add(first+1,first+2);lazy.build();CHECK(lazy.query(first,n).v==14);lazy.update(first,n,{1});CHECK(lazy.query(first,n).v==18);
    graphs::_mst<> mst(n,base);for(int i=first;i<n;++i)mst.add(i,i+1,3);CHECK(mst.build());CHECK(mst.ret().v==9);CHECK(mst.build());CHECK(mst.ret().v==9);mst.add(first,n,1);CHECK(mst.build());CHECK(mst.ret().v==7);
    graphs::_bm bm(n,n,base);bm.addsol(first,first);CHECK(bm.build()==1);CHECK(bm.build()==1);CHECK(bm.ret()==1);bm.addsol(n,n);CHECK(bm.build()==2);CHECK(bm.ret()==2);
    graphs::_hungarian<> assign(first+1,base);assign.set(first,first,1);assign.set(first+1,first+1,2);assign.set(first,first+1,100);assign.set(first+1,first,100);CHECK(assign.build()==3);CHECK(assign.build()==3);assign.set(first,first,20);CHECK(assign.build()==22);
    flows::_flow<> flow(n,base);flow.addsol(first,n,5);CHECK(flow.build(first,n)==5);CHECK(flow.build(first,n)==5);CHECK(flow.augment(first,n)==0);flow.reset();CHECK(flow.augment(first,n,2)==2);CHECK(flow.augment(first,n,2)==2);CHECK(flow.build(first,n)==5);flow.addsol(first,n,3);CHECK(flow.build(first,n)==8);
    flows::_mcmf<> cost(n,1,base);cost.addsol(first,n,2,3);CHECK((cost.build(first,n)==pair<long long,long long>{2,6}));CHECK((cost.build(first,n)==pair<long long,long long>{2,6}));cost.addsol(first,n,1,1);CHECK((cost.build(first,n)==pair<long long,long long>{3,7}));CHECK((cost.augment(first,n)==pair<long long,long long>{0,0}));CHECK((cost.build(first,n,1)==pair<long long,long long>{1,1}));CHECK((cost.build(first,n,1)==pair<long long,long long>{1,1}));
    paths::_dij<> dij(n,base);dij.addsol(first,n,5);dij.build(first);CHECK(dij.ret(n)==5);dij.addsol(first,n,2);dij.build(first);CHECK(dij.ret(n)==2);dij.build(n);CHECK(dij.ret(n)==0);
    paths::_floyd<> floyd(n,base);floyd.addsol(first,first+1,5);floyd.addsol(first+1,n,3);floyd.build();CHECK(floyd.ret(first,n)==8);floyd.build();CHECK(floyd.ret(first,n)==8);floyd.addsol(first,n,2);floyd.build();CHECK(floyd.ret(first,n)==2);
    graphs::_scc scc(n,base);scc.addsol(first,n);scc.build();scc.build();CHECK(!scc.same(first,n));scc.addsol(n,first);scc.build();CHECK(scc.same(first,n));
    graphs::_2sat sat(n,base);sat.add_true(sat.pos(first));CHECK(sat.build());CHECK(sat.build());sat.add_false(sat.pos(first));CHECK(!sat.build());
    graphs::_vtree vt(n,base);for(int i=first;i<n;++i)vt.add(i,i+1);vt.build();CHECK(vt.make({first,n}).size()==2);vt.build(n);CHECK(vt.dist_tree(first,n)==3);CHECK(vt.make({first,n}).size()==2);
    graphs::_3cycle triangle(n,base);triangle.add(first,first+1);triangle.add(first+1,first+2);triangle.add(first+2,first);triangle.build();CHECK(triangle.ret()==1);triangle.build();CHECK(triangle.ret()==1);triangle.add(first,n);triangle.add(first+1,n);triangle.build();CHECK(triangle.ret()==2);
    graphs::_4cycle square(n,base);for(int i=first;i<=n;++i)square.add(i,i==n?first:i+1);square.build();CHECK(square.ret()==1);square.build();CHECK(square.ret()==1);
    external_mo::state=123;
    offline::_mos<external_mo> empty_mo(n,0,base);empty_mo.build();
    CHECK(external_mo::state==0);CHECK(empty_mo.get_result().empty());
    external_mo::state=456;empty_mo.build();CHECK(external_mo::state==0);
    offline::_mos<external_mo> mo(n,0,base);for(int i=first;i<=n;++i)mo.set(i,{i-first+1});mo.add(first,n);mo.build();CHECK(mo.get_result()[0].v==10);mo.build();CHECK(mo.get_result()[0].v==10);mo.set(n,{100});mo.build();CHECK(mo.get_result()[0].v==106);
}
void check_bulk_and_numeric(){
    vector<range_templates::monoid::node>a{{99},{2},{3}};range_templates::_seg<> seg;seg.make(a);CHECK(seg.query(1,2).v==5);a[1]={7};seg.make(a);CHECK(seg.query(1,2).v==10);
    strings_life::_kmp<> k;k.make(string("ab"));int found=0;k.it_match(string("abab"),[&](int){++found;});CHECK(found==2);k.make(string("b"));found=0;k.it_match(string("abab"),[&](int){++found;});CHECK(found==2);
    strings_life::_sa<> sa;sa.make(string("aba"));CHECK(sa.get_sa()==vector<int>({2,0,1}));sa.make(string(""));CHECK(sa.get_sa().empty());
    strings_life::_mnc<> m;m.make(string("aba"));CHECK(m.max_len()==3);m.make(string("x"));CHECK(m.max_len()==1);
    strings_life::_z<> z;z.make(string("aaa"));CHECK(z.result()==vector<int>({3,2,1}));z.make(string("ab"));CHECK(z.result()==vector<int>({2,0}));
    numbers_life::_es<20> es;auto primes=es.ret();es.build();CHECK(es.ret()==primes);es.build();CHECK(es.ret()==primes);
    numbers_life::_ls sieve(20,3);CHECK(sieve.phi(18)==6);sieve.build();CHECK(sieve.phi(18)==6);CHECK(sieve.prime==primes);sieve.n=30;sieve.build();CHECK(sieve.is_prime(29));
    numbers_life::_gcd gcd;auto[g,a1,b1]=gcd.build(30,18);CHECK(g==6 && 30*a1+18*b1==6);
    geometry_life::_ch hull;for(auto p:vector<pair<long long,long long>>{{0,0},{1,0},{1,1},{0,1}})hull.add(p);hull.build();CHECK(hull.ret().size()==4);hull.build();CHECK(hull.ret().size()==4);hull.add(0,0);hull.build();CHECK(hull.ret().size()==4);
    geometry_life::_rc diameter;for(auto p:vector<pair<long long,long long>>{{0,0},{1,0},{1,1},{0,1}})diameter.add(p);diameter.build();CHECK(abs(diameter.dist()-sqrtl(2))<1e-9);diameter.build();CHECK(abs(diameter.dist()-sqrtl(2))<1e-9);diameter.add(2,0);diameter.build();CHECK(abs(diameter.dist()-sqrtl(5))<1e-9);
    offline::_lcs lcs("ab","ac");lcs.build();CHECK(lcs.ret()==1);lcs.b="ab";lcs.build();CHECK(lcs.ret()==2);
    offline::_pbs pbs(0,4);pbs.add({0});pbs.build();CHECK(pbs.l[0]==0);pbs.lo=2;pbs.hi=5;pbs.build();CHECK(pbs.l[0]==2);
}
int main(){
    if(base_main())return 1;
    try{check_automata();for(bool base:{false,true})check_rebuild(base);check_bulk_and_numeric();cout<<"PASS "<<checks<<" combined base/lifecycle checks\n";return 0;}
    catch(const exception&e){cerr<<"FAIL "<<e.what()<<'\n';return 1;}
}
