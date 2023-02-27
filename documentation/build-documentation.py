import typing
from pathlib import Path
from argparse import ArgumentParser
import subprocess
import re

class Version:
    def __init__(self,major:int,minor:int,patch:int=-1) -> None:
        self.major = major
        self.minor = minor
        self.patch = patch if patch >= 0  else 0
        self.patch_was_specified = patch >= 0
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
        if self.patch_was_specified:
            return "{}.{}.{}".format(self.major,self.minor,self.patch)
        return "{}.{}".format(self.major,self.minor)
    
    """Construct a version from a version string.
    :param version_string: The version string. Should be in format
    MAJOR.MINOR or MAJOR.MINOR.PATCH
    """
    @staticmethod
    def from_string(version_string:str)->"Version":
        match = re.match('([0-9]+)\\.([0-9]+)(\\.[0-9]+)?', version_string)
        if match is None:
            raise Exception('Invalid version string "{}"'.format(version_string))
        patch=-1
        (major,minor,patch_str) = match.groups()
        major = int(major)
        minor = int(minor)
        patch = patch if patch_str is None else int(patch_str[1:])
        return Version(major,minor,patch)

# Global variables
ROOT_PATH = Path(__file__).parent.resolve() # Folder of the current script
MOVETK_DOXYGEN_CONFIG = ROOT_PATH / 'movetk-doxy-config.cfg.in'
MOVETK_DOXYGEN_CONFIG_OUT = ROOT_PATH / 'movetk-doxy-config.cfg'
DEFAULT_OUTPUT_FOLDER = ROOT_PATH / '..' / 'docs'
BIBLIOGRAPHY_FILE = ROOT_PATH / 'movetk.bib'
BIBLIOGRAPHY_GEN_SOURCE = ROOT_PATH / 'resources' / 'bibliography.js.in'
BIBLIOGRAPHY_GEN_TARGET = ROOT_PATH / 'resources' / 'bibliography.js'

REQUIRED_DOXYGEN_VERSION = Version(1,9,6)

def check_doxygen():
    """Check if doxygen is present and that it has the right version
    """
    result = subprocess.run(['doxygen','--version'], stdout=subprocess.PIPE,universal_newlines=True)
    if result.returncode != 0:
        raise Exception("Got non-zero exit code when running doxygen. Make sure it is installed")
    version_string = str(result.stdout).strip()
    doxy_ver = Version.from_string(version_string)
    if doxy_ver.less(REQUIRED_DOXYGEN_VERSION):
        raise Exception("Need at least doxygen version {}".format(REQUIRED_DOXYGEN_VERSION))

class DocumentationParameters:
    def __init__(self,output_folder:str='',movetk_version:str='') -> None:
        self.output_folder = output_folder
        self.movetk_version = movetk_version
    def get_value(self, key:str):
        if hasattr(self,key):
            return getattr(self,key)
        raise Exception("Unknown documentation parameter '{}'".format(key))

def prepare_config(doc_params:DocumentationParameters):
    # Read the config template file
    with MOVETK_DOXYGEN_CONFIG.open() as config_template_stream:
        config_template = config_template_stream.read()
    # Replace anything between {{}} with the appropriate DocumentationParameters
    # member
    regex = re.compile(r"\{\{\s*([0-9a-zA-Z_]+)\s*\}\}")
    def substitution_func(match_obj):
        key = match_obj.group(1).strip()
        print('Replacing {}'.format(key))
        return doc_params.get_value(key)
    config_template = re.sub(regex, substitution_func, config_template)
    # Write to output file, to be used with doxygen
    with MOVETK_DOXYGEN_CONFIG_OUT.open('w') as config_out_stream:
        config_out_stream.write(config_template)

def run_doxygen():
    """
    Generate the documentation using doxygen
    """
    subprocess.run([
        'doxygen',
        '-q',
        str(MOVETK_DOXYGEN_CONFIG_OUT)
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
    check_doxygen()
    # Determine outputfolder
    output_folder = Path(args.output_folder) if args.output_folder is not None else DEFAULT_OUTPUT_FOLDER.resolve()
    version = Version.from_string(getattr(args, 'mtk-version'))
    doc_params = DocumentationParameters(output_folder=str(output_folder),movetk_version=str(version))
    print('Outputting to {}'.format(output_folder))
    # Add bibliography support via javascript
    add_bibliography()

    # Prepare the config file
    prepare_config(doc_params)

    # Run doxygen
    run_doxygen()
    