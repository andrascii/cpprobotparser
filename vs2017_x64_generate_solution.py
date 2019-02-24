import os
import subprocess

separatorString = "\n" + ("*" * 80)

buildDirectory = "build"
cmakeCachePath = "".join([buildDirectory, "/CMakeCache.txt"])

try:
    if os.path.exists(cmakeCachePath):
        os.remove(cmakeCachePath)

    if not os.path.exists(buildDirectory):
        os.mkdir(buildDirectory)

    subprocess.call(["cmake", "-G", "Visual Studio 15 2017 Win64", "-B", buildDirectory, "-S", "./"])

    input(separatorString)

except Exception as exception:
    print("Error:", str(exception))
    input()
