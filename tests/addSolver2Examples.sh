# Use this template to add a new line in each example with another test solver
# Should be run from a subdir
# sed -i .bak creates a backup
grep -rl 'mzn20_fd' * | xargs --verbose sed  -i.bak 's/\bmzn20_fd\b/mzn20_fd\n% RUNS ON mzn-fzn_fd/g'
