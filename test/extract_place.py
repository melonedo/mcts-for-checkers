inputf = "log.txt"
outputf = "place.txt"
outfile = open(outputf, "wt")
with open(inputf) as infile:
    import re
    for line in infile:
        m = re.search('"data":"(PLACE .+)"', line)
        if m is not None:
            outfile.write(m.group(1)+'\n')
