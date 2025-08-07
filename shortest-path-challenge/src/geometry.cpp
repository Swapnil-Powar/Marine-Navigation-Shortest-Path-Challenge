#include "geometry.h"
#include <cmath>
#include <iostream>
namespace marine_nav 
{
    double Point::distance_to(const Point& other) const 
    {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    bool Segment::is_point_on_correct_side(const Point& point, bool should_be_left) const 
    {
        double cross = (right.x - left.x) * (point.y - left.y) - (right.y - left.y) * (point.x - left.x);
        bool is_on_left = cross > 0;
        return should_be_left ? is_on_left : !is_on_left;
    }

    Point Segment::get_optimal_crossing_point(const Point& from, const Point& to) const 
    {
        double mid_x = (left.x + right.x) / 2.0;
        double mid_y = (left.y + right.y) / 2.0;
        return Point("crossing_" + std::to_string(order), mid_x, mid_y);
    }

    GeometryEngine::GeometryEngine() 
    {
        geos_context_ = GEOS_init_r();
        if (!geos_context_) 
        {
            throw std::runtime_error("Failed to initialize GEOS context");
        }
    }

    GeometryEngine::~GeometryEngine() 
    {
        if (geos_context_) 
        {
            GEOS_finish_r(geos_context_);
        }
    }

    bool GeometryEngine::line_intersects_obstacles(const Point& from, const Point& to, const std::vector<Segment>& segments) const 
    {
        GEOSCoordSequence* coord_seq = GEOSCoordSeq_create_r(geos_context_, 2, 2);
        GEOSCoordSeq_setX_r(geos_context_, coord_seq, 0, from.x);
        GEOSCoordSeq_setY_r(geos_context_, coord_seq, 0, from.y);
        GEOSCoordSeq_setX_r(geos_context_, coord_seq, 1, to.x);
        GEOSCoordSeq_setY_r(geos_context_, coord_seq, 1, to.y);
        GEOSGeometry* line = GEOSGeom_createLineString_r(geos_context_, coord_seq);
        bool intersects = false;
        for (const auto& segment : segments) 
        {
            GEOSCoordSequence* seg_coord_seq = GEOSCoordSeq_create_r(geos_context_, 2, 2);
            GEOSCoordSeq_setX_r(geos_context_, seg_coord_seq, 0, segment.left.x);
            GEOSCoordSeq_setY_r(geos_context_, seg_coord_seq, 0, segment.left.y);
            GEOSCoordSeq_setX_r(geos_context_, seg_coord_seq, 1, segment.right.x);
            GEOSCoordSeq_setY_r(geos_context_, seg_coord_seq, 1, segment.right.y);
            GEOSGeometry* seg_line = GEOSGeom_createLineString_r(geos_context_, seg_coord_seq);
            if (GEOSIntersects_r(geos_context_, line, seg_line)) 
            {
                intersects = true;
                GEOSGeom_destroy_r(geos_context_, seg_line);
                break;
            }
            GEOSGeom_destroy_r(geos_context_, seg_line);
        }
        GEOSGeom_destroy_r(geos_context_, line);
        return intersects;
    }
 
    bool GeometryEngine::path_maintains_constraints(const std::vector<Point>& path, const std::vector<Segment>& segments) const 
    {    
        for (size_t i = 0; i < path.size() - 1; ++i) 
        {
            const Point& from = path[i];
            const Point& to = path[i + 1];    
            for (const auto& segment : segments) 
            {
                if (!segment.is_point_on_correct_side(segment.left, true)) 
                {
                    return false;
                }
                if (!segment.is_point_on_correct_side(segment.right, false)) 
                {
                    return false;
                }
            }
        }
        return true;
    }

    double GeometryEngine::calculate_distance(const Point& from, const Point& to) const 
    {
        return from.distance_to(to);
    }

    bool GeometryEngine::is_visible(const Point& from, const Point& to, const std::vector<Segment>& segments, int current_segment_order) const 
    {
        for (const auto& segment : segments) 
        {
            if (segment.order <= current_segment_order) 
            {
                continue; 
            }
            if (line_intersects_obstacles(from, to, {segment})) 
            {
                return false;
            }
        }
        for (const auto& segment : segments) 
        {
            double cross_left = (to.x - from.x) * (segment.left.y - from.y) - (to.y - from.y) * (segment.left.x - from.x);
            double cross_right = (to.x - from.x) * (segment.right.y - from.y) - (to.y - from.y) * (segment.right.x - from.x);
            if (cross_left <= 0 || cross_right >= 0) 
            {
                return false;
            }
        }
        return true;
    }
}
