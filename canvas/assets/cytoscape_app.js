function query_graph() {
  xhr = new XMLHttpRequest();
  xhr.open('GET', './graph', true);
  xhr.onreadystatechange = function() {
    if(xhr.readyState === 4) {
      cyInstance.json(JSON.parse(xhr.responseText));
      cyInstance.elements().layout({name: layout_name, fit: first_run}).run();
      first_run = false;
    }
  }
  xhr.send();
}

function view_position() {
console.log('here')
   layout_name = 'preset';
   first_run = true;
  query_graph();
}

function view_tree() {
console.log('there')
   layout_name = 'dagre';
   first_run = true;
  query_graph();
}


var cyInstance = new cytoscape({
  container: document.getElementById('cy'),
  style: [{selector: 'node', style: {'background-color': '#666','label': 'data(id)'}},
    {selector: 'edge',style: {'width': 5,'line-color': '#ccc','target-arrow-color': '#ccc',
        'target-arrow-shape': 'triangle','arrow-scale': 5}}]});

var first_run = true;
var layout_name = 'preset';
query_graph();
setInterval(query_graph, 1000);
