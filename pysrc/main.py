import subprocess
import os
import select
import numpy as np


def parse_matrix(matStr):
    try:
        data = np.fromiter((float(x) for x in matStr.replace(';', ',').split(',')), dtype=float)
        rows = matStr.count(';') + 1
        cols = matStr.split(';')[0].count(',') + 1
        return data.reshape(rows, cols)
    except ValueError as e:
        print(f"Error parsing matrix: {e}")
        return None


if __name__ == "__main__":

    build_dir = "build"
    exec_path = os.path.join(build_dir, "bin", "PendulumExec")

    try:
        os.makedirs(build_dir, exist_ok=True)
        subprocess.run(["cmake", ".."], cwd=build_dir, check=True)
        subprocess.run(["cmake", "--build", ".", f"-j{os.cpu_count()}"], cwd=build_dir, check=True)
        print("Build successful.")
    except subprocess.CalledProcessError as e:
        print(f"Error during CMake build: {e}")
        exit(1)

    process = subprocess.Popen(
        [exec_path],  
        stdin = subprocess.PIPE,
        stdout = subprocess.PIPE,
        stderr = subprocess.PIPE,
    )

    if process.stdin is None or process.stderr == None or process.stdout == None:
        print("Failed to initialize program")
        exit(1)


    while process.poll() is None:
        ready, _, _ = select.select([process.stdout.fileno(), process.stderr.fileno()], [], [], 0.1)

        for fd in ready:
            if fd == process.stdout.fileno():
                output = process.stdout.readline().decode('utf-8')
                if output:
                    print(parse_matrix(output.strip()), end = "               \r", flush = True)
            elif fd == process.stderr.fileno():
                error = process.stderr.readline().decode('utf-8')
                if error:
                    print(error.strip())


        process.stdin.write(b"1\n")
        try:
            process.stdin.flush()
        except Exception as e:
            print("Pipe broke, exiting")
            break
