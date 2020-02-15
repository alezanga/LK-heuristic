import matplotlib.pyplot as plt
import numpy as np


def plotTimes(opt_times, heur_times, title, filename):
    """
    Plots the execution times

    @param opt_times (list[tuple[int, float]]): execution times for CPLEX algorithm (first int is the problem size)

    @param heur_times (list[tuple[int, float]]): execution times for heuristic

    @param title (str): a string with plot title

    @param filename (str): a string with filename to save png plot (empty => do not save)
    """
    if (len(opt_times) != len(heur_times)):
        return None

    if filename:
        print("Generating execution time plot in folder plots/")

    plt.clf()
    opt_times.sort(key=lambda x: x[0])
    heur_times.sort(key=lambda x: x[0])
    N_opt, t_opt = zip(*opt_times)
    N_heur, t_heur = zip(*heur_times)

    plt.xticks(N_opt, N_opt, rotation=45)

    plt.plot(N_opt, t_opt, linestyle='--', marker='o', color='green',
             label='CPLEX optimal')
    plt.plot(N_heur, t_heur, linestyle='--', marker='o', color='orange',
             label='Lin-Keringhan heuristic')

    plt.title(title)
    plt.legend()
    plt.xlabel('Problem size (number of vertices)')
    plt.ylabel('Execution time (s)')
    if filename:
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
    if (len(opt_value) != len(heur_value)):
        return None

    if filename:
        print("Generating error plot in folder plots/")

    plt.clf()

    opt_value.sort(key=lambda x: x[0])
    heur_value.sort(key=lambda x: x[0])
    N_opt, v_opt = zip(*opt_value)
    _, v_heur = zip(*heur_value)

    plt.xticks(N_opt, N_opt, rotation=45)

    error_heur = list()
    valtoplot = zip(N_opt, v_heur, v_opt)
    for n, approx, opt in valtoplot:
        err_h = abs(approx - opt) / opt * 100
        error_heur.append(err_h)
        plt.annotate(("%.3f" % err_h), xy=(n, err_h), rotation=45,
                     xytext=(0, 20), ha='center', textcoords='offset points', fontsize=7)

    plt.plot(N_opt, error_heur, linestyle='--', marker='o',
             color='red', label='LK relative error (%)')

    bottom, top = plt.ylim()  # return the current ylim
    plt.ylim((bottom, top+1))

    plt.title(title)
    plt.legend()
    plt.xlabel('Problem size (number of vertices)')
    plt.ylabel('Objective value error')
    if filename:
        plt.savefig(('plots/' + filename + '.png'), format='png', dpi=800)
    else:
        plt.show()


def plotPath(coords, tour=[], filename=""):
    """
    Plots vertices and tour

    @param coords (list[tuple[float, float]]): vertices coordinates

    @param tour (list[int]): ordered indexes for tour

    @param filename (str): file where to save plot (empty => do not save and show)
    """

    if filename:
        print("Generating tour image in folder plots/")

    n = len(coords)
    plt.clf()
    plt.title("TSP path (size " + str(len(coords)) + ")")

    x, y = zip(*coords)

    plt.scatter(x, y, marker='o', s=9, color='red', zorder=0)

    if len(tour) == len(coords):
        for i in range(0, n-1):
            u = tour[i]
            v = tour[i+1]
            plt.plot([x[u], x[v]], [y[u], y[v]], 'g-', zorder=1)
        u = tour[0]
        v = tour[n-1]
        plt.plot([x[u], x[v]], [y[u], y[v]], 'g-', zorder=1)

    plt.axis('off')

    if filename:
        plt.savefig(('plots/' + filename + '.png'), format='png', dpi=800)
    else:
        plt.show()
