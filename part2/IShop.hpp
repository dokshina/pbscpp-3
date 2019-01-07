#ifndef __ISHOP_HPP
#define __ISHOP_HPP

#include <unordered_map>
#include <string>
#include <mutex>

#include "IProduct.hpp"

class IShop
{
public:
    typedef unsigned long ShopId;
    typedef std::unordered_map<IProduct::ProductId, IProduct*> ProductsMap;
    typedef double Income;
    enum class ShopState
    {
        Open,
        Closed,
    };

public:
    static ShopId shop_id;

    explicit IShop();

    ShopId GetId() const;
    ShopState GetState() const;
    const ProductsMap& GetProducts() const;
    Income GetIncome() const;

    void Attach(IProduct* product);
    void Detach(IProduct* product);

    void Close();
    bool Sell(const IProduct& product, IProduct::Price);

    std::string ToString() const;

private:
    ShopId id_;
    ProductsMap products_;
    volatile ShopState state_;
    volatile Income income_;
    std::mutex m_;
};

#endif
