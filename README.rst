=====
plfit
=====
-------------------------------------------------
Fitting power-law distributions to empirical data
-------------------------------------------------

:Author: Tamas Nepusz
:Build status: |travis_build_status|

This program fits power-law distributions to empirical (discrete or
continuous) data, according to the method of Clauset, Shalizi and
Newman [1]_.

Requirements
------------

You only need a recent C compiler (e.g., ``gcc``), CMake_ and ``make`` to
compile the main executable. If you also want to compile the Python module with
the same functionality, you will also need SWIG_.

.. _SWIG: http://www.swig.org
.. _CMake: http://www.cmake.org

It is possible (although a bit more complicated) to compile the program
and the Python module without CMake_ or ``make``; see the
`Compiling without CMake or make`_ section below.

Compilation
-----------

Compiling with CMake
^^^^^^^^^^^^^^^^^^^^

This is easy. Assuming that you have downloaded the source code already,
just do this from the command line::

    $ mkdir build
    $ cd build
    $ cmake ..
    $ make

If you have permission to write in ``/usr/local``, you can also invoke
``make install`` at this point, which will place the executable in
``/usr/local/bin``. Adjusting ``CMAKE_INSTALL_PREFIX`` using ``ccmake``
will let you install ``plfit`` in a different folder.

The above code snippet compiles the main executable (``plfit``) but not
the Python module. If you need the Python module as well, the dance goes
like this::

    $ mkdir build
    $ cd build
    $ cmake ..
    $ ccmake .
    [ccmake starts up; set the PLFIT_COMPILE_PYTHON_MODULE variable to ON]
    $ make

Compiling without CMake or make
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

I like to use a separate build folder for compiling stuff, so we start
again by creating one::

    $ mkdir build
    $ cd build

Now we simply compile all the C files into a single executable using
``gcc``::

    $ gcc -o plfit -I../src -lm ../src/*.c

To build the Python interface as well, some extra legwork is needed
with SWIG::

    $ swig -Wall -O -python -outcurrentdir ../src/plfit.i
    $ gcc -shared -fPIC -o _plfit.so -I../src -I/usr/include/python2.7 \
      -lm plfit_wrap.c ../src/*.c

where ``/usr/include/python2.7`` should be replaced with the directory
where the ``Python.h`` file of your Python interpreter is.

That's it. The Python interface itself consists of two files:
``plfit.py`` and ``_plfit.so``. Both of them are to be found in the
``build`` folder.

Usage
-----

From the command line
^^^^^^^^^^^^^^^^^^^^^

Prepare a data file consisting of your input data, one number (i.e. sample)
per line. So, for instance, if you want to find out whether the degrees
of some graph follow a power-law distribution or not, just dump the
degrees into the input file, one vertex per line. If you are interested
in whether the magnitude of earthquakes follow a power-law, dump the
magnitudes into the input file, one earthquake per line. There is no
need to calculate the cumulative distribution function or the probability
density function before, nor to normalize the input data. Then simply run::

    $ ./plfit input_data.txt

For some inspiration, see the sample data files under the directory ``test``.
The program will assume that the data is discrete when the file contains
integers only; otherwise it will assume that the data is continuous. If your
data file contains integers only but you know that they come from a continuous
distribution, use ``-c`` to force continuous fitting::

    $ ./plfit -c input_data.txt

The ``-h`` switch gives you a nice help message::

    $ ./plfit -h

You may also ask ``plfit`` to calculate the first four moments of the input
data (besides doing a regular power law fitting) using the ``-M`` switch. You
can then use the values of the moments to decide whether the distribution is
skewed and fat-tailed enough to be considered a "real" power-law.

``plfit`` has two output modes: a human-readable and a brief (but easily
parseable) format. By default, the results are printed in human-readable
format, which looks like this::

    input.txt:
            Central moments
            mean     =      2.71550
            variance =     43.36610
            std.dev. =      6.58529
            skewness =     23.30801
            kurtosis =    798.21995
            ex.kurt. =    795.21995

            Discrete MLE
            alpha =      2.58306
            xmin  =      2.00000
            L     =  -9155.61707
            D     =      0.00428
            p     =      0.99996

The moments are printed only if ``-M`` is given in the command line. The output
of the brief mode (when ``-b`` is given) looks like this::

    input.txt: M 2.7155 43.3661 23.308 798.22
    input.txt: D 2.58306 2 -9155.62 0.00428253 0.999965

where the line containing ``M`` (i.e. the first one) lists the first four
central moments (mean, variance, skewness, kurtosis) and the line containing
``D`` (for discrete data) or ``C`` (for continuous data) contains the fitted
exponent, minimum X value, log-likelihood (L), Kolmogorov-Smirnov statistic (D)
and p-value (p). Note that the ``M`` line does not list the standard deviation
(which is simply the square root of the variance) and the excess kurtosis
(which is simply the kurtosis minus 3).

From Python
^^^^^^^^^^^

The Python module is just a thin wrapper around the original C calls,
so take a look at ``plfit.h`` and use your instincts to figure out
how the corresponding Python functions are named ;) This should
get you started::

    >>> import plfit
    >>> data = [float(line) for line in open("input_file.txt")]
    >>> plfit.plfit_discrete(data)
    >>> plfit.alpha, plfit.xmin, plfit.L

You can also invoke ``dir(plfit)`` to examine the namespace of the
``plfit`` module, listing all the functions and data types.
There's also a test script in ``test/test_python_module.py``.

Using multiple CPU cores when fitting power-laws
------------------------------------------------

``plfit`` has experimental support for parallel computation on multi-core
machines. This is not tested thoroughly yet, but I am using it regularly on my
machine so it should be okay in general.

The default compilation settings of ``plfit`` compile the executable in
sequential mode, therefore ``plfit`` will use only a single core of your machine.
However, some parts of the calculation may be speeded up by distributing the
work among multiple CPU cores. If you want to enable parallelized computing,
launch ``ccmake .`` in the build directory, set the ``PLFIT_USE_OPENMP``
option to ``ON``, and recompile everything by calling ``make``. If you are
compiling ``plfit`` without CMake, add the necessary compiler switch to enable
OpenMP in the compiler. For ``gcc``, you should add the ``-fopenmp`` switch, so
the compiler invocation should start with ``gcc -fopenmp``.

Theoretically, the results provided by ``plfit`` in parallelized mode should be
exactly the same as the results provided in sequential mode (apart from the
exact p-values of course, which tend to fluctuate between runs). If you have
a dataset for which this does not hold, please file a bug report for ``plfit``
on GitHub_.

.. _GitHub: http://github.com/ntamas/plfit

References
----------

.. [1] Clauset A, Shalizi CR and Newman MEJ: Power-law distributions
       in empirical data. SIAM Review 51, 661-703 (2009).

.. |travis_build_status| image:: https://travis-ci.org/ntamas/plfit.png
   :alt: Build status on Travis
