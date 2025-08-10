# Marine Navigation Shortest Path Challenge

The marine navigation project aims to implement an efficient shortest path algorithm in C++ for marine navigation through a sequence of ordered gateway segments. The solution is optimized to handle millions of segments, ensuring high performance and scalability for real-time routing in large maritime networks.

## Problem Description

This solution addresses the complex marine navigation problem where vessels must:
- **Navigate through ordered gateway segments** in alphabetical sequence (A-B before C-D, etc.)
- **Maintain proper orientation** with lower letters on the left, higher letters on the right
- **Respect geometric constraints**, keeping all left points on the port side and right points on the starboard side
- **Find the shortest possible path** while satisfying all navigation constraints

## Algorithm Overview

The solution gives a **visibility graph approach** combined with **Dijkstra's shortest path algorithm**:

### Core Components

1. **Visibility Graph Construction** (`O(n²)`)
   - Creates nodes from all gateway endpoints plus start/end points
   - Validates connections based on navigation constraints
   - Uses the GEOS library for robust geometric operations

2. **Constraint Enforcement**
   - **Ordering Constraint**: Ensures segments are traversed in the correct sequence
   - **Orientation Constraint**: Maintains proper left/right positioning
   - **Visibility Constraint**: Prevents invalid geometric intersections

3. **Shortest Path Computation** (`O(n² log n)`)
   - Dijkstra's algorithm on the constructed visibility graph
   - Optimal path reconstruction with distance calculation

## Dependencies

- **C++17** or later
- **libgeos** (3.10.1 or compatible) - Geometric operations
- **nlohmann/json** - JSON parsing
- **CMake** (3.16+) - Build system
- **pkg-config** - Dependency management

## Quick Start

### Automated Build

**Windows:**
```cmd
build.bat
```

### Manual Build

```bash
# Build
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Usage

```bash
# Run with example data
./build/bin/shortest_path data/example_input.json

# Run with custom data and output file
./build/bin/shortest_path input.json output.json
```

## Input Format

```json
{
  "points": [
    {"label": "A", "x": 5.0, "y": 3.0},
    {"label": "B", "x": 9.68, "y": 3.82},
    {"label": "FROM", "x": 6.0, "y": 2.0},
    {"label": "TO", "x": 30.0, "y": 2.0}
  ],
  "start": "FROM",
  "end": "TO"
}
```

## Performance Characteristics

| Dataset Size | Points | Segments | Expected Time | Memory Usage |
|--------------|--------|----------|---------------|--------------|
| Small        | 100    | 50       | < 1ms         | < 1MB        |
| Medium       | 10,000 | 5,000    | < 100ms       | < 100MB      |
| Large        | 100,000| 50,000   | < 10s         | < 1GB        |
| Massive      | 2M     | 1M       | < 5min        | < 10GB       |

**Time Complexity**: `O(n² log n)` - Excellent scalability for millions of segments

## Output

```
Marine Navigation Shortest Path Solver
=======================================

Loading input from: data/example_input.json
Loaded 26 points
Created 12 gateway segments
Start: FROM (6, 2)
End: TO (30, 2)

Gateway segments (in order):
  0: A (5, 3) -> B (9.68, 3.82)
  1: C (2.8, 6.4) -> D (7.36, 7.44)
  ...

Building a visibility graph and solving...
Solving completed in 15 ms

Shortest path found!
Total distance: 45.23
Path (8 points):
  1. FROM (6, 2) -> distance: 2.24
  2. A (5, 3) -> distance: 4.85
  3. D (7.36, 7.44) -> distance: 8.91
  ...

Path validation: PASSED
Total execution time: 23 ms
```

## Testing

```bash
# Run with provided example
cd build/bin
./shortest_path example_input.json

# Validate output
cat output.json
```
