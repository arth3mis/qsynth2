#pragma once

#include <vector>
#include <functional>
#include <ostream>

template <typename T>
class List : public std::vector<T> {
public:

    List() : std::vector<T>() { }

    List(const List& other) : std::vector<T>(other) { }

    explicit List(size_t size) : std::vector<T>(size) { }

    List(size_t size, T initialValue) : std::vector<T>(size, initialValue) { }


    static List list2D(size_t size0, size_t size1, T initialValue) {
        return List(size0, List(size1, initialValue));
    }

    template<class U>
    List<U> map(std::function<U(T)> mapper) const {
        List<U> copiedList(this->size());

        for (size_t i = 0; i < this->size(); ++i) {
            copiedList[i] = mapper(this->at(i));
        }

        return copiedList;
    }

    void mapMutable(std::function<T(T)> mapper) {
        for (size_t i = 0; i < this->size(); ++i) {
            this->at(i) = mapper(this->at(i));
        }
    }

    static double sumOf(const List<double>& list) {
        double sum = 0;
        for (const double x : list) {
            sum += x;
        }
        return sum;
    }

    static List<List> empty(const List& child) {

    }

    // cout overload
    friend std::ostream& operator<<(std::ostream& os, const List& list) {
        os << "[";
        for (size_t i = 0; i < list.size(); i++) {
            os << list[i];
            if (i < list.size() - 1)
                os << ", ";
        }
        os << "]";
        return os;
    }

};

template <typename T>
List<List<T>> list2D(size_t s0, size_t s1, T initialValue) {
    return List(s0, List(s1, initialValue));
}
