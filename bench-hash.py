# -*- coding: utf-8 -*-

import sys, os, subprocess, signal
import time

programs = [
    'hash-empty',
    'hash-bplus',
    'hash-brute',
    'hash-glib',
    'hash-sparse',
]

minallocs   =  128*1024
maxallocs   =  128*1024
interval    =  2
best_out_of =  1

#minallocs  = 16000
#maxallocs  = 512*1000
#interval = 2
#best_out_of = 3

# for the final run, use this:

outfile = open('output-%s' % sys.argv[1], 'w')

for program in programs:
    command = ['make', '-B', 'build/' + program]
    print '>> ', ' '.join(command)
    subprocess.check_call(command)

nkeys = minallocs
while nkeys <= maxallocs:
    for program in programs:
        fastest_attempt = 1000000
        fastest_attempt_data = ''

        for attempt in range(best_out_of):
            command = ['make', '-B', 'SHOOTOUT_COUNT=%d' % (nkeys),  'results/' + program]
            print '>> ', ' '.join(command)
            proc = subprocess.Popen(command, stdout=subprocess.PIPE)

            # wait for the program to fill up memory and spit out its "ready" message
            proc.stdout.readline().strip()
            proc.stdout.readline().strip()
            try:
                runtime = float(proc.stdout.readline().strip())
            except:
                runtime = 0

            ps_proc = subprocess.Popen(['ps up %d | tail -n1' % proc.pid], shell=True, stdout=subprocess.PIPE)
            nbytes = int(ps_proc.stdout.read().split()[4]) * 1024
            ps_proc.wait()

            os.kill(proc.pid, signal.SIGKILL)
            proc.wait()

            with open('results/%d-%s.plot' % (nkeys, program), 'w') as plot:
                print >>plot, '''
set terminal pngcairo enhanced fontscale 1.0 size 4096, 1024
set output 'results/%d-%s.png'

unset colorbox

set palette positive nops_allcF maxcolors 0 gamma 1.0 color model HSV
set palette defined ( 0 0 1 1, 0.1 1 1 1, 0.2 0 0.75 1, 0.3 1 0.75 1, 0.4 0 0.5 1, 0.5 1 0.5 1, 0.6 0 0.25 1, 0.7 1 0.25 1, 0.8 0 0 1, 1 1 0 1 )

set auto x
set format y "10^{%%L}"

set ytics nomirror
set grid ytics xtics

set yrange [0.01:100000000]
# set xrange [0:5]
set logscale y

set title "Timings"
set ylabel "time taken in nanosecond"
set xlabel "time elapsed in µseconds"

t=0
timeof(y,x)=(t=t+x)/1000000''' % (nkeys, program)
                if 'empty' in program:
                    print >>plot, '''
plot '< cat results/%s | grep ^alloc' using 2:3:(log10($3/10)**2/5.0) with lp pt 7 ps variable lw 0.05, \\
     '< cat results/%s | grep ^dealloc' using 2:3:(log10($3/10)**2/5.0) with lp pt 7 ps variable lw 0.05''' % (program, program)
                else:
                    print >>plot, '''
plot '< paste results/%s results/hash-empty | grep ^alloc' using 2:($3/$6):(log10($3/$6)**2/5.0) with lp pt 7 ps variable lw 0.05, \\
     '< paste results/%s results/hash-empty | grep ^dealloc' using 2:($3/$6):(log10($3/$6)**2/5.0) with lp pt 7 ps variable lw 0.05''' % (program, program)

            command = ['gnuplot', 'results/%d-%s.plot' % (nkeys, program)]
            print '>> ', ' '.join(command)
            subprocess.check_call(command)

            if nbytes and runtime: # otherwise it crashed
                line = ','.join(map(str, [benchtype, (nkeys), program, nbytes, "%0.6f" % runtime]))

                if runtime < fastest_attempt:
                    fastest_attempt = runtime
                    fastest_attempt_data = line

        if fastest_attempt != 1000000:
            print >> outfile, fastest_attempt_data
            print fastest_attempt_data

    nkeys *= interval

print 'for file in output-*; do cat ${file} | python make_chart_data.py | python make_html.py ${file/output-/}; done'