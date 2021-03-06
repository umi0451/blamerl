#include "map.h"
#include "log.h"
#include <cstdlib>

CellType::CellType(const Sprite & _sprite, bool _passable, bool _transparent, const std::string & cell_name)
	: sprite(_sprite), passable(_passable), transparent(_transparent), name(cell_name)
{
}

CellType::CellType()
	: sprite(' '), passable(false), transparent(true), name("void")
{
}

Cell::Cell(int cell_type)
	: type(cell_type), visible(false), seen(false)
{
}

// --------------------------------------------------------------------------

Transport::Transport(int _x, int _y, bool _is_up, const std::string & _name)
    : x(_x), y(_y), is_up(_is_up), sprite(is_up ? '<' : '>'), name(_name)
{
}

// --------------------------------------------------------------------------

Door::Door(int _x, int _y)
	: x(_x), y(_y), opened(false), sprite('+')
{
	name = "a door";
}

void Door::open()
{
	opened = true;
	sprite = '-';
}

void Door::close()
{
	opened = false;
	sprite = '+';
}

//------------------------------------------------------------------------------

unsigned Map::register_type(const CellType & cell_type)
{
	cell_types.push_back(cell_type);
	return cell_types.size() - 1;
}

const CellType & Map::get_cell_type(const Cell & cell) const
{
    return get_cell_type(cell.type);
}

const CellType & Map::get_cell_type(unsigned cell_type_id) const
{
    if(cell_type_id >= cell_types.size()) {
        static CellType void_cell;
        return void_cell;
    }
    return cell_types[cell_type_id];
}

Map::Map()
	: width(0), height(0)
{
}

Map::Map(int w, int h)
	: width(w), height(h), map(width * height)
{
}

void Map::generate(int w, int h)
{
    map.resize(w * h, Cell());
    width = w;
    height = h;
	doors.clear();
    cell_types.clear();
    log("Cells created: {0}.").arg(map.size());

	Cell floor = register_type(CellType('.', true, true, "a floor"));
	Cell wall = register_type(CellType('#', false, false, "a wall"));
	Cell brick_wall = register_type(CellType('#', false, false, "a brick wall"));
	Cell wooden_wall = register_type(CellType('#', false, false, "a wooden wall"));
	Cell doorway = register_type(CellType('.', true, true, "a doorway"));
	Cell glass = register_type(CellType('=', false, true, "a glass wall"));
    log("Cell types registered: {0}.").arg(cell_types.size());

    fill(floor);
    log("Map filled with floor.");

    for(int i = 0; i < 10; ++i) {
        int x = rand() % width;
        int y = rand() % height;
        cell(x, y) = wall;
    }
    log("Random columns placed.");
    for(int i = 0; i < 5; ++i) {
        int x1 = rand() % (width / 2);
        int y = rand() % height;
        int x2 = width / 2 + rand() % (width / 2);
        for(int x = x1; x <= x2; ++x) {
            cell(x, y) = brick_wall;
        }
        log("\tWall created {0}..{1} at y={2}").arg(x1).arg(x2).arg(y);
        if(x2 - x1 > 2) {
            int glass_start = x1 + 1 + rand() % (x2 - x1 - 2);
            int glass_width = rand() % (x2 - 1 - glass_start);
            for(int x = glass_start; x <= glass_start + glass_width; ++x) {
                cell(x, y) = glass;
            }
            log("\tGlass settled: {0}..{1}.").arg(glass_start).arg(glass_start + glass_width);
        }
		int door_x = x1 + rand() % (x2 - x1);
		cell(door_x, y) = doorway;
		doors.push_back(Door(door_x, y));
        log("\tDoor placed at {0}, {1}.").arg(door_x).arg(y);
    }
    log("Horizontal walls placed.");
    for(int i = 0; i < 5; ++i) {
        int x = rand() % width;
        int y1 = rand() % (height / 2);
        int y2 = height / 2 + rand() % (height / 2);
        for(int y = y1; y <= y2; ++y) {
            cell(x, y) = wooden_wall;
        }
		int door_y = y1 + rand() % (y2 - y1);
		cell(x, door_y) = doorway;
		doors.push_back(Door(x, door_y));
    }
    log("Vertical walls placed.");

    transports.clear();
    transports.push_back(Transport(rand() % width, rand() % height, false, "a downstairs"));
    transports.push_back(Transport(rand() % width, rand() % height, true, "an upstairs"));

    log("Map is successfully generated.");
}

bool Map::valid(int x, int y) const
{
	return (x >= 0 && x < width && y >= 0 && y < height);
}

void Map::fill(const Cell & filler)
{
    for(std::vector<Cell>::iterator c = map.begin(); c != map.end(); ++c) {
        *c = filler;
    }
}

Cell & Map::cell(int x, int y)
{
	if(!valid(x, y)) {
        throw OutOfMapException(x, y);
	}
	return map[x + y * width];
}

const Cell & Map::cell(int x, int y) const
{
	if(!valid(x, y)) {
        throw OutOfMapException(x, y);
	}
	return map[x + y * width];
}

const Sprite & Map::sprite(int x, int y) const
{
	if(!cell(x, y).visible && !cell(x, y).seen) {
		static Sprite CANNOT_SEE = ' ';
		return CANNOT_SEE;
	}
	for(std::vector<Transport>::const_iterator transport = transports.begin(); transport != transports.end(); ++transport) {
		if(transport->x == x && transport->y == y) {
			return transport->sprite;
		}
	}
	for(std::vector<Door>::const_iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			return door->sprite;
		}
	}
	return get_cell_type(cell(x, y)).sprite;
}

const std::string & Map::name(int x, int y) const
{
	for(std::vector<Transport>::const_iterator transport = transports.begin(); transport != transports.end(); ++transport) {
		if(transport->x == x && transport->y == y) {
			return transport->name;
		}
	}
	for(std::vector<Door>::const_iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			return door->name;
		}
	}
	return get_cell_type(cell(x, y)).name;
}

bool Map::transparent(int x, int y) const
{
	for(std::vector<Transport>::const_iterator transport = transports.begin(); transport != transports.end(); ++transport) {
		if(transport->x == x && transport->y == y) {
			return true;
		}
	}
	for(std::vector<Door>::const_iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			return door->opened;
		}
	}
	return get_cell_type(cell(x, y)).transparent;
}

bool Map::passable(int x, int y) const
{
	for(std::vector<Transport>::const_iterator transport = transports.begin(); transport != transports.end(); ++transport) {
		if(transport->x == x && transport->y == y) {
			return true;
		}
	}
	for(std::vector<Door>::const_iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			return door->opened;
		}
	}
	return get_cell_type(cell(x, y)).passable;
}

void Map::open_at(int x, int y)
{
	for(std::vector<Door>::iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			if(!door->opened) {
				door->open();
			}
		}
	}
}

void Map::close_at(int x, int y)
{
	for(std::vector<Door>::iterator door = doors.begin(); door != doors.end(); ++door) {
		if(door->x == x && door->y == y) {
			if(door->opened) {
				door->close();
			}
		}
	}
}

