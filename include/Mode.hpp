#ifndef MODE_HPP
#define MODE_HPP

enum class Mode {
    NOTHING,
    INSERT_NODES,
    INSERT_ARIST,
    REMOVE_ARISTS,
    REMOVE_NODE,
    COUNT
};

enum class Algorithm {
    A_STAR,
    DIJKSTRA,
    COUNT
};

enum class Map {
    RANDOM,
    LIMA,
    AREQUIPA,
    NEW_YORK,
    COUNT
};

enum class Select {
    NOTHING,
    CALCULATE_ROUTE,
    CHANGE_ALG,
    CREATE_MAP,
    CHANGE_MAP,
    COUNT
};

#endif
