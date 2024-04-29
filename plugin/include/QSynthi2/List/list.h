#pragma once

#include <vector>
#include <functional>

template <typename T>
class List : public std::vector<T> {
public:

    List() : std::vector<T>() { }


    List(List<T> const &other) : std::vector<T>(other) { }

    explicit List(size_t size) : std::vector<T>(size) { }

    List(size_t size, T initialValue) : std::vector<T>(size, initialValue) { }


    template<class U>
    List<U> map(std::function<U(T)> mapper) {
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
        for (double x : list) {
            sum += x;
        }
        return sum;
    }

    static List<List<T>> empty(const List<T>& child) {

    }

};