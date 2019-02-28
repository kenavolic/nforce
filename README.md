# nforce

[![Build Status](https://travis-ci.org/kenavolic/nforce.svg?branch=master)](https://travis-ci.org/kenavolic/nforce)

nforce is a c++ 17 poc library used to apply textual user-defined boolean rules
defined through a boolean expression to a domain specific model.

From a set of specific rules defined for a given application and responsible for their 
own interpretation and parsing, it is possible to create a boolean expression that combine 
them and apply the defined constraint to the application model.

# Supported Platforms

Tests have been performed on the following platforms:

  * clang++-7
  * g++-7

# Install

  * Clone project
~~~
    > git clone https://github.com/kenavolic/nforce.gitxx
~~~

  * Generate build system
~~~
    > mkdir nforce_build
    > cd nforce_build
    > cmake  ../nforce
~~~

  * Compilation
~~~
    > make
    > make install
~~~
    + /!\ On windows, open generated solution and build solution and install target


# Usage

See the provided example for an usage example.
