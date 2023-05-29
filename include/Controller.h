//-------------------INCLUDE SECTION---------------------------------------------------
#include <unordered_map>
#include <string>
#include <utility>
#include <fstream>
#include <sstream>
#include <iterator>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <functional>
#include <map>

//-------------------USING SECTION---------------------------------------------------
using NormFunction = std::function<double(double, double)>; 
using SortedCityMap = std::multimap<double, std::string>;
using Coordinates = std::pair<double, double>;

//-------------------CLASS SECTION---------------------------------------------------
class Controller {
private:
    std::map<std::string, Coordinates> m_cityCoords; // store city coordinates

    /**
    * this function validates the input of the user, it recieves the prompt message to show to user
    * error message suitable for this input, the validator of that input and an input handler.
    */
    template<typename T, typename Validator, typename InputHandler> 
    T validateInput(const std::string& prompt, const std::string& errorMessage, Validator validator, InputHandler inputHandler) {
        while (true) {
            std::cout << prompt << std::endl;
            T input;
            inputHandler(input);

            if (validator(input)) {
                return input;
            }

            std::cout << errorMessage << std::endl;
        }
    }

    void load_data(const std::string& file_path); // load data function

    SortedCityMap getCitiesInRadius(const std::string& selectedCity, double radius, NormFunction norm);

    long long countCitiesNorthOf(const std::string& city, const SortedCityMap& citiesInRadius);

    void printSearchResults(const SortedCityMap& citiesInRadius, long long northCount);

    std::string validateCity(); // validator that checks the City Name

    double validateRadius(); // validator that checks the radius

    NormFunction validateNorm(); // validator that checks the norm

    double computeDistance(NormFunction norm, const Coordinates& coord1, const Coordinates& coord2);


public:
    void run();
};

