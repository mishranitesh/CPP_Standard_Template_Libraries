#ifndef NITESH_MAP_CONTAINER_HPP
#define NITESH_MAP_CONTAINER_HPP

#include <iostream>
#include <utility>
#include <cstdlib>
#include <random>
#include <cstring>

#define MAX_NODE_LEVEL 100
#define PROB_HALF 0.5
#define LOWEST_LEVEL 0

// Macro used in constructor to initialize member variables
#define MEMBER_INIT_CTOR                                                            \
    /* Initialization of size and max level */                                      \
    _num_of_elements = 0;                                                           \
    _map_level = 0;                                                                 \
    /* Memory allocation for new random level generator class object */             \
    _rand_level_gen = new RandomLevelGenerator(PROB_HALF, MAX_NODE_LEVEL);          \
    /* Memory allocation for head and tail nodes of Skip List */                    \
    _head_node = new SkipNode<K, M>(MAX_NODE_LEVEL);                                \
    _tail_node = new SkipNode<K, M>(MAX_NODE_LEVEL);                                \
    /* Initialization direction for head and tail nodes */                          \
    _head_node->_fwd_nodes[LOWEST_LEVEL] = _tail_node;                              \
    _head_node->_prev_node = nullptr;                                               \
    _tail_node->_fwd_nodes[LOWEST_LEVEL] = nullptr;                                 \
    _tail_node->_prev_node = _head_node;

// Macro used in destructor to deallocate heap memory
#define DESTROY_ALLOCATIONS                                                         \
    SkipNode<K, M> *next_node = nullptr, *head_node = _head_node;                   \
    while (head_node != nullptr) {                                                  \
        next_node = head_node->_fwd_nodes[LOWEST_LEVEL];                            \
        delete head_node; /* Calls destructor of SkipNode class */                  \
        head_node = nullptr;                                                        \
        head_node = next_node;                                                      \
    }                                                                               \
    delete _rand_level_gen;


namespace nm {

    /*
     * Implementation of Random Level Generator class
     * Generate new random level according to provided Max Level and Probability
    */
    class RandomLevelGenerator {
    public:
        RandomLevelGenerator() = delete;

        RandomLevelGenerator(float prob, int level) : _prob{prob}, _level{level} {}

        // Function to generate new random level (Using C++11 random generator - number between 0 and RAND_MAX)
        int generate_random_level() {
            int level = 0;
            /*std::default_random_engine generator;
            std::uniform_int_distribution<unsigned long> distribution(0, RAND_MAX);
            while (distribution(generator) < RAND_MAX * _prob && level < _level) {
                ++level;
            }*/
            while (level < _level && rand() < RAND_MAX * _prob) {
                ++level;
            }
            return level;
        }

    private:
        float _prob;
        int _level;
    };

    // Forward declaration of Map class template
    template<typename K, typename M>
    class Map;

    /*
     * Implementation of Skip Node class template
     * Map support bidirectional iterators
    */
    template<typename K, typename M>
    class SkipNode {
    public:
        friend class Map<K, M>;

        typedef std::pair<const K, M> ValueType;

        SkipNode() = delete; // Default ctor
        SkipNode(const SkipNode &) = delete; // Copy ctor
        SkipNode &operator=(const SkipNode &) = delete; // Assignment operator

        SkipNode(int level) : _value{nullptr}, _prev_node{nullptr}, _level_node{level} {
            // Allocate memory for forward nodes and initialize them to point nullptr
            _fwd_nodes = new SkipNode<K, M> *[level + 1];
            for (int i = 0; i <= level; ++i) {
                _fwd_nodes[i] = (SkipNode<K, M> *) nullptr;
            }
        }

        SkipNode(int level, const ValueType &value) : _prev_node{nullptr}, _level_node{level} {
            // Allocate memory for forward nodes and initialize them to point nullptr
            _fwd_nodes = new SkipNode<K, M> *[level + 1];
            for (int i = 0; i <= level; ++i) {
                _fwd_nodes[i] = (SkipNode<K, M> *) nullptr;
            }
            // Use copy constructor of std::pair<const K, M>
            _value = new ValueType(value);
        }

        ~SkipNode() {
            delete[] _fwd_nodes;
            delete _value;
        }

    private:
        ValueType *_value; // Mapped Type or mapped object to represent entire pair
        SkipNode **_fwd_nodes; // Link to forward nodes in the skip list
        SkipNode *_prev_node; // Link to previous node in the skip list
        int _level_node; // Level of each skip node
    };

    /*
     * Implementation of Map Container class template (elements stored as in <key, value> pair) using Skip List data structure
     * Map support bidirectional iterators
    */
    template<typename K, typename M>
    class Map {
    public:
        typedef std::pair<const K, M> ValueType;

        Map() {
            // Using macro to initialize private member variables (Create empty map)
            MEMBER_INIT_CTOR
        }

        Map(const Map &existing_map) {
            if (existing_map._head_node != nullptr) {
                // Using macro to initialize private member variables (Create empty map)
                MEMBER_INIT_CTOR
                // Traverse through the existing map and insert element by element [Complexity of O(nlgn)] --- TO-DO in O(n)
                SkipNode<K, M> *temp_node = existing_map._head_node->_fwd_nodes[LOWEST_LEVEL];
                while (temp_node != existing_map._tail_node) {
                    const ValueType &existing_value = *(temp_node->_value);
                    insert(existing_value);
                    temp_node = temp_node->_fwd_nodes[LOWEST_LEVEL];
                }
            }
        }

        Map(std::initializer_list<std::pair<const K, M>> init_list) {
            // Using macro to initialize private member variables (Create empty map)
            MEMBER_INIT_CTOR
            // Traverse on initializer list and insert elements
            const ValueType *existing_value = init_list.begin();
            while (existing_value != init_list.end()) {
                insert(*existing_value);
                ++existing_value;
            }
        }

        Map &operator=(const Map &existing_map) {
            // Handling self assignment of map objects
            if (this != &existing_map) {
                // Using macro to destroy all memory allocations to clear current Map elements
                DESTROY_ALLOCATIONS
                // Using macro to initialize private member variables (Create empty map)
                MEMBER_INIT_CTOR
                // Traverse through the existing map and insert element by element
                SkipNode<K, M> *temp_node = existing_map._head_node->_fwd_nodes[LOWEST_LEVEL];
                while (temp_node != existing_map._tail_node) {
                    const ValueType &existing_value = *(temp_node->_value);
                    insert(existing_value);
                    temp_node = temp_node->_fwd_nodes[LOWEST_LEVEL];
                }
            }
            return *this;
        }

        ~Map() {
            // Using macro to destroy all memory allocations
            DESTROY_ALLOCATIONS
        }

        /*
         * Implementation of Nested Iterator class
        */
        class Iterator {
        public:
            Iterator() = delete;    // Default ctor
            Iterator(const Iterator &iter) : _iter_ptr{iter.get_iter_ptr()} {} // Copy ctor
            Iterator(SkipNode<K, M> *iter_ptr) : _iter_ptr{iter_ptr} {} // Parameter ctor
            ~Iterator() {} // Destructor

            Iterator &operator=(const Iterator &iter) {
                _iter_ptr = iter.get_iter_ptr();
                return *this;
            }

            SkipNode<K, M> *get_iter_ptr() const {
                return _iter_ptr;
            }

            // Returns a reference to the incremented iterator (preincrement)
            Iterator &operator++() {
                if (_iter_ptr != nullptr) {
                    _iter_ptr = _iter_ptr->_fwd_nodes[LOWEST_LEVEL];
                }
                return *this;
            }

            // Returns an iterator pointing to the element prior to incrementing (postincrement)
            Iterator operator++(int) {
                Map<K, M>::Iterator temp_iter{*this};
                if (_iter_ptr != nullptr) {
                    _iter_ptr = _iter_ptr->_fwd_nodes[LOWEST_LEVEL];
                }
                return temp_iter;
            }

            // Returns a reference to the decremented iterator (predecrement)
            Iterator &operator--() {
                if (_iter_ptr != nullptr) {
                    _iter_ptr = _iter_ptr->_prev_node;
                }
                return *this;
            }

            // Returns an iterator pointing to the element prior to decrementing (postdecrement)
            Iterator operator--(int) {
                Map<K, M>::Iterator temp_iter{*this};
                if (_iter_ptr != nullptr) {
                    _iter_ptr = _iter_ptr->_prev_node;
                }
                return temp_iter;
            }

            // Returns a reference to the ValueType object contained in this element of the list
            ValueType &operator*() const {
                return *_iter_ptr->_value;
            }

            // Special member access operator for the element
            ValueType *operator->() const {
                return _iter_ptr->_value;
            }

        private:
            SkipNode<K, M> *_iter_ptr;
        };

        /*
         * Implementation of Nested ConstIterator class
        */
        class ConstIterator {
        public:
            ConstIterator() = delete;   // Default ctor
            ConstIterator(const ConstIterator &const_iter) : _iter_ptr{const_iter.get_iter_ptr()} {} // Copy ctor
            ConstIterator(const Iterator &const_iter) : _iter_ptr{const_iter.get_iter_ptr()} {} // Conversion ctor
            ConstIterator(SkipNode<K, M> *const_iter_ptr) : _iter_ptr{const_iter_ptr} {} // Parameter ctor
            ~ConstIterator() {} // Destructor

            ConstIterator &operator=(const ConstIterator &const_iter) {
                _iter_ptr = const_iter.get_iter_ptr();
                return *this;
            }

            SkipNode<K, M> *get_iter_ptr() const {
                return _iter_ptr;
            }

            // Returns a reference to the incremented ConstIterator (preincrement)
            ConstIterator &operator++() {
                if (_iter_ptr != nullptr) {
                    _iter_ptr = _iter_ptr->_fwd_nodes[LOWEST_LEVEL];
                }
                return *this;
            }

            // Returns an ConstIterator pointing to the element prior to incrementing (postincrement)
            ConstIterator operator++(int) {
                Map<K, M>::ConstIterator temp_const_iter{*this};
                if (_iter_ptr != nullptr) {
                    _iter_ptr = _iter_ptr->_fwd_nodes[LOWEST_LEVEL];
                }
                return temp_const_iter;
            }

            // Returns a reference to the decremented ConstIterator (predecrement)
            ConstIterator &operator--() {
                if (_iter_ptr != nullptr) {
                    _iter_ptr = _iter_ptr->_prev_node;
                }
                return *this;
            }

            // Returns an ConstIterator pointing to the element prior to decrementing (postdecrement)
            ConstIterator operator--(int) {
                Map<K, M>::ConstIterator temp_const_iter{*this};
                if (_iter_ptr != nullptr) {
                    _iter_ptr = _iter_ptr->_prev_node;
                }
                return temp_const_iter;
            }

            // Returns a const reference to the ValueType object contained in this element of the list
            const ValueType &operator*() const {
                return *_iter_ptr->_value;
            }

            // Special member access operator for the element
            const ValueType *operator->() const {
                return _iter_ptr->_value;
            }

        private:
            SkipNode<K, M> *_iter_ptr;
        };

        /*
         * Implementation of Nested ReverseIterator class
        */
        class ReverseIterator {
        public:
            ReverseIterator() = delete; // Default ctor
            ReverseIterator(const ReverseIterator &rev_iter) : _iter_ptr{rev_iter.get_iter_ptr()} {} // Copy ctor
            ReverseIterator(SkipNode<K, M> *rev_iter_ptr) : _iter_ptr{rev_iter_ptr} {} // Parameter ctor
            ~ReverseIterator() {} // Destructor

            ReverseIterator &operator=(const ReverseIterator &rev_iter) {
                _iter_ptr = rev_iter.get_iter_ptr();
                return *this;
            }

            SkipNode<K, M> *get_iter_ptr() const {
                return _iter_ptr;
            }

            // Returns a reference to the incremented ReverseIterator (preincrement)
            ReverseIterator &operator++() {
                if (_iter_ptr != nullptr) {
                    _iter_ptr = _iter_ptr->_prev_node;
                }
                return *this;
            }

            // Returns an ReverseIterator pointing to the element prior to incrementing (postincrement)
            ReverseIterator operator++(int) {
                Map<K, M>::ReverseIterator temp_rev_iter{*this};
                if (_iter_ptr != nullptr) {
                    _iter_ptr = _iter_ptr->_prev_node;
                }
                return temp_rev_iter;
            }

            // Returns a reference to the decremented ReverseIterator (predecrement)
            ReverseIterator &operator--() {
                if (_iter_ptr != nullptr) {
                    _iter_ptr = _iter_ptr->_fwd_nodes[LOWEST_LEVEL];
                }
                return *this;
            }

            // Returns an ReverseIterator pointing to the element prior to decrementing (postdecrement)
            ReverseIterator operator--(int) {
                Map<K, M>::ReverseIterator temp_rev_iter{*this};
                if (_iter_ptr != nullptr) {
                    _iter_ptr = _iter_ptr->_fwd_nodes[LOWEST_LEVEL];
                }
                return temp_rev_iter;
            }

            // Returns a const reference to the ValueType object contained in this element of the list
            ValueType &operator*() const {
                return *_iter_ptr->_value;
            }

            // Special member access operator for the element
            ValueType *operator->() const {
                return _iter_ptr->_value;
            }

        private:
            SkipNode<K, M> *_iter_ptr;
        };

        // Return number of elements in the map (size of Map)
        size_t size() const {
            return _num_of_elements;
        }

        // Returns true if the Map has no entries in it, false otherwise
        bool empty() const {
            return (_num_of_elements == 0);
        }

        // Returns an Iterator pointing to the first element, in order
        Iterator begin() {
            return Iterator{_head_node->_fwd_nodes[LOWEST_LEVEL]};
        }

        // Returns an Iterator pointing one past the last element, in order
        Iterator end() {
            return Iterator{_tail_node};
        }

        // Returns a ConstIterator pointing to the first element, in order
        ConstIterator begin() const {
            return ConstIterator{_head_node->_fwd_nodes[LOWEST_LEVEL]};
        }

        // Returns a ConstIterator pointing one past the last element, in order
        ConstIterator end() const {
            return ConstIterator{_tail_node};
        }

        // Returns an ReverseIterator to the first element in reverse order
        ReverseIterator rbegin() {
            return ReverseIterator{_tail_node->_prev_node};
        }

        // Returns an ReverseIterator pointing to one past the last element in reverse order
        ReverseIterator rend() {
            return ReverseIterator{_head_node};
        }

        // Returns an iterator to the given key (key is not found, return the end() iterator)
        Iterator find(const K &);

        // Returns an iterator to the given key (key is not found, return the end() iterator)
        ConstIterator find(const K &) const;

        // Returns a reference to the mapped object at the specified key (key is not in the Map, throws std::out_of_range)
        M &at(const K &);

        // Returns a const reference to the mapped object at the specified key (key is not in the map, throws std::out_of_range)
        const M &at(const K &) const;

        // If key is in the map, return a reference to the corresponding mapped object
        // If not, value initialize a mapped object for that key and returns a reference to it
        M &operator[](const K &);

        // Inserts the given pair into the map.
        // If the key does not exist, returns an iterator pointing to the new element and true
        // If the key exists, returns an iterator pointing to the element with the same key and false.
        std::pair<Iterator, bool> insert(const ValueType &);

        // Inserts object or range of objects into the map
        // Given range is half-open and range insert is a member template
        template<typename IT_T>
        void insert(IT_T range_beg, IT_T range_end);

        // Removes the given object indicated by Iterator from the map
        void erase(Iterator pos);

        // Removes the given object indicated by Key from the map
        // Throws std::out_of_range if the key is not in the Map
        void erase(const K &);

        // Removes all elements from the map
        void clear();

        // Compares the given maps for equality (Two maps compare equal if below satisfies)
        // If they have the same number of elements and if all elements compare equal
        template<typename Key_T, typename Mapped_T>
        friend bool operator==(const Map<Key_T, Mapped_T> &, const Map<Key_T, Mapped_T> &);

        // Compares the given maps for inequality
        // Logical complement of the equality operator
        template<typename Key_T, typename Mapped_T>
        friend bool operator!=(const Map<Key_T, Mapped_T> &, const Map<Key_T, Mapped_T> &);

        // Implementation using lexicographic sorting
        // Corresponding elements from each maps must be compared one-by-one
        // Map M1 is less than M2 if there is an element in M1 that is less than
        // the corresponding element in the same position in map M2
        // OR if all corresponding elements in both maps are equal
        template<typename Key_T, typename Mapped_T>
        friend bool operator<(const Map<Key_T, Mapped_T> &, const Map<Key_T, Mapped_T> &);

        // Friend functions to compare Iterators
        friend bool operator==(const Iterator &iter_1, const Iterator &iter_2) {
            return (iter_1.get_iter_ptr() == iter_2.get_iter_ptr());
        }

        friend bool operator!=(const Iterator &iter_1, const Iterator &iter_2) {
            return (iter_1.get_iter_ptr() != iter_2.get_iter_ptr());
        }

        friend bool operator==(const ConstIterator &const_iter_1, const ConstIterator &const_iter_2) {
            return (const_iter_1.get_iter_ptr() == const_iter_2.get_iter_ptr());
        }

        friend bool operator!=(const ConstIterator &const_iter_1, const ConstIterator &const_iter_2) {
            return (const_iter_1.get_iter_ptr() != const_iter_2.get_iter_ptr());
        }

        friend bool operator==(const Iterator &iter, const ConstIterator &const_iter) {
            return (iter.get_iter_ptr() == const_iter.get_iter_ptr());
        }

        friend bool operator!=(const Iterator &iter, const ConstIterator &const_iter) {
            return (iter.get_iter_ptr() != const_iter.get_iter_ptr());
        }

        friend bool operator==(const ConstIterator &const_iter, const Iterator &iter) {
            return (const_iter.get_iter_ptr() == iter.get_iter_ptr());
        }

        friend bool operator!=(const ConstIterator &const_iter, const Iterator &iter) {
            return (const_iter.get_iter_ptr() != iter.get_iter_ptr());
        }

        friend bool operator==(const ReverseIterator &rev_iter_1, const ReverseIterator &rev_iter_2) {
            return (rev_iter_1.get_iter_ptr() == rev_iter_2.get_iter_ptr());
        }

        friend bool operator!=(const ReverseIterator &rev_iter_1, const ReverseIterator &rev_iter_2) {
            return (rev_iter_1.get_iter_ptr() != rev_iter_2.get_iter_ptr());
        }

    private:
        size_t _num_of_elements;    // Represents number of elements in Map
        int _map_level;        // Represents maximum node level present in the Map
        SkipNode<K, M> *_head_node;
        SkipNode<K, M> *_tail_node;
        RandomLevelGenerator *_rand_level_gen;
    };

    /*
     * Function to find the Key in the Map and return the Iterator accordingly
     * Otherwise return end() iterator
     */
    template<typename K, typename M>
    typename Map<K, M>::Iterator Map<K, M>::find(const K &find_key) {

        // Variable declarations and definitions
        SkipNode<K, M> *ret_node, *temp_node = _head_node;

        // Traverse through the skip list to find the correct location to insert the new pair
        for (int lvl = _map_level; lvl >= 0; --lvl) {
            while (temp_node->_fwd_nodes[lvl] != nullptr && temp_node->_fwd_nodes[lvl]->_value != nullptr
                   && temp_node->_fwd_nodes[lvl]->_value->first < find_key) {
                temp_node = temp_node->_fwd_nodes[lvl];
            }
        }
        temp_node = temp_node->_fwd_nodes[LOWEST_LEVEL];

        if (temp_node->_value != nullptr && temp_node->_value->first == find_key) {
            ret_node = temp_node;
        } else {
            ret_node = _tail_node;
        }
        return Map<K, M>::Iterator(ret_node);
    }

    /*
     * Function to find the Key in the Map and return the ConstIterator accordingly
     * Otherwise return end() iterator
     */
    template<typename K, typename M>
    typename Map<K, M>::ConstIterator Map<K, M>::find(const K &find_key) const {

        // Variable declarations and definitions
        SkipNode<K, M> *ret_node, *temp_node = _head_node;

        // Traverse through the skip list to find the correct location to insert the new pair
        for (int lvl = _map_level; lvl >= 0; --lvl) {
            while (temp_node->_fwd_nodes[lvl] != nullptr && temp_node->_fwd_nodes[lvl]->_value != nullptr
                   && temp_node->_fwd_nodes[lvl]->_value->first < find_key) {
                temp_node = temp_node->_fwd_nodes[lvl];
            }
        }
        temp_node = temp_node->_fwd_nodes[LOWEST_LEVEL];

        if (temp_node->_value != nullptr && temp_node->_value->first == find_key) {
            ret_node = temp_node;
        } else {
            ret_node = _tail_node;
        }
        return Map<K, M>::ConstIterator(ret_node);
    }

    /*
     * Returns a reference to the mapped object at the specified key
     * Otherwise throws std::out_of_range
     */
    template<typename K, typename M>
    M &Map<K, M>::at(const K &find_key) {

        // Variable declarations and definitions
        SkipNode<K, M> *temp_node = _head_node;

        // Traverse through the skip list to find the correct location to insert the new pair
        for (int lvl = _map_level; lvl >= 0; --lvl) {
            while (temp_node->_fwd_nodes[lvl] != nullptr && temp_node->_fwd_nodes[lvl]->_value != nullptr
                   && temp_node->_fwd_nodes[lvl]->_value->first < find_key) {
                temp_node = temp_node->_fwd_nodes[lvl];
            }
        }
        temp_node = temp_node->_fwd_nodes[LOWEST_LEVEL];

        if (temp_node->_value != nullptr && temp_node->_value->first == find_key) {
            return temp_node->_value->second;
        } else {
            throw std::out_of_range("Error ---> Key not found!!");
        }
    }

    /*
     * Returns a const reference to the mapped object at the specified key
     * Otherwise throws std::out_of_range
     */
    template<typename K, typename M>
    const M &Map<K, M>::at(const K &find_key) const {
        // Variable declarations and definitions
        SkipNode<K, M> *temp_node = _head_node;

        // Traverse through the skip list to find the correct location to insert the new pair
        for (int lvl = _map_level; lvl >= 0; --lvl) {
            while (temp_node->_fwd_nodes[lvl] != nullptr && temp_node->_fwd_nodes[lvl]->_value != nullptr
                   && temp_node->_fwd_nodes[lvl]->_value->first < find_key) {
                temp_node = temp_node->_fwd_nodes[lvl];
            }
        }
        temp_node = temp_node->_fwd_nodes[LOWEST_LEVEL];

        if (temp_node->_value != nullptr && temp_node->_value->first == find_key) {
            return temp_node->_value->second;
        } else {
            throw std::out_of_range("Error ---> Key not found!!");
        }
    }

    /*
     * If key is in the map, return a reference to the corresponding mapped object
     * Otherwise value initialize a mapped object for that key and returns a reference to it (after insert)
     */
    template<typename K, typename M>
    M &Map<K, M>::operator[](const K &find_key) {

        // Variable declarations and definitions
        SkipNode<K, M> *temp_node = _head_node;

        // Traverse through the skip list to find the correct location to insert the new pair
        for (int lvl = _map_level; lvl >= 0; --lvl) {
            while (temp_node->_fwd_nodes[lvl] != nullptr && temp_node->_fwd_nodes[lvl]->_value != nullptr
                   && temp_node->_fwd_nodes[lvl]->_value->first < find_key) {
                temp_node = temp_node->_fwd_nodes[lvl];
            }
        }
        temp_node = temp_node->_fwd_nodes[LOWEST_LEVEL];

        if (temp_node->_value != nullptr && temp_node->_value->first == find_key) {
            return temp_node->_value->second;
        } else {
            std::pair<typename Map<K, M>::Iterator, bool> new_pair = insert(std::make_pair(find_key, M()));
            Map<K, M>::Iterator new_iter = new_pair.first;
            return new_iter.get_iter_ptr()->_value->second;
        }
    }

    /*
     * Function to implement insert a new pair into Map using skip list data structure
     */
    template<typename K, typename M>
    std::pair<typename Map<K, M>::Iterator, bool> Map<K, M>::insert(const std::pair<const K, M> &new_pair) {

        // Variable declarations and definitions
        int new_level = 0;
        SkipNode<K, M> *temp_node = _head_node;
        SkipNode<K, M> **updated_nodes = new SkipNode<K, M> *[MAX_NODE_LEVEL + 1];
        memset(updated_nodes, '\0', ((MAX_NODE_LEVEL + 1) * (sizeof(SkipNode<K, M> *))));
        const K new_key = new_pair.first;
        bool insert_success;

        // Traverse through the skip list to find the correct location to insert the new pair
        for (int lvl = _map_level; lvl >= 0; --lvl) {
            while (temp_node->_fwd_nodes[lvl] != nullptr && temp_node->_fwd_nodes[lvl]->_value != nullptr
                   && temp_node->_fwd_nodes[lvl]->_value->first < new_key) {
                temp_node = temp_node->_fwd_nodes[lvl];
            }
            updated_nodes[lvl] = temp_node;
        }
        temp_node = temp_node->_fwd_nodes[LOWEST_LEVEL];

        // Handling condition of duplicate keys
        if (temp_node->_value != nullptr && temp_node->_value->first == new_key) {
            insert_success = false;
        } else {
            // Logic to insert new pair if it is not duplicate one
            new_level = _rand_level_gen->generate_random_level();
            if (new_level > _map_level) {
                for (int i = _map_level + 1; i <= new_level; ++i) {
                    updated_nodes[i] = _head_node;
                }
                _map_level = new_level;
            }

            // Logic to manage forward pointers
            temp_node = new SkipNode<K, M>(new_level, new_pair);
            for (int i = 0; i <= new_level; ++i) {
                temp_node->_fwd_nodes[i] = updated_nodes[i]->_fwd_nodes[i];
                updated_nodes[i]->_fwd_nodes[i] = temp_node;
            }

            // Logic to manage previous pointer
            temp_node->_prev_node = updated_nodes[0];
            if (temp_node->_fwd_nodes[LOWEST_LEVEL] != _tail_node) {
                temp_node->_fwd_nodes[LOWEST_LEVEL]->_prev_node = temp_node;
            } else {
                _tail_node->_prev_node = temp_node;
            }

            // Increase the number of elements in the Map
            ++_num_of_elements;
            insert_success = true;
        }

        delete[] updated_nodes;
        Map<K, M>::Iterator new_iter(temp_node);
        return std::make_pair(new_iter, insert_success);
    }

    /*
     * Function to insert pairs from given range of pairs
     */
    template<typename K, typename M>
    template<typename IT_T>
    void Map<K, M>::insert(IT_T range_beg, IT_T range_end) {
        // Traverse through given range and insert element by element
        while (range_beg != range_end) {
            insert(*range_beg);
            ++range_beg;
        }
    }

    /*
     * Function to erase node with the specified Key pointed by Iterator
     * Otherwise throws exception std::out_of_range
     */
    template<typename K, typename M>
    void Map<K, M>::erase(Map<K, M>::Iterator pos) {

        // Variable declarations and definitions
        SkipNode<K, M> *temp_node = _head_node;
        SkipNode<K, M> **updated_nodes = new SkipNode<K, M> *[MAX_NODE_LEVEL + 1];
        memset(updated_nodes, '\0', ((MAX_NODE_LEVEL + 1) * (sizeof(SkipNode<K, M> *))));
        K erase_key = pos.get_iter_ptr()->_value->first;

        // Traverse through the skip list to find the correct location to insert the new pair
        for (int lvl = _map_level; lvl >= 0; --lvl) {
            while (temp_node->_fwd_nodes[lvl] != nullptr && temp_node->_fwd_nodes[lvl]->_value != nullptr
                   && temp_node->_fwd_nodes[lvl]->_value->first < erase_key) {
                temp_node = temp_node->_fwd_nodes[lvl];
            }
            updated_nodes[lvl] = temp_node;
        }
        temp_node = temp_node->_fwd_nodes[LOWEST_LEVEL];

        // Check the condition for same Node which is targeted to erase
        if (pos.get_iter_ptr() == temp_node) {
            // Erase the node if key found in the Map and node is having the same address as passed Node
            if (temp_node->_value != nullptr && temp_node->_value->first == erase_key) {
                // Redirect forward node pointers from using deleting node
                for (int lvl = 0; lvl <= _map_level && updated_nodes[lvl]->_fwd_nodes[lvl] == temp_node; ++lvl) {
                    updated_nodes[lvl]->_fwd_nodes[lvl] = temp_node->_fwd_nodes[lvl];
                }
                // Redirect backward node pointers using the deleting node
                if (temp_node->_fwd_nodes[LOWEST_LEVEL] == _tail_node) {
                    _tail_node->_prev_node = temp_node->_prev_node;
                } else {
                    temp_node->_fwd_nodes[LOWEST_LEVEL]->_prev_node = temp_node->_prev_node;
                }
                // Delete respective node with passed Key
                delete temp_node;
                // Update modified level of Map (skip list)
                while (_map_level > 0 && _head_node->_fwd_nodes[_map_level] == _tail_node) {
                    --_map_level;
                }
                // Reduce the number of elements from the Map
                --_num_of_elements;
                delete[] updated_nodes;
            } else {
                // Throw out_of_range exception if key is not present in the Map
                delete[] updated_nodes;
                throw std::out_of_range("Erase Error ---> Key not found!!");
            }
        } else {
            delete[] updated_nodes;
        }
    }

    /*
     * Function to erase node with the specified Key from the Map
     * Otherwise throws exception std::out_of_range
     */
    template<typename K, typename M>
    void Map<K, M>::erase(const K &erase_key) {

        // Variable declarations and definitions
        SkipNode<K, M> *temp_node = _head_node;
        SkipNode<K, M> **updated_nodes = new SkipNode<K, M> *[MAX_NODE_LEVEL + 1];
        memset(updated_nodes, '\0', ((MAX_NODE_LEVEL + 1) * (sizeof(SkipNode<K, M> *))));

        // Traverse through the skip list to find the correct location to insert the new pair
        for (int lvl = _map_level; lvl >= 0; --lvl) {
            while (temp_node->_fwd_nodes[lvl] != nullptr && temp_node->_fwd_nodes[lvl]->_value != nullptr
                   && temp_node->_fwd_nodes[lvl]->_value->first < erase_key) {
                temp_node = temp_node->_fwd_nodes[lvl];
            }
            updated_nodes[lvl] = temp_node;
        }
        temp_node = temp_node->_fwd_nodes[LOWEST_LEVEL];

        // Erase the node if key found in the Map
        if (temp_node->_value != nullptr && temp_node->_value->first == erase_key) {
            // Redirect forward node pointers from using deleting node
            for (int lvl = 0; lvl <= _map_level && updated_nodes[lvl]->_fwd_nodes[lvl] == temp_node; ++lvl) {
                updated_nodes[lvl]->_fwd_nodes[lvl] = temp_node->_fwd_nodes[lvl];
            }
            // Redirect backward node pointers using the deleting node
            if (temp_node->_fwd_nodes[LOWEST_LEVEL] == _tail_node) {
                _tail_node->_prev_node = temp_node->_prev_node;
            } else {
                temp_node->_fwd_nodes[LOWEST_LEVEL]->_prev_node = temp_node->_prev_node;
            }
            // Delete respective node with passed Key
            delete temp_node;
            // Update modified level of Map (skip list)
            while (_map_level > 0 && _head_node->_fwd_nodes[_map_level] == _tail_node) {
                --_map_level;
            }
            // Reduce the number of elements from the Map
            --_num_of_elements;
            delete[] updated_nodes;
        } else {
            // Throw out_of_range exception if key is not present in the Map
            delete[] updated_nodes;
            throw std::out_of_range("Erase Error ---> Key not found!!");
        }
    }

    /*
     * Function to clear all the elements of Map
     */
    template<typename K, typename M>
    void Map<K, M>::clear() {
        DESTROY_ALLOCATIONS
        MEMBER_INIT_CTOR
    }

    /*
     * Function to check equality of two Maps
     */
    template<typename K, typename M>
    bool operator==(const Map<K, M> &map_1, const Map<K, M> &map_2) {
        bool is_equal = false;
        if (map_1.size() == map_2.size()) {
            // Traverse through both maps and check for every corresponding object types
            //auto map_1_iter, map_2_iter;
            for (auto map_1_iter = map_1.begin(), map_2_iter = map_2.begin();
                 map_1_iter != map_1.end() && map_2_iter != map_2.end();
                 ++map_1_iter, ++map_2_iter) {
                // Compare the elements using dereference
                if (*map_1_iter != *map_2_iter) {
                    return false;
                } else {
                    is_equal = true;
                }
            }
        }
        return is_equal;
    }

    /*
     * Function to check inequality of two Maps
     */
    template<typename K, typename M>
    bool operator!=(const Map<K, M> &map_1, const Map<K, M> &map_2) {
        return !(map_1 == map_2);
    }

    /*
     * Function to compare two Maps
     */
    template<typename K, typename M>
    bool operator<(const Map<K, M> &map_1, const Map<K, M> &map_2) {
        // Traverse through both maps and check for every corresponding object types
        for (auto map_1_iter = map_1.begin(), map_2_iter = map_2.begin();
             map_1_iter != map_1.end() && map_2_iter != map_2.end();
             ++map_1_iter, ++map_2_iter) {
            // Compare the pair elements
            if (*map_1_iter < *map_2_iter) {
                return true;
            } else if (*map_1_iter > *map_2_iter) {
                return false;
            }
        }
        // If all elements are equal and size of M1 is less than M2 then return true otherwise false
        return (map_1.size() < map_2.size());
    }
}

#endif