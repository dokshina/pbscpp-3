#include <iostream>
#include <thread>
#include <condition_variable>
#include <exception>

#include "IShop.hpp"
#include "IProduct.hpp"

IProduct::ProductId IProduct::product_id = 0;
IShop::ShopId IShop::shop_id = 0;

enum class ProductType {
    Cheese,
    Milk,
};


class Cheese : public IProduct {
public:
    Cheese(const double& weight, const IProduct::Price& price)
                                        : IProduct(price)
                                        , type_(ProductType::Cheese)
                                        , weight_(weight) {}
private:
    ProductType type_;
    double weight_;
};


class Milk : public IProduct {
public:
    Milk(const double& volume, const IProduct::Price& price)
                                        : IProduct(price)
                                        , type_(ProductType::Milk)
                                        , volume_(volume) {}
private:
    ProductType type_;
    double volume_;
};


int main() {
    std::condition_variable check_income;
    std::mutex m;
    volatile bool notified = false;
    volatile bool income_checked = false;

    IShop* shopPtr1 = new IShop;
    IShop* shopPtr2 = new IShop;

    IProduct::Price MILK_PRICE1 = 10.5;
    IProduct::Price CHEESE_PRICE1 = 4.0;
    IProduct::Price MILK_PRICE2 = 21.9;
    IProduct::Price CHEESE_PRICE2 = 1.5;

    std::thread sales1([&]() {
        Milk milk(1.0, MILK_PRICE1);
        milk.StartSales();
        milk.Attach(shopPtr1);
        milk.Attach(shopPtr2);

        Cheese cheese(0.5, CHEESE_PRICE1);
        cheese.StartSales();
        cheese.Attach(shopPtr1);
        cheese.Attach(shopPtr2);

        auto milkPrice = milk.GetPrice();
        auto cheesePrice = cheese.GetPrice();

        shopPtr2->Sell(milk, milkPrice);
        shopPtr2->Sell(cheese, cheesePrice);
        shopPtr1->Sell(milk, milkPrice);

        {
            std::unique_lock<std::mutex> lock(m);
            notified = true;
            check_income.notify_one();
        }

        {
            while (!income_checked) ;
            std::unique_lock<std::mutex> lock(m);
            std::cout << "Shop2 closing" << std::endl;
            shopPtr2->Close();
            notified = true;
            check_income.notify_one();
        }
    });

    std::thread sales2([&]() {
        Milk milk(2.0, MILK_PRICE2);
        milk.StartSales();
        milk.Attach(shopPtr1);

        Cheese cheese(0.2, CHEESE_PRICE2);
        cheese.StartSales();
        cheese.Attach(shopPtr2);

        auto milkPrice = milk.GetPrice();
        auto cheesePrice = cheese.GetPrice();

        shopPtr1->Sell(milk, milkPrice);
        shopPtr1->Sell(milk, milkPrice);
        shopPtr2->Sell(cheese, cheesePrice);

        {
            std::unique_lock<std::mutex> lock(m);
            while (!notified) check_income.wait(lock);

            std::cout << "Shop1 Income: " << shopPtr1->GetIncome() << std::endl;
            std::cout << "Shop2 Income: " << shopPtr2->GetIncome() << std::endl;
            assert(shopPtr2->GetIncome() == MILK_PRICE1+CHEESE_PRICE1 + CHEESE_PRICE2);
            assert(shopPtr1->GetIncome() == MILK_PRICE1 + 2*MILK_PRICE2);

            notified = false;
            income_checked = true;
        }

        {
            std::unique_lock<std::mutex> lock(m);
            while (!notified) check_income.wait(lock);

            try {
                std::cout << "Shop2 trying to sell" << std::endl;
                shopPtr2->Sell(milk, milkPrice);
                assert(false);
            } catch (const std::exception& err) {
                std::cout << "Exception caught" << std::endl;
            }

            notified = false;
        }
    });

    sales1.join();
    sales2.join();

    return 0;
}
