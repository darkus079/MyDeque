#include "Deque.h"

template<typename Field>
void Deque<Field>::_reserve(size_t blockCnt)
{
    if (blockCnt <= _blockCnt) {
        return;
    }

    Field** newd = new Field*[blockCnt];

    for (int i = 0; i < blockCnt; ++i) {
        newd[i] = reinterpret_cast<Field*>(new int8_t[_blockSz * sizeof(Field)]);
    }

    int ind = (blockCnt - _blockCnt) / 2;
    int newHead = _head + _blockSz * ind;
    int newTail = newHead + _tail - _head;
    int i = ind;

    for (int i = 0; i < ind; ++i) {
        newd[i] = reinterpret_cast<Field*>(new int8_t[_blockSz * sizeof(Field)]);
    }

    for (int i = ind + _blockCnt; i < blockCnt; ++i) {
        newd[i] = reinterpret_cast<Field*>(new int8_t[_blockSz * sizeof(Field)]);
    }

    try {
        for (; i < ind + _blockCnt; ++i) {
            int oldInd = i - ind;
            newd[i] = _d[oldInd];
        }
    }
    catch (...) {
        for (int j = newHead; j < i; ++j) {
            reinterpret_cast<Field*>((*(newd + j / _blockSz) + j % _blockSz))->~Field();
        }

        for (int j = 0; j < blockCnt; ++j) {
            delete[] reinterpret_cast<int8_t*>(newd[j]);
        }

        delete[] newd;
        throw;
    }

    if (_d != nullptr) { 
        delete[] _d;
    }

    _head = newHead;
    _tail = newTail;
    _d = newd;
    _blockCnt = blockCnt;
}

template<typename Field>
Deque<Field>::Deque(size_t sz,Field elem)
{
    _blockCnt = 1;

    while (_blockCnt * _blockSz <= sz) {
        _blockCnt *= 2;
    }

    _d = new Field*[_blockCnt];

    for (int i = 0; i < _blockCnt; ++i) {
        _d[i] = reinterpret_cast<Field*>(new int8_t[_blockSz * sizeof(Field)]);
    }

    _head = (_blockCnt * _blockSz - sz) / 2;
    _tail = _head + sz;

    int i = _head;

    try {
        for (; i < _tail; ++i) {
            new(*(_d + i / _blockSz) + i % _blockSz) Field(elem);
        }
    }
    catch (...) {
        for (int j = _head; j < i; ++j) {
            reinterpret_cast<Field*>((*(_d + j / _blockSz) + j % _blockSz))->~Field();
        }
        
        for (int j = 0; j < _blockCnt; ++j) {
            delete[] reinterpret_cast<int8_t*>(_d[j]);
        }

        delete[] _d;
        throw;
    }
}

template<typename Field>
Deque<Field>::Deque(const Deque<Field>& d)
{
    _blockCnt = 0;
    _reserve(d._blockCnt);
    _head = d._head;
    _tail = d._tail;

    int i = _head;

    try {
        for (; i < _tail; ++i) {
            new(*(_d + i / _blockSz) + i % _blockSz) Field(d._d[i / _blockSz][i % _blockSz]);
        }
    }
    catch (...) {
        for (int j = _head; j < i; ++j) {
            reinterpret_cast<Field*>((*(_d + j / _blockSz) + j % _blockSz))->~Field();
        }
        
        for (int j = 0; j < _blockCnt; ++j) {
            delete[] reinterpret_cast<int8_t*>(_d[j]);
        }

        delete[] _d;
    }
}

template<typename Field>
Deque<Field>::~Deque()
{
    for (int i = _head; i < _tail; ++i) {
        reinterpret_cast<Field*>((*(_d + i / _blockSz) + i % _blockSz))->~Field();
    }

    for (int i = 0; i < _blockCnt; ++i) {
        delete[] reinterpret_cast<int8_t*>(_d[i]);
    }

    if (_d != nullptr) {
        delete[] _d;
    }
}

template<typename Field>
void Deque<Field>::push_back(Field elem)
{
    if (_tail >= _blockCnt * _blockSz) {
        _reserve(5 * _blockCnt);
    }

    new(*(_d + _tail / _blockSz) + _tail % _blockSz) Field(elem);
    ++_tail;
}

template<typename Field>
void Deque<Field>::pop_back()
{
    int ind = _tail - 1;
    reinterpret_cast<Field*>((*(_d + ind / _blockSz) + ind % _blockSz))->~Field();
    --_tail;
}

template<typename Field>
void Deque<Field>::push_front(Field elem)
{
    if (_head <= 0) {
        _reserve(5 * _blockCnt);
    }

    int ind = _head - 1;
    new(*(_d + ind / _blockSz) + ind % _blockSz) Field(elem);
    --_head;
}

template<typename Field>
void Deque<Field>::pop_front()
{
    reinterpret_cast<Field*>((*(_d + _head / _blockSz) + _head % _blockSz))->~Field();
    ++_head;
}

template<typename Field>
Field& Deque<Field>::operator[](size_t i)
{
    int ind = _head + i;
    return _d[ind / _blockSz][ind % _blockSz];
}

template<typename Field>
const Field& Deque<Field>::operator[](size_t i) const
{
    int ind = _head + i;
    return _d[ind / _blockSz][ind % _blockSz];
}

template<typename Field>
Field& Deque<Field>::at(size_t i)
{
    int ind = _head + i;

    if (ind >= _tail) {
        throw std::out_of_range("Index out of range!");
    }

    return _d[ind / _blockSz][ind % _blockSz];
}

template<typename Field>
const Field& Deque<Field>::at(size_t i) const
{
    int ind = _head + i;

    if (ind >= _tail) {
        throw std::out_of_range("Index out of range!");
    }

    return _d[ind / _blockSz][ind % _blockSz];
}

template<typename Field>
Field& Deque<Field>::back()
{
    int tmp = _tail - 1;
    return _d[tmp / _blockSz][tmp % _blockSz];
}

template<typename Field>
const Field& Deque<Field>::back() const
{
    int tmp = _tail - 1;
    return _d[tmp / _blockSz][tmp % _blockSz];
}

template<typename Field>
Field& Deque<Field>::front()
{
    return _d[_head / _blockSz][_head % _blockSz];
}

template<typename Field>
const Field& Deque<Field>::front() const
{
    return _d[_head / _blockSz][_head % _blockSz];
}

template<typename Field>
bool Deque<Field>::empty() const
{
    return (_head == _tail);
}

template<typename Field>
size_t Deque<Field>::size() const
{
    return static_cast<size_t>(_tail - _head);
}

template<typename Field>
template<bool isConst>
Deque<Field>::common_iterator<isConst>::common_iterator(Field** d, int pos) : _d(d), _pos(pos)
{
    _ptr = &(_d[_pos / _blockSz][_pos % _blockSz]);
}

template<typename Field>
template<bool isConst>
template<bool isConst2>
Deque<Field>::common_iterator<isConst>::common_iterator(const common_iterator<isConst2>& it) {
    _ptr = reinterpret_cast<std::conditional_t<isConst, const Field*, Field*>>(it.getptr());
    _pos = it.getpos();
    _d = it.getd();
}

template<typename Field>
template<bool isConst>
int Deque<Field>::common_iterator<isConst>::getpos() const {
    return _pos;
}

template<typename Field>
template<bool isConst>
Field** Deque<Field>::common_iterator<isConst>::getd() const {
    return _d;
}

template<typename Field>
template<bool isConst>
std::conditional_t<isConst, const Field*, Field*> Deque<Field>::common_iterator<isConst>::getptr() const {
    return _ptr;
}

template<typename Field>
template<bool isConst>
bool Deque<Field>::common_iterator<isConst>::operator!=(common_iterator<isConst> it)
{
    return it._pos != _pos;
}

template<typename Field>
template<bool isConst>
bool Deque<Field>::common_iterator<isConst>::operator==(common_iterator<isConst> it)
{
    return it._pos == _pos;
}

template<typename Field>
template<bool isConst>
std::conditional_t<isConst,const Field&,Field&> Deque<Field>::common_iterator<isConst>::operator*()
{
    return (std::conditional_t<isConst,const Field&,Field&>)(*_ptr);
}

template<typename Field>
template<bool isConst>
Deque<Field>::common_iterator<isConst>& Deque<Field>::common_iterator<isConst>::operator++()
{
    ++_pos;
    _ptr = &(_d[_pos / _blockSz][_pos % _blockSz]);
    return (*this);
}

template<typename Field>
template<bool isConst>
Deque<Field>::common_iterator<isConst>& Deque<Field>::common_iterator<isConst>::operator--()
{
    --_pos;
    _ptr = &(_d[_pos / _blockSz][_pos % _blockSz]);
    return (*this);
}

template<typename Field>
template<bool isConst>
std::conditional_t<isConst,const Field*,Field*> Deque<Field>::common_iterator<isConst>::operator->()
{
    return (std::conditional_t<isConst,const Field*,Field*>)(_ptr);
}

template<typename Field>
template<bool isConst>
Deque<Field>::common_iterator<isConst>& Deque<Field>::common_iterator<isConst>::operator+=(int val)
{
    _pos += val;
    _ptr = &(_d[_pos / _blockSz][_pos % _blockSz]);
    return (*this);
}

template<typename Field>
template<bool isConst>
Deque<Field>::common_iterator<isConst>& Deque<Field>::common_iterator<isConst>::operator-=(int val)
{
    _pos -= val;
    _ptr = &(_d[_pos / _blockSz][_pos % _blockSz]);
    return (*this);
}

template<typename Field>
typename Deque<Field>::iterator Deque<Field>::begin()
{
    return iterator(_d, _head);
}

template<typename Field>
typename Deque<Field>::iterator Deque<Field>::begin() const
{
    return iterator(_d, _head);
}

template<typename Field>
typename Deque<Field>::iterator Deque<Field>::end()
{
    return iterator(_d, _tail);
}

template<typename Field>
typename Deque<Field>::iterator Deque<Field>::end() const
{
    return iterator(_d, _tail);
}

template<typename Field>
typename Deque<Field>::const_iterator Deque<Field>::cbegin() const
{
    return const_iterator(_d, _head);
}

template<typename Field>
typename Deque<Field>::const_iterator Deque<Field>::cend() const
{
    return const_iterator(_d, _tail);
}

template<typename Field>
typename Deque<Field>::reverse_iterator Deque<Field>::rbegin()
{
    return reverse_iterator(iterator(_d, _tail));
}

template<typename Field>
typename Deque<Field>::reverse_iterator Deque<Field>::rbegin() const
{
    return reverse_iterator(iterator(_d, _tail));
}

template<typename Field>
typename Deque<Field>::reverse_iterator Deque<Field>::rend()
{
    return reverse_iterator(iterator(_d, _head));
}

template<typename Field>
typename Deque<Field>::reverse_iterator Deque<Field>::rend() const
{
    return reverse_iterator(iterator(_d, _head));
}

template<typename Field>
typename Deque<Field>::const_reverse_iterator Deque<Field>::crbegin() const
{
    return const_reverse_iterator(iterator(_d, _tail));
}

template<typename Field>
typename Deque<Field>::const_reverse_iterator Deque<Field>::crend() const
{
    return const_reverse_iterator(iterator(_d, _head));
}
