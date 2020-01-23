#ifndef PYTHON_ADAPTER
#define PYTHON_ADAPTER

#include <assert.h>
#include <stdlib.h>
#include <string>
#ifdef _WIN32
#include <Python.h>
#else
#include <python2.7/Python.h>
#endif
#include <vector>

using std::pair;
using std::vector;

#ifdef _WIN32
// Windows setenv function conversion
int setenv(const char* name, const char* value, int overwrite) {
  return _putenv_s(name, value);
}
#endif

void initPython() {
  // Set up python program
  setenv("PYTHONPATH", "src/", 1);
  Py_SetProgramName(const_cast<char*>("Plotting function"));  // Py 2.7
  // std::string name("Plotting function");  // Py 3.6
  // Py_SetProgramName(
  //     const_cast<wchar_t*>(std::wstring(name.begin(), name.end()).c_str()));
}

void plot_times(const vector<pair<unsigned int, double>>& opti,
                const vector<pair<unsigned int, double>>& heur,
                const std::string& title, const std::string& filename) {
  // Prepare optimal times
  PyObject* py_opt_times = PyList_New(opti.size());
  for (unsigned int i = 0; i < opti.size(); ++i) {
    PyObject* tuple = Py_BuildValue("(id)", opti[i].first, opti[i].second);
    // Add tuple to list
    PyList_SET_ITEM(py_opt_times, i, tuple);
  }

  // Prepare heur times
  PyObject* py_heur_times = PyList_New(heur.size());
  for (unsigned int i = 0; i < heur.size(); ++i) {
    PyObject* tuple = Py_BuildValue("(id)", heur[i].first, heur[i].second);
    // Add tuple to list
    PyList_SET_ITEM(py_heur_times, i, tuple);
  }

  // Load the module object
  PyObject* pModule = PyImport_ImportModule("plot_script");
  // if (pModule == nullptr) {
  //   PyErr_Print();
  //   std::exit(1);
  // }

  assert(pModule != nullptr);

  // Get plot function reference
  PyObject* pFunc = PyObject_GetAttrString(pModule, "plotTimes");
  assert(pFunc != nullptr);

  assert(PyCallable_Check(pFunc));  // Function must be callable

  // Prepare string with name and save file name
  PyObject* py_title = Py_BuildValue("s#", title.c_str(), title.size());
  PyObject* py_fname = Py_BuildValue("s#", filename.c_str(), filename.size());

  // Call plot function
  PyObject* fun = PyObject_CallFunctionObjArgs(
      pFunc, py_opt_times, py_heur_times, py_title, py_fname, NULL);
}

void plot_objvalues(const vector<pair<unsigned int, double>>& opti,
                    const vector<pair<unsigned int, double>>& heur,
                    const std::string& title, const std::string& filename) {
  // Prepare optimal obj values
  PyObject* py_opt_val = PyList_New(opti.size());
  for (unsigned int i = 0; i < opti.size(); ++i) {
    PyObject* tuple = Py_BuildValue("(id)", opti[i].first, opti[i].second);
    // Add tuple to list
    PyList_SET_ITEM(py_opt_val, i, tuple);
  }

  // Prepare heur obj values
  PyObject* py_heur_val = PyList_New(heur.size());
  for (unsigned int i = 0; i < heur.size(); ++i) {
    PyObject* tuple = Py_BuildValue("(id)", heur[i].first, heur[i].second);
    // Add tuple to list
    PyList_SET_ITEM(py_heur_val, i, tuple);
  }

  // Load the module object
  PyObject* pModule = PyImport_ImportModule("plot_script");
  /*if (pModule == nullptr) {
    PyErr_Print();
    std::exit(1);
  }*/

  assert(pModule != nullptr);

  // Get plot function reference
  PyObject* pFunc = PyObject_GetAttrString(pModule, "plotError");
  assert(pFunc != nullptr);

  assert(PyCallable_Check(pFunc));  // Function must be callable

  // Prepare string with name and save file name
  PyObject* py_title = Py_BuildValue("s#", title.c_str(), title.size());
  PyObject* py_fname = Py_BuildValue("s#", filename.c_str(), filename.size());

  // Call plot function
  PyObject* fun = PyObject_CallFunctionObjArgs(pFunc, py_opt_val, py_heur_val,
                                               py_title, py_fname, NULL);
}

// void plotClusters(const vector<vector<County>>& counties,
//                   const vector<pair<int, Coordinate>>& centroids,
//                   const std::string& title = "",
//                   const std::string& fname = "") {
//   // PyGILState_STATE gstate = PyGILState_Ensure();

//   // Prepare a list of list with representing the list of clusters
//   PyObject *py_clusters = nullptr, *py_cluster = nullptr;
//   py_clusters = PyList_New(counties.size());
//   for (int i = 0; i < counties.size(); ++i) {
//     py_cluster = PyList_New(counties[i].size());
//     for (int j = 0; j < counties[i].size(); ++j) {
//       PyList_SET_ITEM(py_cluster, j,
//                       Py_BuildValue("{s:d,s:d}", "x", counties[i][j].getX(),
//                                     "y", counties[i][j].getY()));
//     }
//     PyList_SET_ITEM(py_clusters, i, py_cluster);
//   }

//   // Prepare a list with centroids (tuple of coords)
//   PyObject* py_centroids = PyList_New(centroids.size());
//   for (int i = 0; i < centroids.size(); ++i) {
//     PyObject* centroids_coords =
//         Py_BuildValue("{s:d,s:d}", "x", centroids[i].second.getX(), "y",
//                       centroids[i].second.getY());
//     // Add tuple to list
//     PyList_SET_ITEM(py_centroids, i, centroids_coords);
//   }

//   // PyGILState_Release(gstate);

//   // Load the module object
//   PyObject* pModule = PyImport_ImportModule("plot_script");
//   /*if (pModule == nullptr) {
//     PyErr_Print();
//     std::exit(1);
//   }*/

//   assert(pModule != nullptr);

//   // Get plot function reference
//   PyObject* pFunc = PyObject_GetAttrString(pModule, "plotClusters");
//   assert(pFunc != nullptr);

//   assert(PyCallable_Check(pFunc));  // Function must be callable

//   // Prepare string with name and save file name
//   PyObject* py_title = Py_BuildValue("s#", title.c_str(), title.size());
//   PyObject* py_fname = Py_BuildValue("s#", fname.c_str(), fname.size());

//   // Call plot function
//   PyObject* fun = PyObject_CallFunctionObjArgs(pFunc, py_clusters,
//   py_centroids,
//                                                py_title, py_fname, NULL);
// }

#endif /* PYTHON_ADAPTER */
