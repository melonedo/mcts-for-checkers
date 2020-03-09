# Extract game records "X ..."

indir = "log.txt"
outdir = "record.txt"

with open(indir) as inf:
    with open(outdir, 'wt') as outf:
        import re
        log = inf.read()
        for i in re.findall(r'"(X \d+ .+?)"', log):
            outf.write(i)
            outf.write('\n')
print('Done')
