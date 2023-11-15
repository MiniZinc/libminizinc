const HTML_COLORS = ['red', 'yellow', 'blue', 'lime', 'magenta', 'cyan', 'orange'];

(async function() {
  // getUserData can be used to retrieve the JSON data passed to vis_server()
  const userData = await MiniZincIDE.getUserData();
  document.getElementById('count').textContent = userData.n;
  
  // Handler to set the colors for the solution
  function setSolution(data) {
    for (const r in data) {
      document.getElementById(r).setAttribute('fill', data[r]);
    }
  }

  // Visualise last solution on startup
  const numSols = await MiniZincIDE.getNumSolutions();
  if (numSols > 0) {
    const solution = await MiniZincIDE.getSolution(numSols - 1);
    setSolution(solution.data);
  }

  // Show new solutions if we're following the latest solution
  let followLatest = true;
  MiniZincIDE.on('solution', (solution) => {
    if (followLatest) {
      setSolution(solution.data);
    }
  });

  MiniZincIDE.on('goToSolution', async (index) => {
    // Requesting index -1 turns on following latest solution
    // Otherwise, we stop showing the latest solution and show the requested one
    followLatest = index === -1;
    const solution = await MiniZincIDE.getSolution(index);
    setSolution(solution.data);
  })
})();
