#include "Configuration.h" // adjust this to Configuration.h only

// helper function
string trim(const string &str)
{
    size_t start = str.find_first_not_of(" \t");
    size_t end = str.find_last_not_of(" \t");
    return (start == string::npos) ? "" : str.substr(start, end - start + 1);
}

void processFile(const string &filename, TerrainType **&battlefield, int &NUM_ROWS, int &NUM_COLS, vector<UNIT_NAME> &units, int &EVENT_CODE)
{
    // TODO
    ifstream file(filename);
    if (!file)
        return;
    string line;

    // process col and row first
    while (getline(file, line))
    {
        istringstream iss(line);
        string key, value;
        if (getline(iss, key, '='))
        {
            getline(iss, value);
            key = trim(key);
            value = trim(value);

            if (key == "NUM_ROWS")
            {
                NUM_ROWS = stoi(value);
            }
            else if (key == "NUM_COLS")
            {
                NUM_COLS = stoi(value);
                // allocate default bij = roa
                battlefield = new TerrainType *[NUM_ROWS];
                for (int i = 0; i < NUM_ROWS; i++)
                {
                    battlefield[i] = new TerrainType[NUM_COLS];
                    for (int j = 0; j < NUM_COLS; j++)
                    {
                        battlefield[i][j] = ROAD;
                    }
                }
            }
        }
    }
    file.clear();  // reset file
    file.seekg(0); // move to first char of file

    while (getline(file, line))
    {
        istringstream iss(line);
        string key, value;
        if (getline(iss, key, '='))
        {
            getline(iss, value);
            key = trim(key);
            value = trim(value);
            if (key == "EVENT_CODE")
            {
                string twodigits = value.substr(max(0, (int)value.size() - 2));
                EVENT_CODE = stoi(twodigits);
            }
            else if (key.find("ARRAY_") == 0)
            {
                TerrainType terrainType;
                if (key == "ARRAY_FOREST")
                    terrainType = FOREST;
                else if (key == "ARRAY_RIVER")
                    terrainType = RIVER;
                else if (key == "ARRAY_FORTIFICATION")
                    terrainType = FORTIFICATION;
                else if (key == "ARRAY_URBAN")
                    terrainType = URBAN;
                else if (key == "ARRAY_SPECIAL_ZONE")
                    terrainType = SPECIAL_ZONE;
                else
                    continue;
                size_t pos = 0;
                while ((pos = value.find("(")) != string::npos)
                {
                    size_t end = value.find(")", pos);
                    if (end == string::npos)
                        break;

                    string coord = value.substr(pos + 1, end - pos - 1);
                    int x, y;
                    sscanf(coord.c_str(), "%d,%d", &x, &y);
                    battlefield[x][y] = terrainType;

                    value = value.substr(end + 1);
                }
            }
            else if (key == "UNIT_LIST")
            {
                while (getline(file, line) && line != "]")
                {
                    line = trim(line);
                    if (line.find('(') == string::npos)
                        continue;

                    // unit name
                    size_t openParen = line.find('(');
                    string unitName = line.substr(0, openParen);
                    InfantryType type;

                    if (unitName == "SNIPER")
                        type = SNIPER;
                    else if (unitName == "ANTIAIRCRAFTSQUAD")
                        type = ANTIAIRCRAFTSQUAD;
                    else if (unitName == "MORTARSQUAD")
                        type = MORTARSQUAD;
                    else if (unitName == "ENGINEER")
                        type = ENGINEER;
                    else if (unitName == "SPECIALFORCES")
                        type = SPECIALFORCES;
                    else if (unitName == "REGULARINFANTRY")
                        type = REGULARINFANTRY;
                    else
                        continue;

                    int quantity, weight, x, y, armyBelong;
                    sscanf(line.c_str() + openParen, "(%d,%d,(%d,%d),%d)", &quantity, &weight, &x, &y, &armyBelong);

                    UNIT_NAME unit;
                    unit.quantity = quantity;
                    unit.weight = weight;
                    unit.position_x = x;
                    unit.position_y = y;
                    unit.armyBelong = armyBelong;
                    unit.type = type;

                    units.push_back(unit);
                }
            }
        }
    }
    file.close();
}
string print_battlefield(const TerrainType **battlefield, const int &NUM_ROWS, const int &NUM_COLS)
{
    stringstream ss;
    // ss << "VOTIEN";
    // TODO
    const string ter[] = {"ROA", "FOR", "RIV", "FOR", "URB", "SPE"};

    for (int i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLS; j++)
        {
            ss << ter[battlefield[i][j]];
            if (j < NUM_COLS - 1)
                ss << " ";
        }
        if (i < NUM_ROWS - 1)
            ss << "\n";
    }
    return ss.str();
}
string print_UNIT_NAME(const vector<UNIT_NAME> &units)
{
    stringstream ss;
    // ss << "VOTIEN";
    // TODO
    ss << "[\n";

    const string InfantryNames[] = {"SNIPER", "ANTIAIRCRAFTSQUAD", "MORTARSQUAD",
                                    "ENGINEER", "SPECIALFORCES", "REGULARINFANTRY"};
    int len = units.size();
    for (int i = 0; i < len; i++)
    {
        const UNIT_NAME &unit = units[i];

        ss << InfantryNames[unit.type] << "("
           << unit.quantity << ","
           << unit.weight << ",("
           << unit.position_x << ","
           << unit.position_y << "),"
           << unit.armyBelong << ")";

        if (i < len - 1)
            ss << ",\n";
    }

    ss << "\n]";

    return ss.str();
}
void clearBattlefield(TerrainType **battlefield, const int &NUM_ROWS, const int &NUM_COLS)
{
    // TODO
    for (int i = 0; i < NUM_ROWS; i++)
    {
        for (int j = 0; j < NUM_COLS; j++)
            battlefield[i][j] = ROAD;
    }

    for (int i = 0; i < NUM_ROWS; i++)
        delete[] battlefield[i];

    delete[] battlefield;

    battlefield = nullptr;
}

// int main()
// {
//     int NUM_ROWS = 2;
//     int NUM_COLS = 3;
//     TerrainType **battlefield = new TerrainType *[NUM_ROWS];
//     for (int i = 0; i < NUM_ROWS; i++)
//         battlefield[i] = new TerrainType[NUM_COLS];

//     // (tương đương {{0, 1, 2}, {3, 4, 5}})
//     battlefield[0][0] = ROAD;
//     battlefield[0][1] = FOREST;
//     battlefield[0][2] = RIVER;
//     battlefield[1][0] = FORTIFICATION;
//     battlefield[1][1] = URBAN;
//     battlefield[1][2] = SPECIAL_ZONE;
//         cout << print_battlefield(battlefield, NUM_ROWS, NUM_COLS);

//         for (int i = 0; i < NUM_ROWS; i++)
//         {
//             delete[] battlefield[i];
//         }
//         delete[] battlefield;
//         return 0;

//     vector<UNIT_NAME> units = {
//         {5, 2, 1, 2, false, TANK},
//         {5, 2, 3, 2, true, TANK},
//         {5, 2, 1, 1, false, REGULARINFANTRY},
//         {5, 2, 3, 3, true, REGULARINFANTRY}};

//     cout << print_UNIT_NAME(units);

//     clearBattlefield(battlefield, NUM_ROWS, NUM_COLS);
//     cout << (battlefield == nullptr);

//     int NUM_ROWS, NUM_COLS, EVENT_CODE;
//     TerrainType **battlefield = nullptr;
//     vector<UNIT_NAME> units;

//     processFile("test.txt", battlefield, NUM_ROWS, NUM_COLS, units, EVENT_CODE);

//     cout << "NUM_ROWS = " << NUM_ROWS << "\n";
//     cout << "NUM_COLS = " << NUM_COLS << "\n";
//     cout << "EVENT_CODE = " << EVENT_CODE << "\n";

//     cout << print_battlefield(battlefield, NUM_ROWS, NUM_COLS) << "\n";
//     cout << print_UNIT_NAME(units) << "\n";
//     cout << "Number of units: " << units.size() << endl;
//     if (units.empty())
//     {
//         cout << "No units loaded!\n";
//     }
//     else
//     {
//         cout << print_UNIT_NAME(units) << endl;
//     }
// }