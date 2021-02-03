#include <optional>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <io2d.h>
#include "route_model.h"
#include "render.h"
#include "route_planner.h"

using namespace std::experimental;

static std::optional<std::vector<std::byte>> ReadFile(const std::string &path)
{   
    std::ifstream is{path, std::ios::binary | std::ios::ate};
    if( !is )
        return std::nullopt;
    
    auto size = is.tellg();
    std::vector<std::byte> contents(size);    
    
    is.seekg(0);
    is.read((char*)contents.data(), size);

    if( contents.empty() )
        return std::nullopt;
    return std::move(contents);
}

int main(int argc, const char **argv)
{    
    std::string osm_data_file = "";
    if( argc > 1 ) {
        for( int i = 1; i < argc; ++i )
            if( std::string_view{argv[i]} == "-f" && ++i < argc )
                osm_data_file = argv[i];
    }
    else {
        std::cout << "To specify a map file use the following format: " << std::endl;
        std::cout << "Usage: [executable] [-f filename.osm]" << std::endl;
        osm_data_file = "../map.osm";
    }
    
    std::vector<std::byte> osm_data;
 
    if( osm_data.empty() && !osm_data_file.empty() ) {
        std::cout << "Reading OpenStreetMap data from the following file: " <<  osm_data_file << std::endl;
        auto data = ReadFile(osm_data_file);
        if( !data )
            std::cout << "Failed to read." << std::endl;
        else
            osm_data = std::move(*data);
    }
    
    // TODO 1: Declare floats `start_x`, `start_y`, `end_x`, and `end_y` and get
    // user input for these values using std::cin. Pass the user input to the
    // RoutePlanner object below in place of 10, 10, 90, 90.

    // initializing navigation input variables
    int start_x;
    int start_y;
    int end_x;
    int end_y;
    char c;

    // Reading user input into navigation input variables
    std::cout << "Please enter the START coordinates as integers in the format xx, yy: ";
    if (std::cin >> start_x >> c >> start_y && c == ',' ){
        std::cout << "Thanks, we will use: [" << start_x << "," << start_y << "] as the START point \n\n";
    }
    // Using standard values in case the reading fails and informing the user
    else {
        start_x = 10;
        start_y = 10;
        std::cout << "We could not read your input and will use: [" << start_x << "," << start_y << "] as the START point \n\n";
    }

    // reset c before next input to detect wrong input properly
    c = 0;
    std::cin.clear();
    std::cin.ignore ( 100 , '\n' ); 
    
    std::cout << "Please enter the END coordinates as integers in the format xx, yy: ";
    if (std::cin >> end_x >> c >> end_y && c == ',' ){
        std::cout << "Thanks, we will use: [" << end_x << "," << end_y << "] as the END point \n\n";
    }
    // Using standard values in case the reading fails and informing the user
    else {
        end_x = 90;
        end_y = 90;
        std::cout << "We could not read your input and will use: [" << end_x << "," << end_y << "] as the END point \n\n";
    }


    // Build Model.
    RouteModel model{osm_data};

    // Create RoutePlanner object and perform A* search.
    RoutePlanner route_planner{model, (float)start_x, (float)start_y, (float)end_x, (float)end_y};
    route_planner.AStarSearch();

    std::cout << "Distance: " << route_planner.GetDistance() << " meters. \n";

    // Render results of search.
    Render render{model};

    auto display = io2d::output_surface{400, 400, io2d::format::argb32, io2d::scaling::none, io2d::refresh_style::fixed, 30};
    display.size_change_callback([](io2d::output_surface& surface){
        surface.dimensions(surface.display_dimensions());
    });
    display.draw_callback([&](io2d::output_surface& surface){
        render.Display(surface);
    });
    display.begin_show();
}
