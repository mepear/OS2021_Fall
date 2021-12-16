#!/bin/bash
bazel-bin/mma_test
find . -name '*|*' |xargs rm