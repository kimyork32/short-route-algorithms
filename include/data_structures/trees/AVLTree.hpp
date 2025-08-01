#pragma once

#include "../core/Common.hpp"
#include "../core/Traits.hpp"
#include <functional>
#include <memory>
#include <stack>
#include <iterator>

DS_NAMESPACE_BEGIN

template<typename Key, typename Value, typename Compare = std::less<Key>>
class AVLTree {
private:
    struct Node {
        Key key;
        Value value;
        int height;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        
        Node(const Key& k, const Value& v) 
            : key(k), value(v), height(1), left(nullptr), right(nullptr) {}
        
        Node(Key&& k, Value&& v) 
            : key(std::move(k)), value(std::move(v)), height(1), left(nullptr), right(nullptr) {}
    };
    
    using NodePtr = std::unique_ptr<Node>;
    NodePtr root_;
    size_t size_;
    Compare compare_;
    
    // Height helper functions
    int get_height(const NodePtr& node) const {
        return node ? node->height : 0;
    }
    
    void update_height(NodePtr& node) {
        if (node) {
            node->height = 1 + std::max(get_height(node->left), get_height(node->right));
        }
    }
    
    int get_balance(const NodePtr& node) const {
        return node ? get_height(node->left) - get_height(node->right) : 0;
    }
    
    // Rotation operations for balancing
    NodePtr rotate_right(NodePtr&& y) {
        auto x = std::move(y->left);
        y->left = std::move(x->right);
        x->right = std::move(y);
        
        update_height(x->right);
        update_height(x);
        
        return x;
    }
    
    NodePtr rotate_left(NodePtr&& x) {
        auto y = std::move(x->right);
        x->right = std::move(y->left);
        y->left = std::move(x);
        
        update_height(y->left);
        update_height(y);
        
        return y;
    }
    
    // Insertion with AVL balancing
    NodePtr insert_recursive(NodePtr&& node, const Key& key, const Value& value, bool& inserted) {
        // Base case - create new node
        if (!node) {
            inserted = true;
            return std::make_unique<Node>(key, value);
        }
        
        // Standard BST insertion
        if (compare_(key, node->key)) {
            node->left = insert_recursive(std::move(node->left), key, value, inserted);
        } else if (compare_(node->key, key)) {
            node->right = insert_recursive(std::move(node->right), key, value, inserted);
        } else {
            // Key already exists - update value
            node->value = value;
            inserted = false;
            return node;
        }
        
        // Update height
        update_height(node);
        
        // Get balance factor and perform rotations if needed
        int balance = get_balance(node);
        
        // Left Left Case
        if (balance > 1 && compare_(key, node->left->key)) {
            return rotate_right(std::move(node));
        }
        
        // Right Right Case
        if (balance < -1 && compare_(node->right->key, key)) {
            return rotate_left(std::move(node));
        }
        
        // Left Right Case
        if (balance > 1 && compare_(node->left->key, key)) {
            node->left = rotate_left(std::move(node->left));
            return rotate_right(std::move(node));
        }
        
        // Right Left Case
        if (balance < -1 && compare_(key, node->right->key)) {
            node->right = rotate_right(std::move(node->right));
            return rotate_left(std::move(node));
        }
        
        return node;
    }
    
    // Move-based insertion
    NodePtr insert_recursive(NodePtr&& node, Key&& key, Value&& value, bool& inserted) {
        if (!node) {
            inserted = true;
            return std::make_unique<Node>(std::move(key), std::move(value));
        }
        
        if (compare_(key, node->key)) {
            node->left = insert_recursive(std::move(node->left), std::move(key), std::move(value), inserted);
        } else if (compare_(node->key, key)) {
            node->right = insert_recursive(std::move(node->right), std::move(key), std::move(value), inserted);
        } else {
            node->value = std::move(value);
            inserted = false;
            return node;
        }
        
        update_height(node);
        
        int balance = get_balance(node);
        
        if (balance > 1 && compare_(key, node->left->key)) {
            return rotate_right(std::move(node));
        }
        
        if (balance < -1 && compare_(node->right->key, key)) {
            return rotate_left(std::move(node));
        }
        
        if (balance > 1 && compare_(node->left->key, key)) {
            node->left = rotate_left(std::move(node->left));
            return rotate_right(std::move(node));
        }
        
        if (balance < -1 && compare_(key, node->right->key)) {
            node->right = rotate_right(std::move(node->right));
            return rotate_left(std::move(node));
        }
        
        return node;
    }
    
    // Find minimum node in subtree
    Node* find_min(NodePtr& node) {
        while (node->left) {
            node = node->left.get();
        }
        return node.get();
    }
    
    // Deletion with AVL balancing
    NodePtr delete_recursive(NodePtr&& node, const Key& key, bool& deleted) {
        if (!node) {
            deleted = false;
            return nullptr;
        }
        
        if (compare_(key, node->key)) {
            node->left = delete_recursive(std::move(node->left), key, deleted);
        } else if (compare_(node->key, key)) {
            node->right = delete_recursive(std::move(node->right), key, deleted);
        } else {
            // Found node to delete
            deleted = true;
            
            // Case 1: Leaf node or node with only one child
            if (!node->left || !node->right) {
                auto temp = node->left ? std::move(node->left) : std::move(node->right);
                return temp;
            }
            
            // Case 2: Node with two children
            // Find inorder successor (minimum in right subtree)
            Node* successor = find_min(node->right);
            
            // Copy successor's data to this node
            node->key = successor->key;
            node->value = successor->value;
            
            // Delete the successor
            node->right = delete_recursive(std::move(node->right), successor->key, deleted);
        }
        
        // Update height
        update_height(node);
        
        // Get balance factor and perform rotations if needed
        int balance = get_balance(node);
        
        // Left Left Case
        if (balance > 1 && get_balance(node->left) >= 0) {
            return rotate_right(std::move(node));
        }
        
        // Left Right Case
        if (balance > 1 && get_balance(node->left) < 0) {
            node->left = rotate_left(std::move(node->left));
            return rotate_right(std::move(node));
        }
        
        // Right Right Case
        if (balance < -1 && get_balance(node->right) <= 0) {
            return rotate_left(std::move(node));
        }
        
        // Right Left Case
        if (balance < -1 && get_balance(node->right) > 0) {
            node->right = rotate_right(std::move(node->right));
            return rotate_left(std::move(node));
        }
        
        return node;
    }
    
    // Search helper
    Node* find_recursive(const NodePtr& node, const Key& key) const {
        if (!node) return nullptr;
        
        if (compare_(key, node->key)) {
            return find_recursive(node->left, key);
        } else if (compare_(node->key, key)) {
            return find_recursive(node->right, key);
        } else {
            return node.get();
        }
    }
    
    // Deep copy helper for copy constructor
    NodePtr copy_tree(const NodePtr& other) {
        if (!other) return nullptr;
        
        auto new_node = std::make_unique<Node>(other->key, other->value);
        new_node->height = other->height;
        new_node->left = copy_tree(other->left);
        new_node->right = copy_tree(other->right);
        
        return new_node;
    }
    
public:
    // Iterator implementation for pathfinding traversal
    class iterator {
    private:
        std::stack<Node*> stack_;
        Node* current_;
        
        void push_left(Node* node) {
            while (node) {
                stack_.push(node);
                node = node->left.get();
            }
        }
        
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::pair<const Key&, Value&>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type;
        
        iterator() : current_(nullptr) {}
        
        explicit iterator(Node* root) {
            push_left(root);
            if (!stack_.empty()) {
                current_ = stack_.top();
                stack_.pop();
            } else {
                current_ = nullptr;
            }
        }
        
        reference operator*() const {
            return {current_->key, current_->value};
        }
        
        iterator& operator++() {
            if (current_) {
                push_left(current_->right.get());
                if (!stack_.empty()) {
                    current_ = stack_.top();
                    stack_.pop();
                } else {
                    current_ = nullptr;
                }
            }
            return *this;
        }
        
        iterator operator++(int) {
            iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        bool operator==(const iterator& other) const {
            return current_ == other.current_;
        }
        
        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }
    };
    
    class const_iterator {
    private:
        std::stack<const Node*> stack_;
        const Node* current_;
        
        void push_left(const Node* node) {
            while (node) {
                stack_.push(node);
                node = node->left.get();
            }
        }
        
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::pair<const Key&, const Value&>;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = value_type;
        
        const_iterator() : current_(nullptr) {}
        
        explicit const_iterator(const Node* root) {
            push_left(root);
            if (!stack_.empty()) {
                current_ = stack_.top();
                stack_.pop();
            } else {
                current_ = nullptr;
            }
        }
        
        reference operator*() const {
            return {current_->key, current_->value};
        }
        
        const_iterator& operator++() {
            if (current_) {
                push_left(current_->right.get());
                if (!stack_.empty()) {
                    current_ = stack_.top();
                    stack_.pop();
                } else {
                    current_ = nullptr;
                }
            }
            return *this;
        }
        
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        
        bool operator==(const const_iterator& other) const {
            return current_ == other.current_;
        }
        
        bool operator!=(const const_iterator& other) const {
            return !(*this == other);
        }
    };
    
    // Constructors and destructor
    AVLTree() : root_(nullptr), size_(0), compare_(Compare{}) {}
    
    explicit AVLTree(const Compare& comp) : root_(nullptr), size_(0), compare_(comp) {}
    
    // Copy constructor
    AVLTree(const AVLTree& other) : size_(other.size_), compare_(other.compare_) {
        root_ = copy_tree(other.root_);
    }
    
    // Move constructor
    AVLTree(AVLTree&& other) noexcept 
        : root_(std::move(other.root_)), size_(other.size_), compare_(std::move(other.compare_)) {
        other.size_ = 0;
    }
    
    // Copy assignment
    AVLTree& operator=(const AVLTree& other) {
        if (this != &other) {
            root_ = copy_tree(other.root_);
            size_ = other.size_;
            compare_ = other.compare_;
        }
        return *this;
    }
    
    // Move assignment
    AVLTree& operator=(AVLTree&& other) noexcept {
        if (this != &other) {
            root_ = std::move(other.root_);
            size_ = other.size_;
            compare_ = std::move(other.compare_);
            other.size_ = 0;
        }
        return *this;
    }
    
    ~AVLTree() = default;
    
    // Capacity
    bool empty() const noexcept { return size_ == 0; }
    size_t size() const noexcept { return size_; }
    
    // Element access
    Value& operator[](const Key& key) {
        auto* node = find_recursive(root_, key);
        if (node) {
            return node->value;
        }
        
        // Insert new element with default-constructed value
        bool inserted = false;
        root_ = insert_recursive(std::move(root_), key, Value{}, inserted);
        if (inserted) {
            ++size_;
        }
        return find_recursive(root_, key)->value;
    }
    
    Value& at(const Key& key) {
        auto* node = find_recursive(root_, key);
        if (!node) {
            throw std::out_of_range("AVLTree::at: key not found");
        }
        return node->value;
    }
    
    const Value& at(const Key& key) const {
        auto* node = find_recursive(root_, key);
        if (!node) {
            throw std::out_of_range("AVLTree::at: key not found");
        }
        return node->value;
    }
    
    // Modifiers
    std::pair<iterator, bool> insert(const Key& key, const Value& value) {
        bool inserted = false;
        root_ = insert_recursive(std::move(root_), key, value, inserted);
        if (inserted) {
            ++size_;
        }
        return {iterator(find_recursive(root_, key)), inserted};
    }
    
    std::pair<iterator, bool> insert(Key&& key, Value&& value) {
        bool inserted = false;
        auto key_copy = key; // Keep copy for finding after move
        root_ = insert_recursive(std::move(root_), std::move(key), std::move(value), inserted);
        if (inserted) {
            ++size_;
        }
        return {iterator(find_recursive(root_, key_copy)), inserted};
    }
    
    template<typename... Args>
    std::pair<iterator, bool> emplace(const Key& key, Args&&... args) {
        bool inserted = false;
        root_ = insert_recursive(std::move(root_), key, Value(std::forward<Args>(args)...), inserted);
        if (inserted) {
            ++size_;
        }
        return {iterator(find_recursive(root_, key)), inserted};
    }
    
    size_t erase(const Key& key) {
        bool deleted = false;
        root_ = delete_recursive(std::move(root_), key, deleted);
        if (deleted) {
            --size_;
            return 1;
        }
        return 0;
    }
    
    void clear() {
        root_.reset();
        size_ = 0;
    }
    
    // Lookup
    iterator find(const Key& key) {
        auto* node = find_recursive(root_, key);
        return node ? iterator(node) : end();
    }
    
    const_iterator find(const Key& key) const {
        auto* node = find_recursive(root_, key);
        return node ? const_iterator(node) : end();
    }
    
    bool contains(const Key& key) const {
        return find_recursive(root_, key) != nullptr;
    }
    
    size_t count(const Key& key) const {
        return contains(key) ? 1 : 0;
    }
    
    // Iterators
    iterator begin() { return iterator(root_.get()); }
    const_iterator begin() const { return const_iterator(root_.get()); }
    const_iterator cbegin() const { return const_iterator(root_.get()); }
    
    iterator end() { return iterator(); }
    const_iterator end() const { return const_iterator(); }
    const_iterator cend() const { return const_iterator(); }
    
    // Tree-specific operations for pathfinding optimization
    int height() const {
        return get_height(root_);
    }
    
    bool is_balanced() const {
        return std::abs(get_balance(root_)) <= 1;
    }
    
    // Range operations for pathfinding area queries
    template<typename OutputIt>
    void range_query(const Key& lower, const Key& upper, OutputIt out) const {
        range_query_recursive(root_.get(), lower, upper, out);
    }
    
private:
    template<typename OutputIt>
    void range_query_recursive(const Node* node, const Key& lower, const Key& upper, OutputIt& out) const {
        if (!node) return;
        
        // If current key is greater than lower bound, recurse left
        if (compare_(lower, node->key)) {
            range_query_recursive(node->left.get(), lower, upper, out);
        }
        
        // If current key is in range, add it to result
        if (!compare_(node->key, lower) && !compare_(upper, node->key)) {
            *out++ = std::make_pair(node->key, node->value);
        }
        
        // If current key is less than upper bound, recurse right
        if (compare_(node->key, upper)) {
            range_query_recursive(node->right.get(), lower, upper, out);
        }
    }
    
public:
    // Statistics for pathfinding optimization
    struct Statistics {
        size_t node_count;
        int tree_height;
        int max_balance_factor;
        double average_depth;
    };
    
    Statistics get_statistics() const {
        Statistics stats{};
        stats.node_count = size_;
        stats.tree_height = height();
        
        if (empty()) {
            stats.max_balance_factor = 0;
            stats.average_depth = 0.0;
        } else {
            int max_balance = 0;
            double total_depth = 0;
            calculate_stats(root_.get(), 0, max_balance, total_depth);
            stats.max_balance_factor = max_balance;
            stats.average_depth = total_depth / size_;
        }
        
        return stats;
    }
    
private:
    void calculate_stats(const Node* node, int depth, int& max_balance, double& total_depth) const {
        if (!node) return;
        
        total_depth += depth;
        max_balance = std::max(max_balance, std::abs(get_balance(node)));
        
        calculate_stats(node->left.get(), depth + 1, max_balance, total_depth);
        calculate_stats(node->right.get(), depth + 1, max_balance, total_depth);
    }
};

// Pathfinding-optimized type aliases
template<typename T>
using PathfindingAVLMap = AVLTree<Point, T>;

using PathfindingAVLDistanceMap = PathfindingAVLMap<double>;
using PathfindingAVLParentMap = PathfindingAVLMap<Point>;

DS_NAMESPACE_END
