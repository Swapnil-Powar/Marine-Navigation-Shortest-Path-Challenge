#pragma once
#include "geometry.h"
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
namespace marine_nav 
{
    struct InputData 
    {
        std::vector<Point> points;
        std::vector<Segment> segments;
        Point start;
        Point end;
        InputData(const Point& s, const Point& e) : start(s), end(e) {}
    };
    
    class JsonParser 
    {
        public:
            static InputData parse_input_file(const std::string& filename);    
            static InputData parse_input_string(const std::string& json_str);
            static std::string export_path_to_json(const std::vector<Point>& path, double total_distance);
            static void export_path_to_file(const std::vector<Point>& path, double total_distance, const std::string& filename);
        private:
            static std::vector<Segment> create_segments_from_points(const std::vector<Point>& points);
            static Point find_point_by_label(const std::vector<Point>& points, const std::string& label);
    };
} 
