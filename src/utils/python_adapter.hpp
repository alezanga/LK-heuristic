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

class PyWrapper {
 public:
  PyWrapper() {
    // Set up python program
    setenv("PYTHONPATH", "src/", 1);
    Py_SetProgramName(const_cast<char*>("Plotting function"));  // Py 2.7
    // std::string name("Plotting function");  // Py 3.6
    // Py_SetProgramName(
    //     const_cast<wchar_t*>(std::wstring(name.begin(),
    //     name.end()).c_str()));
    Py_Initialize();  // Init Python interpreter
  }

  ~PyWrapper() {
    Py_Finalize();  // Undo all initializations and destroy the interpreter
  }

  void plot_times(const vector<pair<unsigned int, double>>& opti,
                  const vector<pair<unsigned int, double>>& heur,
                  const std::string& title, const std::string& filename) const {
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
    [[maybe_unused]] PyObject* fun = PyObject_CallFunctionObjArgs(
        pFunc, py_opt_times, py_heur_times, py_title, py_fname, NULL);

    // Py_DECREF(fun);
    Py_DECREF(py_fname);
    Py_DECREF(py_title);
    Py_DECREF(pFunc);
    Py_DECREF(pModule);
    Py_DECREF(py_heur_times);
    Py_DECREF(py_opt_times);
  }

  void plot_errors(const vector<pair<unsigned int, double>>& opti,
                   const vector<pair<unsigned int, double>>& heur,
                   const vector<pair<unsigned int, double>>& nn,
                   const std::string& title,
                   const std::string& filename) const {
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

    // Prepare nearest neigh obj values
    PyObject* py_nn_val = PyList_New(nn.size());
    for (unsigned int i = 0; i < nn.size(); ++i) {
      PyObject* tuple = Py_BuildValue("(id)", nn[i].first, nn[i].second);
      // Add tuple to list
      PyList_SET_ITEM(py_nn_val, i, tuple);
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
    [[maybe_unused]] PyObject* fun = PyObject_CallFunctionObjArgs(
        pFunc, py_opt_val, py_heur_val, py_nn_val, py_title, py_fname, NULL);

    // Py_DECREF(fun);
    Py_DECREF(py_fname);
    Py_DECREF(py_title);
    Py_DECREF(pFunc);
    Py_DECREF(pModule);
    Py_DECREF(py_heur_val);
    Py_DECREF(py_opt_val);
    Py_DECREF(py_nn_val);
  }

  void plot_points(const vector<pair<double, double>>& coord,
                   const vector<vertex>& path = vector<vertex>(),
                   const std::string& filename = "") const {
    PyObject* py_coord = PyList_New(coord.size());
    for (unsigned int i = 0; i < coord.size(); ++i) {
      PyObject* xy =
          Py_BuildValue("(dd)", coord.at(i).first, coord.at(i).second);
      // Add tuple to list
      PyList_SET_ITEM(py_coord, i, xy);
    }

    // bool haspath = !path.empty();
    PyObject* py_path = PyList_New(path.size());
    for (unsigned int i = 0; i < path.size(); ++i) {
      PyObject* vert = Py_BuildValue("i", path[i]);
      // Add tuple to list
      PyList_SET_ITEM(py_path, i, vert);
    }

    // Load the module object
    PyObject* pModule = PyImport_ImportModule("plot_script");

    assert(pModule != nullptr);

    // Get plot function reference
    PyObject* pFunc = PyObject_GetAttrString(pModule, "plotPath");
    assert(pFunc != nullptr);

    assert(PyCallable_Check(pFunc));  // Function must be callable

    // Prepare string with file name
    PyObject* py_fname = Py_BuildValue("s#", filename.c_str(), filename.size());

    // Call plot function
    [[maybe_unused]] PyObject* fun =
        PyObject_CallFunctionObjArgs(pFunc, py_coord, py_path, py_fname, NULL);

    Py_DECREF(py_fname);
    Py_DECREF(pFunc);
    Py_DECREF(py_path);
    Py_DECREF(py_coord);
    // Py_DECREF(fun);
    Py_DECREF(pModule);
  }
};

#endif /* PYTHON_ADAPTER */
