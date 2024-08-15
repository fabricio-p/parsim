#include <sstream>
#include <iomanip>
#include <type_traits>

#include "body_csv_reader.hpp"
#define DEBUGGING
#include "util.hpp"

namespace {
class RowParser {
public:
    RowParser(std::istream& _is) : is(_is) {}
    template <bool Skip = true, typename T>
    RowParser& readColumn(T& v) {
        if constexpr (std::is_base_of<std::basic_string<char>, T>::value) {
            std::getline(is, v, ',');
        } else {
            is>>v;
            if constexpr (Skip) {
                is.ignore(1);
            }
        }
        return *this;
    }
private:
    std::istream& is;
};
}

BodyCSVReader::BodyCSVReader(std::istream& _in) : in(_in) {}

void BodyCSVReader::readInto(Simulation& simulation) {
    // ignore the first row with the column names
    // btw the column names are
    // posx,posy,velx,vely,radius,color,material
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    for (;;) {
        DUMP(in.eof());
        if (in.eof()) {
            break;
        }
        Vector2
            position
        ,   velocity
        ;
        float radius;
        Color color;
        std::string colorDesc;
        size_t material;
        RowParser(in)
            .readColumn(position.x)
            .readColumn(position.y)
            .readColumn(velocity.x)
            .readColumn(velocity.y)
            .readColumn(radius)
            .readColumn(colorDesc)
            .readColumn(material);
        if (colorDesc[0] == '#') {
            std::istringstream ss(colorDesc);
            ss.ignore(1);
            ss
                >>std::setw(2)>>color.r
                >>std::setw(2)>>color.g
                >>std::setw(2)>>color.b
                >>std::setw(2)>>color.a;
        } else {
            color = colorMap[colorDesc];
        }
        DUMP(position);
        simulation.add(position, velocity, radius, color, material);
    }
}