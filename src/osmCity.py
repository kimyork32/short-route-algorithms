import osmnx as ox
import matplotlib.pyplot as plt
import sys

# distrito, ciudad, pais 
args = sys.argv[1:]

district = args[2]
city =  args[1]
country = args[0]


# Mostrar logs de osmnx si quieres
ox.settings.log_console = True
ox.settings.use_cache = True

# Obtener el grafo de la ciudad (ejemplo: Arequipa, Perú)
place_name = f"{district}, {city}, {country}"
# place_name = "Arequipa, Peru"
G = ox.graph_from_place(place_name, network_type='drive')

# Dibujar el grafo con nodos (puntos) y aristas (segmentos)
fig, ax = ox.plot_graph(G,
                        node_size=3,         # Tamaño de los nodos
                        node_color="red",     # Color de los nodos
                        edge_color="gray",    # Color de los caminos
                        edge_linewidth=0.8,   # Grosor de los caminos
                        bgcolor="white")      # Fondo blanco

nodes, edges = ox.graph_to_gdfs(G)


# Bounding box del grafo
north = max(nodes['y'])
south = min(nodes['y'])
east  = max(nodes['x'])
west  = min(nodes['x'])

h = 2000
a = (east + west) / 2
b = (north + south) / 2

xp = h / a
yp = h / b

with open(f"../resources/city.txt", 'w') as file:

    file.write(f"{north}" + "\n") # norte top
    file.write(f"{south}" + "\n") # sur botton
    file.write(f"{east}" + "\n") # este right
    file.write(f"{west}" + "\n") # oeste left

    for i, (u, v, key, data) in enumerate(G.edges(keys=True, data=True)):
        if 'geometry' in data:
            # Extraer y aplanar los puntos: lon1 lat1 lon2 lat2 ...
            coords = [f"{lon} {lat}" for lon, lat in data['geometry'].coords]
            line = " ".join(coords)
            file.write(line + "\n")
        else:
            # Línea recta entre nodos u y v
            x1, y1 = G.nodes[u]['x'], G.nodes[u]['y']
            x2, y2 = G.nodes[v]['x'], G.nodes[v]['y']
            line = f"{x1} {y1} {x2} {y2}"
            file.write(line + "\n")
