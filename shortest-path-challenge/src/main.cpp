#include "json_parser.h"
#include "shortest_path.h"
#include <iostream>
#include <chrono>
using namespace marine_nav;
void print_usage(const char* program_name) 
{
    std::cout << "Usage: " << program_name << " <input_file.json> [output_file.json]\n";
    std::cout << "  input_file.json  - JSON file containing points and start/end labels\n";
    std::cout << "  output_file.json - Optional output file for the result (default: output.json)\n";
}

void print_path_info(const PathResult& result) 
{
    if (!result.found) 
    {
        std::cout << "No path found!\n";
        return;
    }
    std::cout << "Shortest path found!\n";
    std::cout << "Total distance: " << result.total_distance << "\n";
    std::cout << "Path (" << result.path.size() << " points):\n";
    for (size_t i = 0; i < result.path.size(); ++i) 
    {
        const auto& point = result.path[i];
        std::cout << "  " << (i + 1) << ". " << point.label 
                  << " (" << point.x << ", " << point.y << ")";
        if (i < result.path.size() - 1) 
        {
            double segment_distance = point.distance_to(result.path[i + 1]);
            std::cout << " -> distance: " << segment_distance;
        }
        std::cout << "\n";
    }
}

int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        print_usage(argv[0]);
        return 1;
    }
    std::string input_file = argv[1];
    std::string output_file = (argc >= 3) ? argv[2] : "output.json";
    try 
    {
        std::cout << "Marine Navigation Shortest Path Solver\n";
        std::cout << "======================================\n\n";
        std::cout << "Loading input from: " << input_file << "\n";
        auto start_time = std::chrono::high_resolution_clock::now();
        InputData input_data = JsonParser::parse_input_file(input_file);
        std::cout << "Loaded " << input_data.points.size() << " points\n";
        std::cout << "Created " << input_data.segments.size() << " gateway segments\n";
        std::cout << "Start: " << input_data.start.label 
                  << " (" << input_data.start.x << ", " << input_data.start.y << ")\n";
        std::cout << "End: " << input_data.end.label 
                  << " (" << input_data.end.x << ", " << input_data.end.y << ")\n\n";
        std::cout << "Gateway segments (in order):\n";
        for (const auto& segment : input_data.segments) 
        {
            std::cout << "  " << segment.order << ": " 
                      << segment.left.label << " (" << segment.left.x << ", " << segment.left.y << ")"
                      << " -> " 
                      << segment.right.label << " (" << segment.right.x << ", " << segment.right.y << ")\n";
        }
        std::cout << "\n";
        std::cout << "Building visibility graph and solving...\n";
        ShortestPathSolver solver;
        auto solve_start = std::chrono::high_resolution_clock::now();
        PathResult result = solver.solve(input_data.segments, input_data.start, input_data.end);
        auto solve_end = std::chrono::high_resolution_clock::now();
        auto solve_duration = std::chrono::duration_cast<std::chrono::milliseconds>(solve_end - solve_start);
        std::cout << "Solving completed in " << solve_duration.count() << " ms\n\n";
        print_path_info(result);
        if (result.found) 
        {
            std::cout << "\nValidating path...\n";
            bool is_valid = solver.validate_path(result.path, input_data.segments, input_data.start, input_data.end);
            std::cout << "Path validation: " << (is_valid ? "PASSED" : "FAILED") << "\n";
            if (!is_valid) 
            {
                std::cerr << "Warning: The computed path does not satisfy all constraints!\n";
            }
        }
        if (result.found) 
        {
            std::cout << "\nExporting result to: " << output_file << "\n";
            JsonParser::export_path_to_file(result.path, result.total_distance, output_file);
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        std::cout << "\nTotal execution time: " << total_duration.count() << " ms\n";
        return result.found ? 0 : 1;
    } 
    catch (const std::exception& e) 
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
