#include "pch.h"
#include <iostream>

class A
{

};

class BA
{

};

class BA1 : public BA
{

};

class BA2 : public BA
{

};

class BA11 : public BA1
{

};

class BB
{

};

class BB1 : public BB
{

};

class BB2 : public BB
{

};

class BB21 : public BB2
{

};


template <typename T, typename Base>
struct DerHolder
    : public Base
{
    T _obj;
};


template <typename T>
struct Holder
{
    T _obj;
};


class Nulltype {};

template <typename ...Args>
struct TypeList
{
    using Head = Nulltype;
    using Tail = Nulltype;
};

template <typename H, typename ...T>
struct TypeList<H, T...>
{
    using Head = H;
    using Tail = TypeList<T...>;
};

typedef TypeList<> EmptyTypeList;

template <typename H>
struct TypeList<H>
{
    using Head = H;
    using Tail = EmptyTypeList;
};



//...................................................................//


template <typename T>
struct Length
{
    static const int value = 1 + Length<T::Tail>::value;
};

template <>
struct Length<Nulltype>
{
    static const int value = 0;
};

//...................................................................//

template <size_t ind, typename ...T>
struct TypeAt {
    using res = Nulltype;
};

template <size_t ind, typename ...T>
struct TypeAt<ind, TypeList<T...>>
{
    using res = typename TypeAt<ind - 1, typename TypeList<T...>::Tail>::res;
};

template <size_t ind>
struct TypeAt<ind, Nulltype>
{
    using res = Nulltype;
};

template <typename ...T>
struct TypeAt<0, TypeList<T...>>
{
    using res = typename TypeList<T...>::Head;
};

//...................................................................//

/*
template <typename U, typename ...T>
struct IndexOf
{
    static const size_t value = 1;
};

template <typename ...K, typename U>
struct IndexOf<U, TypeList<K...>>
{
    static const size_t value = 1 + IndexOf<U, typename TypeList<K...>::Tail>::value;
};

template <typename T, typename ...K>
struct IndexOf<TypeList<T, K...>, T>
{
    static const size_t value = 0;
};


//...............................................................//
template <typename U, typename ...T>
struct IndexOf
{
    static const size_t value = 1;
};

template <typename ...K, typename U>
struct IndexOf<U, TypeList<K...>>
{
    static const size_t value = 1 + IndexOf<U, typename TypeList<K...>::Tail>::value;
};

template <typename T, typename ...K>
struct IndexOf<TypeList<T, K...>, T>
{
    static const size_t value = 0;
};
*/
//..................................................................//

template <typename T, typename TList>
struct HasType;

template <typename T, typename... Types>
struct HasType<T, TypeList<Types...>> : std::disjunction<std::is_same<T, Types>...> {};

//...................................................................//

template < typename U, size_t ind, typename ...T>
struct Add;


template < typename U, size_t ind, typename ...K>
struct Add<U, ind, TypeList<K...> >
{
    using result = TypeList<typename TypeList<K...>::Head, typename Add< U, ind - 1, typename TypeList<K...>::Tail>::result>;
};

template <typename U, typename T>
struct Add<U, 0, T>
{
    using result = TypeList <U, T>;
};

template <typename U, size_t ind>
struct Add<U, ind, TypeList<Nulltype>>
{
    using result = U;
};

//...................................................................//

template <typename U, typename ...T>
struct Remove;

template <typename U, typename ...T>
struct Remove<U, TypeList<T...>>
{
    using result = TypeList<typename TypeList<T...>::Head, typename Remove<U, typename TypeList<T...>::Tail>::result>;
};

template <typename T, typename ...K>
struct Remove<T, TypeList<T, K...>>
{
    using result = typename Remove<T, typename TypeList<T, K...>::Tail>::result;
};

template <typename T>
struct Remove< T, EmptyTypeList>
{
    using result = Nulltype;
};

//...................................................................//

template <class T, class U, class V> struct Replace;

template<class U, class V>
struct Replace<Nulltype, U, V>
{
    using res = TypeList<Nulltype>;
};

template <class ...T, class U, class V>
struct Replace<TypeList<U, T...>, U, V>
{
    using res = TypeList<V, T...>;
};

template <class ...T, class U, class V>
struct Replace<TypeList<T...>, U, V>
{
    using res = TypeList< typename TypeList<T...>::Head,
        typename Replace<typename TypeList<T...>::Tail, U, V>::res >;
};



//...................................................................//

template<bool is, typename T, typename U> struct Select;

template<typename T, typename U>
struct Select<true, T, U>
{
    using res = T;
};

template<typename T, typename U>
struct Select<false, T, U>
{
    using res = U;
};

//...................................................................//

template <typename T, typename U>
class Conversion
{
    using Small = char;
    struct Big { char x[2]; };
    static Small Test(const U&);
    static Big Test(...);
    static T makeT();

public:
    static constexpr bool exists = sizeof(Small) == sizeof(Test(makeT()));
    static constexpr bool equal = false;
};

template <typename T>
class Conversion<T, T>
{
public:
    static constexpr bool exists = true;
    static constexpr bool equal = true;
};

template <bool one, bool two>
struct ANDNOT
{
    static constexpr bool res = false;
};

template <>
struct ANDNOT<true, false>
{
    static constexpr bool res = true;
};

template <typename T, typename U>
struct isBase
{
    static constexpr bool res = ANDNOT< Conversion<T, U>::exists, Conversion<T, U>::equal >::res;
};



//...................................................................//

template <class TList, class T>
struct MostDerived;

template <class T>
struct MostDerived<Nulltype, T>
{
    using res = T;
};

template <class ...TList, class T>
struct MostDerived<TypeList<TList...>, T>
{
    using Cand = typename MostDerived<typename TypeList<TList...>::Tail, T>::res;

    using res = typename Select<isBase<typename TypeList<TList...>::Head, Cand>::res,
        typename TypeList<TList...>::Head, Cand>::res;
};

template<class T> struct DerivedToFront
{
    using res = Nulltype;
};

template<>
struct DerivedToFront<TypeList<Nulltype>>
{
    using res = Nulltype;
};

template <typename H>
struct DerivedToFront<TypeList<H>>
{
    using res = H;
};

template <typename ...TList>
struct DerivedToFront<TypeList<TList...>>
{
private:

    using TheMostDerived = typename MostDerived<typename TypeList<TList...>::Tail, typename TypeList<TList...>::Head>::res;

    using Temp = typename Replace< typename TypeList<TList...>::Tail, TheMostDerived, typename TypeList<TList...>::Head >::res;

    using L = typename DerivedToFront<Temp>::res;

public:
    using res = TypeList<TheMostDerived, L>;

};

template <class TList, template <class> class Unit>
class GenScatterHierarchy;

template <class ...TList, template <class> class Unit>
class GenScatterHierarchy<typename TypeList<TList...>, Unit> :
    public Unit<typename TypeList<TList...>::Head>,
    public GenScatterHierarchy<typename TypeList<TList...>::Tail, Unit>
{
    using LeftBase = Unit<typename TypeList<TList...>::Head>;
    using RightBase = GenScatterHierarchy<typename TypeList<TList...>::Tail, Unit>;
};


template <class T, template <class> class Unit>
class GenScatterHierarchy<typename TypeList<T>, Unit> :
    public Unit<typename TypeList<T>::Head>
{
    using LeftBase = T;
    using RightBase = Nulltype;
};


template <template <class> class Unit>
class GenScatterHierarchy<typename TypeList<Nulltype>, Unit>
{
    using LeftBase = Nulltype;
    using RightBase = Nulltype;
};

/*----------------*/

template<class TList, template <class T, class Base> class Unit, class Root = Nulltype>
struct GetLinearHierarchy;

template<class ...TList, template <class T, class Base> class Unit, class Root>
struct GetLinearHierarchy<TypeList<TList...>, Unit, Root> :
    public Unit< typename TypeList<TList...>::Head, GetLinearHierarchy<typename TypeList<TList...>::Tail, Unit, Root>>
{

};

template<class TList, template <class T, class Base> class Unit, class Root>
struct GetLinearHierarchy<TypeList<TList>, Unit, Root> :
    public Unit< TList, Root>
{

};

/*-------------------------------*/

/*----------------*/

template <class T, T v>
struct integral_constant
{
    static constexpr T value = v;
    using value_type = T;

    constexpr operator value_type() const noexcept { return value; }
    constexpr value_type operator()() const noexcept { return value; }
};


template <typename T>
struct Type2Type
{
    using OriginalType = T;
};

struct Good
{
    Good() { std::cout << "Good Created" << std::endl; }
};

struct Chair : public Good
{
    Chair() { std::cout << "Chair Created" << std::endl; }
};

struct Table : public Good
{
    Table() { std::cout << "Table Created" << std::endl; }
};

struct Sofa : public Good
{
    Sofa() { std::cout << "Sofa Created" << std::endl; }
};

struct WoodenSofa : public Sofa
{
    WoodenSofa() { std::cout << "WoodenSofa Created" << std::endl; }
};

struct WoodenTable : public Table
{
    WoodenTable() { std::cout << "WoodenTable Created" << std::endl; }
};

struct WoodenChair : public Chair
{
    WoodenChair() { std::cout << "WoodenChair Created" << std::endl; }
};

struct MetalSofa : public Sofa
{
    MetalSofa() { std::cout << "MetalSofa Created" << std::endl; }
};

struct MetalTable : public Table
{
    MetalTable() { std::cout << "MetalTable Created" << std::endl; }
};

struct MetalChair : public Chair
{
    MetalChair() { std::cout << "MetalChair Created" << std::endl; }
};

struct MetalRichSofa : public MetalSofa
{
    MetalRichSofa() { std::cout << "MetalRichSofa Created" << std::endl; }
};

struct MetalRichTable : public MetalTable
{
    MetalRichTable() { std::cout << "MetalRichTable Created" << std::endl; }
};

struct MetalRichChair : public MetalChair
{
    MetalRichChair() { std::cout << "MetalRichChair Created" << std::endl; }
};

struct MetalPoorSofa : public MetalSofa
{
    MetalPoorSofa() { std::cout << "MetalPoorSofa Created" << std::endl; }
};

struct MetalPoorTable : public MetalTable
{
    MetalPoorTable() { std::cout << "MetalPoorTable Created" << std::endl; }
};

struct MetalPoorChair : public MetalChair
{
    MetalPoorChair() { std::cout << "MetalPoorChair Created" << std::endl; }
};


template <class T>
struct AbstractFactoryUnit
{
    virtual T* DoCreate(Type2Type<T>) { return new T; };
    virtual ~AbstractFactoryUnit() = default;
};

template
<
    class TList,
    template <class> class Unit = AbstractFactoryUnit
>
struct AbstractFactory : public GenScatterHierarchy<TList, Unit>
{
    typedef TList ProductList;
    template <class T> T* Create()
    {
        Unit <T>& unit = *this;
        return unit.DoCreate(Type2Type<T>());
    }
};

template
<
    class TList,
    template <class> class Unit = AbstractFactoryUnit
>
struct GetSimpleFactory
{
    using GetAbstractFactory = AbstractFactory<TList>;
};

template <int NLists, template <class> class Unit, class TListList>
struct GenFuckedUpHierarchy;

// Single List

template <template <class> class Unit, class ...Types>
struct GenFuckedUpHierarchy<1, Unit, typename TypeList<Types...>> :
    public GenScatterHierarchy<Types..., Unit>
{
    using ProductList = typename TypeList<Types...>;
    using LeftBase = GenScatterHierarchy<Types..., Unit>;
    using RightBase = Nulltype;
};

//List of Lists
template <int NLists, template <class> class Unit, class ...TLists>
struct GenFuckedUpHierarchy<NLists, Unit, typename TypeList<TLists...>> :
    public GenScatterHierarchy<typename TypeList<TLists...>::Head, Unit>,
    public GenFuckedUpHierarchy<NLists - 1, Unit, typename TypeList<TLists...>::Tail>
{
    using LeftBase = GenScatterHierarchy<typename TypeList<TLists...>::Head, Unit>;
    using RightBase = GenFuckedUpHierarchy<NLists - 1, Unit, typename TypeList<TLists...>::Tail>;
    using ProductList = typename TypeList<TLists...>::Head;
};

template
<
    int NLists,
    class TList,
    template <typename> typename Unit = AbstractFactoryUnit
>
struct AbstractFuckedUpFactory : public GenFuckedUpHierarchy<NLists, Unit, TList>
{

    template <class T> T* Create()
    {
        Unit <T>& unit = *this;
        return unit.DoCreate(Type2Type<T>());
    }
    using Parent = GenFuckedUpHierarchy<NLists, Unit, TList>;
    using ProductList =  TList;
};

template
<
    bool bTypeInList,
    class CurrentFactory,
    class TypeToFind,
    class ChildFactory
>
struct FindFactoryInHierarchy;


// Not found children factory
template
<
    class TypeToFind,
    class ChildFactory,
    int NLists,
    template <class> class Unit,
    class ...TLists
>
struct FindFactoryInHierarchy<false, TypeToFind, ChildFactory, GenFuckedUpHierarchy<NLists, Unit, TLists...>>
{
    using CurrentFactory = typename GenFuckedUpHierarchy<NLists, Unit, TLists...>;
    using CurrentTypeList = typename CurrentFactory::ProductList;
    using CurrentTypeToFind = TypeToFind;
    static const bool iof = HasType<CurrentTypeToFind, CurrentTypeList>::value;
    using result = typename FindFactoryInHierarchy <
        iof, // check if needed value is in related list
        TypeToFind,
        CurrentFactory, // In case of success will set result to this
        typename CurrentFactory::RightBase
    >::result;
};

// Type found in children factory, return children factory
template
<
    int NLists,
    class TypeToFind,
    template <class> class Unit,
    class ...TLists,
    class CurrFactory
>
struct FindFactoryInHierarchy<true, TypeToFind, GenFuckedUpHierarchy<NLists, Unit, TLists...>, CurrFactory>
{
    using result = typename AbstractFuckedUpFactory<NLists, TLists...>;
};

template
<
    int NElements,
	int NLists,
    class ...TypeTypeList
>
struct GetAbstractFactory
{
	using TypeTypeListList = TypeList<TypeTypeList...>;
    using AbstractFactory = AbstractFuckedUpFactory<NLists, TypeTypeListList>;
    template <typename T>
    using GetConcreteFactory = typename FindFactoryInHierarchy<false, T, Nulltype, typename AbstractFactory::Parent>;
};



int main()
{
    //Factory* MyFactory = new SimpleFactoryHierarchy::GetAbstractFactory<Chair>::result;

    using MyFactoryHierarchy = GetAbstractFactory<3, 5, TypeList<Chair, Table, Sofa>, TypeList< WoodenChair, WoodenTable, WoodenSofa>,
        TypeList< MetalChair, MetalTable, MetalSofa>, TypeList< MetalRichChair, MetalRichTable, MetalRichSofa>,
        TypeList< MetalPoorChair, MetalPoorTable, MetalPoorSofa>>;
	
    //Good* Sofafactory = new MyFactoryHierarchy;
    //auto* pAbstractFactory = new MyFactoryHierarchy::AbstractFactory();
    auto pWoodenTableFactory = new MyFactoryHierarchy::GetConcreteFactory<MetalRichTable>::result();
    auto pWoodenTable = pWoodenTableFactory->Create<MetalRichTable>();
    /*auto* pConcreteFactorySofa = new MyFactoryHierarchy::GetTypeListWithType<Sofa>::result;*/
    /*using debug_tlist = FindFactoryInHierarchy<0, WoodenTable, Nulltype, MyFactoryHierarchy::AbstractFactory::Parent>::CurrentTypeList;
    debug_tlist dtl;
    using current_type_to_find = FindFactoryInHierarchy<0, WoodenTable, Nulltype, MyFactoryHierarchy::AbstractFactory::Parent>::CurrentTypeToFind;
    current_type_to_find cttf;
    auto debug = FindFactoryInHierarchy<0, WoodenTable, Nulltype, MyFactoryHierarchy::AbstractFactory::Parent>::iof;
    auto a = pAbstractFactory->Create<Sofa>();*/
    //auto b = pTypeListWithType->Create<Chair>();
    
    std::cout << Conversion<MetalPoorSofa, Sofa>::exists;

    /*
    std::cout << sizeof(sch) << std::endl;
    std::cout << sizeof(lh) << std::endl;

    TypeList<int, int, float, A> a;

	const int v = Length< TypeList<int, int, float, A, A>>::value;

	TypeAt<2, TypeList<int, int, float, A>>::res c = 1.f;

	size_t i = IndexOf<TypeList<int, float, double>, double>::value;

	Add<char, 1, TypeList<int, float, double>>::result d;

	Remove<char, TypeList<int, char, float, char, double>>::result e;

	Replace<TypeList<int, float, double>, double, char>::res rp;

	MostDerived<TypeList<BB, BA1, BB21, BA2, BA, BB2, BB1>, BB>::res md;

	Replace<TypeList<BB, BA1, BB21, BA2, BA, BB2, BB1>, MostDerived<TypeList<BB, BA1, BB21, BA2, BA, BB2, BB1, BA>, BA>::res, BB>::res;

	constexpr bool isb = isBase<BA1, BA>::res;
	constexpr bool isc = Conversion<BA1, BA>::exists;

	std::cout << Conversion<BA1, BA>::exists;

	DerivedToFront<TypeList<BB, BA1, BB21, BA2, BA, BB2, BB1>>::res dtf;


	GenScatterHierarchy<TypeList<int, float, double>, Holder> sch;
	GetLinearHierarchy<TypeList<int, float, double>, DerHolder> lh;	*/
}