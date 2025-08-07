#include "json_parser.h"
#include <fstream>
#include <iostream>
#include <algorithm>
using json = nlohmann::json;
namespace marine_nav 
{
    InputData JsonParser::parse_input_file(const std::string& filename) 
    {
        std::ifstream file(filename);
        if (!file.is_open()) 
        {
            throw std::runtime_error("Could not open file: " + filename);
        }
        json j;
        file >> j;
        return parse_input_string(j.dump());
    }

    InputData JsonParser::parse_input_string(const std::string& json_str) 
    {
        json j = json::parse(json_str);
        std::vector<Point> points;
        if (j.contains("points") && j["points"].is_array()) 
        {
            for (const auto& point_json : j["points"]) 
            {
                std::string label = point_json["label"];
                double x = point_json["x"];
                double y = point_json["y"];
                points.emplace_back(label, x, y);
            }
        }
        std::string start_label = j.value("start", "FROM");
        std::string end_label = j.value("end", "TO");
        Point start = find_point_by_label(points, start_label);
        Point end = find_point_by_label(points, end_label);
        InputData input_data(start, end);
        input_data.points = points;
        std::vector<Point> segment_points;
        for (const auto& point : points) 
        {
            if (point.label != start_label && point.label != end_label) 
            {
                segment_points.push_back(point);
            }
        }
        input_data.segments = create_segments_from_points(segment_points);
        return input_data;
    }

    std::vector<Segment> JsonParser::create_segments_from_points(const std::vector<Point>& points) 
    {
        std::vector<Segment> segments;
        std::vector<Point> sorted_points = points;
        std::sort(sorted_points.begin(), sorted_points.end(), [](const Point& a, const Point& b) 
        {
            return a.label < b.label;
        });
        for (size_t i = 0; i < sorted_points.size(); i += 2) 
        {
            if (i + 1 < sorted_points.size()) 
            {
                const Point& left = sorted_points[i];    
                const Point& right = sorted_points[i + 1];   
                int order = i / 2; 
                segments.emplace_back(left, right, order);
            }
        }    
        return segments;
    }

    Point JsonParser::find_point_by_label(const std::vector<Point>& points, const std::string& label) 
    {
        auto it = std::find_if(points.begin(), points.end(), [&label](const Point& p) 
        {
            return p.label == label;
        });
        if (it != points.end()) 
        {
            return *it;
        }
        throw std::runtime_error("Point with label '" + label + "' not found");
    }

    std::string JsonParser::export_path_to_json(const std::vector<Point>& path, double total_distance) 
    {
        json result;    
        result["total_distance"] = total_distance;
        result["path"] = json::array();
        for (const auto& point : path) 
        {
            json point_json;
            point_json["label"] = point.label;
            point_json["x"] = point.x;
            point_json["y"] = point.y;
            result["path"].push_back(point_json);
        }
        return result.dump(2);
    }

    void JsonParser::export_path_to_file(const std::vector<Point>& path, double total_distance, const std::string& filename) 
    {
        std::string json_str = export_path_to_json(path, total_distance);
        std::ofstream file(filename);
        if (!file.is_open()) 
        {
            throw std::runtime_error("Could not create output file: " + filename);
        }
        file << json_str;
        file.close();
        std::cout << "Path exported to: " << filename << std::endl;
    }
} 
