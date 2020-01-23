import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import matplotlib.cm as cm
import numpy as np


# def uniqueish_color():
#     """
#     Gets a random color

#     @return random color
#     """
#     return plt.cm.gist_ncar(np.random.random())


def plotTimes(opt_times, heur_times, title, filename):
    """
    Plots the execution times

    @param opt_times (list[tuple[int, float]]): execution times for CPLEX algorithm (first int is the problem size)

    @param heur_times (list[tuple[int, float]]): execution times for heuristic

    @param title (str): a string with plot title

    @param filename (str): a string with filename to save png plot (empty => do not save)
    """

    if title:
        print("Generazione grafico con tempi di esecuzione nella cartella plots/")

    plt.clf()
    opt_times.sort(key=lambda x: x[0])
    heur_times.sort(key=lambda x: x[0])
    N_opt, t_opt = zip(*opt_times)
    N_heur, t_heur = zip(*heur_times)

    plt.xticks(N_opt, N_opt)

    plt.plot(N_opt, t_opt, linestyle='--', marker='o', color='green',
             label='CPLEX optimal')
    plt.plot(N_heur, t_heur, linestyle='--', marker='o', color='orange',
             label='Lin-Keringhan heuristic')

    plt.title(title)
    plt.legend()
    plt.xlabel('Problem size (number of vertices)')
    plt.ylabel('Execution time (s)')
    if title:
        plt.savefig(('plots/' + filename + '.png'), format='png', dpi=800)
    else:
        plt.show()


def plotError(opt_value, heur_value, title, filename):
    """
    Plots obj values and errors

    @param opt_value (list[tuple[int, float]]): optimal obj values for each problem

    @param heur_value (list[tuple[int, float]]): heuristic obj values for each problem

    @param title (str): a string with plot title

    @param filename (str): a string with filename to save png plot (empty => do not save)
    """

    if title:
        print("Generazione grafico sull'errore nella cartella plots/")

    plt.clf()

    opt_value.sort(key=lambda x: x[0])
    heur_value.sort(key=lambda x: x[0])
    N_opt, v_opt = zip(*opt_value)
    N_heur, v_heur = zip(*heur_value)

    plt.xticks(N_opt, N_opt)

    plt.plot(N_opt, v_opt, linestyle='--', marker='o', color='blue',
             label='CPLEX optimal')
    plt.plot(N_heur, v_heur, linestyle='--', marker='o', color='red',
             label='Lin-Keringhan heuristic')

    errors = zip(N_heur, v_heur, v_opt)
    for x, y, opt in errors:
        err = abs(y - opt) / opt * 100
        plt.annotate(("%.3f" % err) + " %", xy=(x, y),
                     xytext=(-10, 10), textcoords="offset points")

    plt.title(title)
    plt.legend()
    plt.xlabel('Problem size (number of vertices)')
    plt.ylabel('Objective value')
    if title:
        plt.savefig(('plots/' + filename + '.png'), format='png', dpi=800)
    else:
        plt.show()


# def plotClusters(clusters, centroids, title, filename):
#     """
#     Plots the graphs of clustering

#     @param centroids (list[dict]): a list of dictionary t with pair (t['x'], t['y']) which represents
#     the coordinates (x, y) of a centroid

#     @param clusters (list[list[dict]]): each innermost list represent a cluster and its tuples
#     are the coordinates of its points. Note that clusters[i] cluster has centroid centroid[i]

#     @param title (str): a string with clustering title

#     @param filename (str): a string with filename to save png plot (empty => do not save)
#     """

#     print("Generazione dei grafici del clustering nella cartella plots/")

#     k = len(clusters)

#     plt.clf()
#     colors = cm.rainbow(np.linspace(0, 1, k))
#     plt.xticks([])
#     plt.yticks([])
#     img = mpimg.imread('img/USA_Counties.png')
#     for cluster, centroid, color in zip(clusters, centroids, colors):
#         x_list = []
#         y_list = []
#         # Draw lines from centroid to each point (county)
#         for county in cluster:
#             xs = [county['x'], centroid['x']]
#             ys = [county['y'], centroid['y']]
#             x_list.append(xs)
#             y_list.append(ys)
#             plt.plot(xs, ys, color=color, linewidth=0.5, zorder=2)
#             plt.plot(xs, ys, color='k', linewidth=0.75, zorder=1)
#         # Draw each cluster point (county)
#         plt.scatter(x_list, y_list, s=0.80, color=color,  # s is point size
#                     alpha=0.5, edgecolors='k', linewidths=0.5, zorder=4)
#         # Draw centroid of cluster
#         plt.scatter(centroid['x'], centroid['y'], s=50, color=color, edgecolors='k',
#                     linewidths=0.75, marker='H', zorder=3)
#     plt.title(title)
#     img = plt.imshow(img)
#     if filename:
#         plt.savefig(('plots/' + filename + '.png'), format='png', dpi=800)

    # plt.show()
