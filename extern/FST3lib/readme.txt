Feature Selection Toolbox 3 (FST3) is a standalone C++ library for feature 
selection (also known as attribute or variable selection), capable of reducing 
problem dimensionality to maximize the accuracy of data models, performance of 
automatic decision rules as well as to reduce data acquisition cost.

See http://fst.utia.cz for more information on FST3 and on the feature selection 
problem.


--------------------------------------------------------------------------------
Copyright:
* FST3 software (with exception of any externally linked libraries) is 
  copyrighted by Institute of Information Theory and Automation (UTIA), Academy of 
  Sciences of the Czech Republic. * FST3 source codes as presented here do not 
  contain code of third parties. FST3 may need linkage to external libraries to 
  exploit its functionality in full. For details on obtaining and possible usage 
  restrictions of external libraries follow their original sources (referenced 
  from FST3 documentation wherever applicable). * FST3 software is available free 
  of charge for non-commercial use. Please address all inquires concerning 
  possible commercial use of FST3, or if in doubt, to FST3 maintainer in charge 
  (see Contacts) * Derivative works based on FST3 are permitted as long as they 
  remain non-commercial only. * Re-distribution of FST3 software is not allowed 
  without explicit consent of the copyright holder.

Disclaimer of Warranty:
* FST3 software is presented "as is", without warranty of any kind, either 
  expressed or implied, including, but not limited to, the implied warranties of 
  merchantability and fitness for a particular purpose. The entire risk as to the 
  quality and performance of the program is with you. Should the program prove 
  defective, you assume the cost of all necessary servicing, repair or correction.
  
Limitation of Liability:
* The copyright holder will in no event be liable to you for damages, including 
  any general, special, incidental or consequential damages arising out of the use 
  or inability to use the code (including but not limited to loss of data or data 
  being rendered inaccurate or losses sustained by you or third parties or a 
  failure of the program to operate with any other programs).


--------------------------------------------------------------------------------
Usage:

See source files demo??.cpp for commented library usage examples.
All demos can be compiled at once using 'make demos_all', 
then run successively using 'rundemos.sh'.
Note that the provided sample output ('rundemos.log') may differ from
what you get wherever randomization is involved.


--------------------------------------------------------------------------------
Compilation:

Linux / Cygwin:
* FST3 compiles under Linux (tested with gcc 4.x under Redhat and Cygwin). Boost 
  library must be available, but it is likely to be present in all Linux and 
  Cygwin current distributions by default. FST3 has been tested with Boost 
  versions 1.33.1 and 1.44. LibSVM library is needed to compile some of the demos; 
  if needed, download the library from its source web (for link see 
  http://fst.utia.cz/?fst3_down) and follow instructions in FST3 makefile.
  
Windows / Visual C++:
* FST3 compiles under Visual C++ (tested with version 2010). If not present, first 
  install the Boost library. The easiest way is to use the Windows installer 
  available at http://www.boostpro.com/download/. FST3 has 
  been tested with Boost versions 1.33.1 and 1.44. In Visual C++ create Empty 
  Project and manually Add Existing header files (hpp) and source files (cpp). Add 
  only one demo cpp at a time. Open Properties (Project Pages) dialog and for each 
  configuration (Debug, Release) do the following: in Configuration Properties -
  C/C++ - General add to Additional Include Directories the path to the directory 
  where Boost is installed (typically c:\program files\boost\boost_1_x). Add 
  the same path in Configuration Properties - Linker - General to Additional 
  Library Directories. In case of threaded demos add FST_THREADED to Preprocessor
  Definitions under Configuration Properties - C/C++ - Preprocessor. In case
  of debug target add DEBUG therein, in case of the release target add NDEBUG. 
  LibSVM library is needed to compile some  of the demos; if needed, download 
  the library from its source web (for link to LibSVM site see http://fst.utia.cz/?fst3_down) 
  and add the needed source codes to the project.
  Note: VC produces "unsafe/deprecated" warnings related solely to TRN and ARFF file format 
  filters because of the old C-style input output calls (fscanf, strtok etc.).
  These compiler messages can be switched off by defining _CRT_SECURE_NO_WARNINGS
  without any negative consequence. These filters are considered to be of relatively 
  minor importance; our effort is focused primarily on the core FST codes.
  Note: some of the normal model related sources use M_PI. Visual C++ may not
  by default have it defined. If this is the case, add _USE_MATH_DEFINES to Preprocessor
  Definitions under Configuration Properties - C/C++ - Preprocessor.
  


--------------------------------------------------------------------------------
Thread safety:

Please note that the library as a whole in not thread safe. Only those portions
of the library that are used inside threaded feature selection routines are thread safe.
These include mostly classes derived from Clonable, in particular Criterion
and Data_Accessor derived and related code.


--------------------------------------------------------------------------------
Known problems:

- In rare occurences the Divergence criterion on some data breaches monotonicity 
  (adding a feature to a subset causes Divergence to decrease). This is undesirable
  in connection with Branch & Bound. The reason is numerical instability of
  matrix operations on some data, where values become too large or too small.
  There is currently no workaround, except the recommendation to prefer
  Bhattacharyya distance, where this problem could not be observed.
  The Bhattacharyya implementation has been verified as correct by comparison with an
  unrelated library.
  
  Remark: numerical problems are expectable with probabilistic criteria in general
  when the number of training samples in data is insufficient with respect to
  dimensionality. This is a theoretical problem - the only reasonable workaround is
  obtaining more training samples.

- an uncatchable crash has been observed when launching Boost threads on a very-high-dimensional
  task with very complex criterion function (function with large memory footprint) 
  due to insufficient system memory on one particular system setup. The same could not
  be reproduced when with different Boost version which correctly throws bad_alloc.

- SVM training/testing freezes on some data. LINEAR and SIGMOID kernels seem to be
  particularly affected, RBF kernel is least affected. Evaluating individual features
  seems to be more prone to freeze than evaluating feature groups. Note that this
  problem could not be located in FST codes and is almost certainly the problem in LibSVM.
  
- Textual output of time consuming multi-threaded searches may occassionally stall on some
  systems after a long period of time (has been observed with Search_Monte_Carlo_Threaded
  and Search_Exhaustive_Threaded). The search process in such case continues correctly
  but does not print any textual output any more. The easiest workaround is to
  switch textual output off using set_output_detail(FST::SILENT); the output stream is
  then not used in the course of search, only at the end to report final results.
  Or, redirect final results reporting to a different stream then the one used in the course
  of search for status reporting.


--------------------------------------------------------------------------------
Version history:

3.1.1.beta
- fixed a bug in Sparse ARFF filter that prevented correct Sparse ARFF files from being read
- standard ARFF filter is now less sensitive to unusually formatted headers and accepts more ARFF
  files straight away

3.1.0.beta
- optimal Branch & Bound methods
  + BBB, Basic Branch & Bound
  + IBB, Improved Branch & Bound
  + BBPP, Branch & Bound with Partial Prediction (averaging predictor)
  + FBB, Fast Branch & Bound (averaging predictor)
- DAF, Dependency-Aware Feature Ranking, is a new highly efficient method for very-high-dimensional 
  FS; unlike BIF it does not ignore contextual information and, consequently, is capable of 
  yielding considerably better results (enables wrapper-based feature selection with dimensionality 
  in the order of 10^5-10^6; works with arbitrary wrapper)
  + DAF0 (standard)
  + DAF1 (normalized)
- SFRS/SBRS - Sequential Retreating Search algorithm is related to Floating Search but 
  more thorough. Suitable also for use with secondary criterion (result regularization)
- 'generalized' variants of all sequential methods enabling more thorough search by testing 
  feature g-tuples instead of just single features per step (see 1982 Devijver Kittler book)
  + (G)SFS, (G)SBS, 
  + (G)SFFS, (G)SBFS, 
  + (G)OS, 
  + (G)DOS
  + (G)SFRS, (G)SBRS
- all sequential methods now allow start from arbitrary subset (useful for tuning
  of results using several different methods)
- individual ranking (BIF) threaded implementation (handy in very high dimensional tasks)
- Monte Carlo and threaded Monte Carlo method selects the best from a random sequence of feature subsets
- SFS/SBS, SFFS/SBFS, and SFRS/SBRS now enable post-search retreival of best results of each subset size
  as observed in the course of search
- modified SFFS implementation to fit more closely the original definition (now runs faster)
- re-implmented threading in sequential methods now more efficient due to reduced number of thread creations/destructions
- search method output is now redirectable to arbitrary output stream
- search method output can be switched off (introduced output levels SILENT, NORMAL, DETAILED)
- improved result trackers (cloning, joining, etc.)
- arbitrary data part access substitution (TEST for TRAIN, etc.) to enable bias estimation
- bias estimating wrapper
- cleaner stopwatch implementation
- now permits missing values in data - such values are substituted per feature by the mean value over valid values
- classifiers now implement method classify(), enabling classification of an arbitrary sample
- refactored directory structure
- lots of new demos showing broader variety of usage scenarios
- demos grouped according to purpose (for easier orientation especially of novice users)
- various minor improvements and additions (e.g., alternative random initialization of subsets, etc.)
- corrected several bugs and minor issues
  

3.0.2.beta
- added Exhaustive Search procedure in both sequential and threaded
  implementations to enable optimal feature selection
- corrected minor issues to support LibSVM 3.0
- result trackers now support cloning and memory usage limits
- added logfile with captured output of all demos
  for verification purposes (rundemos.log)
- corrected several minor issues

3.0.1.beta
- added support for reading ARFF (Waikato Weka) data files
- corrected minor issues to enable compilation in Visual C++

3.0.0.beta
- initial public release
- templated C++ code, using Boost library
- feature selection criteria
  ~ classification accuracy estimation based (wrappers), see data access options below
    + normal Bayes classifier
    + k-Nearest Neighbor classifier (based on various L-distances)
    + Support Vectior Machine (optional, depends on external LibSVM library)
  ~ normal model based (filter)
    + Bhattacharyya distance
    + Divergence
    + Generalized Mahalanobis distance
  ~ multinomial model based (filter) - Bhattacharyya, Mutual Information
  ~ criteria ensembles
  ~ hybrids
- feature selection methods
  ~ ranking (BIF, best individual features)
  ~ sequential search (hill-climbing)
    + sequential selection (SFS/SBS, restricted/unrestricted)
    + floating search (SFFS/SBFS, restricted/unrestricted)
    + oscillating search (OS, deterministic, randomized, restricted/unrestricted)
    + dynamic oscillating search (DOS, deterministic, randomized, restricted/unrestricted)
    + in any of the above: threaded, sequential, hybrid or ensemble based feature preference evaluation
  ~ supporting techniques (freely combinable with methods above)
    + subset size optimization vs. subset size as user parameter
    + result regularization (preference of solutions with slightly lower criterion value to counter over-fitting)
    + feature acquisition cost minimization
    + feature selection process stability evaluation
    + two-process similarity evaluation (to determine impact of parameter change etc.)
- flexible data processing
  ~ nested multi-level sampling (splitting to training, valitation, test and possibly other data parts)
  ~ sampling through extendable objects (includes re-substitution, cross-valiation, hold-out, leave-one-out, random sampling, etc.)
  ~ normalization through extendable objects (interval shrinking, whitening)
  ~ support for textual flat data format TRN (see FST1)
