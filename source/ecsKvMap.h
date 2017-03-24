/*
 * Copyright (c) 2016 Galen Cochrane
 * Galen Cochrane <galencochrane@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/*
 * Wrapper class for whatever implementation of a hash map I wish to use.
 * Right now, I'm using std::unordered_map from C++ standard library.
 */

#ifndef HASHMAP_H
#define  HASHMAP_H

#include <unordered_map>

namespace ecs {

  template<class K, class V>
  class KvMap {
    private:
      std::unordered_map<K, V> internalMap;

    public:
      KvMap();
      ~KvMap();
      V &at(const K &key);
      V& operator [] (const K& key);
      V& operator [] (K&& key);
      void clear() noexcept;
      bool contains(const K &key) const;
      template<class... Args>
      bool emplace(Args &&... args);
      bool erase(const K &key);
      void reserve(std::size_t n);
      size_t count(const K &key) const;
      typedef typename std::unordered_map<K, V>::const_iterator const_iterator;
      const_iterator begin() const;// { return internalMap.begin(); }
      const_iterator end() const;// { return internalMap.end(); }
  };

  template<class K, class V>
  KvMap<K, V>::KvMap() { }
  template<class K, class V>
  KvMap<K, V>::~KvMap() { }
  template<class K, class V>
  V &KvMap<K, V>::at(const K &key) {
    return internalMap.at(key);
  }
  template<class K, class V>
  V& KvMap<K, V>::operator [] (const K& key) {
    return internalMap[key];
  };
  template<class K, class V>
  V& KvMap<K, V>::operator [] (K&& key) {
    return internalMap[key];
  };
  template<class K, class V>
  void KvMap<K, V>::clear() noexcept {
    internalMap.clear();
  }
  template<class K, class V>
  bool KvMap<K, V>::contains(const K &key) const {
    return (bool) internalMap.count(key);
  }
  template<class K, class V>
  template<class... Args>
  bool KvMap<K, V>::emplace(Args &&... args) {
    return internalMap.emplace(std::forward<Args>(args)...).second;
  }
  template<class K, class V>
  bool KvMap<K, V>::erase(const K &key) {
    return (bool) internalMap.erase(key);
  }
  template<class K, class V>
  void KvMap<K, V>::reserve(std::size_t n) {
    internalMap.reserve(n);
  }
  template<class K, class V>
  size_t KvMap<K, V>::count(const K &key) const {
    return internalMap.count(key);
  };
  template<class K, class V>
  typename KvMap<K, V>::const_iterator KvMap<K, V>::begin() const {
    return internalMap.begin();
  }
  template<class K, class V>
  typename KvMap<K, V>::const_iterator KvMap<K, V>::end() const {
    return internalMap.end();
  }
}

#endif	/* HASHMAP_H */

