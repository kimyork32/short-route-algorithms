#pragma once

#include "../core/Common.hpp"
#include "../core/Hash.hpp"
#include "../hash/HashMap.hpp"
#include "../hash/HashSet.hpp"
#include "../linear/Vector.hpp"
#include "../linear/PriorityQueue.hpp"
#include <algorithm>
#include <functional>
#include <limits>

DS_NAMESPACE_BEGIN

// Forward declarations for pathfinding algorithms
template<typename Vertex, typename Weight = double>
class Graph;

// Edge representation for pathfinding
template<typename Vertex, typename Weight = double>
struct Edge {
    Vertex from;
    Vertex to;
    Weight weight;
    
    Edge() = default;
    Edge(const Vertex& f, const Vertex& t, const Weight& w) 
        : from(f), to(t), weight(w) {}
    Edge(Vertex&& f, Vertex&& t, Weight&& w) 
        : from(std::move(f)), to(std::move(t)), weight(std::move(w)) {}
    
    bool operator==(const Edge& other) const {
        return from == other.from && to == other.to && weight == other.weight;
    }
    
    bool operator<(const Edge& other) const {
        if (from != other.from) return from < other.from;
        if (to != other.to) return to < other.to;
        return weight < other.weight;
    }
};

// Pathfinding result structure
template<typename Vertex, typename Weight = double>
struct PathfindingResult {
    bool path_found;
    Weight total_distance;
    Vector<Vertex> path;
    size_t nodes_explored;
    double computation_time_ms;
    
    PathfindingResult() : path_found(false), total_distance(Weight{}), nodes_explored(0), computation_time_ms(0.0) {}
};

// Graph implementation optimized for pathfinding algorithms
template<typename Vertex, typename Weight>
class Graph {
public:
    using VertexType = Vertex;
    using WeightType = Weight;
    using EdgeType = Edge<Vertex, Weight>;
    using AdjacencyList = Vector<std::pair<Vertex, Weight>>;
    using AdjacencyMap = HashMap<Vertex, AdjacencyList>;
    using VertexSet = HashSet<Vertex>;
    using DistanceMap = HashMap<Vertex, Weight>;
    using ParentMap = HashMap<Vertex, Vertex>;
    
private:
    AdjacencyMap adjacency_list_;
    VertexSet vertices_;
    size_t edge_count_;
    bool is_directed_;
    
    // Heuristic function for A* algorithm
    std::function<Weight(const Vertex&, const Vertex&)> heuristic_;
    
public:
    // Constructors
    Graph(bool directed = false) : edge_count_(0), is_directed_(directed) {
        // Default heuristic (returns 0, making A* behave like Dijkstra)
        heuristic_ = [](const Vertex&, const Vertex&) -> Weight { return Weight{}; };
    }
    
    explicit Graph(size_t expected_vertices, bool directed = false) 
        : edge_count_(0), is_directed_(directed) {
        adjacency_list_.reserve(expected_vertices);
        vertices_.reserve(expected_vertices);
        heuristic_ = [](const Vertex&, const Vertex&) -> Weight { return Weight{}; };
    }
    
    // Copy constructor
    Graph(const Graph& other) 
        : adjacency_list_(other.adjacency_list_), vertices_(other.vertices_), 
          edge_count_(other.edge_count_), is_directed_(other.is_directed_),
          heuristic_(other.heuristic_) {}
    
    // Move constructor
    Graph(Graph&& other) noexcept
        : adjacency_list_(std::move(other.adjacency_list_)), vertices_(std::move(other.vertices_)),
          edge_count_(other.edge_count_), is_directed_(other.is_directed_),
          heuristic_(std::move(other.heuristic_)) {
        other.edge_count_ = 0;
    }
    
    // Assignment operators
    Graph& operator=(const Graph& other) {
        if (this != &other) {
            adjacency_list_ = other.adjacency_list_;
            vertices_ = other.vertices_;
            edge_count_ = other.edge_count_;
            is_directed_ = other.is_directed_;
            heuristic_ = other.heuristic_;
        }
        return *this;
    }
    
    Graph& operator=(Graph&& other) noexcept {
        if (this != &other) {
            adjacency_list_ = std::move(other.adjacency_list_);
            vertices_ = std::move(other.vertices_);
            edge_count_ = other.edge_count_;
            is_directed_ = other.is_directed_;
            heuristic_ = std::move(other.heuristic_);
            other.edge_count_ = 0;
        }
        return *this;
    }
    
    // Capacity and properties
    size_t vertex_count() const { return vertices_.size(); }
    size_t edge_count() const { return edge_count_; }
    bool is_directed() const { return is_directed_; }
    bool empty() const { return vertices_.empty(); }
    
    // Vertex operations
    bool add_vertex(const Vertex& vertex) {
        if (vertices_.contains(vertex)) {
            return false;
        }
        
        vertices_.insert(vertex);
        adjacency_list_[vertex] = AdjacencyList{};
        return true;
    }
    
    bool add_vertex(Vertex&& vertex) {
        if (vertices_.contains(vertex)) {
            return false;
        }
        
        auto vertex_copy = vertex;
        vertices_.insert(std::move(vertex));
        adjacency_list_[vertex_copy] = AdjacencyList{};
        return true;
    }
    
    bool contains_vertex(const Vertex& vertex) const {
        return vertices_.contains(vertex);
    }
    
    bool remove_vertex(const Vertex& vertex) {
        if (!vertices_.contains(vertex)) {
            return false;
        }
        
        // Remove all edges involving this vertex
        auto it = adjacency_list_.find(vertex);
        if (it != adjacency_list_.end()) {
            edge_count_ -= it->second.size();
            adjacency_list_.erase(it);
        }
        
        // Remove edges from other vertices to this vertex
        if (!is_directed_) {
            for (auto& [v, adj_list] : adjacency_list_) {
                auto edge_it = std::find_if(adj_list.begin(), adj_list.end(),
                    [&vertex](const auto& edge) { return edge.first == vertex; });
                if (edge_it != adj_list.end()) {
                    adj_list.erase(edge_it);
                    --edge_count_;
                }
            }
        } else {
            // For directed graphs, we need to check all adjacency lists
            for (auto& [v, adj_list] : adjacency_list_) {
                adj_list.erase(
                    std::remove_if(adj_list.begin(), adj_list.end(),
                        [&vertex](const auto& edge) { return edge.first == vertex; }),
                    adj_list.end());
            }
        }
        
        vertices_.erase(vertex);
        return true;
    }
    
    // Edge operations
    bool add_edge(const Vertex& from, const Vertex& to, const Weight& weight = Weight{}) {
        // Add vertices if they don't exist
        add_vertex(from);
        add_vertex(to);
        
        // Check if edge already exists
        auto it = adjacency_list_.find(from);
        if (it != adjacency_list_.end()) {
            auto& adj_list = it->second;
            auto edge_it = std::find_if(adj_list.begin(), adj_list.end(),
                [&to](const auto& edge) { return edge.first == to; });
            if (edge_it != adj_list.end()) {
                // Update existing edge weight
                edge_it->second = weight;
                return false;
            }
        }
        
        // Add new edge
        adjacency_list_[from].push_back({to, weight});
        ++edge_count_;
        
        // Add reverse edge for undirected graphs
        if (!is_directed_) {
            adjacency_list_[to].push_back({from, weight});
            ++edge_count_;
        }
        
        return true;
    }
    
    bool add_edge(const EdgeType& edge) {
        return add_edge(edge.from, edge.to, edge.weight);
    }
    
    bool remove_edge(const Vertex& from, const Vertex& to) {
        auto it = adjacency_list_.find(from);
        if (it == adjacency_list_.end()) {
            return false;
        }
        
        auto& adj_list = it->second;
        auto edge_it = std::find_if(adj_list.begin(), adj_list.end(),
            [&to](const auto& edge) { return edge.first == to; });
        
        if (edge_it == adj_list.end()) {
            return false;
        }
        
        adj_list.erase(edge_it);
        --edge_count_;
        
        // Remove reverse edge for undirected graphs
        if (!is_directed_) {
            auto rev_it = adjacency_list_.find(to);
            if (rev_it != adjacency_list_.end()) {
                auto& rev_adj_list = rev_it->second;
                auto rev_edge_it = std::find_if(rev_adj_list.begin(), rev_adj_list.end(),
                    [&from](const auto& edge) { return edge.first == from; });
                if (rev_edge_it != rev_adj_list.end()) {
                    rev_adj_list.erase(rev_edge_it);
                    --edge_count_;
                }
            }
        }
        
        return true;
    }
    
    bool contains_edge(const Vertex& from, const Vertex& to) const {
        auto it = adjacency_list_.find(from);
        if (it == adjacency_list_.end()) {
            return false;
        }
        
        const auto& adj_list = it->second;
        return std::find_if(adj_list.begin(), adj_list.end(),
            [&to](const auto& edge) { return edge.first == to; }) != adj_list.end();
    }
    
    Weight get_edge_weight(const Vertex& from, const Vertex& to) const {
        auto it = adjacency_list_.find(from);
        if (it == adjacency_list_.end()) {
            throw std::invalid_argument("Graph::get_edge_weight: from vertex not found");
        }
        
        const auto& adj_list = it->second;
        auto edge_it = std::find_if(adj_list.begin(), adj_list.end(),
            [&to](const auto& edge) { return edge.first == to; });
        
        if (edge_it == adj_list.end()) {
            throw std::invalid_argument("Graph::get_edge_weight: edge not found");
        }
        
        return edge_it->second;
    }
    
    // Adjacency access
    const AdjacencyList& get_neighbors(const Vertex& vertex) const {
        auto it = adjacency_list_.find(vertex);
        if (it == adjacency_list_.end()) {
            throw std::invalid_argument("Graph::get_neighbors: vertex not found");
        }
        return it->second;
    }
    
    AdjacencyList& get_neighbors(const Vertex& vertex) {
        auto it = adjacency_list_.find(vertex);
        if (it == adjacency_list_.end()) {
            throw std::invalid_argument("Graph::get_neighbors: vertex not found");
        }
        return it->second;
    }
    
    // Iterator access to vertices
    auto vertices_begin() const { return vertices_.begin(); }
    auto vertices_end() const { return vertices_.end(); }
    auto vertices_cbegin() const { return vertices_.cbegin(); }
    auto vertices_cend() const { return vertices_.cend(); }
    
    // Heuristic function for A* algorithm
    void set_heuristic(std::function<Weight(const Vertex&, const Vertex&)> heuristic) {
        heuristic_ = std::move(heuristic);
    }
    
    // Dijkstra's shortest path algorithm
    PathfindingResult<Vertex, Weight> dijkstra(const Vertex& start, const Vertex& goal) const {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        PathfindingResult<Vertex, Weight> result;
        
        if (!contains_vertex(start) || !contains_vertex(goal)) {
            return result;
        }
        
        // Initialize data structures
        DistanceMap distances;
        ParentMap parents;
        VertexSet visited;
        PriorityQueue<std::pair<Weight, Vertex>, Vector<std::pair<Weight, Vertex>>, 
                     std::greater<std::pair<Weight, Vertex>>> pq;
        
        // Initialize distances to infinity
        for (const auto& vertex : vertices_) {
            distances[vertex] = std::numeric_limits<Weight>::max();
        }
        distances[start] = Weight{};
        
        pq.push({Weight{}, start});
        
        while (!pq.empty()) {
            auto [current_dist, current_vertex] = pq.top();
            pq.pop();
            
            ++result.nodes_explored;
            
            if (visited.contains(current_vertex)) {
                continue;
            }
            
            visited.insert(current_vertex);
            
            if (current_vertex == goal) {
                result.path_found = true;
                result.total_distance = current_dist;
                break;
            }
            
            // Explore neighbors
            auto it = adjacency_list_.find(current_vertex);
            if (it != adjacency_list_.end()) {
                for (const auto& [neighbor, edge_weight] : it->second) {
                    if (visited.contains(neighbor)) {
                        continue;
                    }
                    
                    Weight new_distance = current_dist + edge_weight;
                    
                    if (new_distance < distances[neighbor]) {
                        distances[neighbor] = new_distance;
                        parents[neighbor] = current_vertex;
                        pq.push({new_distance, neighbor});
                    }
                }
            }
        }
        
        // Reconstruct path if found
        if (result.path_found) {
            Vector<Vertex> path;
            Vertex current = goal;
            
            while (true) {
                path.push_back(current);
                auto parent_it = parents.find(current);
                if (parent_it == parents.end() || parent_it->second == current) {
                    break;
                }
                current = parent_it->second;
            }
            
            std::reverse(path.begin(), path.end());
            result.path = std::move(path);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        result.computation_time_ms = duration.count() / 1000.0;
        
        return result;
    }
    
    // A* shortest path algorithm
    PathfindingResult<Vertex, Weight> a_star(const Vertex& start, const Vertex& goal) const {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        PathfindingResult<Vertex, Weight> result;
        
        if (!contains_vertex(start) || !contains_vertex(goal)) {
            return result;
        }
        
        // Initialize data structures
        DistanceMap g_scores; // Actual distance from start
        DistanceMap f_scores; // g_score + heuristic
        ParentMap parents;
        VertexSet closed_set;
        PriorityQueue<std::pair<Weight, Vertex>, Vector<std::pair<Weight, Vertex>>, 
                     std::greater<std::pair<Weight, Vertex>>> open_queue;
        
        // Initialize scores
        for (const auto& vertex : vertices_) {
            g_scores[vertex] = std::numeric_limits<Weight>::max();
            f_scores[vertex] = std::numeric_limits<Weight>::max();
        }
        
        g_scores[start] = Weight{};
        f_scores[start] = heuristic_(start, goal);
        open_queue.push({f_scores[start], start});
        
        while (!open_queue.empty()) {
            auto [current_f, current_vertex] = open_queue.top();
            open_queue.pop();
            
            ++result.nodes_explored;
            
            if (closed_set.contains(current_vertex)) {
                continue;
            }
            
            closed_set.insert(current_vertex);
            
            if (current_vertex == goal) {
                result.path_found = true;
                result.total_distance = g_scores[current_vertex];
                break;
            }
            
            // Explore neighbors
            auto it = adjacency_list_.find(current_vertex);
            if (it != adjacency_list_.end()) {
                for (const auto& [neighbor, edge_weight] : it->second) {
                    if (closed_set.contains(neighbor)) {
                        continue;
                    }
                    
                    Weight tentative_g_score = g_scores[current_vertex] + edge_weight;
                    
                    if (tentative_g_score < g_scores[neighbor]) {
                        parents[neighbor] = current_vertex;
                        g_scores[neighbor] = tentative_g_score;
                        f_scores[neighbor] = tentative_g_score + heuristic_(neighbor, goal);
                        open_queue.push({f_scores[neighbor], neighbor});
                    }
                }
            }
        }
        
        // Reconstruct path if found
        if (result.path_found) {
            Vector<Vertex> path;
            Vertex current = goal;
            
            while (true) {
                path.push_back(current);
                auto parent_it = parents.find(current);
                if (parent_it == parents.end() || parent_it->second == current) {
                    break;
                }
                current = parent_it->second;
            }
            
            std::reverse(path.begin(), path.end());
            result.path = std::move(path);
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        result.computation_time_ms = duration.count() / 1000.0;
        
        return result;
    }
    
    // Bulk operations for efficient graph construction
    void reserve(size_t vertex_count, size_t edge_count) {
        adjacency_list_.reserve(vertex_count);
        vertices_.reserve(vertex_count);
    }
    
    template<typename VertexIterator>
    void add_vertices(VertexIterator begin, VertexIterator end) {
        for (auto it = begin; it != end; ++it) {
            add_vertex(*it);
        }
    }
    
    template<typename EdgeIterator>
    void add_edges(EdgeIterator begin, EdgeIterator end) {
        for (auto it = begin; it != end; ++it) {
            add_edge(*it);
        }
    }
    
    // Graph analysis operations
    bool is_connected() const {
        if (vertices_.empty()) return true;
        
        VertexSet visited;
        Vector<Vertex> stack;
        
        // Start DFS from first vertex
        auto first_vertex = *vertices_.begin();
        stack.push_back(first_vertex);
        visited.insert(first_vertex);
        
        while (!stack.empty()) {
            Vertex current = stack.back();
            stack.pop_back();
            
            auto it = adjacency_list_.find(current);
            if (it != adjacency_list_.end()) {
                for (const auto& [neighbor, weight] : it->second) {
                    if (!visited.contains(neighbor)) {
                        visited.insert(neighbor);
                        stack.push_back(neighbor);
                    }
                }
            }
        }
        
        return visited.size() == vertices_.size();
    }
    
    size_t connected_components() const {
        VertexSet visited;
        size_t components = 0;
        
        for (const auto& vertex : vertices_) {
            if (!visited.contains(vertex)) {
                // Start new component
                ++components;
                Vector<Vertex> stack;
                stack.push_back(vertex);
                visited.insert(vertex);
                
                while (!stack.empty()) {
                    Vertex current = stack.back();
                    stack.pop_back();
                    
                    auto it = adjacency_list_.find(current);
                    if (it != adjacency_list_.end()) {
                        for (const auto& [neighbor, weight] : it->second) {
                            if (!visited.contains(neighbor)) {
                                visited.insert(neighbor);
                                stack.push_back(neighbor);
                            }
                        }
                    }
                }
            }
        }
        
        return components;
    }
    
    // Clear all data
    void clear() {
        adjacency_list_.clear();
        vertices_.clear();
        edge_count_ = 0;
    }
    
    // Statistics
    struct GraphStatistics {
        size_t vertex_count;
        size_t edge_count;
        double average_degree;
        size_t max_degree;
        size_t min_degree;
        bool is_connected;
        size_t connected_components;
    };
    
    GraphStatistics get_statistics() const {
        GraphStatistics stats{};
        stats.vertex_count = vertex_count();
        stats.edge_count = edge_count();
        stats.is_connected = is_connected();
        stats.connected_components = connected_components();
        
        if (vertex_count() > 0) {
            size_t total_degree = 0;
            stats.max_degree = 0;
            stats.min_degree = std::numeric_limits<size_t>::max();
            
            for (const auto& vertex : vertices_) {
                auto it = adjacency_list_.find(vertex);
                size_t degree = it != adjacency_list_.end() ? it->second.size() : 0;
                
                total_degree += degree;
                stats.max_degree = std::max(stats.max_degree, degree);
                stats.min_degree = std::min(stats.min_degree, degree);
            }
            
            stats.average_degree = static_cast<double>(total_degree) / vertex_count();
        } else {
            stats.average_degree = 0.0;
            stats.max_degree = 0;
            stats.min_degree = 0;
        }
        
        return stats;
    }
};

// Specialized graph types for pathfinding
using PointGraph = Graph<Point, double>;
using WeightedPointGraph = Graph<Point, double>;

// Factory functions for common pathfinding scenarios
inline PointGraph create_grid_graph(int width, int height, bool allow_diagonals = false) {
    PointGraph graph(width * height, false);
    
    // Add vertices
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            graph.add_vertex({x, y});
        }
    }
    
    // Add edges
    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            Point current{x, y};
            
            // 4-connected neighbors
            if (x > 0) graph.add_edge(current, {x-1, y}, 1.0);
            if (x < width-1) graph.add_edge(current, {x+1, y}, 1.0);
            if (y > 0) graph.add_edge(current, {x, y-1}, 1.0);
            if (y < height-1) graph.add_edge(current, {x, y+1}, 1.0);
            
            // Diagonal neighbors (8-connected)
            if (allow_diagonals) {
                const double sqrt2 = 1.4142135623730951;
                if (x > 0 && y > 0) graph.add_edge(current, {x-1, y-1}, sqrt2);
                if (x > 0 && y < height-1) graph.add_edge(current, {x-1, y+1}, sqrt2);
                if (x < width-1 && y > 0) graph.add_edge(current, {x+1, y-1}, sqrt2);
                if (x < width-1 && y < height-1) graph.add_edge(current, {x+1, y+1}, sqrt2);
            }
        }
    }
    
    return graph;
}

DS_NAMESPACE_END
