#pragma once
#include "visibility_graph.h"
#include <vector>
#include <queue>
#include <limits>
namespace marine_nav 
{
    struct PathResult 
    {
        std::vector<Point> path;
        double total_distance;
        bool found;
        PathResult() : total_distance(std::numeric_limits<double>::infinity()), found(false) {}
    };

    class ShortestPathSolver 
    {
        private:
            struct DijkstraNode 
            {
                int node_index;
                double distance;
                int previous;
                DijkstraNode(int idx, double dist, int prev = -1) 
                    : node_index(idx), distance(dist), previous(prev) {}
                bool operator>(const DijkstraNode& other) const 
                {
                    return distance > other.distance;
                }
            };
            VisibilityGraph graph_;
            std::vector<Point> reconstruct_path(const std::vector<int>& previous, int start_idx, int end_idx) const;
        public:
            ShortestPathSolver();
            PathResult solve(const std::vector<Segment>& segments, const Point& start, const Point& end);
            const VisibilityGraph& get_graph() const { return graph_; }
            bool validate_path(const std::vector<Point>& path, const std::vector<Segment>& segments, const Point& start, const Point& end) const;
    };
} 
