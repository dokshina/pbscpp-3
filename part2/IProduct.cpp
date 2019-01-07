#include <iostream>
#include <string>
#include <unordered_map>

#include "IProduct.hpp"
#include "IShop.hpp"


IProduct::IProduct(const Price& price) : price_(price) {
    id_ = product_id++;
    state_ = ProductState::Created;
}

IProduct::~IProduct() {
    if (state_ == IProduct::ProductState::SalesStarted) {
        this->StopSales();
    }
}

IProduct::ProductId IProduct::GetId() const {
    return id_;
}

IProduct::Price IProduct::GetPrice() const {
    return price_;
}

IProduct::ProductState IProduct::GetState() const {
    return state_;
}

void IProduct::ChangePrice(const IProduct::Price& price) {
    price_ = price;
}

void IProduct::Attach(IShop* shop) {
    if (shop == nullptr) return;
    shop->Attach(this);
}

void IProduct::Detach(IShop* shop) {
    if (shop == nullptr) return;
    shop->Detach(this);
}

inline std::string StateToString(IProduct::ProductState state) {
    switch (state) {
        case IProduct::ProductState::Created:        return "Created";
        case IProduct::ProductState::SalesStarted:   return "SalesStarted";
        case IProduct::ProductState::SalesStopped:   return "SalesStopped";
        default:                                     return "[Unknown state]";
    }
}

std::string IProduct::ToString() const {
    std::string res = "Product(";
    res +=  "Id: " + std::to_string(id_) + ", ";
    res += "Price: " + std::to_string(price_) + ", ";
    res += "State: " + StateToString(state_) + ", ";
    res += ")";
    return res;
}

void IProduct::StartSales() {
    std::lock_guard<std::mutex> lock(m_);

    if (state_ != ProductState::Created) {
        throw std::logic_error("Wrong state to start sales");
    }

    state_ = ProductState::SalesStarted;
}

void IProduct::StopSales() {
    std::lock_guard<std::mutex> lock(m_);

    if (state_ != ProductState::SalesStarted) {
        throw std::logic_error("Wrong state to stop sales");
    }

    state_ = ProductState::SalesStopped;
}
