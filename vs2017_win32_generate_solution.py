import os
import subprocess

cmakeCachePath = "build/CMakeCache.txt"
buildDirectory = "build"

try:
    if os.path.exists(cmakeCachePath):
        os.remove(cmakeCachePath)

    if not os.path.exists(buildDirectory):
        os.mkdir(buildDirectory)

    subprocess.call(["cmake", "-G", "Visual Studio 15 2017", "-B", buildDirectory, "-S", "./"])

    input("Done!")

except Exception as exception:
    print("Error:", str(exception))
    input()
