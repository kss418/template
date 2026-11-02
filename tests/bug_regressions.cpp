#include <bits/stdc++.h>
namespace fft_templates {
#include "../FFT.cpp"
}
namespace numeric_templates {
#include "../Number_Theory.cpp"
}
namespace combinatorics {
using numeric_templates::_dynamic_mint;
using numeric_templates::_crt;
#include "../Combinatorics.cpp"
}
namespace geometry {
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

int main(){
    int failures = 0;
    for(auto [name, test] : vector<pair<string,void(*)()>>{
        {"crt",check_crt},{"crt_dependents",check_crt_dependents},{"fft_arrays",check_fft_arrays},{"fft_output",check_fft_output},{"fft_carry",check_fft_carry},{"manacher",check_manacher},{"cht",check_cht},{"pollard_one",check_pollard_one},{"line_same",check_line_same},
        {"xor_thresholds",check_xor_thresholds},{"bcc_parallel",check_bcc_parallel},{"alphabet_policies",check_alphabets}}){
        try { test(); cout << "PASS " << name << '\n'; }
        catch(const exception& e){ ++failures; cerr << "FAIL " << name << ": " << e.what() << '\n'; }
    }
    cout << checks << " bug-regression checks, " << failures << " failed groups\n";
    return failures != 0;
}
