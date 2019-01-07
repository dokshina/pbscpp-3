#include <iostream>
#include <unordered_map>

#include <gtest/gtest.h>

#include "IProduct.hpp"
#include "IShop.hpp"

IProduct::ProductId IProduct::product_id = 0;
IShop::ShopId IShop::shop_id = 0;

template<typename TKey, typename TValue>
bool contains(const std::unordered_map<TKey, TValue>& map, const TKey& key) {
    return map.find(key) != map.end();
}

TEST(Test, Product) {
    IProduct prod1(10);
    ASSERT_EQ(10, prod1.GetPrice());
    ASSERT_EQ(prod1.GetState(), IProduct::ProductState::Created);

    prod1.ChangePrice(20);
    ASSERT_EQ(20, prod1.GetPrice());

    ASSERT_THROW(prod1.StopSales(), std::logic_error);
    prod1.StartSales();
    ASSERT_EQ(prod1.GetState(), IProduct::ProductState::SalesStarted);

    ASSERT_THROW(prod1.StartSales(), std::logic_error);
    prod1.StopSales();
    ASSERT_EQ(prod1.GetState(), IProduct::ProductState::SalesStopped);

    ASSERT_THROW(prod1.StartSales(), std::logic_error);
    ASSERT_THROW(prod1.StopSales(), std::logic_error);
}

TEST(Test, Shop) {
    IShop shop1;
    ASSERT_EQ(shop1.GetState(), IShop::ShopState::Open);
    ASSERT_EQ(0, shop1.GetIncome());

    shop1.Close();
    ASSERT_EQ(shop1.GetState(), IShop::ShopState::Closed);

    ASSERT_THROW(shop1.Close(), std::logic_error);
}

TEST(Test, AttachDetach) {
    IProduct prod1(10);
    IProduct prod2(20);
    IShop shop1;
    IShop shop2;

    ASSERT_TRUE(shop1.GetProducts().empty());
    ASSERT_TRUE(shop2.GetProducts().empty());

    prod1.Attach(&shop1);
    ASSERT_EQ(1, shop1.GetProducts().size());
    ASSERT_TRUE(contains(shop1.GetProducts(), prod1.GetId()));

    prod2.Attach(&shop1);
    ASSERT_EQ(2, shop1.GetProducts().size());
    ASSERT_TRUE(contains(shop1.GetProducts(), prod1.GetId()));
    ASSERT_TRUE(contains(shop1.GetProducts(), prod2.GetId()));

    prod1.Attach(&shop2);
    ASSERT_EQ(1, shop2.GetProducts().size());
    ASSERT_TRUE(contains(shop2.GetProducts(), prod1.GetId()));

    prod1.Detach(&shop2);
    ASSERT_FALSE(contains(shop2.GetProducts(), prod1.GetId()));

    ASSERT_THROW(prod1.Detach(&shop2), std::runtime_error);

    shop2.Close();
    ASSERT_THROW(prod2.Attach(&shop2), std::logic_error);
    ASSERT_THROW(prod1.Detach(&shop2), std::logic_error);
}

TEST(Test, Sale) {
    IProduct prod1(10);
    IProduct prod2(20);
    IShop shop1;
    IShop shop2;

    prod1.StartSales();
    prod1.Attach(&shop1);
    ASSERT_TRUE(shop1.Sell(prod1, prod1.GetPrice()));
    ASSERT_EQ(prod1.GetPrice(), shop1.GetIncome());
    ASSERT_FALSE(shop1.Sell(prod1, prod1.GetPrice() + 1));

    prod2.StartSales();
    prod2.Attach(&shop1);
    ASSERT_TRUE(shop1.Sell(prod2, prod2.GetPrice()));
    ASSERT_EQ(prod1.GetPrice() + prod2.GetPrice(), shop1.GetIncome());

    prod2.StopSales();
    ASSERT_FALSE(shop1.Sell(prod2, prod2.GetPrice()));

    auto oldPrice = prod1.GetPrice();
    prod1.ChangePrice(20);
    ASSERT_FALSE(shop1.Sell(prod1, oldPrice));

    ASSERT_THROW(shop2.Sell(prod1, prod1.GetPrice()), std::runtime_error);

    shop1.Close();
    ASSERT_THROW(shop1.Sell(prod1, prod1.GetPrice()), std::logic_error);
}
