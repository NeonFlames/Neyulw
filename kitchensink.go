package main

import (
	"os"
	"slices"
	"cmp"
)

func pathExists(path string) bool {
	if _, err := os.Stat(path); err == nil {
		return true
	}
	return false
}

func sortMap[K cmp.Ordered, V any](m map[K]V) map[K]V {
	ret := make(map[K]V, len(m))
	keys := make([]K, 0, len(m))
	for k := range(m) {
		keys = append(keys, k)
	}

	slices.Sort(keys)
	for _, k := range(keys) {
		ret[k] = m[k]
	}
	return ret
}

type NeyulwMap[K cmp.Ordered, V any] struct {
	items map[K]V;
	_keys map[int]K
}

func neyulwMap[K cmp.Ordered, V any](m map[K]V) NeyulwMap[K, V] {
	var ret NeyulwMap[K, V]

	ret._keys = make(map[int]K, len(m))
	ret.items = make(map[K]V, len(m))
	keys := make([]K, 0, len(m))
	for k := range(m) {
		keys = append(keys, k)
	}
	slices.Sort(keys)
	for index, k := range(keys) {
		ret._keys[index] = k
		ret.items[k] = m[k]
	}

	return ret
}

func NewNeyulwMap[K cmp.Ordered, V any]() NeyulwMap[K, V] {
	var ret NeyulwMap[K, V]
	ret._keys = make(map[int]K, 0)
	ret.items = make(map[K]V, 0)
	return ret
}

func (nm NeyulwMap[K, V]) Len() int {
	return len(nm.items)
}

func (nm NeyulwMap[K, V]) Sort() {
	keys := make([]K, 0, len(nm.items))
	for k := range(nm.items) {
		keys = append(keys, k)
	}
	slices.Sort(keys)
	for index, k := range(keys) {
		nm._keys[index] = k
	}
}

func (nm NeyulwMap[K, V]) Add(key K, val V) {
	if _, ok := nm.items[key]; ok {
		nm.items[key] = val
	} else {
		i := len(nm._keys)
		nm._keys[i] = key;
		nm.items[key] = val;
	}
}

func (nm NeyulwMap[K, V]) Get(key K) (V, bool) {
	val, ok := nm.items[key];
	return val, ok
}

func (nm NeyulwMap[K, V]) At(index int) (V, bool) {
	var val V
	key, ok := nm._keys[index]
	if !ok {
		return val, false
	}
	val, ok = nm.items[key]
	return val, ok
}

func (nm NeyulwMap[K, V]) Remove(key K) {
	if _, ok := nm.items[key]; !ok {
		return
	}
	index := 0
	mlen := len(nm.items)
	for index < mlen {
		if nm._keys[index] == key {
			break
		}
	}
	delete(nm._keys, index)
	delete(nm.items, key)
	index++;
	for index < mlen {
		nm._keys[index-1] = nm._keys[index]
		delete(nm._keys, index)
	}
}

func (nm NeyulwMap[K, V]) All(yield func(key K, val V) bool) {
	for k, v := range(nm.items) {
		if !yield(k, v) {
			return
		}
	}
}

func (nm NeyulwMap[K, V]) Ordered(yield func(key K, val V) bool) {
	for i := 0; i < len(nm._keys); i++ {
		k := nm._keys[i]
		v := nm.items[k]
		if !yield(k, v) {
			return
		}
	}
}
