#ifndef CS3910__EXCTRACT_H_
#define CS3910__EXCTRACT_H_

#include <algorithm>
#include <cassert>
#include <fstream>
#include <istream>
#include <iterator>
#include <string>
#include <vector>

struct CS3910ExtractTraits
{
    using value_type = struct {
        std::string id;
        double x;
        double y;
    };

    static void Preamble(std::istream& ins)
    {
        // Skip the headers
        std::string header;
        std::getline(ins, header);
        std::getline(ins, header);
        std::getline(ins, header);
    }

    static bool Extract(std::istream& ins, value_type& data)
    {
        std::string line;
        if (!(ins >> line))
            return false;

        auto first = line.begin();
        auto last = std::find(first, line.end(), ',');

        data.id = std::string{ first, last };

        first = last + 1;
        last = std::find(first, line.end(), ',');

        data.x = std::stod(std::string{ first, last });

        first = last + 1;
        last = std::find(first, line.end(), ',');
        data.y = std::stod(std::string{ first, last });
        return true;
    }
};
template<typename ExtractTraits>
std::vector<typename ExtractTraits::value_type> ExtractDataFrom(char const* fileName)
{
    assert(static_cast<bool>(fileName) && "The filename is nullptr");
    std::ifstream csv{};
    csv.open(fileName, std::ios_base::in);

    ExtractTraits::Preamble(csv);
    std::vector<typename ExtractTraits::value_type> data{};
    for (typename ExtractTraits::value_type value; ExtractTraits::Extract(csv, value);)
        data.emplace_back(std::move(value));

    return data;
};

#endif // !CS3910__EXCTRACT_H_
