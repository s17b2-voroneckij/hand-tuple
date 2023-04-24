#include <iostream>
#include <any>
 
using std::cout;
using std::any_cast;
 
template<typename... Types>
class my_tuple;
 
template<typename First, typename ...Rest>
class my_tuple<First, Rest...> {
protected:
    my_tuple<Rest...> rest;
    First first;
 
public:
    my_tuple() = default;
 
    explicit my_tuple(First first, Rest ...rest) {
        this->first = first;
        this->rest = my_tuple<Rest...>(rest...);
    }
 
    template<int n>
    std::any get() {
        if (n == 0) {
            return first;
        }
        return rest.template get<n - 1>();
    }
};
 
template<>
class my_tuple<> {
public:
    my_tuple() = default;
 
    template<auto n>
    std::any get() {
        return {};
    }
};
 
template<auto n, typename... Types>
struct get_helper;
 
template<typename First, typename... Rest>
struct get_helper<0, First, Rest...> : public my_tuple<First, Rest...> {
    typedef First type;
 
    explicit get_helper(const my_tuple<First, Rest...>& tup) : my_tuple<First, Rest...>(tup) {}
 
    type get_by_number() {
        return this->first;
    }
};
 
template<auto n, typename First, typename... Rest>
struct get_helper<n, First, Rest...> : public my_tuple<First, Rest...> {
    typedef typename get_helper<n - 1, Rest...>::type type;
 
    explicit get_helper(const my_tuple<First, Rest...>& tup) : my_tuple<First, Rest...>(tup) {}
 
    type get_by_number() {
        get_helper<n - 1, Rest...> helper(this->rest);
        return helper.get_by_number();
    }
};
 
template<auto n, typename... Types>
typename get_helper<n, Types...>::type my_get(const my_tuple<Types...>& tup) {
    get_helper<n, Types...> helper(tup);
    return helper.get_by_number();
}
 
template<typename GetType, typename... Types>
struct get_by_type_helper;
 
template<typename First, typename... Rest>
struct get_by_type_helper<First, First, Rest...> : public my_tuple<First, Rest...> {
    explicit get_by_type_helper(const my_tuple<First, Rest...>& tup) : my_tuple<First, Rest...>(tup) {}
 
    First get_by_type() {
        return this->first;
    }
};
 
template<typename GetType, typename First, typename... Rest>
struct get_by_type_helper<GetType, First, Rest...> : public my_tuple<First, Rest...> {
    explicit get_by_type_helper(const my_tuple<First, Rest...>& tup) : my_tuple<First, Rest...>(tup) {}
 
    GetType get_by_type() {
        get_by_type_helper<GetType, Rest...> helper(this->rest);
        return helper.get_by_type();
    }
};
 
template<typename GetType, typename... Types>
GetType my_get(const my_tuple<Types...>& tup) {
    get_by_type_helper<GetType, Types...> helper(tup);
    return helper.get_by_type();
}
 
int main() {
    my_tuple<int> tup(1);
    // cout << any_cast<int>(tup.get<0>()) << " " << std::any_cast<long>(tup.get<1>()) << std::endl;
    my_tuple<int, long, double, long double> tup2(0, 1ll, 1.0, 2.0);
    cout << any_cast<int>(tup2.template get<0>()) << " " << any_cast<long>(tup2.template get<1>()) <<
         " " << any_cast<double>(tup2.template get<2>()) << " " << any_cast<long double>(tup2.template get<3>()) << std::endl;
 
    //cout << tup.get_r<0>() << " " /*<< tup.get_r<1>() */<< std::endl;
    //cout << my_get<0>(tup) << std::endl;
    cout << my_get<0>(tup) << std::endl;
    cout << my_get<2>(tup2) << std::endl;
    cout << my_get<long double>(tup2) << std::endl;
}
