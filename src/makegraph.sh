#!/bin/sh

bin/graph $1 | dot -Tpdf > $1.pdf
