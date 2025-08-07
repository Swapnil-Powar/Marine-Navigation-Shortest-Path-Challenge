#pragma once
#include <vector>
#include <string>
#include <memory>
#include <geos_c.h>
namespace marine_nav 
{
    struct Point 
    {
        std::string label;
        double x, y;
        Point(const std::string& label, double x, double y) 
            : label(label), x(x), y(y) {}
        double distance_to(const Point& other) const;
    };

    struct Segment 
    {
        Point left;
        Point right;
        int order; 
            Segment(const Point& left, const Point& right, int order)
            : left(left), right(right), order(order) {}
        bool is_point_on_correct_side(const Point& point, bool should_be_left) const;
        Point get_optimal_crossing_point(const Point& from, const Point& to) const;
    };
    
    class GeometryEngine 
    {
        private:
            GEOSContextHandle_t geos_context_;
        public:
            GeometryEngine();
            ~GeometryEngine();    
            bool line_intersects_obstacles(const Point& from, const Point& to, const std::vector<Segment>& segments) const;
            bool path_maintains_constraints(const std::vector<Point>& path, const std::vector<Segment>& segments) const;
            double calculate_distance(const Point& from, const Point& to) const;  
            bool is_visible(const Point& from, const Point& to, const std::vector<Segment>& segments, int current_segment_order) const;
    };
} 
