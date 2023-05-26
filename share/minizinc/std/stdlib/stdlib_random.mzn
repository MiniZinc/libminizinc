/***
  @groupdef stdlib.random Random Number Generator builtins

 These functions implement random number generators from different 
 probability distributions.
*/

/** @group stdlib.random Return a sample from the normal distribution defined by \(\a mean, \a std\) */
function float: normal(float: mean, float: std);  

/** @group stdlib.random Return a sample from the normal distribution defined by \(\a mean, \a std\) */
function float: normal(int: mean, float: std);  

/** @group stdlib.random Return a sample from the uniform distribution defined by \(\a lowerbound, \a upperbound\) */
function float: uniform(float: lowerbound, float: upperbound);  

/** @group stdlib.random Return a sample from the uniform distribution defined by \(\a lowerbound, \a upperbound\) */
function int: uniform(int: lowerbound, int: upperbound);  

/** @group stdlib.random Return a sample from the uniform distribution defined by \a S */
function int: uniform(set of int: S) =
  if card(S) == max(S) - min(S) + 1 then
    uniform(min(S),max(S))
  else
    [ i | i in S ][uniform(1,card(S))]
  endif;

/** @group stdlib.random Return a sample from the poisson distribution defined by \a mean */
function int: poisson(float: mean);

/** @group stdlib.random Return a sample from the poisson distribution defined by an integer \a mean */
function int: poisson(int: mean);

/** @group stdlib.random Return a sample from the gamma distribution defined by \(\a alpha, \a beta\) */
function float: gamma(float: alpha, float: beta);

/** @group stdlib.random Return a sample from the gamma distribution defined by \(\a alpha, \a beta\) */
function float: gamma(int: alpha, float: beta);

/** @group stdlib.random Return a sample from the Weibull distribution defined by \(\a shape, \a scale\) */
function float: weibull(float: shape, float: scale);

/** @group stdlib.random Return a sample from the Weibull distribution defined by \(\a shape, \a scale\) */
function float: weibull(int: shape, float: scale);

/** @group stdlib.random Return a sample from the exponential distribution defined by \(\a lambda\) */
function float: exponential(int: lambda);

/** @group stdlib.random Return a sample from the exponential distribution defined by \(\a lambda\) */
function float: exponential(float: lambda);

/** @group stdlib.random Return a sample from the lognormal distribution defined by \(\a mean, \a std\) */
function float: lognormal(float: mean, float: std);  

/** @group stdlib.random Return a sample from the lognormal distribution defined by \(\a mean, \a std\) */
function float: lognormal(int: mean, float: std);  

/** @group stdlib.random Return a sample from the chi-squared distribution defined by the degree of freedom \(\a n\) */
function float: chisquared(int: n);

/** @group stdlib.random Return a sample from the chi-squared distribution defined by the degree of freedom \(\a n\) */
function float: chisquared(float: n);

/** @group stdlib.random Return a sample from the cauchy distribution defined by \(\a mean, \a scale\) */
function float: cauchy(float: mean, float: scale);

/** @group stdlib.random Return a sample from the cauchy distribution defined by \(\a mean, \a scale\) */
function float: cauchy(int: mean, float: scale);

/** @group stdlib.random Return a sample from the Fisher-Snedecor F-distribution defined by the degrees of freedom \(\a d1, \a d2\) */
function float: fdistribution(float: d1, float: d2);

/** @group stdlib.random Return a sample from the Fisher-Snedecor F-distribution defined by the degrees of freedom \(\a d1, \a d2\) */
function float: fdistribution(int: d1, int: d2);

/** @group stdlib.random Return a sample from the student's t-distribution defined by the sample size \(\a n\) */
function float: tdistribution(float: n);

/** @group stdlib.random Return a sample from the student's t-distribution defined by the sample size \(\a n\) */
function float: tdistribution(int: n);

/** @group stdlib.random Return a sample from the discrete distribution defined by the array of weights \(\a weights\) that assigns a weight to each integer starting from zero */
function int: discrete_distribution(array[int] of int: weights);

/** @group stdlib.random Return a boolean sample from the Bernoulli distribution defined by probability \(\a p\) */
function bool: bernoulli(float: p);

/** @group stdlib.random Return a sample from the binomial distribution defined by sample number \a t and probability \a p */
function int: binomial(int: t, float: p);  

