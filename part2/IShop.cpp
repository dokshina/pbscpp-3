#include <iostream>
#include <string>

#include "IShop.hpp"
#include "IProduct.hpp"

IShop::IShop() : income_(0) {
    id_ = shop_id++;
    state_ = ShopState::Open;
}

IShop::ShopId IShop::GetId() const {
    return id_;
}

IShop::ShopState IShop::GetState() const {
    return state_;
}

const IShop::ProductsMap& IShop::GetProducts() const {
    return products_;
}

IShop::Income IShop::GetIncome() const {
    return income_;
}

void IShop::Attach(IProduct* product) {
    std::lock_guard<std::mutex> lock(m_);

    if (state_ == ShopState::Closed) {
        throw std::logic_error("Wrong state to attach");
    }

    if (products_.find(product->GetId()) == products_.end()) {
        products_.insert({ product->GetId(), product });
    }
}

void IShop::Detach(IProduct* product) {
    std::lock_guard<std::mutex> lock(m_);

    if (state_ == ShopState::Closed) {
        throw std::logic_error("Wrong state to detach");
    }

    if (products_.find(product->GetId()) == products_.end()) {
        throw std::runtime_error("Invalid product");
    }

    products_.erase(product->GetId());
}

void IShop::Close() {
    std::lock_guard<std::mutex> lock(m_);

    if (state_ == ShopState::Closed) {
        throw std::logic_error("Wrong state to close");
    }

    state_ = ShopState::Closed;
    products_.clear();
}

bool IShop::Sell(const IProduct& product, IProduct::Price price) {
    std::lock_guard<std::mutex> lock(m_);

    if (state_ == ShopState::Closed) {
        throw std::logic_error("Shop was closed");
    }

    if (products_.find(product.GetId()) == products_.end()) {
        throw std::runtime_error("Product not found");
    }

    if (product.GetState() != IProduct::ProductState::SalesStarted) {
        return false;
    }

    if (product.GetPrice() != price) {
        return false;
    }

    income_ += price;
    return true;
}

std::string IShop::ToString() const {
    std::string res = "Shop(";
    res += "Id: " + std::to_string(id_) + ", ";

    res += "Products: [";
    for (const auto& product : products_) {
        res += std::to_string(product.first);
        res += ", ";
    }
    if (!products_.empty()) {
        res.pop_back(); res.pop_back();
    }
    res += "]";

    res += ")";
    return res;
}
