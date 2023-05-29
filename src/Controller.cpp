//-------------------INCLUDE SECTION---------------------------------------------------
#include "Controller.h"
#include <set>

//-------------------FUNCTIONS---------------------------------------------------------
/*
* this function recieves the path of data.txt file that has all the cities, opens the 
* file and extracts the data needed, the city name and its coordinates, and stores 
* them into the suitable class members. The function as well, handles all errors if
* any occur
*/
void Controller::load_data(const std::string& file_path) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file\n");
    }

    std::string line;
    while (std::getline(file, line)) {
        const std::string cityName = line;

        if (!std::getline(file, line)) {
            throw std::runtime_error("Could not read line\n");
        }

        double lat, lon;
        std::istringstream iss(line);
        if (!(iss >> lat && iss.ignore(3) && iss >> lon)) {
            throw std::runtime_error("Could not parse data\n");
        }

        m_cityCoords[cityName] = std::make_pair(lat, lon);
        iss.clear(); // Clear the stream state
    }

    if (file.bad()) {
        throw std::runtime_error("Could not read file");
    }
}

//-------------------------------------------------------------------------------------
/*
* this function recieves the city name, radius and which norm function the user wanted.
* and computes the distance between it and the other cities, it obviusly skips itself,
* each city found in that radius is added a data structure of type SortedCityMap, which 
* are the Cities found in the wanted radius SORTED. the function returns this data structure.
*/
SortedCityMap Controller::getCitiesInRadius(const std::string& city, double radius, NormFunction normFunction) {
    SortedCityMap citiesInRadius;

    for (const auto& cityData : m_cityCoords) {
        const std::string& cityName = cityData.first;
        const Coordinates& cityCoords = cityData.second;

        if (cityName == city) {
            continue; 
        }

        double distance = computeDistance(normFunction, m_cityCoords[city], cityCoords);

        if (distance <= radius) {
            citiesInRadius.emplace(distance, cityName);
        }
    }

    return citiesInRadius;
}

//-------------------------------------------------------------------------------------
/*
* this function recieves the city name, and the cities in that radius, it counts the 
* cities found up north of the wanted city. the function returns the counter.
*/
long long Controller::countCitiesNorthOf(const std::string& city, const SortedCityMap& citiesInRadius) {
    auto it = citiesInRadius.begin();
    while (it != citiesInRadius.end() && it->second != city) {
        ++it;
    }

    long long count = 0;
    if (it != citiesInRadius.end()) { 
        for (++it; it != citiesInRadius.end(); ++it) {
            count += (m_cityCoords[it->second].first > m_cityCoords[city].first);
        }
    }

    return count;
}

//-------------------------------------------------------------------------------------
/*
* this function recieves the cities in the wanted radius data structure and the amount
* of cities found up north that wanted city, the function simply displays these data 
* to the user.
*/
void Controller::printSearchResults(const SortedCityMap& citiesInRadius, long long northCount) {
    std::cout << "Cities found within the specified radius:" << std::endl;
    for (const auto& cityData : citiesInRadius) {
        std::cout << cityData.second << " (" << cityData.first << " km away)" << std::endl;
    }

    std::cout << "Number of cities located to the north of the selected city: " << northCount << std::endl;
}

//-------------------------------------------------------------------------------------
/*
* this function validates if the city is indeed found in the file. uisng Lambda functions, it
* sends the prompt error message, validator and the input handler.
*/
std::string Controller::validateCity() {
    return validateInput<std::string>("Please enter selected city name (with line break after it), or enter '0' to exit:",
        "City not found in the city list",
        [&](const std::string& input) {
            return (input == "0" || m_cityCoords.count(input) > 0);
        },
        [&](std::string& input) {
            std::getline(std::cin, input);
        });
}

//-------------------------------------------------------------------------------------
/*
* this function validates if the radius given is correct. uisng Lambda functions, it
* sends the prompt error message, validator and the input handler.
*/
double Controller::validateRadius() {
    return validateInput<double>("Please enter the radius (in km) for the city search (with line break after it):",
        "Invalid radius, please enter a positive value",
        [](double input) {
            return input > 0.0;
        },
        [](double& input) {
            std::cin >> input;
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        });
}

//-------------------------------------------------------------------------------------
/*
* this function validates if the norm function chosen is from the list of options provided.
* uisng Lambda functions, it sends the prompt error message, validator and the input handler.
*/
NormFunction Controller::validateNorm() {
    int norm;
    std::cout << "Please enter the wanted norm (0 - L2, Euclidean distance, 1 - Linf, Chebyshev distance, 2 - L1, Manhattan distance): ";
    std::cin >> norm;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::map<int, NormFunction> norms{
        { 0, [](double x, double y) { return std::hypot(x, y); } },
        { 1, [](double x, double y) { return std::max(std::abs(x), std::abs(y)); } },
        { 2, [](double x, double y) { return std::abs(x) + std::abs(y); } }
    };

    while (norms.count(norm) == 0) {
        std::cout << "Invalid norm, please enter a valid norm: ";
        std::cin >> norm;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    return norms[norm];
}

//-------------------------------------------------------------------------------------
/*
* this function recieves the norm function chose, and the coordinated of both cities,
* computes the distance using the norm function chosen and returns the distance in double
*/
double Controller::computeDistance(NormFunction norm, const Coordinates& coord1, const Coordinates& coord2) {
    double xDiff = coord1.first - coord2.first;
    double yDiff = coord1.second - coord2.second;
    return norm(xDiff, yDiff);
}

//-------------------------------------------------------------------------------------
/*
* this function is called form main and handles the correct executions of each function.
* it runs in an infinite loop to keep getting input from the user unless he writes 0 
* which breaks the loop, allowing him to exit the program.
*/
void Controller::run() {
    std::string file_path = "data.txt";
    load_data(file_path);

    try {
        while (true) {
            std::string selectedCity = validateCity();
            if (selectedCity == "0") {
                break;
            }

            double radius = validateRadius();
            NormFunction norm = validateNorm();

            SortedCityMap citiesInRadius = getCitiesInRadius(selectedCity, radius, norm);
            long long northCount = countCitiesNorthOf(selectedCity, citiesInRadius);

            printSearchResults(citiesInRadius, northCount);
        }
    }
    catch (const std::exception& ex) {
        std::cout << "Error: " << ex.what() << std::endl;
    }
}

