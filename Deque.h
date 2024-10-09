#pragma once
#include <iterator>
#include <deque>
#include <type_traits>

template<typename Field = int>
class Deque {
private:
    static const int _blockSz = 256;
    int _head, _tail, _blockCnt;
    Field** _d = nullptr;
    void _reserve(size_t sz);

public:
    Deque(size_t sz = 0, Field elem = Field());
    Deque(const Deque<Field>& d);
    ~Deque();
    void push_back(Field elem);
    void pop_back();
    void push_front(Field elem);
    void pop_front();
    Field& operator[](size_t i);
    const Field& operator[](size_t i) const;
    Field& at(size_t i);
    const Field& at(size_t i) const;
    Field& back();
    const Field& back() const;
    Field& front();
    const Field& front() const;
    bool empty() const;
    size_t size() const;

    template<bool isConst>
    struct common_iterator : public std::iterator<std::random_access_iterator_tag,
                                                  Field,
                                                  std::ptrdiff_t,
                                                  Field*,
                                                  Field&> {
    private:
        int _pos;
        Field** _d;
        static const int _blockSz = Deque<Field>::_blockSz;
        std::conditional_t<isConst, const Field*, Field*> _ptr;

    public:
        common_iterator(Field** d, int pos);

        template<bool isConst2>
        common_iterator(const common_iterator<isConst2>& it);

        int getpos() const;
        Field** getd() const;
        std::conditional_t<isConst, const Field*, Field*> getptr() const;

        bool operator!=(common_iterator<isConst> it);
        bool operator==(common_iterator<isConst> it);
        std::conditional_t<isConst, const Field&, Field&> operator*();
        common_iterator& operator++();
        common_iterator& operator--();
        std::conditional_t<isConst, const Field*, Field*> operator->();
        common_iterator& operator+=(int val);
        common_iterator& operator-=(int val);
    };

    using const_iterator = common_iterator<true>;
    using iterator = common_iterator<false>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    iterator begin();
    iterator begin() const;
    iterator end();
    iterator end() const;
    const_iterator cbegin() const;
    const_iterator cend() const;
    reverse_iterator rbegin();
    reverse_iterator rbegin() const;
    reverse_iterator rend();
    reverse_iterator rend() const;
    const_reverse_iterator crbegin() const;
    const_reverse_iterator crend() const;
};

#include "Deque.cpp"
