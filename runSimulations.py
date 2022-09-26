import subprocess
import threading
import sys
import os.path
import time


def run(experiment):
    output_path = os.path.join("traces", os.path.basename(experiment))
    sem.acquire()
    start = time.time()
    print("Running: "+experiment+ " "+output_path)
    subprocess.run("argos3 -zc %s -l %s" % (experiment, output_path),
                   env={"ARGOS_PLUGIN_PATH":"./build/simulator/"},
                   check=True,
                   shell=True)
    end = time.time()
    sem.release()
    print("Thread took "+str(end-start)+"s")


if __name__ == '__main__':

    concurrent_workers = int(sys.argv[1])
    sem = threading.Semaphore(concurrent_workers)

    experiments = sys.argv[2:]
    print(experiments)

    threads = []

    for e in experiments:
        threads.append(threading.Thread(target=run, args=(e,)))

    for thread in threads:
        thread.start()

    for thread in threads:
        thread.join()
