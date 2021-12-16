#!/bin/bash
bazel-bin/q2
find . -name '*|*' |xargs rm