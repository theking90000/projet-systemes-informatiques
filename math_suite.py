def solve(input, output, only_longest, debug):
    import logging

    logger = logging.getLogger(__name__)
    log_level = logging.DEBUG if debug == 2 else logging.INFO if debug == 1 else logging.CRITICAL
    logging.basicConfig(level=log_level)
    logger.info("I will solve the file {} and write to {}, only longest {}".format(input, output, only_longest))
    longest = []
    for l in input.readlines():
        value, iteration = l.strip().split(" ")
        logger.info("Solving {} iterations of the value {}".format(iteration, value))
        for i in range(int(iteration)):
            logger.info("Iteration {}".format(i))
            new_value = ""
            j = 0
            while j < len(value):
                orig_j = j
                c = value[j]
                j += 1
                while j < len(value) and value[j] == c:
                    j += 1
                new_value += str(j - orig_j) + c
            
            logger.debug("Iterating over {} gives {}".format(value, new_value))
            value = new_value

        logger.info("Result: {}".format(value))
        if only_longest:
            # Buggy, does not count the number of different digits
            if len(longest) == 0 or len(value) > len(longest[0]):
                longest = [value]
            elif len(value) == len(longest[0]):
                longest.append(value)
        else:
            print(new_value, file=output)
            

    if only_longest:
        for v in longest:
            print(v, file=output)

    logger.info("Done")


if __name__ == "__main__":
    import argparse
    import sys

    parser = argparse.ArgumentParser("Mathematical suite solver")
    parser.add_argument("--input", type=str, default=None, help="Input file, default to stdin")
    parser.add_argument("--output", type=str, default=None, help="Output file, default to stdout")
    parser.add_argument("--only-longest", action="store_true", help="Only write the result(s) having 1) the most different numbers and 2) the longest ones")
    parser.add_argument("--debug", type=int, default=0, help="Logs to understand the program operations (0: none, 1: partial, 2: complete)")
    args = parser.parse_args()
    

    out_file = sys.stdout if args.output is None else open(args.output, 'w')
    in_file = sys.stdin if args.input is None else open(args.input)

    solve(in_file, out_file, args.only_longest, args.debug)
