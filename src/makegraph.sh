#!/bin/sh

dist/graph $1 | dot -Tpdf > $1.pdf
