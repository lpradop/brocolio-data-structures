#pragma once
#include "stack.hpp"
#include "typenames.hpp"
#include <functional>
#include <iostream>
#include <stdexcept>
namespace brocolio::container {
// binary search tree by Brocolio de la CHUNSA
template <class KeyType> class binary_search_tree {
public:
  class iterator;
  binary_search_tree() = default;
  binary_search_tree(binary_search_tree const&); // TODO
  binary_search_tree(binary_search_tree&&);      // TODO
  binary_search_tree(KeyType const root_key);    // REVIEW
  binary_search_tree(
      KeyType const root_key,
      std::function<bool(KeyType const&, KeyType const&)> ordering_function);

  binary_search_tree(
      std::function<bool(KeyType const&, KeyType const&)> ordering_function);

  ~binary_search_tree(); // TODO

  iterator begin() const { return iterator{this, root_}; }
  iterator end() const { return iterator{this, nullptr}; }
  bool insert(KeyType const key);
  bool remove(KeyType const key);
  bool search(KeyType const key) const;
  std::size_t size() const { return size_; };
  bool empty() const { return size_ == 0; };

  void
  print(transversal_method const method = transversal_method::in_order) const;

  KeyType max_key() const;
  KeyType min_key() const;

  KeyType successor(KeyType const key, transversal_method const method =
                                           transversal_method::in_order) const;

  KeyType predecessor(
      KeyType const key,
      transversal_method const method = transversal_method::in_order) const;

private:
  struct node;
  node* root_{nullptr};
  bool remove_flag{true};
  std::function<bool(KeyType const&, KeyType const&)> ordering_function_{
      [](KeyType const a, KeyType const b) { return a < b; }};

  std::size_t size_{0};

  node const* search_node(node const* const root, KeyType const key) const;
  node* search_node(node* const root, KeyType const key);

  node const* successor_node(node const* const given_node,
                             transversal_method const method) const;

  node* successor_node(node* const given_node, transversal_method const method);

  node const* predecessor_node(node const* const given_node,
                               transversal_method const method) const;

  node* predecessor_node(node* const given_node,
                         transversal_method const method);

  node const* min_key_node(node const* const root) const;
  node const* max_key_node(node const* const root) const;
  bool remove_node(node* const given_node);
};

template <class KeyType> struct binary_search_tree<KeyType>::node {
  KeyType key{};
  node* left{nullptr};
  node* right{nullptr};
  node* parent{nullptr};
};

template <class KeyType> class binary_search_tree<KeyType>::iterator {
public:
  iterator() = default;
  iterator(iterator const&) = default;
  iterator(iterator&&) = default;
  iterator(binary_search_tree const* const tree, node const* it_node)
      : tree_(tree), it_node_(it_node) {}
  ~iterator() = default;
  iterator& operator++() {
    it_node_ = tree_->successor_node(it_node_, transversal_method::in_order);
    return *this;
  };
  // iterator operator++(int) {
  //   if (it_node_ != nullptr) {
  //     iterator tmp{*this};
  //     operator++();
  //     return tmp;
  //   } else {
  //     return *this;
  //   }
  // }
  KeyType const& operator*() const { return it_node_->key; }
  bool operator==(iterator const& other) const {
    return (this->it_node_ == other.it_node_);
  }
  bool operator!=(iterator const& other) const { return !(*this == other); }

private:
  node const* it_node_{nullptr};
  binary_search_tree const* const tree_{nullptr};
};

template <class KeyType>
binary_search_tree<KeyType>::binary_search_tree(KeyType const root_key)
    : root_(new node{root_key, nullptr, nullptr, nullptr}) {}

template <class KeyType>
binary_search_tree<KeyType>::binary_search_tree(
    KeyType const root_key,
    std::function<bool(KeyType const&, KeyType const&)> ordering_function)
    : binary_search_tree(root_key) {
  ordering_function_ = ordering_function;
}

template <class KeyType>
binary_search_tree<KeyType>::binary_search_tree(
    std::function<bool(KeyType const&, KeyType const&)> ordering_function)
    : ordering_function_(ordering_function) {}

template <class KeyType> binary_search_tree<KeyType>::~binary_search_tree() {
  // TODO
}

template <class KeyType>
bool binary_search_tree<KeyType>::insert(KeyType const key) {
  if (root_ == nullptr) {
    root_ = new node{key, nullptr, nullptr, nullptr};
    ++size_;
    return true;
  } else {
    node* tmp{root_};
    while (true) {
      if (ordering_function_(key, tmp->key)) {
        if (tmp->left == nullptr) {
          tmp->left = new node{key, nullptr, nullptr, tmp};
          ++size_;
          return true;
        } else {
          tmp = tmp->left;
        }
      } else if (ordering_function_(tmp->key, key)) {
        if (tmp->right == nullptr) {
          tmp->right = new node{key, nullptr, nullptr, tmp};
          ++size_;
          return true;
        } else {
          tmp = tmp->right;
        }
      } else {
        return false;
      }
    }
  }
}

template <class KeyType>
bool binary_search_tree<KeyType>::remove(KeyType const key) {
  if (auto tmp{search_node(root_, key)}; tmp == nullptr) {
    return false;
  } else {
    remove_node(tmp);
    --size_;
    return true;
  }
}

template <class KeyType>
bool binary_search_tree<KeyType>::search(KeyType const key) const {
  if (auto tmp{search_node(root_, key)}; tmp == nullptr) {
    return false;
  } else {
    return true;
  }
}

template <class KeyType>
typename binary_search_tree<KeyType>::node const*
binary_search_tree<KeyType>::search_node(node const* const root,
                                         KeyType const key) const {
  for (node const* tmp{root}; tmp != nullptr;) {
    if (key == tmp->key) {
      return tmp;
    } else if (ordering_function_(key, tmp->key)) {
      tmp = tmp->left;
    } else {
      tmp = tmp->right;
    }
  }
  return nullptr;
}

template <class KeyType>
typename binary_search_tree<KeyType>::node*
binary_search_tree<KeyType>::search_node(node* const root, KeyType const key) {
  return const_cast<node*>(std::as_const(*this).search_node(root, key));
}

template <class KeyType>
void binary_search_tree<KeyType>::print(transversal_method const method) const {
  if (root_ == nullptr) {
    std::cout << "empty tree" << std::endl;
    return;
  }

  stack<node const*> node_stack{};

  switch (method) {
  case transversal_method::pre_order:
    node_stack.push(root_);
    while (!node_stack.empty()) {
      auto current_node{node_stack.top()};

      std::cout << current_node->key << " ";
      node_stack.pop();
      if (current_node->right != nullptr) {
        node_stack.push(current_node->right);
      }
      if (current_node->left != nullptr) {
        node_stack.push(current_node->left);
      }
    }
    break;

  case transversal_method::in_order: {
    node const* current_node{root_};
    while (current_node != nullptr || !node_stack.empty()) {
      while (current_node != nullptr) {
        node_stack.push(current_node);
        current_node = current_node->left;
      }
      current_node = node_stack.pop();
      std::cout << current_node->key << " ";
      current_node = current_node->right;
    }
    break;
  }

  case transversal_method::post_order: {
    node const* previous_node{nullptr};
    node const* current_node{root_};
    while (current_node != nullptr || !node_stack.empty()) {
      if (current_node != nullptr) {
        node_stack.push(current_node);
        current_node = current_node->left;
      } else {
        current_node = node_stack.top();
        if (current_node->right == nullptr ||
            current_node->right == previous_node) {
          std::cout << current_node->key << " ";
          node_stack.pop();
          previous_node = current_node;
          current_node = nullptr;
        } else {
          current_node = current_node->right;
        }
      }
    }
    break;
  }
  }
  std::cout << std::endl;
};

template <class KeyType>
typename binary_search_tree<KeyType>::node const*
binary_search_tree<KeyType>::successor_node(
    node const* const given_node, transversal_method const method) const {
  switch (method) {
  case transversal_method::pre_order:
    if (given_node->left != nullptr) {
      return given_node->left;
    } else if (given_node->right != nullptr) {
      return given_node->right;
    } else {
      node const* tmp{given_node};
      node const* ancestor{given_node->parent};
      while (ancestor != nullptr) {
        if (ancestor->right != nullptr && ancestor->right != tmp) {
          return ancestor->right;
        } else {
          tmp = tmp->parent;
          ancestor = ancestor->parent;
        }
      }
      return nullptr;
    }

  case transversal_method::in_order:
    if (given_node->right != nullptr) {
      return min_key_node(given_node->right);
    } else {
      node const* parent{given_node->parent};
      node const* tmp{given_node};
      while (parent != nullptr && tmp == parent->right) {
        tmp = parent;
        parent = parent->parent;
      }
      return parent;
    }

  case transversal_method::post_order:
    if (given_node == root_) {
      return nullptr;
    } else if (auto parent{given_node->parent}; given_node == parent->right) {
      return parent;
    } else if (parent->right != nullptr) {
      node const* tmp{min_key_node(parent->right)};
      while (tmp->right != nullptr) {
        tmp = min_key_node(tmp->right);
      }
      return tmp;
    } else {
      return parent;
    }
  default:
    return nullptr;
  }
}

template <class KeyType>
typename binary_search_tree<KeyType>::node*
binary_search_tree<KeyType>::successor_node(node* const given_node,
                                            transversal_method const method) {
  return const_cast<node*>(
      std::as_const(*this).successor_node(given_node, method));
}

template <class KeyType>
typename binary_search_tree<KeyType>::node const*
binary_search_tree<KeyType>::predecessor_node(
    node const* const given_node, transversal_method const method) const {
  switch (method) {
  case transversal_method::pre_order:
    if (given_node == root_) {
      return nullptr;
    } else if (auto parent{given_node->parent};
               given_node == parent->left || parent->left == nullptr) {
      return parent;
    } else {
      node const* tmp{parent->left};
      while (tmp->right != nullptr) {
        tmp = tmp->right;
      }
      return tmp;
    }
  case transversal_method::in_order:
    if (given_node->left != nullptr) {
      return max_key_node(given_node->left);
    } else {
      node const* ancestor{root_};
      node const* tmp{nullptr};
      while (ancestor->key != given_node->key) {
        if (ordering_function_(ancestor->key, given_node->key)) {
          tmp = ancestor;
          ancestor = ancestor->right;
        } else {
          ancestor = ancestor->left;
        }
      }
      return tmp;
    }
  case transversal_method::post_order:
    if (given_node->right != nullptr) {
      return given_node->right;
    } else if (given_node->left != nullptr) {
      return given_node->left;
    } else {
      node const* tmp{given_node};
      node const* ancestor{given_node->parent};
      while (ancestor != nullptr) {
        if (ancestor->left != nullptr && ancestor->left != tmp) {
          return ancestor->left;
        } else {
          tmp = tmp->parent;
          ancestor = ancestor->parent;
        }
      }
      return nullptr;
    }
  default:
    return nullptr;
  }
}

template <class KeyType>
typename binary_search_tree<KeyType>::node*
binary_search_tree<KeyType>::predecessor_node(node* const given_node,
                                              transversal_method const method) {
  return const_cast<node*>(
      std::as_const(*this).predecessor_node(given_node, method));
}

template <class KeyType>
typename binary_search_tree<KeyType>::node const*
binary_search_tree<KeyType>::min_key_node(node const* const root) const {
  if (root != nullptr) {
    node const* tmp{root};
    while (tmp->left != nullptr) {
      tmp = tmp->left;
    }
    return tmp;
  } else {
    return nullptr;
  }
}

template <class KeyType>
typename binary_search_tree<KeyType>::node const*
binary_search_tree<KeyType>::max_key_node(node const* const root) const {
  if (root != nullptr) {
    node const* tmp{root};
    while (tmp->right != nullptr) {
      tmp = tmp->right;
    }
    return tmp;
  } else {
    return nullptr;
  }
}

template <class KeyType> KeyType binary_search_tree<KeyType>::min_key() const {
  return min_key_node(root_)->key;
}

template <class KeyType> KeyType binary_search_tree<KeyType>::max_key() const {
  return max_key_node(root_)->key;
}

template <class KeyType>
KeyType
binary_search_tree<KeyType>::successor(KeyType const key,
                                       transversal_method const method) const {
  if (auto tmp{search_node(root_, key)}; tmp != nullptr) {
    auto tmp_successor{successor_node(tmp, method)};
    return tmp_successor != nullptr ? tmp_successor->key : key;
  } else {
    throw std::domain_error{"key not found"};
  }
}

template <class KeyType>
KeyType binary_search_tree<KeyType>::predecessor(
    KeyType const key, transversal_method const method) const {
  if (auto tmp{search_node(root_, key)}; tmp != nullptr) {
    auto tmp_predecessor{predecessor_node(tmp, method)};
    return tmp_predecessor != nullptr ? tmp_predecessor->key : key;
  } else {
    throw std::domain_error{"key not found"};
  }
}

template <class KeyType>
bool binary_search_tree<KeyType>::remove_node(node* const given_node) {
  if (given_node != nullptr) {
    if (given_node->right == nullptr && given_node->left == nullptr) {
      delete given_node;
    } else if (given_node->right != nullptr ^ given_node->left != nullptr) {
      node* child{given_node->left != nullptr ? given_node->left
                                              : given_node->right};
      given_node->key = child->key;
      delete child;
    } else {
      node* tmp{
          remove_flag
              ? successor_node(given_node, transversal_method::in_order)
              : predecessor_node(given_node, transversal_method::in_order)};
      given_node->key = tmp->key;
      remove_node(tmp);
      remove_flag = !remove_flag;
    }
    return true;
  } else {
    return false;
  }
}

} // namespace brocolio::container
