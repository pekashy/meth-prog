#include "pch.h"
#include <iostream>


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



 // class template Append
 // Appends a type or a typelist to another
 // Invocation (TList is a typelist and T is either a type or a typelist):
 // Append<TList, T>::Result
 // returns a typelist that is TList followed by T and NullType-terminated
 
template <class TList, class T>
struct Append;
      
template <> struct Append<Nulltype, Nulltype>
{
	typedef Nulltype Result;
};
    
template <class T> struct Append<Nulltype, T>
{
	typedef TypeList<T,Nulltype> Result;
};
 
template <class Head, class Tail>
struct Append<Nulltype, TypeList<Head, Tail> >
{
	typedef TypeList<Head, Tail> Result;
};
      
template <class Head, class Tail, class T>
struct Append<TypeList<Head, Tail>, T>
{
		typedef TypeList<Head, typename Append<Tail, T>::Result> Result;
};



template <class ...TList>
struct Append<Nulltype, TypeList<TList...> >
{
    using Result = TypeList<TList...>;
};

template <class ...TList, class T>
struct Append<TypeList<TList...>, T>
{
    using Result = typename TypeList<typename TypeList<TList...>::Head, typename Append<typename TypeList<TList...>::Tail, T>::Result>;
};
 // class template Reverse
 // Reverses a typelist
 // Invocation (TList is a typelist):
 // Reverse<TList>::Result
 // returns a typelist that is TList reversed

template <class TList> struct Reverse;

template <>
struct Reverse<Nulltype>
{
	typedef Nulltype Result;
};

template <class Head, class Tail>
struct Reverse< TypeList<Head, Tail>>
{
	typedef typename Append<typename Reverse<Tail>::Result, Head>::Result Result;
};

template <class ...TList>
struct Reverse< typename TypeList<TList...>>
{
    using Result = typename Append<typename TypeList<TList...>::Tail, typename TypeList<TList...>::Head>::Result;
};

template<class TList, template <class T, class Base> class Unit, class Root = Nulltype>
struct GenLinearHierarchy;

template<class ...TList, template <class T, class Base> class Unit, class Root>
struct GenLinearHierarchy<TypeList<TList...>, Unit, Root> :
    public Unit< typename TypeList<TList...>::Head, GenLinearHierarchy<typename TypeList<TList...>::Tail, Unit, Root>>
{

};

template<class TList, template <class T, class Base> class Unit, class Root>
struct GenLinearHierarchy<TypeList<TList>, Unit, Root> :
    public Unit<TList, Root>
{

};
//...................................................................//


// Check if type Exists in TypeList
template <typename T, typename TList>
struct HasType;

template <typename T, typename... Types>
struct HasType<T, TypeList<Types...>> : std::disjunction<std::is_same<T, Types>...> {};


// Classes Definitions for Abstract Factory
struct Good
{
    Good() { std::cout << "Good Created" << std::endl; }
    virtual ~Good() = default;
};

struct Chair : public Good
{
    Chair() { std::cout << "Chair Created" << std::endl; }
    virtual ~Chair() = default;
};

struct Table : public Good
{
    Table() { std::cout << "Table Created" << std::endl; }
    virtual ~Table() = default;

};

struct Sofa : public Good
{
    Sofa() { std::cout << "Sofa Created" << std::endl; }
    virtual ~Sofa() = default;

};

struct WoodenSofa : public Sofa
{
    WoodenSofa() { std::cout << "WoodenSofa Created" << std::endl; }
    virtual ~WoodenSofa() = default;

};

struct WoodenTable : public Table
{
    WoodenTable() { std::cout << "WoodenTable Created" << std::endl; }
    virtual ~WoodenTable() = default;

};

struct WoodenChair : public Chair
{
    WoodenChair() { std::cout << "WoodenChair Created" << std::endl; }
    virtual ~WoodenChair() = default;

};

struct MetalSofa : public Sofa
{
    MetalSofa() { std::cout << "MetalSofa Created" << std::endl; }
    virtual ~MetalSofa() = default;
};

struct MetalTable : public Table
{
    MetalTable() { std::cout << "MetalTable Created" << std::endl; }
    virtual ~MetalTable() = default;
};

struct MetalChair : public Chair
{
    MetalChair() { std::cout << "MetalChair Created" << std::endl; }
    virtual ~MetalChair() = default;
};

struct MetalRichSofa : public MetalSofa
{
    MetalRichSofa() { std::cout << "MetalRichSofa Created" << std::endl; }
    virtual ~MetalRichSofa() = default;
};

struct MetalRichTable : public MetalTable
{
    MetalRichTable() { std::cout << "MetalRichTable Created" << std::endl; }
    virtual ~MetalRichTable() = default;
};

struct MetalRichChair : public MetalChair
{
    MetalRichChair() { std::cout << "MetalRichChair Created" << std::endl; }
    virtual ~MetalRichChair() = default;

};

struct MetalPoorSofa : public MetalSofa
{
    MetalPoorSofa() { std::cout << "MetalPoorSofa Created" << std::endl; }
    virtual ~MetalPoorSofa() = default;
};

struct MetalPoorTable : public MetalTable
{
    MetalPoorTable() { std::cout << "MetalPoorTable Created" << std::endl; }
    virtual ~MetalPoorTable() = default;
};

struct MetalPoorChair : public MetalChair
{
    MetalPoorChair() { std::cout << "MetalPoorChair Created" << std::endl; }
    virtual ~MetalPoorChair() = default;
};

template < typename T>
struct Type2Type
{
    using OriginalType = T;
};

// Creates Objects
template<class T>
struct AbstractFactoryUnit
{
    virtual T* DoCreate(Type2Type<T>) = 0;
    virtual ~AbstractFactoryUnit() = default;
};



// Implements DoCreate for the whole list
template <class ConcreteProduct, class Base>
struct OpNewFactoryUnit : public Base
{
    using BaseProductList = typename Base::ProductList;
    using ProductList = typename  BaseProductList::Tail;
    using AbstractProduct = typename BaseProductList::Head;

    ConcreteProduct* DoCreate(Type2Type<AbstractProduct>)
    {
        auto p = new ConcreteProduct();
        return p;
    }
};

/*
// Holder for factories
template <class T>
struct AbstractFactoryUnit
{
    virtual T* DoCreate() { return new T(); };
    virtual ~AbstractFactoryUnit() = default;
};
*/

// Generate scatter Hierarchy for single TypeList
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
    public Unit<T>
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



// Generate Factory Hierarchy for TypeList of TypeList, structural element is Scatter Hierarchy
template <int NLists, template <class> class Unit, class TListList>
struct GenComplexHierarchy;

// Single List
template <template <class> class Unit, class ...Types>
struct GenComplexHierarchy<1, Unit, typename TypeList<Types...>> :
    public GenScatterHierarchy<Types..., Unit>
{
    using ProductList = typename TypeList<Types...>;
    using LeftBase = GenScatterHierarchy<Types..., Unit>;
    using RightBase = Nulltype;
};

//List of Lists
template <int NLists, template <class> class Unit, class ...TLists>
struct GenComplexHierarchy<NLists, Unit, typename TypeList<TLists...>> :
    public GenScatterHierarchy<typename TypeList<TLists...>::Head, Unit>,
    public GenComplexHierarchy<NLists - 1, Unit, typename TypeList<TLists...>::Tail>
{
    using LeftBase = GenScatterHierarchy<typename TypeList<TLists...>::Head, Unit>;
    using RightBase = GenComplexHierarchy<NLists - 1, Unit, typename TypeList<TLists...>::Tail>;
    using ProductList = typename TypeList<TLists...>::Head;
};


template
<
    class TList,
    template <class> class Unit = AbstractFactoryUnit
>
struct AbstractFactory : public GenScatterHierarchy<TList, Unit>
{
    using ProductList = TList;
    // Dispatches object creation to related class
    template <class T> T* Create()
    {
        Unit <T> unit = this;
        return unit->DoCreate(Type2Type<T>());
    }
    using Parent = GenScatterHierarchy<TList, Unit>;
};

template
<
    int NLists,
    class TList,
    template <class> class Unit = AbstractFactoryUnit
>
struct AbstractComplexFactory : public GenComplexHierarchy<NLists, Unit, TList>
{
    using ProductList = TList;
    // Dispatches object creation to related class
    template <class T> T* Create()
    {
        Unit <T>& unit = *this;
        return unit.DoCreate(Type2Type<T>());
    }
    using Parent = GenComplexHierarchy<NLists, Unit, TList>;
};

// Generates Hierarchy using GenLinearHierarchy
template
<
    class AbstractFact,
    template <class, class> class Creator = OpNewFactoryUnit,
    class ...TList
>
class ConcreteFactory
    : public GenLinearHierarchy<
		typename Reverse<TypeList<TList...>>::Result, Creator, AbstractFact>
{
    using ProductList = typename AbstractFact::ProductList;
    using ConcreteProductList = TypeList<TList...>;
};

// Find needed factory branch with needed factory in hierarchy
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
struct FindFactoryInHierarchy<false, TypeToFind, ChildFactory, GenComplexHierarchy<NLists, Unit, TLists...>>
{
    using CurrentFactory = typename GenComplexHierarchy<NLists, Unit, TLists...>;
	
    using CurrentTypeList = typename CurrentFactory::ProductList;
    using CurrentTypeToFind = TypeToFind;
    static const bool iof = HasType<CurrentTypeToFind, CurrentTypeList>::value;
    using result = typename FindFactoryInHierarchy <
        iof, // check if needed value is in related list
        TypeToFind,
        CurrentFactory, // In case of success will set result to this
        typename CurrentFactory::RightBase // Continue search in here in case of failure
    >::result;
};

// Found factory that generates that type of typeList in hierarchy
template
<
    int NLists,
    class TypeToFind,
    template <class> class Unit,
    class ...TLists,
    class CurrFactory
>
struct FindFactoryInHierarchy<true, TypeToFind, GenComplexHierarchy<NLists, Unit, TLists...> /* Child Factory, answer */, CurrFactory>
{
    using CurrentTypeList = typename GenComplexHierarchy<NLists, Unit, TLists...>::ProductList; // List that includes needed type
    using CurrentFactory = typename AbstractFactory<CurrentTypeList>; // Factory that generates that type
    using result = typename ConcreteFactory<CurrentFactory, OpNewFactoryUnit, CurrentTypeList>; // Concrete factory
};



// Interface to Use Abstract Factory pattern
template
<
    int NElements,
	int NLists,
    class ...TypeTypeList
>
struct GetAbstractFactory
{
	using TypeTypeListList = TypeList<TypeTypeList...>;
    using AbstractFactory = AbstractComplexFactory<NLists, TypeTypeListList>;
    template <typename T>
    using GetConcreteFactory = typename FindFactoryInHierarchy<false, T, Nulltype, typename AbstractFactory::Parent>;
};


int main()
{
    using MyFactoryHierarchy = GetAbstractFactory<3, 5, TypeList<Chair, Table, Sofa>, TypeList< WoodenChair, WoodenTable, WoodenSofa>,
        TypeList< MetalChair, MetalTable, MetalSofa>, TypeList< MetalRichChair, MetalRichTable, MetalRichSofa>,
        TypeList< MetalPoorChair, MetalPoorTable, MetalPoorSofa>>;
	
    auto pMetalRichChairFactory = new MyFactoryHierarchy::GetConcreteFactory<MetalSofa>::result();
    /*auto pMetalRichChair = pMetalRichChairFactory->Create<MetalRichChair>();

    auto pMetalSofaFactory = new MyFactoryHierarchy::GetConcreteFactory<MetalSofa>::result();
    auto pMetalSofa = pMetalSofaFactory->Create<MetalSofa>();

    auto pWoodenTableFactory = new MyFactoryHierarchy::GetConcreteFactory<WoodenTable>::result();
    auto pWoodenTable = pWoodenTableFactory->Create<WoodenTable>();*/
}