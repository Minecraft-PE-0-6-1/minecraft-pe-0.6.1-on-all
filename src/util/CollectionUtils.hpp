#pragma once

struct PairKeyFunctor {
    template <typename T>
    typename T::first_type operator()(T& pair) const {
        return pair.first;
    }
};
struct PairValueFunctor {
    template <typename T>
    typename T::second_type operator()(T& pair) const {
        return pair.second;
    }
};

