#pragma once
#include <vector>
#include <utility>
#include <optional>
#include <tuple>
#include <iterator>
#include <initializer_list>

namespace aim {

        /** Ordered hash map
         */
        template <typename KeyT, typename ValueT>
        struct hash_map {
                using key_t = KeyT;
                using value_t = ValueT;
                using pair_t = std::tuple<std::size_t, value_t>;
                using hash_fn = std::hash<key_t>;

                using data_t = std::vector<pair_t>;
                using iterator = typename data_t::iterator;
                using const_iterator = typename data_t::const_iterator;

                hash_map()
                {}

                hash_map(std::initializer_list<pair_t> other)
                : data_(other)
                {}

                hash_map(const hash_map&) = default;
                hash_map(hash_map&&) = default;

                iterator begin() {
                        return std::begin(data_);
                }

                const_iterator begin() const {
                        return std::begin(data_);
                }

                iterator end() {
                        return std::end(data_);
                }

                const_iterator end() const {
                        return std::end(data_);
                }

                /** \brief Nonmutable iteration over map using function.
                 * \param func Function object takes type `pair_t` type as argument.
                 */
                template <typename Func>
                void iter(Func func) const {
                        for(const auto& node : data_)
                                func(node);
                }

                /** |brief Find existing element or return past-end element iterator.
                 */
                iterator find(const key_t& key) {
                        const auto hash_ = hash_fn{}(key);
                        auto res_ = _find(hash_);
                        return res_.found ? res_.itr : end();
                }

                /** |brief Find existing element or return past-end element iterator.
                 */
                const_iterator find(const key_t& key) const {
                        return find(key);
                }

                /** \brief Inserts new element to map.
                 * \param key Key to access.
                 * \param value New value.
                 * \return New element was created or reassigned to existing key.
                 */
                bool insert(const key_t& key, const value_t& value) {
                        const auto hash_ = hash_fn{}(key);
                        const auto& own = _find(hash_);
                        if (own.found) {
                                std::get<1>(*own.itr) = value;
                                return false;
                        }
                        else {
                                auto iter = own.itr;
                                std::advance(iter, 1);
                                data_.insert(iter, std::make_tuple(hash_, value));
                                return true;
                        }
                }

                /** \brief Removes element by its key.
                 * \param key
                 * \return True if element was removed and false otherwise.
                 */
                bool remove(const key_t& key) {
                        const auto hash_ = hash_fn{}(key);
                        const auto& own = _find(hash_);
                        if (own.found) {
                                data_.erase(own.itr);
                                return true;
                        }
                        else {
                                return false;
                        }
                }

        private:

                struct search_result {
                        bool found; /// Element was found
                        iterator itr; /// Iterator to previous element
                };

                /** \brief Searches element by its key's hash.
                 * \param hash_ Key's hash.
                 */
                search_result _find(const std::size_t hash_) {
                        auto left = begin();
                        auto right = end();
                        bool searching = true;

                        auto result = search_result{false, right};

                        while (searching) {
                                const std::size_t dist = std::distance(left, right) / 2;

                                auto itr = left;

                                if (dist == 0) {
                                        result.found = false;
                                        result.itr = itr;
                                        break;
                                }

                                std::advance(itr, dist);

                                const auto itr_hash = std::get<0>(*itr);

                                if (itr_hash < hash_) {
                                        left = itr;
                                }
                                else if (itr_hash > hash_) {
                                        right = itr;
                                }
                                else if (itr_hash == hash_) {
                                        result.found = true;
                                        result.itr = itr;
                                        searching = false;
                                }
                                else {
                                        result.found = true;
                                        result.itr = itr;
                                        searching = false;
                                }
                        }
                        return result;
                }

                data_t data_;
        };
}
