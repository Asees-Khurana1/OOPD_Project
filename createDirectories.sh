#!/bin/bash

# Create directories
mkdir -p directory1 directory2

# Case 1: 100 files of 1GB each
echo "Creating 100 files of 1GB each in directory1"
time for i in {1..100}; do dd if=/dev/zero of=directory1/file${i} bs=1G count=1; done

# Case 2: 100 files of 10MB each and recursive subdirectories
echo "Creating 100 files of 10MB each in directory2 and subdirectories recursively"
time for i in {1..100}; do dd if=/dev/zero of=directory2/file${i} bs=10M count=1; done
for i in {1..99}; do cp -r directory2 directory2/subdirectory${i}; done

# Counting the total number of files in directory2 (recursively)
total_files=$(find directory2 -type f | wc -l)
echo "Total files in directory2 (including subdirectories): $total_files"
