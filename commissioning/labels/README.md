Hail Labels
===========

To generate labels:

    ./labels.py [michgan|lab11llc] <id> <count>

For example:

    ./labels.py michigan 005c 100

To make 100 labels with the block "M" on them starting at address
`C0:98:E5:13:00:5C`.

Warning
-------

You may have to manually install `svgutils` in order to get everything working.
Their pip package is way out of date. [Go here](https://github.com/btel/svg_utils).

