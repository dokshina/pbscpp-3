#ifndef __IPRODUCT_HPP
#define __IPRODUCT_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <mutex>

class IShop;

class IProduct
{
public:
    typedef unsigned long ProductId;
    typedef double Price;
    enum class ProductState
    {
        Created,
        SalesStarted,
        SalesStopped
    };

public:
    static ProductId product_id;

    explicit IProduct(const Price& price);
    ~IProduct();

    ProductId GetId() const;
    Price GetPrice() const;
    ProductState GetState() const;

    void ChangePrice(const double& price);

    void StartSales();
    void StopSales();

    void Attach(IShop* shop);
    void Detach(IShop* shop);

    std::string ToString() const;

private:
    ProductId id_;
    volatile Price price_;
    volatile ProductState state_;
    std::mutex m_;
};

#endif
