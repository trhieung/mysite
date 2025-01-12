import io
import os
import sys
import argparse

from setuptools import setup, find_packages #find_namespace_packages
from libs import *

def read(*paths, **kwargs):
    """Read the contents of a text file safely.
    >>> read("project_name", "VERSION")
    '0.1.0'
    >>> read("README.md")
    ...
    """

    content = ""
    with io.open(
        os.path.join(os.path.dirname(__file__), *paths),
        encoding=kwargs.get("encoding", "utf8"),
    ) as open_file:
        content = open_file.read().strip()
    return content

def read_requirements(path):
    return [
        line.strip()
        for line in read(path).split("\n")
        if not line.startswith(('"', "#", "-", "git+"))
    ]

def parse_arguments():
    """Parse command-line arguments."""
    parser = argparse.ArgumentParser(description="Setup script for the project.")
    parser.add_argument(
        "-c", "--config", required=True, help="The build command must be \"bdist_wheel\"."
    )
    parser.add_argument(
        "-n", "--project-name", required=True, help="The name of the project to set up."
    )
    parser.add_argument(
        "-v", "--version", required=True, help="The version of the project."
    )
    # Parse arguments but don't stop at known arguments
    args = parser.parse_known_args()
    return args

if __name__ == "__main__":
    # Parse command-line arguments
    args, unknown = parse_arguments()

    # Assign the project name and version
    config = args.config
    project_name = args.project_name
    version = args.version
    
    if not(proCheck.validProName(project_name)):
        print("Invalid project name! Please ensure your project folder is included in the 'template' directory.")
        exit()

    project_dir = f"./{_global.dir_projects}/{project_name}"
    sys.argv = [sys.argv[0], config]

    setup(
        # Prepare meta in4s
        name= project_name,
        version=version,
        # description="project_description",
        # url="https://github.com/author_name/project_urlname/",
        # long_description=read("README.md"),
        # long_description_content_type="text/markdown",
        author=project_name,
        # entry_points={
        #     "console_scripts": [f"{project_name} = {project_name}:{project_name}_desc"]
        # },

        # Prepare package build
        packages=(
            # find_packages(),
            find_packages(where=project_dir)
        ),
        python_requires="==3.11.*",

        ## local design don't need this because config install offline through .whl in make tool
        # install_requires=read_requirements("requirements.txt"),
        # extras_require={"test": read_requirements("requirements-test.txt")},
    )

# python3.11 setup.py -c bdist_wheel -n glucus -v 0.0.1
# python3.11 -c "import glucus; print(glucus)"