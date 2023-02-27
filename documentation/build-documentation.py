import typing
from pathlib import Path
from argparse import ArgumentParser
import subprocess
import re

class Version:
    def __init__(self,major:int,minor:int,patch:int=0) -> None:
        self.major = major
        self.minor = minor
        self.patch = patch
    def less(self, other_version:"Version"):
        if self.major < other_version.major:
            return True
        if self.minor < other_version.minor:
            return True
        if self.patch < other_version.patch:
            return True
        return False
    def equals(self, other_version):
        return not self.less(other_version) and not other_version.less(self)
    def __repr__(self) -> str:
        return "{}.{}.{}".format(self.major,self.minor,self.patch)
    
    """Construct a version from a version string.
    :param version_string: The version string. Should be in format
    MAJOR.MINOR or MAJOR.MINOR.PATCH
    """
    @staticmethod
    def from_string(version_string:str)->"Version":
        match = re.match('([0-9]+)\\.([0-9]+)(\\.[0-9]+)?', version_string)
        if match is None:
            raise Exception('Invalid version string "{}"'.format(version_string))
        patch=0
        (major,minor,patch_str) = match.groups()
        major = int(major)
        minor = int(minor)
        patch = patch if patch_str is None else int(patch_str[1:])
        return Version(major,minor,patch)

# Global variables
ROOT_PATH = Path(__file__).parent.resolve()
MOVETK_DOXYGEN_CONFIG = 'movetk-doxy-config.cfg'
DEFAULT_OUTPUT_FOLDER = ROOT_PATH / '..' / 'docs'
BIBLIOGRAPHY_FILE = ROOT_PATH / 'movetk.bib'
BIBLIOGRAPHY_GEN_SOURCE = ROOT_PATH / 'resources' / 'bibliography.js.in'
BIBLIOGRAPHY_GEN_TARGET = ROOT_PATH / 'resources' / 'bibliography.js'

def check_doxygen():
    """Check if doxygen is present and that it has the right version
    """
    result = subprocess.run(['doxygen','--version'], stdout=subprocess.PIPE,universal_newlines=True)
    if result.returncode != 0:
        raise Exception("Got non-zero exit code when running doxygen. Make sure it is installed")
    version_string = str(result.stdout).strip()
    doxy_ver = Version.from_string(version_string)
    if doxy_ver.less(Version(1,9,6)):
        raise Exception("Need at least doxygen version 1.9.6")

def run_doxygen(output_folder:Path):
    """
    Generate the documentation using doxygen
    """
    subprocess.run([
        'doxygen',
        str(ROOT_PATH / MOVETK_DOXYGEN_CONFIG),
        str(output_folder)
        ],check=True)
    
def add_bibliography():
    with (BIBLIOGRAPHY_FILE).open() as bib_file:
        bibiliography = bib_file.read()
        # Escape quote characters
        bibiliography = bibiliography.replace('\'','\\\'')
        bibiliography = bibiliography.replace('\n','')
        bibiliography = bibiliography.replace('\r','')
    with BIBLIOGRAPHY_GEN_SOURCE.open() as template:
        template_content = template.read()
        template_content = template_content.replace('$bibliography',bibiliography)
        with BIBLIOGRAPHY_GEN_TARGET.open('w') as template_out:
            template_out.write(template_content)
    
if __name__ == '__main__':
    arg_parser = ArgumentParser('Generate documentation for MoveTK')
    arg_parser.add_argument('mtk-version',type=str,help='MoveTk version, specified as x.y or x.y.z'
                            'Note that x.y implies x.y.0')
    arg_parser.add_argument('--output-folder','-o',type=str,help='Define the output folder. Otherwise, ../docs is picked')
    args = arg_parser.parse_args()
    print(args)
    check_doxygen()
    # Determine outputfolder
    output_folder = Path(args.output_folder) if args.output_folder is not None else DEFAULT_OUTPUT_FOLDER.resolve()
    # Add bibliography support via javascript
    add_bibliography()

    

    # Run doxygen
    run_doxygen(output_folder)
    