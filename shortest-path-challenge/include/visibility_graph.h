#pragma once
#include "geometry.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
namespace marine_nav 
{
    struct GraphNode 
    {
        Point point;
        int segment_order;  
        bool is_left;       
        GraphNode(const Point& p, int order = -1, bool left = false)
            : point(p), segment_order(order), is_left(left) {}
    };
    
    struct GraphEdge 
    {
        int from_node;
        int to_node;
        double weight;
        GraphEdge(int from, int to, double w) : from_node(from), to_node(to), weight(w) {}
    };

    class VisibilityGraph 
    {
        private:
            std::vector<GraphNode> nodes_;
            std::vector<std::vector<GraphEdge>> adjacency_list_;
            GeometryEngine geometry_engine_;
            bool can_connect_nodes(const GraphNode& from, const GraphNode& to, const std::vector<Segment>& segments) const;
            bool respects_ordering_constraint(const GraphNode& from, const GraphNode& to) const;
            bool respects_orientation_constraint(const GraphNode& from, const GraphNode& to, const std::vector<Segment>& segments) const;
        public:
            VisibilityGraph();
            void build_graph(const std::vector<Segment>& segments, const Point& start, const Point& end);
            const std::vector<std::vector<GraphEdge>>& get_adjacency_list() const 
            {
                return adjacency_list_;
            }
            const GraphNode& get_node(int index) const 
            {
                return nodes_[index];
            }
            size_t get_node_count() const 
            {
                return nodes_.size();
            }
            int find_node_index(const std::string& label) const;
            void print_graph() const;
    };
} 
