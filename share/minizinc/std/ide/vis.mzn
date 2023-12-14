
/***
    @groupdef ide MiniZincIDE tools

    This library contains features designed for use from within the MiniZinc IDE.
    These are not guaranteed to work when run outside the IDE.
*/

/***
    @groupdef ide.visualisation MiniZincIDE solution visualisation tools

    Provides functionality for web-based solution visualisation.

    All predefined visualisations are designed to used from :mzn:`output` items:

    .. code-block:: minizinc

      var 1..10: x;
      output vis_line(x, "x-value");

    Multiple such output items can be used in a single model to open multiple visualisations concurrently.

    Custom visualisations can be created using the :mzn:`vis_server` function:

    .. code-block:: minizinc

      int: n;
      var 1..10: x;
      output :: vis_server("my-vis.html", (n: n)) (x: x);
    
    The file ``my-vis.html`` will be opened and the MiniZinc IDE JavaScript API can be used to listen for events
    and retrieve solutions. For more information, see :ref:`ch-visualisation`.
*/

/** @group ide.visualisation
    Launch a visualisation server for a custom visualisation using the given \a file. */
function ann: vis_server(string: file) = vis_server(file, <>);

/** @group ide.visualisation
    Launch a visualisation server for a custom visualisation using the given \a file.
    The \a user_data value can be retrieved in the page using ``MiniZincIDE.getUserData()``. */
function ann: vis_server(string: file, opt $T: user_data) = mzn_output_section(mzn_vis_server(file, user_data), true);

/** @group ide.visualisation
    Launch a visualisation server for a custom visualisation using the given \a file.
    The \a user_data value can be retrieved in the page using ``MiniZincIDE.getUserData()``. */
function ann: vis_server(string: file, array [$X] of opt $T: user_data) = mzn_output_section(mzn_vis_server(file, user_data), true);

function string: mzn_vis_server(string: file, opt $T: user_data) = let {
  int: i = mzn_increment_counter("mzn_vis");
  string: section = "mzn_vis_\(i)";
  constraint trace_to_json_section(section, (
    url: file,
    userData: user_data
  ));
} in section;

function string: mzn_vis_server(string: file, array [$X] of opt $T: user_data) = let {
  int: i = mzn_increment_counter("mzn_vis");
  string: section = "mzn_vis_\(i)";
  constraint trace_to_json_section(section, (
    url: file,
    userData: user_data
  ));
} in section;

/** @group ide.visualisation 
    Add a line graph of \a x with respect to solution time, labelling the trace \a label.
*/
function ann: vis_line(var int: x, string: label) = vis_line([x], [label]);
/** @group ide.visualisation 
    Add a line graph of \a x with respect to solution time, labelling the trace \a label.
*/
function ann: vis_line(var float: x, string: label) = vis_line([x], [label]);

/** @group ide.visualisation 
    Add line graphs of the values in \a x with respect to solution time on the same axes, labelling the series using their index.
*/
function ann: vis_line(array [$$E] of var int: x) = vis_line(x, [i: show(i) | i in index_set(x)]);
/** @group ide.visualisation 
    Add line graphs of the values in \a x with respect to solution time on the same axes, labelling the series using their index.
*/
function ann: vis_line(array [$$E] of var float: x) = vis_line(x, [i: show(i) | i in index_set(x)]);

/** @group ide.visualisation 
    Add line graphs of the values in \a x with respect to solution time on the same axes, labelling the series with \a series_labels.
*/
function ann: vis_line(array [$$E] of var int: x, array [$$E] of string: series_labels) =
    let {
        constraint assert(
            index_sets_agree(x, series_labels), 
            "index set of series labels doesn't match x"
        );
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_scatter.html", (
                'type': "line-time",
                seriesLabels: series_labels
            )),
            x
        );
    } in output_only;

/** @group ide.visualisation 
    Add line graphs of the values in \a x with respect to solution time on the same axes, labelling the series with \a series_labels.
*/
function ann: vis_line(array [$$E] of var float: x, array [$$E] of string: series_labels) =
    let {
        constraint assert(
            index_sets_agree(x, series_labels), 
            "index set of series labels doesn't match x"
        );
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_scatter.html", (
                'type':  "line-time",
                seriesLabels: series_labels
            )),
            x
        );
    } in output_only;

/** @group ide.visualisation 
    Add a line graph of \a x against \a y, labelling the axes \a x_label and \a y_label respectively. */
function ann: vis_line(
    array [$$E] of var int: x,
    array [$$E] of var int: y,
    string: x_label,
    string: y_label
) = let {
        constraint assert(
            index_sets_agree(x, y), 
            "x and y must have same index sets"
        );
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_scatter.html", (
                'type': "line",
                seriesLabels: [concat(["(", x_label, ",", y_label, ")"])],
                xLabel: x_label,
                yLabel: y_label
            )),
            [(x: x, y: y)]
        );
    } in output_only;
/** @group ide.visualisation 
    Add a line graph of \a x against \a y, labelling the axes \a x_label and \a y_label respectively. */
function ann: vis_line(
    array [$$E] of var float: x,
    array [$$E] of var float: y,
    string: x_label,
    string: y_label
) = let {
        constraint assert(
            index_sets_agree(x, y), 
            "x and y must have same index sets"
        );
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_scatter.html", (
                'type': "line",
                seriesLabels: [concat(["(", x_label, ",", y_label, ")"])],
                xLabel: x_label,
                yLabel: y_label
            )),
            [(x: x, y: y)]
        );
    } in output_only;

/** @group ide.visualisation 
    Add a scatter plot of \a x against \a y, labelling the axes \a x_label and \a y_label respectively. */
function ann: vis_scatter(
    array [$$E] of var int: x,
    array [$$E] of var int: y,
    string: x_label,
    string: y_label
) = let {
        constraint assert(
            index_sets_agree(x, y), 
            "x and y must have same index sets"
        );
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_scatter.html", (
                'type': "scatter",
                seriesLabels: [concat(["(", x_label, ",", y_label, ")"])],
                xLabel: x_label,
                yLabel: y_label
            )),
            [(x: x, y: y)]
        );
    } in output_only;
/** @group ide.visualisation 
    Add a scatter plot of \a x against \a y, labelling the axes \a x_label and \a y_label respectively. */
function ann: vis_scatter(
    array [$$E] of var float: x,
    array [$$E] of var float: y,
    string: x_label,
    string: y_label
) = let {
        constraint assert(
            index_sets_agree(x, y), 
            "x and y must have same index sets"
        );
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_scatter.html", (
                'type': ("scatter"),
                seriesLabels: [concat(["(", x_label, ",", y_label, ")"])],
                xLabel: x_label,
                yLabel: y_label
            )),
            [(x: x, y: y)]
        );
    } in output_only;

/** @group ide.visualisation 
    Create a scatter-plot visualisation.
    The point (\a x, \a y) is added to the plot for each solution.

    @param x: the x coordinate
    @param y: the y coordinate
    @param x_label: the x-axis label
    @param y_label: the y-axis label
    @param series_label: the series label
 */
function ann: vis_scatter_cumulative(
    var int: x,
    var int: y,
    string: x_label,
    string: y_label,
    string: series_label) =
    vis_scatter_cumulative([x], [y], x_label, y_label, [series_label]);
/** @group ide.visualisation 
    Create a scatter-plot visualisation.
    The point (\a x, \a y) is added to the plot for each solution.

    @param x: the x coordinate
    @param y: the y coordinate
    @param x_label: the x-axis label
    @param y_label: the y-axis label
    @param series_label: the series label
 */
function ann: vis_scatter_cumulative(
    var float: x,
    var float: y,
    string: x_label,
    string: y_label,
    string: series_label
) = vis_scatter_cumulative([x], [y], x_label, y_label, [series_label]);

/** @group ide.visualisation 
    Create a scatter-plot visualisation with multiple series.
    The point (\a x[i], \a y[i]) for each series \a i is added to the plot for each solution.
    Series are labelled using the index set of \a x and \a y.

    @param x: the x coordinate for each series
    @param y: the y coordinate for each series
    @param x_label: the x-axis label
    @param y_label: the y-axis label
 */
function ann: vis_scatter_cumulative(
    array [$$E] of var int: x,
    array [$$E] of var int: y,
    string: x_label,
    string: y_label
) = vis_scatter_cumulative(x, y, x_label, y_label, [i: show(i) | i in index_set(x)]);
/** @group ide.visualisation 
    Create a scatter-plot visualisation with multiple series.
    The point (\a x[i], \a y[i]) for each series \a i is added to the plot for each solution.
    Series are labelled using the index set of \a x and \a y.

    @param x: the x coordinate for each series
    @param y: the y coordinate for each series
    @param x_label: the x-axis label
    @param y_label: the y-axis label
 */
function ann: vis_scatter_cumulative(
    array [$$E] of var float: x,
    array [$$E] of var float: y,
    string: x_label,
    string: y_label
) = vis_scatter_cumulative(x, y, x_label, y_label, [i: show(i) | i in index_set(x)]);

/** @group ide.visualisation 
    Create a scatter-plot visualisation with multiple series.
    The point (\a x[i], \a y[i]) for each series \a i is added to the plot for each solution.

    @param x: the x coordinate for each series
    @param y: the y coordinate for each series
    @param x_label: the x-axis label
    @param y_label: the y-axis label
    @param series_labels: the label for each series
 */
function ann: vis_scatter_cumulative(
    array [$$E] of var int: x,
    array [$$E] of var int: y,
    string: x_label,
    string: y_label,
    array [$$E] of string: series_labels
) = let {
        constraint assert(
            index_sets_agree(x, y), 
            "x and y must have same index sets"
        );
        constraint assert(
            index_sets_agree(x, series_labels), 
            "index set of series labels doesn't match x and y" 
        );
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_scatter.html", (
                'type': "scatter-cumulative",
                seriesLabels: series_labels,
                xLabel: x_label,
                yLabel: y_label
            )),
            [( x: x[i], y: y[i] ) | i in index_set(x)]
        );
    } in output_only;
/** @group ide.visualisation 
    Create a scatter-plot visualisation with multiple series.
    The point (\a x[i], \a y[i]) for each series \a i is added to the plot for each solution.

    @param x: the x coordinate for each series
    @param y: the y coordinate for each series
    @param x_label: the x-axis label
    @param y_label: the y-axis label
    @param series_labels: the label for each series
 */
function ann: vis_scatter_cumulative(
    array [$$E] of var float: x,
    array [$$E] of var float: y,
    string: x_label,
    string: y_label,
    array [$$E] of string: series_labels
) = let {
        constraint assert(
            index_sets_agree(x, y), 
            "x and y must have same index sets"
        );
        constraint assert(
            index_sets_agree(x, series_labels), 
            "index set of series labels doesn't match x and y" 
        );
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_scatter.html", (
                'type': "scatter-cumulative",
                seriesLabels: series_labels,
                xLabel: x_label,
                yLabel: y_label
            )),
            [( x: x[i], y: y[i] ) | i in index_set(x)]
        );
    } in output_only;

/** @group ide.visualisation 
    Visualise a bar chart of the values in \a x (labelled by index set)
 */
function ann: vis_bar(array [$$D] of var int: x) =
    vis_bar(x, [i: show(i) | i in index_set(x)]);
/** @group ide.visualisation 
    Visualise a bar chart of the values in \a x (labelled by index set)
 */
function ann: vis_bar(array [$$D] of var float: x) =
    vis_bar(x, [i: show(i) | i in index_set(x)]);

/** @group ide.visualisation 
    Visualise a bar chart of the values in \a x.
    Chart values are labelled with \a data_labels. 
 */
function ann: vis_bar(
    array [$$D] of var int: x,
    array [$$D] of string: data_labels
) = let {
        constraint assert(
            index_sets_agree(x, data_labels), 
            "x and data labels must have same index sets"
        );
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_bar.html", (
                'type': "bar",
                seriesLabels: <>,
                dataLabels: data_labels
            )),
            [(1, i): x[i]| i in index_set(x)]
        );
    } in output_only;
/** @group ide.visualisation 
    Visualise a bar chart of the values in \a x.
    Chart values are labelled with \a data_labels. 
 */
function ann: vis_bar(
    array [$$D] of var float: x,
    array [$$D] of string: data_labels
) = let {
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_bar.html", (
                'type': "bar",
                seriesLabels: <>,
                dataLabels: data_labels
            )),
            [(1, i): x[i]| i in index_set(x)]
        );
    } in output_only;

/** @group ide.visualisation 
    Visualise a bar chart with grouped data (labelled by index set).
    \a x[\p i, \p j] is the value for \p j in data series \p i.
 */
function ann: vis_bar(array [$$S, $$D] of var int: x) =
    vis_bar(x, [i: show(i) | i in index_set_1of2(x)], [i: show(i) | i in index_set_2of2(x)]);
/** @group ide.visualisation 
    Visualise a bar chart with grouped data (labelled by index set).
    \a x[\p i, \p j] is the value for \p j in data series \p i.
 */
function ann: vis_bar(array [$$S, $$D] of var float: x) =
    vis_bar(x, [i: show(i) | i in index_set_1of2(x)], [i: show(i) | i in index_set_2of2(x)]);

/** @group ide.visualisation 
    Visualise a bar chart with grouped data.
    \a x[\p i, \p j] is the value for \p j in data series \p i.
    Series are labelled with \a series_labels. 
    Chart values are labelled with \a data_labels. 
 */
function ann: vis_bar(
    array [$$S, $$D] of var int: x,
    array [$$S] of string: series_labels,
    array [$$D] of string: data_labels
) = let {
        constraint assert(index_set_1of2(x) == index_set(series_labels), "index set of series labels do not match x");
        constraint assert(index_set_2of2(x) == index_set(data_labels), "index set of data labels do not match x");
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_bar.html", (
                'type': "bar",
                seriesLabels: series_labels,
                dataLabels: data_labels
            )),
            x
        );
    } in output_only;
/** @group ide.visualisation 
    Visualise a bar chart with grouped data.
    \a x[\p i, \p j] is the value for \p j in data series \p i.
    Series are labelled with \a series_labels. 
    Chart values are labelled with \a data_labels. 
 */
function ann: vis_bar(
    array [$$S, $$D] of var float: x,
    array [$$S] of string: series_labels,
    array [$$D] of string: data_labels
) =
    let {
        constraint assert(index_set_1of2(x) == index_set(series_labels), "index set of series labels do not match x");
        constraint assert(index_set_2of2(x) == index_set(data_labels), "index set of data labels do not match x");
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_bar.html", (
                'type': "bar",
                seriesLabels: series_labels,
                dataLabels:  data_labels
            )),
            x
        );
    } in output_only;

/** @group ide.visualisation 
    Visualise a column chart of the values in \a x (labelled by index set)
 */
function ann: vis_column(array [$$D] of var int: x) =
    vis_column(x, [i: show(i) | i in index_set(x)]);
/** @group ide.visualisation 
    Visualise a column chart of the values in \a x (labelled by index set)
 */
function ann: vis_column(array [$$D] of var float: x) =
    vis_column(x, [i: show(i) | i in index_set(x)]);

/** @group ide.visualisation 
    Visualise a column chart of the values in \a x.
    Chart values are labelled with \a data_labels. 
 */
function ann: vis_column(array [$$D] of var int: x, array [$$D] of string: data_labels) =
    let {
        constraint assert(index_sets_agree(x, data_labels), "index set of data labels do not match x");
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_bar.html", (
                'type': "column",
                seriesLabels: <>,
                dataLabels: data_labels
            )), 
            [(1, i): x[i]| i in index_set(x)]
        );
    } in output_only;
/** @group ide.visualisation 
    Visualise a column chart of the values in \a x.
    Chart values are labelled with \a data_labels. 
 */
function ann: vis_column(array [$$D] of var float: x, array [$$D] of string: data_labels) =
    let {
        constraint assert(index_sets_agree(x, data_labels), "index set of data labels do not match x");
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_bar.html", (
                'type': "column",
                seriesLabels: <>,
                dataLabels: data_labels
            )),
            [(1, i): x[i]| i in index_set(x)]
        );
    } in output_only;

/** @group ide.visualisation 
    Visualise a column chart with grouped data (labelled by index set).
    \a x[\p i, \p j] is the value for \p j in data series \p i.
 */
function ann: vis_column(array [$$S, $$D] of var int: x) =
    vis_column(x, [i: show(i) | i in index_set_1of2(x)], [i: show(i) | i in index_set_2of2(x)]);
/** @group ide.visualisation 
    Visualise a column chart with grouped data (labelled by index set).
    \a x[\p i, \p j] is the value for \p j in data series \p i.
 */
function ann: vis_column(array [$$S, $$D] of var float: x) =
    vis_column(x, [i: show(i) | i in index_set_1of2(x)], [i: show(i) | i in index_set_2of2(x)]);

/** @group ide.visualisation 
    Visualise a column chart with grouped data.
    \a x[\p i, \p j] is the value for \p j in data series \p i.
    Series are labelled with \a series_labels. 
    Chart values are labelled with \a data_labels. 
 */
function ann: vis_column(
    array [$$S, $$D] of var int: x,
    array [$$S] of string: series_labels,
    array [$$D] of string: data_labels
) = let {
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_bar.html", (
                'type': "column",
                seriesLabels: series_labels,
                dataLabels: data_labels
            )),
            x
        );
    } in output_only;
/** @group ide.visualisation 
    Visualise a column chart with grouped data.
    \a x[\p i, \p j] is the value for \p j in data series \p i.
    Series are labelled with \a series_labels. 
    Chart values are labelled with \a data_labels. 
 */
function ann: vis_column(
    array [$$S, $$D] of var float: x,
    array [$$S] of string: series_labels,
    array [$$D] of string: data_labels
) = let {
        constraint assert(index_set_1of2(x) == index_set(series_labels), "index set of series labels do not match x");
        constraint assert(index_set_2of2(x) == index_set(data_labels), "index set of data labels do not match x");
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_bar.html", (
                'type': "column",
                seriesLabels: series_labels,
                dataLabels: data_labels
            )),
            x
        );
    } in output_only;

/** @group ide.visualisation 
    Visualise a Gantt chart of tasks with start times \a start and durations \a dur.
    The tasks are labelled using the index set of \a start.
 */
function ann: vis_gantt(array [$$E] of var int: start, array [$$E] of var int: dur) =
    vis_gantt(start, dur, [i: show(i) | i in index_set(start)]);

/** @group ide.visualisation 
    Visualise a Gantt chart of tasks with start times \a start and durations \a dur.
    The tasks are labelled using \a labels.
 */
function ann: vis_gantt(
    array [$$E] of var int: start,
    array [$$E] of var int: dur,
    array [$$E] of string: labels
) = let {
        constraint assert(
            index_sets_agree(start, dur) /\ index_sets_agree(start, labels),
            "index sets of start, dur, and labels must match"
        );
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_gantt.html", (
                'type': "gantt",
                labels: labels
            )),
            ( start: start, dur: dur)
        );
    } in output_only;

/** @group ide.visualisation 
    Visualise a Gantt chart of tasks with start times \a start and durations \a dur.
    The tasks are labelled using \a labels and given the colours \a colors.
 */
function ann: vis_gantt(
    array [$$E] of var int: start,
    array [$$E] of var int: dur,
    array [$$E] of string: labels,
    array [$$E] of string: colors
) = let {
        constraint assert(
            index_sets_agree(start, dur) /\ index_sets_agree(start, labels)/\ index_sets_agree(start, colors),
            "index sets of start, dur, labels, and colors must match"
        );
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_gantt.html", (
                'type': "gantt",
                labels: labels,
                colors: colors
            )),
            ( start: start, dur: dur)
        );
    } in output_only;

function ann: vis_graph_internal(
    bool: directed,
    bool: highlight,
    array [$$N] of string: node_labels,
    array [$$E] of $$N: from,
    array [$$E] of $$N: to,
    array [$$E] of string: edge_labels,
    array [$$N] of var bool: ns,
    array [$$E] of var bool: es
) = let {
        constraint assert(index_sets_agree(from, to),"vis_graph: index set of from and to must be identical");
        constraint assert(index_sets_agree(from, es),"vis_graph: index set of from and es must be identical");
        constraint assert(index_sets_agree(edge_labels, es),"vis_graph: index set of edge_labels and es must be identical");
        constraint assert(dom_array(from) subset index_set(ns),"vis_graph: nodes in from must be in index set of ns");
        constraint assert(dom_array(to) subset index_set(ns),"vis_graph: nodes in to must be in index set of ns");
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_network.html", (
                'type': if directed then "digraph" else "graph" endif,
                highlight: highlight,
                nodes: [show(i) | i in index_set(ns)],
                nodeLabels: node_labels,
                from: [show(n) | n in from],
                to: [show(n) | n in to],
                edgeLabels: edge_labels
            )),
            ( ns: ns, es: es )
        );
    } in output_only;

/** @group ide.visualisation
    Create a visualisation of the subgraph \a ns and \a es of a given undirected graph.
    The nodes are labelled using the index set of \a ns.

    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param edge_labels: labels to use for each edge
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
 */
function ann: vis_graph(
    array [$$E] of $$N: from,
    array [$$E] of $$N: to,
    array [$$E] of string: edge_labels,
    array [$$N] of var bool: ns,
    array [$$E] of var bool: es
) = vis_graph([i: show(i) | i in index_set(ns)], from, to, edge_labels, ns, es);
/** @group ide.visualisation
    Create a visualisation of the subgraph \a ns and \a es of a given undirected graph.
    The nodes are labelled using the index set of \a ns and the edges are labelled using the index
    set of \a es.

    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
 */
function ann: vis_graph(
    array [$$E] of $$N: from,
    array [$$E] of $$N: to,
    array [$$N] of var bool: ns,
    array [$$E] of var bool: es
) = vis_graph([i: show(i) | i in index_set(ns)], from, to, [i: show(i) | i in index_set(es)], ns, es);
/** @group ide.visualisation
    Create a visualisation of the subgraph \a ns and \a es of a given undirected graph.

    @param node_labels: the label for each node
    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param edge_labels: labels to use for each edge
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
 */
function ann: vis_graph(
    array [$$N] of string: node_labels,
    array [$$E] of $$N: from,
    array [$$E] of $$N: to,
    array [$$E] of string: edge_labels,
    array [$$N] of var bool: ns,
    array [$$E] of var bool: es
) = vis_graph_internal(false, false, node_labels, from, to, edge_labels, ns, es);

/** @group ide.visualisation
    Create a visualisation of the subgraph \a ns and \a es of a given undirected graph.
    The nodes are labelled using the index set of \a ns.
    The entire graph is drawn, with the given subgraph highlighted.

    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param edge_labels: labels to use for each edge
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
 */
function ann: vis_graph_highlight(
    array [$$E] of $$N: from,
    array [$$E] of $$N: to,
    array [$$E] of string: edge_labels,
    array [$$N] of var bool: ns,
    array [$$E] of var bool: es
) = vis_graph_highlight([i: show(i) | i in index_set(ns)], from, to, edge_labels, ns, es);
/** @group ide.visualisation
    Create a visualisation of the subgraph \a ns and \a es of a given undirected graph.
    The nodes are labelled using the index set of \a ns and the edges are labelled using the index
    set of \a es.
    The entire graph is drawn, with the given subgraph highlighted.

    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
 */
function ann: vis_graph_highlight(
    array [$$E] of $$N: from,
    array [$$E] of $$N: to,
    array [$$N] of var bool: ns,
    array [$$E] of var bool: es
) = vis_graph_highlight([i: show(i) | i in index_set(ns)], from, to, [i: show(i) | i in index_set(es)], ns, es);
/** @group ide.visualisation
    Create a visualisation of the subgraph \a ns and \a es of a given undirected graph.
    The entire graph is drawn, with the given subgraph highlighted.

    @param node_labels: the label for each node
    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param edge_labels: labels to use for each edge
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
 */
function ann: vis_graph_highlight(
    array [$$N] of string: node_labels,
    array [$$E] of $$N: from,
    array [$$E] of $$N: to,
    array [$$E] of string: edge_labels,
    array [$$N] of var bool: ns,
    array [$$E] of var bool: es
) = vis_graph_internal(false, true, node_labels, from, to, edge_labels, ns, es);

/** @group ide.visualisation
    Create a visualisation of the subgraph \a ns and \a es of a given directed graph.
    The nodes are labelled using the index set of \a ns.

    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param edge_labels: labels to use for each edge
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
 */
function ann: vis_digraph(
    array [$$E] of $$N: from,
    array [$$E] of $$N: to,
    array [$$E] of string: edge_labels,
    array [$$N] of var bool: ns,
    array [$$E] of var bool: es
) = vis_digraph([i: show(i) | i in index_set(ns)], from, to, edge_labels, ns, es);
/** @group ide.visualisation
    Create a visualisation of the subgraph \a ns and \a es of a given directed graph.
    The nodes are labelled using the index set of \a ns and the edges are labelled using the index
    set of \a es.

    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
 */
function ann: vis_digraph(
    array [$$E] of $$N: from,
    array [$$E] of $$N: to,
    array [$$N] of var bool: ns,
    array [$$E] of var bool: es
) = vis_digraph([i: show(i) | i in index_set(ns)], from, to, [i: show(i) | i in index_set(es)], ns, es);
/** @group ide.visualisation
    Create a visualisation of the subgraph \a ns and \a es of a given directed graph.

    @param node_labels: the label for each node
    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param edge_labels: labels to use for each edge
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
 */
function ann: vis_digraph(
    array [$$N] of string: node_labels,
    array [$$E] of $$N: from,
    array [$$E] of $$N: to,
    array [$$E] of string: edge_labels,
    array [$$N] of var bool: ns,
    array [$$E] of var bool: es
) = vis_graph_internal(true, false, node_labels, from, to, edge_labels, ns, es);

/** @group ide.visualisation
    Create a visualisation of the subgraph \a ns and \a es of a given directed graph.
    The nodes are labelled using the index set of \a ns.
    The entire graph is drawn, with the given subgraph highlighted.

    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param edge_labels: labels to use for each edge
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
 */
function ann: vis_digraph_highlight(
    array [$$E] of $$N: from,
    array [$$E] of $$N: to,
    array [$$E] of string: edge_labels,
    array [$$N] of var bool: ns,
    array [$$E] of var bool: es
) = vis_digraph_highlight([i: show(i) | i in index_set(ns)], from, to, edge_labels, ns, es);
/** @group ide.visualisation
    Create a visualisation of the subgraph \a ns and \a es of a given directed graph.
    The nodes are labelled using the index set of \a ns and the edges are labelled using the index
    set of \a es.
    The entire graph is drawn, with the given subgraph highlighted.

    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
 */
function ann: vis_digraph_highlight(
    array [$$E] of $$N: from,
    array [$$E] of $$N: to,
    array [$$N] of var bool: ns,
    array [$$E] of var bool: es
) = vis_digraph_highlight([i: show(i) | i in index_set(ns)], from, to, [i: show(i) | i in index_set(es)], ns, es);
/** @group ide.visualisation
    Create a visualisation of the subgraph \a ns and \a es of a given directed graph.
    The entire graph is drawn, with the given subgraph highlighted.

    @param node_labels: the label for each node
    @param from: the leaving node for each edge
    @param to: the entering node for each edge
    @param edge_labels: labels to use for each edge
    @param ns: a Boolean for each node whether it is in the subgraph
    @param es: a Boolean for each edge whether it is in the subgraph
 */
function ann: vis_digraph_highlight(
    array [$$N] of string: node_labels,
    array [$$E] of $$N: from,
    array [$$E] of $$N: to,
    array [$$E] of string: edge_labels,
    array [$$N] of var bool: ns,
    array [$$E] of var bool: es
) = vis_graph_internal(true, true, node_labels, from, to, edge_labels, ns, es);

/** @group ide.visualisation
    Visualise the positions of 2-dimensional objects which take a particular shape composed of rectangles. 

    @param rect_x: x-position of each rectangle
    @param rect_y: y-position of each rectangle
    @param rect_dx: width of each rectangle
    @param rect_dy: height of each rectangle
    @param shape: the rectangles which comprise each shape
    @param x: the x-position of each object
    @param y: the y-position of each object
    @param kind: which shape each object is 
 */
function ann: vis_geost_2d(
    array [$$E] of int: rect_x,
    array [$$E] of int: rect_y,
    array [$$E] of int: rect_dx,
    array [$$E] of int: rect_dy,
    array [$$K] of set of $$E: shape,
    array [$$T] of var int: x,
    array [$$T] of var int: y,
    array [$$T] of var $$K: kind
) = let {
        constraint assert(
            index_sets_agree(rect_x, rect_y) /\ index_sets_agree(rect_x, rect_dx) /\ index_sets_agree(rect_x, rect_dy),
             "first four arguments must have same index set"
        );
        constraint assert(
            index_sets_agree(x, y) /\ index_sets_agree(x, kind),
             "last three arguments must have same index set"
        );
        constraint assert(
            dom_array(kind) subset index_set(shape),
             "kind must be a subset of the index set of shape"
        );
        constraint output_to_json_section(
            mzn_vis_server("std/ide/vis_geost.html", (
                'type': "geost-2d",
                rectIndexSet: [show(i) | i in index_set(rect_x)],
                rectX: rect_x,
                rectY: rect_y,
                rectDx: rect_dx,
                rectDy: rect_dy,
                shapeIndexSet: [show(i) | i in index_set(shape)],
                shape: [(i, j): if j in shape[i] then show(j) else <> endif | i in index_set(shape), j in index_set(rect_x)]
            )),
            ( x: x, y: y, kind: [show(k) | k in kind])
        );
    } in output_only;

/** @group ide.visualisation
    Visualise the positions of 2-dimensional objects which take a particular shape composed of rectangles. 

    @param rect_size: the size of each rectangle
    @param rect_offset: the position  of each rectangle
    @param shape: the rectangles which comprise each shape
    @param x: the position of each object
    @param kind: which shape each object is
 */
function ann: vis_geost_2d(
    array[$$E, 1..2] of int: rect_size,
    array[$$E, 1..2] of int: rect_offset,
    array[$$K] of set of $$E: shape,
    array[$$T, 1..2] of var int: x,
    array[$$T] of var $$K: kind
) = vis_geost_2d(
        rect_offset[.., 1],
        rect_offset[.., 2],
        rect_size[.., 1],
        rect_size[.., 2],
        shape,
        x[.., 1],
        x[.., 2],
        kind
    );

/** @group ide.visualisation
    Visualise the positions of 2-dimensional objects which take a particular shape composed of rectangles. 

    @param rect_size: the size of each rectangle
    @param rect_offset: the position  of each rectangle
    @param shape: the rectangles which comprise each shape
    @param x: the position of each object
    @param kind: which shape each object is
 */
function ann: vis_geost_2d(
    array[$$E] of tuple(int, int): rect_size,
    array[$$E] of tuple(int, int): rect_offset,
    array[$$K] of set of $$E: shape,
    array[$$T] of tuple(var int, var int): x,
    array[$$T] of var $$K: kind
) = vis_geost_2d(
        [i: rect_offset[i].1 | i in index_set(rect_offset)],
        [i: rect_offset[i].2 | i in index_set(rect_offset)],
        [i: rect_size[i].1 | i in index_set(rect_size)],
        [i: rect_size[i].2 | i in index_set(rect_size)],
        shape,
        [i: x[i].1 | i in index_set(x)],
        [i: x[i].2 | i in index_set(x)],
        kind
    );
