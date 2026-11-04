#include <bits/stdc++.h>
namespace fft_templates {
#include "../FFT.cpp"
}
namespace numeric_templates {
#include "../Number_Theory.cpp"
}
namespace combinatorics {
using numeric_templates::_mint;
using numeric_templates::_dynamic_mint;
using numeric_templates::_crt;
#include "../Combinatorics.cpp"
}
namespace geometry {
using numeric_templates::_mint;
using numeric_templates::_dynamic_mint;
#include "../Geometry.cpp"
}
namespace automata {
#include "../Automata.cpp"
}
namespace strings {
#include "../String.cpp"
}
namespace offline {
#include "../Others.cpp"
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

template<class T>
concept fft_make_input = requires(const fft_templates::_fft<8>& f,const vector<T>& values){f.make(values);};
template<class T>
concept fft_inverse_output = requires(const fft_templates::_fft<8>& f,const fft_templates::_fft<8>::node& value){f.template inverse<T>(value);};
static_assert(fft_make_input<int> && fft_make_input<long long> && fft_make_input<unsigned>);
static_assert(fft_make_input<bool> && fft_make_input<__int128_t>);
static_assert(!fft_make_input<float> && !fft_make_input<double> && !fft_make_input<complex<double>>);
static_assert(fft_inverse_output<int> && fft_inverse_output<long long>);
static_assert(!fft_inverse_output<double>);
template<class A, class B>
concept fft_addable = requires(A a,const B& b){a+b;a+=b;};
template<class A, class B>
concept fft_multipliable = requires(A a,const B& b){a*b;a*=b;};
template<class F, class N>
concept fft_invertible = requires(const F& f,const N& n){f.inverse(n);};
using fft_node8=fft_templates::_fft<8>::node;
using fft_node16=fft_templates::_fft<16>::node;
static_assert(fft_addable<fft_node8,fft_node8> && fft_multipliable<fft_node8,fft_node8>);
static_assert(!fft_addable<fft_node8,fft_node16> && !fft_multipliable<fft_node8,fft_node16>);
static_assert(fft_invertible<fft_templates::_fft<8>,fft_node8>);
static_assert(!fft_invertible<fft_templates::_fft<8>,fft_node16>);
static_assert(is_default_constructible_v<fft_templates::_fft<8>>);
static_assert(!is_constructible_v<fft_templates::_fft<8>,size_t>);

void check_fft_arrays(){
    using value=long long;
    auto add=[](vector<value> a,const vector<value>& b){
        a.resize(max(a.size(),b.size()),0);for(size_t i=0;i<b.size();++i)a[i]+=b[i];return a;
    };
    auto mul=[](const vector<value>& a,const vector<value>& b){
        if(a.empty()||b.empty())return vector<value>{};
        vector<value> result(a.size()+b.size()-1,0);
        for(size_t i=0;i<a.size();++i)for(size_t j=0;j<b.size();++j)result[i+j]+=a[i]*b[j];
        return result;
    };
    fft_templates::_fft<0> empty;auto zero=empty.make(vector<int>{});CHECK(empty.inverse(zero).empty());
    CHECK(empty.inverse(zero+zero).empty());CHECK(empty.inverse(zero*zero).empty());
    fft_templates::_fft<1> single;auto constant=single.make(vector<int>{7});CHECK(single.inverse(constant*constant)==vector<value>{49});
    fft_templates::_fft<3> small;const vector<int> input{1,2};auto original=small.make(input);
    CHECK(small.inverse(original*original)==vector<value>({1,4,4}));CHECK(input==vector<int>({1,2}));
    auto square=original;square*=square;CHECK(small.inverse(square)==vector<value>({1,4,4}));
    fft_templates::_fft<3> same_size;auto other=same_size.make(vector<value>{3,4});
    CHECK(small.inverse(original*other+original)==vector<value>({4,12,8}));
    fft_templates::_fft<4> unsigned_plan;auto ua=unsigned_plan.make(vector<unsigned>{1,2});auto ub=unsigned_plan.make(vector<bool>{1,0,1});
    CHECK(unsigned_plan.inverse<unsigned>(ua*ub)==vector<unsigned>({1,2,1,2}));
    auto wide=unsigned_plan.make(vector<__int128_t>{-2,3});CHECK(unsigned_plan.inverse(wide)==vector<value>({-2,3}));
    mt19937 rng(921);
    for(int la=0;la<=9;++la)for(int lb=0;lb<=9;++lb)for(int trial=0;trial<3;++trial){
        vector<value> a(la);vector<int> b(lb);
        for(auto& v:a)v=(int)(rng()%21)-10;
        for(auto& v:b)v=(int)(rng()%21)-10;
        const auto saved_a=a;const auto saved_b=b;vector<value> bv(all(b));
        auto product=mul(a,bv),squared=mul(a,a),triple=mul(product,a);
        fft_templates::_fft<32> plan;const auto& read=plan;auto fa=read.make(a),fb=read.make(b);
        CHECK(read.inverse(fa)==a);CHECK(read.inverse(fb)==bv);
        CHECK(read.inverse(fa+fb)==add(a,bv));CHECK(read.inverse(fa*fb)==product);
        CHECK(read.inverse(fa*fb+fa)==add(product,a));CHECK(read.inverse((fa*fb)*fa)==triple);
        auto inplace=fa;inplace*=inplace;CHECK(read.inverse(inplace)==squared);
        inplace=fa;inplace+=inplace;CHECK(read.inverse(inplace)==add(a,a));
        auto ints=read.inverse<int>(fa*fb);CHECK(ints==vector<int>(all(product)));
        CHECK(read.inverse(fa)==a);CHECK(read.inverse(fb)==bv);CHECK(a==saved_a);CHECK(b==saved_b);
    }
    for(int length:{16,17,31,32,33,64,65}){
        vector<value> a(length),b(length-1);
        for(auto& v:a)v=(int)(rng()%2001)-1000;
        for(auto& v:b)v=(int)(rng()%2001)-1000;
        fft_templates::_fft<129> plan;auto fa=plan.make(a),fb=plan.make(b);
        CHECK(plan.inverse(fa*fb)==mul(a,b));CHECK(plan.inverse(fa+fb)==add(a,b));
    }
    auto detached=[](){fft_templates::_fft<8> plan;return plan.make(vector<int>{2,3});}();
    fft_templates::_fft<8> reader;CHECK(reader.inverse(detached*detached)==vector<value>({4,12,9}));
}

void check_fft_output(){
    fft_templates::_fft<8> plan;
    const auto a=plan.make(vector<int>{3,2,1});
    ostringstream out;out<<a<<':'<<a;CHECK(out.str()=="123:123");
    CHECK(plan.inverse(a)==vector<long long>({3,2,1}));
    ostringstream zeros;zeros<<plan.make(vector<int>{0,0,1});CHECK(zeros.str()=="100");
    ostringstream coefficients;coefficients<<plan.make(vector<int>{12,34});CHECK(coefficients.str()=="3412");
    ostringstream zero;zero<<plan.make(vector<int>{0});CHECK(zero.str()=="0");
    fft_templates::_fft<0> empty;ostringstream blank;blank<<empty.make(vector<int>{});CHECK(blank.str()=="0");
    const auto nine=plan.make(vector<int>{9,9});
    ostringstream product;product<<(nine*nine);CHECK(product.str()=="8116281");
    CHECK(plan.inverse(nine)==vector<long long>({9,9}));
}

void check_fft_carry(){
    using value=long long;fft_templates::_fft<8> plan;
    const auto a=plan.make(vector<int>{9,9});const auto square=a*a;
    auto decimal=plan.carry(square);
    static_assert(same_as<decltype(decimal),fft_templates::_fft<8>::node>);
    CHECK(plan.inverse(decimal)==vector<value>({1,0,8,9}));
    ostringstream out;out<<decimal;CHECK(out.str()=="9801");
    CHECK(plan.inverse(plan.carry(decimal))==vector<value>({1,0,8,9}));
    CHECK(plan.inverse(square)==vector<value>({81,162,81}));
    CHECK(plan.inverse(plan.carry(decimal*plan.make(vector<int>{2})))==vector<value>({2,0,6,9,1}));
    CHECK(plan.inverse(plan.carry(plan.make(vector<int>{3,2,1}),2))==vector<value>({1,1,0,1}));
    CHECK(plan.inverse(plan.carry(plan.make(vector<int>{31,31}),16))==vector<value>({15,0,2}));
    CHECK(plan.inverse(plan.carry(plan.make(vector<int>{0,0,0})))==vector<value>{0});
    CHECK(plan.inverse(plan.carry(plan.make(vector<int>{9,0,0})))==vector<value>{9});
    CHECK(plan.inverse(plan.carry(plan.make(vector<int>{81})))==vector<value>({1,8}));
    auto empty=plan.carry(plan.make(vector<int>{}));CHECK(plan.inverse(empty).empty());
    ostringstream empty_out;empty_out<<empty;CHECK(empty_out.str()=="0");
    fft_templates::_fft<0> no_capacity;CHECK(no_capacity.inverse(no_capacity.carry(no_capacity.make(vector<int>{}))).empty());
    auto decode=[](const vector<value>& digits,value radix){
        __int128_t result=0;for(size_t i=digits.size();i--;)result=result*radix+digits[i];return result;
    };
    mt19937 rng(461);fft_templates::_fft<64> random_plan;
    for(value radix:{2,3,10,16,1000})for(int length=0;length<=8;++length)for(int trial=0;trial<5;++trial){
        vector<value> input(length);for(auto& coefficient:input)coefficient=rng()%1000;
        auto source=random_plan.make(input);auto normalized=random_plan.carry(source,radix);
        auto digits=random_plan.inverse(normalized);
        CHECK(decode(digits,radix)==decode(input,radix));
        for(auto digit:digits)CHECK(0<=digit && digit<radix);
        CHECK(digits.empty() || digits.size()==1 || digits.back()!=0);
        CHECK(random_plan.inverse(source)==input);
        CHECK(random_plan.inverse(random_plan.carry(normalized,radix))==digits);
    }
    fft_templates::_fft<128> wide;value big=(1LL<<62)+(1LL<<61);
    auto source=wide.make(vector<value>{big,big});auto coefficients=wide.inverse(source);
    auto digits=wide.inverse(wide.carry(source,2));
    CHECK(decode(digits,2)==decode(coefficients,2));for(auto digit:digits)CHECK(digit==0 || digit==1);
}

template<class T>
concept rc_point_input = requires(T& value){value.add(1LL,2LL);};

void check_rc_split(){
    using hull_type=geometry::_ch;using diameter_type=geometry::_rc;
    using point=pair<long long,long long>;
    static_assert(same_as<decltype(declval<const hull_type&>().ret()),const vector<point>&>);
    static_assert(same_as<decltype(declval<const hull_type&>().idx()),const vector<hull_type::node>&>);
    static_assert(same_as<decltype(declval<const diameter_type&>().ret()),const pair<point,point>&>);
    static_assert(same_as<decltype(declval<const diameter_type&>().idx()),const point&>);
    static_assert(!rc_point_input<diameter_type>);
    auto distance=[](point a,point b){long long dx=a.first-b.first,dy=a.second-b.second;return dx*dx+dy*dy;};
    auto cross=[](point a,point b,point c){return (__int128_t)(b.first-a.first)*(c.second-a.second)-(__int128_t)(b.second-a.second)*(c.first-a.first);};
    auto brute=[&](const vector<point>& input){long long mx=0;for(auto a:input)for(auto b:input)mx=max(mx,distance(a,b));return mx;};
    auto verify=[&](const diameter_type& d,const vector<point>& input){
        long long mx=brute(input);CHECK(d.dist2()==mx);CHECK(abs(d.dist()-sqrtl((long double)mx))<1e-9L);
        const auto& [a,b]=d.ret();auto [i,j]=d.idx();
        if(input.empty()){CHECK(i==-1 && j==-1);return;}
        CHECK(0<=i && i<(long long)input.size() && 0<=j && j<(long long)input.size());
        CHECK(input[i]==a && input[j]==b);CHECK(distance(a,b)==mx);
    };
    auto run=[&](const vector<point>& input){
        hull_type hull;for(auto p:input)hull.add(p);
        auto original=hull.arr;hull.build();CHECK(hull.arr.size()==original.size());
        for(size_t i=0;i<original.size();++i)CHECK(hull.arr[i].x==original[i].x && hull.arr[i].y==original[i].y && hull.arr[i].idx==original[i].idx);
        const auto& h=as_const(hull);const auto saved_points=h.ret();vector<long long> saved_ids;
        CHECK(h.ret().size()==h.idx().size());
        set<point> seen;
        for(size_t i=0;i<h.idx().size();++i){
            const auto& node=h.idx()[i];point p={node.x,node.y};CHECK(seen.insert(p).second);
            CHECK(p==h.ret()[i] && input[node.idx]==p);
            CHECK(node.idx==find(all(input),p)-input.begin());saved_ids.push_back(node.idx);
        }
        if(!h.ret().empty())CHECK(h.ret()[0]==*min_element(all(h.ret())));
        if(h.ret().size()>=3)for(size_t i=0;i<h.ret().size();++i){
            auto a=h.ret()[i],b=h.ret()[(i+1)%h.ret().size()],c=h.ret()[(i+2)%h.ret().size()];
            CHECK(cross(a,b,c)>0);for(auto p:input)CHECK(cross(a,b,p)>=0);
        }
        diameter_type indexed;indexed.build(h.idx());verify(indexed,input);
        diameter_type coords;coords.build(h.ret());verify(coords,h.ret());
        auto polygon=h.ret();
        for(int direction=0;direction<2;++direction){
            reverse(all(polygon));if(!polygon.empty())rotate(polygon.begin(),polygon.begin()+1,polygon.end());
            coords.build(polygon);verify(coords,polygon);
        }
        auto first=indexed.ret();auto ids=indexed.idx();indexed.build();verify(indexed,input);
        CHECK(indexed.ret()==first && indexed.idx()==ids);
        hull.build();CHECK(hull.ret()==saved_points);for(size_t i=0;i<saved_ids.size();++i)CHECK(hull.idx()[i].idx==saved_ids[i]);
        point extra=input.empty()?point{90,95}:point{input[0].first+90,input[0].second+95};
        hull.add(extra);hull.build();verify(indexed,input);
        auto extended=input;extended.push_back(extra);indexed.build(hull.idx());verify(indexed,extended);
    };
    for(const auto& input:vector<vector<point>>{
        {},{{2,3}},{{0,0},{4,3}},{{1,1},{1,1},{1,1}},
        {{2,0},{0,0},{3,0},{1,0},{0,0}},{{0,0},{2,0},{2,2},{0,2},{1,1},{0,0}},
        {{-5,-5},{5,5},{-5,5},{5,-5},{0,0}},
        {{1000000000,1000000000},{-1000000000,-1000000000},{1000000000,-1000000000}},
        {{1000000000000LL,1000000000000LL},{1000000000001LL,1000000000002LL},{1000000000003LL,1000000000000LL}}
    })run(input);
    mt19937 rng(7727);
    for(unsigned mask=0;mask<(1u<<9);++mask){
        vector<point> input;for(int i=0;i<9;++i)if((mask>>i)&1)input.push_back({i%3,i/3});run(input);
    }
    for(int round=0;round<300;++round){
        vector<point> input;int n=rng()%28;for(int i=0;i<n;++i)input.push_back({(int)(rng()%31)-15,(int)(rng()%31)-15});run(input);
    }
    vector<point> boundary{{0,0},{1,0},{2,0},{3,0},{3,1},{3,2},{3,3},{2,3},{1,3},{0,3},{0,2},{0,1}};
    for(int direction=0;direction<2;++direction){
        for(size_t shift=0;shift<boundary.size();++shift){
            rotate(boundary.begin(),boundary.begin()+1,boundary.end());diameter_type d;d.build(boundary);verify(d,boundary);
            auto closed=boundary;closed.push_back(boundary.front());closed.insert(closed.begin(),closed.front());
            d.build(closed);verify(d,closed);
        }
        reverse(all(boundary));
    }
    for(const auto& line:vector<vector<point>>{{{1,0},{3,0},{2,0},{0,0}},{{2,2},{3,3},{-1,-1},{0,0}}}){
        diameter_type d;d.build(line);verify(d,line);
    }
    auto detached=[](){hull_type hull;hull.add(0,0);hull.add(4,3);hull.build();diameter_type d;d.build(hull.idx());return d;}();
    CHECK(detached.dist2()==25);detached.build();CHECK(detached.dist2()==25);
    hull_type edited;edited.add(0,0);edited.add(1,0);edited.add(0,1);edited.build();
    edited.arr[1].x=10;edited.build();diameter_type d;d.build(edited.idx());verify(d,{{0,0},{10,0},{0,1}});
}

template<size_t m>
void verify_gf2(geometry::_gf2<m>& system){
    using solver=geometry::_gf2<m>;
    size_t n=system.bs.size();
    static_assert(same_as<decltype(as_const(system).ret()),const typename solver::result&>);
    static_assert(same_as<decltype(as_const(system).ret().get_value()),const bitset<m>&>);
    auto input=system.bs;set<unsigned> image;vector<unsigned> solutions;
    unsigned rhs=0;for(size_t row=0;row<n;++row)rhs|=(unsigned)input[row][m]<<row;
    for(unsigned value=0;value<(1u<<m);++value){
        unsigned mapped=0;
        for(size_t row=0;row<n;++row){
            bool parity=false;for(size_t col=0;col<m;++col)parity^=input[row][col] && ((value>>col)&1);
            mapped|=(unsigned)parity<<row;
        }
        image.insert(mapped);if(mapped==rhs)solutions.push_back(value);
    }
    int rank=0;while((1u<<rank)<image.size())++rank;
    system.build();CHECK(system.bs==input);
    const auto& answer=as_const(system).ret();CHECK(&answer==&as_const(system).ret());
    CHECK(answer.rank()==rank && answer.free_count()==(long long)m-rank);
    CHECK(answer.inconsistent()==solutions.empty());CHECK(answer.unique()==(solutions.size()==1));
    CHECK(answer.multiple()==(solutions.size()>1));
    if(!solutions.empty()){
        auto value=(unsigned)answer.get_value().to_ullong();
        CHECK(find(all(solutions),value)!=solutions.end());
        CHECK(solutions.size()==(1u<<answer.free_count()));
    }
}

template<size_t m>
void check_gf2_dimensions(){
    for(size_t n=0;n<=3;++n){
        geometry::_gf2<m> system(n);
        for(unsigned encoded=0;encoded<(1u<<(n*(m+1)));++encoded){
            unsigned bits=encoded;
            for(size_t row=0;row<n;++row){system.bs[row]=bitset<m+1>(bits);bits>>=m+1;}
            verify_gf2(system);
            if(encoded%31==0){
                auto before=system.ret();system.build();const auto& after=system.ret();
                CHECK(after.status==before.status && after.get_value()==before.get_value());
                CHECK(after.rank()==before.rank() && after.free_count()==before.free_count());
            }
        }
    }
}

void check_gf2(){
    check_gf2_dimensions<0>();check_gf2_dimensions<1>();check_gf2_dimensions<2>();check_gf2_dimensions<3>();
    geometry::_gf2<2> empty;verify_gf2(empty);CHECK(empty.ret().multiple() && empty.ret().free_count()==2);
    for(long long n:vector<long long>{5,0,2,1,7}){
        geometry::_gf2<2> input(n);
        if(n){input.set_a(n-1,1,1);input.set_b(n-1,1);}
        verify_gf2(input);CHECK(input.bs.size()==(size_t)n);
        if(n)CHECK(input.ret().get_value().to_ulong()==2);
        input.clear();verify_gf2(input);CHECK(input.bs.size()==(size_t)n);
    }
    geometry::_gf2<2> system(2);
    system.set_a(0,0,1);system.set_a(0,1,1);system.set_b(0,0);
    system.set_a(1,0,1);system.set_b(1,1);verify_gf2(system);
    CHECK(system.ret().unique() && system.ret().get_value().to_ulong()==3);
    system.set_b(0,1);verify_gf2(system);CHECK(system.ret().get_value().to_ulong()==1);
    system.set_a(0,1,0);system.set_b(0,0);verify_gf2(system);CHECK(system.ret().inconsistent());
    system.set_b(1,0);verify_gf2(system);
    CHECK(system.ret().multiple() && system.ret().rank()==1 && system.ret().free_count()==1);
    CHECK(system.ret().get_value().none());
    system.set_a(1,1,1);system.set_b(1,1);verify_gf2(system);
    CHECK(system.ret().unique() && system.ret().get_value().to_ulong()==2);
    system.clear();verify_gf2(system);CHECK(system.ret().multiple() && system.ret().free_count()==2);
    geometry::_gf2<129> wide(130);
    for(int i=0;i<129;++i){
        wide.set_a(i,i,1);if(i+1<129)wide.set_a(i,i+1,1);
        wide.set_b(i,(i%2) ^ (i+1<129?(i+1)%2:0));wide.set_a(129,i,1);
    }
    auto input=wide.bs;wide.build();CHECK(wide.bs==input);
    CHECK(wide.ret().unique() && wide.ret().rank()==129 && wide.ret().free_count()==0);
    for(int i=0;i<129;++i)CHECK(wide.ret().get_value()[i]==(bool)(i%2));
    wide.set_b(129,1);input=wide.bs;wide.build();CHECK(wide.bs==input);
    CHECK(wide.ret().inconsistent() && wide.ret().rank()==129 && wide.ret().free_count()==0);
    wide.clear();wide.build();CHECK(wide.ret().multiple() && wide.ret().free_count()==129);
    CHECK(wide.ret().rank()==0 && wide.ret().get_value().none());
}

template<uint64_t mod>
void check_fixed_comb_mod(){
    combinatorics::_comb<mod> table((int)min<uint64_t>(mod-1,60));
    vector<vector<uint64_t>> pascal(table.n+1,vector<uint64_t>(table.n+1));
    for(int n=0;n<=table.n;++n){
        pascal[n][0]=pascal[n][n]=1;
        for(int r=1;r<n;++r)pascal[n][r]=(pascal[n-1][r-1]+pascal[n-1][r])%mod;
        for(int r=0;r<=n;++r)CHECK(table.ret(n,r)==(long long)pascal[n][r]);
        CHECK(table.ret(n,-1)==0 && table.ret(n,n+1)==0);
    }
    CHECK(table.ret(-1,0)==0);
}

void check_fixed_comb(){
    check_fixed_comb_mod<2>();check_fixed_comb_mod<7>();check_fixed_comb_mod<61>();
    check_fixed_comb_mod<998244353>();check_fixed_comb_mod<1000000007>();
    combinatorics::_comb table(5);CHECK(table.ret(5,2)==10);
    combinatorics::_comb<> zero(0);CHECK(zero.ret(0,0)==1 && zero.ret(0,1)==0);
    static_assert(same_as<decltype(table.fac)::value_type,numeric_templates::_mint<998244353>>);
    static_assert(!is_constructible_v<combinatorics::_comb<7>,int,int>);
}

template<class A,class B>
concept matrix_arithmetic=requires(const A& a,const B& b){a+b;a*b;};

template<uint64_t mod>
void check_fixed_matrix_mod(){
    using matrix=geometry::_mod_matrix<mod>;using mint=numeric_templates::_mint<mod>;
    auto norm=[](long long v){__int128_t x=v;x%=mod;if(x<0)x+=mod;return (uint64_t)x;};
    auto product=[](const vector<vector<long long>>& a,const vector<vector<long long>>& b){
        vector<vector<long long>> ret(a.size(),vector<long long>(b[0].size()));
        for(size_t i=0;i<a.size();++i)for(size_t j=0;j<b[0].size();++j)
            for(size_t k=0;k<b.size();++k)ret[i][j]+=a[i][k]*b[k][j];
        return ret;
    };
    auto make=[](const vector<vector<long long>>& a){
        matrix ret((int)a.size(),(int)a[0].size());
        for(int i=0;i<ret.n;++i)for(int j=0;j<ret.m;++j)ret.set(i,j,a[i][j]);return ret;
    };
    auto equal=[&](const matrix& got,const vector<vector<long long>>& want){
        CHECK(got.n==(int)want.size() && got.m==(int)want[0].size());
        for(int i=0;i<got.n;++i)for(int j=0;j<got.m;++j)CHECK(got.at(i,j).v==norm(want[i][j]));
    };
    matrix a(2,3);for(const auto& v:a.arr)CHECK(v==0);
    a.set(0,0,-1);a.add(0,0,3);CHECK(a.at(0,0)==2);a.at(1,2)=-3;CHECK(a.at(1,2).v==norm(-3));
    static_assert(same_as<decltype(a.at(0,0)),mint&>);
    static_assert(same_as<decltype(as_const(a).at(0,0)),const mint&>);
    const vector<vector<long long>> av{{1,2,3},{-4,5,6}},bv{{7,-8},{9,10},{11,12}};
    auto left=make(av),right=make(bv);equal(left*right,{{58,48},{83,154}});
    equal(left+left,{{2,4,6},{-8,10,12}});equal(left.transpose(),{{1,-4},{2,5},{3,6}});
    equal(left*matrix::id(3),av);equal(matrix::id(2)*left,av);
    auto fib=make({{1,1},{1,0}});equal(fib.pow(0),{{1,0},{0,1}});equal(fib.pow(10),{{89,55},{55,34}});
    equal(left,av);equal(right,bv);
    matrix empty;CHECK(empty.n==0 && empty.m==0 && empty.arr.empty());
    auto empty_pow=empty.pow(0);CHECK(empty_pow.n==0 && empty_pow.m==0 && empty_pow.arr.empty());
    matrix rows(0,3),cols(3,0);auto empty_product=rows*cols;
    CHECK(empty_product.n==0 && empty_product.m==0 && empty_product.arr.empty());
    auto transposed=rows.transpose();CHECK(transposed.n==3 && transposed.m==0 && transposed.arr.empty());
    auto zeros=cols*rows;CHECK(zeros.n==3 && zeros.m==3);for(const auto& v:zeros.arr)CHECK(v==0);
    mt19937 rng(393);
    for(int n=1;n<=4;++n)for(int k=1;k<=4;++k)for(int m=1;m<=4;++m){
        vector<vector<long long>> av(n,vector<long long>(k)),bv(k,vector<long long>(m));
        for(auto& row:av)for(auto& v:row)v=(int)(rng()%21)-10;
        for(auto& row:bv)for(auto& v:row)v=(int)(rng()%21)-10;
        equal(make(av)*make(bv),product(av,bv));
    }
}

void check_fixed_matrix(){
    check_fixed_matrix_mod<1>();check_fixed_matrix_mod<7>();check_fixed_matrix_mod<12>();
    check_fixed_matrix_mod<998244353>();check_fixed_matrix_mod<18446744073709551557ULL>();
    geometry::_mod_matrix a(1,1);a.set(0,0,998244354);CHECK(a.at(0,0)==1);
    static_assert(!matrix_arithmetic<geometry::_mod_matrix<7>,geometry::_mod_matrix<11>>);
    static_assert(!is_constructible_v<geometry::_mod_matrix<7>,int,int,int>);
    ostringstream out;geometry::_mod_matrix<7> b(1,2);b.set(0,0,-1);b.set(0,1,9);out<<b;
    CHECK(out.str()=="6 2 \n");
}

template<class A,class B>
concept dynamic_arithmetic = requires(A a,B b){a+b;a-b;a*b;a/b;};
template<class A,class B>
concept dynamic_compound = requires(A a,B b){a+=b;a-=b;a*=b;a/=b;};

template<uint64_t mod>
concept fixed_modulus = requires{typename numeric_templates::_mint<mod>;};
template<class A,class B>
concept fixed_arithmetic = requires(A a,B b){a+b;a-b;a*b;a/b;a==b;a!=b;};

template<uint64_t mod>
void check_fixed_mint_mod(){
    using mint=numeric_templates::_mint<mod>;using dynamic=numeric_templates::_dynamic_mint;
    using u64=uint64_t;using signed_wide=__int128_t;using wide=__uint128_t;
    static_assert(mint::get_mod()==mod);static_assert(same_as<decltype(mint::v),u64>);
    static_assert(same_as<decltype(&mint::pow),mint(mint::*)(u64) const>);
    auto norm=[](signed_wide a){a%=mod;if(a<0)a+=mod;return (u64)a;};
    auto equal=[](const mint& a,u64 value){CHECK(a.v==value);};
    mint zero;equal(zero,0);vector<mint> dp(4);for(const auto& v:dp)equal(v,0);
    array<mint,3> arr{};for(const auto& v:arr)equal(v,0);
    fill(all(dp),2);for(const auto& v:dp)equal(v,2%mod);
    equal(accumulate(all(dp),mint(0)),(u64)(wide(8)%mod));
    auto verify=[&](signed_wide ra,signed_wide rb){
        u64 av=norm(ra),bv=norm(rb);mint a=ra,b=rb;dynamic da(ra,mod),db(rb,mod);
        equal(a,av);equal(b,bv);
        equal(a+b,(u64)((wide(av)+bv)%mod));equal(a-b,(u64)((wide(av)+mod-bv)%mod));equal(a*b,(u64)(wide(av)*bv%mod));
        equal(a+rb,(u64)((wide(av)+bv)%mod));equal(a-rb,(u64)((wide(av)+mod-bv)%mod));equal(a*rb,(u64)(wide(av)*bv%mod));
        equal(ra+b,(u64)((wide(av)+bv)%mod));equal(ra-b,(u64)((wide(av)+mod-bv)%mod));equal(ra*b,(u64)(wide(av)*bv%mod));
        CHECK((a+b).v==(da+db).v && (a-b).v==(da-db).v && (a*b).v==(da*db).v);
        auto t=a;t+=b;equal(t,(u64)((wide(av)+bv)%mod));t=a;t-=b;equal(t,(u64)((wide(av)+mod-bv)%mod));t=a;t*=b;equal(t,(u64)(wide(av)*bv%mod));
        t=a;t+=rb;equal(t,(u64)((wide(av)+bv)%mod));t=a;t-=rb;equal(t,(u64)((wide(av)+mod-bv)%mod));t=a;t*=rb;equal(t,(u64)(wide(av)*bv%mod));
        t=a;t+=t;equal(t,(u64)(wide(av)*2%mod));t=a;t-=t;equal(t,0);t=a;t*=t;equal(t,(u64)(wide(av)*av%mod));
        t=rb;equal(t,bv);equal(+a,av);equal(-a,av?mod-av:0);
        CHECK((a==b)==(av==bv));CHECK((a!=b)==(av!=bv));CHECK((a==rb)==(av==bv));CHECK((ra==b)==(av==bv));
        if(mod>1 && gcd(bv,mod)==1){auto inv=b.inv();equal(b*inv,1);u64 want=(u64)(wide(av)*inv.v%mod);equal(a/b,want);equal(a/rb,want);equal(ra/b,want);t=a;t/=rb;equal(t,want);t=b;t/=t;equal(t,1);}
        u64 want=1%mod;for(int e=0;e<=7;++e){equal(a.pow(e),want);want=(u64)(wide(want)*av%mod);}
    };
    vector<signed_wide> input{numeric_limits<long long>::min(),-1,0,1,numeric_limits<long long>::max(),(signed_wide)numeric_limits<u64>::max()};
    for(auto a:input)for(auto b:input)verify(a,b);
    mt19937_64 rng(mod^919);for(int trial=0;trial<100;++trial)verify((signed_wide)rng(),-(signed_wide)rng());
}

void check_fixed_mint(){
    using m7=numeric_templates::_mint<7>;using m11=numeric_templates::_mint<11>;
    using u64=uint64_t;constexpr u64 mx=numeric_limits<u64>::max(),high=u64(1)<<63;
    static_assert(fixed_modulus<1> && !fixed_modulus<0>);
    static_assert(is_default_constructible_v<m7> && is_convertible_v<int,m7> && is_convertible_v<u64,m7>);
    static_assert(!is_constructible_v<m7,double> && !is_convertible_v<m11,m7>);
    static_assert(fixed_arithmetic<m7,int> && fixed_arithmetic<int,m7> && fixed_arithmetic<m7,m7>);
    static_assert(!fixed_arithmetic<m7,m11> && !fixed_arithmetic<m7,double> && !fixed_arithmetic<numeric_templates::_dynamic_mint,m7>);
    m7 a=3;a=5;CHECK(a==5 && 5==a && a!=4 && 4!=a);CHECK(2-a==4 && 2/a==6 && -a==2);
    istringstream in("-9 invalid");in>>a;CHECK(a==5);in>>a;CHECK(in.fail() && a==5);
    ostringstream out;out<<a<<' '<<-a;CHECK(out.str()=="5 2");
    using large=numeric_templates::_mint<mx>;large b=-1;CHECK((b+b).v==mx-2 && (b*b)==1);CHECK(large(2).inv().v==high);
    ostringstream full;full<<b;CHECK(full.str()=="18446744073709551614");
    CHECK(numeric_templates::_mint<8>(3).inv()==3 && numeric_templates::_mint<12>(5).inv()==5);
    CHECK(numeric_templates::_mint<17>(3).pow(mx)==6 && numeric_templates::_mint<17>(3).pow(high)==1);
    check_fixed_mint_mod<1>();check_fixed_mint_mod<7>();check_fixed_mint_mod<12>();check_fixed_mint_mod<998244353>();
    check_fixed_mint_mod<high>();check_fixed_mint_mod<mx-58>();check_fixed_mint_mod<mx>();
}

void check_dynamic_mint(){
    using mint=numeric_templates::_dynamic_mint;using value=long long;using wide=__int128_t;
    static_assert(is_default_constructible_v<mint> && is_constructible_v<mint,value,value>);
    static_assert(!is_constructible_v<mint,value> && !is_convertible_v<value,mint>);
    static_assert(dynamic_arithmetic<mint,mint> && dynamic_arithmetic<mint,int> && dynamic_arithmetic<int,mint>);
    static_assert(dynamic_compound<mint,mint> && dynamic_compound<mint,value>);
    static_assert(!dynamic_arithmetic<mint,double> && !dynamic_arithmetic<string,mint>);
    static_assert(!dynamic_compound<mint,double> && !dynamic_compound<mint,string>);
    auto norm=[](wide a,value m){a%=m;if(a<0)a+=m;return (value)a;};
    auto equal=[](const mint& a,value v,value m){CHECK(a.v==v && a.mod==m);};
    mint a(3,7);a=5;equal(a,5,7);a=-9;equal(a,5,7);
    mint b(6,11);a=b;equal(a,6,11);a=a;equal(a,6,11);
    vector<mint> dp(4,mint(3,7));fill(all(dp),0);for(const auto& v:dp)equal(v,0,7);
    a=mint(3,7);equal(2+a,5,7);equal(2-a,6,7);equal(2*a,6,7);equal(2/a,3,7);
    CHECK(a==mint(3,7));CHECK(a!=mint(3,11));CHECK(a!=mint(4,7));
    CHECK(a==3 && 3==a && a==10 && 10==a);CHECK(a!=2 && 2!=a);
    equal(+a,3,7);equal(-a,4,7);equal(-mint(0,7),0,7);equal(-mint(0,1),0,1);
    istringstream in("10 -9 invalid");in>>a;equal(a,3,7);in>>a;equal(a,5,7);in>>a;CHECK(in.fail());equal(a,5,7);
    ostringstream out;out<<a<<' '<<-a;CHECK(out.str()=="5 2");
    const value mx=numeric_limits<value>::max();
    vector<value> mods{1,2,7,12,1000000007,mx};
    vector<value> raws{numeric_limits<value>::min(),-1,0,1,mx};
    auto verify=[&](value ra,value rb,value m){
        value av=norm(ra,m),bv=norm(rb,m);mint x(ra,m),y(rb,m);
        equal(x,av,m);equal(y,bv,m);
        equal(x+y,norm((wide)av+bv,m),m);equal(x-y,norm((wide)av-bv,m),m);equal(x*y,norm((wide)av*bv,m),m);
        equal(x+rb,norm((wide)av+rb,m),m);equal(x-rb,norm((wide)av-rb,m),m);equal(x*rb,norm((wide)av*rb,m),m);
        equal(ra+y,norm((wide)ra+bv,m),m);equal(ra-y,norm((wide)ra-bv,m),m);equal(ra*y,norm((wide)ra*bv,m),m);
        auto t=x;t+=y;equal(t,norm((wide)av+bv,m),m);t=x;t-=y;equal(t,norm((wide)av-bv,m),m);t=x;t*=y;equal(t,norm((wide)av*bv,m),m);
        t=x;t+=rb;equal(t,norm((wide)av+rb,m),m);t=x;t-=rb;equal(t,norm((wide)av-rb,m),m);t=x;t*=rb;equal(t,norm((wide)av*rb,m),m);
        t=x;t+=t;equal(t,norm((wide)av*2,m),m);t=x;t-=t;equal(t,0,m);t=x;t*=t;equal(t,norm((wide)av*av,m),m);
        equal(-x,av?m-av:0,m);CHECK((x==rb)==(av==bv));CHECK((ra==y)==(av==bv));CHECK((x!=y)==(av!=bv));
        t=x;t=rb;equal(t,bv,m);equal(x,av,m);equal(y,bv,m);
        if(m>1 && gcd(bv,m)==1){
            auto inv=y.inv();equal(y*inv,1,m);value want=norm((wide)av*inv.v,m);
            equal(x/y,want,m);equal(x/rb,want,m);equal(ra/y,want,m);
            t=x;t/=y;equal(t,want,m);t=x;t/=rb;equal(t,want,m);t=y;t/=t;equal(t,1,m);
        }
        value want=1%m;for(int e=0;e<=8;++e){equal(x.pow(e),want,m);want=norm((wide)want*av,m);}
    };
    for(value m:mods)for(value ra:raws)for(value rb:raws)verify(ra,rb,m);
    mt19937_64 rng(918);
    for(int trial=0;trial<700;++trial){value m=trial%3?2+rng()%1000000008:mx-(value)(rng()%1000);verify((value)rng(),(value)rng(),m);}
    a=mint(mx-1,mx);a+=a;equal(a,mx-2,mx);
    equal(mint(3,8).inv(),3,8);equal(mint(5,12).inv(),5,12);
}

void check_dynamic_mint_unsigned(){
    using mint=numeric_templates::_dynamic_mint;using u64=uint64_t;using signed_wide=__int128_t;using wide=__uint128_t;
    static_assert(same_as<decltype(mint::mod),u64> && same_as<decltype(mint::v),u64>);
    static_assert(same_as<decltype(&mint::pow),mint(mint::*)(u64) const>);
    static_assert(same_as<decltype(mint::norm(signed_wide(0),u64(7))),u64>);
    static_assert(same_as<tuple_element_t<0,decltype(mint::gcd(u64(2),u64(7)))>,u64>);
    static_assert(same_as<tuple_element_t<1,decltype(mint::gcd(u64(2),u64(7)))>,signed_wide>);
    const u64 mx=numeric_limits<u64>::max(),high=u64(1)<<63;
    auto equal=[](const mint& a,u64 v,u64 mod){CHECK(a.v==v && a.mod==mod);};
    mint a(-1,mx);equal(a,mx-1,mx);equal(a+a,mx-2,mx);equal(a*a,1,mx);equal(mint(0,mx)-a,1,mx);
    ostringstream out;out<<a;CHECK(out.str()=="18446744073709551614");
    istringstream in("-1");in>>a;equal(a,mx-1,mx);
    equal(a.inv(),mx-1,mx);equal(mint(2,mx).inv(),high,mx);
    equal(mint(numeric_limits<long long>::min(),mx),high-1,mx);
    a=mint(3,7);equal(a+mx,4,7);equal(mx-a,5,7);equal(mx/a,5,7);a=mx;equal(a,1,7);CHECK(a==mx && mx==a);
    auto [g,cx,cy]=mint::gcd(2,mx);CHECK(g==1 && cx<0);CHECK(signed_wide(2)*cx+signed_wide(mx)*cy==1);
    equal(mint(3,17).pow(mx),6,17);equal(mint(3,17).pow(high),1,17);equal(mint(3,17).pow(high+3),10,17);
    equal(mint(0,1).pow(mx),0,1);equal(mint(0,7).pow(mx),0,7);
    mt19937_64 rng(583);
    auto verify=[&](u64 av,u64 bv,u64 mod){
        av%=mod;bv%=mod;mint x(av,mod),y(bv,mod);
        equal(x,av,mod);equal(y,bv,mod);
        equal(x+y,(u64)((wide(av)+bv)%mod),mod);
        equal(x-y,(u64)((wide(av)+mod-bv)%mod),mod);
        equal(x*y,(u64)(wide(av)*bv%mod),mod);
        equal(x+bv,(u64)((wide(av)+bv)%mod),mod);equal(av-y,(u64)((wide(av)+mod-bv)%mod),mod);
        equal(av*y,(u64)(wide(av)*bv%mod),mod);
        auto t=x;t+=t;equal(t,(u64)(wide(av)*2%mod),mod);t=x;t-=t;equal(t,0,mod);t=x;t*=t;equal(t,(u64)(wide(av)*av%mod),mod);
        if(mod>1 && gcd(bv,mod)==1){auto inv=y.inv();equal(y*inv,1,mod);equal((x/y)*y,av,mod);t=y;t/=t;equal(t,1,mod);}
        signed_wide raw=-(signed_wide)rng();signed_wide rem=raw%mod;if(rem<0)rem+=mod;
        equal(mint(raw,mod),(u64)rem,mod);t=raw;equal(t,(u64)rem,mod);
    };
    for(u64 mod:{u64(1),u64(2),high,high+1,mx-58,mx})for(u64 av:{u64(0),u64(1),high,mx-1,mx})for(u64 bv:{u64(0),u64(1),high,mx-1,mx})verify(av,bv,mod);
    for(int trial=0;trial<500;++trial){verify(rng(),rng(),rng()|high);u64 e=rng(),want=1;for(u64 i=0;i<e%16;++i)want=want*3%17;equal(mint(3,17).pow(e),want,17);}
}

void check_crt(){
    using crt=numeric_templates::_crt;using mint=numeric_templates::_dynamic_mint;
    static_assert(same_as<decltype(declval<const crt&>().ret()),const crt::result&>);
    crt g;g.add(2,3);g.add(pair<long long,long long>{3,5});g.add(mint(8,15));g.build();
    const auto& read=g;CHECK(read.ret().status==crt::state::known);
    CHECK(read.ret().get_value()==8 && read.ret().get_mod()==15);
    const auto* stored=&read.ret();for(int i=0;i<5;++i)CHECK(&read.ret()==stored);
    CHECK(g.arr.size()==3);g.build();CHECK(read.ret().get_value()==8 && read.ret().get_mod()==15);
    g.add(0,3);g.build();CHECK(read.ret().inconsistent());
    g.arr.back()=mint(2,3);g.build();CHECK(read.ret().status==crt::state::known && read.ret().get_value()==8);
    g.clear();g.build();CHECK(read.ret().get_value()==0 && read.ret().get_mod()==1);
    for(long long m=1;m<=9;++m)for(long long n=1;n<=9;++n)for(long long a=0;a<m;++a)for(long long b=0;b<n;++b){
        long long mod=lcm(m,n),want=-1;
        for(long long x=0;x<mod;++x)if(x%m==a && x%n==b){want=x;break;}
        g.clear();g.add(a-m,m);g.add(b+n,n);g.build();const auto& r=read.ret();
        CHECK(r.inconsistent()==(want==-1));
        if(want!=-1){CHECK(r.status==crt::state::known);CHECK(r.get_value()==want);CHECK(r.get_mod()==mod);}
        CHECK(g.arr.size()==2 && g.arr[0].v==a && g.arr[1].v==b);
        g.build();CHECK(read.ret().inconsistent()==(want==-1));
        if(want!=-1)CHECK(read.ret().get_value()==want && read.ret().get_mod()==mod);
    }
    mt19937 rng(731);
    for(int trial=0;trial<120;++trial){
        g.clear();vector<pair<long long,long long>> input;long long mod=1;
        for(int j=0,sz=rng()%7;j<sz;++j){long long m=1+rng()%12,a=(int)(rng()%61)-30;input.push_back({a,m});g.add(a,m);mod=lcm(mod,m);}
        long long want=-1;for(long long x=0;x<mod;++x){bool ok=1;for(auto[a,m]:input)ok&=x%m==(a%m+m)%m;if(ok){want=x;break;}}
        g.build();CHECK(read.ret().inconsistent()==(want==-1));
        if(want!=-1)CHECK(read.ret().get_value()==want && read.ret().get_mod()==mod);
    }
    long long mx=numeric_limits<long long>::max();
    g.clear();g.add(numeric_limits<long long>::min(),mx);g.add(-1,mx);g.add(0,1);g.build();
    CHECK(read.ret().status==crt::state::known && read.ret().get_value()==mx-1 && read.ret().get_mod()==mx);
    long long m=3037000499LL,n=3037000493LL,mod=(long long)((__int128_t)m*n),want=mod-1234567;
    g.clear();g.add(want%m,m);g.add(want%n,n);g.build();CHECK(read.ret().get_value()==want && read.ret().get_mod()==mod);
    reverse(all(g.arr));g.build();CHECK(read.ret().get_value()==want && read.ret().get_mod()==mod);
    g.clear();g.add(1,1LL<<62);g.add(1,2);g.build();CHECK(read.ret().get_value()==1 && read.ret().get_mod()==(1LL<<62));
    g.clear();g.add(0,1LL<<62);g.add(1,6);g.build();CHECK(read.ret().inconsistent());
}

void check_crt_dependents(){
    using crt=numeric_templates::_crt;
    combinatorics::_comb_crt comb;comb.add(2,3);comb.add(3,2);
    vector<vector<long long>> pascal(21,vector<long long>(21));pascal[0][0]=1;
    for(int n=1;n<=20;++n){pascal[n][0]=1;for(int r=1;r<=n;++r)pascal[n][r]=pascal[n-1][r-1]+pascal[n-1][r];}
    for(int n=0;n<=20;++n)for(int r=-1;r<=n+1;++r){
        auto result=comb.ret(n,r);static_assert(same_as<decltype(result),crt::result>);
        long long want=r<0 || r>n ? 0 : pascal[n][r]%72;
        CHECK(result.status==crt::state::known && result.get_value()==want && result.get_mod()==72);
    }
    auto saved=comb.ret(10,4);comb.add(5,1);auto changed=comb.ret(10,4);
    CHECK(saved.get_mod()==72 && changed.get_mod()==360 && changed.get_value()==210);
    comb.clear();comb.add(2,1);comb.add(2,2);auto repeated=comb.ret(10,4);
    CHECK(repeated.status==crt::state::known && repeated.get_mod()==4 && repeated.get_value()==2);
    comb.clear();comb.add(7,1);CHECK(comb.ret(4,2).get_value()==6);
    numeric_templates::_pt tower;tower.add(2);tower.add(3);tower.add(2);tower.add_mod(72);tower.build();
    const auto& read=tower;static_assert(same_as<decltype(read.ret()),const crt::result&>);
    CHECK(read.ret().status==crt::state::known && read.ret().get_value()==8 && read.ret().get_mod()==72);
    tower.arr[0]=3;tower.build();CHECK(read.ret().get_value()==27);tower.build();CHECK(read.ret().get_value()==27);
    tower.add_mod(5);tower.build();CHECK(read.ret().get_value()==243 && read.ret().get_mod()==360);
    tower.clear();tower.add(2);tower.add_mod(8);tower.add(3);tower.build();CHECK(read.ret().get_value()==0 && read.ret().get_mod()==8);
    tower.clear();tower.add(3);tower.add_mod(3,3);tower.build();CHECK(read.ret().get_value()==3 && read.ret().get_mod()==27);
}

class predecessor_kmp_policy{
public:
    static void build(span<const int>){}
    static bool check(int expected,int distance,int state){return expected?expected==distance:distance==0 || distance>state;}
    static bool build_chk(span<const int> pat,int cur,int state){return check(pat[state],pat[cur],state);}
    static bool match_chk(span<const int> pat,span<const int> text,int idx,int state){return check(pat[state],text[idx],state);}
};

class order_kmp_policy{
private:
    vector<int> lower,upper,equal;
    bool check(span<const int> text,int idx,int state)const{
        int first=idx-state;
        if(equal[state]!=-1)return text[idx]==text[first+equal[state]];
        return (lower[state]==-1 || text[first+lower[state]]<text[idx]) && (upper[state]==-1 || text[idx]<text[first+upper[state]]);
    }
public:
    void build(span<const int> pat){
        int n=pat.size();lower.assign(n,-1);upper.assign(n,-1);equal.assign(n,-1);
        for(int i=0;i<n;++i)for(int j=0;j<i;++j){
            if(pat[j]==pat[i])equal[i]=j;
            else if(pat[j]<pat[i] && (lower[i]==-1 || pat[j]>pat[lower[i]]))lower[i]=j;
            else if(pat[j]>pat[i] && (upper[i]==-1 || pat[j]<pat[upper[i]]))upper[i]=j;
        }
    }
    bool build_chk(span<const int> pat,int cur,int state)const{return check(pat,cur,state);}
    bool match_chk(span<const int>,span<const int> text,int idx,int state)const{return check(text,idx,state);}
};

class modular_kmp_policy{
private:
    int mod = 3;
    int norm(int x)const{return (x%mod+mod)%mod;}
public:
    void build(span<const int>){}
    bool build_chk(span<const int> pat,int cur,int state)const{return norm(pat[cur])==norm(pat[state]);}
    bool match_chk(span<const int> pat,span<const int> text,int idx,int state)const{return norm(text[idx])==norm(pat[state]);}
};

class owned_kmp_policy{
private:
    vector<int> saved;
public:
    void build(span<const int> pat){saved.assign(all(pat));}
    bool build_chk(span<const int> pat,int cur,int state)const{return pat[cur]==saved[state];}
    bool match_chk(span<const int> pat,span<const int> text,int idx,int state)const{return text[idx]==saved[state];}
};

class move_only_kmp_policy{
private:
    unique_ptr<int> mod=make_unique<int>(3);
public:
    void build(span<const int>){}
    bool build_chk(span<const int> pat,int cur,int state)const{return pat[cur]%*mod==pat[state]%*mod;}
    bool match_chk(span<const int> pat,span<const int> text,int idx,int state)const{return text[idx]%*mod==pat[state]%*mod;}
};

class prepared_kmp_policy{
private:
    int phase=0;
    int norm(int x)const{return x%(phase?3:2);}
public:
    void build(span<const int>){phase^=1;}
    bool build_chk(span<const int> pat,int cur,int state)const{return norm(pat[cur])==norm(pat[state]);}
    bool match_chk(span<const int> pat,span<const int> text,int idx,int state)const{return norm(text[idx])==norm(pat[state]);}
};

template<class K>
concept scalar_kmp_go=requires(const K& k){k.go(0,1);};

template<class T,class Policy>
int kmp_value_step(const strings::_kmp<T,Policy>& k,int state,type_identity_t<T> value){
    return k.go(state,span<const T>(&value,1),0);
}

void check_kmp_policy(){
    vector<int> prepared_pattern{1,2},prepared_text{4,5};
    using prepared_type=strings::_kmp<int,prepared_kmp_policy>;
    prepared_type prepared{span<const int>(prepared_pattern)};
    auto prepared_copy=prepared;prepared_type prepared_assigned;prepared_assigned=prepared;
    for(const auto* candidate:{&prepared,&prepared_copy,&prepared_assigned}){
        vector<int> matches;candidate->it_match(span<const int>(prepared_text),[&](int i){matches.push_back(i);});CHECK(matches==vector<int>{0});
    }
    prepared.make(span<const int>(prepared_pattern));
    for(const auto* candidate:{&prepared_copy,&prepared_assigned}){
        vector<int> matches;candidate->it_match(span<const int>(prepared_text),[&](int i){matches.push_back(i);});CHECK(matches==vector<int>{0});
    }
    using order_type=strings::_kmp<int,order_kmp_policy>;
    using prev_type=strings::_kmp<int,predecessor_kmp_policy>;
    static_assert(!scalar_kmp_go<strings::_kmp<int>> && !scalar_kmp_go<prev_type>);
    static_assert(!scalar_kmp_go<order_type>);
    using move_type=strings::_kmp<int,move_only_kmp_policy>;
    static_assert(!is_copy_constructible_v<move_type> && !is_copy_assignable_v<move_type>);
    static_assert(is_move_constructible_v<move_type> && is_move_assignable_v<move_type>);
    auto words=[](int n){
        int count=1;for(int i=0;i<n;++i)count*=3;vector<vector<int>> result;
        for(int mask=0;mask<count;++mask){vector<int> word(n);int code=mask;for(auto& v:word){v=code%3;code/=3;}result.push_back(word);}return result;
    };
    auto order_equal=[](span<const int> a,span<const int> b){
        if(a.size()!=b.size())return false;
        for(size_t i=0;i<a.size();++i)for(size_t j=0;j<a.size();++j)
            if((a[i]<a[j])!=(b[i]<b[j]) || (a[i]==a[j])!=(b[i]==b[j]))return false;
        return true;
    };
    auto renamed_equal=[](span<const int> a,span<const int> b){
        if(a.size()!=b.size())return false;
        for(size_t i=0;i<a.size();++i)for(size_t j=0;j<a.size();++j)
            if((a[i]==a[j])!=(b[i]==b[j]))return false;
        return true;
    };
    auto previous=[](const vector<int>& input){
        vector<int> encoded;map<int,int> seen;
        for(int i=0;i<(int)input.size();++i){auto it=seen.find(input[i]);encoded.push_back(it==seen.end()?0:i-it->second);seen[input[i]]=i;}return encoded;
    };
    auto expected_matches=[](const vector<int>& pat,const vector<int>& text,const auto& equal){
        vector<int> result;if(pat.empty())return result;
        for(size_t i=0;i+pat.size()<=text.size();++i)if(equal(span<const int>(pat),span<const int>(text).subspan(i,pat.size())))result.push_back(i);return result;
    };
    for(int m=0;m<=3;++m)for(const auto& pat:words(m)){
        order_type order{span<const int>(pat)};auto encoded_pat=previous(pat);prev_type renamed{span<const int>(encoded_pat)};
        vector<int> fail_order(m),fail_renamed(m);
        for(int i=0;i<m;++i)for(int len=1;len<=i;++len){
            if(order_equal(span<const int>(pat).first(len),span<const int>(pat).subspan(i-len+1,len)))fail_order[i]=len;
            if(renamed_equal(span<const int>(pat).first(len),span<const int>(pat).subspan(i-len+1,len)))fail_renamed[i]=len;
        }
        CHECK(order.fail()==fail_order && renamed.fail()==fail_renamed);
        vector<int> borders;order.it_border([&](int n){borders.push_back(n);});
        vector<int> want_borders;for(int len=m-1;len>0;--len)if(order_equal(span<const int>(pat).first(len),span<const int>(pat).last(len)))want_borders.push_back(len);
        CHECK(borders==want_borders);
        for(int n=0;n<=4;++n)for(const auto& text:words(n)){
            auto encoded_text=previous(text);vector<int> order_matches,renamed_matches;
            const auto& read_order=order;const auto& read_renamed=renamed;
            read_order.it_match(span<const int>(text),[&](int i){order_matches.push_back(i);});
            read_renamed.it_match(span<const int>(encoded_text),[&](int i){renamed_matches.push_back(i);});
            CHECK(order_matches==expected_matches(pat,text,order_equal));CHECK(renamed_matches==expected_matches(pat,text,renamed_equal));
            int a=0,b=0;
            for(int i=0;i<n;++i){
                a=read_order.go(a,span<const int>(text),i);b=read_renamed.go(b,span<const int>(encoded_text),i);
                int want_a=0,want_b=0;
                for(int len=1;len<=m && len<=i+1;++len){
                    auto part=span<const int>(text).subspan(i-len+1,len);
                    if(order_equal(span<const int>(pat).first(len),part))want_a=len;
                    if(renamed_equal(span<const int>(pat).first(len),part))want_b=len;
                }
                CHECK(a==want_a && b==want_b);
            }
        }
    }
    vector<int> pat{2,5,3},other{1,1,2},text{10,30,20,20,30,10};order_type a{span<const int>(pat)},b{span<const int>(other)};
    auto copy=a;a.make(span<const int>(other));vector<int> matches;copy.it_match(span<const int>(text),[&](int i){matches.push_back(i);});CHECK(matches==expected_matches(pat,text,order_equal));
    a.clear();CHECK(a.go(0,span<const int>(text),0)==0);a.make(span<const int>(pat));matches.clear();a.it_match(span<const int>(text),[&](int i){matches.push_back(i);});CHECK(matches==expected_matches(pat,text,order_equal));
    matches.clear();b.it_match(span<const int>(text),[&](int i){matches.push_back(i);});CHECK(matches==expected_matches(other,text,order_equal));
    for(int split=0;split<=(int)text.size();++split){
        int state=0;matches.clear();for(auto [l,r]:vector<pair<int,int>>{{0,split},{split,(int)text.size()}})for(int i=l;i<r;++i){state=a.go(state,span<const int>(text),i);if(state==3)matches.push_back(i-2);}
        CHECK(matches==expected_matches(pat,text,order_equal));
    }
    strings::_kmp<int,modular_kmp_policy> mod{span<const int>(pat)};vector<int> values{5,8,6};int state=0;
    for(int value:values)state=kmp_value_step(mod,state,value);CHECK(state==3);mod.clear();mod.make(span<const int>(pat));state=0;for(int value:values)state=kmp_value_step(mod,state,value);CHECK(state==3);
    strings::_kmp<int> plain{span<const int>(pat)};state=0;for(int i=0;i<(int)pat.size();++i)state=plain.go(state,span<const int>(pat),i);CHECK(state==3);
    order_type letters(string("bec"));matches.clear();letters.it_match(string("aymmyb"),[&](int i){matches.push_back(i);});CHECK(matches==vector<int>{0});
    vector<int> original{1,2,1};strings::_kmp<int,owned_kmp_policy> source{span<const int>(original)};
    auto copied=source;matches.clear();copied.it_match(span<const int>(original),[&](int i){matches.push_back(i);});CHECK(matches==vector<int>{0});
    strings::_kmp<int,owned_kmp_policy> assigned;assigned=source;
    vector<int> changed{3,4,3};source.make(span<const int>(changed));
    for(const auto* candidate:{&copied,&assigned}){
        matches.clear();candidate->it_match(span<const int>(original),[&](int i){matches.push_back(i);});CHECK(matches==vector<int>{0});
    }
    copied=copied;matches.clear();copied.it_match(span<const int>(original),[&](int i){matches.push_back(i);});CHECK(matches==vector<int>{0});
    auto moved=move(copied);matches.clear();moved.it_match(span<const int>(original),[&](int i){matches.push_back(i);});CHECK(matches==vector<int>{0});
    assigned=move(moved);matches.clear();assigned.it_match(span<const int>(original),[&](int i){matches.push_back(i);});CHECK(matches==vector<int>{0});
    auto detached=[&](){strings::_kmp<int,owned_kmp_policy> local{span<const int>(original)};return strings::_kmp<int,owned_kmp_policy>(local);}();
    matches.clear();detached.it_match(span<const int>(original),[&](int i){matches.push_back(i);});CHECK(matches==vector<int>{0});
    move_type moving{span<const int>(pat)};auto moved_policy=move(moving);state=0;for(int value:values)state=kmp_value_step(moved_policy,state,value);CHECK(state==3);
    move_type move_assigned;move_assigned=move(moved_policy);state=0;for(int value:values)state=kmp_value_step(move_assigned,state,value);CHECK(state==3);
}

void check_kmp_go(){
    auto naive_state=[](const vector<int>& pattern,const vector<int>& text){
        int answer=0;
        for(int len=1;len<=(int)pattern.size() && len<=(int)text.size();++len){
            bool equal=true;for(int i=0;i<len;++i)if(pattern[i]!=text[text.size()-len+i]){equal=false;break;}
            if(equal)answer=len;
        }
        return answer;
    };
    auto naive_matches=[](const vector<int>& pattern,const vector<int>& text){
        vector<int> matches;if(pattern.empty())return matches;
        for(int i=0;i+(int)pattern.size()<=(int)text.size();++i){
            bool equal=true;for(int j=0;j<(int)pattern.size();++j)if(pattern[j]!=text[i+j]){equal=false;break;}
            if(equal)matches.push_back(i);
        }
        return matches;
    };
    for(int m=0;m<=4;++m)for(int mask=0;mask<(1<<m);++mask){
        vector<int> pattern(m);for(int i=0;i<m;++i)pattern[i]=(mask>>i)&1;
        strings::_kmp<int> kmp{span<const int>(pattern)};const auto& view=kmp;
        for(int state=0;state<=m;++state)for(int value:{-1,0,1,2}){
            vector<int> history(pattern.begin(),pattern.begin()+state);history.push_back(value);
            CHECK(kmp_value_step(view,state,value)==naive_state(pattern,history));
        }
        for(int n=0;n<=6;++n)for(int bits=0;bits<(1<<n);++bits){
            vector<int> input(n);for(int i=0;i<n;++i)input[i]=(bits>>i)&1;
            vector<int> history,matches;int state=0;
            for(int i=0;i<n;++i){
                history.push_back(input[i]);state=view.go(state,span<const int>(input),i);CHECK(state==naive_state(pattern,history));
                if(m && state==m)matches.push_back(i-m+1);
            }
            CHECK(matches==naive_matches(pattern,input));
            vector<int> batch;view.it_match(span<const int>(input),[&](int index){batch.push_back(index);});CHECK(batch==matches);
        }
    }
    strings::_kmp<> text_pattern("aba");string text="ababa";vector<int> states;int state=0;
    for(unsigned char c:text){state=kmp_value_step(text_pattern,state,c);states.push_back(state);}
    CHECK(states==vector<int>({1,2,3,2,3}));
    for(size_t split=0;split<=text.size();++split){
        int current=0,index=0;vector<int> matches;
        for(const auto& part:vector<string>{text.substr(0,split),text.substr(split)})for(unsigned char c:part){
            current=kmp_value_step(text_pattern,current,c);if(current==3)matches.push_back(index-2);++index;
        }
        CHECK(matches==vector<int>({0,2}));
    }
    string pattern;pattern.push_back(char(255));pattern.push_back(0);pattern.push_back(char(255));
    string bytes=pattern+pattern.substr(1);strings::_kmp<> byte_kmp(pattern);vector<int> streamed,batch;state=0;
    for(int i=0;i<(int)bytes.size();++i){state=kmp_value_step(byte_kmp,state,(unsigned char)bytes[i]);if(state==3)streamed.push_back(i-2);}
    byte_kmp.it_match(bytes,[&](int index){batch.push_back(index);});CHECK(streamed==vector<int>({0,2}) && batch==streamed);
    text_pattern.make(string("aa"));state=0;states.clear();for(unsigned char c:string("aaaa")){state=kmp_value_step(text_pattern,state,c);states.push_back(state);}
    CHECK(states==vector<int>({1,2,2,2}));text_pattern.clear();CHECK(kmp_value_step(text_pattern,0,'a')==0);
    strings::_kmp<int> numeric;vector<int> numbers{-3,INT_MAX,-3};numeric.make(span<const int>(numbers));state=0;
    for(int value:numbers)state=kmp_value_step(numeric,state,value);CHECK(state==3);
}

template<class T>
void verify_manacher_values(const vector<T>& input){
    strings::_mnc<T> m{span<const T>(input)};
    int n=(int)input.size(),maximum=0;long long odd=0,even=0;
    vector<int> expected(2*n+1,0);
    for(int l=0;l<n;++l)for(int r=l;r<n;++r){
        bool palindrome=true;for(int a=l,b=r;a<b;++a,--b)if(!(input[a]==input[b])){palindrome=false;break;}
        CHECK(m.is_pal(l,r)==palindrome);
        if(!palindrome)continue;
        int len=r-l+1;maximum=max(maximum,len);if(len%2)++odd;else ++even;
        expected[l+r+1]=max(expected[l+r+1],len);
    }
    CHECK(m.cnt_odd()==odd);CHECK(m.cnt_even()==even);CHECK(m.cnt()==odd+even);CHECK(m.max_len()==maximum);
    CHECK(m.get_rad()==expected);
    for(int i=0;i<n;++i){int r=expected[2*i+1]/2;CHECK(m.range_odd(i)==make_pair(i-r,i+r));}
    for(int i=0;i<=n;++i){int r=expected[2*i]/2;CHECK(m.range_even(i)==make_pair(i-r,i+r-1));}
    auto saved=m.get_rad();m.build(span<const T>(input));CHECK(m.get_rad()==saved && m.cnt()==odd+even);
    m.build(span<const T>{});CHECK(m.get_rad()==vector<int>{0} && m.cnt()==0 && m.max_len()==0);
    m.build(span<const T>(input));CHECK(m.get_rad()==saved && m.cnt_odd()==odd && m.cnt_even()==even);
}

void check_manacher_values(){
    verify_manacher_values(vector<int>{0x3f3f3f3f});
    verify_manacher_values(vector<char>{char(255)});
    for(const auto& input:vector<vector<int>>{
        {},{INT_MIN},{INT_MAX},{0x3f3f3f3f,0x3f3f3f3f},
        {0x3f3f3f3f,7,0x3f3f3f3f},{7,0x3f3f3f3f,7},
        {0x3f3f3f3f,7,7,0x3f3f3f3f},{INT_MIN,0,INT_MAX,0,INT_MIN}
    })verify_manacher_values(input);
    verify_manacher_values(vector<long long>{strings::INF,strings::INF,0,strings::INF,strings::INF});
    verify_manacher_values(vector<uint64_t>{0,UINT64_MAX,0,UINT64_MAX,0});
    verify_manacher_values(vector<char>{0,char(255),0,char(255)});
    const array<int,3> alphabet{0x3f3f3f3f,INT_MIN,INT_MAX};
    for(int n=0,power=1;n<=6;++n,power*=3)for(int mask=0;mask<power;++mask){
        vector<int> input(n);int value=mask;for(auto& v:input){v=alphabet[value%3];value/=3;}verify_manacher_values(input);
    }
    for(int value=0;value<256;++value){
        string text(3,char(value));strings::_mnc<char> bytes(text);strings::_mnc<> normal(text);
        CHECK(bytes.cnt()==6 && bytes.cnt_odd()==4 && bytes.cnt_even()==2 && bytes.max_len()==3);
        CHECK(bytes.get_rad()==normal.get_rad());CHECK(bytes.is_pal(0,2));
    }
    string mixed;for(unsigned c:{0u,255u,128u,255u,0u})mixed.push_back(char(c));
    strings::_mnc<char> byte_string(mixed);vector<char> byte_values(all(mixed));verify_manacher_values(byte_values);
    CHECK(byte_string.cnt()==7 && byte_string.max_len()==5);
    string changed="aba";strings::_mnc<> reusable(changed);reusable.build(mixed);
    CHECK(reusable.get_rad()==byte_string.get_rad());reusable.build(string(""));CHECK(reusable.get_rad()==vector<int>{0});
    reusable.build(changed);CHECK(reusable.cnt()==4 && reusable.max_len()==3);
    vector<int> large(100000,0x3f3f3f3f);strings::_mnc<int> repeated{span<const int>(large)};
    CHECK(repeated.cnt()==5000050000LL && repeated.cnt_odd()==2500050000LL && repeated.cnt_even()==2500000000LL);
    CHECK(repeated.max_len()==100000 && repeated.is_pal(0,99999));
}

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

void check_cht(){
    using line=offline::_cht::line;
    auto best=[](const vector<line>& lines,long long x,bool is_min){
        line answer=lines.front();
        __int128_t value=(__int128_t)answer.a*x+answer.b;
        for(const auto& cur:lines){
            __int128_t next=(__int128_t)cur.a*x+cur.b;
            if(is_min?next<=value:next>=value){answer=cur;value=next;}
        }
        return answer;
    };
    auto compare=[&](const line& actual,const line& expected,long long x){
        CHECK(actual.a==expected.a);CHECK(actual.b==expected.b);
        CHECK(actual.f(x)==(long long)((__int128_t)expected.a*x+expected.b));
    };
    auto batch=[&](bool is_min,const vector<line>& lines,const vector<long long>& queries){
        offline::_cht tree(is_min);for(const auto& cur:lines)tree.add(cur);
        for(long long x:queries){
            auto expected=best(lines,x,is_min);
            compare(tree.query_monotone(x),expected,x);compare(as_const(tree).query(x),expected,x);
        }
    };
    batch(true,{{1,0},{0,5}},{1,5,10});
    batch(true,{{2,0},{1,-3},{0,-4}},{-4,-3,-2,-1,0});
    batch(false,{{0,0},{1,3},{2,4}},{-4,-3,-2,-1,0});
    batch(true,{{3,5},{3,7},{3,-2},{3,-2}},{-10,0,10});
    batch(false,{{3,5},{3,7},{3,-2},{3,7}},{-10,0,10});
    batch(true,{{2,LLONG_MIN+10},{1,LLONG_MAX-10},{0,LLONG_MIN+20}},{0,1,10});
    batch(true,{{LLONG_MAX,0},{0,1},{LLONG_MIN+1,2}},{-1,0,1});
    CHECK(line(LLONG_MAX,-LLONG_MAX).f(2)==LLONG_MAX);
    offline::_cht changed(0);changed.add({0,5});changed.add({1,0});
    CHECK(changed.query_monotone(10).f(10)==10);
    changed.clear();changed.add({1,0});changed.add({0,5});
    CHECK(changed.query_monotone(1).f(1)==1);CHECK(changed.query_monotone(10).f(10)==5);
    changed.clear(0);changed.add({0,5});changed.add({1,0});
    CHECK(changed.query_monotone(1).f(1)==5);CHECK(changed.query_monotone(10).f(10)==10);
    changed.clear(1);changed.add({1,0});changed.add({0,5});
    CHECK(changed.query_monotone(1).f(1)==1);CHECK(as_const(changed).query(10).f(10)==5);

    for(bool is_min:{false,true})for(unsigned seed=1;seed<=16;++seed){
        mt19937 rng(633+seed);offline::_cht tree(is_min);vector<line> lines;
        for(int round=0;round<2;++round){
            tree.clear(is_min);lines.clear();
            CHECK(tree.query(7).f(7)==(is_min?offline::INF:offline::MINF));
            CHECK(tree.query_monotone(-1000).f(-1000)==(is_min?offline::INF:offline::MINF));
            long long slope=is_min?100:-100,x=-60;
            for(int step=0;step<65;++step){
                slope+=(is_min?-1:1)*(long long)(rng()%4);
                line cur{slope,(int)(rng()%4001)-2000};tree.add(cur);lines.push_back(cur);
                long long arbitrary=(int)(rng()%301)-150;
                compare(as_const(tree).query(arbitrary),best(lines,arbitrary,is_min),arbitrary);
                x+=rng()%4;compare(tree.query_monotone(x),best(lines,x,is_min),x);
            }
            for(int step=0;step<40;++step){
                x+=rng()%4;compare(tree.query_monotone(x),best(lines,x,is_min),x);
                long long arbitrary=(int)(rng()%301)-150;
                compare(as_const(tree).query(arbitrary),best(lines,arbitrary,is_min),arbitrary);
            }
            offline::_cht built(is_min);for(const auto& cur:lines)built.add(cur);
            for(long long q=-150;q<=150;q+=3){
                compare(built.query_monotone(q),best(lines,q,is_min),q);
                compare(as_const(built).query(-q),best(lines,-q,is_min),-q);
            }
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

void check_lift_large_cost(){
    for(bool base:{false,true}){
        graphs::_st lift(base,4,base);
        lift.add(base,base,2000000000LL);lift.build();
        CHECK((lift.ret(base,1)==pair<int,long long>{base,0}));
        CHECK((lift.ret(base,6000000000LL)==pair<int,long long>{base,6000000000LL}));
        lift.build();CHECK((lift.ret(base,1)==pair<int,long long>{base,0}));
        lift.add(base,base,5000000000000000000LL);lift.build();
        CHECK((lift.ret(base,LLONG_MAX)==pair<int,long long>{base,5000000000000000000LL}));
        lift.add(base,base,LLONG_MAX);lift.build();
        CHECK((lift.ret(base,LLONG_MAX)==pair<int,long long>{base,LLONG_MAX}));
    }
}

void check_binary_search_large_bounds(){
    for(auto [l,r,target]:vector<tuple<long long,long long,long long>>{
        {LLONG_MAX-10,LLONG_MAX-1,LLONG_MAX-5},
        {LLONG_MIN,LLONG_MIN+10,LLONG_MIN+4},
        {LLONG_MIN,LLONG_MAX,0},{-10,10,-3},{7,7,7},
        {LLONG_MIN,LLONG_MAX,LLONG_MIN},{LLONG_MIN,LLONG_MAX,LLONG_MAX}}){
        int calls=0;
        CHECK(offline::_bs::max_right(l,r,[&](long long v){CHECK(++calls<=65);CHECK(l<=v&&v<=r);return v<=target;})==target);
        calls=0;
        CHECK(offline::_bs::min_left(l,r,[&](long long v){CHECK(++calls<=65);CHECK(l<=v&&v<=r);return v>=target;})==target);
    }
}

void check_ternary_search_large_bounds(){
    for(auto [l,r,peak]:vector<tuple<long long,long long,long long>>{
        {4000000000000000000LL,4000000000000000010LL,4000000000000000007LL},
        {LLONG_MIN,LLONG_MIN+10,LLONG_MIN+4},{LLONG_MAX-10,LLONG_MAX,LLONG_MAX},
        {LLONG_MIN,LLONG_MIN,LLONG_MIN},{LLONG_MAX,LLONG_MAX,LLONG_MAX},{-10,10,-3}}){
        int calls=0;
        auto distance=[&](long long v){CHECK(++calls<=100);CHECK(l<=v&&v<=r);return (long long)abs((__int128)v-peak);};
        CHECK(offline::_ts::min(l,r,distance)==0);
        calls=0;
        CHECK(offline::_ts::max(l,r,[&](long long v){return -distance(v);})==0);
    }
    auto distance=[](long long v){return (long long)(abs((__int128)v)/4);};
    CHECK(offline::_ts::min(LLONG_MIN,LLONG_MAX,distance)==0);
    CHECK(offline::_ts::max(LLONG_MIN,LLONG_MAX,[&](long long v){return -distance(v);})==0);
}

void check_ccw_large_coordinates(){
    geometry::_ccw ccw;
    CHECK(ccw.ret(-2000000000LL,-2000000000LL,2000000000LL,-2000000000LL,-2000000000LL,2000000000LL)==1);
    CHECK(ccw.ret(-2000000000LL,-2000000000LL,-2000000000LL,2000000000LL,2000000000LL,-2000000000LL)==-1);
    CHECK(ccw.ret({-2000000000LL,-2000000000LL},{0,0},{2000000000LL,2000000000LL})==0);
    CHECK(ccw.ret({LLONG_MIN,0},{LLONG_MAX,1},{0,2})==1);
}

void check_dlog_phi(){
    numeric_templates::_dlog d;
    for(auto [n,expected]:vector<pair<long long,long long>>{{1,1},{2,1},{7,6},{12,4},{36,12},{49,42},{100,40},{1000000007,1000000006}})
        CHECK(d.phi(n)==expected);
    d.factor(60);CHECK(d.prime==vector<long long>({2,3,5}));
    d.factor(7);CHECK(d.prime==vector<long long>({7}));
}

void check_dlog_large_arithmetic(){
    numeric_templates::_dlog d;
    CHECK(d.pow(4294967290LL,2,4294967291LL)==1);
    CHECK(d.pow(LLONG_MAX-1,2,LLONG_MAX)==1);
    CHECK(d.sqrt(LLONG_MAX)==3037000499LL);
    CHECK(d.sqrt(0)==0);CHECK(d.sqrt(1)==0);CHECK(d.sqrt(4)==1);CHECK(d.sqrt(9)==2);CHECK(d.sqrt(10)==3);
    CHECK(d.ret(4294967290LL,4294967290LL,4294967291LL)==1);
    CHECK(d.ret(4294967290LL,1,4294967291LL)==0);
    for(long long p:{2,3,5,7,11,13,17,19,23,29,31})for(long long a=1;a<p;a++)for(long long b=0;b<p;b++){
        long long expected=-1,cur=1;
        for(long long exponent=0;exponent<p-1;exponent++){
            if(cur==b){expected=exponent;break;}
            cur=cur*a%p;
        }
        CHECK(d.ret(a,b,p)==expected);
    }
}

void check_dlog_normalized_target(){
    numeric_templates::_dlog d;
    for(auto [a,b,p,expected]:vector<tuple<long long,long long,long long,long long>>{
        {2,8,7,0},{2,15,7,0},{2,-6,7,0},{2,-13,7,0},
        {2,9,7,1},{2,-5,7,1},{2,11,7,2},{2,-3,7,2},
        {2,10,7,-1},{2,-4,7,-1},{2,7,7,-1},
        {2,LLONG_MAX-6,7,0},{2,LLONG_MIN+2,7,0},
        {2,14,13,0},{2,-11,13,1},{3,16,13,1},{3,-4,13,2},{3,15,13,-1}})
        CHECK(d.ret(a,b,p)==expected);
}

void check_area_translated(){
    geometry::_area area;
    for(long long offset:{1000000000000LL,-1000000000000LL,0LL}){
        for(long long width:{1LL,2000000000LL}){
            vector<pair<long long,long long>> polygon{{offset,offset},{offset+width,offset},{offset+width,offset+1},{offset,offset+1}};
            for(int rotation=0;rotation<4;rotation++){
                CHECK(area.ret(polygon)==2*width);
                reverse(polygon.begin(),polygon.end());CHECK(area.ret(polygon)==2*width);
                reverse(polygon.begin(),polygon.end());rotate(polygon.begin(),polygon.begin()+1,polygon.end());
            }
        }
    }
    vector<pair<long long,long long>> empty;CHECK(area.ret(empty)==0);
    vector<pair<long long,long long>> triangle{{0,0},{3,0},{0,1}};CHECK(area.ret(triangle)==3);
}

int main(){
    int failures = 0;
    for(auto [name, test] : vector<pair<string,void(*)()>>{
        {"kmp_policy",check_kmp_policy},{"kmp_go",check_kmp_go},{"rc_split",check_rc_split},{"gf2",check_gf2},{"fixed_comb",check_fixed_comb},{"fixed_matrix",check_fixed_matrix},{"fixed_mint",check_fixed_mint},{"dynamic_mint",check_dynamic_mint},{"dynamic_mint_unsigned",check_dynamic_mint_unsigned},{"crt",check_crt},{"crt_dependents",check_crt_dependents},{"fft_arrays",check_fft_arrays},{"fft_output",check_fft_output},{"fft_carry",check_fft_carry},{"manacher_values",check_manacher_values},{"manacher",check_manacher},{"cht",check_cht},{"pollard_one",check_pollard_one},{"line_same",check_line_same},
        {"xor_thresholds",check_xor_thresholds},{"bcc_parallel",check_bcc_parallel},{"alphabet_policies",check_alphabets},
        {"lift_large_cost",check_lift_large_cost},{"binary_search_large_bounds",check_binary_search_large_bounds},
        {"ternary_search_large_bounds",check_ternary_search_large_bounds},{"ccw_large_coordinates",check_ccw_large_coordinates},
        {"dlog_phi",check_dlog_phi},{"dlog_large_arithmetic",check_dlog_large_arithmetic},
        {"dlog_normalized_target",check_dlog_normalized_target},{"area_translated",check_area_translated}}){
        try { test(); cout << "PASS " << name << '\n'; }
        catch(const exception& e){ ++failures; cerr << "FAIL " << name << ": " << e.what() << '\n'; }
    }
    cout << checks << " bug-regression checks, " << failures << " failed groups\n";
    return failures != 0;
}
