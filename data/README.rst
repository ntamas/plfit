Testsuite
=========

This directory contains input data for the testsuite.  In the future,
we should also write a testsuite based on the data.


Input data
----------

Below is a description of the input data for the testsuite.

* The data files ``discrete_data.txt`` and ``continuous_data.txt`` are the
  original example files from Aaron Clauset's own implementation of the
  power law fitting method. Fitting ``discrete_data.txt`` should result in
  alpha = 2.58, xmin = 2 and a log-likelihood of -9155.63. Fitting
  ``continuous_data.txt`` should yield alpha=2.53282, xmin=1.43628 and
  a log-likelihood of -9276.42.

* The data files ``celegans-indegree.dat``,
  ``celegans-outdegree.dat``, and ``celegans-totaldegree.dat`` contain
  the in-degree, out-degree, and total degree sequences, respectively,
  of the neural network of the C. elegans.  The degree sequences are
  computed from the dataset compiled by [WattsStrogatz1998]_, who in
  turn obtained the dataset from original experimental data by
  [WhiteEtAl1986]_.

* The data file ``condmat2005-degree.dat`` contains the 2005 update of
  the collaboration network of scientists posting preprints on the
  condensed matter archive at www.arxiv.org.  The network is weighted,
  with weights assigned as described in [Newman2001a]_.  The data
  should be cited as an updated version of the dataset used in
  [Newman2001b]_.  The network is undirected and we assume it has no
  self-loops nor multiple edges.  The degree sequence reflects these
  assumptions.

* The data file ``karate.dat`` contains the (total) degree sequence of
  the Zachary karate club network, as described in [Zachary1977]_.

* The data files ``karate-commented.dat``, ``karate-commented_dos.dat``,
	and ``karate-commented_corrupted.dat`` are *commented* version of the
	data file ``karate.dat``. Inline comments with a single # (hash) as
	left delimiter are supported: all the text from the ASCII character #
	(35) to the end of the line is ignored. The first data file mainly
	serves as illustration; the two others, with a self-explanatory
	subscript, are mainly meant for testing.


.. [Newman2001a]
   M. E. J. Newman. Scientific collaboration networks: I. Network
   construction and fundamental results. *Physical Review E*,
   64(1):016131, 2001, doi:10.1103/PhysRevE.64.016131.

.. [Newman2001b]
   M. E. J. Newman. The structure of scientific collaboration
   networks. *Proceedings of the National Academy of Sciences USA*,
   98(2):404--409, 2001, doi:10.1073/pnas.98.2.404.

.. [WattsStrogatz1998]
   Duncan J. Watts and Steven H. Strogatz.  Collective dynamics of
   'small-world' networks.  *Nature*, 393(4):440--442, 1998,
   doi:10.1038/30918.

.. [WhiteEtAl1986]
   J. G. White, E. Southgate, J. N. Thompson, and S. Brenner.  The
   Structure of the Nervous System of the Nematode Caenorhabditis
   elegans.  *Philosophical Transactions of The Royal Society B*,
   314(1165):1--340, 1986, doi:10.1098/rstb.1986.0056.

.. [Zachary1977]
   Wayne W. Zachary. An Information Flow Model for Conflict and
   Fission in Small Groups. *Journal of Anthropological Research*,
   33(4):452--473, 1977.
