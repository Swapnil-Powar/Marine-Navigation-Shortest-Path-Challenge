#include "shortest_path.h"
#include <iostream>
#include <algorithm>
#include <climits>
namespace marine_nav 
{
    ShortestPathSolver::ShortestPathSolver() {}
    PathResult ShortestPathSolver::solve(const std::vector<Segment>& segments, const Point& start, const Point& end) 
    {
        PathResult result;
        graph_.build_graph(segments, start, end);
        int start_idx = graph_.find_node_index(start.label);
        int end_idx = graph_.find_node_index(end.label);
        if (start_idx == -1 || end_idx == -1) 
        {
            std::cerr << "Error: Could not find start or end node in graph\n";
            return result;
        }
        const auto& adj_list = graph_.get_adjacency_list();
        size_t num_nodes = graph_.get_node_count();
        std::vector<double> distances(num_nodes, std::numeric_limits<double>::infinity());
        std::vector<int> previous(num_nodes, -1);
        std::vector<bool> visited(num_nodes, false);
        std::priority_queue<DijkstraNode, std::vector<DijkstraNode>, std::greater<DijkstraNode>> pq;
        distances[start_idx] = 0.0;
        pq.emplace(start_idx, 0.0);
        while (!pq.empty()) 
        {
            DijkstraNode current = pq.top();
            pq.pop();    
            int u = current.node_index;
            if (visited[u]) 
            {
                continue;
            }
            visited[u] = true;
            if (u == end_idx) 
            {
                break;
            }
            for (const auto& edge : adj_list[u]) 
            {
                int v = edge.to_node;
                double weight = edge.weight;    
                if (!visited[v] && distances[u] + weight < distances[v]) 
                {
                    distances[v] = distances[u] + weight;
                    previous[v] = u;
                    pq.emplace(v, distances[v]);
                }
            }
        }
        if (distances[end_idx] == std::numeric_limits<double>::infinity()) 
        {
            std::cerr << "No path found from start to end\n";
            return result;
        }
        result.path = reconstruct_path(previous, start_idx, end_idx);
        result.total_distance = distances[end_idx];
        result.found = true;
        return result;
    }
    std::vector<Point> ShortestPathSolver::reconstruct_path(const std::vector<int>& previous, int start_idx, int end_idx) const 
    {
        std::vector<Point> path;
        std::vector<int> path_indices;
        int current = end_idx;
        while (current != -1) 
        {
            path_indices.push_back(current);
            current = previous[current];
        }
        std::reverse(path_indices.begin(), path_indices.end());
        for (int idx : path_indices) 
        {
            path.push_back(graph_.get_node(idx).point);
        }
        return path;
    }

    bool ShortestPathSolver::validate_path(const std::vector<Point>& path, const std::vector<Segment>& segments, const Point& start, const Point& end) const 
    {
        if (path.empty()) 
        {
            return false;
        }
        if (path.front().label != start.label || path.back().label != end.label) 
        {
            return false;
        }
        std::vector<int> segment_orders_visited;
        for (const auto& point : path) 
        {
            for (const auto& segment : segments) 
            {
                if (point.label == segment.left.label || point.label == segment.right.label) 
                {
                    segment_orders_visited.push_back(segment.order);
                    break;
                }
            }
        }
        for (size_t i = 1; i < segment_orders_visited.size(); ++i) 
        {
            if (segment_orders_visited[i] < segment_orders_visited[i-1]) 
            {
                return false;
            }
        }
        GeometryEngine geometry_engine;
        for (size_t i = 0; i < path.size() - 1; ++i) 
        {
            if (!geometry_engine.path_maintains_constraints({path[i], path[i+1]}, segments)) 
            {
                return false;
            }
        }   
        return true;
    }
} 
