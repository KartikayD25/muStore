# #!/bin/bash

# # Directory for storing pre-generated strings
# DATA_DIR="./pregenerated_strings"
# PROTO_FILE="test_message.proto"  # Your proto file path

# # First, generate strings if they don't exist
# if [ ! -d "$DATA_DIR" ]; then
#     echo "Generating strings... This may take a while..."
#     ./string_generator "$DATA_DIR"
# fi

# # Total message sizes in MB
# message_sizes=(8 40 80 400)
# methods=("SG" "ZC")

# # Loop over each serialization method
# for method in "${methods[@]}"; do
#     for size in "${message_sizes[@]}"; do
#         echo "Testing serialization method $method with total size ${size}MB"
        
#         # Run each test 5 times
#         for i in {1..500}; do
#             name="serialization-$method-$size-$i"
#             echo "Running test $i for $method method with ${size}MB"
#             ./lib_test $size $i "$DATA_DIR" $method "$PROTO_FILE" &
#             # SERVER_PID=$!
#             # echo "PID: $SERVER_PID"
#             # ./ftrace_script.sh $SERVER_PID $method $size 1 $i
#             wait
#         done
        
#         echo "Completed tests for $size MB"
#     done
# done

#!/bin/bash

# Directory for storing pre-generated strings
DATA_DIR="./pregenerated_strings"
PROTO_FILE="test_message.proto"
NUM_THREADS=6
INSTANCES_PER_THREAD=100

# Function to run tests for a specific method and size
run_test_set() {
    local method=$1
    local size=$2
    local thread_id=$3

    echo "Thread $thread_id: Testing serialization method $method with size ${size}MB"
    
    for i in $(seq 1 $INSTANCES_PER_THREAD); do
        ./lib_test $size 1 "$DATA_DIR" $method "$PROTO_FILE" &
        wait $!
    done
}

# Total message sizes in MB
message_sizes=(8 40 80)
methods=("ZC")

# Main loop over methods and sizes
for method in "${methods[@]}"; do
    for size in "${message_sizes[@]}"; do
        echo "Starting $NUM_THREADS threads for method $method, size ${size}MB"
        
        # # Start NUM_THREADS parallel test sets
        # for thread_id in $(seq 0 $((NUM_THREADS-1))); do
        #     run_test_set "$method" "$size" "$thread_id" &
        # done
        
        # # Wait for all threads to complete before moving to next size/method combination
        # wait
        seq $((NUM_THREADS * INSTANCES_PER_THREAD)) | \
        xargs -P $NUM_THREADS -I {} \
             ./lib_test $size 1 "$DATA_DIR" $method "$PROTO_FILE"

        echo "Completed all tests for method $method with size ${size}MB"
    done
done

echo "All tests completed"