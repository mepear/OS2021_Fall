int=1
while(( $int<=30 ))
do
    echo "value is $int"
    let "int++"
    bazel-bin/mma_test
done