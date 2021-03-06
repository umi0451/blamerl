#pragma once
#include <vector>
#include <string>
#include <map>

typedef int Sprite;

struct CellType {
    Sprite sprite;
    bool passable;
	bool transparent;
	std::string name;
    CellType();
    CellType(const Sprite & _sprite, bool _passable, bool _transparent, const std::string & cell_name);
};

struct Cell {
	int type;
	bool visible;
	bool seen;

	Cell(int cell_type = 0);
};

struct Door {
	int x, y;
	bool opened;
	Sprite sprite;
	std::string name;
	Door(int _x, int _y);
	void open();
	void close();
};

struct Transport {
    int x, y;
    bool is_up;
    Sprite sprite;
    std::string name;
    Transport(int _x, int _y, bool _is_up, const std::string & _name);
};

class Map {
public:
    struct OutOfMapException { int x, y; OutOfMapException(int _x, int _y) : x(_x), y(_y) {} };

	int width, height;

	std::vector<Door> doors;
	std::vector<Transport> transports;

    std::vector<CellType> cell_types;
    unsigned register_type(const CellType & cell_type);
    const CellType & get_cell_type(const Cell & cell) const;
    const CellType & get_cell_type(unsigned cell_type_id) const;

	std::vector<Cell> map;

	Map();
	Map(int w, int h);
    void generate(int w, int h);

	bool valid(int x, int y) const;

    void fill(const Cell & filler);

	const Sprite & sprite(int x, int y) const;
	const std::string & name(int x, int y) const;
	bool passable(int x, int y) const;
	bool transparent(int x, int y) const;
	void open_at(int x, int y);
	void close_at(int x, int y);

	Cell & cell(int x, int y);
	const Cell & cell(int x, int y) const;
};

