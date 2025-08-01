#pragma once

#include "../core/Common.hpp"
#include "../core/Traits.hpp"
#include <functional>
#include <memory>
#include <stack>
#include <iterator>

DS_NAMESPACE_BEGIN

template<typename Key, typename Value, typename Compare = std::less<Key>>
class RedBlackTree {
private:
    enum class Color { RED, BLACK };
    
    struct Node {
        Key key;
        Value value;
        Color color;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        Node* parent; // Raw pointer to avoid circular dependency
        
        Node(const Key& k, const Value& v, Color c = Color::RED) 
            : key(k), value(v), color(c), left(nullptr), right(nullptr), parent(nullptr) {}
        
        Node(Key&& k, Value&& v, Color c = Color::RED) 
            : key(std::move(k)), value(std::move(v)), color(c), left(nullptr), right(nullptr), parent(nullptr) {}
    };
    
    using NodePtr = std::unique_ptr<Node>;
    NodePtr root_;
    size_t size_;
    Compare compare_;
    
    // Helper functions for Red-Black Tree properties
    bool is_red(const Node* node) const {
        return node && node->color == Color::RED;
    }
    
    bool is_black(const Node* node) const {
        return !node || node->color == Color::BLACK;
    }
    
    void set_color(Node* node, Color color) {
        if (node) {
            node->color = color;
        }
    }
    
    Node* get_parent(const Node* node) const {
        return node ? node->parent : nullptr;
    }
    
    Node* get_grandparent(const Node* node) const {
        return get_parent(get_parent(node));
    }
    
    Node* get_uncle(const Node* node) const {
        Node* grandparent = get_grandparent(node);
        if (!grandparent) return nullptr;
        
        Node* parent = get_parent(node);
        return (parent == grandparent->left.get()) 
            ? grandparent->right.get() 
            : grandparent->left.get();
    }
    
    // Rotation operations
    void rotate_left(Node* x) {
        Node* y = x->right.get();
        if (!y) return;
        
        // Transfer ownership
        auto y_ptr = std::move(x->right);
        x->right = std::move(y_ptr->left);
        
        // Update parent pointers
        if (x->right) {
            x->right->parent = x;
        }
        
        y_ptr->parent = x->parent;
        
        if (!x->parent) {
            // x was root
            root_ = std::move(y_ptr);
        } else if (x == x->parent->left.get()) {
            x->parent->left = std::move(y_ptr);
        } else {
            x->parent->right = std::move(y_ptr);
        }
        
        // Complete the rotation
        Node* y_raw = root_.get();
        if (x->parent) {
            y_raw = (x->parent->left && x->parent->left.get() != x) 
                ? x->parent->left.get() 
                : x->parent->right.get();
        }
        
        y_raw->left = std::unique_ptr<Node>(x);
        x->parent = y_raw;
    }
    
    void rotate_right(Node* y) {
        Node* x = y->left.get();
        if (!x) return;
        
        // Transfer ownership
        auto x_ptr = std::move(y->left);
        y->left = std::move(x_ptr->right);
        
        // Update parent pointers
        if (y->left) {
            y->left->parent = y;
        }
        
        x_ptr->parent = y->parent;
        
        if (!y->parent) {
            // y was root
            root_ = std::move(x_ptr);
        } else if (y == y->parent->left.get()) {
            y->parent->left = std::move(x_ptr);
        } else {
            y->parent->right = std::move(x_ptr);
        }
        
        // Complete the rotation
        Node* x_raw = root_.get();
        if (y->parent) {
            x_raw = (y->parent->left && y->parent->left.get() != y) 
                ? y->parent->left.get() 
                : y->parent->right.get();
        }
        
        x_raw->right = std::unique_ptr<Node>(y);
        y->parent = x_raw;
    }
    
    // Red-Black Tree insertion fixup
    void insert_fixup(Node* node) {
        while (node && node->parent && is_red(node->parent)) {
            Node* parent = node->parent;
            Node* grandparent = get_grandparent(node);
            
            if (!grandparent) break;
            
            if (parent == grandparent->left.get()) {
                // Parent is left child of grandparent
                Node* uncle = grandparent->right.get();
                
                if (is_red(uncle)) {
                    // Case 1: Uncle is red
                    set_color(parent, Color::BLACK);
                    set_color(uncle, Color::BLACK);
                    set_color(grandparent, Color::RED);
                    node = grandparent;
                } else {
                    if (node == parent->right.get()) {
                        // Case 2: Node is right child
                        node = parent;
                        rotate_left(node);
                        parent = node->parent;
                        grandparent = get_grandparent(node);
                    }
                    
                    // Case 3: Node is left child
                    if (parent && grandparent) {
                        set_color(parent, Color::BLACK);
                        set_color(grandparent, Color::RED);
                        rotate_right(grandparent);
                    }
                }
            } else {
                // Parent is right child of grandparent
                Node* uncle = grandparent->left.get();
                
                if (is_red(uncle)) {
                    // Case 1: Uncle is red
                    set_color(parent, Color::BLACK);
                    set_color(uncle, Color::BLACK);
                    set_color(grandparent, Color::RED);
                    node = grandparent;
                } else {
                    if (node == parent->left.get()) {
                        // Case 2: Node is left child
                        node = parent;
                        rotate_right(node);
                        parent = node->parent;
                        grandparent = get_grandparent(node);
                    }
                    
                    // Case 3: Node is right child
                    if (parent && grandparent) {
                        set_color(parent, Color::BLACK);
                        set_color(grandparent, Color::RED);
                        rotate_left(grandparent);
                    }
                }
            }
        }
        
        // Root must always be black
        set_color(root_.get(), Color::BLACK);
    }
    
    // Standard BST insertion
    std::pair<Node*, bool> insert_bst(const Key& key, const Value& value) {
        if (!root_) {
            root_ = std::make_unique<Node>(key, value, Color::BLACK);
            return {root_.get(), true};
        }
        
        Node* current = root_.get();
        Node* parent = nullptr;
        
        while (current) {
            parent = current;
            if (compare_(key, current->key)) {
                current = current->left.get();
            } else if (compare_(current->key, key)) {
                current = current->right.get();
            } else {
                // Key already exists - update value
                current->value = value;
                return {current, false};
            }
        }
        
        // Create new node
        auto new_node = std::make_unique<Node>(key, value);
        Node* new_node_raw = new_node.get();
        new_node->parent = parent;
        
        if (compare_(key, parent->key)) {
            parent->left = std::move(new_node);
        } else {
            parent->right = std::move(new_node);
        }
        
        return {new_node_raw, true};
    }
    
    // Move-based BST insertion
    std::pair<Node*, bool> insert_bst(Key&& key, Value&& value) {
        if (!root_) {
            root_ = std::make_unique<Node>(std::move(key), std::move(value), Color::BLACK);
            return {root_.get(), true};
        }
        
        Node* current = root_.get();
        Node* parent = nullptr;
        
        while (current) {
            parent = current;
            if (compare_(key, current->key)) {
                current = current->left.get();
            } else if (compare_(current->key, key)) {
                current = current->right.get();
            } else {
                // Key already exists - update value
                current->value = std::move(value);
                return {current, false};
            }
        }
        
        // Create new node
        auto new_node = std::make_unique<Node>(std::move(key), std::move(value));
        Node* new_node_raw = new_node.get();
        new_node->parent = parent;
        
        if (compare_(new_node->key, parent->key)) {
            parent->left = std::move(new_node);
        } else {
            parent->right = std::move(new_node);
        }
        
        return {new_node_raw, true};
    }
    
    // Find node with given key
    Node* find_node(const Key& key) const {
        Node* current = root_.get();
        
        while (current) {
            if (compare_(key, current->key)) {
                current = current->left.get();
            } else if (compare_(current->key, key)) {
                current = current->right.get();
            } else {
                return current;
            }
        }
        
        return nullptr;
    }
    
    // Find minimum node in subtree
    Node* find_min(Node* node) const {
        while (node && node->left) {
            node = node->left.get();
        }
        return node;
    }
    
    // Find maximum node in subtree
    Node* find_max(Node* node) const {
        while (node && node->right) {
            node = node->right.get();
        }
        return node;
    }
    
    // Deep copy helper
    NodePtr copy_tree(const NodePtr& other, Node* parent = nullptr) {
        if (!other) return nullptr;
        
        auto new_node = std::make_unique<Node>(other->key, other->value, other->color);
        new_node->parent = parent;
        new_node->left = copy_tree(other->left, new_node.get());
        new_node->right = copy_tree(other->right, new_node.get());
        
        return new_node;
    }
    
public:
    // Iterator implementation
    class iterator {
    private:
        Node* current_;
        const RedBlackTree* tree_;
        
        Node* find_next(Node* node) const {
            if (node->right) {
                return tree_->find_min(node->right.get());
            }
            
            Node* parent = node->parent;
            while (parent && node == parent->right.get()) {
                node = parent;
                parent = parent->parent;
            }
            return parent;
        }
        
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::pair<const Key&, Value&>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type;
        
        iterator() : current_(nullptr), tree_(nullptr) {}
        iterator(Node* node, const RedBlackTree* tree) : current_(node), tree_(tree) {}
        
        reference operator*() const {
            return {current_->key, current_->value};
        }
        
        iterator& operator++() {
            if (current_) {
                current_ = find_next(current_);
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
        const Node* current_;
        const RedBlackTree* tree_;
        
        const Node* find_next(const Node* node) const {
            if (node->right) {
                return tree_->find_min(node->right.get());
            }
            
            const Node* parent = node->parent;
            while (parent && node == parent->right.get()) {
                node = parent;
                parent = parent->parent;
            }
            return parent;
        }
        
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = std::pair<const Key&, const Value&>;
        using difference_type = std::ptrdiff_t;
        using pointer = const value_type*;
        using reference = value_type;
        
        const_iterator() : current_(nullptr), tree_(nullptr) {}
        const_iterator(const Node* node, const RedBlackTree* tree) : current_(node), tree_(tree) {}
        
        reference operator*() const {
            return {current_->key, current_->value};
        }
        
        const_iterator& operator++() {
            if (current_) {
                current_ = find_next(current_);
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
    RedBlackTree() : root_(nullptr), size_(0), compare_(Compare{}) {}
    
    explicit RedBlackTree(const Compare& comp) : root_(nullptr), size_(0), compare_(comp) {}
    
    // Copy constructor
    RedBlackTree(const RedBlackTree& other) : size_(other.size_), compare_(other.compare_) {
        root_ = copy_tree(other.root_);
    }
    
    // Move constructor
    RedBlackTree(RedBlackTree&& other) noexcept 
        : root_(std::move(other.root_)), size_(other.size_), compare_(std::move(other.compare_)) {
        other.size_ = 0;
    }
    
    // Copy assignment
    RedBlackTree& operator=(const RedBlackTree& other) {
        if (this != &other) {
            root_ = copy_tree(other.root_);
            size_ = other.size_;
            compare_ = other.compare_;
        }
        return *this;
    }
    
    // Move assignment
    RedBlackTree& operator=(RedBlackTree&& other) noexcept {
        if (this != &other) {
            root_ = std::move(other.root_);
            size_ = other.size_;
            compare_ = std::move(other.compare_);
            other.size_ = 0;
        }
        return *this;
    }
    
    ~RedBlackTree() = default;
    
    // Capacity
    bool empty() const noexcept { return size_ == 0; }
    size_t size() const noexcept { return size_; }
    
    // Element access
    Value& operator[](const Key& key) {
        auto* node = find_node(key);
        if (node) {
            return node->value;
        }
        
        // Insert new element with default-constructed value
        auto [new_node, inserted] = insert_bst(key, Value{});
        if (inserted) {
            insert_fixup(new_node);
            ++size_;
        }
        return new_node->value;
    }
    
    Value& at(const Key& key) {
        auto* node = find_node(key);
        if (!node) {
            throw std::out_of_range("RedBlackTree::at: key not found");
        }
        return node->value;
    }
    
    const Value& at(const Key& key) const {
        auto* node = find_node(key);
        if (!node) {
            throw std::out_of_range("RedBlackTree::at: key not found");
        }
        return node->value;
    }
    
    // Modifiers
    std::pair<iterator, bool> insert(const Key& key, const Value& value) {
        auto [node, inserted] = insert_bst(key, value);
        if (inserted) {
            insert_fixup(node);
            ++size_;
        }
        return {iterator(node, this), inserted};
    }
    
    std::pair<iterator, bool> insert(Key&& key, Value&& value) {
        auto [node, inserted] = insert_bst(std::move(key), std::move(value));
        if (inserted) {
            insert_fixup(node);
            ++size_;
        }
        return {iterator(node, this), inserted};
    }
    
    template<typename... Args>
    std::pair<iterator, bool> emplace(const Key& key, Args&&... args) {
        auto [node, inserted] = insert_bst(key, Value(std::forward<Args>(args)...));
        if (inserted) {
            insert_fixup(node);
            ++size_;
        }
        return {iterator(node, this), inserted};
    }
    
    void clear() {
        root_.reset();
        size_ = 0;
    }
    
    // Lookup
    iterator find(const Key& key) {
        auto* node = find_node(key);
        return iterator(node, this);
    }
    
    const_iterator find(const Key& key) const {
        auto* node = find_node(key);
        return const_iterator(node, this);
    }
    
    bool contains(const Key& key) const {
        return find_node(key) != nullptr;
    }
    
    size_t count(const Key& key) const {
        return contains(key) ? 1 : 0;
    }
    
    // Iterators
    iterator begin() { 
        return iterator(empty() ? nullptr : find_min(root_.get()), this); 
    }
    
    const_iterator begin() const { 
        return const_iterator(empty() ? nullptr : find_min(root_.get()), this); 
    }
    
    const_iterator cbegin() const { 
        return const_iterator(empty() ? nullptr : find_min(root_.get()), this); 
    }
    
    iterator end() { return iterator(nullptr, this); }
    const_iterator end() const { return const_iterator(nullptr, this); }
    const_iterator cend() const { return const_iterator(nullptr, this); }
    
    // Tree-specific operations for pathfinding optimization
    int black_height() const {
        return calculate_black_height(root_.get());
    }
    
    bool is_valid_red_black_tree() const {
        return check_red_black_properties(root_.get()) != -1;
    }
    
private:
    int calculate_black_height(const Node* node) const {
        if (!node) return 1; // Null nodes are black
        
        int left_height = calculate_black_height(node->left.get());
        int right_height = calculate_black_height(node->right.get());
        
        if (left_height != right_height) return -1; // Invalid tree
        
        return left_height + (is_black(node) ? 1 : 0);
    }
    
    int check_red_black_properties(const Node* node) const {
        if (!node) return 1; // Null nodes are black
        
        // Property 1: Every node is either red or black (implicit)
        // Property 2: Root is black
        if (node == root_.get() && is_red(node)) return -1;
        
        // Property 3: Red nodes have black children
        if (is_red(node)) {
            if (is_red(node->left.get()) || is_red(node->right.get())) {
                return -1;
            }
        }
        
        // Property 4: All paths have same black height
        int left_height = check_red_black_properties(node->left.get());
        int right_height = check_red_black_properties(node->right.get());
        
        if (left_height == -1 || right_height == -1 || left_height != right_height) {
            return -1;
        }
        
        return left_height + (is_black(node) ? 1 : 0);
    }
    
public:
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
        int black_height;
        size_t red_node_count;
        size_t black_node_count;
        double average_depth;
        bool is_valid;
    };
    
    Statistics get_statistics() const {
        Statistics stats{};
        stats.node_count = size_;
        stats.black_height = black_height();
        stats.is_valid = is_valid_red_black_tree();
        
        if (empty()) {
            stats.red_node_count = 0;
            stats.black_node_count = 0;
            stats.average_depth = 0.0;
        } else {
            size_t red_count = 0, black_count = 0;
            double total_depth = 0;
            calculate_color_stats(root_.get(), 0, red_count, black_count, total_depth);
            stats.red_node_count = red_count;
            stats.black_node_count = black_count;
            stats.average_depth = total_depth / size_;
        }
        
        return stats;
    }
    
private:
    void calculate_color_stats(const Node* node, int depth, size_t& red_count, 
                              size_t& black_count, double& total_depth) const {
        if (!node) return;
        
        total_depth += depth;
        
        if (is_red(node)) {
            ++red_count;
        } else {
            ++black_count;
        }
        
        calculate_color_stats(node->left.get(), depth + 1, red_count, black_count, total_depth);
        calculate_color_stats(node->right.get(), depth + 1, red_count, black_count, total_depth);
    }
};

// Pathfinding-optimized type aliases
template<typename T>
using PathfindingRBMap = RedBlackTree<Point, T>;

using PathfindingRBDistanceMap = PathfindingRBMap<double>;
using PathfindingRBParentMap = PathfindingRBMap<Point>;

DS_NAMESPACE_END
