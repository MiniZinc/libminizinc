const HTML_COLORS = ['red', 'yellow', 'blue', 'lime', 'magenta', 'cyan', 'orange'];

(async function() {
  // getUserData can be used to retrieve the JSON data passed to ide_launch_server()
  const userData = await MiniZincIDE.getUserData();
  // Get element for each REGION
  const regions = userData.regions.map(r => document.getElementById(r.toLowerCase()));
  // Create a mapping between COLOR enum name and HTML color
  const colors = {};
  userData.colors.forEach((c, i) => {
    colors[c] = HTML_COLORS[i % HTML_COLORS.length]
  });

  // Handler to set the colors for the solution
  function setSolution(data) {
    data.forEach((c, i) => {
      regions[i].setAttribute('fill', colors[c]);
    });
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
