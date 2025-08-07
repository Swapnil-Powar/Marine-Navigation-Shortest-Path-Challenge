#include "visibility_graph.h"
#include <iostream>
#include <algorithm>
namespace marine_nav 
{
    VisibilityGraph::VisibilityGraph() {}
    void VisibilityGraph::build_graph(const std::vector<Segment>& segments, const Point& start, const Point& end) 
    {
        nodes_.clear();
        adjacency_list_.clear();
        nodes_.emplace_back(start, -1, false);
        for (const auto& segment : segments) 
        {
            nodes_.emplace_back(segment.left, segment.order, true);  
            nodes_.emplace_back(segment.right, segment.order, false); 
        }
        nodes_.emplace_back(end, INT_MAX, false);
        adjacency_list_.resize(nodes_.size());
        for (size_t i = 0; i < nodes_.size(); ++i) 
        {
            for (size_t j = i + 1; j < nodes_.size(); ++j) 
            {
                if (can_connect_nodes(nodes_[i], nodes_[j], segments)) 
                {
                    double distance = geometry_engine_.calculate_distance(nodes_[i].point, nodes_[j].point);
                    adjacency_list_[i].emplace_back(i, j, distance);
                    adjacency_list_[j].emplace_back(j, i, distance);
                }
            }
        }
    }

    bool VisibilityGraph::can_connect_nodes(const GraphNode& from, const GraphNode& to, const std::vector<Segment>& segments) const 
    {
        if (!respects_ordering_constraint(from, to)) 
        {
            return false;
        }
        if (!respects_orientation_constraint(from, to, segments)) 
        {
            return false;
        }
        int current_order = std::max(from.segment_order, to.segment_order);
        if (!geometry_engine_.is_visible(from.point, to.point, segments, current_order)) 
        {
            return false;
        }
        return true;
    }

    bool VisibilityGraph::respects_ordering_constraint(const GraphNode& from, const GraphNode& to) const 
    {
        if (from.segment_order > to.segment_order && to.segment_order != INT_MAX) 
        {
            return false;
        }
        if (from.segment_order == -1) 
        {
            return true; 
        }
        if (to.segment_order == INT_MAX) 
        { 
            return true; 
        }
        return true;
    }

    bool VisibilityGraph::respects_orientation_constraint(const GraphNode& from, const GraphNode& to, const std::vector<Segment>& segments) const 
    {
        for (const auto& segment : segments) 
        {
            int max_order = std::max(from.segment_order, to.segment_order);
            if (segment.order < max_order) 
            {
                continue;
            }
            double dx = to.point.x - from.point.x;
            double dy = to.point.y - from.point.y;
            double cross_left = dx * (segment.left.y - from.point.y) - dy * (segment.left.x - from.point.x);
            if (cross_left <= 0) 
            { 
                return false;
            }
            double cross_right = dx * (segment.right.y - from.point.y) - dy * (segment.right.x - from.point.x);
            if (cross_right >= 0) 
            { 
                return false;
            }
        }
        return true;
    }

    int VisibilityGraph::find_node_index(const std::string& label) const 
    {
        for (size_t i = 0; i < nodes_.size(); ++i) 
        {
            if (nodes_[i].point.label == label) 
            {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    void VisibilityGraph::print_graph() const 
    {
        std::cout << "Visibility Graph:\n";
        std::cout << "Nodes (" << nodes_.size() << "):\n";
        for (size_t i = 0; i < nodes_.size(); ++i) 
        {
            const auto& node = nodes_[i];
            std::cout << "  " << i << ": " << node.point.label 
                      << " (" << node.point.x << ", " << node.point.y << ")"
                      << " order=" << node.segment_order
                      << " left=" << (node.is_left ? "true" : "false") << "\n";
        }
        std::cout << "\nEdges:\n";
        for (size_t i = 0; i < adjacency_list_.size(); ++i) 
        {
            for (const auto& edge : adjacency_list_[i]) 
            {
                if (edge.from_node < edge.to_node) 
                { 
                    std::cout << "  " << nodes_[edge.from_node].point.label 
                              << " -> " << nodes_[edge.to_node].point.label
                              << " (weight: " << edge.weight << ")\n";
                }
            }
        }
    }
} 
