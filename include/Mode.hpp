#ifndef MODE_HPP
#define MODE_HPP

enum class Mode {
    NOTHING,
    INSERT_NODES,
    INSERT_ARIST,
    REMOVE_ARISTS,
    REMOVE_NODE,
    SELECT_START_END_NODE,
    PLACE_BARRIERS,
    REMOVE_BARRIERS,
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
    // Botones de modo
    MODE_INSERT_NODES,
    MODE_INSERT_EDGES,
    MODE_REMOVE_NODES,
    MODE_REMOVE_EDGES,
    MODE_SELECT_START_END,
    MODE_PLACE_BARRIERS,
    MODE_REMOVE_BARRIERS,
    MODE_NOTHING,
    COUNT
};

#endif
